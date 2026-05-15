#nullable disable

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

namespace MapCreater
{
    public partial class MainForm : Form
    {
        private Bitmap _mapImage;
        private byte[,] _gridData;
        private int _gridSize = 32;
        private Point _offset = Point.Empty;
        private Point _lastMousePos = Point.Empty;
        private bool _isDragging;
        private bool _isDrawing;
        private bool _drawModeBlock = true;
        private Rectangle _miniMapBounds;

        private MenuStrip _menuStrip;
        private ToolStrip _toolStrip;
        private Panel _mainPanel;
        private PictureBox _miniMapPictureBox;
        private NumericUpDown _gridSizeUpDown;
        private ToolStripButton _btnBlock;
        private ToolStripButton _btnPass;

        public MainForm()
        {
            InitializeComponent();
            SetStyle(ControlStyles.DoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint, true);
            UpdateStyles();
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();

            this.Text = "传奇地图生成工具";
            this.Size = new Size(1200, 800);
            this.StartPosition = FormStartPosition.CenterScreen;

            _menuStrip = new MenuStrip();
            var fileMenu = new ToolStripMenuItem("文件(&F)");
            fileMenu.DropDownItems.Add("导入图片(&I)", null, ImportImage_Click);
            fileMenu.DropDownItems.Add("导出数据(&E)", null, ExportData_Click);
            fileMenu.DropDownItems.Add("退出(&X)", null, (s, e) => Close());
            _menuStrip.Items.Add(fileMenu);

            var viewMenu = new ToolStripMenuItem("视图(&V)");
            viewMenu.DropDownItems.Add("重置视图(&R)", null, ResetView_Click);
            _menuStrip.Items.Add(viewMenu);

            this.MainMenuStrip = _menuStrip;
            this.Controls.Add(_menuStrip);

            _toolStrip = new ToolStrip();
            _toolStrip.GripStyle = ToolStripGripStyle.Hidden;

            _toolStrip.Items.Add(new ToolStripLabel("网格大小:"));
            _gridSizeUpDown = new NumericUpDown();
            _gridSizeUpDown.Minimum = 8;
            _gridSizeUpDown.Maximum = 128;
            _gridSizeUpDown.Value = 32;
            _gridSizeUpDown.Width = 60;
            _gridSizeUpDown.ValueChanged += GridSizeUpDown_ValueChanged;
            _toolStrip.Items.Add(new ToolStripControlHost(_gridSizeUpDown));

            _toolStrip.Items.Add(new ToolStripSeparator());

            _btnBlock = new ToolStripButton("绘制阻挡");
            _btnBlock.CheckOnClick = true;
            _btnBlock.Checked = true;
            _btnBlock.Click += (s, e) =>
            {
                _drawModeBlock = true;
                _btnPass.Checked = false;
            };
            _toolStrip.Items.Add(_btnBlock);

            _btnPass = new ToolStripButton("绘制可通过");
            _btnPass.CheckOnClick = true;
            _btnPass.Click += (s, e) =>
            {
                _drawModeBlock = false;
                _btnBlock.Checked = false;
            };
            _toolStrip.Items.Add(_btnPass);

            _toolStrip.Items.Add(new ToolStripSeparator());
            _toolStrip.Items.Add("左键拖动图片，右键绘制阻挡");

            this.Controls.Add(_toolStrip);

            _mainPanel = new DoubleBufferedPanel();
            _mainPanel.Dock = DockStyle.Fill;
            _mainPanel.BackColor = Color.Black;
            _mainPanel.MouseDown += MainPanel_MouseDown;
            _mainPanel.MouseMove += MainPanel_MouseMove;
            _mainPanel.MouseUp += MainPanel_MouseUp;
            _mainPanel.Paint += MainPanel_Paint;
            this.Controls.Add(_mainPanel);

            _miniMapPictureBox = new PictureBox();
            _miniMapPictureBox.Location = new Point(this.ClientSize.Width - 210, 30);
            _miniMapPictureBox.Size = new Size(200, 200);
            _miniMapPictureBox.BackColor = Color.FromArgb(64, 64, 64);
            _miniMapPictureBox.BorderStyle = BorderStyle.FixedSingle;
            _miniMapPictureBox.MouseDown += MiniMap_MouseDown;
            _miniMapPictureBox.MouseMove += MiniMap_MouseMove;
            _miniMapPictureBox.Paint += MiniMap_Paint;
            this.Controls.Add(_miniMapPictureBox);
            _miniMapPictureBox.BringToFront();

            _miniMapBounds = new Rectangle(0, 0, 200, 200);

            this.Resize += MainForm_Resize;

            this.ResumeLayout(false);
            this.PerformLayout();
        }

