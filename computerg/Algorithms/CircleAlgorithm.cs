using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Algorithms
{
    public class CircleAlgorithm : GraphicsAlgorithmBase
    {
        public CircleAlgorithm()
        {
            Name = "圆绘制算法";
            Description = "演示中点圆绘制算法，利用八分圆对称性提高绘制效率。";

            Properties.Add(new AlgorithmProperty
            {
                Name = "CenterX",
                DisplayName = "圆心 X",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "CenterY",
                DisplayName = "圆心 Y",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "Radius",
                DisplayName = "半径",
                Value = 80.0,
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

            Properties.Add(new AlgorithmProperty
            {
                Name = "FillCircle",
                DisplayName = "填充圆",
                Value = false,
                ValueType = typeof(bool),
                IsEnabled = true
            });

            SubscribeToPropertyChanges();
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = GetPropertyValue<double>("CenterX");
            double centerY = GetPropertyValue<double>("CenterY");
            double radius = GetPropertyValue<double>("Radius");
            int lineWidth = GetPropertyValue<int>("LineWidth");
            bool showPoints = GetPropertyValue<bool>("ShowPoints");
            bool fillCircle = GetPropertyValue<bool>("FillCircle");

            double cx = width / 2 + centerX;
            double cy = height / 2 - centerY;

            var brush = Brushes.DarkGreen;
            var pen = new Pen(brush, lineWidth);

            if (showPoints)
            {
                int x = 0;
                int y = (int)radius;
                int d = 1 - (int)radius;

                Draw8Points(dc, (int)cx, (int)cy, x, y, brush);

                while (x < y)
                {
                    if (d < 0)
                    {
                        d += 2 * x + 3;
                    }
                    else
                    {
                        d += 2 * (x - y) + 5;
                        y--;
                    }
                    x++;
                    Draw8Points(dc, (int)cx, (int)cy, x, y, brush);
                }
            }

            if (fillCircle)
            {
                dc.DrawEllipse(brush, null, new Point(cx, cy), radius, radius);
            }
            else
            {
                dc.DrawEllipse(null, pen, new Point(cx, cy), radius, radius);
            }

            dc.DrawEllipse(Brushes.Red, null, new Point(cx, cy), 4, 4);

            dc.DrawText(new FormattedText(
                $"圆心: ({centerX:F0}, {centerY:F0})",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 20));

            dc.DrawText(new FormattedText(
                $"半径: {radius:F0}",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 40));
        }

        private void Draw8Points(DrawingContext dc, int cx, int cy, int x, int y, Brush brush)
        {
            dc.DrawRectangle(brush, null, new Rect(cx + x - 2, cy + y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx - x - 2, cy + y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx + x - 2, cy - y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx - x - 2, cy - y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx + y - 2, cy + x - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx - y - 2, cy + x - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx + y - 2, cy - x - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(cx - y - 2, cy - x - 2, 4, 4));
        }

        public override string GetCode()
        {
            return @"// 中点圆算法实现
public static void DrawCircle(int x0, int y0, int radius)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    
    Draw8Points(x0, y0, x, y);
    
    while (x < y)
    {
        if (d < 0)
        {
            // 选择东边像素
            d += 2 * x + 3;
        }
        else
        {
            // 选择东南像素
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        Draw8Points(x0, y0, x, y);
    }
}

// 利用八分对称性绘制8个对称点
private static void Draw8Points(int x0, int y0, int x, int y)
{
    SetPixel(x0 + x, y0 + y);
    SetPixel(x0 - x, y0 + y);
    SetPixel(x0 + x, y0 - y);
    SetPixel(x0 - x, y0 - y);
    SetPixel(x0 + y, y0 + x);
    SetPixel(x0 - y, y0 + x);
    SetPixel(x0 + y, y0 - x);
    SetPixel(x0 - y, y0 - x);
}

// 算法特点：
// 1. 仅使用整数运算
// 2. 利用八分对称性，计算量减少到1/8
// 3. 效率高，适合硬件实现";
        }
    }
}