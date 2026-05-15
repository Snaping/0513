using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Algorithms
{
    public class LineAlgorithm : GraphicsAlgorithmBase
    {
        public LineAlgorithm()
        {
            Name = "直线绘制算法";
            Description = "演示DDA和Bresenham两种直线绘制算法的参数调整效果。";

            Properties.Add(new AlgorithmProperty
            {
                Name = "X1",
                DisplayName = "起点 X",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "Y1",
                DisplayName = "起点 Y",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "X2",
                DisplayName = "终点 X",
                Value = 200.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "Y2",
                DisplayName = "终点 Y",
                Value = 150.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "LineWidth",
                DisplayName = "线宽",
                Value = 2,
                ValueType = typeof(int),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "ShowPoints",
                DisplayName = "显示像素点",
                Value = true,
                ValueType = typeof(bool),
                IsEnabled = true
            });

            SubscribeToPropertyChanges();
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double x1 = GetPropertyValue<double>("X1");
            double y1 = GetPropertyValue<double>("Y1");
            double x2 = GetPropertyValue<double>("X2");
            double y2 = GetPropertyValue<double>("Y2");
            int lineWidth = GetPropertyValue<int>("LineWidth");
            bool showPoints = GetPropertyValue<bool>("ShowPoints");

            double centerX = width / 2;
            double centerY = height / 2;

            Point p1 = new Point(centerX + x1, centerY - y1);
            Point p2 = new Point(centerX + x2, centerY - y2);

            var pen = new Pen(Brushes.Blue, lineWidth);
            dc.DrawLine(pen, p1, p2);

            if (showPoints)
            {
                int dx = (int)Math.Abs(x2 - x1);
                int dy = (int)Math.Abs(y2 - y1);
                int steps = Math.Max(dx, dy);

                for (int i = 0; i <= steps; i++)
                {
                    double t = (double)i / steps;
                    double px = p1.X + t * (p2.X - p1.X);
                    double py = p1.Y + t * (p2.Y - p1.Y);
                    dc.DrawRectangle(Brushes.Red, null, new Rect(px - 2, py - 2, 4, 4));
                }
            }

            dc.DrawText(new FormattedText(
                $"起点: ({x1:F0}, {y1:F0})",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 20));

            dc.DrawText(new FormattedText(
                $"终点: ({x2:F0}, {y2:F0})",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 40));
        }

        public override string GetCode()
        {
            return @"// DDA直线算法实现
public static void DrawLineDDA(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = Math.Max(Math.Abs(dx), Math.Abs(dy));
    
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    
    float x = x1;
    float y = y1;
    
    for (int i = 0; i <= steps; i++)
    {
        SetPixel((int)Math.Round(x), (int)Math.Round(y));
        x += xInc;
        y += yInc;
    }
}

// Bresenham直线算法实现
public static void DrawLineBresenham(int x1, int y1, int x2, int y2)
{
    int dx = Math.Abs(x2 - x1);
    int dy = Math.Abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    
    while (true)
    {
        SetPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}";
        }
    }
}