"""Builds a Jet Ranger document (.odt) from the Markdown sources in ./src/<name>/*.md.

Run with LibreOffice's python through run_lo.ps1:
    .\\run_lo.ps1 build_docs.py master
    .\\run_lo.ps1 build_docs.py guide
Output: ./out/<name>.odt  (and ./out/<name>.pdf for checking)

Supported Markdown subset: # .. #### headings, paragraphs, - / 1. lists (two levels), pipe tables,
``` code blocks, > callouts, ![caption](diagrams/x.svg) figures, <!-- pagebreak -->,
inline **bold**, *italic*, `code`, [text](url) (text only).
"""
import glob
import os
import re
import sys
import time

import uno
from com.sun.star.beans import PropertyValue
from com.sun.star.text.ControlCharacter import PARAGRAPH_BREAK, LINE_BREAK
from com.sun.star.style.BreakType import PAGE_BEFORE, PAGE_AFTER, NONE as BREAK_NONE
from com.sun.star.text.TextContentAnchorType import AS_CHARACTER
from com.sun.star.awt.FontWeight import BOLD, NORMAL
from com.sun.star.table import BorderLine2

HERE = os.path.dirname(os.path.abspath(__file__))

BODY_FONT = "Liberation Sans"
MONO_FONT = "Liberation Mono"
ACCENT = 0x1F4E8C          # headings
GREY_BG = 0xF1F1EE         # code blocks, callouts
HEAD_BG = 0xE4EAF3         # table header
LINE_COL = 0xB8B8B2
PAGE_W_MM = 170            # A4 210 - 2 x 20 mm margins


def pv(n, v):
    p = PropertyValue()
    p.Name = n
    p.Value = v
    return p


def connect():
    local = uno.getComponentContext()
    resolver = local.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", local)
    for _ in range(60):
        try:
            return resolver.resolve("uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext")
        except Exception:
            time.sleep(1)
    raise SystemExit("could not connect to soffice on port 2002")


# ------------------------------------------------------------------------------------------
# Markdown parsing
# ------------------------------------------------------------------------------------------
def parse_markdown(text):
    """-> list of blocks: ('h',lvl,text) ('p',text) ('ul',[(lvl,text)]) ('ol',[(lvl,text)]) ('table',[rows])
    ('code',[lines]) ('quote',text) ('fig',caption,path) ('pagebreak',)"""
    lines = text.replace("\r\n", "\n").split("\n")
    blocks = []
    i = 0
    n = len(lines)
    while i < n:
        ln = lines[i]
        s = ln.strip()
        if not s:
            i += 1
            continue
        if s.startswith("<!--"):
            if "pagebreak" in s:
                blocks.append(("pagebreak",))
            i += 1
            continue
        m = re.match(r"^(#{1,4})\s+(.*)$", s)
        if m:
            blocks.append(("h", len(m.group(1)), m.group(2).strip()))
            i += 1
            continue
        if s.startswith("```"):
            i += 1
            code = []
            while i < n and not lines[i].strip().startswith("```"):
                code.append(lines[i].rstrip("\n"))
                i += 1
            i += 1
            blocks.append(("code", code))
            continue
        m = re.match(r"^!\[(.*?)\]\((.*?)\)\s*$", s)
        if m:
            blocks.append(("fig", m.group(1), m.group(2)))
            i += 1
            continue
        if s.startswith("|") and i + 1 < n and re.match(r"^\s*\|?[\s:\-|]+\|?\s*$", lines[i + 1]) and "-" in lines[i + 1]:
            rows = []
            while i < n and lines[i].strip().startswith("|"):
                row = lines[i].strip()
                is_sep = bool(re.match(r"^\|?[\s:\-|]+\|?$", row)) and not re.search(r"[A-Za-z0-9]", row)
                if not is_sep:
                    cells = [c.strip() for c in re.split(r"(?<!\\)\|", row.strip("|"))]
                    rows.append([c.replace("\\|", "|") for c in cells])
                i += 1
            blocks.append(("table", rows))
            continue
        if re.match(r"^\s*([-*])\s+", ln) or re.match(r"^\s*\d+[.)]\s+", ln):
            ordered = bool(re.match(r"^\s*\d+[.)]\s+", ln))
            items = []
            while i < n and lines[i].strip():
                l2 = lines[i]
                m1 = re.match(r"^(\s*)([-*])\s+(.*)$", l2)
                m2 = re.match(r"^(\s*)\d+[.)]\s+(.*)$", l2)
                if m1 or m2:
                    mm = m1 or m2
                    indent = len(mm.group(1).replace("\t", "    "))
                    body = mm.group(3) if m1 else mm.group(2)
                    items.append([1 if indent >= 2 else 0, body.strip()])
                elif items and (l2.startswith("  ") or l2.startswith("\t")):
                    items[-1][1] += " " + l2.strip()
                else:
                    break
                i += 1
            blocks.append(("ol" if ordered else "ul", [(a, b) for a, b in items]))
            continue
        if s.startswith(">"):
            buf = []
            while i < n and lines[i].strip().startswith(">"):
                buf.append(lines[i].strip().lstrip(">").strip())
                i += 1
            blocks.append(("quote", " ".join(b for b in buf if b)))
            continue
        # paragraph
        buf = [s]
        i += 1
        while i < n and lines[i].strip() and not re.match(r"^(#{1,4}\s|```|\||>|!\[|\s*([-*]|\d+[.)])\s+|<!--)", lines[i].strip()) \
                and not re.match(r"^\s*([-*]|\d+[.)])\s+", lines[i]):
            buf.append(lines[i].strip())
            i += 1
        blocks.append(("p", " ".join(buf)))
    return blocks


