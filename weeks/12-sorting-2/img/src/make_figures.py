#!/usr/bin/env python3
"""Generates every SVG figure for week 12.

Run from anywhere:  python3 weeks/12-sorting-2/img/src/make_figures.py
The charts read bench_sample.csv (`w12_bench --csv`, release preset) and
comparisons_sample.txt (comparison counts of the reference solutions,
n = 65 536, measured with tests/counting.h).
"""

import csv
import math
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"
YELLOW = "#fff4d6"
ORANGE = (TINT2, S2)
GREEN = (TINT3, S3)
BLUE = (TINT1, S1)
GREY = (PANEL, GRID)
REDC = (TINT_RED, RED)


def cell(s, x, y, w, h, text, fill=TINT1, stroke=S1, tf=INK, size=16):
    s.rect(x, y, w, h, fill, stroke, rx=5, sw=1.4)
    if text != "":
        s.text(x + w / 2, y + h / 2 + size * 0.36, str(text), size=size, anchor="middle", family=MONO, weight=700,
               fill=tf)


def draw_array(s, a, x0, y0, w=44, h=38, hi=None, size=16):
    hi = hi or {}
    for i, v in enumerate(a):
        f, st = hi.get(i, BLUE)
        cell(s, x0 + i * w, y0, w - 4, h, v, fill=f, stroke=st, size=size)


# ---------------------------------------------------------------- merge sort

def merge_trace():
    a, b = [1, 4, 7, 9], [2, 3, 8]
    s = Svg(1100, 480, "Сливане на два сортирани масива")
    s.text(40, 46, "Сливане: по-малкият от двата „върха“ отива на изхода", size=24, weight=700)
    i = j = 0
    out = []
    rows = []
    while i < len(a) or j < len(b):
        if j == len(b) or (i < len(a) and not b[j] < a[i]):
            out.append(a[i])
            took = ("L", i)
            i += 1
        else:
            out.append(b[j])
            took = ("R", j)
            j += 1
        rows.append((i, j, list(out), took))
    for k, (i, j, o, took) in enumerate(rows[:6]):
        y = 90 + k * 56
        hiA = {x: GREY for x in range(i)}
        hiB = {x: GREY for x in range(j)}
        if took[0] == "L":
            hiA[took[1]] = ORANGE
        else:
            hiB[took[1]] = ORANGE
        draw_array(s, a, 40, y, w=40, h=34, hi=hiA, size=14)
        draw_array(s, b, 220, y, w=40, h=34, hi=hiB, size=14)
        s.text(370, y + 23, "→", size=18, fill=INK2)
        draw_array(s, o, 400, y, w=40, h=34, hi={len(o) - 1: ORANGE} | {x: GREEN for x in range(len(o) - 1)},
                   size=14)
    s.text(40, 80, "ляво", size=13, fill=MUTED)
    s.text(220, 80, "дясно", size=13, fill=MUTED)
    s.text(400, 80, "изход", size=13, fill=MUTED)
    s.text(40, 440, "…после остатъкът от ляво (9) се копира наведнъж. ≤ n − 1 сравнения, Θ(n) време, Θ(n) допълнителна памет.",
           size=14, fill=INK2)
    s.text(760, 120, "При равни: от ЛЯВОТО първо", size=15, weight=700)
    s.text(760, 144, "→ равните запазват реда си:", size=14, fill=INK2)
    s.text(760, 166, "merge sort е стабилен", size=14, fill=INK2)
    s.text(760, 210, "if (comp(right, left))", size=14, family=MONO)
    s.text(794, 232, "take right;", size=14, family=MONO)
    s.text(760, 254, "else", size=14, family=MONO)
    s.text(794, 276, "take left;", size=14, family=MONO)
    s.save(OUT / "merge-trace.svg")


