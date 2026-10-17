"""Generates every diagram used by the Jet Ranger documents as vector SVG into ./diagrams.

Run:  python make_diagrams.py            (all diagrams)
      python make_diagrams.py NAME ...   (only the named ones)

The diagrams live in the diagrams_*.py modules; each one is a function decorated with @diagram
that returns an Svg (see svgdraw.py). Canvas width is 880 px so the text stays readable when the
figure is scaled to the page width.
"""
import os
import sys

import diagrams_a  # noqa: F401  system, network, protocol, life cycle
import diagrams_b  # noqa: F401  boards
import diagrams_c  # noqa: F401  PC software
import diagrams_d  # noqa: F401  guide and tools
from dgcommon import OUT, REGISTRY


def build(names):
    os.makedirs(OUT, exist_ok=True)
    todo = names or list(REGISTRY)
    for n in todo:
        svg = REGISTRY[n]()
        svg.save(os.path.join(OUT, n + ".svg"))
        print("wrote", n + ".svg")


if __name__ == "__main__":
    build(sys.argv[1:])
