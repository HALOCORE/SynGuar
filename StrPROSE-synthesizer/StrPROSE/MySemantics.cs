using Microsoft.ProgramSynthesis.Rules.Concepts;
using Microsoft.ProgramSynthesis.Transformation.Text.Build.NodeTypes;
using Microsoft.ProgramSynthesis.Utils;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace PROSE_StrGen
{
    public static class MySemantics
    {
        public static bool verbose = false;
        public static string Concats(string term, string program)
        {
            if (term == null || program == null) return null;
            return term + program;
        }

        public static string UpperCase(string term, string x)
        {
            if (x.IndexOf(term) >= 0) return term.ToUpper();
            else return null;
        }

        public static string LowerCase(string term, string x)
        {
            if (x.IndexOf(term) >= 0) return term.ToLower();
            else return null;
        }

        public static string ConstStr(string constStr)
        {
            return constStr;
        }

        public static string SubString(string x, int? startPos, int? endPos)
        {
            if(verbose) Console.Write("SubString called. " + x + ", " + startPos + ", " + endPos + " result->");
            if (startPos == null || endPos == null) return null;
            if (startPos < 0 || startPos > x.Length) return null;
            if (endPos < 0 || endPos > x.Length) return null;
            if (endPos <= startPos) return null;    //make substring always non-empty
            string result = x.Substring((int)startPos, (int)endPos - (int)startPos);
            if (verbose) Console.WriteLine(result);
            return result;
        }


        public static int? AbsPos(string x, int ka)
        {
            if (verbose) Console.Write("AbsPos called. " + x + ", " + ka + " result->");
            if (ka > x.Length || ka < -x.Length - 1)
            {
                return null;
            }

            int absPos = ka >= 0 ? ka : x.Length + ka + 1;
            if (verbose) Console.WriteLine(absPos);
            return absPos;
        }

        public static int? RelPos(string x, Tuple<Regex, Regex, int, int> kr)
        {
            if (verbose) Console.Write("RelPos called. " + x + ", " + kr + " result->");
            Regex left = kr.Item1;
            Regex right = kr.Item2;
            int k = kr.Item3;
            int offset = kr.Item4;
            
            MatchCollection rightMatches = right.Matches(x);
            MatchCollection leftMatches = left.Matches(x);

            List<int> matchPoss = new List<int>();

            foreach (Match leftMatch in leftMatches)
                foreach (Match rightMatch in rightMatches)
                    if (rightMatch.Index == leftMatch.Index + leftMatch.Length)
                    {
                        matchPoss.Add(rightMatch.Index);
                    }
            
            if(k >= matchPoss.Count || k < -matchPoss.Count) 
            //k should not be smaller than -matchPoss.Count?
            {
                return null;
            }

            int selectedPos = k >= 0 ? matchPoss[k] : matchPoss[matchPoss.Count + k]; 
            //when k = 1, means the last one.

            int finalPos = selectedPos + offset;
            if (finalPos > x.Length || finalPos < 0)
            {
                return null;
            }
            if (verbose) Console.WriteLine(finalPos);
            return finalPos;
        }
    }
}