INLINE_RE = re.compile(r"(\*\*[^*]+\*\*|`[^`]+`|\*[^*\s][^*]*\*|\[[^\]]+\]\([^)]+\))")


CELL_COLOUR_RE = re.compile(r"^\[\[(#[0-9A-Fa-f]{6})\]\]\s*")


def split_cell_colour(val):
    """'[[#D32F2F]]12 V' -> (0xD32F2F, '12 V'); no marker -> (None, val)."""
    m = CELL_COLOUR_RE.match(val)
    if not m:
        return None, val
    return int(m.group(1)[1:], 16), val[m.end():]


def inline_tokens(text):
    """-> [(kind, text)] kind in plain/bold/code/italic"""
    out = []
    pos = 0
    for m in INLINE_RE.finditer(text):
        if m.start() > pos:
            out.append(("plain", text[pos:m.start()]))
        tok = m.group(0)
        if tok.startswith("**"):
            inner = tok[2:-2]
            for k, part in enumerate(re.split(r"`([^`]+)`", inner)):
                if part:
                    out.append(("boldcode" if k % 2 else "bold", part))
        elif tok.startswith("`"):
            out.append(("code", tok[1:-1]))
        elif tok.startswith("["):
            out.append(("plain", re.match(r"\[([^\]]+)\]", tok).group(1)))
        else:
            out.append(("italic", tok[1:-1]))
        pos = m.end()
    if pos < len(text):
        out.append(("plain", text[pos:]))
    return out


