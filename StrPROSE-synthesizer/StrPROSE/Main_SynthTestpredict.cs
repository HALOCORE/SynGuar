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
        public static void Main_SynthTestpredict(string test_file, int eg_number)
        {
            Console.WriteLine("================ synthesis test and predict rest ================");
            var fname = test_file;

            List<string[]> strExamples = Utility.getStrExamplesFromCSVFile(fname);
            Console.WriteLine(" #Examples: " + strExamples.Count);


            //init grammar and synthesis engine.
            MyWitness.verbose = true;
            MySemantics.verbose = true;
            MySynthesizer synth = new MySynthesizer();
            int addedCount = 0;
            foreach (var eg in strExamples) {
                synth.AddExample(eg);
                addedCount += 1;
                if(addedCount >= eg_number) break;
            }

            MySemantics.verbose = false;

            Stopwatch sw = new Stopwatch();
            sw.Start();
            ProgramSet vsa = synth.LearnProgramSet();
            sw.Stop();
            TimeSpan ts2 = sw.Elapsed;
            Console.WriteLine("\n -------------------------");
            Console.WriteLine("Cost: " + ts2.TotalMilliseconds + " milliseconds.");
            Console.WriteLine("VSA Volume: " + vsa.Volume);
            Console.WriteLine("VSA Size: " + vsa.Size);
            var etr = vsa.RealizedPrograms.GetEnumerator();
            etr.MoveNext();
            
            var currentProgram = etr.Current;
            string currentProgramStr = currentProgram.PrintAST(Microsoft.ProgramSynthesis.AST.ASTSerializationFormat.HumanReadable);
            
            int outputCounter = 0;
            foreach (var eg in strExamples) {
                var result = currentProgram.runExampleInput(eg[0]);
                Console.WriteLine(eg[0] + "~" + result + "!" + eg[1]);
                outputCounter ++;
                if(outputCounter >= 200) break;
            }

            Console.WriteLine("currentProgramStr: " + currentProgramStr);
            return;
        }
    }
}
