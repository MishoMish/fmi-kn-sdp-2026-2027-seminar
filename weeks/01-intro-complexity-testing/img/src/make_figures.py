#!/usr/bin/env python3
"""Generates every SVG figure for week 01.

Run from anywhere:  python3 weeks/01-intro-complexity-testing/img/src/make_figures.py
Output goes to the img/ folder next to this src/ folder. No dependencies
beyond the Python standard library.

The doubling-experiment chart reads bench_sample.csv (output of
`w01_bench --csv` built with the release preset).
"""

import csv
import math
import struct
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE.parent

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

    def save(self, name):
        self.add("</svg>")
        (OUT / name).write_text("\n".join(self.parts) + "\n", encoding="utf-8")
        print("wrote", name)


def polyline(points):
    return "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in points)


# ---------------------------------------------------------------- pipeline

def compile_pipeline():
    s = Svg(1100, 430, "От изходен код до изпълним файл: препроцесор, компилатор, линкер")
    s.text(40, 46, "От изходен код до изпълним файл", size=24, weight=700)
    s.text(40, 74, "Всеки .cpp файл е отделна единица на транслация (translation unit) и се компилира самостоятелно.",
           size=15, fill=INK2)

    cols = [40, 250, 470, 690, 900]
    labels = ["изходен код", "препроцесор", "компилатор", "линкер", "резултат"]
    for x, lab in zip(cols, labels):
        s.text(x + 80, 116, lab.upper(), size=13, fill=MUTED, anchor="middle", weight=600)

    for row, (src, obj) in enumerate([("main.cpp", "main.o"), ("stats.cpp", "stats.o")]):
        y = 140 + row * 130
        s.box(cols[0], y, 160, 64, [src, "#include \"stats.h\""], fill=TINT1, stroke=S1, mono_first=True)
        s.line(cols[0] + 160, y + 32, cols[1], y + 32, arrow=True)
        s.box(cols[1], y, 160, 64, ["g++ -E", "#include → текст"], mono_first=True)
        s.line(cols[1] + 160, y + 32, cols[2], y + 32, arrow=True)
        s.box(cols[2], y, 160, 64, ["g++ -c", "→ " + obj], mono_first=True)
        s.line(cols[2] + 160, y + 32, cols[3], 236, arrow=True)

    s.box(cols[3], 204, 160, 64, ["ld", "свързва символите"], fill=TINT3, stroke=S3, mono_first=True)
    s.box(cols[3], 316, 160, 56, ["libstdc++", "std::vector, cout…"], fill=PANEL, mono_first=True)
    s.line(cols[3] + 80, 316, cols[3] + 80, 270, arrow=True)
    s.line(cols[3] + 160, 236, cols[4], 236, arrow=True)
    s.box(cols[4], 204, 160, 64, ["./app", "изпълним файл"], fill=TINT2, stroke=S2, mono_first=True)

    s.text(40, 408, "Грешка от компилатора (синтаксис, типове) идва от един .cpp; "
           "„undefined reference“ идва от линкера — липсва дефиниция.", size=15, fill=INK2)
    s.save("compile-pipeline.svg")


def cmake_workflow():
    s = Svg(1100, 360, "Работен цикъл с CMake: configure, build, test")
    s.text(40, 46, "CMake: три команди, една папка build/", size=24, weight=700)
    stages = [
        ("1 · configure", "cmake --preset default", "чете CMakeLists.txt,", "създава build/default/", TINT1, S1),
        ("2 · build", "cmake --build --preset default", "компилира и линква", "само променените файлове", TINT3, S3),
        ("3 · test", "ctest --preset default", "пуска тестовете,", "показва кои падат", TINT2, S2),
    ]
    x = 40
    for k, (title, cmd, l1, l2, fill, stroke) in enumerate(stages):
        s.rect(x, 90, 330, 170, fill, stroke)
        s.text(x + 20, 124, title, size=20, weight=700)
        s.rect(x + 14, 140, 302, 40, SURFACE, GRID, rx=6)
        s.text(x + 24, 166, cmd, size=14, family=MONO)
        s.text(x + 20, 212, l1, size=16, fill=INK2)
        s.text(x + 20, 236, l2, size=16, fill=INK2)
        if k < 2:
            s.line(x + 330, 175, x + 355, 175, arrow=True)
        x += 355
    s.path("M 900,260 C 900,320 560,320 560,262", INK2, sw=1.5, dash="5 5", arrow=True)
    s.text(730, 330, "промених .cpp → само 2 и 3", size=15, fill=INK2, anchor="middle", italic=True)
    s.text(40, 330, "1 се пуска веднъж (или при нов файл).", size=15, fill=INK2, italic=True)
    s.save("cmake-workflow.svg")


