using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing;

namespace MyUiVerify
{
    public static class ScreenElementHighlighter
    {
        [DllImport("User32.dll")]
        private static extern IntPtr GetDC(IntPtr hwnd);

        [DllImport("User32.dll")]
        private static extern int ReleaseDC(IntPtr hwnd, IntPtr dc);
        
        public static void HighlightArea(Rectangle rect)
        {
            IntPtr desktopPtr = GetDC(IntPtr.Zero);
            using (Graphics g = Graphics.FromHdc(desktopPtr))
            {
                g.DrawRectangle(new Pen(Brushes.Red, 2.0f), rect);
            }
            ReleaseDC(IntPtr.Zero, desktopPtr);
        }
    }
}
