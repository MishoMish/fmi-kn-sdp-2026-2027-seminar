#!/usr/bin/env python3
"""Generates every SVG figure for week 03.

Run from anywhere:  python3 weeks/03-array-binary-search/img/src/make_figures.py
The search chart reads bench_sample.csv (`w03_bench --csv`, release preset).
"""

import csv
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"


def cells(s, x, y, values, w=56, h=44, fills=None, strokes=None, labels=True, idx=True, text_fill=None):
    for i, v in enumerate(values):
        fill = fills[i] if fills else PANEL
        stroke = strokes[i] if strokes else GRID
        s.rect(x + i * w, y, w - 4, h, fill, stroke, rx=6, sw=1.5)
        if labels and v != "":
            col = text_fill[i] if text_fill else INK
            s.text(x + i * w + (w - 4) / 2, y + h / 2 + 6, str(v), size=16, anchor="middle", family=MONO,
                   fill=col)
        if idx:
            s.text(x + i * w + (w - 4) / 2, y + h + 18, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)


# ---------------------------------------------------------------- structures

def linear_structures():
    s = Svg(1100, 420, "Линейна структура: абстракция и две представяния")
    s.text(40, 46, "Линейна структура: всеки елемент има предишен и следващ", size=24, weight=700)
    s.text(40, 72, "Една и съща абстракция — две коренно различни представяния в паметта", size=15, fill=INK2)
    for i, v in enumerate(["A", "B", "C", "D", "E"]):
        x = 60 + i * 130
        s.circle(x + 30, 130, 26, TINT1, stroke=S1)
        s.text(x + 30, 136, v, size=18, anchor="middle", weight=600)
        if i < 4:
            s.line(x + 60, 130, x + 126, 130, arrow=True)
    s.text(60, 186, "първи", size=13, fill=MUTED)
    s.text(610, 186, "последен", size=13, fill=MUTED)
    s.text(790, 124, "АТД: редица", size=17, weight=700)
    s.text(790, 148, "достъп по позиция, вмъкване,", size=14, fill=INK2)
    s.text(790, 168, "изтриване, обхождане подред", size=14, fill=INK2)

    s.text(40, 238, "масив — непрекъснат блок (седмици 03–04)", size=16, weight=600)
    cells(s, 40, 254, ["A", "B", "C", "D", "E"], fills=[TINT1] * 5, strokes=[S1] * 5, idx=False)
    s.text(40, 330, "позиция i → адрес начало + i·size: Θ(1)", size=14, fill=INK2)

    s.text(600, 238, "свързан списък — възли с указатели (седмици 05–06)", size=16, weight=600)
    xs = [600, 690, 780, 870, 960]
    ys = [258, 296, 254, 300, 262]
    for i, (x, y) in enumerate(zip(xs, ys)):
        s.rect(x, y, 44, 34, TINT2, S2, rx=5)
        s.text(x + 22, y + 23, "ABCDE"[i], size=15, anchor="middle", family=MONO)
        if i < 4:
            s.line(x + 44, y + 17, xs[i + 1], ys[i + 1] + 17, stroke=S2, sw=1.5, arrow=True)
    s.text(600, 370, "позиция i → минаваме през i възела: Θ(i)", size=14, fill=INK2)
    s.text(40, 404, "Стек, опашка и дек (седмица 07) са ограничени интерфейси върху едно от двете представяния.",
           size=15, fill=INK2)
    s.save(OUT / "linear-structures.svg")


