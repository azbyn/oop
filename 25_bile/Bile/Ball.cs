using System;
using System.Drawing;
using System.Numerics;

namespace Bile
{
    public class Ball
    {
        Vector2 pos;
        Vector2 v = new Vector2();
        Vector2 a = new Vector2();
        float r = 1;
        float ρ = 4;
        float drag = 1.2f;

        Color color;
        public Color Color { get => color; set => color = value; }

        public Vector2 Pos { get => pos; set => pos = value; }
        public Vector2 Velocity { get => v; set => v = value; }
        public Vector2 Acceleration { get => a; set => a = value; }

        // public Vector2 Pos => pos;
        // public Vector2 Velocity => v;
        // public Vector2 Acceleration => a;

        public float Density { get => ρ; set => ρ = value; }
        public float Radius { get => r; set => r = value; }
        public float Drag { get => drag; set => drag = value; }

        const float π = (float) Math.PI;

        public float Mass { get => ρ*r*r*π; set => ρ = (value / (r*r*π)); }
        float m { get => Mass; set => Mass = value; }

        public RectangleF Rect => new RectangleF(pos.X - r, pos.Y - r, 2*r, 2*r);


        public Vector2 FullAcceleration => a + GetDrag(1)/m;
        //air resistance
        Vector2 GetDrag(float Δt)
        {
            const float ρ_air = 1.225f;
            //cross sectional area
            var A = Radius;
            var C = drag;

            // F_drag = 1/2 ρ v² C A
            var F = .5f * ρ_air * -v * v.Length() * C * A * Δt;
            return F;
        }

        public Ball(Vector2 pos, float radius, Color color)
        {
            this.pos = pos;
            this.color = color;
            this.r = radius;
        }

        public void AddAcceleration(Vector2 val)
        {
            a += val;
        }

        public void AddImpulse(Vector2 val)
        {
            a += val/m;
        }

        public void ApplyForce(Vector2 F) {
            // F = ma 
            // so a = F/m
            a += F / m;
        }
        public bool Overlaps(Ball other) 
        {
            var r = (Radius + other.Radius);
            return (other.pos - pos).LengthSquared() < r*r;
        }
        public bool Contains(Vector2 p) => (p - pos).LengthSquared() < r * r;
    }
}