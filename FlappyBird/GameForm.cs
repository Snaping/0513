using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace FlappyBird
{
    public enum GameState
    {
        Guide,
        Playing,
        GameOver
    }

    public partial class GameForm : Form
    {
        private const int GameWidth = 400;
        private const int GameHeight = 600;

        private Bird bird;
        private List<Pipe> pipes;
        private List<Particle> particles;
        private Random random;
        private SoundManager soundManager;

        private System.Windows.Forms.Timer gameTimer;
        private GameState gameState;
        private int score;
        private int highScore;
        private float pipeSpeed;
        private float pipeSpawnTimer;
        private float pipeSpawnInterval;
        private float groundOffset;

        private Bitmap offScreenBitmap;
        private Graphics offScreenGraphics;

        public GameForm()
        {
            InitializeComponent();
            SetupGame();
        }

        private void InitializeComponent()
        {
            this.Text = "飞扬的小鸟";
            this.Size = new Size(GameWidth + 16, GameHeight + 39);
            this.MinimumSize = new Size(GameWidth + 16, GameHeight + 39);
            this.MaximumSize = new Size(GameWidth + 16, GameHeight + 39);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = true;
            this.DoubleBuffered = true;
            this.BackColor = Color.FromArgb(135, 206, 235);
            this.KeyPreview = true;

            this.KeyDown += GameForm_KeyDown;
            this.MouseDown += GameForm_MouseDown;
            this.Paint += GameForm_Paint;
            this.FormClosed += GameForm_FormClosed;
        }

        private void SetupGame()
        {
            random = new Random();
            soundManager = new SoundManager();

            bird = new Bird(80, GameHeight / 2);
            pipes = new List<Pipe>();
            particles = new List<Particle>();

            offScreenBitmap = new Bitmap(GameWidth, GameHeight);
            offScreenGraphics = Graphics.FromImage(offScreenBitmap);
            offScreenGraphics.SmoothingMode = SmoothingMode.AntiAlias;

            gameTimer = new System.Windows.Forms.Timer();
            gameTimer.Interval = 16;
            gameTimer.Tick += GameTimer_Tick;

            ResetGame();
            gameState = GameState.Guide;
            gameTimer.Start();
        }

        private void ResetGame()
        {
            bird.Reset(80, GameHeight / 2);
            pipes.Clear();
            particles.Clear();
            score = 0;
            pipeSpeed = 2.5f;
            pipeSpawnInterval = 100;
            pipeSpawnTimer = 50;
            groundOffset = 0;
        }

        private void StartGame()
        {
            ResetGame();
            gameState = GameState.Playing;
            bird.Jump();
            soundManager.PlayJump();
            SpawnParticles(bird.X + 10, bird.Y + 15, 10);
        }

        private void GameOver()
        {
            gameState = GameState.GameOver;
            if (score > highScore)
                highScore = score;
            soundManager.PlayHit();
            SpawnParticles(bird.X + 20, bird.Y + 15, 25);
        }

        private void SpawnParticles(float x, float y, int count)
        {
            for (int i = 0; i < count; i++)
            {
                particles.Add(new Particle(x, y, random));
            }
        }

        private void GameTimer_Tick(object sender, EventArgs e)
        {
            UpdateGame();
            Invalidate();
        }

        private void UpdateGame()
        {
            groundOffset = (groundOffset + pipeSpeed) % 30;

            for (int i = particles.Count - 1; i >= 0; i--)
            {
                particles[i].Update();
                if (particles[i].IsDead())
                    particles.RemoveAt(i);
            }

            if (gameState != GameState.Playing)
                return;

            bird.Update();

            if (bird.Y > GameHeight - 100 - bird.Height || bird.Y < 0)
            {
                GameOver();
                return;
            }

            pipeSpawnTimer++;
            if (pipeSpawnTimer >= pipeSpawnInterval)
            {
                pipes.Add(new Pipe(GameWidth + 50, GameHeight, random));
                pipeSpawnTimer = 0;
            }

            for (int i = pipes.Count - 1; i >= 0; i--)
            {
                pipes[i].Update(pipeSpeed);

                if (pipes[i].IsOffScreen())
                {
                    pipes.RemoveAt(i);
                    continue;
                }

                if (pipes[i].Intersects(bird.GetBounds()))
                {
                    GameOver();
                    return;
                }

                if (!pipes[i].Passed && pipes[i].X + pipes[i].Width < bird.X)
                {
                    pipes[i].Passed = true;
                    score++;
                    soundManager.PlayScore();
                    SpawnParticles(bird.X + 30, bird.Y + 15, 8);

                    if (score % 5 == 0)
                    {
                        pipeSpeed += 0.3f;
                        if (pipeSpeed > 5) pipeSpeed = 5;
                        if (pipeSpawnInterval > 70) pipeSpawnInterval -= 5;
                    }
                }
            }
        }

        private void GameForm_Paint(object sender, PaintEventArgs e)
        {
            DrawGame();
            e.Graphics.DrawImage(offScreenBitmap, 0, 0);
        }

        private void DrawGame()
        {
            offScreenGraphics.Clear(Color.FromArgb(135, 206, 235));

            DrawBackground();
            DrawGround();

            foreach (var pipe in pipes)
                pipe.Draw(offScreenGraphics, GameHeight);

            bird.Draw(offScreenGraphics);

            foreach (var particle in particles)
                particle.Draw(offScreenGraphics);

            DrawScore();

            if (gameState == GameState.Guide)
                DrawGuide();
            else if (gameState == GameState.GameOver)
                DrawGameOver();
        }

        private void DrawBackground()
        {
            DrawCloud(100, 80, 1);
            DrawCloud(250, 120, 0.8f);
            DrawCloud(50, 180, 0.6f);

            DrawHill(0, 350, 200, 80);
            DrawHill(150, 380, 250, 100);

            DrawTree(300, 380);
            DrawTree(100, 390);
        }

        private void DrawCloud(float x, float y, float scale)
        {
            using (SolidBrush brush = new SolidBrush(Color.FromArgb(220, 255, 255, 255)))
            {
                float s = scale;
                offScreenGraphics.FillEllipse(brush, x, y, 50 * s, 25 * s);
                offScreenGraphics.FillEllipse(brush, x + 20 * s, y - 10 * s, 40 * s, 30 * s);
                offScreenGraphics.FillEllipse(brush, x + 40 * s, y, 45 * s, 25 * s);
            }
        }

        private void DrawHill(float x, float y, float width, float height)
        {
            using (SolidBrush brush = new SolidBrush(Color.FromArgb(255, 144, 238, 144)))
            {
                offScreenGraphics.FillEllipse(brush, x - width / 2, y - height, width, height * 2);
            }
        }

        private void DrawTree(float x, float y)
        {
            using (SolidBrush trunkBrush = new SolidBrush(Color.FromArgb(255, 139, 69, 19)))
            {
                offScreenGraphics.FillRectangle(trunkBrush, x, y, 15, 30);
            }

            using (SolidBrush leafBrush = new SolidBrush(Color.FromArgb(255, 34, 139, 34)))
            {
                offScreenGraphics.FillEllipse(leafBrush, x - 20, y - 35, 55, 45);
            }
        }

        private void DrawGround()
        {
            int groundY = GameHeight - 100;

            using (SolidBrush grassBrush = new SolidBrush(Color.FromArgb(255, 76, 175, 80)))
            {
                offScreenGraphics.FillRectangle(grassBrush, 0, groundY, GameWidth, 20);
            }

            using (SolidBrush dirtBrush = new SolidBrush(Color.FromArgb(255, 139, 69, 19)))
            {
                offScreenGraphics.FillRectangle(dirtBrush, 0, groundY + 20, GameWidth, 80);
            }

            for (int x = -30 + (int)groundOffset; x < GameWidth + 30; x += 30)
            {
                using (Pen grassLinePen = new Pen(Color.FromArgb(255, 46, 125, 50), 2))
                {
                    offScreenGraphics.DrawLine(grassLinePen, x, groundY + 15, x + 5, groundY);
                    offScreenGraphics.DrawLine(grassLinePen, x + 10, groundY + 15, x + 15, groundY + 3);
                }

                using (Pen dirtLinePen = new Pen(Color.FromArgb(255, 101, 67, 33), 1))
                {
                    offScreenGraphics.DrawLine(dirtLinePen, x + 5, groundY + 40, x + 20, groundY + 45);
                    offScreenGraphics.DrawLine(dirtLinePen, x + 25, groundY + 70, x + 40, groundY + 65);
                }
            }
        }

        private void DrawScore()
        {
            string scoreText = score.ToString();
            using (Font font = new Font("Arial", 36, FontStyle.Bold))
            {
                SizeF size = offScreenGraphics.MeasureString(scoreText, font);
                float x = (GameWidth - size.Width) / 2;

                using (GraphicsPath path = new GraphicsPath())
                {
                    path.AddString(scoreText, font.FontFamily, (int)font.Style, font.Size,
                        new PointF(x + 3, 23), StringFormat.GenericDefault);
                    using (Pen outlinePen = new Pen(Color.FromArgb(255, 0, 0, 0), 4))
                    {
                        offScreenGraphics.DrawPath(outlinePen, path);
                    }
                }

                using (SolidBrush textBrush = new SolidBrush(Color.FromArgb(255, 255, 255, 255)))
                {
                    offScreenGraphics.DrawString(scoreText, font, textBrush, x, 20);
                }
            }
        }

        private void DrawGuide()
        {
            using (Font titleFont = new Font("Arial", 28, FontStyle.Bold))
            using (Font guideFont = new Font("Arial", 14, FontStyle.Regular))
            using (Font startFont = new Font("Arial", 18, FontStyle.Bold))
            {
                string title = "飞扬的小鸟";
                SizeF titleSize = offScreenGraphics.MeasureString(title, titleFont);
                offScreenGraphics.DrawString(title, titleFont,
                    new SolidBrush(Color.FromArgb(255, 255, 255, 255)),
                    (GameWidth - titleSize.Width) / 2, 100);

                string[] guideLines = new string[]
                {
                    "游戏规则：",
                    "点击屏幕或按空格键使小鸟上升",
                    "避开上下管道，通过间隙得分",
                    "每过5个管道，难度会提升！",
                    "",
                    "小心不要碰到管道或地面！"
                };

                float startY = 180;
                foreach (string line in guideLines)
                {
                    SizeF lineSize = offScreenGraphics.MeasureString(line, guideFont);
                    offScreenGraphics.DrawString(line, guideFont,
                        new SolidBrush(Color.FromArgb(255, 255, 255, 255)),
                        (GameWidth - lineSize.Width) / 2, startY);
                    startY += 28;
                }

                string startText = "点击屏幕或按空格开始";
                SizeF startSize = offScreenGraphics.MeasureString(startText, startFont);
                offScreenGraphics.DrawString(startText, startFont,
                    new SolidBrush(Color.FromArgb(255, 255, 230, 100)),
                    (GameWidth - startSize.Width) / 2, startY + 30);
            }
        }

        private void DrawGameOver()
        {
            using (Font gameOverFont = new Font("Arial", 32, FontStyle.Bold))
            using (Font scoreFont = new Font("Arial", 20, FontStyle.Bold))
            using (Font restartFont = new Font("Arial", 16, FontStyle.Regular))
            {
                string gameOverText = "游戏结束";
                SizeF gameOverSize = offScreenGraphics.MeasureString(gameOverText, gameOverFont);
                offScreenGraphics.DrawString(gameOverText, gameOverFont,
                    new SolidBrush(Color.FromArgb(255, 255, 100, 100)),
                    (GameWidth - gameOverSize.Width) / 2, 150);

                string scoreText = "得分: " + score;
                SizeF scoreSize = offScreenGraphics.MeasureString(scoreText, scoreFont);
                offScreenGraphics.DrawString(scoreText, scoreFont,
                    new SolidBrush(Color.FromArgb(255, 255, 255, 255)),
                    (GameWidth - scoreSize.Width) / 2, 220);

                string highScoreText = "最高分: " + highScore;
                SizeF highScoreSize = offScreenGraphics.MeasureString(highScoreText, scoreFont);
                offScreenGraphics.DrawString(highScoreText, scoreFont,
                    new SolidBrush(Color.FromArgb(255, 255, 230, 100)),
                    (GameWidth - highScoreSize.Width) / 2, 260);

                string restartText = "点击屏幕或按空格重新开始";
                SizeF restartSize = offScreenGraphics.MeasureString(restartText, restartFont);
                offScreenGraphics.DrawString(restartText, restartFont,
                    new SolidBrush(Color.FromArgb(255, 255, 255, 255)),
                    (GameWidth - restartSize.Width) / 2, 340);
            }
        }

        private void GameForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Space || e.KeyCode == Keys.Up)
            {
                HandleInput();
            }
        }

        private void GameForm_MouseDown(object sender, MouseEventArgs e)
        {
            HandleInput();
        }

        private void HandleInput()
        {
            if (gameState == GameState.Guide)
            {
                StartGame();
            }
            else if (gameState == GameState.Playing)
            {
                bird.Jump();
                soundManager.PlayJump();
                SpawnParticles(bird.X + 10, bird.Y + 15, 5);
            }
            else if (gameState == GameState.GameOver)
            {
                ResetGame();
                gameState = GameState.Guide;
            }
        }

        private void GameForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            gameTimer.Stop();
            offScreenGraphics.Dispose();
            offScreenBitmap.Dispose();
        }
    }
}
