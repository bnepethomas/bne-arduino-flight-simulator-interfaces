using System.Drawing.Drawing2D;

namespace JetRangerPanelEmulator
{
    // What one round dial looks like: the same faces as the instrument map (the scale, the numerals and the legend of the real gauge).
    public sealed class DialSpec
    {
        public string Token = "";
        public string[] Legend = Array.Empty<string>();
        public double Min, Max;
        public float A0 = -135, A1 = 135;          // degrees clockwise from straight up at Min and at Max
        public double Major = 1;
        public int Minor = 1;
        public Func<double, string>? Label;
        public double Rest;                        // where the needle goes when the board has no data (the calibrated zero)
        public float LegendTop = 0.58f;            // where the legend starts, as a fraction of the radius below the centre
    }

    public static class Gfx
    {
        public static readonly Color Amber = Color.FromArgb(255, 179, 0), Red = Color.FromArgb(242, 51, 31), Green = Color.FromArgb(63, 209, 63),
                                     Blue = Color.FromArgb(60, 140, 255), White = Color.FromArgb(240, 240, 240);

        public static Color Mix(Color a, Color b, double t)
        {
            t = Math.Clamp(t, 0, 1);
            return Color.FromArgb((int)(a.R + (b.R - a.R) * t), (int)(a.G + (b.G - a.G) * t), (int)(a.B + (b.B - a.B) * t));
        }

        public static PointF Polar(float cx, float cy, float r, double deg)
        {
            double a = deg * Math.PI / 180.0;
            return new PointF(cx + r * (float)Math.Sin(a), cy - r * (float)Math.Cos(a));
        }

        public static void Smooth(Graphics g)
        {
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
        }

        static void Text(Graphics g, string s, float x, float y, float size, Color c, bool bold = false, string family = "Segoe UI")
        {
            using var f = new Font(family, size, bold ? FontStyle.Bold : FontStyle.Regular, GraphicsUnit.Pixel);
            using var b = new SolidBrush(c);
            var sf = new StringFormat { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center };
            g.DrawString(s, f, b, x, y, sf);
        }

        // One round gauge. light is the backlight (0 = dark panel, 1 = full): it dims the scale and the legend, the needle stays visible.
        public static void Dial(Graphics g, float cx, float cy, float r, DialSpec s, double value, double light)
        {
            var bezel = new RectangleF(cx - r, cy - r, 2 * r, 2 * r);
            using (var br = new LinearGradientBrush(bezel, Color.FromArgb(240, 240, 238), Color.FromArgb(95, 95, 92), 45f)) g.FillEllipse(br, bezel);
            using (var br = new SolidBrush(Color.FromArgb(34, 34, 34))) g.FillEllipse(br, cx - r * 0.9f, cy - r * 0.9f, r * 1.8f, r * 1.8f);
            using (var br = new SolidBrush(Color.FromArgb(14, 14, 14))) g.FillEllipse(br, cx - r * 0.86f, cy - r * 0.86f, r * 1.72f, r * 1.72f);

            double Ang(double v) => s.A0 + (s.A1 - s.A0) * (v - s.Min) / (s.Max - s.Min);
            Color scale = Mix(Color.FromArgb(62, 62, 62), Color.FromArgb(245, 245, 245), light);
            Color minor = Mix(Color.FromArgb(52, 52, 52), Color.FromArgb(216, 216, 216), light);
            int n = (int)Math.Round((s.Max - s.Min) / s.Major);
            using var major = new Pen(scale, Math.Max(1f, r * 0.035f));
            using var minorPen = new Pen(minor, Math.Max(0.8f, r * 0.018f));
            for (int i = 0; i <= n; i++)
            {
                double v = s.Min + i * s.Major;
                g.DrawLine(major, Polar(cx, cy, r * 0.78f, Ang(v)), Polar(cx, cy, r * 0.64f, Ang(v)));
                if (i < n)
                    for (int k = 1; k < s.Minor; k++)
                    {
                        double vm = v + k * s.Major / s.Minor;
                        g.DrawLine(minorPen, Polar(cx, cy, r * 0.78f, Ang(vm)), Polar(cx, cy, r * 0.71f, Ang(vm)));
                    }
                string lab = s.Label != null ? s.Label(v) : "";
                if (lab.Length > 0)
                {
                    var p = Polar(cx, cy, r * 0.52f, Ang(v));
                    Text(g, lab, p.X, p.Y, r * 0.19f, scale, true);
                }
            }
            for (int i = 0; i < s.Legend.Length; i++)
                Text(g, s.Legend[i], cx, cy + r * (s.LegendTop + 0.17f * i), r * (i == 0 ? 0.14f : 0.12f), i == 0 ? scale : minor, i == 0);

            double na = Ang(Math.Clamp(value, Math.Min(s.Min, s.Max), Math.Max(s.Min, s.Max)));
            var tip = Polar(cx, cy, r * 0.74f, na);
            var tail = Polar(cx, cy, r * 0.16f, na + 180);
            var left = Polar(cx, cy, r * 0.045f, na - 90);
            var right = Polar(cx, cy, r * 0.045f, na + 90);
            using (var br = new SolidBrush(Mix(Color.FromArgb(170, 170, 170), Color.FromArgb(247, 247, 247), 0.35 + 0.65 * light)))
            using (var pen = new Pen(Color.FromArgb(34, 34, 34), 0.8f))
            {
                var pts = new[] { tip, right, tail, left };
                g.FillPolygon(br, pts);
                g.DrawPolygon(pen, pts);
            }
            using (var br = new SolidBrush(Color.FromArgb(200, 200, 200))) g.FillEllipse(br, cx - r * 0.09f, cy - r * 0.09f, r * 0.18f, r * 0.18f);
            using (var br = new SolidBrush(Color.FromArgb(9, 255, 255, 255))) g.FillEllipse(br, cx - r * 0.7f, cy - r * 0.62f, r * 0.9f, r * 0.55f);
        }

