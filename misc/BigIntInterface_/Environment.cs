using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    public class Environment
    {
        int idCounter = 0;
        struct Local {
            public readonly int id;
            public readonly string name;
            public EvalObject val;

            public Local(int id, string name, EvalObject val) {
                this.id = id;
                this.name = name;
                this.val = val;
            }
        }
        List<Local> locals;
        Dictionary<string, EvalObject> values;

        //returns null if it doesn't find the variable
        public EvalObject TryFind(string name)
        {
            for (int i = locals.Count - 1; i >= 0; --i)
            {
                var l = locals[i];
                if (l.name == name) return l.val;
            }
            if (!values.ContainsKey(name))
                return null;
            return values[name];
        }
        public EvalObject Assign(string name, EvalObject val) {
            for (int i = locals.Count - 1; i >= 0; --i)
            {
                var l = locals[i];
                if (l.name == name) return l.val = val;
            }
            return values[name] = val;
        }
        public int PushLocal(string name, EvalObject val)
        {
            var id = idCounter++;
            locals.Add(new Local(id, name, val));
            return id;
        }
        public void PopLocal(int id)
        {
            locals.RemoveAll(l => l.id == id);
        }
    }
}
