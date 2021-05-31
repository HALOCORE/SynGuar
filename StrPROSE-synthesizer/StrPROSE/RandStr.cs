using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace PROSE_StrGen
{
    public static class RandStr
    {
        const string charsA = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const string charsa = "abcdefghijklmnopqrstuvwxyz";
        const string chars0 = "0123456789";
        const string charsD = ",.-;|";
        const string charsE = "     ";
        const string charsAll = charsA + charsa + chars0 + charsD + charsE + charsE + charsE;

        private static Random random = null;
        public static void setRandomSeed(int seed) {
            Console.WriteLine("# [RandStr] set seed: " + seed.ToString());
            random = new Random(seed);
        }
        public static string randomStr(int minLen, int maxLen)
        {
            int length = random.Next(minLen, maxLen);
            return new string(Enumerable.Repeat(charsAll, length)
              .Select(s => s[random.Next(s.Length)]).ToArray());
        }
    }
}
