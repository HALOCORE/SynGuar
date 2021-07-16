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

        public static string runExampleInput(this ProgramNode program, string exampleInput)
        {
            var input = State.CreateForExecution(program.Grammar.InputSymbol, exampleInput);
            var output = (string)program.Invoke(input);
            return output;
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
    }
}
