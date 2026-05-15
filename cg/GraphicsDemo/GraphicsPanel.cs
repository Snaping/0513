
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace GraphicsDemo
{
    public class GraphicsPanel : Panel
    {
        private PictureBox canvas;
        private Bitmap bitmap;
        private Color currentColor = Color.Black;

        public GraphicsPanel()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.Size = new Size(560, 400);
            
            canvas = new PictureBox();
            canvas.Location = new Point(0, 0);
            canvas.Size = this.Size;
            canvas.BorderStyle = BorderStyle.FixedSingle;
            canvas.BackColor = Color.White;
            this.Controls.Add(canvas);

            bitmap = new Bitmap(canvas.Width, canvas.Height);
            canvas.Image = bitmap;
        }

        public void ClearCanvas()
        {
            using (Graphics g = Graphics.FromImage(bitmap))
            {
                g.Clear(Color.White);
            }
            canvas.Refresh();
        }

        public void RenderPoint(int x, int y, Color color)
        {
            if (x >= 0 && x < bitmap.Width && y >= 0 && y < bitmap.Height)
            {
                bitmap.SetPixel(x, y, color);
            }
        }

        public void RenderPoints(List<Point> points, Color color)
        {
            foreach (Point p in points)
            {
                RenderPoint(p.X, p.Y, color);
            }
            canvas.Refresh();
        }

        public void RenderLine(int x1, int y1, int x2, int y2, Color color)
        {
            List<Point> points = GraphicsAlgorithms.BresenhamLine(x1, y1, x2, y2);
            RenderPoints(points, color);
        }

        public Color CurrentColor
        {
            get { return currentColor; }
            set { currentColor = value; }
        }

        public Bitmap CanvasBitmap
        {
            get { return bitmap; }
        }

        public PictureBox Canvas
        {
            get { return canvas; }
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);
            if (canvas != null)
            {
                canvas.Size = this.Size;
                bitmap = new Bitmap(canvas.Width, canvas.Height);
                canvas.Image = bitmap;
            }
        }
    }
}