# ------------------------------------------------------------------------------------------
# Document construction
# ------------------------------------------------------------------------------------------
class Builder:
    def __init__(self, ctx, title, subtitle, version_line):
        self.ctx = ctx
        smgr = ctx.ServiceManager
        self.desktop = smgr.createInstanceWithContext("com.sun.star.frame.Desktop", ctx)
        self.doc = self.desktop.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
        self.text = self.doc.Text
        self.cur = self.text.createTextCursor()
        self.fig_no = 0
        self.counters = [0, 0, 0]
        self.title = title
        self.subtitle = subtitle
        self.version_line = version_line
        self.styles_setup()

    # ---- styles ----
    def pstyle(self, name):
        return self.doc.StyleFamilies.getByName("ParagraphStyles").getByName(name)

    def new_pstyle(self, name, parent="Standard"):
        fam = self.doc.StyleFamilies.getByName("ParagraphStyles")
        if not fam.hasByName(name):
            st = self.doc.createInstance("com.sun.star.style.ParagraphStyle")
            fam.insertByName(name, st)
        st = fam.getByName(name)
        st.ParentStyle = parent
        return st

    def styles_setup(self):
        d = self.doc
        ps = d.StyleFamilies.getByName("PageStyles").getByName("Standard")
        ps.Width, ps.Height = 21000, 29700
        for side in ("LeftMargin", "RightMargin"):
            setattr(ps, side, 2000)
        ps.TopMargin, ps.BottomMargin = 2000, 2000
        ps.HeaderIsOn = True
        ps.FooterIsOn = True
        ps.HeaderHeight = 700
        ps.FooterHeight = 700

        std = self.pstyle("Standard")
        std.CharFontName = BODY_FONT
        std.CharHeight = 10
        std.ParaBottomMargin = 160
        std.ParaTopMargin = 0
        std.ParaLineSpacing = uno.createUnoStruct("com.sun.star.style.LineSpacing", Mode=1, Height=118)
        std.ParaOrphans = 2
        std.ParaWidows = 2

        sizes = {1: 20, 2: 15, 3: 12.5, 4: 11}
        for lvl in range(1, 5):
            h = self.pstyle("Heading %d" % lvl)
            h.CharFontName = BODY_FONT
            h.CharHeight = sizes[lvl]
            h.CharWeight = BOLD
            h.CharColor = ACCENT
            h.ParaTopMargin = {1: 0, 2: 500, 3: 380, 4: 300}[lvl]
            h.ParaBottomMargin = {1: 300, 2: 200, 3: 160, 4: 120}[lvl]
            h.ParaKeepTogether = True
            if lvl == 1:
                h.BreakType = PAGE_BEFORE
        code = self.new_pstyle("Code Block")
        code.CharFontName = MONO_FONT
        code.CharHeight = 8
        code.ParaBackColor = GREY_BG
        code.ParaTopMargin = 0
        code.ParaBottomMargin = 200
        code.ParaLeftMargin = 150
        code.ParaRightMargin = 150
        code.ParaLineSpacing = uno.createUnoStruct("com.sun.star.style.LineSpacing", Mode=1, Height=100)
        callout = self.new_pstyle("Callout")
        callout.ParaBackColor = 0xEAF1FB
        callout.ParaLeftMargin = 250
        callout.ParaRightMargin = 150
        callout.ParaTopMargin = 100
        callout.ParaBottomMargin = 220
        callout.CharHeight = 9.5
        bl = BorderLine2()
        bl.OuterLineWidth = 40
        bl.Color = 0x2F6FEB
        bl.LineWidth = 40
        callout.LeftBorder = bl
        callout.LeftBorderDistance = 150
        cap = self.new_pstyle("Figure Caption")
        cap.CharHeight = 9
        cap.CharColor = 0x555555
        cap.CharPosture = uno.Enum("com.sun.star.awt.FontSlant", "ITALIC")
        cap.ParaAdjust = 3  # CENTER
        cap.ParaBottomMargin = 300
        fig = self.new_pstyle("Figure")
        fig.ParaAdjust = 3
        fig.ParaBottomMargin = 60
        try:
            fig.ParaKeepWithNext = True
        except Exception:
            pass
        li = self.new_pstyle("List Item")
        li.ParaBottomMargin = 60
        tc = self.new_pstyle("Table Cell")
        tc.CharHeight = 8.5
        tc.ParaBottomMargin = 0
        tc.ParaTopMargin = 0
        tc.ParaLineSpacing = uno.createUnoStruct("com.sun.star.style.LineSpacing", Mode=1, Height=105)
        th = self.new_pstyle("Table Head", "Table Cell")
        th.CharWeight = BOLD
        tt = self.new_pstyle("Cover Title")
        tt.CharHeight = 30
        tt.CharWeight = BOLD
        tt.CharColor = ACCENT
        tt.ParaBottomMargin = 300
        ch = self.new_pstyle("Contents Heading")
        ch.CharHeight = 20
        ch.CharWeight = BOLD
        ch.CharColor = ACCENT
        ch.BreakType = PAGE_BEFORE
        ch.ParaBottomMargin = 300
        ts = self.new_pstyle("Cover Subtitle")
        ts.CharHeight = 14
        ts.CharColor = 0x555555
        ts.ParaBottomMargin = 200

        cs = d.StyleFamilies.getByName("CharacterStyles")
        for name, mono in (("Code Char", True),):
            if not cs.hasByName(name):
                c = d.createInstance("com.sun.star.style.CharacterStyle")
                cs.insertByName(name, c)
            c = cs.getByName(name)
            c.CharFontName = MONO_FONT
            c.CharColor = 0x1F4E8C
            c.CharHeight = 9

        # chapter numbering 1 / 1.1 / 1.1.1 (best effort: the property types are picky)
        try:
            rules = d.getChapterNumberingRules()
            for lvl in range(3):
                props = list(rules.getByIndex(lvl))
                for i, p in enumerate(props):
                    if p.Name == "NumberingType":
                        props[i] = pv("NumberingType", uno.Any("short", 4))
                    elif p.Name == "ParentNumbering":
                        props[i] = pv("ParentNumbering", uno.Any("short", lvl + 1))
                    elif p.Name == "Suffix":
                        props[i] = pv("Suffix", " ")
                    elif p.Name == "Prefix":
                        props[i] = pv("Prefix", "")
                rules.replaceByIndex(lvl, uno.Any("[]com.sun.star.beans.PropertyValue", tuple(props)))
        except Exception as ex:
            print("chapter numbering not applied:", type(ex).__name__)

    # ---- low level insertion ----
    def end_para(self):
        self.text.insertControlCharacter(self.cur, PARAGRAPH_BREAK, False)

    def set_pstyle(self, name):
        self.cur.ParaStyleName = name

    def add_inline(self, text, cursor=None, base_bold=False, textobj=None):
        c = cursor or self.cur
        to = textobj or self.text
        for kind, t in inline_tokens(text):
            c.setPropertyToDefault("CharStyleName")
            c.CharWeight = BOLD if (kind in ("bold", "boldcode") or base_bold) else NORMAL
            c.CharPosture = uno.Enum("com.sun.star.awt.FontSlant", "ITALIC" if kind == "italic" else "NONE")
            if kind in ("code", "boldcode"):
                c.CharStyleName = "Code Char"
            to.insertString(c, t, False)
        c.CharWeight = NORMAL
        c.CharPosture = uno.Enum("com.sun.star.awt.FontSlant", "NONE")
        c.setPropertyToDefault("CharStyleName")

    def paragraph(self, text, style="Standard"):
        self.set_pstyle(style)
        self.cur.NumberingStyleName = ""
        self.add_inline(text)
        self.end_para()

    def heading(self, lvl, text):
        self.set_pstyle("Heading %d" % lvl)
        self.cur.NumberingStyleName = ""
        self.cur.setPropertyToDefault("CharWeight")
        label = re.sub(r"[`*]", "", text)
        nonum = label.endswith("{-}")
        if nonum:
            label = label[:-3].strip()
            m = re.match(r"Appendix ([A-Z])", label)
            if lvl == 1:
                self.app_letter = m.group(1) if m else None
                self.app_counters = [0, 0]
        elif getattr(self, "app_letter", None) and lvl in (2, 3):
            self.app_counters[lvl - 2] += 1
            if lvl == 2:
                self.app_counters[1] = 0
            label = self.app_letter + "." + ".".join(str(c) for c in self.app_counters[:lvl - 1]) + "  " + label
        elif lvl <= 3:
            self.counters[lvl - 1] += 1
            for j in range(lvl, 3):
                self.counters[j] = 0
            label = ".".join(str(c) for c in self.counters[:lvl]) + "  " + label
        self.text.insertString(self.cur, label, False)
        self.end_para()

    def list_block(self, items, ordered):
        for idx, (lvl, body) in enumerate(items):
            self.set_pstyle("List Item")
            self.cur.NumberingStyleName = "Numbering 123" if ordered else "List 1"
            try:
                self.cur.NumberingLevel = lvl
            except Exception:
                pass
            if ordered:
                try:
                    self.cur.ParaIsNumberingRestart = (idx == 0)
                    if idx == 0:
                        self.cur.NumberingStartValue = 1
                except Exception:
                    pass
            self.add_inline(body)
            self.end_para()
        self.cur.NumberingStyleName = ""
        self.cur.setPropertyToDefault("NumberingLevel")
        try:
            self.cur.ParaIsNumberingRestart = False
        except Exception:
            pass

    def code_block(self, lines):
        self.set_pstyle("Code Block")
        self.cur.NumberingStyleName = ""
        for i, ln in enumerate(lines):
            self.text.insertString(self.cur, ln.replace("\t", "    ") if ln else " ", False)
            if i < len(lines) - 1:
                self.text.insertControlCharacter(self.cur, LINE_BREAK, False)
        self.end_para()

    def figure(self, caption, path):
        full = os.path.normpath(os.path.join(HERE, path))
        if not os.path.exists(full):
            self.paragraph("[missing figure: %s]" % path, "Callout")
            return
        self.fig_no += 1
        w_px, h_px = svg_size(full)
        if full.lower().endswith(".png"):
            # screenshots: natural size at 96 dpi, scaled down to fit the page width and a maximum height
            w_mm = min(PAGE_W_MM, w_px * 0.2646)
            h_mm = w_mm * h_px / w_px
            if h_mm > 215:
                w_mm = w_mm * 215 / h_mm
                h_mm = 215
        else:
            w_mm = min(PAGE_W_MM, w_px * 0.2646 * 0.92)
            h_mm = w_mm * h_px / w_px
        self.set_pstyle("Figure")
        self.cur.NumberingStyleName = ""
        try:
            self.cur.ParaKeepTogether = True   # in the UNO API this means "keep with next paragraph"
        except Exception:
            pass
        g = self.doc.createInstance("com.sun.star.text.TextGraphicObject")
        g.GraphicURL = uno.systemPathToFileUrl(full)
        g.AnchorType = AS_CHARACTER
        g.Width = int(w_mm * 100)
        g.Height = int(h_mm * 100)
        g.Name = "Figure %d" % self.fig_no
        self.text.insertTextContent(self.cur, g, False)
        self.end_para()
        self.set_pstyle("Figure Caption")
        try:
            self.cur.ParaKeepTogether = False
        except Exception:
            pass
        self.text.insertString(self.cur, "Figure %d \u2014 %s" % (self.fig_no, caption), False)
        self.end_para()

    def table(self, rows):
        ncols = max(len(r) for r in rows)
        rows = [r + [""] * (ncols - len(r)) for r in rows]
        t = self.doc.createInstance("com.sun.star.text.TextTable")
        t.initialize(len(rows), ncols)
        self.set_pstyle("Standard")
        self.text.insertTextContent(self.cur, t, False)
        # widths proportional to content length
        lens = []
        for c in range(ncols):
            cells = [re.sub(r"[`*]", "", split_cell_colour(rows[r][c])[1]) for r in range(len(rows))]
            m = max(len(x) for x in cells)
            word = max((len(w) for x in cells for w in x.split()), default=1)
            # a column must at least fit its longest unbreakable word (mono text is wider)
            need = word * 1.2 + 3
            lens.append(max(need, min(m, 55) * 0.7, 8))
        total = sum(lens)
        sep = t.TableColumnSeparators
        rel = t.TableColumnRelativeSum
        cum = 0
        for i in range(ncols - 1):
            cum += lens[i]
            sep[i].Position = int(cum / total * rel)
        t.TableColumnSeparators = sep
        # borders
        border = BorderLine2()
        border.OuterLineWidth = 6
        border.LineWidth = 6
        border.Color = LINE_COL
        tb = t.TableBorder2
        tb.TopLine = tb.BottomLine = tb.LeftLine = tb.RightLine = tb.HorizontalLine = tb.VerticalLine = border
        tb.IsTopLineValid = tb.IsBottomLineValid = tb.IsLeftLineValid = tb.IsRightLineValid = True
        tb.IsHorizontalLineValid = tb.IsVerticalLineValid = True
        t.TableBorder2 = tb
        t.LeftMargin = 0
        t.RepeatHeadline = True
        t.HeaderRowCount = 1
        t.Split = True
        for r, row in enumerate(rows):
            for c, val in enumerate(row):
                cell = t.getCellByPosition(c, r)
                cell.LeftBorderDistance = 70
                cell.RightBorderDistance = 70
                cell.TopBorderDistance = 40
                cell.BottomBorderDistance = 40
                cc = cell.createTextCursor()
                cc.ParaStyleName = "Table Head" if r == 0 else "Table Cell"
                if r == 0:
                    cell.BackColor = HEAD_BG
                else:
                    bg, val = split_cell_colour(val)
                    if bg is not None:
                        cell.BackColor = bg
                        lum = 0.299 * ((bg >> 16) & 255) + 0.587 * ((bg >> 8) & 255) + 0.114 * (bg & 255)
                        cc.CharColor = 0xFFFFFF if lum < 150 else 0x1F1F1F
                self.add_inline(val, cc, base_bold=(r == 0), textobj=cell)
        # move cursor after the table
        self.cur.gotoEnd(False)
        self.set_pstyle("Standard")

    def page_break(self):
        self.cur.BreakType = PAGE_BEFORE

    # ---- front matter ----
    def cover(self):
        for _ in range(7):
            self.paragraph(" ", "Standard")
        self.paragraph(self.title, "Cover Title")
        self.paragraph(self.subtitle, "Cover Subtitle")
        self.paragraph(self.version_line, "Standard")

    def toc(self):
        self.set_pstyle("Contents Heading")
        self.cur.NumberingStyleName = ""
        self.text.insertString(self.cur, "Contents", False)
        self.end_para()
        self.set_pstyle("Standard")
        idx = self.doc.createInstance("com.sun.star.text.ContentIndex")
        idx.Title = ""
        idx.CreateFromOutline = True
        idx.Level = 3
        self.text.insertTextContent(self.cur, idx, False)
        self.end_para()
        self.toc_index = idx

    def footer_header(self):
        ps = self.doc.StyleFamilies.getByName("PageStyles").getByName("Standard")
        ht = ps.HeaderText
        ht.String = self.title
        hc = ht.createTextCursor()
        hc.gotoStart(False)
        hc.gotoEnd(True)
        hc.CharHeight = 8
        hc.CharColor = 0x777777
        ft = ps.FooterText
        fc = ft.createTextCursor()
        fc.ParaAdjust = 3  # center
        fc.CharHeight = 8
        fc.CharColor = 0x777777
        ft.insertString(fc, "Page ", False)
        pn = self.doc.createInstance("com.sun.star.text.TextField.PageNumber")
        pn.NumberingType = 4
        ft.insertTextContent(fc, pn, False)
        ft.insertString(fc, " of ", False)
        pc = self.doc.createInstance("com.sun.star.text.TextField.PageCount")
        pc.NumberingType = 4
        ft.insertTextContent(fc, pc, False)

    def finish(self, out_odt, out_pdf):
        for i in range(self.doc.getDocumentIndexes().getCount()):
            self.doc.getDocumentIndexes().getByIndex(i).update()
        os.makedirs(os.path.dirname(out_odt), exist_ok=True)
        self.doc.storeToURL(uno.systemPathToFileUrl(out_odt), (pv("FilterName", "writer8"),))
        self.doc.storeToURL(uno.systemPathToFileUrl(out_pdf), (pv("FilterName", "writer_pdf_Export"),))
        try:
            self.doc.close(True)
        except Exception:
            pass


