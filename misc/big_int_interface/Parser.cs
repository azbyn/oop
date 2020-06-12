using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    using Pos = Int32;
    using TT = Token.Type;

    public class ParserException : Exception {
        public readonly Pos pos;
        public ParserException(Pos pos, string msg) : base(msg) {
            this.pos = pos;
        }
    }
    /*
      main: expression
      expression: funCall EOF

      funCall: [funCall] assign
      assign: boolOp [ "=" assign]
      boolOp: [boolOp ("&&" | "||")] equals
      equals: [equals ("==" | "!=")] compare
      compare: bOr [("<"|">"|"<="|">=") bOr]
      bOr: [bOr ("|" | "^^")] bAnd
      bAnd: [bAnd "&"] bShift
      bShift: [bShift ("<<" | ">>")] add
      add: [add ("+" | "-")] multi
      multi: [multi ("*" | "/" | "%")] power
      power: prefix ["^" power]
      prefix: ["+" | "-" | "!"] lambda
      lambda: "λ" Identifier "." expression
            | term
      term: "(" expression ")"
          | Int | Var
     */
    public class Parser
    {
        List<Token> tokens = null;
        Pos pos = 0;

        public AstNode Result { get; private set; }
        public ParserException Error { get; private set; }


        ParserException Expected(Token.Type tt) { return Expected(tt.ToString()); }
        ParserException Expected(string tt) {
            return new ParserException(pos, $"Expected {tt}, found {Type()}");
        }
        // public struct Error {
        //     readonly Pos pos;
        //     readonly string msg;
        //     public override string ToString() {
        //         return $"@{pos}: {msg}";
        //     }
        // }
        public Parser() {}
        bool IsEof() { return Type() == Token.Type.Eof; }
        Token.Type Type() {
            if (pos >= tokens.Count) return TT.Eof;
            return tokens[pos].type;
        }

        bool Is(Token.Type tt) {return Type() == tt;}
        bool Is(params Token.Type[] types) {
            Token.Type tt = Type();
            return types.Contains(tt);
        }
        bool Consume(Token.Type tt) {
            var res = Is(tt);
            if (res) ++pos;
            return res;
        }
        bool Consume(params Token.Type[] tt) {
            var res = Is(tt);
            if (res) ++pos;
            return res;
        }

        bool Consume(out Token t, params Token.Type[] tt) {
            var res = Is(tt);
            if (res)
                t = tokens[pos++];
            else t = new Token(); //sigh
            return res;
        }
        void Require(Token.Type tt) {
            if (!Consume(tt))
                throw Expected(tt);
        }
        void Require(out Token t, Token.Type tt) {
            if (!Consume(out t, tt))
                throw Expected(tt);
        }
        static AstNode MakeBinary(Pos pos, Token.Type type, AstNode x, AstNode y) {
            switch (type) {
            case TT.Plus:  return new AstNode.Binary(pos,x,y, (a,b)=>a+b);
            case TT.Minus: return new AstNode.Binary(pos,x,y, (a,b)=>a-b);
            case TT.Div:   return new AstNode.Binary(pos,x,y, (a,b)=>a/b);
            case TT.Mul:   return new AstNode.Binary(pos,x,y, (a,b)=>a*b);
            case TT.Mod:   return new AstNode.Binary(pos,x,y, (a,b)=>a%b);
            case TT.BAnd:  return new AstNode.Binary(pos,x,y, (a,b)=>a&b);
            case TT.Xor:   return new AstNode.Binary(pos,x,y, (a,b)=>a^b);
            case TT.BOr:   return new AstNode.Binary(pos,x,y, (a,b)=>a|b);

            case TT.Lsh:   return new AstNode.Binary(pos,x,y, BigInt.Lsh);
            case TT.Rsh:   return new AstNode.Binary(pos,x,y, BigInt.Rsh);
            case TT.Pow:   return new AstNode.Binary(pos,x,y, BigInt.Pow);

            case TT.And:   return new AstNode.And(pos,x,y);
            case TT.Or:    return new AstNode.Or (pos,x,y);

            case TT.Eq: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a==b));
            case TT.Ne: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a!=b));
            case TT.Lt: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a<=b));
            case TT.Gt: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a>=b));
            case TT.Le: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a<b));
            case TT.Ge: return new AstNode.Binary(pos,x,y,(a,b)=>(BigInt)(a>b));

            default: throw new NotSupportedException();
            }
        }
        static AstNode MakeUnary(Pos pos, Token.Type type, AstNode x) {
            switch (type) {
            case TT.Plus:  return new AstNode.Unary(pos,x,(a)=>a);
            case TT.Minus: return new AstNode.Unary(pos,x,(a)=>-a);

            case TT.Not:   return new AstNode.Unary(pos,x,(a)=>(BigInt)a.IsZero);
            default: throw new NotSupportedException();
            }
        }

        AstNode LeftRecursiveImpl(Func<AstNode> next, params Token.Type[] args)
        {
            var a = next();
            for (;;)
            {
                Pos p = pos;
                if (Consume(out Token t, args))
                    a = MakeBinary(p, t.type, a, next());
                else
                    return a;
            }
        }
        //-----The recursive descent parser bit-----

        //return true on success
        public bool Parse(List<Token> tokens) {
            Error = null;
            Result = null;
            if (tokens.Count == 0) return true;
            this.tokens = tokens;
            pos = 0;
            Main();
            return Error == null;
        }
        // main: expression
        void Main() {
            try {
                Result = Expression();
            } catch (ParserException p) {
                Error = p;
            }
        }

        // expression: funCall EOF
        AstNode Expression()
        {
            return FunCall();
        }
        // funCall: [funCall] assign
        AstNode FunCall()
        {
            var a = Assign();
            for (;;)
            {
                if (IsEof() || Is(TT.RightParen) ||
                    Is(TT.Question) || Is(TT.Colon)) return a;
                Pos p = pos;
                a = new AstNode.FunCall(p, a, Assign());
            }
        }
        // assign: ternary ["=" assign]
        AstNode Assign()
        {
            var a = Ternary();
            Pos p = pos;
            if (Consume(Token.Type.Assign))
                return new AstNode.Assign(p, a, Assign());
            return a;
        }

        // ternary: boolOp [ "?" expression ":" ternary ]
        AstNode Ternary() {
            var cond = BoolOp();
            Pos p = pos;
            if (!Consume(TT.Question))
                return cond;
            var a = Expression();
            Require(TT.Colon);
            return new AstNode.If(pos, cond, a, Ternary());
        }

        // boolOp: [boolOp ("&&" | "||")] equals
        AstNode BoolOp()
        {
            var a = Equals();
            for (;;) {
                Pos p = pos;

                if (Consume(TT.And)) {
                    a = new AstNode.And(p, a, Equals());
                }
                else if (Consume(TT.Or)) {
                    a = new AstNode.Or(p, a, Equals());
                }
                else {
                    return a;
                }
            }
        }
        // equals: [equals ("==" | "!=")] compare
        AstNode Equals()  { return LeftRecursiveImpl(Compare, TT.Eq, TT.Ne); }

        // compare: bOr [("<"|">"|"<="|">=") bOr]
        AstNode Compare() { return LeftRecursiveImpl(BOr, TT.Lt, TT.Gt, TT.Le, TT.Ge); }

        // bOr: [bOr ("|" | "^^")] bAnd
        AstNode BOr() { return LeftRecursiveImpl(BAnd, TT.BOr, TT.Xor); }

        // bAnd: [bAnd "&"] bShift
        AstNode BAnd() { return LeftRecursiveImpl(BShift, TT.BAnd); }

        // bShift: [bShift ("<<" | ">>")] add
        AstNode BShift() { return LeftRecursiveImpl(Add, TT.Lsh, TT.Rsh); }

        // add: [add ("+" | "-")] multi
        AstNode Add() { return LeftRecursiveImpl(Multi, TT.Plus, TT.Minus); }

        // multi: [multi ("*" | "/" | "%")] power
        AstNode Multi() { return LeftRecursiveImpl(Power, TT.Mul, TT.Div, TT.Mod); }
        // power: prefix ["^" power]
        AstNode Power() {
            var a = Prefix();
            Pos p = pos;
            if (!Consume(TT.Pow))
                return a;
            return MakeBinary(p, TT.Pow, a, Power());
        }

        // prefix: ["+" | "-" | "!"] lambda
        AstNode Prefix() {
            Pos p = pos;
            if (Consume(out Token t, TT.Plus, TT.Minus, TT.Not))
                return MakeUnary(p, t.type, Lambda());
            return Lambda();
        }

        // lambda: "λ" Var "." expression
        //       | term
        AstNode Lambda() {
            Pos p = pos;
            if (!Consume(TT.Lambda)) {
                return Term();
            }
            Require(out Token t, TT.Var);
            Require(TT.Dot);
            return new AstNode.Lambda(p, t.val, Expression());
        }
        // term: "(" expression ")"
        //     | Int | Var

        AstNode Term() {
            Pos p = pos;
            if (Consume(TT.LeftParen)) {
                var e = Expression();
                Console.WriteLine("OK");
                Require(TT.RightParen);
                return e;
            }
            Token t;
            if (Consume(out t, TT.Int)) {
                return new AstNode.Int(p, t.val);
            }
            if (Consume(out t, TT.Var)) {
                return new AstNode.Variable(p, t.val);
            }

            throw Expected("term");
        }
    }
}