        // The attitude indicator: pitch in degrees (positive nose up moves the horizon down), bank in degrees (positive right wing down turns the horizon the other way).
        public static void Attitude(Graphics g, float cx, float cy, float r, double pitch, double bank, double light)
        {
            var bezel = new RectangleF(cx - r, cy - r, 2 * r, 2 * r);
            using (var br = new LinearGradientBrush(bezel, Color.FromArgb(240, 240, 238), Color.FromArgb(95, 95, 92), 45f)) g.FillEllipse(br, bezel);
            using (var br = new SolidBrush(Color.FromArgb(17, 17, 17))) g.FillEllipse(br, cx - r * 0.9f, cy - r * 0.9f, r * 1.8f, r * 1.8f);
            var state = g.Save();
            using (var clip = new GraphicsPath())
            {
                clip.AddEllipse(cx - r * 0.84f, cy - r * 0.84f, r * 1.68f, r * 1.68f);
                g.SetClip(clip);
            }
            g.TranslateTransform(cx, cy);
            g.RotateTransform((float)-bank);
            g.TranslateTransform(0, (float)(pitch * r * 0.02 * 2.4));
            using (var sky = new LinearGradientBrush(new RectangleF(-r * 2, -r * 2, r * 4, r * 2), Color.FromArgb(40, 110, 200), Color.FromArgb(150, 200, 245), 90f)) g.FillRectangle(sky, -r * 2, -r * 2, r * 4, r * 2);
            using (var gnd = new LinearGradientBrush(new RectangleF(-r * 2, 0, r * 4, r * 2), Color.FromArgb(150, 100, 50), Color.FromArgb(70, 45, 20), 90f)) g.FillRectangle(gnd, -r * 2, 0, r * 4, r * 2);
            using (var pen = new Pen(Color.White, 1.6f))
            {
                g.DrawLine(pen, -r * 2, 0, r * 2, 0);
                foreach (int k in new[] { -20, -10, 10, 20 })
                {
                    float y = -(float)(k * r * 0.02 * 2.4);
                    float wd = r * (Math.Abs(k) == 10 ? 0.28f : 0.4f);
                    g.DrawLine(pen, -wd, y, wd, y);
                }
            }
            g.Restore(state);
            using (var pen = new Pen(Color.White, 1.2f))
                foreach (int d in new[] { -60, -45, -30, -20, -10, 0, 10, 20, 30, 45, 60 })
                    g.DrawLine(pen, Polar(cx, cy, r * 0.84f, d), Polar(cx, cy, r * (d % 30 == 0 ? 0.74f : 0.78f), d));
            var tri = new[] { Polar(cx, cy, r * 0.74f, -bank), Polar(cx, cy, r * 0.86f, -5 - bank), Polar(cx, cy, r * 0.86f, 5 - bank) };
            using (var br = new SolidBrush(Color.FromArgb(255, 212, 59))) g.FillPolygon(br, tri);
            using (var pen = new Pen(Color.FromArgb(255, 196, 0), Math.Max(2f, r * 0.03f)) { StartCap = LineCap.Round, EndCap = LineCap.Round })
            {
                g.DrawLine(pen, cx - r * 0.62f, cy, cx - r * 0.28f, cy);
                g.DrawLine(pen, cx + r * 0.28f, cy, cx + r * 0.62f, cy);
                g.DrawLine(pen, cx, cy - r * 0.04f, cx, cy + r * 0.08f);
            }
            if (light < 0.99)
                using (var br = new SolidBrush(Color.FromArgb((int)((1 - light) * 90), 0, 0, 0))) g.FillEllipse(br, cx - r * 0.84f, cy - r * 0.84f, r * 1.68f, r * 1.68f);
            using (var br = new SolidBrush(Color.FromArgb(12, 255, 255, 255))) g.FillEllipse(br, cx - r * 0.7f, cy - r * 0.7f, r * 0.9f, r * 0.5f);
        }

