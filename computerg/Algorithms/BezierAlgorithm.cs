using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Algorithms
{
    public class BezierAlgorithm : GraphicsAlgorithmBase
    {
        public BezierAlgorithm()
        {
            Name = "贝塞尔曲线算法";
            Description = "演示三次贝塞尔曲线的生成，通过控制点调整曲线形状。";

            Properties.Add(new AlgorithmProperty
            {
                Name = "P0X",
                DisplayName = "起点 P0 X",
                Value = -120.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P0Y",
                DisplayName = "起点 P0 Y",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P1X",
                DisplayName = "控制点 P1 X",
                Value = -40.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P1Y",
                DisplayName = "控制点 P1 Y",
                Value = 80.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P2X",
                DisplayName = "控制点 P2 X",
                Value = 40.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P2Y",
                DisplayName = "控制点 P2 Y",
                Value = -80.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P3X",
                DisplayName = "终点 P3 X",
                Value = 120.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "P3Y",
                DisplayName = "终点 P3 Y",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "ShowControlPoints",
                DisplayName = "显示控制点",
                Value = true,
                ValueType = typeof(bool),
                IsEnabled = true
            });

            SubscribeToPropertyChanges();
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double p0x = GetPropertyValue<double>("P0X");
            double p0y = GetPropertyValue<double>("P0Y");
            double p1x = GetPropertyValue<double>("P1X");
            double p1y = GetPropertyValue<double>("P1Y");
            double p2x = GetPropertyValue<double>("P2X");
            double p2y = GetPropertyValue<double>("P2Y");
            double p3x = GetPropertyValue<double>("P3X");
            double p3y = GetPropertyValue<double>("P3Y");
            bool showControlPoints = GetPropertyValue<bool>("ShowControlPoints");

            double centerX = width / 2;
            double centerY = height / 2;

            Point p0 = new Point(centerX + p0x, centerY - p0y);
            Point p1 = new Point(centerX + p1x, centerY - p1y);
            Point p2 = new Point(centerX + p2x, centerY - p2y);
            Point p3 = new Point(centerX + p3x, centerY - p3y);

            if (showControlPoints)
            {
                var controlPen = new Pen(Brushes.Gray, 1);
                dc.DrawLine(controlPen, p0, p1);
                dc.DrawLine(controlPen, p1, p2);
                dc.DrawLine(controlPen, p2, p3);

                DrawPoint(dc, p0, "P0", Brushes.Red);
                DrawPoint(dc, p1, "P1", Brushes.Green);
                DrawPoint(dc, p2, "P2", Brushes.Blue);
                DrawPoint(dc, p3, "P3", Brushes.Red);
            }

            var pathGeometry = new PathGeometry();
            var figure = new PathFigure();
            figure.StartPoint = p0;

            var bezierSegment = new BezierSegment(p1, p2, p3, true);
            figure.Segments.Add(bezierSegment);
            pathGeometry.Figures.Add(figure);

            dc.DrawGeometry(null, new Pen(Brushes.Purple, 3), pathGeometry);

            for (int i = 0; i <= 20; i++)
            {
                double t = (double)i / 20;
                double x = BezierPoint(t, p0.X, p1.X, p2.X, p3.X);
                double y = BezierPoint(t, p0.Y, p1.Y, p2.Y, p3.Y);
                dc.DrawRectangle(Brushes.Magenta, null, new Rect(x - 2, y - 2, 4, 4));
            }

            dc.DrawText(new FormattedText(
                $"采样点数: 21",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 20));
        }

        private double BezierPoint(double t, double p0, double p1, double p2, double p3)
        {
            double mt = 1 - t;
            return mt * mt * mt * p0 +
                   3 * mt * mt * t * p1 +
                   3 * mt * t * t * p2 +
                   t * t * t * p3;
        }

        private void DrawPoint(DrawingContext dc, Point p, string label, Brush brush)
        {
            dc.DrawEllipse(brush, new Pen(Brushes.Black, 1), p, 6, 6);
            dc.DrawText(new FormattedText(
                label,
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 10, Brushes.Black, 96),
                new Point(p.X + 10, p.Y - 5));
        }

        public override string GetCode()
        {
            return @"// 三次贝塞尔曲线算法实现

// 计算参数t处的贝塞尔曲线点
public static Point GetBezierPoint(double t, Point p0, Point p1, Point p2, Point p3)
{
    double mt = 1 - t;
    
    // 伯恩斯坦多项式
    double x = mt * mt * mt * p0.X + 
               3 * mt * mt * t * p1.X + 
               3 * mt * t * t * p2.X + 
               t * t * t * p3.X;
               
    double y = mt * mt * mt * p0.Y + 
               3 * mt * mt * t * p1.Y + 
               3 * mt * t * t * p2.Y + 
               t * t * t * p3.Y;
    
    return new Point(x, y);
}

// 德卡斯特里奥算法（递归求值）
public static Point DeCasteljau(double t, Point[] points)
{
    if (points.Length == 1) return points[0];
    
    Point[] newPoints = new Point[points.Length - 1];
    for (int i = 0; i < points.Length - 1; i++)
    {
        newPoints[i] = new Point(
            (1 - t) * points[i].X + t * points[i + 1].X,
            (1 - t) * points[i].Y + t * points[i + 1].Y
        );
    }
    
    return DeCasteljau(t, newPoints);
}

// 绘制贝塞尔曲线
public static void DrawBezier(Point p0, Point p1, Point p2, Point p3, int segments = 50)
{
    Point prev = p0;
    
    for (int i = 1; i <= segments; i++)
    {
        double t = (double)i / segments;
        Point current = GetBezierPoint(t, p0, p1, p2, p3);
        DrawLine(prev, current);
        prev = current;
    }
}

// 算法特点：
// 1. 曲线经过首末两点
// 2. 首端切向量平行于P0P1
// 3. 末端切向量平行于P2P3
// 4. 具有凸包性质
// 5. 仿射变换不变性";
        }
    }
}