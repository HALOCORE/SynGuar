using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text.RegularExpressions;
using Microsoft.ProgramSynthesis;
using Microsoft.ProgramSynthesis.Extraction.Text.Build.NodeTypes;
using Microsoft.ProgramSynthesis.Learning;
using Microsoft.ProgramSynthesis.Rules;
using Microsoft.ProgramSynthesis.Specifications;

namespace PROSE_StrGen
{
    public class MyWitness : DomainLearningLogic
    {
        public static bool verbose = false;
        public MyWitness(Grammar grammar) : base(grammar) {}

        [WitnessFunction(nameof(MySemantics.Concats), 0)]
        public DisjunctiveExamplesSpec WitnessConcatsTerm1(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("\n----------------- ConcatsTerm1 -----------------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var term1s = new List<string>();

                Debug.Assert(example.Value.Count() == 1);
                foreach (string output in example.Value)
                {
                    if (output.Length < 2) continue;

                    for (int i = 1; i < output.Length; ++i) //substring must be non-empty.
                    {
                        term1s.Add(output.Substring(0, i));
                    }
                    Debug.Assert(term1s.Count != 0);
                }
                if (term1s.Count == 0) return null;
                result[inputState] = term1s.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.Concats), 1, DependsOnParameters = new[] { 0 })]
        public DisjunctiveExamplesSpec WitnessConcatsTerm2(GrammarRule rule, DisjunctiveExamplesSpec spec, ExampleSpec term1Spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("-------------- ConcatsTerm2 --------------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));
            if (verbose) Console.WriteLine("term1Spec: " + prettySpec(term1Spec));

            
            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var term2s = new List<string>();

                Debug.Assert(example.Value.Count() == 1);
                foreach (string output in example.Value)
                {
                    var start = (string)term1Spec.Examples[inputState];
                    Debug.Assert(output.Length > start.Length);
                    term2s.Add(output.Substring(start.Length));
                }
                if (term2s.Count == 0) return null;
                result[inputState] = term2s.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.ConstStr), 0)]
        public DisjunctiveExamplesSpec WitnessConstStr(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- ConstStr -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;

                var constStrs = new List<string>();
                Debug.Assert(example.Value.Count() > 0);
                foreach (string output in example.Value)
                {
                    if (output == "") continue;  //substring only accept non-empty cases.
                    constStrs.Add(output);
                }

                Debug.Assert(constStrs.Count > 0);
                result[inputState] = constStrs.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.UpperCase), 0)]
        public DisjunctiveExamplesSpec WitnessUpperCaseTerm(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- UpperCaseTerm -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var input = inputState[rule.Body[1]] as string;
                var upperInput = input.ToUpper();

                var possibleStrs = new HashSet<string>();
                Debug.Assert(example.Value.Count() > 0);
                foreach (string output in example.Value)
                {
                    if (output == "") continue;  //substring only accept non-empty cases.

                    string upperOutput = output.ToUpper();
                    if (output != upperOutput) continue;

                    for (int i = upperInput.IndexOf(upperOutput); i >= 0; i = upperInput.IndexOf(upperOutput, i + 1))
                    {
                        string possibleStr = input.Substring(i, output.Length);
                        Debug.Assert(possibleStr.ToUpper() == upperOutput);
                        possibleStrs.Add(possibleStr);
                    }
                }

                if (possibleStrs.Count == 0) return null;
                result[inputState] = possibleStrs.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.LowerCase), 0)]
        public DisjunctiveExamplesSpec WitnessLowerCaseTerm(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- LowerCaseTerm -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var input = inputState[rule.Body[1]] as string;
                var lowerInput = input.ToLower();

                var possibleStrs = new HashSet<string>();
                Debug.Assert(example.Value.Count() > 0);
                foreach (string output in example.Value)
                {
                    if (output == "") continue;  //substring only accept non-empty cases.

                    string lowerOutput = output.ToLower();
                    if (output != lowerOutput) continue;

                    for (int i = lowerInput.IndexOf(lowerOutput); i >= 0; i = lowerInput.IndexOf(lowerOutput, i + 1))
                    {
                        string possibleStr = input.Substring(i, output.Length);
                        Debug.Assert(possibleStr.ToLower() == lowerOutput);
                        possibleStrs.Add(possibleStr);
                    }
                }

                if (possibleStrs.Count == 0) return null;
                result[inputState] = possibleStrs.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.SubString), 1)]
        public DisjunctiveExamplesSpec WitnessStartPosition(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- StartPosition -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var input = inputState[rule.Body[0]] as string;

                var occurrences = new List<int>();
                foreach (string output in example.Value)
                {
                    if (output == "") continue;  //substring only accept non-empty cases.
                    for (int i = input.IndexOf(output); i >= 0; i = input.IndexOf(output, i + 1))
                    {
                        occurrences.Add(i);
                    }
                }
                if (occurrences.Count == 0) return null;
                result[inputState] = occurrences.Cast<object>();
            }

            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }


        [WitnessFunction(nameof(MySemantics.SubString), 2, DependsOnParameters = new[] { 1 })]
        public DisjunctiveExamplesSpec WitnessEndPosition(GrammarRule rule, DisjunctiveExamplesSpec spec, ExampleSpec startSpec)
        {
            var result = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- EndPosition(Disjunctive) -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));
            if (verbose) Console.WriteLine("startSpec: " + prettySpec(startSpec));

            foreach (KeyValuePair<State, IEnumerable<object>> examples in spec.DisjunctiveExamples)
            {
                State inputState = examples.Key;
                var x = inputState[rule.Body[0]] as string;

                List<int> outposs = new List<int>();
                foreach (string output in examples.Value)
                {
                    int egDisjuncCount = examples.Value.Count();
                    if (egDisjuncCount > 1) if (verbose) Console.WriteLine("!!! " + inputState + " egDisjuncCount=" + egDisjuncCount);

                    foreach (int start in startSpec.DisjunctiveExamples[inputState])
                    {
                        Debug.Assert(egDisjuncCount > 0);
                        if (egDisjuncCount == 1)
                        {
                            Debug.Assert(x.Substring(start, output.Length) == output);
                            outposs.Add(start + output.Length);
                        }
                        else
                        {
                            if (x.Length >= start + output.Length && x.Substring(start, output.Length) == output)
                                outposs.Add(start + output.Length);
                        }
                    }
                }

                Debug.Assert(outposs.Count > 0);
                result[inputState] = outposs.Cast<object>();
            }
            var finalresult = new DisjunctiveExamplesSpec(result);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }


        [WitnessFunction(nameof(MySemantics.AbsPos), 1)]
        public DisjunctiveExamplesSpec WitnessKa(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var kExamples = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- Ka -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var x = inputState[rule.Body[0]] as string;

                var positions = new List<int>();
                Debug.Assert(example.Value != null);
                foreach (int pos in example.Value)
                {
                    positions.Add(pos);
                    positions.Add(pos - x.Length - 1);
                }
                if (positions.Count == 0) return null;
                kExamples[inputState] = positions.Cast<object>();
            }

            var finalresult = DisjunctiveExamplesSpec.From(kExamples);
            if (verbose) Console.WriteLine("result: " + prettySpec(finalresult));
            return finalresult;
        }

        [WitnessFunction(nameof(MySemantics.RelPos), 1)]
        public DisjunctiveExamplesSpec WitnessKr(GrammarRule rule, DisjunctiveExamplesSpec spec)
        {
            var kExamples = new Dictionary<State, IEnumerable<object>>();
            if (verbose) Console.WriteLine("----------- Kr -----------");
            if (verbose) Console.WriteLine("spec: " + prettySpec(spec));

            foreach (KeyValuePair<State, IEnumerable<object>> example in spec.DisjunctiveExamples)
            {
                State inputState = example.Key;
                var x = inputState[rule.Body[0]] as string;

                var positions = new List<Tuple<Regex, Regex, int, int>>();

                if (example.Value == null) return null; //if any example is null, return null
                foreach (int output in example.Value)
                {
                    List<Tuple<Regex, Regex, int, int>>[] krs;
                    GetStringMatchKrs(x, out krs);
                    List<Tuple<Regex, Regex, int, int>> krsForPutput = krs[output];
                    if (krsForPutput.Count() == 0)
                    {
                        continue;
                    }
                    foreach(var tp in krsForPutput)
                    {
                        Debug.Assert(tp.Item1 != anythingRegex || tp.Item2 != anythingRegex);
                        positions.Add(tp);
                    }
                }

                if (positions.Count == 0) return null;
                kExamples[inputState] = positions.Cast<object>();
            }

            var finalresult = DisjunctiveExamplesSpec.From(kExamples);
            if (verbose) Console.WriteLine("result: " + prettySpecSummary(finalresult));
            return finalresult;
        }

        public static readonly Regex anythingRegex = new Regex(@".*?");    // anything
        public static readonly Regex[] UsefulRegexes =
        {
            new Regex(@"^\s*"), // Start
            new Regex(@"\s*$"),    // end
           anythingRegex,    // anything
            new Regex(@"\w+"), // Word
            new Regex(@"\d+"), // Number
            new Regex(@"(-?\d+)(\.\d+)?"),
            new Regex(@"[A-Z]+"),
            new Regex(@"\s+"), // Space
            new Regex(@"[\,\.\;\-\|]"), // sepertor
        };
        

        private static Dictionary<string, List<Tuple<Regex,Regex,int,int>>[]> stringMatchesCache 
            = new Dictionary<string, List<Tuple<Regex, Regex, int, int>>[]>();
        
        private static void GetStringMatchKrs(string inp, out List<Tuple<Regex, Regex, int, int>>[] matchKrs)
        {
            if (!stringMatchesCache.ContainsKey(inp))
            {
                var krs = new List<Tuple<Regex, Regex, int, int>>[inp.Length + 1];
                List<int>[] leftMatches = new List<int>[inp.Length + 1];
                List<int>[] rightMatches = new List<int>[inp.Length + 1];
                for (var p = 0; p <= inp.Length; ++p)
                {
                    krs[p] = new List<Tuple<Regex, Regex, int, int>>();
                    leftMatches[p] = new List<int>();
                    rightMatches[p] = new List<int>();
                }
                for (int i = 0; i < UsefulRegexes.Length; ++i)
                {
                    Regex r = UsefulRegexes[i];
                    foreach (Match m in r.Matches(inp))
                    {
                        leftMatches[m.Index + m.Length].Add(i);
                        rightMatches[m.Index].Add(i);
                    }
                }
                int[,] kCounter = new int[UsefulRegexes.Length, UsefulRegexes.Length];
                for (int i = 0; i < UsefulRegexes.Length; i++)
                    for (int j = 0; j < UsefulRegexes.Length; j++)
                        kCounter[i,j] = 0;

                for(int p = 0; p <= inp.Length; ++p)
                {
                    foreach(int leftMatchIdx in leftMatches[p])
                    {
                        foreach (int rightMatchIdx in rightMatches[p])
                        {
                            int k = kCounter[leftMatchIdx, rightMatchIdx];
                            for (int offset = -3; offset <= 3; ++offset)
                            {
                                if (p + offset < 0 || p + offset > inp.Length) continue;
                                if (UsefulRegexes[leftMatchIdx] == anythingRegex &&
                                    UsefulRegexes[rightMatchIdx] == anythingRegex) continue;
                                krs[p + offset].Add(new Tuple<Regex, Regex, int, int>(
                                    UsefulRegexes[leftMatchIdx],
                                    UsefulRegexes[rightMatchIdx],
                                    k, offset));
                            }
                            kCounter[leftMatchIdx, rightMatchIdx] += 1;
                        }
                    }
                }

                for (int p = 0; p <= inp.Length; ++p)
                {
                    foreach (int leftMatchIdx in leftMatches[p])
                    {
                        foreach (int rightMatchIdx in rightMatches[p])
                        {
                            int k = kCounter[leftMatchIdx, rightMatchIdx];
                            for (int offset = -3; offset <= 3; ++offset)
                            {
                                if (p + offset < 0 || p + offset > inp.Length) continue;
                                if (UsefulRegexes[leftMatchIdx] == anythingRegex &&
                                    UsefulRegexes[rightMatchIdx] == anythingRegex) continue;
                                krs[p + offset].Add(new Tuple<Regex, Regex, int, int>(
                                    UsefulRegexes[leftMatchIdx],
                                    UsefulRegexes[rightMatchIdx],
                                    -k, offset));
                            }
                            kCounter[leftMatchIdx, rightMatchIdx] -= 1;
                            Debug.Assert(kCounter[leftMatchIdx, rightMatchIdx] >= 0);
                        }
                    }
                }

                stringMatchesCache.Add(inp, krs);
                matchKrs = krs;

            } else
            {
                matchKrs = stringMatchesCache[inp];
            }
        }



        // ===================================== Debug
        public string prettySpec(DisjunctiveExamplesSpec spec)
        {
            List<string> strs = new List<string>();
            foreach (var x in spec.DisjunctiveExamples)
            {
                strs.Add(x.Key.ToString());
                strs.Add(" ==> ");
                foreach (var obj in x.Value)
                {
                    strs.Add(obj.ToString());
                    strs.Add(", ");
                }
            }
            return string.Concat(strs);
        }

        public string prettySpecSummary(DisjunctiveExamplesSpec spec)
        {
            List<string> strs = new List<string>();
            foreach (var x in spec.DisjunctiveExamples)
            {
                strs.Add(x.Key.ToString());
                int count = x.Value.Count();
                strs.Add(" ==> ");
                strs.Add("{" +  "... " + count + " items }");
            }
            return string.Concat(strs);
        }


    }
}
