using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using Microsoft.ProgramSynthesis;
using Microsoft.ProgramSynthesis.Specifications;
using Microsoft.ProgramSynthesis.Learning;
using Microsoft.ProgramSynthesis.Compiler;
using Microsoft.ProgramSynthesis.VersionSpace;
using Microsoft.ProgramSynthesis.Learning.Strategies;
using System.Numerics;
using System.Dynamic;
using System.Diagnostics;
using Microsoft.ProgramSynthesis.Transformation.Tree.Build.NodeTypes;

namespace PROSE_StrGen
{
    class MySynthesizer
    {
        public static readonly string grammarText = @"
using Microsoft.ProgramSynthesis.Utils;

using semantics PROSE_StrGen.MySemantics;
using learner PROSE_StrGen.MyWitness;

language StrGen;

@input string x;
@start string program := recTerm;

string recTerm := catTerm | Concats(catTerm, recTerm);
string catTerm := ConstStr(cs) | convTerm;
string convTerm := term | UpperCase(term, x) | LowerCase(term, x);
string term := SubString(x, pos, pos);
int? pos := AbsPos(x, ka) | RelPos(x, kr);

string cs;
int ka;
Tuple<Regex,Regex,int,int> kr; 
";

        public static readonly Grammar grammar = DSLCompiler.Compile(new CompilerOptions
        {
            InputGrammarText = grammarText,
            References = CompilerReference.FromAssemblyFiles(typeof(MySynthesizer).GetTypeInfo().Assembly)
        }).Value;

        private static SynthesisEngine engine;

        public int ExampleCount
        {
            get
            {
                return examples.Count;
            }
        }

        public MySynthesizer()
        {
            Console.WriteLine("# Constructing Synthesizer ...");
            //GrammarTest();

            var witnessFunctions = new MyWitness(grammar);
            var deductiveSynthesis = new DeductiveSynthesis(witnessFunctions);
            var synthesisExtrategies = new ISynthesisStrategy[] { deductiveSynthesis };
            var synthesisConfig = new SynthesisEngine.Config { Strategies = synthesisExtrategies };
            engine = new SynthesisEngine(grammar, synthesisConfig);

            Reset();
            //LearnTest();
        }


        private Dictionary<State, object> examples = new Dictionary<State, object>();
        public void AddExample(string[] example)
        {
            if (example.Length != 2)
                throw new NotSupportedException("#example != 2 not supported.");
            var input = State.CreateForLearning(grammar.InputSymbol, example[0]);
            var output = example[1];
            examples.Add(input, output);
        }

        public BigInteger currentVSASize { get; private set; }
        public string currentProgram { get; private set; }
        public bool lastEgImportant { get; private set; }
        public void Reset() { 
            this.currentVSASize = -1;
            this.currentProgram = null;
            this.lastEgImportant = false;
            this.examples.Clear();
        }

        public bool restoreState(List<Utility.ExampleRecordResultCache> recordsCache, int example_size) {
            this.Reset();
            if (example_size <= 0) throw new InvalidOperationException("restoreState must have example_size > 0");
            int currentEgSize = 0;
            foreach(var record in recordsCache) {

                if(record.important) {
                    this.AddExample(new string[] {record.input0, record.output});
                }
                this.currentProgram = record.program;
                this.currentVSASize = record.H;
                this.lastEgImportant = record.important;

                currentEgSize += 1;
                if (currentEgSize == example_size) break;
            }
            if (currentEgSize == example_size) return true;
            else return false;
        }
        public ProgramSet AutoAddExampleAndLearnProgramSet(string[] example)
        {
            if (example.Length != 2)
                throw new NotSupportedException("# example != 2 not supported.");
            
            var input = State.CreateForLearning(grammar.InputSymbol, example[0]);
            var output = example[1];
            if(examples.ContainsKey(input)) {
                Console.WriteLine("# WARNING: duplicated examples!");
                this.lastEgImportant = false;
                
            } else {
                examples.Add(input, output);
            }
            var spec = new ExampleSpec(examples);
            ProgramSet learned = engine.LearnGrammar(spec);
            BigInteger newSize = learned.Size;
            if (newSize < currentVSASize || currentVSASize == -1)
            {
                Console.WriteLine("# Important Example.  #eg=" + examples.Count + " newVSAsize=" + newSize);
                this.currentVSASize = newSize;
                this.lastEgImportant = true;
                var etr = learned.RealizedPrograms.GetEnumerator();
                etr.MoveNext();
                this.currentProgram = etr.Current.PrintAST(Microsoft.ProgramSynthesis.AST.ASTSerializationFormat.HumanReadable);
            } 
            else
            {
                Debug.Assert(newSize == currentVSASize);
                this.lastEgImportant = false;
                examples.Remove(input);
                Console.Write("#");
            }
            return learned;
        }

        public ProgramSet LearnProgramSet()
        {
            var spec = new ExampleSpec(examples);
            ProgramSet learned = engine.LearnGrammar(spec);
            Console.WriteLine("#LearnProgramSet size=" + learned.Size);
            return learned;
        }


        // ========================================= test code
        public bool GrammarTest()
        {
            Console.WriteLine("--- GrammarTest ---");
            //string prog1 = @"SubString(x, 2)"; //RelPos(x, (\s+, \w+), 0, 0)
            //var ast = ProgramNode.Parse(prog1, grammar, ASTSerializationFormat.HumanReadable);
            //var input = State.CreateForExecution(grammar.InputSymbol, "PROSE Rocks");
            //var output = (string)ast.Invoke(input);
            //Console.WriteLine(output);
            return true;
        }

        public bool LearnTest()
        {
            var input1 = State.CreateForLearning(grammar.InputSymbol, "PROSE Rocks PROSE");
            var input2 = State.CreateForLearning(grammar.InputSymbol, "HAHAH key HAHAH");
            string desiredOutput1 = "PROSE";
            string desiredOutput2 = "HAHAH";
            var spec = new ExampleSpec(new Dictionary<State, object> { 
                [input1] = desiredOutput1, [input2] = desiredOutput2 });

            ProgramSet learned = engine.LearnGrammar(spec);
            Console.WriteLine(learned.Size);

            if (learned.Size == 0) return false;
            return true;
        }

    }
}
