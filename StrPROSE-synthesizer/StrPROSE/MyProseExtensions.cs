using System;
using System.Collections.Generic;

using Microsoft.ProgramSynthesis;
using Microsoft.ProgramSynthesis.VersionSpace;
using System.Linq;
using System.Reflection;
using Microsoft.ProgramSynthesis.AST;
using Microsoft.ProgramSynthesis.Features;
using System.Diagnostics;
using System.Globalization;
using Antlr4.Runtime;
using System.Xml.Linq;

namespace PROSE_StrGen
{
    public static class MyProseExtensions
    {
        public static bool checkExample(this ProgramNode program, string[] example)
        {
            Debug.Assert(example.Length == 2);
            var input = State.CreateForExecution(program.Grammar.InputSymbol, example[0]);
            var output = (string)program.Invoke(input);
            if (output == example[1]) return true;
            return false;
        }

        public static string execute(this ProgramNode program, string inputStr)
        {
            var input = State.CreateForExecution(program.Grammar.InputSymbol, inputStr);
            var output = (string)program.Invoke(input);
            return output;
        }

        public static string ToXMLTxtFile(this ProgramNode program, string filename)
        {
            string text = program.PrintAST(ASTSerializationSettings.Xml);
            System.IO.File.WriteAllText(filename + ".xml", text);
            Console.WriteLine("#ToXMLFile done: " + filename + ".xml");
            string text2 = program.PrintAST(ASTSerializationSettings.HumanReadable);
            System.IO.File.WriteAllText(filename + ".txt", text2);
            Console.WriteLine("#ToTxtFile done: " + filename + ".txt");
            return text2;
        }

        public static ProgramNode fromXMLFile(this Grammar grammar, string filename)
        {
            string text = System.IO.File.ReadAllText(filename);
            XElement xmlTree = XElement.Parse(text);
            //Console.WriteLine(xmlTree);
            var programNode = ProgramNode.ParseXML(grammar, xmlTree);
            return programNode;
        }

        public static Dictionary<ProgramSet, double> sumScores = new Dictionary<ProgramSet, double>();
        public static double getSumScores(this ProgramSet programSet, IFeature feature, FeatureCalculationContext context = null)
        {
            if (sumScores.ContainsKey(programSet)) { return sumScores.GetValueOrDefault(programSet); }
            else
            {
                UnionProgramSet uprog = programSet as UnionProgramSet;
                JoinProgramSet jprog = programSet as JoinProgramSet;
                DirectProgramSet dprog = programSet as DirectProgramSet;
                if (uprog != null) return uprog.getSumScores(feature, context);
                else if (jprog != null) return jprog.getSumScores(feature, context);
                else if (dprog != null) return dprog.getSumScores(feature, context);
                else throw new Exception("Unknown ProgramSet type.");
            };
        }

        public static double getSumScores(this UnionProgramSet programSet, IFeature feature, FeatureCalculationContext context = null)
        {
            double scoreSum = programSet.UnionSpaces.Aggregate(0, (double acc, ProgramSet set) => acc + set.getSumScores(feature, context));
            sumScores[programSet] = scoreSum;
            return scoreSum;
        }

        public static double getSumScores(this JoinProgramSet programSet, IFeature feature, FeatureCalculationContext context = null)
        {
            double scoreSum = programSet.ParameterSpaces.Aggregate(1, (double s, ProgramSet p) => s * p.getSumScores(feature, context));
            sumScores[programSet] = scoreSum;
            return scoreSum;
        }

        static PropertyInfo directProgramSetProgramsProperty = typeof(DirectProgramSet).GetProperty("Programs", BindingFlags.Instance | BindingFlags.NonPublic);
        public static double getSumScores(this DirectProgramSet programSet, IFeature feature, FeatureCalculationContext context = null)
        {
            Func<ProgramNode, object> keySelector = (ProgramNode p) => p.GetFeatureValue(feature, new LearningInfo(context, p));
            if (context == null) keySelector = (ProgramNode p) => p.GetFeatureValue(feature, null);

            IReadOnlyList<ProgramNode> progs = (IReadOnlyList<ProgramNode>)directProgramSetProgramsProperty.GetValue(programSet);
            double scoreSum = 0;
            foreach (var prog in progs)
            {
                double rawScore = (double)keySelector(prog);
                double score = Math.Abs(rawScore) + 1;
                scoreSum += score;
            }
            sumScores[programSet] = scoreSum;
            return scoreSum;
        }
    }
}
