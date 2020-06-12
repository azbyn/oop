using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    using Pos = Int32;
    
    public class EvalException : Exception {
        public readonly Pos pos;
        public EvalException(Pos pos, string msg) : base(msg) {
            this.pos = pos;
        }
    }

    public class Evaluator : AstNode.IVisitor<EvalObject>
    {
        Environment env;
        private Evaluator() {}
        static readonly Evaluator instance = new Evaluator();
        public static Evaluator Instance { get { return instance; } }

        public EvalObject Eval(AstNode node)
        {
            return Visit((dynamic) node);
        }
        public EvalObject Visit(AstNode.Int v)
        {
            return new EvalObject.Int(v.val);
        }
        public EvalObject Visit(AstNode.Binary v)
        {
            var a = EnforceEvalInt(v.a);
            var b = EnforceEvalInt(v.b);
            return new EvalObject.Int(v.fun(a, b));
        }
        public EvalObject Visit(AstNode.Unary v)
        {
            var a = EnforceEvalInt(v.a);
            return new EvalObject.Int(v.fun(a));
        }
        public EvalObject Visit(AstNode.Variable v)
        {
            var val = env.TryFind(v.name);
            if (val == null)
                throw new EvalException(v.pos, "Variable undefined");
            return val;
        }
        public EvalObject Visit(AstNode.FunCall v)
        {
            var fun = Eval(v.fun);
            switch (fun) {
            case EvalObject.Int:
                throw new EvalException(v.pos, "Int is not callable");
            case EvalObject.Builtin b:
                return b.Call(Eval(v.arg));
            case EvalObject.Lambda l:
                return l.Call(Eval(v.arg), env);
            default:
                throw new InvalidOperationException();
            }
        }
        public EvalObject Visit(AstNode.Lambda v) {
            return new EvalObject.Lambda(v);
        }
        public EvalObject Visit(AstNode.Assign v) {
            var variable = v.lvalue as AstNode.Variable;
            if (variable == null)
                throw new EvalException(v.pos, "Object is not assignable");
            var value = Eval(v.rvalue);
            return env.Assign(variable.name, value);
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

        BigInt EnforceEvalInt(AstNode n) {
             return EnforceInt(n.pos, Eval(n));
        }

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