def array_object():
    s = Svg(1000, 330, "Обектът FixedArray и буферът, който притежава")
    s.text(40, 46, "FixedArray<int>: малък обект + буфер в heap-а", size=24, weight=700)
    s.text(40, 100, "обектът (напр. в стека)", size=14, fill=MUTED, weight=600)
    fields = [("data_", "0x7f3a…"), ("size_", "3"), ("capacity_", "6")]
    for k, (name, val) in enumerate(fields):
        y = 112 + k * 44
        s.rect(40, y, 230, 40, TINT1, S1, rx=6)
        s.text(56, y + 26, name, size=15, family=MONO, weight=600)
        s.text(254, y + 26, val, size=15, family=MONO, fill=INK2, anchor="end")
    s.text(420, 100, "буферът в heap-а (capacity_ = 6 елемента)", size=14, fill=MUTED, weight=600)
    vals = ["4", "8", "15", "", "", ""]
    fills = [S1, S1, S1, PANEL, PANEL, PANEL]
    cells(s, 420, 112, vals, w=86, h=48, fills=fills, strokes=[S1] * 3 + [GRID] * 3,
          text_fill=[SURFACE] * 3 + [INK] * 3)
    s.path("M 270,132 C 330,132 360,136 418,136", S1, sw=2, arrow=True)
    s.path("M 420,196 L 420,206 L 674,206 L 674,196", MUTED, sw=1.5)
    s.text(547, 228, "използвани: size_ = 3", size=14, anchor="middle", weight=600)
    s.path("M 420,240 L 420,250 L 932,250 L 932,240", MUTED, sw=1.5)
    s.text(676, 272, "заделени: capacity_ = 6", size=14, anchor="middle", weight=600)
    s.text(40, 314, "Инвариант: 0 ≤ size_ ≤ capacity_. Обектът притежава буфера: деструкторът го освобождава.",
           size=15, fill=INK2)
    s.save(OUT / "array-object.svg")


def insert_frames():
    base = ["A", "B", "C", "D", "E", "", ""]
    steps = [
        (base, None, "insertAt(2, X): място 2 трябва да се освободи"),
        (["A", "B", "C", "D", "E", "E", ""], (4, 5), "стъпка 1: data[5] = data[4]  (отзад напред!)"),
        (["A", "B", "C", "D", "D", "E", ""], (3, 4), "стъпка 2: data[4] = data[3]"),
        (["A", "B", "C", "C", "D", "E", ""], (2, 3), "стъпка 3: data[3] = data[2]"),
        (["A", "B", "X", "C", "D", "E", ""], None, "запис: data[2] = X, size = 6 — общо size − index = 3 премествания"),
    ]
    for k, (vals, move, caption) in enumerate(steps):
        s = Svg(1000, 250, f"Вмъкване в масив, стъпка {k + 1}")
        s.text(40, 44, "Вмъкване в средата: всичко отдясно се мести", size=22, weight=700)
        fills, strokes = [], []
        for i, v in enumerate(vals):
            if k == 4 and i == 2:
                fills.append(S3)
                strokes.append(S3)
            elif move and i == move[1]:
                fills.append(S2)
                strokes.append(S2)
            elif v == "":
                fills.append(PANEL)
                strokes.append(GRID)
            else:
                fills.append(TINT1)
                strokes.append(S1)
        tf = [SURFACE if (f in (S2, S3)) else INK for f in fills]
        cells(s, 60, 100, vals, w=96, h=52, fills=fills, strokes=strokes, text_fill=tf)
        if move:
            a, b = move
            xa, xb = 60 + a * 96 + 46, 60 + b * 96 + 46
            s.path(f"M {xa},96 Q {(xa + xb) / 2},62 {xb},96", S2, sw=2, arrow=True)
        if k == 0:
            s.path("M 252,88 L 252,70", S3, sw=2)
            s.text(252, 64, "X", size=18, anchor="middle", weight=700)
        s.text(40, 226, caption, size=17, weight=600)
        s.save(OUT / f"insert-shift-{k + 1}.svg")


