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
        public static void Main_SynthTestrun(string test_dir)
        {
            Console.WriteLine("================ synthesis debug and test ================");
            string rootDir = test_dir;
            var fnameInfo = Utility.pickFileFromDir(rootDir, "csv");
            var fname = fnameInfo.Item1;
            var fid = fnameInfo.Item2;

            List<string[]> strExamples = Utility.getStrExamplesFromCSVFile(fname);
            Console.WriteLine(" #Examples: " + strExamples.Count);


            //init grammar and synthesis engine.
            MyWitness.verbose = true;
            MySemantics.verbose = true;
            MySynthesizer synth = new MySynthesizer();
            foreach (var eg in strExamples) synth.AddExample(eg);

            Stopwatch sw = new Stopwatch();
            sw.Start();
            ProgramSet vsa = synth.LearnProgramSet();
            sw.Stop();
            TimeSpan ts2 = sw.Elapsed;
            Console.WriteLine("\n -------------------------");
            Console.WriteLine("Cost: " + ts2.TotalMilliseconds + " milliseconds.");
            Console.WriteLine("VSA Volume: " + vsa.Volume);

            MySemantics.verbose = false;
            int validateCounter = 0;
            string progFilename = Path.Join(rootDir, fid);
            ProgramNode lastProg = null;
            foreach (ProgramNode prog in vsa.RealizedPrograms)
            {
                if (validateCounter == 0)
                {
                    Console.WriteLine("#first-prog: " + prog.ToString());
                    prog.ToXMLTxtFile(progFilename);
                    //ProgramNode newProg = ProgramNode.Parse(prog.PrintAST(), prog.Grammar);
                    //Console.WriteLine(newProg);
                    //PROSE cannot deserialize program.
                    Console.WriteLine("# Validating ...");
                }
                
                validateCounter++;
                foreach (var eg in strExamples)
                {
                    Debug.Assert(prog.checkExample(eg));
                }
                lastProg = prog;
            }
            Console.WriteLine("check " + validateCounter + " programs correct.");
            Console.WriteLine("#" + validateCounter + "-prog: " + lastProg.ToString());
            return;
        }
    }
}
