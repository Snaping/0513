
using System;
using System.Collections.Generic;
using System.Drawing;

namespace GraphicsDemo
{
    public static class GraphicsAlgorithms
    {
        public static List<Point> DDA(int x1, int y1, int x2, int y2)
        {
            List<Point> points = new List<Point>();
            int dx = x2 - x1;
            int dy = y2 - y1;
            int steps = Math.Max(Math.Abs(dx), Math.Abs(dy));
            
            if (steps == 0)
            {
                points.Add(new Point(x1, y1));
                return points;
            }
            
            float xIncrement = (float)dx / steps;
            float yIncrement = (float)dy / steps;
            
            float x = x1;
            float y = y1;
            
            for (int i = 0; i <= steps; i++)
            {
                points.Add(new Point((int)Math.Round(x), (int)Math.Round(y)));
                x += xIncrement;
                y += yIncrement;
            }
            
            return points;
        }

        public static List<Point> BresenhamLine(int x1, int y1, int x2, int y2)
        {
            List<Point> points = new List<Point>();
            int dx = Math.Abs(x2 - x1);
            int dy = Math.Abs(y2 - y1);
            int sx = x1 < x2 ? 1 : -1;
            int sy = y1 < y2 ? 1 : -1;
            int err = dx - dy;
            
            int x = x1;
            int y = y1;
            
            while (true)
            {
                points.Add(new Point(x, y));
                
                if (x == x2 && y == y2)
                    break;
                
                int e2 = 2 * err;
                
                if (e2 > -dy)
                {
                    err -= dy;
                    x += sx;
                }
                
                if (e2 < dx)
                {
                    err += dx;
                    y += sy;
                }
            }
            
            return points;
        }

        public static List<Point> MidpointCircle(int cx, int cy, int radius)
        {
            List<Point> points = new List<Point>();
            int x = 0;
            int y = radius;
            int p = 1 - radius;
            
            void PlotPoints()
            {
                points.Add(new Point(cx + x, cy + y));
                points.Add(new Point(cx - x, cy + y));
                points.Add(new Point(cx + x, cy - y));
                points.Add(new Point(cx - x, cy - y));
                points.Add(new Point(cx + y, cy + x));
                points.Add(new Point(cx - y, cy + x));
                points.Add(new Point(cx + y, cy - x));
                points.Add(new Point(cx - y, cy - x));
            }
            
            PlotPoints();
            
            while (x < y)
            {
                x++;
                
                if (p < 0)
                {
                    p += 2 * x + 1;
                }
                else
                {
                    y--;
                    p += 2 * (x - y) + 1;
                }
                
                PlotPoints();
            }
            
            return points;
        }

