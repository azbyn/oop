using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BigIntInterface
{
 /*   public abstract class EvalObject {
        public abstract bool BoolValue { get; }

        public class BoundLambda : EvalObject 
        {
            readonly AstNode.Lambda val;

            public override bool BoolValue => true;

            public BoundLambda(AstNode.Lambda l) {
                val = l;
            }
            public EvalObject Call(EvalObject arg, Environment env) {
                var i = env.PushLocal(val.param, arg);
                var res = Evaluator.Instance.Eval(val.body);
                env.PopLocal(i);
                return res;
            }
            public override string ToString() => val.ToString();
        }
        public class Lambda : EvalObject 
        {
            readonly AstNode.Lambda val;


            public Lambda(AstNode.Lambda l) {
                val = l;
            }
            public override bool BoolValue => true;
            public EvalObject Call(EvalObject arg, Environment env) {
                var i = env.PushLocal(val.param, arg);
                var res = Evaluator.Instance.Eval(val.body);
                env.PopLocal(i);
                return res;
            }
            public override string ToString() => val.ToString();
        }

        public class Int : EvalObject {
            public override bool BoolValue => !val.IsZero;
            public readonly BigInt val;
            public Int(BigInt val) { this.val = val; }
            public override string ToString() => val.ToString();
        }
    }*/
}