        // A caution lamp (legend of one or two lines): lit it shows its colour, dark it only shows the legend, and only while the panel backlight is on.
        public static void Lamp(Graphics g, RectangleF r, string line1, string line2, Color colour, bool on, double light, float fixedSize = 0)
        {
            using var path = RoundRect(r, Math.Min(r.Height * 0.2f, 8f));
            Color fill = on ? colour : Mix(Color.FromArgb(28, 28, 28), Mix(colour, Color.Black, 0.8), 0.45);
            if (on)
                using (var glow = new SolidBrush(Color.FromArgb(55, colour))) g.FillRectangle(glow, r.X - 3, r.Y - 3, r.Width + 6, r.Height + 6);
            using (var br = new SolidBrush(fill)) g.FillPath(br, path);
            using (var pen = new Pen(Color.FromArgb(90, 90, 90), 1f)) g.DrawPath(pen, path);
            Color text = on ? Color.FromArgb(20, 20, 20) : Mix(Color.FromArgb(46, 46, 46), Color.FromArgb(205, 205, 205), light);
            float fs = Math.Min(r.Height * (line2.Length > 0 ? 0.3f : 0.4f), r.Width * 0.14f);
            fs = Math.Min(fs, (r.Width - 8) / (Math.Max(line1.Length, line2.Length) * 0.62f));
            if (fixedSize > 0) fs = fixedSize;
            if (line2.Length > 0)
            {
                Text(g, line1, r.X + r.Width / 2, r.Y + r.Height * 0.34f, fs, text, true);
                Text(g, line2, r.X + r.Width / 2, r.Y + r.Height * 0.68f, fs, text, true);
            }
            else Text(g, line1, r.X + r.Width / 2, r.Y + r.Height / 2, fs, text, true);
        }

        // A round LED with its label underneath.
        public static void Led(Graphics g, float cx, float cy, float r, Color colour, bool on, string label, double light)
        {
            if (on)
                using (var glow = new SolidBrush(Color.FromArgb(70, colour))) g.FillEllipse(glow, cx - r * 1.7f, cy - r * 1.7f, r * 3.4f, r * 3.4f);
            using (var br = new SolidBrush(on ? colour : Mix(Color.FromArgb(30, 30, 30), colour, 0.18))) g.FillEllipse(br, cx - r, cy - r, 2 * r, 2 * r);
            using (var pen = new Pen(Color.FromArgb(80, 80, 80), 1f)) g.DrawEllipse(pen, cx - r, cy - r, 2 * r, 2 * r);
            if (label.Length > 0) Text(g, label, cx, cy + r * 2.1f, Math.Max(9f, r * 0.95f), Mix(Color.FromArgb(70, 70, 70), Color.FromArgb(215, 215, 215), light), false);
        }

        // A character OLED: the text lines on a black panel in the display's blue-white, or blank, or solid white for the OLED test.
        public static void Oled(Graphics g, RectangleF r, string[] lines, int columns, bool on, bool solid = false)
        {
            using (var br = new SolidBrush(solid ? Color.FromArgb(235, 240, 245) : Color.FromArgb(4, 6, 9))) g.FillRectangle(br, r);
            using (var pen = new Pen(Color.FromArgb(110, 110, 110), 2f)) g.DrawRectangle(pen, r.X, r.Y, r.Width, r.Height);
            if (!on || solid) return;
            float cell = (r.Width - 12) / columns;
            float size = Math.Min(cell * 1.65f, (r.Height - 10) / Math.Max(1, lines.Length) * 0.85f);
            using var f = new Font("Consolas", size, FontStyle.Regular, GraphicsUnit.Pixel);
            using var tb = new SolidBrush(Color.FromArgb(120, 215, 255));
            for (int row = 0; row < lines.Length; row++)
                for (int col = 0; col < lines[row].Length && col < columns; col++)
                {
                    char ch = lines[row][col];
                    if (ch == ' ') continue;
                    g.DrawString(ch.ToString(), f, tb, r.X + 6 + col * cell + (cell - size * 0.55f) / 2, r.Y + 5 + row * ((r.Height - 10) / lines.Length) + 1);
                }
        }

        public static void Caption(Graphics g, string text, float x, float y, float size, Color c, bool bold = false)
            => Text(g, text, x, y, size, c, bold);

        public static GraphicsPath RoundRect(RectangleF r, float radius)
        {
            var p = new GraphicsPath();
            float d = radius * 2;
            p.AddArc(r.X, r.Y, d, d, 180, 90);
            p.AddArc(r.Right - d, r.Y, d, d, 270, 90);
            p.AddArc(r.Right - d, r.Bottom - d, d, d, 0, 90);
            p.AddArc(r.X, r.Bottom - d, d, d, 90, 90);
            p.CloseFigure();
            return p;
        }
    }
}
