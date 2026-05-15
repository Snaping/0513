using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace FlappyBird
{
    public class Pipe
    {
        public float X { get; private set; }
        public int TopHeight { get; private set; }
        public int BottomY { get; private set; }
        public int Width => 70;
        public int Gap => 160;
        public bool Passed { get; set; }

        private const int BorderWidth = 6;

        public Pipe(float x, int gameHeight, Random random)
        {
            X = x;
            int minGapTop = 80;
            int maxGapTop = gameHeight - Gap - 80 - 100;
            int gapTop = random.Next(minGapTop, maxGapTop);

            TopHeight = gapTop;
            BottomY = gapTop + Gap;
            Passed = false;
        }

        public void Update(float speed)
        {
            X -= speed;
        }

        public void Draw(Graphics g, int gameHeight)
        {
            DrawTopPipe(g);
            DrawBottomPipe(g, gameHeight);
        }

        private void DrawTopPipe(Graphics g)
        {
            int pipeX = (int)X;
            int pipeHeight = TopHeight;

            using (LinearGradientBrush bodyBrush = new LinearGradientBrush(
                new Rectangle(pipeX + BorderWidth, 0, Width - BorderWidth * 2, pipeHeight),
                Color.FromArgb(255, 80, 180, 80),
                Color.FromArgb(255, 60, 140, 60),
                LinearGradientMode.Horizontal))
            {
                g.FillRectangle(bodyBrush, pipeX + BorderWidth, 0, Width - BorderWidth * 2, pipeHeight);
            }

            int capWidth = Width + 10;
            int capHeight = 30;
            int capX = pipeX - 5;
            int capY = pipeHeight - capHeight;

            using (LinearGradientBrush capBrush = new LinearGradientBrush(
                new Rectangle(capX, capY, capWidth, capHeight),
                Color.FromArgb(255, 100, 200, 100),
                Color.FromArgb(255, 50, 130, 50),
                LinearGradientMode.Vertical))
            {
                g.FillRectangle(capBrush, capX, capY, capWidth, capHeight);
            }

            using (Pen borderPen = new Pen(Color.FromArgb(255, 40, 100, 40), BorderWidth))
            {
                g.DrawRectangle(borderPen, pipeX + BorderWidth / 2, 0, Width - BorderWidth, pipeHeight);
                g.DrawRectangle(borderPen, capX + BorderWidth / 2, capY + BorderWidth / 2,
                    capWidth - BorderWidth, capHeight - BorderWidth);
            }
        }

        private void DrawBottomPipe(Graphics g, int gameHeight)
        {
            int pipeX = (int)X;
            int pipeY = BottomY;
            int pipeHeight = gameHeight - BottomY - 100;

            using (LinearGradientBrush bodyBrush = new LinearGradientBrush(
                new Rectangle(pipeX + BorderWidth, pipeY, Width - BorderWidth * 2, pipeHeight),
                Color.FromArgb(255, 80, 180, 80),
                Color.FromArgb(255, 60, 140, 60),
                LinearGradientMode.Horizontal))
            {
                g.FillRectangle(bodyBrush, pipeX + BorderWidth, pipeY, Width - BorderWidth * 2, pipeHeight);
            }

            int capWidth = Width + 10;
            int capHeight = 30;
            int capX = pipeX - 5;
            int capY = pipeY;

            using (LinearGradientBrush capBrush = new LinearGradientBrush(
                new Rectangle(capX, capY, capWidth, capHeight),
                Color.FromArgb(255, 100, 200, 100),
                Color.FromArgb(255, 50, 130, 50),
                LinearGradientMode.Vertical))
            {
                g.FillRectangle(capBrush, capX, capY, capWidth, capHeight);
            }

            using (Pen borderPen = new Pen(Color.FromArgb(255, 40, 100, 40), BorderWidth))
            {
                g.DrawRectangle(borderPen, pipeX + BorderWidth / 2, pipeY, Width - BorderWidth, pipeHeight);
                g.DrawRectangle(borderPen, capX + BorderWidth / 2, capY + BorderWidth / 2,
                    capWidth - BorderWidth, capHeight - BorderWidth);
            }
        }

        public bool Intersects(RectangleF birdRect)
        {
            RectangleF topRect = new RectangleF(X, 0, Width, TopHeight - 25);
            RectangleF bottomRect = new RectangleF(X, BottomY + 25, Width, 10000);

            return birdRect.IntersectsWith(topRect) || birdRect.IntersectsWith(bottomRect);
        }

        public bool IsOffScreen()
        {
            return X + Width < -50;
        }
    }
}
