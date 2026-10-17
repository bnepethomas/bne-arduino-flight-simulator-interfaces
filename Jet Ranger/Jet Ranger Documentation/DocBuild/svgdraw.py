"""Tiny dependency-free SVG drawing helper used by make_diagrams.py.

Diagrams are built from rounded "cards" (title + lines of text), group frames, labelled
arrows and a few helpers for charts. Everything is plain SVG so LibreOffice can embed the
result as a vector image and the sources can be regenerated at any time.
"""
from xml.sax.saxutils import escape

FONT = "Liberation Sans, Arial, Helvetica, sans-serif"
MONO = "Liberation Mono, Consolas, Courier New, monospace"

# colour themes: (fill, stroke)
THEMES = {
    "gray":   ("#F1F1EE", "#C9C9C4"),
    "white":  ("#FFFFFF", "#C9C9C4"),
    "blue":   ("#E8F0FB", "#2F6FEB"),
    "green":  ("#E7F4EA", "#2E7D32"),
    "amber":  ("#FFF3DC", "#C77700"),
    "red":    ("#FDECEA", "#C62828"),
    "purple": ("#F0EAFB", "#6A3FC7"),
    "teal":   ("#E3F5F4", "#00796B"),
    "dark":   ("#2B2F36", "#2B2F36"),
}
INK = "#1F1F1F"
INK2 = "#555555"
LINE = "#8A8A85"


def text_width(s, size, weight=400):
    """Rough width of a string in px (average Liberation Sans glyph ~0.53 em)."""
    k = 0.56 if weight >= 600 else 0.52
    return len(s) * size * k


