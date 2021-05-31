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
        //Main2: VSASize loop.
        public static void Main_SynthesizeLoop(string example_file, string cache_dir)
        {
            Console.WriteLine("================ Synthesize Loop (" + example_file + ")================");
            string[] cacheFiles = Directory.GetFiles(cache_dir).Where(x => x.EndsWith(".csv")).ToArray();

            string cacheFilenamepart = Path.GetFileName(example_file) + ".cache.csv";
            string cacheFile = Path.Join(cache_dir, cacheFilenamepart);
            List<Utility.ExampleRecordResultCache> recordsCache = new List<Utility.ExampleRecordResultCache>();
            if (cacheFiles.Contains(cacheFile))
            {
                Console.WriteLine("# Cache File found: " + cacheFile);
                recordsCache = Utility.getExamplesResultCacheFromCSVFile(cacheFile);
            }
            
            MySynthesizer synth = new MySynthesizer();
            List<string[]> strExamples = Utility.getStrExamplesFromCSVFile(example_file);
            
            //TODO: read from file
            while(true) {
                Console.WriteLine("# Please input example_size:");
                int example_size = Convert.ToInt32(Console.ReadLine());
                if(example_size < 0) break;
                bool isDone = synth.restoreState(recordsCache, example_size);
                if (!isDone) {
                    if(recordsCache.Count >= example_size) throw new ApplicationException("Unexpected: recordsCache.Count >= example_size, but not done.");
                    for(int i = recordsCache.Count; i < example_size; i++)
                    {
                        string[] eg = strExamples[i];
                        Debug.Assert(eg.Length == 2);
                        synth.AutoAddExampleAndLearnProgramSet(eg);
                        Utility.ExampleRecordResultCache record = new Utility.ExampleRecordResultCache(
                            eg[0], eg[1], synth.currentVSASize, synth.lastEgImportant, synth.currentProgram);
                        recordsCache.Add(record);
                    }
                    Utility.writeExamplesResultCacheToCSVFile(cacheFile, recordsCache);
                }
                Console.WriteLine("# synth.currentVSASize: " + synth.currentVSASize);
                Console.WriteLine("# synth.currentProgram: " + synth.currentProgram);
            }
            Console.WriteLine("================");
            Console.WriteLine("# Bye.");
        }
    }
}
