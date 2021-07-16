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
using System.Text.RegularExpressions;

namespace PROSE_StrGen
{
    
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("\n\tStrPROSE 0.1\n\t2021.3.20\n\n");
            if(args.Length == 0 || args[0] == "--help") {
                Console.WriteLine("Command line argments required. \n");
                Console.WriteLine("--samplegen <benchmark_dir> <benchmark_id> <seed> <count> <output_dir>\n\t   Generate examples based on a benchmark program specified by <benchmark_id>.csv and <benchmark_id>.txt");
                Console.WriteLine("--synthloop <example_file> <cache_dir>\n\t   Start a synthesize loop. <example_file> is a csv file with I/O examples.");
                Console.WriteLine("--test <test_dir>\n\t   Select a csv file in <testpath> to do synthesize. (for debugging)");
                return;
            }
            if(args[0] == "--samplegen") {
                string bench_dir = args[1];
                string bench_id = args[2];
                int seed = Convert.ToInt32(args[3]);
                int count = Convert.ToInt32(args[4]);
                string output_dir = args[5];
                
                Mains.Main_SampleGen(bench_dir, bench_id, seed, count, output_dir);
            } else if(args[0] == "--synthloop") {
                string example_file = args[1];
                string cache_dir = args[2];
                
                Mains.Main_SynthesizeLoop(example_file, cache_dir);
            } else if(args[0] == "--test") {
                string test_dir = args[1];

                Mains.Main_SynthTestrun(test_dir);
            } else if(args[0] == "--testpredict") {
                string test_dir = args[1];
                int eg_number = Convert.ToInt32(args[2]);
                Mains.Main_SynthTestpredict(test_dir, eg_number);
            } else {
                Console.WriteLine("ERROR! unknown argument \"" + args[0] + "\". Please use --help to see notes.");
            }
            //Main3("randStr-basic-2", 100000);
            //Main3("randStr-basic-1", 100000);
        }
    }
}
