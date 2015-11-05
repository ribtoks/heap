using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing;
using System.Windows;

namespace MyUiVerify
{
    class HighlightWindow : Window
    {
        public HighlightWindow() :
            base()
        {
            this.WindowStyle = WindowStyle.None;
            this.ShowInTaskbar = false;
            this.AllowsTransparency = true;
            this.Topmost = true;
            this.Background = System.Windows.Media.Brushes.Transparent;
            this.Width = 0;
            this.Height = 0;
        }

        protected override void OnRender(System.Windows.Media.DrawingContext drawingContext)
        {
            System.Windows.Media.Matrix m = PresentationSource.FromVisual(this)
                .CompositionTarget.TransformToDevice;
            double dpiFactor = 1 / m.M11;

            var pen = new System.Windows.Media.Pen(System.Windows.Media.Brushes.Red, 4.0*dpiFactor);
            var rect = new Rect(0, 0, this.Width, this.Height);
            drawingContext.DrawRectangle(System.Windows.Media.Brushes.Transparent, pen, rect);
        }
    }

    public static class ScreenElementHighlighter
    {
        private static HighlightWindow hlWindow = new HighlightWindow();

        [DllImport("User32.dll")]
        private static extern IntPtr GetDC(IntPtr hwnd);

        [DllImport("User32.dll")]
        private static extern int ReleaseDC(IntPtr hwnd, IntPtr dc);
        
        public static void HighlightArea(Rectangle rect)
        {
            /*IntPtr desktopPtr = GetDC(IntPtr.Zero);
            using (Graphics g = Graphics.FromHdc(desktopPtr))
            {
                g.DrawRectangle(new Pen(Brushes.Red, 2.0f), rect);
            }
            ReleaseDC(IntPtr.Zero, desktopPtr);
            */

            hlWindow.Top = rect.Top;
            hlWindow.Left = rect.Left;
            hlWindow.Width = rect.Width;
            hlWindow.Height = rect.Height;

            hlWindow.Show();
        }

        public static void ClearHighlight()
        {
            hlWindow.Close();
        }
    }
}
