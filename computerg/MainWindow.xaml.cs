using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;
using ComputerGraphicsDemo.Algorithms;
using ComputerGraphicsDemo.Services;

namespace ComputerGraphicsDemo
{
    public partial class MainWindow : Window
    {
        private GraphicsAlgorithmBase? _customAlgorithm;
        private string _originalCustomCode = string.Empty;
        private bool _isCustomMode = false;

        public MainWindow()
        {
            InitializeComponent();
            
            AlgorithmComboBox.Items.Add(new LineAlgorithm());
            AlgorithmComboBox.Items.Add(new CircleAlgorithm());
            AlgorithmComboBox.Items.Add(new EllipseAlgorithm());
            AlgorithmComboBox.Items.Add(new BezierAlgorithm());
            AlgorithmComboBox.Items.Add(new TransformAlgorithm());
            
            AlgorithmComboBox.SelectedIndex = 0;
            
            _originalCustomCode = CodeCompiler.GetTemplateCode("基础模板");
            CustomCodeTextBox.Text = _originalCustomCode;
        }

        private void RefreshRender_Click(object sender, RoutedEventArgs e)
        {
            RenderCanvas.InvalidateVisual();
        }

        private void ClearCanvas_Click(object sender, RoutedEventArgs e)
        {
            RenderCanvas.InvalidateVisual();
        }

