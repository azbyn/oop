using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BigIntInterface
{
    using EvalObject = AstNode.EvalObject;
    using Pos = Int32;
    
    public class EvalException : Exception {
        public readonly Pos pos;
        public EvalException(Pos pos, string msg) : base(msg) {
            this.pos = pos;
        }
    }

    public class Evaluator : AstNode.IVisitor<EvalObject>
    {
        readonly Environment env = new Environment();
        private Evaluator() {}

        public static Evaluator Instance { get; } = new Evaluator();

        public EvalObject Eval(AstNode node) => Visit((dynamic) node);

        public EvalObject Visit(AstNode.Int v) => v;

        public EvalObject Visit(AstNode.Binary v)
        {
            var a = EnforceEvalInt(v.a);
            var b = EnforceEvalInt(v.b);
            return new AstNode.Int(v.pos, v.fun(a, b));
        }
        public EvalObject Visit(AstNode.Unary v)
        {
            var a = EnforceEvalInt(v.a);
            return new AstNode.Int(v.pos, v.fun(a));
        }
        public EvalObject Visit(AstNode.Variable v)
        {
            var val = env.TryFind(v.name);
            if (val == null)
                throw new EvalException(v.pos, $"Variable '{v.name}' undefined");
            return val;
        }
        public EvalObject Visit(AstNode.FunCall v)
        {
            var fun = Eval(v.fun);
            return fun switch {
                EvalObject.Int _ => throw new EvalException(v.pos, "Int is not callable"),
                EvalObject.Builtin b => b.Call(Eval(v.arg)),
                EvalObject.Lambda l => l.Call(Eval(v.arg), this),
                _ => throw new InvalidOperationException(),
            };
        }
        public EvalObject Visit(AstNode.Lambda v) => v;
        public EvalObject Visit(AstNode.Assign v) {
            var value = Eval(v.val);
            return env.Assign(v.name, value);
        }
        public EvalObject Visit(AstNode.If v) {
            var cond = Eval(v.cond);
            return cond.BoolValue ? Eval(v.t) : Eval(v.f);
        }
        public EvalObject Visit(AstNode.Or v) {
            var a = Eval(v.a);
            return a.BoolValue ? a : Eval(v.b);
        }
        public EvalObject Visit(AstNode.And v)
        {
            var a = Eval(v.a);
            return a.BoolValue ? Eval(v.b) : a;
        }
        public EvalObject Visit(AstNode.Builtin v) => v;

        BigInt EnforceEvalInt(AstNode n) => EnforceInt(n.pos, Eval(n));

        BigInt EnforceInt(Pos pos, EvalObject obj)
        {
            var i = obj as EvalObject.Int;
            if (i != null)
            {
                return i.val;
                //return new BigInt(i.val);
            }
            string name = obj.GetType().Name;
            if (name.StartsWith("Eval"))
                name = name.Substring(4);
            throw new EvalException(pos, $"Expected Int, got {name}");
        }
    }
}