class Svg:
    def __init__(self, width, height, title=None, subtitle=None, bg="#FFFFFF"):
        self.w = width
        self.h = height
        self.parts = []
        self.bg = bg
        self.title = title
        if title:
            self.text(24, 34, title, size=19, weight=700, fill=INK)
        if subtitle:
            self.text(24, 56, subtitle, size=12.5, fill=INK2)

    # ---- primitives ---------------------------------------------------------------
    def raw(self, s):
        self.parts.append(s)

    def rect(self, x, y, w, h, fill="#FFFFFF", stroke=LINE, rx=10, sw=1.2, dash=None, opacity=None):
        d = f' stroke-dasharray="{dash}"' if dash else ""
        o = f' fill-opacity="{opacity}"' if opacity is not None else ""
        self.parts.append(
            f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" rx="{rx}" ry="{rx}" '
            f'fill="{fill}"{o} stroke="{stroke}" stroke-width="{sw}"{d}/>')

    def text(self, x, y, s, size=12.5, weight=400, fill=INK, anchor="start", mono=False, italic=False):
        fam = MONO if mono else FONT
        st = ' font-style="italic"' if italic else ""
        self.parts.append(
            f'<text x="{x:.1f}" y="{y:.1f}" font-family="{fam}" font-size="{size}" font-weight="{weight}" '
            f'fill="{fill}" text-anchor="{anchor}"{st}>{escape(s)}</text>')

    def line(self, x1, y1, x2, y2, color=LINE, sw=1.4, dash=None):
        d = f' stroke-dasharray="{dash}"' if dash else ""
        self.parts.append(
            f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" stroke="{color}" stroke-width="{sw}"{d}/>')

    def circle(self, cx, cy, r, fill="#FFFFFF", stroke=LINE, sw=1.2):
        self.parts.append(f'<circle cx="{cx:.1f}" cy="{cy:.1f}" r="{r}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')

    def path(self, d, stroke=LINE, sw=1.6, fill="none", dash=None):
        da = f' stroke-dasharray="{dash}"' if dash else ""
        self.parts.append(f'<path d="{d}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"{da}/>')

    # ---- composite ----------------------------------------------------------------
    def card(self, x, y, w, h, title, lines=(), theme="gray", title_size=15, line_size=13, mono_lines=False,
             center=False, sw=1.2, rx=10):
        fill, stroke = THEMES[theme]
        tcol = "#FFFFFF" if theme == "dark" else INK
        lcol = "#DDDDDD" if theme == "dark" else INK2
        self.rect(x, y, w, h, fill, stroke, rx=rx, sw=sw)
        if center:
            cx = x + w / 2
            ty = y + 8 + title_size
            self.text(cx, ty, title, size=title_size, weight=700, fill=tcol, anchor="middle")
            for i, ln in enumerate(lines):
                self.text(cx, ty + 6 + (i + 1) * (line_size + 5), ln, size=line_size, fill=lcol, anchor="middle", mono=mono_lines)
        else:
            ty = y + 10 + title_size
            self.text(x + 12, ty, title, size=title_size, weight=700, fill=tcol)
            for i, ln in enumerate(lines):
                self.text(x + 12, ty + 6 + (i + 1) * (line_size + 5), ln, size=line_size, fill=lcol, mono=mono_lines)
        return (x, y, w, h)

    def frame(self, x, y, w, h, title, sub=None, theme="gray", dash=None):
        fill, stroke = THEMES[theme]
        self.rect(x, y, w, h, fill, stroke, rx=14, sw=1.2, dash=dash, opacity=0.55)
        self.text(x + 14, y + 23, title, size=16, weight=700)
        if sub:
            self.text(x + 14, y + 41, sub, size=12.5, fill=INK2)

    def pill(self, x, y, s, theme="gray", size=12.5, pad=9, h=24, mono=False):
        fill, stroke = THEMES[theme]
        w = text_width(s, size) + pad * 2
        self.rect(x, y, w, h, fill, stroke, rx=h / 2, sw=1)
        self.text(x + w / 2, y + h / 2 + size * 0.35, s, size=size, anchor="middle", mono=mono)
        return w

    def arrow(self, pts, label=None, color=LINE, sw=1.6, dash=None, both=False, label_at=0.5, label_dy=-6, label_dx=0,
              label_anchor="middle", head=True):
        """Polyline arrow through pts [(x,y),...]; arrowhead at the end (and start if both)."""
        d = "M " + " L ".join(f"{x:.1f} {y:.1f}" for x, y in pts)
        da = f' stroke-dasharray="{dash}"' if dash else ""
        ms = f' marker-end="url(#ah{self._cid(color)})"' if head else ""
        mstart = f' marker-start="url(#ahs{self._cid(color)})"' if both else ""
        self.parts.append(f'<path d="{d}" fill="none" stroke="{color}" stroke-width="{sw}"{da}{ms}{mstart}/>')
        self._need_marker(color)
        if label:
            # place label near the middle of the longest segment
            best, bi = -1, 0
            for i in range(len(pts) - 1):
                l = abs(pts[i + 1][0] - pts[i][0]) + abs(pts[i + 1][1] - pts[i][1])
                if l > best:
                    best, bi = l, i
            (x1, y1), (x2, y2) = pts[bi], pts[bi + 1]
            lx = x1 + (x2 - x1) * label_at + label_dx
            ly = y1 + (y2 - y1) * label_at + label_dy
            self._label(lx, ly, label, label_anchor)

    _markers = None

    def _label(self, x, y, label, anchor="middle", size=12):
        w = text_width(label, size) + 8
        x0 = x - w / 2 if anchor == "middle" else (x if anchor == "start" else x - w)
        self.rect(x0 - 2, y - size + 1, w + 4, size + 4, "#FFFFFF", "#FFFFFF", rx=3, sw=0)
        self.text(x if anchor != "start" else x + 2, y, label, size=size, fill=INK2, anchor=anchor)

    def _cid(self, color):
        return color.replace("#", "")

    def _need_marker(self, color):
        if self._markers is None:
            self._markers = set()
        self._markers.add(color)

    def hline_label(self, x1, x2, y, label):
        self.line(x1, y, x2, y, color="#D5D5D0", sw=1)
        self.text((x1 + x2) / 2, y - 5, label, size=10.5, fill=INK2, anchor="middle")

    # ---- output -------------------------------------------------------------------
    def to_string(self):
        defs = ["<defs>"]
        for color in (self._markers or {LINE}):
            cid = self._cid(color)
            defs.append(
                f'<marker id="ah{cid}" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="8" markerHeight="8" orient="auto">'
                f'<path d="M 0 0 L 10 5 L 0 10 z" fill="{color}"/></marker>')
            defs.append(
                f'<marker id="ahs{cid}" viewBox="0 0 10 10" refX="1" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse">'
                f'<path d="M 0 0 L 10 5 L 0 10 z" fill="{color}"/></marker>')
        defs.append("</defs>")
        body = "\n".join(self.parts)
        return (f'<?xml version="1.0" encoding="UTF-8"?>\n'
                f'<svg xmlns="http://www.w3.org/2000/svg" width="{self.w}" height="{self.h}" viewBox="0 0 {self.w} {self.h}">\n'
                f'<rect width="{self.w}" height="{self.h}" fill="{self.bg}"/>\n' + "\n".join(defs) + "\n" + body + "\n</svg>\n")

    def save(self, path):
        with open(path, "w", encoding="utf-8") as f:
            f.write(self.to_string())