def png_size(path):
    import struct
    with open(path, "rb") as f:
        head = f.read(24)
    return struct.unpack(">II", head[16:24])


def svg_size(path):
    if path.lower().endswith(".png"):
        return png_size(path)
    s = open(path, encoding="utf-8").read(600)
    w = re.search(r'width="([\d.]+)"', s)
    h = re.search(r'height="([\d.]+)"', s)
    return float(w.group(1)), float(h.group(1))


def render(b, blocks):
    for blk in blocks:
        k = blk[0]
        if k == "h":
            b.heading(blk[1], blk[2])
        elif k == "p":
            b.paragraph(blk[1])
        elif k == "ul":
            b.list_block(blk[1], False)
        elif k == "ol":
            b.list_block(blk[1], True)
        elif k == "table":
            b.table(blk[1])
        elif k == "code":
            b.code_block(blk[1])
        elif k == "quote":
            b.paragraph(blk[1], "Callout")
        elif k == "fig":
            b.figure(blk[1], blk[2])
        elif k == "pagebreak":
            b.page_break()


def main():
    name = sys.argv[1] if len(sys.argv) > 1 else "sample"
    src_dir = os.path.join(HERE, "src", name)
    meta = {"title": "Jet Ranger", "subtitle": "", "version": ""}
    meta_file = os.path.join(src_dir, "meta.txt")
    if os.path.exists(meta_file):
        for ln in open(meta_file, encoding="utf-8"):
            if "=" in ln:
                k, v = ln.split("=", 1)
                meta[k.strip()] = v.strip()
    ctx = connect()
    b = Builder(ctx, meta["title"], meta["subtitle"], meta["version"])
    b.cover()
    b.toc()
    files = sorted(glob.glob(os.path.join(src_dir, "*.md")))
    for f in files:
        blocks = parse_markdown(open(f, encoding="utf-8").read())
        render(b, blocks)
    b.footer_header()
    b.finish(os.path.join(HERE, "out", name + ".odt"), os.path.join(HERE, "out", name + ".pdf"))
    print("built", name, "from", len(files), "files,", b.fig_no, "figures")


if __name__ == "__main__":
    main()
