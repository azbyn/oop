using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace BigIntInterface
{
    public class TokenizerException : Exception
    {
        public TokenizerException(string line, int pos, string msg)
            : base($"{msg}:\n{line}\n{new string(' ', pos)}^") {}
    }
    public class Tokenizer
    {
        List<Token> result;
        string str;
        string orgStr;

        public static Tokenizer Instance { get; } = new Tokenizer();
        private Tokenizer() {}

        bool Pat(string s, Token.Type tt)
        {
            if (str.StartsWith(s))
            {
                result.Add(new Token(tt, s));
                str = str.Substring(s.Length);
                return true;
            }
            return false;
        }

        bool RPat(string reg, Token.Type tt)
        {
            return RPat(reg, 0, tt);
        }
        bool RPat(string reg, int i, Token.Type tt)
        {
            var m = Regex.Match(str, "^" + reg);
            if (m.Success)
            {
                var s = m.Groups[i].Value;
                result.Add(new Token(tt, s));
                str = str.Substring(m.Length);
                return true;
            }
            return false;
        }

        bool RPat(string reg)
        {
            var m = Regex.Match(str, "^" + reg);
            if (m.Success)
            {
                str = str.Substring(m.Length);
                return true;
            }
            return false;
        }
        // returns true on success
        public bool TryTokenize(string s, out List<Token> res, 
                                out TokenizerException err) 
        {
            try
            {
                res = Tokenize(s);
                err = null;
                return true;
            } 
            catch (TokenizerException e)
            {
                err = e;
                res = null;
                return false;
            }

        }
        //can throw TokenizerException
        public List<Token> Tokenize(string s)
        {
            result = new List<Token>();
            str = string.Join(" ", s.Split('\n','\r'));
            orgStr = str;
            while (str.Length > 0) 
            {
                if (RPat(@"\s+")) continue;
                if (RPat(@"#.*")) continue;
                if (Pat(".", Token.Type.Dot)) continue;
                if (RPat(@"[0-9]+", Token.Type.Int)) continue;
                if (RPat(@"[_a-zA-Z][_a-zA-Z0-9]*", Token.Type.Var)) continue;
                if (Pat("\\", Token.Type.Lambda)) continue;
                if (Pat("λ",  Token.Type.Lambda)) continue;
                if (Pat("(",  Token.Type.LeftParen)) continue;
                if (Pat(")",  Token.Type.RightParen)) continue;
                if (Pat("+",  Token.Type.Plus)) continue;
                if (Pat("-",  Token.Type.Minus)) continue;
                if (Pat("**", Token.Type.Pow)) continue;
                if (Pat("^^", Token.Type.Xor)) continue;
                if (Pat("^",  Token.Type.Pow)) continue;
                if (Pat("*",  Token.Type.Mul)) continue;
                if (Pat("/",  Token.Type.Div)) continue;
                if (Pat("%",  Token.Type.Mod)) continue;
                if (Pat("<<", Token.Type.Lsh)) continue;
                if (Pat(">>", Token.Type.Rsh)) continue;
                if (Pat("==", Token.Type.Eq)) continue;
                if (Pat("!=", Token.Type.Ne)) continue;
                if (Pat("<=", Token.Type.Le)) continue;
                if (Pat(">=", Token.Type.Ge)) continue;
                if (Pat("<" , Token.Type.Lt)) continue;
                if (Pat(">" , Token.Type.Gt)) continue;
                if (Pat("&&", Token.Type.And)) continue;
                if (Pat("||", Token.Type.Or)) continue;
                if (Pat("!",  Token.Type.Not)) continue;
                if (Pat("&",  Token.Type.BAnd)) continue;
                if (Pat("|",  Token.Type.BOr)) continue;
                if (Pat("=",  Token.Type.Assign)) continue;
                if (Pat("?",  Token.Type.Question)) continue;
                if (Pat(":",  Token.Type.Colon)) continue;

                throw new TokenizerException(
                    orgStr, orgStr.Length - str.Length,
                    $"Invalid char '{str[0]}'");
            }
            return result;
        }
    }
}