        private void ExportCode_Click(object sender, RoutedEventArgs e)
        {
            GraphicsAlgorithmBase? algorithm = null;
            
            if (_isCustomMode && _customAlgorithm != null)
            {
                algorithm = _customAlgorithm;
            }
            else if (AlgorithmComboBox.SelectedItem is GraphicsAlgorithmBase preset)
            {
                algorithm = preset;
            }

            if (algorithm != null)
            {
                var dialog = new Microsoft.Win32.SaveFileDialog
                {
                    Filter = "C# Files (*.cs)|*.cs|Text Files (*.txt)|*.txt|All Files (*.*)|*.*",
                    Title = "导出算法代码",
                    FileName = $"{algorithm.Name}.cs"
                };

                if (dialog.ShowDialog() == true)
                {
                    System.IO.File.WriteAllText(dialog.FileName, algorithm.GetCode());
                    MessageBox.Show("代码导出成功！", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
                }
            }
            else
            {
                MessageBox.Show("请先选择或编译一个算法！", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
        }

        private void Algorithm_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (AlgorithmComboBox.SelectedItem is GraphicsAlgorithmBase algorithm)
            {
                AlgorithmDescription.Text = algorithm.Description;
                CodeTextBox.Text = algorithm.GetCode();
                RenderCanvas.Algorithm = algorithm;
                BuildPropertiesPanel(algorithm);
            }
        }

        private void AlgorithmMode_Checked(object sender, RoutedEventArgs e)
        {
            _isCustomMode = rbCustom.IsChecked == true;
            
            if (_isCustomMode)
            {
                CodeEditorTitle.Text = "💻 自定义算法编辑器";
                btnEditMode.Visibility = Visibility.Visible;
                CustomEditorOverlay.Visibility = Visibility.Visible;
                CompileStatusGroup.Visibility = Visibility.Visible;
                CodeTextBox.Visibility = Visibility.Collapsed;
                AlgorithmDescription.Text = "在此编写您自己的图形学算法代码，点击「编译运行」即可预览效果。";
                
                if (_customAlgorithm != null)
                {
                    BuildPropertiesPanel(_customAlgorithm);
                }
                else
                {
                    PropertiesPanel.Children.Clear();
                }
            }
            else
            {
                CodeEditorTitle.Text = "💻 算法代码 (只读)";
                btnEditMode.Visibility = Visibility.Collapsed;
                CustomEditorOverlay.Visibility = Visibility.Collapsed;
                CompileStatusGroup.Visibility = Visibility.Collapsed;
                CodeTextBox.Visibility = Visibility.Visible;
                
                if (AlgorithmComboBox.SelectedItem is GraphicsAlgorithmBase algorithm)
                {
                    AlgorithmDescription.Text = algorithm.Description;
                    CodeTextBox.Text = algorithm.GetCode();
                    RenderCanvas.Algorithm = algorithm;
                    BuildPropertiesPanel(algorithm);
                }
            }
        }

        private void EditMode_Click(object sender, RoutedEventArgs e)
        {
            CustomEditorOverlay.Visibility = Visibility.Visible;
            CodeTextBox.Visibility = Visibility.Collapsed;
        }

        private void ApplyCustomCode_Click(object sender, RoutedEventArgs e)
        {
            CompileAndRun_Click(sender, e);
        }

        private void ResetCustomCode_Click(object sender, RoutedEventArgs e)
        {
            var result = MessageBox.Show("确定要重置为默认模板代码吗？当前修改将丢失。", 
                "确认重置", MessageBoxButton.YesNo, MessageBoxImage.Question);
            
            if (result == MessageBoxResult.Yes)
            {
                CustomCodeTextBox.Text = _originalCustomCode;
            }
        }

        private void CompileAndRun_Click(object sender, RoutedEventArgs e)
        {
            if (!_isCustomMode)
            {
                MessageBox.Show("请先切换到「自定义算法」模式！", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
                rbCustom.IsChecked = true;
                return;
            }

            string code = CustomCodeTextBox.Text;
            
            if (string.IsNullOrWhiteSpace(code))
            {
                MessageBox.Show("请输入算法代码！", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            CompileStatusText.Text = "⏳ 正在编译...";
            CompileStatusText.Foreground = Brushes.Orange;
            ErrorScrollViewer.Visibility = Visibility.Collapsed;

            var result = CodeCompiler.CompileAlgorithm(code, "UserCustomAlgorithm");

            if (result.Success && result.Algorithm != null)
            {
                _customAlgorithm = result.Algorithm;
                _customAlgorithm.Name = "自定义算法";
                _customAlgorithm.Description = "用户自定义的图形学算法";
                
                CompileStatusText.Text = "✅ 编译成功！算法已加载";
                CompileStatusText.Foreground = Brushes.Green;
                
                RenderCanvas.Algorithm = _customAlgorithm;
                BuildPropertiesPanel(_customAlgorithm);
                AlgorithmDescription.Text = _customAlgorithm.Description;
                
                RenderCanvas.InvalidateVisual();
            }
            else
            {
                CompileStatusText.Text = "❌ 编译失败";
                CompileStatusText.Foreground = Brushes.Red;
                ErrorScrollViewer.Visibility = Visibility.Visible;
                ErrorTextBox.Text = result.ErrorMessage ?? string.Join("\n", result.Errors);
                
                MessageBox.Show("编译失败，请检查代码！\n\n" + (result.ErrorMessage ?? string.Join("\n", result.Errors)), 
                    "编译错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void LoadTemplate_Click(object sender, RoutedEventArgs e)
        {
            var window = new Window
            {
                Title = "选择算法模板",
                Width = 400,
                Height = 350,
                WindowStartupLocation = WindowStartupLocation.CenterOwner,
                Owner = this
            };

            var stackPanel = new StackPanel { Margin = new Thickness(20) };
            
            var textBlock = new TextBlock
            {
                Text = "选择一个预设模板：",
                Margin = new Thickness(0, 0, 0, 15)
            };
            stackPanel.Children.Add(textBlock);

            var templates = new[]
            {
                "基础模板",
                "直线算法",
                "圆算法",
                "贝塞尔曲线",
                "分形算法"
            };

            var listBox = new ListBox();
            foreach (var template in templates)
            {
                listBox.Items.Add(template);
            }
            listBox.SelectedIndex = 0;
            listBox.Margin = new Thickness(0, 0, 0, 15);
            stackPanel.Children.Add(listBox);

            var buttonPanel = new StackPanel { Orientation = Orientation.Horizontal, HorizontalAlignment = HorizontalAlignment.Right };
            
            var cancelButton = new Button { Content = "取消", Margin = new Thickness(5), Padding = new Thickness(15, 5, 15, 5) };
            cancelButton.Click += (s, args) => window.Close();
            
            var okButton = new Button { Content = "加载", Background = Brushes.Green, Foreground = Brushes.White, Margin = new Thickness(5), Padding = new Thickness(15, 5, 15, 5) };
            okButton.Click += (s, args) =>
            {
                if (listBox.SelectedItem is string selectedTemplate)
                {
                    if (_isCustomMode)
                    {
                        var result = MessageBox.Show($"加载「{selectedTemplate}」会覆盖当前代码，确定继续吗？", 
                            "确认加载", MessageBoxButton.YesNo, MessageBoxImage.Question);
                        
                        if (result == MessageBoxResult.Yes)
                        {
                            CustomCodeTextBox.Text = CodeCompiler.GetTemplateCode(selectedTemplate);
                            _originalCustomCode = CustomCodeTextBox.Text;
                            window.Close();
                        }
                    }
                    else
                    {
                        CustomCodeTextBox.Text = CodeCompiler.GetTemplateCode(selectedTemplate);
                        _originalCustomCode = CustomCodeTextBox.Text;
                        rbCustom.IsChecked = true;
                        window.Close();
                    }
                }
            };

            buttonPanel.Children.Add(cancelButton);
            buttonPanel.Children.Add(okButton);
            stackPanel.Children.Add(buttonPanel);

            window.Content = stackPanel;
            window.ShowDialog();
        }

        private void BuildPropertiesPanel(GraphicsAlgorithmBase algorithm)
        {
            PropertiesPanel.Children.Clear();

            foreach (var prop in algorithm.Properties)
            {
                var border = new Border
                {
                    BorderBrush = new SolidColorBrush(Color.FromRgb(238, 238, 238)),
                    BorderThickness = new Thickness(0, 0, 0, 1),
                    Padding = new Thickness(4)
                };

                var grid = new Grid();
                grid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });
                grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });

                var checkBox = new CheckBox
                {
                    Content = prop.DisplayName,
                    IsChecked = prop.IsEnabled,
                    VerticalAlignment = VerticalAlignment.Center,
                    Margin = new Thickness(0, 0, 8, 0)
                };

                checkBox.Checked += (s, args) =>
                {
                    prop.IsEnabled = true;
                    RenderCanvas.InvalidateVisual();
                };

                checkBox.Unchecked += (s, args) =>
                {
                    prop.IsEnabled = false;
                    RenderCanvas.InvalidateVisual();
                };

                Grid.SetColumn(checkBox, 0);
                grid.Children.Add(checkBox);

                FrameworkElement? valueControl = null;

                if (prop.ValueType == typeof(double))
                {
                    var slider = new Slider
                    {
                        Minimum = -200,
                        Maximum = 200,
                        Value = (double)(prop.Value ?? 0),
                        IsEnabled = prop.IsEnabled,
                        TickFrequency = 5,
                        IsSnapToTickEnabled = true,
                        VerticalAlignment = VerticalAlignment.Center
                    };
                    slider.ValueChanged += (s, args) =>
                    {
                        prop.Value = args.NewValue;
                        RenderCanvas.InvalidateVisual();
                    };
                    valueControl = slider;
                }
                else if (prop.ValueType == typeof(int))
                {
                    var slider = new Slider
                    {
                        Minimum = 1,
                        Maximum = 20,
                        Value = (int)(prop.Value ?? 1),
                        IsEnabled = prop.IsEnabled,
                        TickFrequency = 1,
                        IsSnapToTickEnabled = true,
                        VerticalAlignment = VerticalAlignment.Center
                    };
                    slider.ValueChanged += (s, args) =>
                    {
                        prop.Value = (int)args.NewValue;
                        RenderCanvas.InvalidateVisual();
                    };
                    valueControl = slider;
                }
                else if (prop.ValueType == typeof(bool))
                {
                    var cb = new CheckBox
                    {
                        Content = "启用",
                        IsChecked = (bool)(prop.Value ?? false),
                        IsEnabled = prop.IsEnabled,
                        VerticalAlignment = VerticalAlignment.Center
                    };
                    cb.Checked += (s, args) =>
                    {
                        prop.Value = true;
                        RenderCanvas.InvalidateVisual();
                    };
                    cb.Unchecked += (s, args) =>
                    {
                        prop.Value = false;
                        RenderCanvas.InvalidateVisual();
                    };
                    valueControl = cb;
                }

                if (valueControl != null)
                {
                    Grid.SetColumn(valueControl, 1);
                    grid.Children.Add(valueControl);
                }

                border.Child = grid;
                PropertiesPanel.Children.Add(border);
            }
        }

        private void RenderCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            RenderCanvas.InvalidateVisual();
        }
    }
}
