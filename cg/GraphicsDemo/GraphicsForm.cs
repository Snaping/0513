
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace GraphicsDemo
{
    public partial class GraphicsForm : Form
    {
        private PictureBox canvas;
        private Bitmap bitmap;
        private Button clearButton;
        private Label statusLabel;
        private Color currentColor = Color.Black;

        public GraphicsForm()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.Text = "图形渲染窗口";
            this.Size = new System.Drawing.Size(600, 500);
            this.StartPosition = FormStartPosition.CenterScreen;

            canvas = new PictureBox();
            canvas.Location = new System.Drawing.Point(10, 40);
            canvas.Size = new System.Drawing.Size(560, 400);
            canvas.BorderStyle = BorderStyle.FixedSingle;
            canvas.BackColor = Color.White;
            this.Controls.Add(canvas);

            bitmap = new Bitmap(canvas.Width, canvas.Height);
            canvas.Image = bitmap;

            clearButton = new Button();
            clearButton.Text = "清空画布";
            clearButton.Location = new System.Drawing.Point(10, 10);
            clearButton.Size = new System.Drawing.Size(100, 20);
            clearButton.Click += ClearButton_Click;
            this.Controls.Add(clearButton);

            statusLabel = new Label();
            statusLabel.Text = "就绪";
            statusLabel.Location = new System.Drawing.Point(120, 10);
            statusLabel.Size = new System.Drawing.Size(200, 20);
            this.Controls.Add(statusLabel);

            Button colorButton = new Button();
            colorButton.Text = "选择颜色";
            colorButton.Location = new System.Drawing.Point(330, 10);
            colorButton.Size = new System.Drawing.Size(100, 20);
            colorButton.Click += ColorButton_Click;
            this.Controls.Add(colorButton);
        }

        private void ClearButton_Click(object sender, EventArgs e)
        {
            using (Graphics g = Graphics.FromImage(bitmap))
            {
                g.Clear(Color.White);
            }
            canvas.Refresh();
            statusLabel.Text = "画布已清空";
        }

        private void ColorButton_Click(object sender, EventArgs e)
        {
            using (ColorDialog cd = new ColorDialog())
            {
                if (cd.ShowDialog() == DialogResult.OK)
                {
                    currentColor = cd.Color;
                    statusLabel.Text = $"当前颜色: {currentColor.Name}";
                }
            }
        }

        public void ExecuteCode(string code)
        {
            try
            {
                ClearCanvas();
                
                CodeParser parser = new CodeParser(code, this);
                parser.ParseAndExecute();
                
                canvas.Refresh();
                statusLabel.Text = "代码执行成功";
            }
            catch (Exception ex)
            {
                statusLabel.Text = $"错误: {ex.Message}";
                MessageBox.Show($"代码执行失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public void ClearCanvas()
        {
            using (Graphics g = Graphics.FromImage(bitmap))
            {
                g.Clear(Color.White);
            }
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
    }
}