def red_green_refactor():
    s = Svg(900, 470, "Цикълът червено – зелено – рефакторинг")
    s.text(450, 44, "Червено → зелено → рефакторинг", size=24, weight=700, anchor="middle")
    cx, cy, r = 450, 265, 150
    nodes = [
        (-90, "ЧЕРВЕНО", "тест, който пада", RED, "#fbe3e3"),
        (30, "ЗЕЛЕНО", "минимален код,", GREEN, TINT3),
        (150, "РЕФАКТОРИНГ", "изчисти кода —", S1, TINT1),
    ]
    extra = {"ЗЕЛЕНО": "за да мине", "РЕФАКТОРИНГ": "тестовете остават зелени", "ЧЕРВЕНО": "(доказва, че тестът работи)"}
    pts = []
    for ang, name, desc, col, tint in nodes:
        a = math.radians(ang)
        x, y = cx + r * math.cos(a), cy + r * math.sin(a)
        pts.append((x, y))
    for k in range(3):
        a0 = math.radians(nodes[k][0] + 34)
        a1 = math.radians(nodes[(k + 1) % 3][0] - 34)
        if a1 < a0:
            a1 += 2 * math.pi
        x0, y0 = cx + r * math.cos(a0), cy + r * math.sin(a0)
        x1, y1 = cx + r * math.cos(a1), cy + r * math.sin(a1)
        s.path(f"M {x0:.1f},{y0:.1f} A {r},{r} 0 0 1 {x1:.1f},{y1:.1f}", INK2, sw=2, arrow=True)
    for (x, y), (ang, name, desc, col, tint) in zip(pts, nodes):
        s.rect(x - 112, y - 42, 224, 84, tint, col, rx=42, sw=2)
        s.text(x, y - 8, name, size=18, weight=700, anchor="middle")
        s.text(x, y + 13, desc, size=14, fill=INK2, anchor="middle")
        s.text(x, y + 30, extra[name], size=14, fill=INK2, anchor="middle")
    s.save("red-green-refactor.svg")


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


def growth_rates():
    s = Svg(1060, 560, "Скорост на растеж: log n, n, n log n, n², 2ⁿ")
    s.text(40, 44, "Колко бързо растат функциите", size=24, weight=700)
    s.text(40, 70, "Брой операции T(n) при n = 1…30 (оста y е отрязана на 200)", size=15, fill=INK2)
    x0, y0, w, h = 110, 100, 680, 380
    ymax, xmax = 200, 30
    fx = lambda n: x0 + (n - 1) / (xmax - 1) * w
    fy = lambda v: y0 + h - min(v, ymax) / ymax * h
    axes(s, x0, y0, w, h, [1, 5, 10, 15, 20, 25, 30], [0, 50, 100, 150, 200], "n — размер на входа",
         "T(n)", fx, fy)
    series = [
        (S1, "log₂ n", lambda n: math.log2(n)),
        (S2, "n", lambda n: n),
        (S3, "n log₂ n", lambda n: n * math.log2(n)),
        (S4, "n²", lambda n: n * n),
        (S5, "2ⁿ", lambda n: 2 ** n),
    ]
    for col, lab, f in series:
        pts, n = [], 1.0
        while n <= xmax:
            v = f(n)
            pts.append((fx(n), fy(v)))
            if v >= ymax:
                break
            n += 0.05
        s.path(polyline(pts), col, sw=2)
        ex, ey = pts[-1]
        s.circle(ex, ey, 4.5, col)
        if ey <= y0 + 1:
            s.text(ex, ey - 12, lab, size=15, anchor="middle", weight=600)
        else:
            s.text(ex + 10, ey + 5, lab, size=15, weight=600)
    legend(s, 830, 120, [(c, l) for c, l, _ in series])
    s.text(830, 270, "При n = 10⁶ и 10⁹", size=14, fill=INK2)
    s.text(830, 290, "операции/секунда:", size=14, fill=INK2)
    rows = [("log n", "20 ns"), ("n", "1 ms"), ("n log n", "20 ms"), ("n²", "17 мин"), ("2ⁿ", "∞ (практически)")]
    for k, (a, b) in enumerate(rows):
        s.text(830, 318 + k * 22, a, size=14, family=MONO)
        s.text(910, 318 + k * 22, b, size=14, fill=INK2)
    s.save("growth-rates.svg")


