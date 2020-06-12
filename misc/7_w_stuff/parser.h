#pragma once
#include "posException.h"
#include "token.h"

#include <span>

struct ParserException : PosException {
    ParserException(Pos pos, std::span<Token> tokens, std::string_view msg)
            : PosException(pos, tokens, msg) {}
};
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
class Parser {
    std::span<Token> tokens;
    Pos pos = 0;

public:
    static Parser Instance() { static Parser i; return i; }

private:
    Parser() {}
    
    ParserException expected(TT tt) { return Expected(toString(tt)); }
    ParserException Expected(std::string_view tt) {
        return ParserException(pos, concat("Expected ", tt, ", found ",
                                           toString(type())));
    }
    bool IsEof() { return pos >= tokens.size(); }
    TT type() {
        if (pos >= tokens.size())
            return TT::Eof;
        return tokens[pos].type;
    }

    bool is(TT tt) { return type() == tt; }
    template<typename ... Args>
    bool is(Args... types) {
        auto tt = Type();
        return (tt == args || ...);
    }
    bool consume(TT tt) {
        var res = Is(tt);
        if (res) ++pos;
        return res;
    }
    template<typename ... Args>
    bool consume(Token& t, Args ... types) {
        var res = is(types ...);
        if (res)
            t = tokens[pos++];
        return res;
    }
    void require(TT tt) {
        if (!consume(tt))
            throw expected(tt);
    }
    void Require(Token& t, Token.Type tt) {
        if (!consume(t&, tt))
            throw expected(tt);
    }
    static AstNode MakeBinary(Pos pos, Token t, AstNode x, AstNode y) {
            AstNode.Binary.Fun fun = t.type switch {
                TT.Plus  => (a, b) => a+b, 
                TT.Minus => (a, b) => a-b,
                TT.Div   => (a, b) => a/b,
                TT.Mul   => (a, b) => a*b,
                TT.Mod   => (a, b) => a%b,
                TT.BAnd  => (a, b) => a&b,
                TT.Xor   => (a, b) => a^b,
                TT.BOr   => (a, b) => a|b,
                TT.Lsh   => BigInt.Lsh,
                TT.Rsh   => BigInt.Rsh,
                TT.Pow   => BigInt.Pow,
                TT.Eq    => (a, b) => (BigInt)(a==b),
                TT.Ne    => (a, b) => (BigInt)(a!=b),
                TT.Le    => (a, b) => (BigInt)(a<=b),
                TT.Ge    => (a, b) => (BigInt)(a>=b),
                TT.Lt    => (a, b) => (BigInt)(a<b),
                TT.Gt    => (a, b) => (BigInt)(a>b),

                // we don't throw ParserException since it's a bug
                // in the code if we reach this point, not a problem
                // with our input
                _ => throw new NotSupportedException(),
            };
            return new AstNode.Binary(pos, t.val, fun, x, y);
        }
        static AstNode MakeUnary(Pos pos, Token t, AstNode x) {
            AstNode.Unary.Fun fun = t.type switch {
                TT.Plus  => a => a,
                TT.Minus => a => -a,
                TT.Not   => a => (BigInt)a.IsZero,
                _ => throw new NotSupportedException(),
            };
            return new AstNode.Unary(pos, t.val, fun, x);
        }

        AstNode LeftRecursiveImpl(Func<AstNode> next, params Token.Type[] args)
        {
            var a = next();
            for (;;)
            {
                Pos p = pos;
                if (Consume(out Token t, args))
                    a = MakeBinary(p, t, a, next());
                else
                    return a;
            }
        }
        //-----The recursive descent parser bit-----

        // returns true on success
        public bool TryParse(List<Token> tokens, out AstNode res,
                             out ParserException err) 
        {
            try 
            {
                res = Parse(tokens);
                err = null;
                return true;
            } 
            catch (ParserException e) 
            {
                err = e;
                res = null;
                return false;
            }
        }

