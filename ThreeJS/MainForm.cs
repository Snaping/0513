using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace ThreeDVisualizer
{
    public partial class MainForm : Form
    {
        private enum Mode { Draw2D, Extrude3D, Rotate3D }
        
        private Mode currentMode;
        private List<PointF> circlePoints;
        private float circleRadius;
        private PointF circleCenter;
        private bool isDrawing;
        private bool isDragging;
        private bool isExtruding;
        private Point lastMousePos;
        private Point extrudeStartPos;
        private float extrudeHeight;
        private Camera camera;
        private List<Vector3> cylinderVertices;
        private List<int[]> cylinderEdges;
        private Bitmap backBuffer;
        private Graphics backGraphics;

        public MainForm()
        {
            InitializeComponent();
            InitializeRenderer();
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            
            this.Text = "3D Visualizer - 圆柱体拉伸工具";
            this.Size = new Size(1200, 700);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer, true);
            
            Panel controlPanel = new Panel
            {
                Dock = DockStyle.Top,
                Height = 60,
                BackColor = Color.FromArgb(45, 45, 48)
            };

            Button btnDraw2D = CreateButton("绘制圆", Color.FromArgb(0, 120, 215));
            btnDraw2D.Click += (s, e) => { currentMode = Mode.Draw2D; UpdateButtonStates(); };
            
            Button btnExtrude = CreateButton("拉伸圆柱", Color.FromArgb(16, 185, 129));
            btnExtrude.Click += (s, e) => { 
                if (circlePoints.Count > 0) { 
                    currentMode = Mode.Extrude3D; 
                    extrudeHeight = 0;
                    isExtruding = false;
                    cylinderVertices.Clear();
                    cylinderEdges.Clear();
                    UpdateInfoLabel();
                    UpdateButtonStates(); 
                    Invalidate();
                } 
            };
            
            Button btnRotate = CreateButton("旋转视角", Color.FromArgb(245, 158, 11));
            btnRotate.Click += (s, e) => { currentMode = Mode.Rotate3D; UpdateButtonStates(); };

            Button btnClear = CreateButton("清空", Color.FromArgb(239, 68, 68));
            btnClear.Click += (s, e) => { ClearAll(); };

            Label lblInfo = new Label
            {
                Text = "高度: 0",
                ForeColor = Color.White,
                AutoSize = true,
                Location = new Point(420, 20),
                Font = new Font("Segoe UI", 10f)
            };
            lblInfo.Name = "infoLabel";

            controlPanel.Controls.AddRange(new Control[] { btnDraw2D, btnExtrude, btnRotate, btnClear, lblInfo });
            btnDraw2D.Location = new Point(20, 15);
            btnExtrude.Location = new Point(120, 15);
            btnRotate.Location = new Point(220, 15);
            btnClear.Location = new Point(320, 15);

            this.Controls.Add(controlPanel);
            this.ResumeLayout(false);

            this.MouseDown += MainForm_MouseDown;
            this.MouseMove += MainForm_MouseMove;
            this.MouseUp += MainForm_MouseUp;
            this.Resize += MainForm_Resize;
        }

        private Button CreateButton(string text, Color backColor)
        {
            return new Button
            {
                Text = text,
                Size = new Size(90, 30),
                BackColor = backColor,
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Segoe UI", 9f, FontStyle.Bold)
            };
        }

        private void InitializeRenderer()
        {
            currentMode = Mode.Draw2D;
            circlePoints = new List<PointF>();
            cylinderVertices = new List<Vector3>();
            cylinderEdges = new List<int[]>();
            circleCenter = new PointF(400, 400);
            circleRadius = 80;
            extrudeHeight = 0;
            camera = new Camera();
            isDrawing = false;
            isDragging = false;
            
            GenerateCirclePoints();
            CreateBackBuffer();
        }

        private void CreateBackBuffer()
        {
            if (backBuffer != null)
            {
                backGraphics.Dispose();
                backBuffer.Dispose();
            }
            backBuffer = new Bitmap(Math.Max(1, ClientSize.Width), Math.Max(1, ClientSize.Height));
            backGraphics = Graphics.FromImage(backBuffer);
            backGraphics.SmoothingMode = SmoothingMode.AntiAlias;
        }

        private void MainForm_Resize(object sender, EventArgs e)
        {
            CreateBackBuffer();
            Invalidate();
        }

        private void GenerateCirclePoints()
        {
            circlePoints.Clear();
            int segments = 32;
            for (int i = 0; i < segments; i++)
            {
                float angle = (float)(i * 2 * Math.PI / segments);
                float x = circleCenter.X + circleRadius * (float)Math.Cos(angle);
                float y = circleCenter.Y + circleRadius * (float)Math.Sin(angle);
                circlePoints.Add(new PointF(x, y));
            }
        }

        private void GenerateCylinder()
        {
            cylinderVertices.Clear();
            cylinderEdges.Clear();

            int segments = circlePoints.Count;
            float scale = 1.5f;

            for (int i = 0; i < segments; i++)
            {
                float angle = (float)(i * 2 * Math.PI / segments);
                float x = circleRadius * scale * (float)Math.Cos(angle);
                float z = circleRadius * scale * (float)Math.Sin(angle);
                
                cylinderVertices.Add(new Vector3(x, -extrudeHeight * scale / 2, z));
                cylinderVertices.Add(new Vector3(x, extrudeHeight * scale / 2, z));
            }

            for (int i = 0; i < segments; i++)
            {
                int next = (i + 1) % segments;
                cylinderEdges.Add(new int[] { i * 2, next * 2 });
                cylinderEdges.Add(new int[] { i * 2 + 1, next * 2 + 1 });
                cylinderEdges.Add(new int[] { i * 2, i * 2 + 1 });
            }
        }

        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (currentMode == Mode.Draw2D)
                {
                    isDrawing = true;
                    circleCenter = e.Location;
                }
                else if (currentMode == Mode.Extrude3D)
                {
                    isExtruding = true;
                    extrudeStartPos = e.Location;
                    lastMousePos = e.Location;
                    extrudeHeight = 10;
                    GenerateCylinder();
                    Invalidate();
                }
                else if (currentMode == Mode.Rotate3D)
                {
                    isDragging = true;
                    lastMousePos = e.Location;
                }
            }
        }

        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDrawing && currentMode == Mode.Draw2D)
            {
                float dx = e.X - circleCenter.X;
                float dy = e.Y - circleCenter.Y;
                circleRadius = (float)Math.Sqrt(dx * dx + dy * dy);
                if (circleRadius < 10) circleRadius = 10;
                GenerateCirclePoints();
                Invalidate();
            }
            else if (isExtruding && currentMode == Mode.Extrude3D)
            {
                float deltaY = e.Y - extrudeStartPos.Y;
                extrudeHeight = Math.Max(10, Math.Min(400, Math.Abs(deltaY) * 1.2f));
                UpdateInfoLabel();
                GenerateCylinder();
                lastMousePos = e.Location;
                Invalidate();
            }
            else if (isDragging && currentMode == Mode.Rotate3D)
            {
                float deltaX = e.X - lastMousePos.X;
                float deltaY = e.Y - lastMousePos.Y;
                camera.RotationY += deltaX * 0.01f;
                camera.RotationX += deltaY * 0.01f;
                camera.RotationX = Math.Max(-1.5f, Math.Min(1.5f, camera.RotationX));
                lastMousePos = e.Location;
                Invalidate();
            }
        }

        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            isDrawing = false;
            isDragging = false;
            if (isExtruding)
            {
                isExtruding = false;
            }
        }

        private void UpdateInfoLabel()
        {
            foreach (Control c in Controls[0].Controls)
            {
                if (c.Name == "infoLabel")
                {
                    c.Text = $"高度: {extrudeHeight:F0}";
                    break;
                }
            }
        }

        private void UpdateButtonStates()
        {
            Panel panel = (Panel)Controls[0];
            foreach (Control c in panel.Controls)
            {
                if (c is Button btn)
                {
                    btn.Enabled = true;
                }
            }
        }

        private void ClearAll()
        {
            circleRadius = 80;
            circleCenter = new PointF(400, 400);
            extrudeHeight = 0;
            GenerateCirclePoints();
            cylinderVertices.Clear();
            cylinderEdges.Clear();
            currentMode = Mode.Draw2D;
            camera = new Camera();
            UpdateInfoLabel();
            UpdateButtonStates();
            Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (backBuffer == null) return;

            backGraphics.Clear(Color.FromArgb(30, 30, 30));
            
            if (currentMode == Mode.Draw2D)
            {
                Draw2DCircle();
            }
            else
            {
                Draw3DCylinder();
            }

            DrawGrid();
            e.Graphics.DrawImage(backBuffer, 0, 0);
        }

        private void Draw2DCircle()
        {
            if (circlePoints.Count < 2) return;

            Pen pen = new Pen(Color.FromArgb(0, 120, 215), 3);
            Brush brush = new SolidBrush(Color.FromArgb(50, 0, 120, 215));

            PointF[] points = circlePoints.ToArray();
            backGraphics.FillPolygon(brush, points);
            backGraphics.DrawPolygon(pen, points);

            backGraphics.DrawString("2D 模式 - 拖动鼠标调整圆大小", 
                new Font("Segoe UI", 12f), Brushes.White, 20, 80);
            
            pen.Dispose();
            brush.Dispose();
        }

        private void Draw3DCylinder()
        {
            int width = ClientSize.Width;
            int height = ClientSize.Height;

            if (cylinderVertices.Count == 0)
            {
                backGraphics.DrawString("拉伸模式 - 按下鼠标并拖动开始拉伸", 
                    new Font("Segoe UI", 14f, FontStyle.Bold), Brushes.LightGreen, 20, 80);
                return;
            }

            List<PointF> projectedPoints = new List<PointF>();
            List<float> depths = new List<float>();
            foreach (var v in cylinderVertices)
            {
                Vector3 p = camera.Project(v, width, height);
                projectedPoints.Add(new PointF(p.X, p.Y));
                depths.Add(p.Z);
            }

            Pen edgePen = new Pen(Color.FromArgb(0, 120, 215), 2);
            Brush topBrush = new SolidBrush(Color.FromArgb(180, 16, 185, 129));
            Brush bottomBrush = new SolidBrush(Color.FromArgb(180, 245, 158, 11));
            Brush sideBrush = new SolidBrush(Color.FromArgb(120, 0, 120, 215));

            int segments = circlePoints.Count;
            PointF[] topFace = new PointF[segments];
            PointF[] bottomFace = new PointF[segments];
            
            for (int i = 0; i < segments; i++)
            {
                topFace[i] = projectedPoints[i * 2 + 1];
                bottomFace[i] = projectedPoints[i * 2];
            }

            float topAvgDepth = 0, bottomAvgDepth = 0;
            for (int i = 0; i < segments; i++)
            {
                topAvgDepth += depths[i * 2 + 1];
                bottomAvgDepth += depths[i * 2];
            }
            topAvgDepth /= segments;
            bottomAvgDepth /= segments;

            if (bottomAvgDepth > topAvgDepth)
            {
                backGraphics.FillPolygon(topBrush, topFace);
                for (int i = 0; i < segments; i++)
                {
                    int next = (i + 1) % segments;
                    PointF[] sideFace = new PointF[] 
                    { 
                        projectedPoints[i * 2], 
                        projectedPoints[next * 2], 
                        projectedPoints[next * 2 + 1], 
                        projectedPoints[i * 2 + 1] 
                    };
                    backGraphics.FillPolygon(sideBrush, sideFace);
                }
                backGraphics.FillPolygon(bottomBrush, bottomFace);
            }
            else
            {
                backGraphics.FillPolygon(bottomBrush, bottomFace);
                for (int i = 0; i < segments; i++)
                {
                    int next = (i + 1) % segments;
                    PointF[] sideFace = new PointF[] 
                    { 
                        projectedPoints[i * 2], 
                        projectedPoints[next * 2], 
                        projectedPoints[next * 2 + 1], 
                        projectedPoints[i * 2 + 1] 
                    };
                    backGraphics.FillPolygon(sideBrush, sideFace);
                }
                backGraphics.FillPolygon(topBrush, topFace);
            }

            foreach (var edge in cylinderEdges)
            {
                backGraphics.DrawLine(edgePen, projectedPoints[edge[0]], projectedPoints[edge[1]]);
            }

            backGraphics.DrawPolygon(edgePen, bottomFace);
            backGraphics.DrawPolygon(edgePen, topFace);

            if (currentMode == Mode.Extrude3D)
            {
                if (isExtruding)
                {
                    backGraphics.DrawString("正在拉伸 - 继续拖动调整高度", 
                        new Font("Segoe UI", 14f, FontStyle.Bold), Brushes.Yellow, 20, 80);
                }
                else
                {
                    backGraphics.DrawString("拉伸完成 - 可再次拖动调整高度", 
                        new Font("Segoe UI", 12f), Brushes.White, 20, 80);
                }
            }
            else if (currentMode == Mode.Rotate3D)
            {
                backGraphics.DrawString("旋转模式 - 拖动鼠标旋转视角", 
                    new Font("Segoe UI", 12f), Brushes.White, 20, 80);
            }

            edgePen.Dispose();
            topBrush.Dispose();
            bottomBrush.Dispose();
            sideBrush.Dispose();
        }

        private void DrawGrid()
        {
            Pen gridPen = new Pen(Color.FromArgb(60, 60, 65), 1);
            int width = ClientSize.Width;
            int height = ClientSize.Height;
            int step = 50;

            for (int x = 0; x < width; x += step)
            {
                backGraphics.DrawLine(gridPen, x, 60, x, height);
            }
            for (int y = 60; y < height; y += step)
            {
                backGraphics.DrawLine(gridPen, 0, y, width, y);
            }

            gridPen.Dispose();
        }
    }
}