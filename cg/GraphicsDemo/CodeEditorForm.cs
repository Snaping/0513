
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace GraphicsDemo
{
    public partial class CodeEditorForm : Form
    {
        private TextBox codeTextBox;
        private ComboBox algorithmComboBox;
        private Button runButton;
        private GraphicsForm graphicsForm;

        private Dictionary<string, string> algorithms;

        public CodeEditorForm()
        {
            InitializeComponent();
            InitializeAlgorithms();
        }

        private void InitializeComponent()
        {
            this.Text = "代码编辑器 - 图形学算法";
            this.Size = new System.Drawing.Size(700, 500);
            this.StartPosition = FormStartPosition.CenterScreen;

            Label algoLabel = new Label();
            algoLabel.Text = "选择算法:";
            algoLabel.Location = new System.Drawing.Point(10, 10);
            algoLabel.Size = new System.Drawing.Size(80, 20);
            this.Controls.Add(algoLabel);

            algorithmComboBox = new ComboBox();
            algorithmComboBox.Location = new System.Drawing.Point(90, 10);
            algorithmComboBox.Size = new System.Drawing.Size(200, 20);
            algorithmComboBox.SelectedIndexChanged += AlgorithmComboBox_SelectedIndexChanged;
            this.Controls.Add(algorithmComboBox);

            runButton = new Button();
            runButton.Text = "运行代码";
            runButton.Location = new System.Drawing.Point(300, 10);
            runButton.Size = new System.Drawing.Size(100, 20);
            runButton.Click += RunButton_Click;
            this.Controls.Add(runButton);

            codeTextBox = new TextBox();
            codeTextBox.Location = new System.Drawing.Point(10, 40);
            codeTextBox.Size = new System.Drawing.Size(660, 420);
            codeTextBox.Multiline = true;
            codeTextBox.ScrollBars = ScrollBars.Both;
            codeTextBox.Font = new Font("Courier New", 10);
            this.Controls.Add(codeTextBox);
        }

        private void InitializeAlgorithms()
        {
            algorithms = new Dictionary<string, string>();

            algorithms.Add("DDA直线算法", 
@"// DDA (Digital Differential Analyzer) 直线绘制算法
// 原理：通过计算像素点的增量来绘制直线
// 时间复杂度：O(max(|dx|, |dy|))

int x1 = 50, y1 = 50;
int x2 = 250, y2 = 200;

List<Point> points = GraphicsAlgorithms.DDA(x1, y1, x2, y2);
RenderPoints(points, Color.Red);");

            algorithms.Add("Bresenham直线算法", 
@"// Bresenham 直线绘制算法
// 原理：使用整数运算，通过误差项决定下一个像素
// 特点：效率高，仅使用整数运算
// 时间复杂度：O(max(|dx|, |dy|))

int x1 = 50, y1 = 50;
int x2 = 250, y2 = 200;

List<Point> points = GraphicsAlgorithms.BresenhamLine(x1, y1, x2, y2);
RenderPoints(points, Color.Blue);");

            algorithms.Add("中点画圆算法", 
@"// Midpoint Circle Algorithm
// 原理：利用圆的对称性，通过中点判断选择像素
// 特点：只计算1/8圆，利用对称性得到完整圆
// 时间复杂度：O(r)

int cx = 300, cy = 200;
int radius = 100;

List<Point> points = GraphicsAlgorithms.MidpointCircle(cx, cy, radius);
RenderPoints(points, Color.Green);");

            algorithms.Add("中点椭圆算法", 
@"// Midpoint Ellipse Algorithm
// 原理：利用椭圆的对称性，分两个区域计算
// 特点：高效的椭圆绘制算法
// 时间复杂度：O(rx + ry)

int cx = 300, cy = 200;
int rx = 120, ry = 80;

List<Point> points = GraphicsAlgorithms.MidpointEllipse(cx, cy, rx, ry);
RenderPoints(points, Color.Purple);");

            algorithms.Add("多边形扫描线填充", 
@"// Scanline Polygon Fill Algorithm
// 原理：按扫描线顺序填充多边形内部
// 步骤：1.求交点 2.配对交点 3.填充线段
// 时间复杂度：O(n * h)

List<Point> polygon = new List<Point>();
polygon.Add(new Point(300, 50));
polygon.Add(new Point(450, 150));
polygon.Add(new Point(400, 280));
polygon.Add(new Point(200, 280));
polygon.Add(new Point(150, 150));

List<Point> filled = GraphicsAlgorithms.ScanlineFill(polygon);
RenderPoints(filled, Color.Orange);");

            algorithms.Add("Cohen-Sutherland线段裁剪", 
@"// Cohen-Sutherland Line Clipping
// 原理：区域编码，快速判断线段与窗口关系
// 编码规则：左(1)、右(2)、下(4)、上(8)
// 时间复杂度：O(1)

int x1 = 50, y1 = 50;
int x2 = 450, y2 = 350;
int xmin = 100, ymin = 100;
int xmax = 400, ymax = 300;

List<Point> clipped = GraphicsAlgorithms.CohenSutherlandClip(x1, y1, x2, y2, xmin, ymin, xmax, ymax);
if (clipped.Count >= 2)
{
    RenderLine(clipped[0].X, clipped[0].Y, clipped[1].X, clipped[1].Y, Color.Red);
}");

            algorithms.Add("综合示例", 
@"// 综合示例：绘制多个图形

// 绘制矩形窗口
RenderLine(100, 100, 400, 100, Color.Gray);
RenderLine(400, 100, 400, 300, Color.Gray);
RenderLine(400, 300, 100, 300, Color.Gray);
RenderLine(100, 300, 100, 100, Color.Gray);

// DDA直线
List<Point> line1 = GraphicsAlgorithms.DDA(50, 50, 250, 200);
RenderPoints(line1, Color.Red);

// Bresenham直线
List<Point> line2 = GraphicsAlgorithms.BresenhamLine(450, 50, 250, 200);
RenderPoints(line2, Color.Blue);

// 圆形
List<Point> circle = GraphicsAlgorithms.MidpointCircle(300, 200, 80);
RenderPoints(circle, Color.Green);");

            foreach (string key in algorithms.Keys)
            {
                algorithmComboBox.Items.Add(key);
            }

            if (algorithmComboBox.Items.Count > 0)
            {
                algorithmComboBox.SelectedIndex = 0;
            }
        }

        private void AlgorithmComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selected = algorithmComboBox.SelectedItem as string;
            if (selected != null && algorithms.ContainsKey(selected))
            {
                codeTextBox.Text = algorithms[selected];
            }
        }

        private void RunButton_Click(object sender, EventArgs e)
        {
            if (graphicsForm == null || graphicsForm.IsDisposed)
            {
                graphicsForm = new GraphicsForm();
                graphicsForm.FormClosed += GraphicsForm_FormClosed;
            }

            string code = codeTextBox.Text;
            graphicsForm.ExecuteCode(code);
            graphicsForm.Show();
            graphicsForm.BringToFront();
        }

        private void GraphicsForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            graphicsForm = null;
        }
    }
}