        public static List<Point> MidpointEllipse(int cx, int cy, int rx, int ry)
        {
            List<Point> points = new List<Point>();
            int rx2 = rx * rx;
            int ry2 = ry * ry;
            int twoRx2 = 2 * rx2;
            int twoRy2 = 2 * ry2;
            int x = 0;
            int y = ry;
            int p1 = ry2 - rx2 * ry + rx2 / 4;
            
            void PlotPoints()
            {
                points.Add(new Point(cx + x, cy + y));
                points.Add(new Point(cx - x, cy + y));
                points.Add(new Point(cx + x, cy - y));
                points.Add(new Point(cx - x, cy - y));
            }
            
            PlotPoints();
            
            while (twoRy2 * x < twoRx2 * y)
            {
                x++;
                
                if (p1 < 0)
                {
                    p1 += twoRy2 * x + ry2;
                }
                else
                {
                    y--;
                    p1 += twoRy2 * x - twoRx2 * y + ry2;
                }
                
                PlotPoints();
            }
            
            int p2 = (int)(ry2 * (x + 0.5f) * (x + 0.5f) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
            
            while (y > 0)
            {
                y--;
                
                if (p2 > 0)
                {
                    p2 += -twoRx2 * y + rx2;
                }
                else
                {
                    x++;
                    p2 += twoRy2 * x - twoRx2 * y + rx2;
                }
                
                PlotPoints();
            }
            
            return points;
        }

        public static List<Point> FloodFill(Bitmap bmp, int x, int y, Color targetColor, Color fillColor)
        {
            List<Point> filledPoints = new List<Point>();
            
            if (x < 0 || x >= bmp.Width || y < 0 || y >= bmp.Height)
                return filledPoints;
            
            Color currentColor = bmp.GetPixel(x, y);
            
            if (currentColor.ToArgb() == targetColor.ToArgb() || 
                currentColor.ToArgb() == fillColor.ToArgb())
                return filledPoints;
            
            Queue<Point> queue = new Queue<Point>();
            queue.Enqueue(new Point(x, y));
            
            while (queue.Count > 0)
            {
                Point p = queue.Dequeue();
                
                if (p.X < 0 || p.X >= bmp.Width || p.Y < 0 || p.Y >= bmp.Height)
                    continue;
                
                Color color = bmp.GetPixel(p.X, p.Y);
                
                if (color.ToArgb() != targetColor.ToArgb())
                    continue;
                
                bmp.SetPixel(p.X, p.Y, fillColor);
                filledPoints.Add(p);
                
                queue.Enqueue(new Point(p.X + 1, p.Y));
                queue.Enqueue(new Point(p.X - 1, p.Y));
                queue.Enqueue(new Point(p.X, p.Y + 1));
                queue.Enqueue(new Point(p.X, p.Y - 1));
            }
            
            return filledPoints;
        }

        public static List<Point> ScanlineFill(List<Point> polygon)
        {
            List<Point> filledPoints = new List<Point>();
            
            if (polygon.Count < 3)
                return filledPoints;
            
            int minY = int.MaxValue;
            int maxY = int.MinValue;
            
            foreach (Point p in polygon)
            {
                minY = Math.Min(minY, p.Y);
                maxY = Math.Max(maxY, p.Y);
            }
            
            for (int y = minY; y <= maxY; y++)
            {
                List<int> intersections = new List<int>();
                
                for (int i = 0; i < polygon.Count; i++)
                {
                    Point p1 = polygon[i];
                    Point p2 = polygon[(i + 1) % polygon.Count];
                    
                    if ((p1.Y <= y && p2.Y > y) || (p2.Y <= y && p1.Y > y))
                    {
                        float x = (float)(y - p1.Y) * (p2.X - p1.X) / (p2.Y - p1.Y) + p1.X;
                        intersections.Add((int)Math.Round(x));
                    }
                }
                
                intersections.Sort();
                
                for (int i = 0; i < intersections.Count; i += 2)
                {
                    if (i + 1 < intersections.Count)
                    {
                        for (int x = intersections[i]; x <= intersections[i + 1]; x++)
                        {
                            filledPoints.Add(new Point(x, y));
                        }
                    }
                }
            }
            
            return filledPoints;
        }

        public static List<Point> CohenSutherlandClip(int x1, int y1, int x2, int y2, int xmin, int ymin, int xmax, int ymax)
        {
            List<Point> points = new List<Point>();
            
            const int INSIDE = 0;
            const int LEFT = 1;
            const int RIGHT = 2;
            const int BOTTOM = 4;
            const int TOP = 8;
            
            int GetCode(int x, int y)
            {
                int code = INSIDE;
                
                if (x < xmin) code |= LEFT;
                else if (x > xmax) code |= RIGHT;
                if (y < ymin) code |= BOTTOM;
                else if (y > ymax) code |= TOP;
                
                return code;
            }
            
            int code1 = GetCode(x1, y1);
            int code2 = GetCode(x2, y2);
            bool accept = false;
            
            while (true)
            {
                if ((code1 | code2) == 0)
                {
                    accept = true;
                    break;
                }
                else if ((code1 & code2) != 0)
                {
                    break;
                }
                else
                {
                    int codeOut = code1 != 0 ? code1 : code2;
                    float x = 0, y = 0;
                    
                    if ((codeOut & TOP) != 0)
                    {
                        x = x1 + (x2 - x1) * (float)(ymax - y1) / (y2 - y1);
                        y = ymax;
                    }
                    else if ((codeOut & BOTTOM) != 0)
                    {
                        x = x1 + (x2 - x1) * (float)(ymin - y1) / (y2 - y1);
                        y = ymin;
                    }
                    else if ((codeOut & RIGHT) != 0)
                    {
                        y = y1 + (y2 - y1) * (float)(xmax - x1) / (x2 - x1);
                        x = xmax;
                    }
                    else if ((codeOut & LEFT) != 0)
                    {
                        y = y1 + (y2 - y1) * (float)(xmin - x1) / (x2 - x1);
                        x = xmin;
                    }
                    
                    if (codeOut == code1)
                    {
                        x1 = (int)x;
                        y1 = (int)y;
                        code1 = GetCode(x1, y1);
                    }
                    else
                    {
                        x2 = (int)x;
                        y2 = (int)y;
                        code2 = GetCode(x2, y2);
                    }
                }
            }
            
            if (accept)
            {
                points.Add(new Point(x1, y1));
                points.Add(new Point(x2, y2));
            }
            
            return points;
        }
    }
}
