using System;
using System.Runtime.InteropServices;
using System.Threading;

namespace FlappyBird
{
    public class SoundManager
    {
        [DllImport("kernel32.dll")]
        public static extern bool Beep(int freq, int duration);

        public void PlayJump()
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                try
                {
                    Beep(600, 80);
                }
                catch { }
            });
        }

        public void PlayScore()
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                try
                {
                    Beep(800, 100);
                    Thread.Sleep(50);
                    Beep(1000, 100);
                }
                catch { }
            });
        }

        public void PlayHit()
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                try
                {
                    Beep(200, 200);
                    Thread.Sleep(100);
                    Beep(150, 300);
                }
                catch { }
            });
        }
    }
}
