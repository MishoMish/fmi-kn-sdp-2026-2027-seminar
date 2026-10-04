"""Shared SVG helpers for every week's figures (weeks/*/img/src/make_figures.py).

Plain standard-library Python. Colors are the same tokens as slides/theme/sdp.css,
so figures and slides match. Every figure has its own white card background,
so it stays readable in GitHub's dark mode too.
"""

import math
from pathlib import Path

# Tokens shared with slides/theme/sdp.css.
SURFACE = "#ffffff"
INK = "#0b0b0b"
INK2 = "#52514e"
MUTED = "#8a8984"
GRID = "#e4e3df"
PANEL = "#f6f5f2"
S1, S2, S3, S4, S5 = "#2a78d6", "#eb6834", "#1baf7a", "#eda100", "#e87ba4"
RED, GREEN = "#e34948", "#1baf7a"
TINT1, TINT2, TINT3, TINT5 = "#e3eefb", "#fde8df", "#dcf3ea", "#fbe7ef"

SANS = "'IBM Plex Sans','Segoe UI','Helvetica Neue',Arial,sans-serif"
MONO = "'IBM Plex Mono',Consolas,Menlo,monospace"


def esc(text):
    return (str(text).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"))


class Svg:
    def __init__(self, width, height, title):
        self.w, self.h = width, height
        self.parts = [
            f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" '
            f'width="{width}" height="{height}" role="img" font-family="{SANS}">',
            f"<title>{esc(title)}</title>",
            f'<rect width="{width}" height="{height}" rx="12" fill="{SURFACE}"/>',
            "<defs>"
            f'<marker id="arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" '
            f'markerHeight="7" orient="auto-start-reverse"><path d="M0,0 L10,5 L0,10 z" fill="{INK2}"/></marker>'
            "</defs>",
        ]

    def add(self, s):
        self.parts.append(s)

    def text(self, x, y, s, size=16, fill=INK, anchor="start", weight=400, family=None, italic=False):
        fam = f' font-family="{family}"' if family else ""
        st = ' font-style="italic"' if italic else ""
        self.add(f'<text x="{x:.1f}" y="{y:.1f}" font-size="{size}" fill="{fill}" '
                 f'text-anchor="{anchor}" font-weight="{weight}"{fam}{st}>{esc(s)}</text>')

    def rect(self, x, y, w, h, fill, stroke="none", rx=8, sw=1.5, dash=None):
        d = f' stroke-dasharray="{dash}"' if dash else ""
        self.add(f'<rect x="{x:.1f}" y="{y:.1f}" width="{w:.1f}" height="{h:.1f}" rx="{rx}" '
                 f'fill="{fill}" stroke="{stroke}" stroke-width="{sw}"{d}/>')

    def line(self, x1, y1, x2, y2, stroke=INK2, sw=1.5, arrow=False, dash=None):
        a = ' marker-end="url(#arrow)"' if arrow else ""
        d = f' stroke-dasharray="{dash}"' if dash else ""
        self.add(f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" stroke="{stroke}" '
                 f'stroke-width="{sw}" stroke-linecap="round"{a}{d}/>')

    def path(self, d, stroke, sw=2, fill="none", dash=None, arrow=False):
        da = f' stroke-dasharray="{dash}"' if dash else ""
        a = ' marker-end="url(#arrow)"' if arrow else ""
        self.add(f'<path d="{d}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}" '
                 f'stroke-linejoin="round" stroke-linecap="round"{da}{a}/>')

    def circle(self, x, y, r, fill, stroke=SURFACE, sw=2):
        self.add(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="{r}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')

    def box(self, x, y, w, h, lines, fill=PANEL, stroke=GRID, size=16, mono_first=False, weight=600):
        self.rect(x, y, w, h, fill, stroke)
        total = len(lines)
        for k, s in enumerate(lines):
            cy = y + h / 2 + (k - (total - 1) / 2) * (size * 1.35) + size * 0.35
            fam = MONO if (mono_first and k == 0) else None
            self.text(x + w / 2, cy, s, size=size if k == 0 else size - 2,
                      fill=INK if k == 0 else INK2, anchor="middle",
                      weight=weight if k == 0 else 400, family=fam)

    def save(self, path):
        self.add("</svg>")
        path = Path(path)
        path.write_text("\n".join(self.parts) + "\n", encoding="utf-8")
        print("wrote", path.name)


def polyline(points):
    return "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in points)


# ---------------------------------------------------------------- charts

def axes(s, x0, y0, w, h, xticks, yticks, xlabel, ylabel, fx, fy, xfmt=str, yfmt=str):
    for t in yticks:
        y = fy(t)
        s.line(x0, y, x0 + w, y, stroke=GRID, sw=1)
        s.text(x0 - 10, y + 5, yfmt(t), size=13, fill=INK2, anchor="end")
    for t in xticks:
        x = fx(t)
        s.line(x, y0 + h, x, y0 + h + 6, stroke=MUTED, sw=1)
        s.text(x, y0 + h + 24, xfmt(t), size=13, fill=INK2, anchor="middle")
    s.line(x0, y0 + h, x0 + w, y0 + h, stroke=MUTED, sw=1)
    s.text(x0 + w / 2, y0 + h + 52, xlabel, size=15, fill=INK2, anchor="middle")
    s.add(f'<text transform="translate({x0 - 58},{y0 + h / 2}) rotate(-90)" font-size="15" '
          f'fill="{INK2}" text-anchor="middle">{esc(ylabel)}</text>')


def legend(s, x, y, items):
    for k, (col, lab) in enumerate(items):
        yy = y + k * 24
        s.line(x, yy, x + 22, yy, stroke=col, sw=3)
        s.text(x + 30, yy + 5, lab, size=14, fill=INK)


