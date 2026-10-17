"""Render SVG files to PNG through LibreOffice (UNO) for visual checking.

Run with LibreOffice's python:  "C:/Program Files/LibreOffice/program/python.exe" svg2png.py [name ...]
Needs a headless soffice listening on port 2002 (see build_docs.ps1 / run_lo.ps1).
"""
import os
import sys
import time

import uno
from com.sun.star.beans import PropertyValue

HERE = os.path.dirname(os.path.abspath(__file__))
SVG_DIR = os.path.join(HERE, "diagrams")
PNG_DIR = os.path.join(SVG_DIR, "png")


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


def main():
    os.makedirs(PNG_DIR, exist_ok=True)
    names = sys.argv[1:] or [f[:-4] for f in sorted(os.listdir(SVG_DIR)) if f.endswith(".svg")]
    ctx = connect()
    smgr = ctx.ServiceManager
    desktop = smgr.createInstanceWithContext("com.sun.star.frame.Desktop", ctx)
    for n in names:
        src = uno.systemPathToFileUrl(os.path.join(SVG_DIR, n + ".svg"))
        dst = uno.systemPathToFileUrl(os.path.join(PNG_DIR, n + ".png"))
        doc = desktop.loadComponentFromURL(src, "_blank", 0, (pv("Hidden", True),))
        page = doc.getDrawPages().getByIndex(0)
        flt = smgr.createInstanceWithContext("com.sun.star.drawing.GraphicExportFilter", ctx)
        flt.setSourceDocument(page)
        w = page.Width
        h = page.Height
        px = 1500
        flt.filter((pv("URL", dst), pv("MediaType", "image/png"),
                    pv("FilterData", uno.Any("[]com.sun.star.beans.PropertyValue",
                                              (pv("PixelWidth", px), pv("PixelHeight", int(px * h / w)))))))
        doc.close(True)
        print("rendered", n)


if __name__ == "__main__":
    main()
