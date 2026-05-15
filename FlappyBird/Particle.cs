using System;
using System.Drawing;

namespace FlappyBird
{
    public class Particle
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float VX { get; set; }
        public float VY { get; set; }
        public float Size { get; set; }
        public float Alpha { get; set; }
        public Color Color { get; set; }
        public float Life { get; set; }
        public float MaxLife { get; set; }

        public Particle(float x, float y, Random random)
        {
            X = x;
            Y = y;

            float angle = (float)(random.NextDouble() * Math.PI * 2);
            float speed = (float)(random.NextDouble() * 3) + 1;
            VX = (float)Math.Cos(angle) * speed;
            VY = (float)Math.Sin(angle) * speed - 2;

            Size = (float)(random.NextDouble() * 4) + 2;
            Alpha = 255;
            Life = 0;
            MaxLife = (float)(random.NextDouble() * 30) + 30;

            Color[] colors = new Color[]
            {
                Color.FromArgb(255, 255, 200, 0),
                Color.FromArgb(255, 255, 150, 0),
                Color.FromArgb(255, 255, 255, 100),
                Color.FromArgb(255, 255, 180, 50)
            };
            Color = colors[random.Next(colors.Length)];
        }

        public void Update()
        {
            X += VX;
            Y += VY;
            VY += 0.1f;
            Life++;

            Alpha = 255 * (1 - Life / MaxLife);
            if (Alpha < 0) Alpha = 0;

            Size *= 0.98f;
        }

        public void Draw(Graphics g)
        {
            if (Alpha <= 0) return;

            using (SolidBrush brush = new SolidBrush(Color.FromArgb((int)Alpha, Color)))
            {
                g.FillEllipse(brush, X - Size / 2, Y - Size / 2, Size, Size);
            }
        }

        public bool IsDead()
        {
            return Life >= MaxLife;
        }
    }
}