def mergesort_tree():
    data = [5, 2, 4, 7, 1, 3, 2, 6]
    s = Svg(1100, 560, "Merge sort: делим наполовина, сливаме нагоре")
    s.text(40, 46, "Merge sort: делим до единични елементи, после сливаме", size=24, weight=700)
    w = 34
    levels = [[data]]
    while len(levels[-1][0]) > 1:
        nxt = []
        for part in levels[-1]:
            m = len(part) // 2
            nxt += [part[:m], part[m:]]
        levels.append(nxt)
    # split phase: rows 0..3 top, merge phase: rows back up
    gap = 26

    def row(y, parts, hi):
        total = sum(len(p) for p in parts) * w + (len(parts) - 1) * gap
        x = 550 - total / 2
        for p in parts:
            draw_array(s, p, x, y, w=w, h=30, hi={i: hi for i in range(len(p))}, size=13)
            x += len(p) * w + gap
    for k, parts in enumerate(levels):
        row(80 + k * 52, parts, BLUE if k < 3 else GREY)
    merged = levels[-1]
    k = len(levels)
    while len(merged) > 1:
        merged = [sorted(merged[i] + merged[i + 1]) for i in range(0, len(merged), 2)]
        row(80 + k * 52, merged, GREEN)
        k += 1
    s.text(40, 112, "делене", size=14, fill=MUTED)
    s.text(40, 112 + 3 * 52, "единични", size=14, fill=MUTED)
    s.text(40, 112 + 5 * 52, "сливане", size=14, fill=MUTED)
    s.text(860, 160, "log₂ n нива", size=16, weight=700)
    s.text(860, 186, "всяко ниво сливане:", size=14, fill=INK2)
    s.text(860, 208, "Θ(n) общо", size=14, fill=INK2)
    s.text(860, 248, "T(n) = 2T(n/2) + Θ(n)", size=15, family=MONO, weight=700)
    s.text(860, 272, "     = Θ(n log n)", size=15, family=MONO, weight=700)
    s.text(860, 312, "винаги — дори при", size=14, fill=INK2)
    s.text(860, 334, "сортиран вход", size=14, fill=INK2)
    s.save(OUT / "mergesort-tree.svg")


# ---------------------------------------------------------------- quicksort

def partition_frames():
    a = [5, 1, 9, 5, 3, 5, 8, 2, 5]
    pivot = 5
    lt, i, gt = 0, 0, len(a)
    frames = [(list(a), lt, i, gt, "начало: lt = i = 0, gt = n; pivot = 5")]
    while i < gt:
        if a[i] < pivot:
            a[lt], a[i] = a[i], a[lt]
            msg = f"{a[lt]} < 5 → размени с a[lt]; lt++, i++"
            lt += 1
            i += 1
        elif a[i] > pivot:
            gt -= 1
            a[i], a[gt] = a[gt], a[i]
            msg = f"{a[gt]} > 5 → размени с a[gt − 1]; gt−− (i остава!)"
        else:
            msg = "= 5 → i++"
            i += 1
        frames.append((list(a), lt, i, gt, msg))
    s = Svg(1100, 90 + len(frames) * 44 + 70, "Тристранно разделяне на Дейкстра")
    s.text(40, 46, "Тристранно разделяне (Dutch national flag): < | = | ? | >", size=24, weight=700)
    for k, (arr, l, ii, g, msg) in enumerate(frames):
        y = 80 + k * 44
        hi = {}
        for x in range(len(arr)):
            if x < l:
                hi[x] = BLUE
            elif x < ii:
                hi[x] = ORANGE
            elif x < g:
                hi[x] = GREY
            else:
                hi[x] = REDC
        draw_array(s, arr, 40, y, w=40, h=34, hi=hi, size=14)
        s.text(420, y + 23, msg, size=13, family=MONO, fill=INK2)
    y = 90 + len(frames) * 44
    for k, (col, lab) in enumerate([(BLUE, "< pivot  [first, lt)"), (ORANGE, "= pivot  [lt, i)"),
                                    (GREY, "непрегледани  [i, gt)"), (REDC, "> pivot  [gt, last)")]):
        s.rect(40 + k * 250, y, 18, 18, col[0], col[1], rx=3)
        s.text(66 + k * 250, y + 14, lab, size=13, family=MONO)
    s.text(40, y + 48, "Всички равни на pivot излизат от рекурсията наведнъж — масив от еднакви елементи: един проход.",
           size=14, fill=INK2)
    s.save(OUT / "partition3.svg")