def asymptotic_bounds():
    s = Svg(1200, 430, "Графично значение на O, Ω и Θ")
    xmax, ymax = 20.0, 34.0
    f = lambda x: 0.9 * x + 3.2 * math.sin(0.9 * x) + 2.5
    panels = [
        ("f(n) ∈ O(g(n))", "горна граница", [(lambda x: 1.8 * x, "c·g(n)", S2)], lambda x: f(x) <= 1.8 * x),
        ("f(n) ∈ Ω(g(n))", "долна граница", [(lambda x: 0.85 * x, "c·g(n)", S2)], lambda x: f(x) >= 0.85 * x),
        ("f(n) ∈ Θ(g(n))", "точна граница",
         [(lambda x: 1.8 * x, "c₂·g(n)", S2), (lambda x: 0.85 * x, "c₁·g(n)", S4)],
         lambda x: 0.85 * x <= f(x) <= 1.8 * x),
    ]
    for k, (title, sub, bounds, ok) in enumerate(panels):
        px = 40 + k * 390
        x0, y0, w, h = px + 20, 100, 330, 250
        fx = lambda v, x0=x0, w=w: x0 + v / xmax * w
        fy = lambda v, y0=y0, h=h: y0 + h - min(v, ymax) / ymax * h
        s.text(px + 185, 46, title, size=21, weight=700, anchor="middle")
        s.text(px + 185, 72, sub, size=15, fill=INK2, anchor="middle")
        # n0 = smallest grid point after which the condition always holds
        xs = [i * 0.02 for i in range(1, int(xmax / 0.02) + 1)]
        n0 = xs[0]
        for x in xs:
            if not ok(x):
                n0 = x
        s.rect(fx(n0), y0, x0 + w - fx(n0), h, "#f2f7fd", rx=0)
        s.line(x0, y0 + h, x0 + w, y0 + h, stroke=MUTED, sw=1)
        s.line(x0, y0, x0, y0 + h, stroke=MUTED, sw=1)
        s.line(fx(n0), y0, fx(n0), y0 + h, stroke=INK2, sw=1, dash="4 4")
        s.text(fx(n0), y0 + h + 22, "n₀", size=15, anchor="middle", weight=600)
        s.text(x0 + w, y0 + h + 22, "n", size=15, fill=INK2, anchor="end", italic=True)
        for bf, lab, col in bounds:
            pts = [(fx(x), fy(bf(x))) for x in xs if bf(x) <= ymax]
            s.path(polyline(pts), col, sw=2)
            ex, ey = pts[-1]
            dy = 24 if bf(xmax) < f(xmax) else -8
            s.text(min(ex + 6, x0 + w - 4), ey + dy, lab, size=14, weight=600,
                   anchor="end" if ex + 60 > x0 + w else "start")
        pts = [(fx(x), fy(f(x))) for x in xs]
        s.path(polyline(pts), S1, sw=2.5)
        peak = max(xs, key=lambda x: f(x) if x < 15 else -1)
        s.text(fx(peak), fy(f(peak)) - 12, "f(n)", size=14, weight=600, anchor="middle")
    s.text(600, 410, "Вдясно от n₀ (оцветената зона) неравенството е в сила за всяко n. "
           "Какво става преди n₀, няма значение.", size=15, fill=INK2, anchor="middle")
    s.save("asymptotic-bounds.svg")


