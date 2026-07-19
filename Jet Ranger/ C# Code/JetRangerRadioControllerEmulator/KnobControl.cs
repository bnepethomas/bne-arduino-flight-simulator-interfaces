using System.ComponentModel;
using System.Drawing.Drawing2D;

namespace JetRangerRadioControllerEmulator
{
    // A clickable stand-in for a physical rotary encoder knob. Clicking the
    // right half turns it clockwise (+1 detent), the left half turns it
    // counter-clockwise (-1 detent). The pointer line is purely cosmetic
    // feedback; it does not represent the underlying frequency value.
    public class KnobControl : Control
    {
        public event EventHandler<int>? Turned;

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public string Caption { get; set; } = "";

        private float pointerAngle;

        public KnobControl()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint
                | ControlStyles.UserPaint
                | ControlStyles.OptimizedDoubleBuffer
                | ControlStyles.ResizeRedraw, true);
            Width = 74;
            Height = 92;
            Cursor = Cursors.Hand;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;

            int diameter = Math.Min(Width, Height - 16) - 8;
            var knobRect = new Rectangle((Width - diameter) / 2, 2, diameter, diameter);

            using (var body = new SolidBrush(Color.Gainsboro))
            using (var edge = new Pen(Color.DimGray, 2))
            {
                g.FillEllipse(body, knobRect);
                g.DrawEllipse(edge, knobRect);
            }

            var center = new PointF(knobRect.X + knobRect.Width / 2f, knobRect.Y + knobRect.Height / 2f);
            float radius = diameter / 2f - 6;
            double rad = pointerAngle * Math.PI / 180.0;
            var tip = new PointF(
                center.X + (float)(radius * Math.Sin(rad)),
                center.Y - (float)(radius * Math.Cos(rad)));
            using (var pointerPen = new Pen(Color.Black, 3))
                g.DrawLine(pointerPen, center, tip);

            using (var hintFont = new Font("Segoe UI", 9f, FontStyle.Bold))
            using (var hintBrush = new SolidBrush(Color.DimGray))
            {
                g.DrawString("-", hintFont, hintBrush, knobRect.Left - 2, center.Y - 9);
                g.DrawString("+", hintFont, hintBrush, knobRect.Right - 4, center.Y - 9);
            }

            if (!string.IsNullOrEmpty(Caption))
            {
                using var captionFont = new Font("Segoe UI", 8f, FontStyle.Bold);
                var size = g.MeasureString(Caption, captionFont);
                g.DrawString(Caption, captionFont, Brushes.Black, (Width - size.Width) / 2f, Height - 14);
            }
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            int direction = e.X >= Width / 2 ? 1 : -1;
            pointerAngle = (pointerAngle + direction * 20f + 360f) % 360f;
            Invalidate();
            Turned?.Invoke(this, direction);
        }
    }
}
