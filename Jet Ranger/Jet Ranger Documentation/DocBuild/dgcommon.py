"""Shared registry for the diagram modules (diagrams_*.py)."""
import os

from svgdraw import Svg, THEMES, INK, INK2, LINE, text_width  # noqa: F401  (re-exported)

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "diagrams")
REGISTRY = {}

BLUE = "#2F6FEB"
GREEN = "#2E7D32"
AMBER = "#C77700"
RED = "#C62828"


def diagram(fn):
    REGISTRY[fn.__name__] = fn
    return fn
