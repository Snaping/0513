using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlappyBird
{
    public class Bird
    {
        public float X { get; private set; }
        public float Y { get; private set; }
        public float Velocity { get; private set; }
        public float Rotation { get; private set; }
        public int Width => 40;
        public int Height => 30;

        private const float Gravity = 0.5f;
        private const float JumpForce = -8f;
        private const float MaxVelocity = 12f;
        private const float MaxRotation = 90f;
        private const float MinRotation = -25f;

        private float wingAngle;
        private float wingSpeed;

        public Bird(float startX, float startY)
        {
            X = startX;
            Y = startY;
            Velocity = 0;
            Rotation = 0;
            wingAngle = 0;
            wingSpeed = 0.3f;
        }

        public void Jump()
        {
            Velocity = JumpForce;
            Rotation = MinRotation;
        }

        public void Update()
        {
            Velocity += Gravity;
            if (Velocity > MaxVelocity)
                Velocity = MaxVelocity;

            Y += Velocity;

            if (Velocity > 0)
            {
                Rotation += 4f;
                if (Rotation > MaxRotation)
                    Rotation = MaxRotation;
            }
            else
            {
                Rotation = MinRotation;
            }

            wingAngle += wingSpeed;
        }

        public void Draw(Graphics g)
        {
            g.TranslateTransform(X + Width / 2, Y + Height / 2);
            g.RotateTransform(Rotation);

            float bodyRadius = 15;
            float bodyX = -bodyRadius;
            float bodyY = -bodyRadius + 2;

            using (GraphicsPath bodyPath = new GraphicsPath())
            {
                bodyPath.AddEllipse(bodyX, bodyY, bodyRadius * 2, bodyRadius * 2);
                using (PathGradientBrush bodyBrush = new PathGradientBrush(bodyPath))
                {
                    bodyBrush.CenterColor = Color.FromArgb(255, 255, 150, 0);
                    bodyBrush.SurroundColors = new Color[] { Color.FromArgb(255, 255, 200, 0) };
                    g.FillPath(bodyBrush, bodyPath);
                }
            }

            float wingY = (float)Math.Sin(wingAngle) * 3;
            using (SolidBrush wingBrush = new SolidBrush(Color.FromArgb(255, 220, 120, 0)))
            {
                g.FillEllipse(wingBrush, -22, wingY, 14, 10);
            }

            float eyeX = 8;
            float eyeY = -5;
            using (SolidBrush eyeWhite = new SolidBrush(Color.White))
            {
                g.FillEllipse(eyeWhite, eyeX - 3, eyeY - 3, 8, 8);
            }
            using (SolidBrush eyeBlack = new SolidBrush(Color.Black))
            {
                g.FillEllipse(eyeBlack, eyeX, eyeY, 4, 4);
            }

            using (SolidBrush beakBrush = new SolidBrush(Color.FromArgb(255, 255, 100, 0)))
            {
                Point[] beak = new Point[]
                {
                    new Point(12, 0),
                    new Point(22, 2),
                    new Point(22, 6),
                    new Point(12, 8)
                };
                g.FillPolygon(beakBrush, beak);
            }

            g.ResetTransform();
        }

        public RectangleF GetBounds()
        {
            return new RectangleF(X + 5, Y + 5, Width - 10, Height - 10);
        }

        public void Reset(float startX, float startY)
        {
            X = startX;
            Y = startY;
            Velocity = 0;
            Rotation = 0;
        }
    }
}