        private void MainForm_Resize(object? sender, EventArgs e)
        {
            if (_miniMapPictureBox != null)
            {
                _miniMapPictureBox.Location = new Point(this.ClientSize.Width - 210, 30);
            }
        }

        private void ImportImage_Click(object? sender, EventArgs e)
        {
            using var openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "PNG图片|*.png|所有文件|*.*";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    _mapImage = new Bitmap(openFileDialog.FileName);
                    InitializeGrid();
                    ResetView();
                    _mainPanel?.Invalidate();
                    _miniMapPictureBox?.Invalidate();
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"导入图片失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void InitializeGrid()
        {
            if (_mapImage == null) return;

            int cols = (_mapImage.Width + _gridSize - 1) / _gridSize;
            int rows = (_mapImage.Height + _gridSize - 1) / _gridSize;
            _gridData = new byte[rows, cols];
        }

        private void GridSizeUpDown_ValueChanged(object? sender, EventArgs e)
        {
            _gridSize = (int)(_gridSizeUpDown?.Value ?? 32);
            if (_mapImage != null)
            {
                InitializeGrid();
                _mainPanel?.Invalidate();
            }
        }

        private void ResetView_Click(object? sender, EventArgs e)
        {
            ResetView();
        }

        private void ResetView()
        {
            if (_mapImage == null || _mainPanel == null) return;

            float scaleX = (float)_mainPanel.ClientSize.Width / _mapImage.Width;
            float scaleY = (float)_mainPanel.ClientSize.Height / _mapImage.Height;
            float scale = Math.Min(scaleX, scaleY) * 0.9f;

            int scaledWidth = (int)(_mapImage.Width * scale);
            int scaledHeight = (int)(_mapImage.Height * scale);

            _offset = new Point(
                (_mainPanel.ClientSize.Width - scaledWidth) / 2,
                (_mainPanel.ClientSize.Height - scaledHeight) / 2
            );

            _mainPanel.Invalidate();
            _miniMapPictureBox?.Invalidate();
        }

        private void MainPanel_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                _isDragging = true;
                _lastMousePos = e.Location;
            }
            else if (e.Button == MouseButtons.Right)
            {
                _isDrawing = true;
                DrawAtPoint(e.Location);
            }
        }