def pivot_choice():
    s = Svg(1100, 470, "Изборът на pivot")
    s.text(40, 46, "Изборът на pivot решава дълбочината на рекурсията", size=24, weight=700)
    # left: sorted input with first-element pivot -> chain
    s.text(40, 92, "сортиран вход, pivot = първият", size=16, weight=700, fill=RED)
    for k in range(7):
        y = 112 + k * 40
        n = 8 - k
        x0 = 40 + k * 20
        cell(s, x0, y, 30, 28, k + 1, fill=TINT_RED, stroke=RED, size=12)
        if n > 1:
            s.rect(x0 + 34, y, (n - 1) * 24, 28, PANEL, GRID, rx=4)
            s.text(x0 + 40 + (n - 1) * 24, y + 19, f"+ {n - 1} " + ("елемент" if n == 2 else "елемента"), size=12,
                   fill=MUTED)
    s.text(40, 410, "n нива × до n сравнения: Θ(n²)", size=15, weight=700, fill=RED)
    s.text(40, 434, "и n рекурсивни извиквания: стекът прелива", size=14, fill=INK2)
    # right: balanced
    s.text(600, 92, "случаен pivot (или медиана от три)", size=16, weight=700, fill=S3)

    def node(x, y, w, label):
        s.rect(x - w / 2, y, w, 28, TINT3, S3, rx=4)
        s.text(x, y + 19, label, size=12, anchor="middle", family=MONO)
    widths = [320, 150, 70, 30]
    for lvl in range(4):
        cnt = 2 ** lvl
        for k in range(cnt):
            x = 600 + 340 * (k + 0.5) / cnt
            node(x, 112 + lvl * 56, widths[lvl] - 6, f"n/{2 ** lvl}" if lvl else "n")
    s.text(600, 360, "≈ log₂ n нива × Θ(n) на ниво", size=15, weight=700, fill=S3)
    s.text(600, 384, "очаквано ≈ 1.39 n log₂ n сравнения", size=14, fill=INK2)
    s.text(600, 410, "(като BST от случаен ред — седмица 09)", size=14, fill=INK2)
    s.text(600, 434, "Най-лошото остава Θ(n²), но с вероятност ≈ 0", size=14, fill=INK2)
    s.save(OUT / "pivot-choice.svg")


# ---------------------------------------------------------------- lower bound

def decision_tree():
    s = Svg(1100, 470, "Дърво на решенията за 3 елемента")
    s.text(40, 46, "Долна граница: всяко сравнително сортиране е дърво на решенията", size=22, weight=700)

    def q(x, y, t):
        s.rect(x - 44, y - 16, 88, 32, TINT1, S1, rx=16)
        s.text(x, y + 5, t, size=14, anchor="middle", family=MONO, weight=700)

    def leaf(x, y, t):
        s.rect(x - 46, y - 15, 92, 30, TINT3, S3, rx=5)
        s.text(x, y + 5, t, size=13, anchor="middle", family=MONO, weight=700)

    def e(x1, y1, x2, y2, lab):
        s.line(x1, y1 + 16, x2, y2 - 16, stroke=INK2, sw=1.4)
        s.text((x1 + x2) / 2 + (-12 if x2 < x1 else 12), (y1 + y2) / 2, lab, size=12,
               anchor="end" if x2 < x1 else "start", fill=MUTED)
    q(430, 100, "a < b ?")
    q(230, 190, "b < c ?")
    q(630, 190, "a < c ?")
    e(430, 100, 230, 190, "да")
    e(430, 100, 630, 190, "не")
    leaf(130, 280, "a b c")
    q(330, 280, "a < c ?")
    e(230, 190, 130, 280, "да")
    e(230, 190, 330, 280, "не")
    leaf(260, 370, "a c b")
    leaf(400, 370, "c a b")
    e(330, 280, 260, 370, "да")
    e(330, 280, 400, 370, "не")
    leaf(530, 280, "b a c")
    q(730, 280, "b < c ?")
    e(630, 190, 530, 280, "да")
    e(630, 190, 730, 280, "не")
    leaf(660, 370, "b c a")
    leaf(800, 370, "c b a")
    e(730, 280, 660, 370, "да")
    e(730, 280, 800, 370, "не")
    s.text(870, 110, "3! = 6 листа", size=15, weight=700)
    s.text(870, 136, "височина ≥ ⌈log₂ 6⌉ = 3", size=14, fill=INK2)
    s.text(870, 180, "n елемента:", size=15, weight=700)
    s.text(870, 206, "≥ n! листа", size=14, fill=INK2)
    s.text(870, 230, "височина ≥ log₂ n!", size=14, fill=INK2)
    s.text(870, 254, "≈ n log₂ n − 1.44 n", size=14, fill=INK2)
    s.text(40, 440, "Височина = сравнения в най-лошия случай. Ω(n log n) за всяко сортиране само със сравнения.",
           size=14, fill=INK2)
    s.save(OUT / "decision-tree.svg")