def shallow_deep():
    s = Svg(1100, 380, "Плитко срещу дълбоко копиране")
    s.text(40, 46, "Копиране на обект, който притежава памет", size=24, weight=700)
    panels = [
        (40, "плитко (копиращ конструктор по подразбиране)", RED, True),
        (580, "дълбоко (нашият копиращ конструктор)", GREEN, False),
    ]
    for px, title, col, shared in panels:
        s.text(px, 96, title, size=16, weight=600)
        for k, name in enumerate(["a", "b"]):
            y = 120 + k * 110
            s.rect(px, y, 120, 40, TINT1, S1, rx=6)
            s.text(px + 14, y + 26, f"{name}.data_", size=14, family=MONO)
        if shared:
            cells(s, px + 230, 170, ["1", "2", "3"], w=60, h=40, fills=[TINT2] * 3, strokes=[S2] * 3, idx=False)
            s.path(f"M {px + 120},140 C {px + 180},140 {px + 190},176 {px + 228},186", INK2, sw=1.6, arrow=True)
            s.path(f"M {px + 120},250 C {px + 180},250 {px + 190},210 {px + 228},196", INK2, sw=1.6, arrow=True)
            s.rect(px, 300, 500, 54, "#fbe3e3", RED, rx=8)
            s.text(px + 16, 324, "един буфер, два собственика:", size=15, weight=600)
            s.text(px + 16, 344, "b[0] = 9 променя и a; двата деструктора → двойно delete[]", size=14, fill=INK2)
        else:
            cells(s, px + 230, 120, ["1", "2", "3"], w=60, h=40, fills=[TINT2] * 3, strokes=[S2] * 3, idx=False)
            cells(s, px + 230, 230, ["1", "2", "3"], w=60, h=40, fills=[TINT3] * 3, strokes=[S3] * 3, idx=False)
            s.line(px + 120, 140, px + 228, 140, stroke=INK2, sw=1.6, arrow=True)
            s.line(px + 120, 250, px + 228, 250, stroke=INK2, sw=1.6, arrow=True)
            s.rect(px, 300, 480, 54, TINT3, GREEN, rx=8)
            s.text(px + 16, 324, "всеки обект има свой буфер:", size=15, weight=600)
            s.text(px + 16, 344, "промените са независими, всеки деструктор трие своето", size=14, fill=INK2)
    s.save(OUT / "shallow-deep-copy.svg")


# ---------------------------------------------------------------- binary search

VALUES = [2, 5, 8, 12, 16, 23, 38, 45, 56, 61, 72, 77, 85, 91, 95, 99]


def bs_frames():
    target = 61
    states = []
    lo, hi = 0, len(VALUES)
    while lo < hi:
        mid = lo + (hi - lo) // 2
        go_right = VALUES[mid] < target
        states.append((lo, hi, mid, go_right))
        if go_right:
            lo = mid + 1
        else:
            hi = mid
    states.append((lo, hi, None, None))
    for k, (lo, hi, mid, go_right) in enumerate(states):
        s = Svg(1100, 300, f"Двоично търсене на lower_bound(61), стъпка {k + 1}")
        s.text(40, 44, f"lowerBound(…, {target})", size=22, weight=700, family=MONO)
        fills, strokes, tf = [], [], []
        for i in range(len(VALUES)):
            if mid is not None and i == mid:
                f, st = S2, S2
            elif i < lo:
                f, st = TINT_RED, RED
            elif i >= hi:
                f, st = TINT3, S3
            else:
                f, st = SURFACE, INK2
            fills.append(f)
            strokes.append(st)
            tf.append(SURFACE if f == S2 else INK)
        cells(s, 40, 100, VALUES, w=64, h=50, fills=fills, strokes=strokes, text_fill=tf)
        x_of = lambda i: 40 + i * 64 - 2
        if lo == hi:
            s.text(x_of(lo), 92, "lo = hi", size=14, weight=700, family=MONO, anchor="middle")
        else:
            s.text(x_of(lo) + 2, 92, "lo", size=14, weight=700, family=MONO)
            s.text(x_of(hi) - 2, 92, "hi", size=14, weight=700, family=MONO,
                   anchor="end" if hi == len(VALUES) else "start")
        s.line(x_of(lo), 96, x_of(lo), 156, stroke=INK, sw=2)
        s.line(x_of(hi), 96, x_of(hi), 156, stroke=INK, sw=2)
        if mid is not None:
            rel = "<" if go_right else "≥"
            action = "lo = mid + 1" if go_right else "hi = mid"
            s.text(40, 214, f"стъпка {k + 1}: lo = {lo}, hi = {hi}, mid = {mid};  "
                            f"v[mid] = {VALUES[mid]} {rel} {target}  →  {action}", size=17, weight=600)
        else:
            s.text(40, 214, f"lo == hi == {lo}: отговорът е позиция {lo} (стойност {VALUES[lo]}). "
                            f"{len(states) - 1} стъпки (най-много ⌊log₂ 16⌋ + 1 = 5)", size=17, weight=600)
        s.rect(40, 240, 22, 18, TINT_RED, RED, rx=3, sw=1)
        s.text(70, 254, "знаем: < 61", size=14, fill=INK2)
        s.rect(200, 240, 22, 18, TINT3, S3, rx=3, sw=1)
        s.text(230, 254, "знаем: ≥ 61", size=14, fill=INK2)
        s.rect(360, 240, 22, 18, SURFACE, INK2, rx=3, sw=1)
        s.text(390, 254, "непроверени: [lo, hi)", size=14, fill=INK2)
        s.save(OUT / f"binary-search-{k + 1}.svg")
    return len(states)


