using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HangmanInterface
{
    public partial class Form1 : Form, IGraphicsDrawer
    {
        readonly HangmanGame game;

        public Form1(string path)
        {
            InitializeComponent();
            ReadFile(path, out int numTries, out string[] words);
            game = new HangmanGame(new FileWordGenerator(words), this, numTries);
        }

        void ReadFile(string path,
                      out int numTries,
                      out string[] words)
        {
            var lines = File.ReadLines(path)
                .Where(x => !string.IsNullOrWhiteSpace(x))
                .Select(s => s.Trim());
            var first = lines.FirstOrDefault();
            var m = Regex.Match(first, @"^\s*MaxTries\s*:\s*([0-9])*\s*$");

            words = lines.Skip(1).ToArray();
            if (!int.TryParse(m.Groups[1].Value, out numTries)) {
                Console.WriteLine($"invalid int: '{m.Groups[1].Value}'");
                numTries = 8;
            }
        }

        private void Form1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (char.IsLetter(e.KeyChar))
                game.CheckLetter(e.KeyChar);
        }

        void IGraphicsDrawer.ResetScreen()
        {
            //we don't have to do this, but whatever
            wrong.Text = "";
            display.Text = "";
        }

        int hangmanState = 0;
        const int MaxHangmanState = 10;
        void IGraphicsDrawer.DrawHangedMan(int state, int numTries)
        {
            hangmanState = ((MaxHangmanState - 1) * state) / (numTries);
            //Console.WriteLine($"state: {state}/{numTries}");
            img.Refresh();
        }

        void IGraphicsDrawer.DrawDisplayWord(char[] display)
        {
            this.display.Text = "";
            foreach (var c in display)
                this.display.Text += $"{c} ";
        }

        void IGraphicsDrawer.DrawWrongLetters(List<char> letters)
        {
            this.wrong.Text = "";
            foreach (var c in letters)
                this.wrong.Text += $"{c} ";
        }

        bool IGraphicsDrawer.EndScreen(bool won)
        {
            string title = won ? "You won!" : "Game Over.";
            var r = MessageBox.Show(title+"\nPlay Again?",
                                    title,
                                    MessageBoxButtons.YesNo,
                                    MessageBoxIcon.Question);
            var res = r == DialogResult.Yes;
            if (!res) 
                Application.Exit();
            return res;
        }

        readonly Pen p = new Pen(Color.Black, 5);
        void img_Paint(object sender, PaintEventArgs e)
        {
            var w = img.Size.Width;
            var h = img.Size.Height;
            var brush = new SolidBrush(Color.Red);
            var botLen = w*.55f;
            var padding = 10;

            //Console.WriteLine($"{w}x{h}");
            
            //e.Graphics.FillRectangle(brush, 0,0, w, h);

 
            var botY = h-padding;
            var botA = new PointF(padding, botY);
            var botB = new PointF(padding+botLen, botY);
            e.Graphics.DrawLine(p, botA, botB);

            var midX = padding+botLen/2;
            var botM = new PointF(midX, botY);

            var topM = new PointF(midX, padding);
            e.Graphics.DrawLine(p, botM, topM);

            var topLen = w*.45f;
            var topB = new PointF(midX+topLen, padding);
            var nooseLen = 30;
            var headT = new PointF(topB.X, padding+nooseLen);

            e.Graphics.DrawLine(p, topM, topB);
            e.Graphics.DrawLine(p, topB, headT);

            if (hangmanState <= 0) 
                return;
            //head
            var headSize = 30;
            var headLT = new PointF(topB.X-headSize, padding+nooseLen);
            e.Graphics.DrawEllipse(p, headLT.X, headLT.Y, headSize*2, headSize*2);
            var bodySize = h*.3f;
            var bodyStartY = headLT.Y+2*nooseLen;
            var bodyEndY = bodyStartY + bodySize;


            var eyeLength = headSize * .3f;
            var eyeHeight = headSize * .6f;

            if (hangmanState <= 1)
                return;
            //eyes
            var eyeSize = 5;
            e.Graphics.DrawEllipse(p, topB.X + eyeLength, headT.Y+eyeHeight, eyeSize,eyeSize);
            if (hangmanState <= 2) 
                return;
            e.Graphics.DrawEllipse(p, topB.X - eyeLength-eyeSize, headT.Y+eyeHeight, eyeSize,eyeSize);

            if (hangmanState <= 3) 
                return;
            //mouth
            var mouthWidth = headSize*.5f;
            var mouthY = headT.Y + headSize*1.35f;
            var p1 = new PointF(topB.X - mouthWidth, mouthY);
            var p2 = new PointF(topB.X + mouthWidth, mouthY);
            var c  = new PointF(topB.X, mouthY + 7);//+5
            e.Graphics.DrawBezier(p, p1,c, c, p2);


            // body
            if (hangmanState <= 4) 
                return;

            var legHeight = 60;
            var legWidth = 40;

            e.Graphics.DrawLine(p, topB.X, bodyStartY, topB.X, bodyEndY);


            // body
            if (hangmanState <= 5) 
                return;
            var armWidth = 40;
            var armY = bodyStartY+30;
            e.Graphics.DrawLine(p, topB.X-armWidth, armY, topB.X, armY);
            if (hangmanState <= 6)
                return;
            e.Graphics.DrawLine(p, topB.X+armWidth, armY, topB.X, armY);

            //legs
            if (hangmanState <= 7) 
                return;
            e.Graphics.DrawLine(p, topB.X-legWidth, bodyEndY+legHeight, topB.X, bodyEndY);
            if (hangmanState <= 8) 
                return;
            e.Graphics.DrawLine(p, topB.X+legWidth, bodyEndY+legHeight, topB.X, bodyEndY);


            // e.Graphics.DrawLine(p, padding, h - padding,
            //                     padding+botLen, h -padding);
        }
    }
}