# ---------------------------------------------------------------- non-comparison

def counting_sort():
    a = [(3, "a"), (1, "b"), (3, "c"), (0, "d"), (1, "e"), (3, "f")]
    k = 4
    s = Svg(1100, 470, "Сортиране чрез броене")
    s.text(40, 46, "Сортиране чрез броене: преброй, сумирай, разположи", size=24, weight=700)
    s.text(40, 92, "① вход (ключ, етикет)", size=15, weight=700)
    for i, (key, tag) in enumerate(a):
        cell(s, 40 + i * 64, 104, 58, 36, f"{key}{tag}", size=14)
    count = [0] * k
    for key, _ in a:
        count[key] += 1
    s.text(40, 182, "② count[ключ]", size=15, weight=700)
    for i, c in enumerate(count):
        cell(s, 40 + i * 64, 194, 58, 36, c, fill=YELLOW, stroke=S4, size=15)
        s.text(40 + i * 64 + 29, 246, str(i), size=11, anchor="middle", fill=MUTED, family=MONO)
    start = [0] * k
    for i in range(1, k):
        start[i] = start[i - 1] + count[i - 1]
    s.text(40, 284, "③ start[ключ] — префиксни суми: къде започва всеки ключ", size=15, weight=700)
    for i, c in enumerate(start):
        cell(s, 40 + i * 64, 296, 58, 36, c, fill=TINT1, stroke=S1, size=15)
    out = [None] * len(a)
    st = list(start)
    for key, tag in a:
        out[st[key]] = f"{key}{tag}"
        st[key] += 1
    s.text(40, 374, "④ обхождаме входа ОТЛЯВО НАДЯСНО: out[start[ключ]++] = елемент", size=15, weight=700)
    for i, v in enumerate(out):
        cell(s, 40 + i * 64, 386, 58, 36, v, fill=TINT3, stroke=S3, size=14)
    s.text(500, 130, "Без нито едно сравнение.", size=15, weight=700)
    s.text(500, 156, "Θ(n + k) време и памет, k = брой ключове", size=14, fill=INK2)
    s.text(500, 210, "Стабилно: 3a, 3c, 3f остават в този ред —", size=14, fill=INK2)
    s.text(500, 232, "затова обхождаме входа по ред.", size=14, fill=INK2)
    s.text(500, 286, "Само за малки цели ключове:", size=14, fill=INK2)
    s.text(500, 308, "оценки, възраст, байт (k = 256).", size=14, fill=INK2)
    s.text(500, 330, "При k = 2^32 — 16 GB за броячите.", size=14, fill=RED)
    s.save(OUT / "counting-sort.svg")


