using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    using Pos = Int32;
    public abstract class AstNode
    {
        public readonly Pos pos;

        protected AstNode(Pos pos) {
            this.pos = pos;
        }

        public class Int : AstNode
        {
            public readonly BigInt val;
            public Int(Pos pos, string val) : base(pos) { this.val = (BigInt) val; }
        }
        public class Binary : AstNode
        {
            public delegate BigInt Fun(BigInt a, BigInt b);
            public readonly AstNode a, b;
            public readonly Fun fun;
            public Binary(Pos pos, AstNode a, AstNode b, Fun fun) : base(pos) {
                this.fun = fun;
                this.a = a;
                this.b = b;
            }
        }

        public class Unary : AstNode {
            public delegate BigInt Fun(BigInt a);
            public readonly AstNode a;
            public readonly Fun fun;

            public Unary(Pos pos, AstNode a, Fun fun) : base(pos) {
                this.fun = fun;
                this.a = a;
            }
        }

        public class Variable : AstNode
        {
            public readonly string name;
            public Variable(Pos pos, string name) :base(pos) {this.name = name;}
        }
        public class FunCall : AstNode
        {
            public readonly AstNode fun;
            public readonly AstNode arg;
            public FunCall(Pos pos, AstNode fun, AstNode arg) :base(pos) {
                this.fun = fun;
                this.arg = arg;
            }
        }
        public class Lambda : AstNode
        {
            public readonly string param;
            public readonly AstNode body;
            public Lambda(Pos pos, string param, AstNode body) :base(pos) {
                this.param = param;
                this.body = body;
            }
        }
        public class Assign : AstNode {
            public readonly AstNode rvalue;
            public readonly AstNode lvalue;
            public Assign(Pos pos, AstNode rvalue, AstNode lvalue) :base(pos) {
                this.lvalue = lvalue;
                this.rvalue = rvalue;
            }
        }

        public class Or : AstNode {
            public readonly AstNode a, b;
            public Or(Pos pos, AstNode a, AstNode b) :base(pos) {
                this.a = a;
                this.b = b;
            }
        }
        public class And : AstNode {
            public readonly AstNode a, b;
            public And(Pos pos, AstNode a, AstNode b) :base(pos) {
                this.a = a;
                this.b = b;
            }
        }
        public class If : AstNode {
            public readonly AstNode cond, t, f;
            public If(Pos pos, AstNode cond, AstNode t, AstNode f) :base(pos) {
                this.t = t;
                this.f = f;
                this.cond = cond;
            }
        }


        public interface IVisitor<T> {
            T Visit(Int v);
            T Visit(Binary v);
            T Visit(Unary v);
            T Visit(Variable v);
            T Visit(FunCall v);
            T Visit(Lambda v);
            T Visit(Assign v);
            T Visit(Or v);
            T Visit(And v);
            T Visit(If v);
        }

        public class LambdaVisitor<T> : IVisitor<T> {
            readonly Func<Int, T> i;      public T Visit(Int v)       { return i(v); }
            readonly Func<Binary, T> bin; public T Visit(Binary v)    { return bin(v); }
            readonly Func<Unary, T> u;    public T Visit(Unary v)     { return u(v); }
            readonly Func<Variable, T> v; public T Visit(Variable v_) { return v(v_); }
            readonly Func<FunCall, T> fc; public T Visit(FunCall v)   { return fc(v); }
            readonly Func<Lambda, T> l;   public T Visit(Lambda v)    { return l(v); }
            readonly Func<Assign, T> a_;  public T Visit(Assign v)    { return a_(v); }
            readonly Func<Or, T> o;       public T Visit(Or v)        { return o(v); }
            readonly Func<And, T> a;      public T Visit(And v)       { return a(v); }
            readonly Func<If, T> if_;     public T Visit(If v)        { return if_(v); }


            public LambdaVisitor(Func<Int, T> i,
                                 Func<Binary, T> bin,
                                 Func<Unary, T> u,
                                 Func<Variable, T> v,
                                 Func<FunCall, T> fc,
                                 Func<Lambda, T> l,
                                 Func<Assign, T> asgn,
                                 Func<Or, T> o,
                                 Func<And, T> a,
                                 Func<If, T> if_) {
                this.i = i;
                this.bin = bin;
                this.v = v;
                this.fc = fc;
                this.l = l;
                this.a_ = asgn;
                this.o = o;
                this.a = a;
                this.u = u;
                this.if_ = if_;
            }

        }
        public string ToStringTree() {
            return ToStringTree(0);
        }

        public string ToStringTree(int level) {
            var indent = new string(' ', level*2);
            if (level != 0) indent += "| ";
            string PrintChildren(params AstNode[] nodes) {
                string res ="\n";
                foreach (var n in nodes)
                    res += n.ToStringTree(level+1);
                return res;
            }
           
            var impl = new LambdaVisitor<string>(
                (Int v) => $"int: {v.val}\n",
                (Binary v) => $"binary: "+PrintChildren(v.a, v.b),
                (Unary v) => $"unary: "+PrintChildren(v.a),
                (Variable v) => $"var '{v.name}'\n",
                (FunCall v) => $"funCall: "+PrintChildren(v.fun, v.arg),
                (Lambda v) => $"lambda {v.param}: "+PrintChildren(v.body),
                (Assign v) => $"assign: "+PrintChildren(v.rvalue, v.rvalue),
                (Or v) => $"or: "+PrintChildren(v.a, v.b),
                (And v) => $"and: "+PrintChildren(v.a, v.b),
                (If v) => $"if: "+PrintChildren(v.cond, v.t, v.f)
            );
            return indent+ impl.Visit((dynamic)this);
        }
        public override string ToString() {
            var impl = new LambdaVisitor<string>(
                (Int v) => v.val.ToString(),
                (Binary v) => $"({v.a} @ {v.b})",
                (Unary v) => $"(~{v.a})",
                (Variable v) => $"{v.name}",
                (FunCall v) => $"({v.fun} {v.arg})",
                (Lambda v) => $"λ{v.param}. {v.body}",
                (Assign v) => $"({v.rvalue} = {v.rvalue})",
                (Or v) => $"({v.a} || {v.b})",
                (And v) => $"({v.a} && {v.b})",
                (If v) => $"({v.cond} ? {v.t} : {v.f})"
            );
            return impl.Visit((dynamic)this);
        }
    }
}