def invariant():
    s = Svg(1100, 260, "Инвариантът на двоичното търсене")
    s.text(40, 46, "Инвариантът: три области, само средната е неизвестна", size=24, weight=700)
    regions = [(40, 360, TINT_RED, RED, "[first, lo)", "всичко тук е < value"),
               (400, 300, SURFACE, INK2, "[lo, hi)", "непроверени"),
               (700, 360, TINT3, S3, "[hi, last)", "всичко тук е ≥ value")]
    for x, w, fill, col, name, desc in regions:
        s.rect(x, 90, w - 6, 70, fill, col, rx=8, sw=2)
        s.text(x + (w - 6) / 2, 122, name, size=18, anchor="middle", weight=700, family=MONO)
        s.text(x + (w - 6) / 2, 146, desc, size=15, anchor="middle", fill=INK2)
    s.text(397, 82, "lo", size=15, weight=700, family=MONO, anchor="middle")
    s.text(697, 82, "hi", size=15, weight=700, family=MONO, anchor="middle")
    s.text(40, 204, "Всяка стъпка гледа mid в средата на [lo, hi) и премества lo или hi — средната област намалява",
           size=15, fill=INK2)
    s.text(40, 228, "поне наполовина. Когато е празна (lo == hi), lo е отговорът. Нищо не се „търси“ — само се стеснява.",
           size=15, fill=INK2)
    s.save(OUT / "binary-search-invariant.svg")


def bounds():
    vals = [1, 3, 3, 3, 7, 9]
    s = Svg(1000, 360, "lower_bound, upper_bound и equal_range при повторения")
    s.text(40, 46, "lower_bound и upper_bound при повторения", size=24, weight=700)
    fills = [PANEL, TINT1, TINT1, TINT1, PANEL, PANEL]
    cells(s, 200, 140, vals, w=90, h=52, fills=fills, strokes=[GRID, S1, S1, S1, GRID, GRID])
    end_x = 200 + 6 * 90
    s.rect(end_x, 140, 86, 52, SURFACE, GRID, rx=6, dash="5 4")
    s.text(end_x + 43, 172, "end", size=14, anchor="middle", fill=MUTED, family=MONO)

    def mark(i, label, y, col):
        x = 200 + i * 90 - 2
        s.line(x, 132, x, y + 6, stroke=col, sw=2.5)
        s.text(x, y, label, size=15, anchor="middle", weight=600, family=MONO)

    mark(1, "lower_bound(3) = 1", 104, S1)
    mark(4, "upper_bound(3) = 4", 104, S3)
    s.path("M 290,226 L 290,236 L 556,236 L 556,226", S1, sw=1.5)
    s.text(423, 258, "equal_range(3) = [1, 4): трите тройки", size=15, anchor="middle", weight=600)
    s.text(40, 302, "lower_bound(5) = 4 = upper_bound(5): 5 липсва, но 4 е мястото, където би се вмъкнало.",
           size=15, fill=INK2)
    s.text(40, 328, "Брой срещания на x = upper_bound(x) − lower_bound(x), за Θ(log n).", size=15, fill=INK2)
    s.save(OUT / "lower-upper-bound.svg")


