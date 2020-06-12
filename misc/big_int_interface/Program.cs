using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace big_int_interface
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());
            var tnz = new Tokenizer();
            // var str = @"\f.\a. !sqrt(213123+a/23)^2/2 ^^ 2 f * 2 j - a (-2) << 2 = 1";
            var str = @"";
            // var str = @"sqrt ^^ 2";
            // var str = @"\a. sqrt(213123+a/23)^2";
            // var str = @"\ a . sqrt (1+1+2)* a 23 ";
            var a = tnz.Tokenize(str);
            Console.WriteLine($"oi: '{str}");

            var par = new Parser();
            var res = par.Parse(a);
            Console.WriteLine($"res: {res}");
            if (!res)
                Console.WriteLine($"err: {par.Error}");
            else
                Console.WriteLine($"ast:\n{par.Result}");
        }
    }
}
