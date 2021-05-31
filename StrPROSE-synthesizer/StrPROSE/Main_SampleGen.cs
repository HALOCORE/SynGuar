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
using Microsoft.ProgramSynthesis.AST;
using System.Diagnostics;

namespace PROSE_StrGen
{
    public partial class Mains
    {
        public static string GetExampleFilename(string bench_id, int seed) {
            return bench_id + ".seed" + seed.ToString() + ".csv";
        }
        //Main_SampleGen: program sample filtering.
        public static void Main_SampleGen(string bench_dir, string bench_id, int seed, int count, string output_dir)
        {
            Console.WriteLine("================ Program Sample Generation ================");
            RandStr.setRandomSeed(seed);
            
            string csvname = Path.Join(bench_dir, bench_id) + ".csv";
            string textname = csvname + ".txt";
            string oldProgReadable = File.ReadAllText(textname);
            List<string[]> strExamples = Utility.getStrExamplesFromCSVFile(csvname);
            Console.WriteLine(" # Examples: " + strExamples.Count);


            //init grammar and synthesis engine.
            MyWitness.verbose = false;
            MySemantics.verbose = false;
            MySynthesizer synth = new MySynthesizer();
            foreach (var eg in strExamples) synth.AddExample(eg);
            ProgramSet progset = synth.LearnProgramSet();
            ProgramNode prog = progset.RealizedPrograms.First();
            string newProgReadable = prog.PrintAST(ASTSerializationFormat.HumanReadable);
            if(oldProgReadable == newProgReadable) {
                Console.WriteLine("# ProgCheck-Passed: " + newProgReadable);
            }
            else {
                Console.WriteLine("# ERROR: program mismatch:");
                Console.WriteLine("OLD: " + oldProgReadable);
                Console.WriteLine("NEW: " + newProgReadable);
            }
            //PROSE cannot do deserialization ...
            //string xmlname = csvname + ".xml";
            //string xmlFormatText = File.ReadAllText(filename);
            //ProgramNode prog = MySynthesizer.grammar.fromXMLFile(filename);
            //Console.WriteLine("# parsed program: " + prog.PrintAST(ASTSerializationFormat.HumanReadable));
            
            int validCounter = 0;
            List<string[]> filteredExamples = new List<string[]>();
            while(true)
            {
                string randStr = RandStr.randomStr(8, 16);
                string result = prog.execute(randStr);
                if(result != null)
                {
                    Debug.Assert(result.Length > 0);
                    validCounter++;
                    filteredExamples.Add(new string[] { randStr, result });
                    if (validCounter >= count) break;
                }
            }
            string egFile = Path.Join(output_dir, GetExampleFilename(bench_id, seed));
            Utility.writeStrExamplesToCSVFile(egFile, filteredExamples);
        }   
    }
}
