using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using ComputerGraphicsDemo.Models;

namespace ComputerGraphicsDemo
{
    public class RenderCanvas : Canvas
    {
        public static readonly DependencyProperty AlgorithmProperty =
            DependencyProperty.Register(
                nameof(Algorithm),
                typeof(GraphicsAlgorithmBase),
                typeof(RenderCanvas),
                new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender, OnAlgorithmChanged));

        public GraphicsAlgorithmBase? Algorithm
        {
            get => (GraphicsAlgorithmBase?)GetValue(AlgorithmProperty);
            set => SetValue(AlgorithmProperty, value);
        }

        private static void OnAlgorithmChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (e.OldValue is GraphicsAlgorithmBase oldAlg)
            {
                oldAlg.PropertyChanged -= (s, args) => ((RenderCanvas)d).InvalidateVisual();
            }

            if (e.NewValue is GraphicsAlgorithmBase newAlg)
            {
                newAlg.PropertyChanged += (s, args) =>
                {
                    if (args.PropertyName == nameof(GraphicsAlgorithmBase.Properties))
                    {
                        ((RenderCanvas)d).InvalidateVisual();
                    }
                };
            }
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);

            dc.DrawRectangle(Brushes.White, null, new Rect(0, 0, ActualWidth, ActualHeight));

            DrawGrid(dc);

            if (Algorithm != null)
            {
                Algorithm.Render(dc, ActualWidth, ActualHeight);
            }
        }

        private void DrawGrid(DrawingContext dc)
        {
            var gridPen = new Pen(Brushes.LightGray, 0.5);
            double gridSize = 20;

            for (double x = 0; x < ActualWidth; x += gridSize)
            {
                dc.DrawLine(gridPen, new Point(x, 0), new Point(x, ActualHeight));
            }

            for (double y = 0; y < ActualHeight; y += gridSize)
            {
                dc.DrawLine(gridPen, new Point(0, y), new Point(ActualWidth, y));
            }

            var axisPen = new Pen(Brushes.DarkGray, 1);
            double centerX = ActualWidth / 2;
            double centerY = ActualHeight / 2;

            dc.DrawLine(axisPen, new Point(centerX, 0), new Point(centerX, ActualHeight));
            dc.DrawLine(axisPen, new Point(0, centerY), new Point(ActualWidth, centerY));
        }
    }
}