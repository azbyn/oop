using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    using Pos = Int32;
    public abstract class EvalObject {
        public abstract bool BoolValue { get; }

        public class Lambda : EvalObject {
            public readonly string param;
            public readonly AstNode body;
            public override bool BoolValue { get { return true; } }
            public Lambda(AstNode.Lambda l) {
                param = l.param;
                body = l.body;
            }
            public EvalObject Call(EvalObject arg, Environment env) {
                var i = env.PushLocal(param, arg);
                var res = Evaluator.Instance.Eval(body);
                env.PopLocal(i);
                return res;
            }
        }
        public class Builtin : EvalObject {
            readonly Func<EvalObject, EvalObject> val;
            public override bool BoolValue { get { return true; } }

            public Builtin(Func<EvalObject, EvalObject> val) { this.val = val; }
            public EvalObject Call(EvalObject arg) { return val(arg); }
        }
        public class Int : EvalObject {
            public override bool BoolValue { get { return !val.IsZero; } }
            public readonly BigInt val;
            public Int(BigInt val) { this.val = val; }
        }
    }
}