def doubling_experiment():
    rows = list(csv.DictReader((HERE / "bench_sample.csv").open()))
    s = Svg(1100, 560, "Измерено време на трите алгоритъма за дубликати, логаритмични оси")
    s.text(40, 44, "Експеримент с удвояване: три алгоритъма за дубликати", size=24, weight=700)
    s.text(40, 70, "Медиана от няколко пускания, вход без дубликати (най-лош случай), "
           "Release build, примерна машина", size=15, fill=INK2)
    x0, y0, w, h = 120, 100, 600, 370
    lx0, lx1 = 10, 20
    ly0, ly1 = -3, 3  # 10^-3 .. 10^3 ms
    fx = lambda n: x0 + (math.log2(n) - lx0) / (lx1 - lx0) * w
    fy = lambda ms: y0 + h - (math.log10(ms) - ly0) / (ly1 - ly0) * h
    axes(s, x0, y0, w, h, [2 ** k for k in range(10, 21, 2)], [10.0 ** e for e in range(-3, 4)],
         "n (логаритмична ос, ×2 на деление)", "време, ms (лог. ос)", fx, fy,
         xfmt=lambda n: f"2^{int(math.log2(n))}",
         yfmt=lambda v: {0.001: "0.001", 0.01: "0.01", 0.1: "0.1", 1: "1", 10: "10", 100: "100", 1000: "1000"}[v])
    series = [(S1, "naive — Θ(n²)", "naive_ms"), (S2, "sorting — Θ(n log n)", "sorting_ms"),
              (S3, "hashing — Θ(n) средно", "hashing_ms")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
        ex, ey = pts[-1]
        s.text(ex + 10, ey + 5 + (8 if key == "hashing_ms" else -8 if key == "sorting_ms" else 0),
               lab.split(" — ")[0], size=14, weight=600)
    legend(s, 830, 130, [(c, l) for c, l, _ in series])
    s.text(830, 230, "На лог-лог графика", size=14, fill=INK2)
    s.text(830, 250, "Θ(nᵏ) е права с наклон k:", size=14, fill=INK2)
    s.text(830, 274, "naive се изкачва 2 пъти", size=14, fill=INK2)
    s.text(830, 294, "по-стръмно от другите две.", size=14, fill=INK2)
    s.text(830, 330, "×2 на n ⇒ ×4 на времето", size=14, weight=600)
    s.text(830, 350, "за naive, ≈×2 за другите.", size=14, weight=600)
    s.save("doubling-experiment.svg")


# ---------------------------------------------------------------- loop pictures

def loop_triangle():
    n, cell = 8, 40
    s = Svg(820, 520, "Вложен цикъл j < i: оцветените клетки са n(n−1)/2")
    s.text(40, 44, "for i in [0, n): for j in [0, i)", size=22, weight=700, family=MONO)
    gx, gy = 110, 130
    for i in range(n):
        s.text(gx - 14, gy + i * cell + cell / 2 + 5, str(i), size=14, fill=INK2, anchor="end", family=MONO)
        for j in range(n):
            on = j < i
            s.rect(gx + j * cell + 1, gy + i * cell + 1, cell - 2, cell - 2,
                   S1 if on else PANEL, rx=4)
    for j in range(n):
        s.text(gx + j * cell + cell / 2, gy - 10, str(j), size=14, fill=INK2, anchor="middle", family=MONO)
    s.text(gx - 40, gy + n * cell / 2, "i", size=18, weight=700, anchor="middle", italic=True)
    s.text(gx + n * cell / 2, gy - 34, "j", size=18, weight=700, anchor="middle", italic=True)
    tx = gx + n * cell + 50
    s.text(tx, 140, "Ред i има точно i клетки.", size=17)
    s.text(tx, 180, "0 + 1 + 2 + … + (n−1)", size=17, family=MONO)
    s.text(tx, 212, "= n(n−1)/2", size=17, family=MONO, weight=600)
    s.text(tx, 252, f"n = {n}: {n * (n - 1) // 2} от {n * n} клетки", size=17)
    s.text(tx, 300, "≈ половината квадрат ⇒", size=17, fill=INK2)
    s.text(tx, 326, "пак Θ(n²).", size=17, weight=700)
    s.text(tx, 372, "Константата ½ изчезва", size=15, fill=INK2, italic=True)
    s.text(tx, 394, "в асимптотичния запис.", size=15, fill=INK2, italic=True)
    s.save("loop-triangle.svg")


def doubling_loop_frames():
    n, cell = 16, 50
    visits = [1, 2, 4, 8]
    for frame in range(len(visits) + 1):
        s = Svg(900, 300, f"Цикъл с удвояване, стъпка {frame + 1}")
        s.text(40, 44, "for (i = 1; i < n; i *= 2)   n = 16", size=21, weight=700, family=MONO)
        gx, gy = 50, 140
        seen = visits[:frame + 1] if frame < len(visits) else visits
        for k in range(n):
            col = S1 if k in seen else PANEL
            if frame < len(visits) and k == visits[frame]:
                col = S2
            s.rect(gx + k * cell + 2, gy, cell - 4, cell - 4, col, rx=6)
            s.text(gx + k * cell + cell / 2 - 2, gy + cell + 18, str(k), size=13, fill=INK2,
                   anchor="middle", family=MONO)
        for a, b in zip(seen, seen[1:]):
            xa, xb = gx + a * cell + cell / 2 - 2, gx + b * cell + cell / 2 - 2
            mid, lift = (xa + xb) / 2, 18 + (xb - xa) * 0.25
            s.path(f"M {xa:.1f},{gy - 4} Q {mid:.1f},{gy - 4 - lift:.1f} {xb:.1f},{gy - 4}", INK2, sw=1.5,
                   arrow=True)
        if frame < len(visits):
            s.text(40, 260, f"итерация {frame + 1}: i = {visits[frame]}", size=18, weight=600)
            s.text(860, 260, "всеки скок е двойно по-дълъг", size=16, fill=INK2, anchor="end", italic=True)
        else:
            s.text(40, 260, "i = 16 ≥ n → край.  4 итерации = log₂ 16", size=18, weight=600)
            s.text(860, 260, "удвои n → само +1 итерация", size=16, fill=INK2, anchor="end", italic=True)
        s.save(f"doubling-loop-{frame + 1}.svg")


# ---------------------------------------------------------------- doubles

def ieee754_layout():
    bits = format(struct.unpack(">Q", struct.pack(">d", 0.1))[0], "064b")
    s = Svg(1100, 330, "Битовото представяне на 0.1 като double (IEEE 754)")
    s.text(40, 44, "0.1 като double: 64 бита", size=24, weight=700)
    bw, gx, gy = 15.5, 40, 90
    for k, b in enumerate(bits):
        if k == 0:
            fill, stroke = TINT5, S5
        elif k < 12:
            fill, stroke = TINT2, S2
        else:
            fill, stroke = TINT1, S1
        x = gx + k * bw + (6 if k >= 1 else 0) + (6 if k >= 12 else 0)
        s.rect(x, gy, bw - 2, 34, fill, stroke, rx=3, sw=1)
        s.text(x + (bw - 2) / 2, gy + 23, b, size=13, anchor="middle", family=MONO)

    def brace(x1, x2, label, sub):
        y = gy + 46
        s.path(f"M {x1},{y} L {x1},{y + 8} L {x2},{y + 8} L {x2},{y}", MUTED, sw=1.5)
        s.text((x1 + x2) / 2, y + 30, label, size=15, weight=600, anchor="middle")
        s.text((x1 + x2) / 2, y + 50, sub, size=14, fill=INK2, anchor="middle")

    brace(gx, gx + bw - 2, "s", "")
    s.text(gx + 6, gy + 116, "знак", size=14, fill=INK2, anchor="middle")
    e_bits = bits[1:12]
    brace(gx + bw + 6, gx + 12 * bw + 4, "експонента: 11 бита",
          f"{int(e_bits, 2)} − 1023 = {int(e_bits, 2) - 1023}")
    brace(gx + 12 * bw + 12, gx + 64 * bw + 10, "мантиса (fraction): 52 бита",
          "1001 1001 1001 … 1010 — периодичната 0011 е отрязана и закръглена")
    s.text(40, 252, "стойност = (−1)ˢ × 1.fraction₂ × 2^(e − 1023)", size=17, family=MONO)
    s.text(40, 286, "0.1 се съхранява като 0.1000000000000000055511151231257827021181583404541015625",
           size=17, family=MONO, weight=600)
    s.text(40, 314, "Двоичният запис на 1/10 е безкраен (0.0001100110011…₂) — както 1/3 = 0.333… в десетична.",
           size=15, fill=INK2)
    s.save("ieee754-layout.svg")


def float_number_line():
    # Toy format: 3 mantissa bits, exponents -2..2 => 8 values per binade.
    s = Svg(1100, 330, "Плътност на числата с плаваща запетая върху реалната права")
    s.text(40, 44, "Числата с плаваща запетая не са равномерно разпределени", size=24, weight=700)
    s.text(40, 70, "Играчка-формат с 3 бита мантиса: 8 числа във всеки интервал [2ᵏ, 2ᵏ⁺¹). "
           "При double са 2⁵² във всеки интервал.", size=15, fill=INK2)
    x0, x1, y = 60, 960, 170
    vmax = 8.0
    fx = lambda v: x0 + v / vmax * (x1 - x0)
    s.line(x0, y, x1 + 40, y, stroke=MUTED, sw=1.5)
    values = set()
    for e in range(-2, 3):
        for m in range(8):
            values.add((1 + m / 8) * 2 ** e)
    for m in range(1, 8):          # subnormals fill the gap down to 0
        values.add(m / 8 * 2 ** -2)
    values.add(0.0)
    for v in sorted(values):
        col = S5 if v < 0.25 and v > 0 else S1
        s.line(fx(v), y - 14, fx(v), y + 14, stroke=col, sw=2)
    for e in range(-1, 4):
        v = 2.0 ** e
        s.text(fx(v), y + 40, f"{v:g}", size=14, fill=INK2, anchor="middle", family=MONO)
    s.text(fx(0), y + 40, "0", size=14, fill=INK2, anchor="middle", family=MONO)
    s.text(x1 + 50, y + 6, "+∞", size=20, weight=700)
    for (a, b, lab) in [(1, 2, "8 числа, стъпка 0.125"), (4, 8, "8 числа, стъпка 0.5")]:
        s.path(f"M {fx(a)},{y - 30} L {fx(a)},{y - 38} L {fx(b)},{y - 38} L {fx(b)},{y - 30}", MUTED, sw=1.5)
        s.text((fx(a) + fx(b)) / 2, y - 48, lab, size=14, anchor="middle", weight=600)
    legend_y = 262
    s.line(60, legend_y, 60, legend_y - 18, stroke=S1, sw=2)
    s.text(72, legend_y - 3, "нормални числа", size=14)
    s.line(230, legend_y, 230, legend_y - 18, stroke=S5, sw=2)
    s.text(242, legend_y - 3, "субнормални (запълват дупката около 0)", size=14)
    s.rect(640, legend_y - 26, 400, 60, PANEL, GRID)
    s.text(660, legend_y - 2, "NaN не е точка от правата:", size=14, weight=600)
    s.text(660, legend_y + 20, "не е <, >, нито == на нищо, дори на себе си.", size=14, fill=INK2)
    s.text(40, 312, "Отрицателните числа са огледален образ; −0 и +0 са различни битове, но −0 == +0.",
           size=15, fill=INK2)
    s.save("float-number-line.svg")


if __name__ == "__main__":
    compile_pipeline()
    cmake_workflow()
    red_green_refactor()
    growth_rates()
    asymptotic_bounds()
    doubling_experiment()
    loop_triangle()
    doubling_loop_frames()
    ieee754_layout()
    float_number_line()