def first_true():
    n = 20
    xs = list(range(0, 9))
    s = Svg(1000, 300, "Двоично търсене по отговора: монотонен предикат")
    s.text(40, 46, "Двоично търсене по отговора", size=24, weight=700)
    s.text(40, 72, f"pred(x) = x·x > {n}: false, false, …, false, true, …, true — търсим първото true",
           size=15, fill=INK2)
    vals = [("T" if x * x > n else "F") for x in xs]
    fills = [TINT3 if v == "T" else TINT_RED for v in vals]
    strokes = [S3 if v == "T" else RED for v in vals]
    cells(s, 60, 120, vals, w=90, h=50, fills=fills, strokes=strokes, idx=False)
    for i, x in enumerate(xs):
        s.text(60 + i * 90 + 43, 192, f"x={x}", size=13, anchor="middle", fill=INK2, family=MONO)
        s.text(60 + i * 90 + 43, 212, f"{x * x}", size=13, anchor="middle", fill=MUTED, family=MONO)
    first = next(i for i, v in enumerate(vals) if v == "T")
    x = 60 + first * 90 - 3
    s.line(x, 110, x, 180, stroke=INK, sw=2.5)
    s.text(x, 104, "firstTrue = 5", size=15, anchor="middle", weight=700, family=MONO)
    s.text(40, 254, "⌊√20⌋ = firstTrue − 1 = 4. Няма масив — „масивът“ е предикатът, който пресмятаме при нужда.",
           size=15, fill=INK2)
    s.text(40, 278, "Работи за всеки монотонен въпрос: минимален капацитет, най-малко време, първа лоша версия…",
           size=15, fill=INK2)
    s.save(OUT / "first-true.svg")


def search_chart():
    rows = list(csv.DictReader((HERE / "bench_sample.csv").open()))
    s = Svg(1100, 520, "Линейно срещу двоично търсене: време на заявка спрямо n")
    s.text(40, 44, "Линейно срещу двоично търсене", size=24, weight=700)
    s.text(40, 70, "Средно време на едно търсене в сортиран std::vector<int>; Release, примерна машина",
           size=15, fill=INK2)
    x0, y0, w, h = 110, 100, 620, 330
    fx = lambda n: x0 + (math.log2(n) - 4) / 20 * w
    fy = lambda ns: y0 + h - (math.log10(ns) - 0) / 4 * h
    axes(s, x0, y0, w, h, [2 ** k for k in range(4, 25, 4)], [1, 10, 100, 1000, 10000],
         "n (лог. ос)", "ns на търсене (лог. ос)", fx, fy, xfmt=lambda n: f"2^{int(math.log2(n))}",
         yfmt=lambda v: f"{v:g}")
    series = [(S2, "линейно, Θ(n)", "linear_ns"), (S1, "двоично, Θ(log n)", "yours_ns")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
        s.text(pts[-1][0] + 10, pts[-1][1] + 5, lab.split(",")[0], size=14, weight=600)
    legend(s, 850, 130, [(c, l) for c, l, _ in series])
    last = rows[-1]
    s.text(850, 210, f"n = 2²⁴ ≈ 16.8 млн:", size=14, fill=INK2)
    s.text(850, 232, f"двоично — {float(last['yours_ns']):.0f} ns", size=14, weight=600)
    s.text(850, 254, "(24 стъпки)", size=14, fill=INK2)
    s.text(850, 290, "При големи n всяка стъпка", size=14, fill=INK2)
    s.text(850, 310, "е промах в кеша — затова", size=14, fill=INK2)
    s.text(850, 330, "кривата се огъва нагоре.", size=14, fill=INK2)
    s.text(40, 500, "std::binary_search дава почти същите числа като вашето lowerBound — същият алгоритъм.",
           size=15, fill=INK2)
    s.save(OUT / "search-chart.svg")


if __name__ == "__main__":
    linear_structures()
    array_object()
    insert_frames()
    shallow_deep()
    print("binary search frames:", bs_frames())
    invariant()
    bounds()
    first_true()
    search_chart()
