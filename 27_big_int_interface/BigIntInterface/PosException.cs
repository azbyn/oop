using System;
using System.Collections.Generic;
using System.Linq;

namespace BigIntInterface 
{
    using Pos = Int32;
    public class PosException : Exception
    {
        public readonly Pos pos;
        public PosException(Pos pos, string msg) : base(msg) 
        {
            this.pos = pos;
        }

        public string ToNiceString(List<Token> tokens) 
        {
            string res = "";
            int spaceCount = 0;
            int i = 0;
            foreach (var t in tokens)
            {
                string s = t.val;
                if (t.type == Token.Type.Var) s += ' ';
                res += s;
                if (i++ < pos)
                    spaceCount += s.Length;
            }

            return Message + "\n"+ res + "\n"+new string(' ', spaceCount)+"^";
        }
    }
}