        private void MainPanel_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isDragging)
            {
                _offset.X += e.X - _lastMousePos.X;
                _offset.Y += e.Y - _lastMousePos.Y;
                _lastMousePos = e.Location;
                _mainPanel.Invalidate();
                _miniMapPictureBox.Invalidate();
            }
            else if (_isDrawing)
            {
                DrawAtPoint(e.Location);
            }
        }

        private void MainPanel_MouseUp(object sender, MouseEventArgs e)
        {
            _isDragging = false;
            _isDrawing = false;
        }

        private void DrawAtPoint(Point screenPos)
        {
            if (_mapImage == null || _gridData == null || _mainPanel == null) return;

            float scale = GetScale();
            int imageX = (int)((screenPos.X - _offset.X) / scale);
            int imageY = (int)((screenPos.Y - _offset.Y) / scale);

            if (imageX < 0 || imageX >= _mapImage.Width || imageY < 0 || imageY >= _mapImage.Height)
                return;

            int col = imageX / _gridSize;
            int row = imageY / _gridSize;

            if (row >= 0 && row < _gridData.GetLength(0) && col >= 0 && col < _gridData.GetLength(1))
            {
                _gridData[row, col] = _drawModeBlock ? (byte)1 : (byte)0;
                _mainPanel.Invalidate();
                _miniMapPictureBox?.Invalidate();
            }
        }

        private float GetScale()
        {
            if (_mapImage == null || _mainPanel == null) return 1.0f;

            float scaleX = 1.0f;
            float scaleY = 1.0f;
            if (_mapImage.Width > 0)
                scaleX = (float)_mainPanel.ClientSize.Width / _mapImage.Width;
            if (_mapImage.Height > 0)
                scaleY = (float)_mainPanel.ClientSize.Height / _mapImage.Height;

            return Math.Min(scaleX, scaleY) * 0.9f;
        }

        private void MainPanel_Paint(object sender, PaintEventArgs e)
        {
            if (_mapImage == null || _mainPanel == null) return;

            var g = e.Graphics;
            g.Clear(Color.Black);

            float scale = GetScale();
            int scaledWidth = (int)(_mapImage.Width * scale);
            int scaledHeight = (int)(_mapImage.Height * scale);

            g.DrawImage(_mapImage, _offset.X, _offset.Y, scaledWidth, scaledHeight);

            if (_gridData != null)
            {
                using var passBrush = new SolidBrush(Color.FromArgb(80, Color.Green));
                using var blockBrush = new SolidBrush(Color.FromArgb(120, Color.Red));
                int gridScaledSize = (int)(_gridSize * scale);
                
                for (int row = 0; row < _gridData.GetLength(0); row++)
                {
                    for (int col = 0; col < _gridData.GetLength(1); col++)
                    {
                        int x = _offset.X + col * gridScaledSize;
                        int y = _offset.Y + row * gridScaledSize;
                        
                        if (_gridData[row, col] == 1)
                        {
                            g.FillRectangle(blockBrush, x, y, gridScaledSize, gridScaledSize);
                        }
                        else
                        {
                            g.FillRectangle(passBrush, x, y, gridScaledSize, gridScaledSize);
                        }
                    }
                }
            }

            int cols = (scaledWidth + (int)(_gridSize * scale) - 1) / (int)(_gridSize * scale);
            int rows = (scaledHeight + (int)(_gridSize * scale) - 1) / (int)(_gridSize * scale);

            using var gridPen = new Pen(Color.FromArgb(128, Color.Yellow));
            for (int i = 0; i <= cols; i++)
            {
                int x = _offset.X + i * (int)(_gridSize * scale);
                g.DrawLine(gridPen, x, _offset.Y, x, _offset.Y + scaledHeight);
            }
            for (int j = 0; j <= rows; j++)
            {
                int y = _offset.Y + j * (int)(_gridSize * scale);
                g.DrawLine(gridPen, _offset.X, y, _offset.X + scaledWidth, y);
            }
        }

        private void MiniMap_Paint(object sender, PaintEventArgs e)
        {
            var g = e.Graphics;
            g.Clear(Color.FromArgb(64, 64, 64));

            if (_mapImage == null) return;

            float scaleX = 180f / _mapImage.Width;
            float scaleY = 180f / _mapImage.Height;
            float scale = Math.Min(scaleX, scaleY);

            int miniWidth = (int)(_mapImage.Width * scale);
            int miniHeight = (int)(_mapImage.Height * scale);
            int offsetX = (200 - miniWidth) / 2;
            int offsetY = (200 - miniHeight) / 2;

            g.DrawImage(_mapImage, offsetX, offsetY, miniWidth, miniHeight);

            if (_gridData != null)
            {
                using var passBrush = new SolidBrush(Color.FromArgb(100, Color.Green));
                using var blockBrush = new SolidBrush(Color.FromArgb(200, Color.Red));
                int gridMiniSize = (int)(_gridSize * scale);
                for (int row = 0; row < _gridData.GetLength(0); row++)
                {
                    for (int col = 0; col < _gridData.GetLength(1); col++)
                    {
                        int x = offsetX + col * gridMiniSize;
                        int y = offsetY + row * gridMiniSize;
                        
                        if (_gridData[row, col] == 1)
                        {
                            g.FillRectangle(blockBrush, x, y, gridMiniSize, gridMiniSize);
                        }
                        else
                        {
                            g.FillRectangle(passBrush, x, y, gridMiniSize, gridMiniSize);
                        }
                    }
                }
            }

            if (_mainPanel != null)
            {
                float mainScale = GetScale();
                float viewScaleX = (float)miniWidth / (_mapImage.Width * mainScale);
                float viewScaleY = (float)miniHeight / (_mapImage.Height * mainScale);

                int viewX = offsetX + (int)(-_offset.X * viewScaleX);
                int viewY = offsetY + (int)(-_offset.Y * viewScaleY);
                int viewW = (int)(_mainPanel.ClientSize.Width * viewScaleX);
                int viewH = (int)(_mainPanel.ClientSize.Height * viewScaleY);

                using var viewPen = new Pen(Color.White, 2);
                g.DrawRectangle(viewPen, viewX, viewY, viewW, viewH);
            }
        }

        private void MiniMap_MouseDown(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                MiniMap_Scroll(e.Location);
            }
        }

        private void MiniMap_MouseMove(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                MiniMap_Scroll(e.Location);
            }
        }

        private void MiniMap_Scroll(Point miniPos)
        {
            if (_mapImage == null || _mainPanel == null) return;

            float scaleX = 180f / _mapImage.Width;
            float scaleY = 180f / _mapImage.Height;
            float scale = Math.Min(scaleX, scaleY);

            int miniWidth = (int)(_mapImage.Width * scale);
            int miniHeight = (int)(_mapImage.Height * scale);
            int offsetX = (200 - miniWidth) / 2;
            int offsetY = (200 - miniHeight) / 2;

            float relX = (miniPos.X - offsetX) / (float)miniWidth;
            float relY = (miniPos.Y - offsetY) / (float)miniHeight;

            relX = Math.Max(0, Math.Min(1, relX));
            relY = Math.Max(0, Math.Min(1, relY));

            float mainScale = GetScale();
            _offset.X = (int)(-_mapImage.Width * mainScale * relX + _mainPanel.ClientSize.Width / 2);
            _offset.Y = (int)(-_mapImage.Height * mainScale * relY + _mainPanel.ClientSize.Height / 2);

            _mainPanel.Invalidate();
            _miniMapPictureBox?.Invalidate();
        }

        private void ExportData_Click(object? sender, EventArgs e)
        {
            if (_mapImage == null || _gridData == null)
            {
                MessageBox.Show("请先导入图片并设置网格数据", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            using var saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "地图数据文件|*.map|二进制文件|*.bin|所有文件|*.*";
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    using var fs = new FileStream(saveFileDialog.FileName, FileMode.Create, FileAccess.Write);
                    using var bw = new BinaryWriter(fs);

                    int mapWidth = _gridData.GetLength(1);
                    int mapHeight = _gridData.GetLength(0);

                    bw.Write((short)mapWidth);
                    bw.Write((short)_mapImage.Height);

                    for (int row = 0; row < mapHeight; row++)
                    {
                        for (int col = 0; col < mapWidth; col++)
                        {
                            bw.Write(_gridData[row, col]);
                        }
                    }

                    MessageBox.Show($"导出成功!\n地图宽度(格子): {mapWidth}\n地图高度(像素): {_mapImage.Height}\n总格子数: {mapWidth * mapHeight}", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"导出失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
    }

    public class DoubleBufferedPanel : Panel
    {
        public DoubleBufferedPanel()
        {
            DoubleBuffered = true;
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.UserPaint, true);
        }
    }
}
