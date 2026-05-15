using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Microsoft.CSharp;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.Services
{
    public class CompilationResult
    {
        public bool Success { get; set; }
        public List<string> Errors { get; set; } = new List<string>();
        public GraphicsAlgorithmBase? Algorithm { get; set; }
        public string? ErrorMessage { get; set; }
    }

    public static class CodeCompiler
    {
        private static readonly string[] RequiredAssemblies = new[]
        {
            "System.dll",
            "System.Core.dll",
            "WindowsBase.dll",
            "PresentationCore.dll",
            "PresentationFramework.dll",
            "System.Xaml.dll"
        };

        public static CompilationResult CompileAlgorithm(string code, string algorithmName)
        {
            var result = new CompilationResult();

            var fullCode = GenerateFullCode(code, algorithmName);

            var compiler = new CSharpCodeProvider();
            var parameters = new CompilerParameters
            {
                GenerateExecutable = false,
                GenerateInMemory = true,
                TreatWarningsAsErrors = false
            };

            foreach (var assembly in RequiredAssemblies)
            {
                try
                {
                    parameters.ReferencedAssemblies.Add(assembly);
                }
                catch { }
            }

            var currentAssembly = Assembly.GetExecutingAssembly().Location;
            parameters.ReferencedAssemblies.Add(currentAssembly);

            var compilerResult = compiler.CompileAssemblyFromSource(parameters, fullCode);

            if (compilerResult.Errors.HasErrors)
            {
                result.Success = false;
                foreach (CompilerError error in compilerResult.Errors)
                {
                    result.Errors.Add($"行 {error.Line}: {error.ErrorText}");
                }
                result.ErrorMessage = string.Join("\n", result.Errors);
                return result;
            }

            try
            {
                var assembly = compilerResult.CompiledAssembly;
                var types = assembly.GetTypes();
                var algorithmType = types.FirstOrDefault(t => typeof(GraphicsAlgorithmBase).IsAssignableFrom(t));

                if (algorithmType == null)
                {
                    result.Success = false;
                    result.ErrorMessage = "找不到继承自 GraphicsAlgorithmBase 的类";
                    return result;
                }

                var instance = Activator.CreateInstance(algorithmType) as GraphicsAlgorithmBase;
                if (instance == null)
                {
                    result.Success = false;
                    result.ErrorMessage = "无法创建算法实例";
                    return result;
                }

                result.Success = true;
                result.Algorithm = instance;
            }
            catch (Exception ex)
            {
                result.Success = false;
                result.ErrorMessage = $"创建实例失败: {ex.Message}";
            }

            return result;
        }

        private static string GenerateFullCode(string userCode, string algorithmName)
        {
            return $@"
using System;
using System.Windows;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo.CustomAlgorithms
{{
    public class {algorithmName} : GraphicsAlgorithmBase
    {{
        public {algorithmName}()
        {{
            Name = ""自定义算法"";
            Description = ""用户自定义的图形学算法"";
            InitializeProperties();
        }}

        partial void InitializeProperties();

{userCode}
    }}
}}";
        }

        public static string GetTemplateCode(string templateName)
        {
            var templates = new Dictionary<string, string>
            {
                ["基础模板"] = BasicTemplate,
                ["直线算法"] = LineTemplate,
                ["圆算法"] = CircleTemplate,
                ["贝塞尔曲线"] = BezierTemplate,
                ["分形算法"] = FractalTemplate
            };

            return templates.TryGetValue(templateName, out var code) ? code : BasicTemplate;
        }

        private static readonly string BasicTemplate = @"
        partial void InitializeProperties()
        {
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Param1"",
                DisplayName = ""参数 1"",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Param2"",
                DisplayName = ""参数 2"",
                Value = 100.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = width / 2;
            double centerY = height / 2;
            
            double param1 = GetPropertyValue<double>(""Param1"");
            double param2 = GetPropertyValue<double>(""Param2"");

            var pen = new Pen(Brushes.Blue, 2);
            dc.DrawEllipse(null, pen, new Point(centerX, centerY), param1, param2);
        }

        public override string GetCode()
        {
            return ""// 用户自定义算法"";
        }
";

        private static readonly string LineTemplate = @"
        partial void InitializeProperties()
        {
            Properties.Add(new AlgorithmProperty
            {
                Name = ""X1"",
                DisplayName = ""起点 X"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Y1"",
                DisplayName = ""起点 Y"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""X2"",
                DisplayName = ""终点 X"",
                Value = 100.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Y2"",
                DisplayName = ""终点 Y"",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""LineWidth"",
                DisplayName = ""线宽"",
                Value = 2,
                ValueType = typeof(int),
                IsEnabled = true
            });
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = width / 2;
            double centerY = height / 2;
            
            double x1 = GetPropertyValue<double>(""X1"");
            double y1 = GetPropertyValue<double>(""Y1"");
            double x2 = GetPropertyValue<double>(""X2"");
            double y2 = GetPropertyValue<double>(""Y2"");
            int lineWidth = GetPropertyValue<int>(""LineWidth"");

            Point p1 = new Point(centerX + x1, centerY - y1);
            Point p2 = new Point(centerX + x2, centerY - y2);

            var pen = new Pen(Brushes.DarkRed, lineWidth);
            dc.DrawLine(pen, p1, p2);

            dc.DrawEllipse(Brushes.Red, null, p1, 4, 4);
            dc.DrawEllipse(Brushes.Red, null, p2, 4, 4);
        }

        public override string GetCode()
        {
            return ""// 自定义直线绘制算法"";
        }
";

        private static readonly string CircleTemplate = @"
        partial void InitializeProperties()
        {
            Properties.Add(new AlgorithmProperty
            {
                Name = ""CenterX"",
                DisplayName = ""圆心 X"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""CenterY"",
                DisplayName = ""圆心 Y"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Radius"",
                DisplayName = ""半径"",
                Value = 80.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""FillCircle"",
                DisplayName = ""填充"",
                Value = false,
                ValueType = typeof(bool),
                IsEnabled = true
            });
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = width / 2 + GetPropertyValue<double>(""CenterX"");
            double centerY = height / 2 - GetPropertyValue<double>(""CenterY"");
            double radius = GetPropertyValue<double>(""Radius"");
            bool fill = GetPropertyValue<bool>(""FillCircle"");

            var brush = Brushes.Green;
            var pen = new Pen(brush, 2);

            if (fill)
            {
                dc.DrawEllipse(brush, null, new Point(centerX, centerY), radius, radius);
            }
            else
            {
                dc.DrawEllipse(null, pen, new Point(centerX, centerY), radius, radius);
            }
        }

        public override string GetCode()
        {
            return ""// 自定义圆绘制算法"";
        }
";

        private static readonly string BezierTemplate = @"
        partial void InitializeProperties()
        {
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P0X"",
                DisplayName = ""P0 X"",
                Value = -100.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P0Y"",
                DisplayName = ""P0 Y"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P1X"",
                DisplayName = ""P1 X"",
                Value = -50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P1Y"",
                DisplayName = ""P1 Y"",
                Value = 80.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P2X"",
                DisplayName = ""P2 X"",
                Value = 50.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P2Y"",
                DisplayName = ""P2 Y"",
                Value = -80.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P3X"",
                DisplayName = ""P3 X"",
                Value = 100.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""P3Y"",
                DisplayName = ""P3 Y"",
                Value = 0.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = width / 2;
            double centerY = height / 2;
            
            Point p0 = new Point(centerX + GetPropertyValue<double>(""P0X""), centerY - GetPropertyValue<double>(""P0Y""));
            Point p1 = new Point(centerX + GetPropertyValue<double>(""P1X""), centerY - GetPropertyValue<double>(""P1Y""));
            Point p2 = new Point(centerX + GetPropertyValue<double>(""P2X""), centerY - GetPropertyValue<double>(""P2Y""));
            Point p3 = new Point(centerX + GetPropertyValue<double>(""P3X""), centerY - GetPropertyValue<double>(""P3Y""));

            var controlPen = new Pen(Brushes.Gray, 1);
            dc.DrawLine(controlPen, p0, p1);
            dc.DrawLine(controlPen, p1, p2);
            dc.DrawLine(controlPen, p2, p3);

            var path = new PathGeometry();
            var figure = new PathFigure();
            figure.StartPoint = p0;
            figure.Segments.Add(new BezierSegment(p1, p2, p3, true));
            path.Figures.Add(figure);
            dc.DrawGeometry(null, new Pen(Brushes.Purple, 3), path);

            DrawPoint(dc, p0, Brushes.Red);
            DrawPoint(dc, p1, Brushes.Green);
            DrawPoint(dc, p2, Brushes.Blue);
            DrawPoint(dc, p3, Brushes.Red);
        }

        private void DrawPoint(DrawingContext dc, Point p, Brush brush)
        {
            dc.DrawEllipse(brush, new Pen(Brushes.Black, 1), p, 5, 5);
        }

        public override string GetCode()
        {
            return ""// 自定义贝塞尔曲线算法"";
        }
";

        private static readonly string FractalTemplate = @"
        partial void InitializeProperties()
        {
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Depth"",
                DisplayName = ""递归深度"",
                Value = 5,
                ValueType = typeof(int),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Scale"",
                DisplayName = ""缩放"",
                Value = 150.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
            
            Properties.Add(new AlgorithmProperty
            {
                Name = ""Angle"",
                DisplayName = ""角度"",
                Value = 45.0,
                ValueType = typeof(double),
                IsEnabled = true
            });
        }

        public override void Render(DrawingContext dc, double width, double height)
        {
            double centerX = width / 2;
            double centerY = height / 2 + 100;
            
            int depth = GetPropertyValue<int>(""Depth"");
            double scale = GetPropertyValue<double>(""Scale"");
            double angle = GetPropertyValue<double>(""Angle"");

            var pen = new Pen(Brushes.DarkGreen, 1);
            DrawTree(dc, centerX, centerY, scale, -90, depth, pen);
        }

        private void DrawTree(DrawingContext dc, double x, double y, double length, double angle, int depth, Pen pen)
        {
            if (depth == 0 || length < 2) return;

            double rad = angle * Math.PI / 180;
            double x2 = x + length * Math.Cos(rad);
            double y2 = y + length * Math.Sin(rad);

            dc.DrawLine(pen, new Point(x, y), new Point(x2, y2));

            double newLength = length * 0.7;
            double angleOffset = GetPropertyValue<double>(""Angle"");
            
            DrawTree(dc, x2, y2, newLength, angle - angleOffset, depth - 1, pen);
            DrawTree(dc, x2, y2, newLength, angle + angleOffset, depth - 1, pen);
        }

        public override string GetCode()
        {
            return ""// 分形树绘制算法 - 递归实现"";
        }
";
    }
}