def radix_trace():
    a = [170, 45, 75, 90, 802, 24, 2, 66]
    s = Svg(1100, 420, "Поразрядно сортиране (LSD)")
    s.text(40, 46, "Поразрядно сортиране (LSD): стабилно по единици, десетици, стотици", size=22, weight=700)
    rows = [(list(a), None, "вход")]
    cur = list(a)
    for d, name in [(1, "по единици"), (10, "по десетици"), (100, "по стотици")]:
        cur = sorted(cur, key=lambda x: (x // d) % 10)  # Python's sort is stable
        rows.append((list(cur), d, name))
    for k, (arr, d, name) in enumerate(rows):
        y = 90 + k * 64
        for i, v in enumerate(arr):
            txt = f"{v:03d}"
            x = 40 + i * 92
            s.rect(x, y, 84, 40, TINT3 if k == 3 else TINT1, S3 if k == 3 else S1, rx=5, sw=1.4)
            for c in range(3):
                place = 10 ** (2 - c)
                hot = d == place
                s.text(x + 22 + c * 20, y + 27, txt[c], size=17, anchor="middle", family=MONO, weight=700,
                       fill=S2 if hot else (INK if k == 0 or place < (d or 1) * 10 else MUTED))
        s.text(800, y + 26, name, size=15, fill=INK2)
    s.text(40, 380, "Всеки проход е стабилно сортиране чрез броене по една цифра. Стабилността пази наредбата от предишните",
           size=14, fill=INK2)
    s.text(40, 402, "проходи. d цифри в база b: Θ(d(n + b)). 32-битови числа, b = 256: 4 прохода — задача 5.", size=14,
           fill=INK2)
    s.save(OUT / "radix-trace.svg")


def bucket():
    xs = [0.78, 0.17, 0.39, 0.26, 0.72, 0.94, 0.21, 0.12, 0.23, 0.68]
    n = len(xs)
    s = Svg(1100, 420, "Сортиране с кофи")
    s.text(40, 46, "Сортиране с кофи: x отива в кофа ⌊x · n⌋", size=24, weight=700)
    for i, x in enumerate(xs):
        cell(s, 40 + i * 70, 80, 64, 34, f"{x:.2f}", size=14)
    buckets = [[] for _ in range(n)]
    for x in xs:
        buckets[int(x * n)].append(x)
    for b in range(n):
        x = 40 + b * 70
        s.text(x + 32, 160, str(b), size=12, anchor="middle", fill=MUTED, family=MONO)
        s.rect(x, 168, 64, 130, PANEL, GRID, rx=6)
        for j, v in enumerate(sorted(buckets[b])):
            cell(s, x + 4, 174 + j * 40, 56, 34, f"{v:.2f}", fill=TINT3, stroke=S3, size=13)
    out = sorted(xs)
    for i, x in enumerate(out):
        cell(s, 40 + i * 70, 316, 64, 34, f"{x:.2f}", fill=TINT3, stroke=S3, size=14)
    s.text(40, 384, "Равномерни данни → средно по 1 елемент в кофа → Θ(n) очаквано. Всичко в една кофа → колкото",
           size=14, fill=INK2)
    s.text(40, 406, "алгоритъма в кофата (insertion sort: Θ(n²)). Задача 6.", size=14, fill=INK2)
    s.save(OUT / "bucket.svg")


# ---------------------------------------------------------------- charts

STYLE = {"merge": S1, "quick": S2, "radix": S3, "std::sort": INK, "std::stable_sort": MUTED}
LABEL = {"merge": "вашият merge sort", "quick": "вашият quicksort (3-way)", "radix": "вашият radix sort",
         "std::sort": "std::sort (introsort)", "std::stable_sort": "std::stable_sort"}


def time_chart(rows):
    s = Svg(1100, 520, "Време за сортиране, случаен вход")
    s.text(40, 44, "Време за сортиране на n случайни uint32 (ms)", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина", size=15, fill=INK2)
    x0, y0, w, h0 = 100, 100, 560, 340
    fx = lambda n: x0 + (math.log10(n) - 4) / 3 * w
    fy = lambda v: y0 + h0 - (math.log10(v) + 2) / 5 * h0
    axes(s, x0, y0, w, h0, [10 ** 4, 10 ** 5, 10 ** 6, 10 ** 7], [0.01, 0.1, 1, 10, 100, 1000], "n (лог. ос)",
         "ms (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    for algo, col in STYLE.items():
        pts = [(fx(int(r["n"])), fy(float(r["ms"]))) for r in rows if r["algorithm"] == algo and r["input"] == "random"]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 700, 120, [(STYLE[a], LABEL[a]) for a in STYLE])
    get = lambda a: float(next(r["ms"] for r in rows if r["n"] == "10000000" and r["algorithm"] == a
                               and r["input"] == "random"))
    s.text(700, 270, "n = 10^7:", size=14, weight=600)
    s.text(700, 292, f"radix {get('radix'):.0f} ms, std::sort {get('std::sort'):.0f} ms,", size=14, fill=INK2)
    s.text(700, 314, f"merge {get('merge'):.0f} ms, quick {get('quick'):.0f} ms", size=14, fill=INK2)
    s.text(700, 350, f"Radix sort: ~{get('std::sort') / get('radix'):.0f}× по-бърз от std::sort —", size=14, fill=INK2)
    s.text(700, 372, "без сравнения, 4 последователни прохода.", size=14, fill=INK2)
    s.save(OUT / "time-chart.svg")


def input_chart(rows):
    n = "1000000"
    algos = ["merge", "quick", "radix", "std::sort", "std::stable_sort"]
    kinds = [("random", "случаен", S1), ("sorted", "сортиран", S3), ("reversed", "обратен", RED),
             ("few", "10 различни стойности", S4)]
    s = Svg(1100, 500, "Влияние на входа, n = 10^6")
    s.text(40, 44, "Различен вход, n = 1 000 000 (ms)", size=24, weight=700)
    x0, y0, w, h0 = 100, 90, 900, 300
    vmax = 60
    fy = lambda v: y0 + h0 - v / vmax * h0
    for t in range(0, 61, 10):
        y = fy(t)
        s.line(x0, y, x0 + w, y, stroke=GRID, sw=1)
        s.text(x0 - 10, y + 5, str(t), size=13, fill=INK2, anchor="end")
    gw = w / len(algos)
    for g, algo in enumerate(algos):
        for k, (kind, _, col) in enumerate(kinds):
            v = float(next(r["ms"] for r in rows if r["n"] == n and r["algorithm"] == algo and r["input"] == kind))
            bx = x0 + g * gw + 16 + k * (gw - 32) / 4
            bw = (gw - 32) / 4 - 4
            s.add(f'<rect x="{bx:.1f}" y="{fy(v):.1f}" width="{bw:.1f}" height="{y0 + h0 - fy(v):.1f}" rx="3" '
                  f'fill="{col}"/>')
            s.text(bx + bw / 2, fy(v) - 6, f"{v:.0f}" if v >= 10 else f"{v:.1f}", size=11, anchor="middle", fill=INK2)
        s.text(x0 + g * gw + gw / 2, y0 + h0 + 24, LABEL[algo].replace("вашият ", ""), size=13, anchor="middle",
               weight=600)
    s.line(x0, y0 + h0, x0 + w, y0 + h0, stroke=MUTED, sw=1)
    for k, (_, lab, col) in enumerate(kinds):
        s.rect(120 + k * 210, 440, 16, 16, col, "none", rx=3)
        s.text(144 + k * 210, 453, lab, size=14)
    s.text(40, 486, "Тристранното разделяне лети при малко различни стойности. Radix не забелязва входа. Merge е 5× по-бърз на сортиран.",
           size=14, fill=INK2)
    s.save(OUT / "input-chart.svg")


def comparisons_chart():
    text = (HERE.parent / "comparisons_sample.txt").read_text()
    vals = {m.group(1).strip(): float(m.group(2)) for m in re.finditer(r"^(.+?)\s+\d+\s+=\s+([\d.]+) n log2 n", text, re.M)}
    lb = re.search(r"log2\(n!\) = (\d+), n log2 n = (\d+)", text)
    bound = int(lb.group(1)) / int(lb.group(2))
    items = [("mergeSort random", "merge sort, случаен"), ("mergeSort sorted", "merge sort, сортиран"),
             ("std::sort random", "std::sort, случаен"), ("quickSort random", "quicksort 3-way, случаен"),
             ("quickSort 10 values", "quicksort 3-way, 10 стойности"), ("std::sort 10 values", "std::sort, 10 стойности")]
    s = Svg(1100, 420, "Брой сравнения спрямо n log2 n")
    s.text(40, 44, "Сравнения / (n log₂ n), n = 65 536 (преброени с CountingLess)", size=22, weight=700)
    x0, y0, w = 330, 80, 640
    fx = lambda v: x0 + v / 2.2 * w
    for k, (key, lab) in enumerate(items):
        y = y0 + k * 46
        v = vals[key]
        col = S1 if "merge" in key else (INK if "std" in key else S2)
        s.rect(x0, y, fx(v) - x0, 30, col, "none", rx=4)
        s.text(x0 - 12, y + 20, lab, size=14, anchor="end")
        s.text(fx(v) + 8, y + 20, f"{v:.2f}", size=14, family=MONO, weight=600)
    xb = fx(bound)
    s.line(xb, y0 - 10, xb, y0 + len(items) * 46, stroke=RED, sw=2, dash="6 4")
    s.text(xb + 6, y0 - 14, f"долна граница log₂ n! = {bound:.2f} n log₂ n", size=13, fill=RED, weight=600)
    s.text(40, 390, "Merge sort е на 1% от долната граница. Тристранният quicksort прави ~2 сравнения на елемент на ниво, "
                    "но при много равни ключове печели.", size=14, fill=INK2)
    s.save(OUT / "comparisons.svg")


if __name__ == "__main__":
    merge_trace()
    mergesort_tree()
    partition_frames()
    pivot_choice()
    decision_tree()
    counting_sort()
    radix_trace()
    bucket()
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    time_chart(rows)
    input_chart(rows)
    comparisons_chart()
