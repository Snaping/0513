using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Algorithms
{
    public class EllipseAlgorithm : GraphicsAlgorithmBase
    {
        public EllipseAlgorithm()
        {
            Name = "椭圆绘制算法";
            Description = "演示中点椭圆算法，利用四分对称性绘制椭圆。";

            Properties.Add(new AlgorithmProperty
            {
                Name = "CenterX",
                DisplayName = "中心 X",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "CenterY",
                DisplayName = "中心 Y",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "RadiusX",
                DisplayName = "长半轴 a",
                Value = 100.0,
                ValueType = typeof(double),
                IsEnabled = true
            });

            Properties.Add(new AlgorithmProperty
            {
                Name = "RadiusY",
                DisplayName = "短半轴 b",
                Value = 60.0,
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
                Name = "FillEllipse",
                DisplayName = "填充椭圆",
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
            double rx = GetPropertyValue<double>("RadiusX");
            double ry = GetPropertyValue<double>("RadiusY");
            int lineWidth = GetPropertyValue<int>("LineWidth");
            bool showPoints = GetPropertyValue<bool>("ShowPoints");
            bool fillEllipse = GetPropertyValue<bool>("FillEllipse");

            double cx = width / 2 + centerX;
            double cy = height / 2 - centerY;

            var brush = Brushes.Teal;
            var pen = new Pen(brush, lineWidth);

            if (showPoints)
            {
                int a = (int)rx;
                int b = (int)ry;
                int x0 = (int)cx;
                int y0 = (int)cy;

                int x = 0;
                int y = b;
                long a2 = a * a;
                long b2 = b * b;
                long d1 = b2 - a2 * b + a2 / 4;
                long dx = 2 * b2 * x;
                long dy = 2 * a2 * y;

                Draw4Points(dc, x0, y0, x, y, brush);

                while (dx < dy)
                {
                    if (d1 < 0)
                    {
                        x++;
                        dx += 2 * b2;
                        d1 += dx + b2;
                    }
                    else
                    {
                        x++;
                        y--;
                        dx += 2 * b2;
                        dy -= 2 * a2;
                        d1 += dx - dy + b2;
                    }
                    Draw4Points(dc, x0, y0, x, y, brush);
                }

                long d2 = b2 * (x + 1) * (x + 1) + a2 * (y - 1) * (y - 1) - a2 * b2;

                while (y > 0)
                {
                    if (d2 > 0)
                    {
                        y--;
                        dy -= 2 * a2;
                        d2 += a2 - dy;
                    }
                    else
                    {
                        y--;
                        x++;
                        dx += 2 * b2;
                        dy -= 2 * a2;
                        d2 += dx - dy + a2;
                    }
                    Draw4Points(dc, x0, y0, x, y, brush);
                }
            }

            if (fillEllipse)
            {
                dc.DrawEllipse(brush, null, new Point(cx, cy), rx, ry);
            }
            else
            {
                dc.DrawEllipse(null, pen, new Point(cx, cy), rx, ry);
            }

            dc.DrawEllipse(Brushes.Red, null, new Point(cx, cy), 4, 4);

            dc.DrawText(new FormattedText(
                $"长半轴 a: {rx:F0}",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 20));

            dc.DrawText(new FormattedText(
                $"短半轴 b: {ry:F0}",
                System.Globalization.CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface("Arial"), 12, Brushes.Black, 96),
                new Point(20, 40));
        }

        private void Draw4Points(DrawingContext dc, int x0, int y0, int x, int y, Brush brush)
        {
            dc.DrawRectangle(brush, null, new Rect(x0 + x - 2, y0 + y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(x0 - x - 2, y0 + y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(x0 + x - 2, y0 - y - 2, 4, 4));
            dc.DrawRectangle(brush, null, new Rect(x0 - x - 2, y0 - y - 2, 4, 4));
        }

        public override string GetCode()
        {
            return @"// 中点椭圆算法实现
public static void DrawEllipse(int x0, int y0, int a, int b)
{
    int x = 0;
    int y = b;
    long a2 = a * a;
    long b2 = b * b;
    
    // 区域1：|斜率| < 1
    long d1 = b2 - a2 * b + a2 / 4;
    long dx = 2 * b2 * x;
    long dy = 2 * a2 * y;
    
    Draw4Points(x0, y0, x, y);
    
    while (dx < dy)
    {
        if (d1 < 0)
        {
            // 选择东边像素
            x++;
            dx += 2 * b2;
            d1 += dx + b2;
        }
        else
        {
            // 选择东南像素
            x++;
            y--;
            dx += 2 * b2;
            dy -= 2 * a2;
            d1 += dx - dy + b2;
        }
        Draw4Points(x0, y0, x, y);
    }
    
    // 区域2：|斜率| > 1
    long d2 = b2 * (x + 1) * (x + 1) + a2 * (y - 1) * (y - 1) - a2 * b2;
    
    while (y > 0)
    {
        if (d2 > 0)
        {
            // 选择南边像素
            y--;
            dy -= 2 * a2;
            d2 += a2 - dy;
        }
        else
        {
            // 选择东南像素
            y--;
            x++;
            dx += 2 * b2;
            dy -= 2 * a2;
            d2 += dx - dy + a2;
        }
        Draw4Points(x0, y0, x, y);
    }
}

// 利用四分对称性绘制4个点
private static void Draw4Points(int x0, int y0, int x, int y)
{
    SetPixel(x0 + x, y0 + y);
    SetPixel(x0 - x, y0 + y);
    SetPixel(x0 + x, y0 - y);
    SetPixel(x0 - x, y0 - y);
}

// 算法特点：
// 1. 利用椭圆四分对称性
// 2. 分两个区域处理，效率更高
// 3. 仅使用整数运算
// 4. 决策参数判断下一像素位置";
        }
    }
}