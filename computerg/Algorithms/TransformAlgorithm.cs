using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Algorithms
{
    public class TransformAlgorithm : GraphicsAlgorithmBase
    {
        public TransformAlgorithm()
        {
            Name = "几何变换算法";
            Description = "演示平移、旋转、缩放等基本几何变换的效果。";

            Properties.Add(new AlgorithmProperty
            {
                Name = "TranslateX",
                DisplayName = "X方向平移",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "TranslateY",
                DisplayName = "Y方向平移",
                Value = 30.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "RotateAngle",
                DisplayName = "旋转角度",
                Value = 30.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "ScaleX",
                DisplayName = "X方向缩放",
                Value = 1.2,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "ScaleY",
                DisplayName = "Y方向缩放",
                Value = 0.8,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "ShowOriginal",
                DisplayName = "显示原始图形",
                Value = true,
                ValueType = typeof(bool),
                IsEnabled = true
            });

            SubscribeToPropertyChanges();
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double tx = GetPropertyValue<double>("TranslateX");
            double ty = GetPropertyValue<double>("TranslateY");
            double angle = GetPropertyValue<double>("RotateAngle");
            double sx = GetPropertyValue<double>("ScaleX");
            double sy = GetPropertyValue<double>("ScaleY");
            bool showOriginal = GetPropertyValue<bool>("ShowOriginal");

            double centerX = width / 2;
            double centerY = height / 2;

            Point[] rectPoints = new Point[]
            {
                new Point(-50, -30),
                new Point(50, -30),
                new Point(50, 30),
                new Point(-50, 30)
            };

            if (showOriginal)
            {
                var originalPoints = new PointCollection();
                foreach (var p in rectPoints)
                {
                    originalPoints.Add(new Point(centerX + p.X, centerY + p.Y));
                }
                DrawPolygon(dc, originalPoints, Brushes.LightGray, new Pen(Brushes.Gray, 1));
            }

            double rad = angle * Math.PI / 180;
            var transformedPoints = new PointCollection();
            foreach (var p in rectPoints)
            {
                double x = p.X;
                double y = p.Y;
                
                double newX = (x * Math.Cos(rad) - y * Math.Sin(rad)) * sx;
                double newY = (x * Math.Sin(rad) + y * Math.Cos(rad)) * sy;
                
                transformedPoints.Add(new Point(centerX + newX + tx, centerY + newY - ty));
            }

            DrawPolygon(dc, transformedPoints, Brushes.DodgerBlue, new Pen(Brushes.Blue, 2));

            dc.DrawEllipse(Brushes.Red, null, new Point(centerX, centerY), 4, 4);

            if (Math.Abs(tx) > 0.1 || Math.Abs(ty) > 0.1)
            {
                var arrowPen = new Pen(Brushes.OrangeRed, 1);
                arrowPen.DashStyle = DashStyles.Dash;
                dc.DrawLine(arrowPen, new Point(centerX, centerY), new Point(centerX + tx, centerY));
                dc.DrawLine(arrowPen, new Point(centerX + tx, centerY), new Point(centerX + tx, centerY - ty));
            }

            dc.DrawText(new FormattedText(
                $"平移: ({tx:F0}, {ty:F0})",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 20));

            dc.DrawText(new FormattedText(
                $"旋转: {angle:F0}°",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 40));

            dc.DrawText(new FormattedText(
                $"缩放: ({sx:F2}, {sy:F2})",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 60));
        }

        private void DrawPolygon(DrawingContext dc, PointCollection points, Brush fillBrush, Pen pen)
        {
            var pathGeometry = new PathGeometry();
            var figure = new PathFigure();
            figure.StartPoint = points[0];
            
            for (int i = 1; i < points.Count; i++)
            {
                figure.Segments.Add(new LineSegment(points[i], true));
            }
            
            figure.IsClosed = true;
            pathGeometry.Figures.Add(figure);
            dc.DrawGeometry(fillBrush, pen, pathGeometry);
        }

        public override string GetCode()
        {
            return @"// 2D几何变换矩阵运算

// 平移变换
public static Point Translate(Point p, double tx, double ty)
{
    return new Point(p.X + tx, p.Y + ty);
}

// 旋转变换（原点为中心）
public static Point Rotate(Point p, double angleDegrees)
{
    double rad = angleDegrees * Math.PI / 180;
    double cos = Math.Cos(rad);
    double sin = Math.Sin(rad);
    
    return new Point(
        p.X * cos - p.Y * sin,
        p.X * sin + p.Y * cos
    );
}

// 缩放变换
public static Point Scale(Point p, double sx, double sy)
{
    return new Point(p.X * sx, p.Y * sy);
}

// 绕任意点旋转
public static Point RotateAroundPoint(Point p, Point center, double angleDegrees)
{
    // 1. 平移到原点
    double x = p.X - center.X;
    double y = p.Y - center.Y;
    
    // 2. 旋转
    double rad = angleDegrees * Math.PI / 180;
    double newX = x * Math.Cos(rad) - y * Math.Sin(rad);
    double newY = x * Math.Sin(rad) + y * Math.Cos(rad);
    
    // 3. 平移回原位置
    return new Point(newX + center.X, newY + center.Y);
}

// 变换矩阵（齐次坐标）
// [x']   [a  b  c] [x]
// [y'] = [d  e  f] [y]
// [1 ]   [0  0  1] [1]

// 复合变换：先缩放，再旋转，最后平移
// M = Mt * Mr * Ms";
        }
    }
}