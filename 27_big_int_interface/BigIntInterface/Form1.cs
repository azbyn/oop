using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BigIntInterface
{
    public partial class Form1 : Form
    {
        void AppendError(string msg)
        {
            output.SelectionStart = output.TextLength;
            output.SelectionLength = 0;

            output.SelectionColor = Color.Red;
            AppendText(msg);
            output.SelectionColor = output.ForeColor;
        }
        void AppendText(string msg)
        {
            output.AppendText(msg+"\n");
            output.SelectionStart = output.TextLength;
            output.ScrollToCaret();
        }
        string last = "";
        void Eval(string str)
        {
            last = str;
            string res;
            output.AppendText($">>> {str}\n");
            bool success = Evaluator.Instance.ReplEval(str, out res, out _);
            if (!success)
                AppendError(res);
            else
                AppendText(res);
        }
        void DoAutoRun(string path)
        {
            try
            {
                string content = "";
                var lines = File.ReadLines(path);
                foreach (string line in lines)
                {
                    if (line == "") continue;
                    if (line.TrimStart()[0] == '#') continue;
                    if (!".(".Contains(line.TrimEnd().Last()))
                    {
                        // Console.WriteLine($"eval = {line}");
                        Eval(content+line);
                        content = "";
                    }
                    else
                        content += line;
                }
                Eval(content);
            }
            catch (Exception e)
            {
                AppendError($"Error {e.Message}");
                // Console.WriteLine($"Error {e}");
            }
            // catch (Exception) { return; }
        }
        public Form1(string path)
        {
            InitializeComponent();
            Evaluator.Instance.Printer = c => {
                Console.Write(c);
                output.Text += c;
            };
            DoAutoRun(path);
        }

        private void input_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                Eval(input.Text);
                input.Text = "";
            }
            else if (e.KeyCode == Keys.Up) {
                if (input.Text == "")
                    input.Text = last;
            }
            else if (e.KeyCode == Keys.Down) {
                if (input.Text != "")
                {
                    last = input.Text;
                    input.Text = "";
                }
            }
        }
    }
}
