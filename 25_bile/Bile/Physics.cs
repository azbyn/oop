using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Numerics;
using System.Linq;

namespace Bile
{
    public static class VectorExtensions {
        public static PointF ToPoint(this Vector2 v) => new PointF(v.X,v.Y);
        public static Vector2 FlippedX(this Vector2 v) => new Vector2(-v.X, v.Y);
        public static Vector2 FlippedY(this Vector2 v) => new Vector2(v.X, -v.Y);

        public static IEnumerable<(int index, T item)> WithIndex<T>(this IEnumerable<T> self)       
            => self.Select((item, index) => (index, item)); 
    }
    public class Physics
    {
        SizeF size;
        List<Ball> balls;

        public List<Ball> Balls => balls;
        Stopwatch stopWatch = new Stopwatch();

        public float Width => size.Width;
        public float Height => size.Height;

        float ratio = 100;

        public T ScreenToPhysics<T>(T a) => (dynamic) a/ratio;
        public T PhysicsToScreen<T>(T a) => (dynamic) a*ratio;

        public RectangleF PhysicsToScreen(RectangleF a) => 
            new RectangleF(a.X*ratio, a.Y * ratio, a.Width*ratio, a.Height*ratio);

        public RectangleF ScreenToPhysics(RectangleF a) => 
            new RectangleF(a.X/ratio, a.Y/ratio, a.Width/ratio, a.Height/ratio);

        public SizeF ScreenToPhysics(SizeF a) => new SizeF(a.Width/ratio, a.Height/ratio);
        public Physics(SizeF size)
        {
            this.size = ScreenToPhysics(size);
            balls = new List<Ball>();
            stopWatch.Start();
        }

        public void AddBall(Ball b)
        {
            balls.Add(b);
        }

        public void Update()
        {
            var dt = stopWatch.Elapsed.TotalSeconds;
            stopWatch.Restart();
            UpdateImpl((float)dt);
        }
        int i = 0;
        void DoCollision(Ball a, Ball b)
        {
            if (!a.Overlaps(b)) return;
            //Console.WriteLine("REEEEEEEEEEE");
            // if (i++ >= 3) return;
            //https://en.wikipedia.org/wiki/Elastic_collision
            const float ε = 1e-7f;

            // Δv_A = 
            Vector2 posDiff = a.Pos - b.Pos;
            float dist = posDiff.Length();
            // avoid division by zero
            var dir = dist < ε ? new Vector2(-1, 0) : (posDiff/dist);
            var vDiff = a.Velocity - b.Velocity;
            var ma = a.Mass;
            var mb = b.Mass;
            // Console.WriteLine($"dt = {a.Radius} - {dist} =  {(a.Radius - dist)}");
            // Console.WriteLine($"ap = {a.Pos}");
            if (vDiff.Length() < ε && posDiff.Length() < (a.Radius + b.Radius) * .95 ) {
                var fact = 1.01f;
                a.Pos += dir * (a.Radius - dist)*fact;
                b.Pos -= dir * (b.Radius - dist)*fact;
            }
            // b.Pos += dir * (b.Radius - dist);
            // Console.WriteLine($"ap = {a.Pos}");

            float ΔvA_size = -Vector2.Dot(vDiff, posDiff) *
                            (2*mb)/(ma+mb);
            
            float ΔvB_size = Vector2.Dot(vDiff, posDiff) *
                            (2*ma)/(ma+mb);
            // Console.WriteLine($"Δav = {ΔvA_size} bv = {ΔvB_size}");
            // Console.WriteLine($"-av = {a.Velocity} bv = {b.Velocity}");
            var f = 1f;// 2f;
            a.Velocity += dir * ΔvA_size *f;
            b.Velocity += dir * ΔvB_size*f;
            // Console.WriteLine($"+av = {a.Velocity} bv = {b.Velocity}");
        }

        void UpdateImpl(float dt)
        {
            // const float ε = 1e-7f;
            // dt *= .1f;
            for (int i = 0; i < balls.Count; ++i)
                for (int j = i+1; j < balls.Count; ++j᠎)
                    DoCollision(balls[i], balls[j]);

            foreach (var (i, b) in balls.WithIndex())
            {
                // Console.WriteLine($"upd {dt} {b.Pos}");
                b.Pos += b.Velocity * dt;
                b.Velocity += b.FullAcceleration * dt;

                // if (b.Velocity.LengthSquared() < ε*ε) b.Velocity = new Vector2();
                // if (b.Acceleration.LengthSquared() < ε*ε) b.Acceleration = new Vector2();
                //Console.WriteLine($"upd {b.Radius} m={b.Mass} a={b.Acceleration.Length()} v={b.Velocity.Length()}");

                var rect = b.Rect;
                var r = b.Radius;
                var impactFactor = .8f;
                void FlipX() {
                    b.Velocity = b.Velocity.FlippedX() *impactFactor;
                    b.Acceleration = b.Acceleration.FlippedX()*impactFactor;
                }
                void FlipY() {
                    b.Velocity = b.Velocity.FlippedY() * impactFactor;
                    b.Acceleration = b.Acceleration.FlippedY()*impactFactor;
                }

                if (rect.Right >= size.Width) {
                    b.Pos = new Vector2(size.Width - r, b.Pos.Y);
                    FlipX();
                }
                else if (rect.Left < 0) {
                    b.Pos = new Vector2(r, b.Pos.Y);
                    FlipX();
                }
                if (rect.Bottom >= size.Height) {
                    b.Pos = new Vector2(b.Pos.X, size.Height - r);
                    FlipY();
                }
                if (rect.Top < 0) {
                    b.Pos = new Vector2(b.Pos.X, r);
                    FlipY();
                }
            }
        }
    }
}