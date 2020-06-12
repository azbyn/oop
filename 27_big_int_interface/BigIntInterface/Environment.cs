using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BigIntInterface
{
    using EvalObject = AstNode.EvalObject;
    public class Environment
    {
        static BigInt Int(EvalObject o) => Evaluator.EnforceInt(0, o);
        static EvalObject I(BigInt v) => new AstNode.Int(0, v);

        static EvalObject B(Func<EvalObject, EvalObject> f, string name)
            => new AstNode.Builtin(f, name);
        static EvalObject B(Func<BigInt, BigInt> f, string name)
            => B(x => I(f(Int(x))), name);
        static EvalObject B(Func<BigInt,EvalObject> f, string name)
            => B(x => f(Int(x)), name);

        static EvalObject B(Func<BigInt, BigInt, BigInt> f, string name)
            => B((EvalObject a) => 
                    B((EvalObject b) =>
                        I(f(Int(a), Int(b))), $"{name} ${a}"), name);


        public CharPrinter Printer { get; set; } = c => Console.Write(c);
        static EvalObject FromString(string s)
            => Evaluator.Instance.Eval(Parser.Instance.Parse(Tokenizer.Instance.Tokenize(s)));

        Dictionary<string, EvalObject> values = new Dictionary<string, EvalObject>() {
            { "sgn",  B(BigInt.Sgn, "sgn") },
            { "pow",  B(BigInt.Pow, "pow") },
            { "sqrt", B(BigInt.Sqrt, "sqrt") },
            { "rand", B(BigInt.Rand, "rand") },
            { "ans",  I(0) },
            { "getc", B((EvalObject _) =>
                        I((int)Console.ReadKey().KeyChar), "getc") },
        };

        public Environment() {
            values["putc"] =
                B(i => { 
                    Printer((char)(int)i);
                    return B((EvalObject x) =>x, "id"); 
                }, "putc");
        }

        //returns null if it doesn't find the variable
        public EvalObject TryFind(string name)
        {
            if (!values.ContainsKey(name))
                return null;
            return values[name];
        }
        public EvalObject Assign(string name, EvalObject val) 
            => values[name] = val;
    }
}
