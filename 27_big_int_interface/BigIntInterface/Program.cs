using System;
using System.Windows.Forms;

namespace BigIntInterface
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            var path = args.Length > 0 ? args[0] : "Autorun.txt";
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1(path));
        }
    }
}
