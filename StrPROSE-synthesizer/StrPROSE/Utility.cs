using System;
using System.IO;
using System.Collections.Generic;
using Microsoft.ProgramSynthesis.Transformation.Text;
using CsvHelper;
using System.Globalization;
using Microsoft.ProgramSynthesis.Utils;
using System.Numerics;
using Microsoft.ProgramSynthesis.VersionSpace;
using System.Linq;
using System.Reflection;
using Microsoft.ProgramSynthesis.AST;
using Microsoft.ProgramSynthesis.Features;
using Microsoft.ProgramSynthesis.Transformation.Text.Features;
using Microsoft.ProgramSynthesis.Wrangling;
using Microsoft.ProgramSynthesis;
using Microsoft.ProgramSynthesis.Rules;
using Microsoft.ProgramSynthesis.Rules.Concepts;
using Microsoft.ProgramSynthesis.Extraction.Text.Build.NodeTypes;
using System.Diagnostics;
using Microsoft.ProgramSynthesis.Transformation.Text.Semantics;

namespace PROSE_StrGen
{
    static class Utility
    {
        public static BigInteger calcUnionSampleUpperBound(double epsilon, double delta, BigInteger sizeOfH)
        {
            double epsilonFactor = 1.0 / Math.Log(1.0 / (1.0 - epsilon));
            double deltaTerm = Math.Log(1.0 / delta);
            double logH = Math.Log((double)sizeOfH);
            double sampleSize = epsilonFactor * (logH + deltaTerm);
            return (BigInteger)sampleSize;
        }

        //
        public static List<string[]> getStrExamplesFromCSVFile(string filename)
        {
            using var streamReader = File.OpenText(filename);
            using var csvReader = new CsvReader(streamReader, CultureInfo.CurrentCulture);
            csvReader.Configuration.HasHeaderRecord = true;

            csvReader.Read();
            csvReader.ReadHeader();
            string value;

            //read head
            int lengthRow = 0;
            for (; csvReader.TryGetField<string>(lengthRow, out value); lengthRow++) { };
            Console.WriteLine("# Length of a row: " + lengthRow);

            List<string[]> strExamples = new List<string[]>();
            //read body
            while (csvReader.Read())
            {
                string[] strings = new string[lengthRow];
                int i = 0;
                for (; csvReader.TryGetField<string>(i, out value); i++)
                {
                    strings[i] = value;
                }
                Debug.Assert(i == lengthRow);
                strExamples.Add(strings);
            }
            return strExamples;
        }

        public class ExampleRecordResultCache
        {
            public ExampleRecordResultCache(string input0, string output, BigInteger H, bool important, string program)
            {
                this.input0 = input0;
                this.output = output;
                this.H = H;
                this.important = important;
                this.program = program;
            }
            public string input0 { get; set; }
            public string output { get; set; }
            public BigInteger H { get; set; }
            public bool important { get; set; }
            public string program { get; set; }
        }
        public static void writeExamplesResultCacheToCSVFile(string filename, List<ExampleRecordResultCache> records)
        {
            using var writer = new StreamWriter(filename);
            using var csvWriter = new CsvWriter(writer, CultureInfo.CurrentCulture);
            Console.WriteLine("# Write " + records.Count + " examples with H to csv: " + filename);
            csvWriter.WriteRecords(records);
        }

        public static List<ExampleRecordResultCache> getExamplesResultCacheFromCSVFile(string filename)
        {
            List<ExampleRecordResultCache> caches = new List<ExampleRecordResultCache>();
            using var streamReader = File.OpenText(filename);
            using var csvReader = new CsvReader(streamReader, CultureInfo.CurrentCulture);
            csvReader.Configuration.HasHeaderRecord = true;

            csvReader.Read();
            csvReader.ReadHeader();
            while (csvReader.Read())
            {
                ExampleRecordResultCache record = csvReader.GetRecord<ExampleRecordResultCache>();
                caches.Add(record);
            }
            return caches;
        }


        public class ExampleRecord
        {
            public ExampleRecord(string input0, string output)
            {
                this.input0 = input0;
                this.output = output;
            }
            public string input0 { get; set; }
            public string output { get; set; }
        }
        public static void writeStrExamplesToCSVFile(string filename, List<string[]> examples)
        {
            using var writer = new StreamWriter(filename);
            using var csvWriter = new CsvWriter(writer, CultureInfo.CurrentCulture);
            List<ExampleRecord> records = new List<ExampleRecord>();
            foreach (var eg in examples)
            {
                Debug.Assert(eg.Length == 2);
                records.Add(new ExampleRecord(eg[0], eg[1]));
            }
            Console.WriteLine("# Write " + records.Count + " examples to csv: " + filename);
            csvWriter.WriteRecords(records);
        }

        public static Example StringArrayToExample(string[] stringEg)
        {
            string resultStr = stringEg[stringEg.Length - 1];
            string[] inputs = new string[stringEg.Length - 1];
            for (int i = 0; i < stringEg.Length - 1; ++i)
                inputs[i] = stringEg[i];
            Example eg = new Example(new InputRow(stringEg), resultStr);
            return eg;
        }

        
        public static Tuple<string, string> pickFileFromDir(string filepath, string extension)
        {
            Console.WriteLine(" ---------- PickFileFromDir: " + filepath + " ----------");
            var files = Directory.GetFiles(filepath).Where(x => x.EndsWith("." + extension)).ToArray();
            Console.WriteLine(" File list: ");
            for (int i = 0; i < files.Length; i++)
            {
                string filename = files[i];
                Console.WriteLine(i + ": " + filename);
            }
            Console.WriteLine(" Please input file index:");
            int index = Convert.ToInt32(Console.ReadLine());
            string namePart = files[index].Split(new char[] { '\\', '/' }).Last();
            Console.WriteLine(" Selected file: " + files[index]);
            return new Tuple<string, string>(files[index], namePart);
        }

        //reflections
        public static Feature<double> getRankingFeatureFromSession(Session session)
        {
            const BindingFlags bindFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
            Learner obj = (Learner)session.GetType().BaseType.GetField("_learner", bindFlags).GetValue(session);
            Feature<double> rankingScore = obj.ScoreFeature;
            return rankingScore;
        }

        public static void printGrammarOfSession(Session session)
        {
            const BindingFlags bindFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;
            Grammar objg = (Grammar)session.GetType().BaseType.GetProperty("Grammar", bindFlags).GetValue(session);
            Console.WriteLine(objg.InputRule);
            foreach (GrammarRule x in objg.Rules)
            {
                ConversionRule cx = x as ConversionRule;
                LetRule lx = x as LetRule;
                TerminalRule tx = x as TerminalRule;
                BlackBoxRule bx = x as BlackBoxRule;
                Pair px = x as Pair;
                if (cx != null)
                {
                    Console.WriteLine(cx.GetType().GetProperty("Display", bindFlags).GetValue(cx));
                }
                else if (lx != null)
                {
                    Console.WriteLine(lx);
                }
                else if (tx != null)
                {
                    Console.WriteLine(tx);
                }
                else if (bx != null)
                {
                    Console.WriteLine(bx.GetType().GetProperty("Display", bindFlags).GetValue(bx));
                }
                else if (px != null)
                {
                    Console.WriteLine(px.GetType().GetProperty("Display", bindFlags).GetValue(px));
                }
                else
                {
                    Console.WriteLine("unknown:" + x);
                }

            }
        }
    }
}