        // throws ParserException on failure
        // may return null if there are no tokens
        public AstNode Parse(List<Token> tokens) 
        {
            if (tokens.Count == 0) return null;
            this.tokens = tokens;
            pos = 0;
            return Main();
        }
        // main: expression
        AstNode Main() => Expression();

        // expression: assign EOF
        AstNode Expression() => Assign();

        // assign: var "=" assign
        //       | funCall
        AstNode Assign()
        {
            var a = FunCall();
            Pos p = pos;
            if (Consume(Token.Type.Assign))
            {
                var variable = a as AstNode.Variable;
                if (variable == null)
                    throw new ParserException(pos, "Variable expected");

                return new AstNode.Assign(p, variable.name, Assign());
            }
            return a;
        }


        // funCall: [funCall] ternary
        AstNode FunCall()
        {
            var a = Ternary();
            for (;;)
            {
                if (IsEof() || Is(TT.RightParen) || Is(TT.Question) || 
                    Is(TT.Colon) || Is(TT.Assign)) return a;
                Pos p = pos;
                a = new AstNode.FunCall(p, a, Ternary());
            }
        }

        // ternary: boolOp [ "?" expression ":" ternary ]
        AstNode Ternary()
        {
            var cond = BoolOp();
            Pos p = pos;

            if (!Consume(TT.Question))
                return cond;
            var a = Expression();
            Require(TT.Colon);

            return new AstNode.If(p, cond, a, Ternary());
        }

        // boolOp: [boolOp ("&&" | "||")] equals
        AstNode BoolOp()
        {
            var a = Equals();
            for (;;) 
            {
                Pos p = pos;

                if (Consume(TT.And))
                    a = new AstNode.And(p, a, Equals());
                else if (Consume(TT.Or))
                    a = new AstNode.Or(p, a, Equals());
                else
                    return a;
            }
        }
        // equals: [equals ("==" | "!=")] compare
        AstNode Equals() => LeftRecursiveImpl(Compare, TT.Eq, TT.Ne);

        // compare: bOr [("<"|">"|"<="|">=") bOr]
        AstNode Compare() => LeftRecursiveImpl(BOr, TT.Lt, TT.Gt, TT.Le, TT.Ge);

        // bOr: [bOr ("|" | "^^")] bAnd
        AstNode BOr() => LeftRecursiveImpl(BAnd, TT.BOr, TT.Xor);

        // bAnd: [bAnd "&"] bShift
        AstNode BAnd() => LeftRecursiveImpl(BShift, TT.BAnd);

        // bShift: [bShift ("<<" | ">>")] add
        AstNode BShift() => LeftRecursiveImpl(Add, TT.Lsh, TT.Rsh);

        // add: [add ("+" | "-")] multi
        AstNode Add() => LeftRecursiveImpl(Multi, TT.Plus, TT.Minus);

        // multi: [multi ("*" | "/" | "%")] power
        AstNode Multi() => LeftRecursiveImpl(Power, TT.Mul, TT.Div, TT.Mod);

        // power: prefix ["^" power]
        AstNode Power()
        {
            var a = Prefix();
            Pos p = pos;

            if (Consume(out Token t, TT.Pow))
                return MakeBinary(p, t, a, Power());
            return a;
        }

        // prefix: ["+" | "-" | "!"] lambda
        AstNode Prefix() {
            Pos p = pos;
            if (Consume(out Token t, TT.Plus, TT.Minus, TT.Not))
                return MakeUnary(p, t, Lambda());
            return Lambda();
        }

        // lambda: "λ" Var "." expression
        //       | term
        AstNode Lambda() 
        {
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
                Require(TT.RightParen);
                return e;
            }
            Token t;
            if (Consume(out t, TT.Int))
                return new AstNode.Int(p, t.val);

            if (Consume(out t, TT.Var))
                return new AstNode.Variable(p, t.val);

            throw Expected("term");
        }
    }
}
