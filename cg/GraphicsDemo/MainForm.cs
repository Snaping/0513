
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace GraphicsDemo
{
    public partial class MainForm : Form
    {
        private TextBox codeTextBox;
        private ComboBox algorithmComboBox;
        private Button runButton;
        private Button clearButton;
        private GraphicsPanel graphicsPanel;
        private Label statusLabel;

        private Dictionary<string, string> algorithms;

        public MainForm()
        {
            InitializeComponent();
            InitializeAlgorithms();
        }

        private void InitializeComponent()
        {
            this.Text = "计算机图形学演示教学工具";
            this.Size = new Size(1000, 650);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.MinimumSize = new Size(800, 500);

            Label algoLabel = new Label();
            algoLabel.Text = "选择算法:";
            algoLabel.Location = new Point(10, 10);
            algoLabel.Size = new Size(80, 20);
            this.Controls.Add(algoLabel);

            algorithmComboBox = new ComboBox();
            algorithmComboBox.Location = new Point(90, 10);
            algorithmComboBox.Size = new Size(180, 20);
            algorithmComboBox.SelectedIndexChanged += AlgorithmComboBox_SelectedIndexChanged;
            this.Controls.Add(algorithmComboBox);

            runButton = new Button();
            runButton.Text = "运行代码";
            runButton.Location = new Point(280, 10);
            runButton.Size = new Size(100, 20);
            runButton.Click += RunButton_Click;
            this.Controls.Add(runButton);

            clearButton = new Button();
            clearButton.Text = "清空画布";
            clearButton.Location = new Point(390, 10);
            clearButton.Size = new Size(100, 20);
            clearButton.Click += ClearButton_Click;
            this.Controls.Add(clearButton);

            statusLabel = new Label();
            statusLabel.Text = "就绪";
            statusLabel.Location = new Point(500, 10);
            statusLabel.Size = new Size(200, 20);
            this.Controls.Add(statusLabel);

            Label codeLabel = new Label();
            codeLabel.Text = "代码编辑区:";
            codeLabel.Location = new Point(10, 40);
            codeLabel.Size = new Size(100, 20);
            this.Controls.Add(codeLabel);

            codeTextBox = new TextBox();
            codeTextBox.Location = new Point(10, 60);
            codeTextBox.Size = new Size(480, 250);
            codeTextBox.Multiline = true;
            codeTextBox.ScrollBars = ScrollBars.Both;
            codeTextBox.Font = new Font("Consolas", 10);
            this.Controls.Add(codeTextBox);

            Label graphicsLabel = new Label();
            graphicsLabel.Text = "图形渲染区:";
            graphicsLabel.Location = new Point(500, 40);
            graphicsLabel.Size = new Size(100, 20);
            this.Controls.Add(graphicsLabel);

            graphicsPanel = new GraphicsPanel();
            graphicsPanel.Location = new Point(500, 60);
            graphicsPanel.Size = new Size(470, 350);
            graphicsPanel.BorderStyle = BorderStyle.FixedSingle;
            this.Controls.Add(graphicsPanel);

            SplitContainer splitContainer = new SplitContainer();
            splitContainer.Location = new Point(10, 320);
            splitContainer.Size = new Size(960, 280);
            splitContainer.Orientation = Orientation.Vertical;
            splitContainer.Panel1MinSize = 200;
            splitContainer.Panel2MinSize = 200;
            this.Controls.Add(splitContainer);

            Label infoLabel = new Label();
            infoLabel.Text = "算法说明:";
            infoLabel.Location = new Point(10, 320);
            infoLabel.Size = new Size(100, 20);
            this.Controls.Add(infoLabel);

            TextBox infoTextBox = new TextBox();
            infoTextBox.Location = new Point(10, 340);
            infoTextBox.Size = new Size(480, 260);
            infoTextBox.Multiline = true;
            infoTextBox.ScrollBars = ScrollBars.Both;
            infoTextBox.Font = new Font("Consolas", 9);
            infoTextBox.ReadOnly = true;
            infoTextBox.BackColor = Color.LightGray;
            this.Controls.Add(infoTextBox);
        }

        private void InitializeAlgorithms()
        {
            algorithms = new Dictionary<string, string>();

            algorithms.Add("DDA直线算法", 
@"// DDA (Digital Differential Analyzer) 直线绘制算法
// 原理：通过计算像素点的增量来绘制直线
// 时间复杂度：O(max(|dx|, |dy|))

int x1 = 50, y1 = 50;
int x2 = 350, y2 = 250;

List<Point> points = GraphicsAlgorithms.DDA(x1, y1, x2, y2);
panel.RenderPoints(points, Color.Red);");

            algorithms.Add("Bresenham直线算法", 
@"// Bresenham 直线绘制算法
// 原理：使用整数运算，通过误差项决定下一个像素
// 特点：效率高，仅使用整数运算
// 时间复杂度：O(max(|dx|, |dy|))

int x1 = 50, y1 = 50;
int x2 = 350, y2 = 250;

List<Point> points = GraphicsAlgorithms.BresenhamLine(x1, y1, x2, y2);
panel.RenderPoints(points, Color.Blue);");

            algorithms.Add("中点画圆算法", 
@"// Midpoint Circle Algorithm
// 原理：利用圆的对称性，通过中点判断选择像素
// 特点：只计算1/8圆，利用对称性得到完整圆
// 时间复杂度：O(r)

int cx = 235, cy = 175;
int radius = 120;

List<Point> points = GraphicsAlgorithms.MidpointCircle(cx, cy, radius);
panel.RenderPoints(points, Color.Green);");

            algorithms.Add("中点椭圆算法", 
@"// Midpoint Ellipse Algorithm
// 原理：利用椭圆的对称性，分两个区域计算
// 特点：高效的椭圆绘制算法
// 时间复杂度：O(rx + ry)

int cx = 235, cy = 175;
int rx = 150, ry = 100;

List<Point> points = GraphicsAlgorithms.MidpointEllipse(cx, cy, rx, ry);
panel.RenderPoints(points, Color.Purple);");

            algorithms.Add("多边形扫描线填充", 
@"// Scanline Polygon Fill Algorithm
// 原理：按扫描线顺序填充多边形内部
// 步骤：1.求交点 2.配对交点 3.填充线段
// 时间复杂度：O(n * h)

List<Point> polygon = new List<Point>();
polygon.Add(new Point(235, 50));
polygon.Add(new Point(380, 120));
polygon.Add(new Point(330, 250));
polygon.Add(new Point(140, 250));
polygon.Add(new Point(90, 120));

List<Point> filled = GraphicsAlgorithms.ScanlineFill(polygon);
panel.RenderPoints(filled, Color.Orange);");

            algorithms.Add("Cohen-Sutherland线段裁剪", 
@"// Cohen-Sutherland Line Clipping
// 原理：区域编码，快速判断线段与窗口关系
// 编码规则：左(1)、右(2)、下(4)、上(8)
// 时间复杂度：O(1)

int x1 = 50, y1 = 50;
int x2 = 450, y2 = 350;
int xmin = 100, ymin = 100;
int xmax = 370, ymax = 250;

List<Point> clipped = GraphicsAlgorithms.CohenSutherlandClip(x1, y1, x2, y2, xmin, ymin, xmax, ymax);
if (clipped.Count >= 2)
{
    panel.RenderLine(clipped[0].X, clipped[0].Y, clipped[1].X, clipped[1].Y, Color.Red);
}

panel.RenderLine(xmin, ymin, xmax, ymin, Color.Gray);
panel.RenderLine(xmax, ymin, xmax, ymax, Color.Gray);
panel.RenderLine(xmax, ymax, xmin, ymax, Color.Gray);
panel.RenderLine(xmin, ymax, xmin, ymin, Color.Gray);");

            algorithms.Add("综合示例", 
@"// 综合示例：绘制多个图形

// 绘制矩形窗口
panel.RenderLine(80, 80, 390, 80, Color.Gray);
panel.RenderLine(390, 80, 390, 270, Color.Gray);
panel.RenderLine(390, 270, 80, 270, Color.Gray);
panel.RenderLine(80, 270, 80, 80, Color.Gray);

// DDA直线 (红色)
List<Point> line1 = GraphicsAlgorithms.DDA(30, 30, 235, 175);
panel.RenderPoints(line1, Color.Red);

// Bresenham直线 (蓝色)
List<Point> line2 = GraphicsAlgorithms.BresenhamLine(430, 30, 235, 175);
panel.RenderPoints(line2, Color.Blue);

// 圆形 (绿色)
List<Point> circle = GraphicsAlgorithms.MidpointCircle(235, 175, 80);
panel.RenderPoints(circle, Color.Green);");

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
            try
            {
                string code = codeTextBox.Text;
                CodeParser parser = new CodeParser(code, graphicsPanel);
                parser.ParseAndExecute();
                statusLabel.Text = "代码执行成功";
            }
            catch (Exception ex)
            {
                statusLabel.Text = $"错误: {ex.Message}";
                MessageBox.Show($"代码执行失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ClearButton_Click(object sender, EventArgs e)
        {
            graphicsPanel.ClearCanvas();
            statusLabel.Text = "画布已清空";
        }
    }
}
