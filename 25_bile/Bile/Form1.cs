using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Numerics;

namespace Bile
{
    public partial class Form1 : Form
    {
        readonly Physics physics;
        readonly BackgroundWorker updater;
        enum Mode {
            None,
            Delete,
            Impulse,
            Add
        };

        Vector2 ballPos => physics.Balls[selectedIndex].Pos;
        Vector2 Δimpulse => mousePos-ballPos;

        Vector2 mousePos = new Vector2();
        int selectedIndex = -1;
        Random rnd = new Random();
        Vector2 startMouse = new Vector2(-1, -1);

        Mode mode = Mode.Impulse;

        public Form1()
        {
            InitializeComponent();
            physics = new Physics(img.Size);

            // physics.AddBall(
            //     new Ball(new Vector2(physics.Width/2, img.Height/2),
            //             .5f, Color.Red)
                // );
            // physics.AddBall(
            //     new Ball(new Vector2(physics.Width*.75f, physics.Height/2),
            //             .2f, Color.Red)
            //     );
            physics.AddBall(
            	new Ball(new Vector2(physics.Width/4, physics.Height/2),
            			.5f, Color.Blue)
            	);
            //physics.Balls[0].AddAcceleration(new Vector2(100, 20));
            // physics.Balls[0].Velocity = new Vector2(-2f, 0);
            // physics.Balls[1].Velocity = new Vector2(2f, .1f);
            // physics.Balls[1].Velocity = new Vector2(500, 20);

            updater = new BackgroundWorker();
            updater.WorkerReportsProgress = true;
            updater.DoWork += (o, e) => {
                var sw = new SpinWait();
                for (;;) {
                    physics.Update();
                    // Thread.Sleep(50);
                    sw.SpinOnce();
                    updater.ReportProgress(0);
                }
            };
            updater.ProgressChanged += (o, e) => img.Refresh();
            updater.RunWorkerAsync();
        }

        private void img_Click(object sender, EventArgs e)
        {

        }

        readonly SolidBrush sb = new SolidBrush(Color.White);
        readonly Pen p = new Pen(Color.Black, 5);
        private void img_Paint(object sender, PaintEventArgs e)
        {
            foreach (var b in physics.Balls)
            {
                sb.Color = b.Color;
                var pos = physics.PhysicsToScreen(b.Pos).ToPoint();
                // Console.WriteLine($"paint {b.Pos} {pos}");
                e.Graphics.FillEllipse(sb, physics.PhysicsToScreen(b.Rect));
                // p.Color = Color.Green;
                // e.Graphics.DrawLine(p, pos, physics.PhysicsToScreen(b.Pos+ b.Velocity).ToPoint());
                // p.Color = Color.Black;
                // e.Graphics.DrawLine(p, pos, physics.PhysicsToScreen(b.Pos+ b.FullAcceleration*10).ToPoint());
            }

            p.Color = Color.Brown;
            if (mode == Mode.Impulse && selectedIndex >= 0) {
                e.Graphics.DrawLine(p, physics.PhysicsToScreen(ballPos).ToPoint(), 
                    physics.PhysicsToScreen(mousePos).ToPoint());
            }
            p.Color = Color.Black;
            e.Graphics.DrawRectangle(p, img.DisplayRectangle);
        }

        private void add_Click(object sender, EventArgs e)
        {
            mode = Mode.Add;
            add.Enabled = false;
            impulse.Enabled = true;
            del.Enabled = true;
        }

        private void impulse_Click(object sender, EventArgs e)
        {
            mode = Mode.Impulse;
            add.Enabled = true;
            impulse.Enabled = false;
            del.Enabled = true;
        }

        private void del_Click(object sender, EventArgs e)
        {
            mode = Mode.Delete;
            add.Enabled = true;
            impulse.Enabled = true;
            del.Enabled = false;
        }

      
        private void img_MouseUp(object sender, MouseEventArgs e)
        {
            if (selectedIndex >= 0)
            {
                var F = -Δimpulse*3;
                physics.Balls[selectedIndex].AddImpulse(F);
            }
            selectedIndex = -1;
        }

        private void img_MouseMove(object sender, MouseEventArgs e)
        {

            if (mode == Mode.Impulse && selectedIndex >= 0)
                mousePos = physics.ScreenToPhysics(new Vector2(e.X, e.Y));
        }
        private void img_MouseDown(object sender, MouseEventArgs e)
        {
            float Lerp(float t, float a, float b) => (1 - t) * a + t * b;

            var p = physics.ScreenToPhysics(new Vector2(e.X, e.Y));
            var i = FindBallIndex(p);
            switch (mode) {
            case Mode.Delete:
                if (i >= 0) physics.Balls.RemoveAt(i);
                break;
            case Mode.Add:
                if (i >= 0) return;
                var r = Lerp(((float) size.Value)/ size.Maximum, 0.1f, 0.5f);
                Color randomColor = Color.FromArgb(rnd.Next(256), rnd.Next(256), rnd.Next(256));
                physics.AddBall(new Ball(p, r, randomColor));
                break;
            case Mode.Impulse:
                if (i < 0) return;
                startMouse = p;
                selectedIndex = i;
                break;
            }
        }

        //may return null
        Ball FindBall(Vector2 pos) {
            var i = FindBallIndex(pos);
            return i < 0 ? null : physics.Balls[i];
        }
        int FindBallIndex(Vector2 pos) 
        {
            int i = 0;
            foreach (var b in physics.Balls)
            {
                Console.WriteLine();
                if (b.Contains(pos)) return i;
                ++i;
            }
            return -1;
        }
    }
}
