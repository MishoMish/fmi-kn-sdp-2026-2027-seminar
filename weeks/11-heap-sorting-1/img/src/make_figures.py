#!/usr/bin/env python3
"""Generates every SVG figure for week 11.

Run from anywhere:  python3 weeks/11-heap-sorting-1/img/src/make_figures.py
The charts read bench_sample.csv (`w11_bench --csv`, release preset).
The traces run the same algorithms as solutions/heap.h and sorts.h, in Python.
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


def cell(s, x, y, w, h, text, fill=TINT1, stroke=S1, tf=INK, size=16):
    s.rect(x, y, w, h, fill, stroke, rx=5, sw=1.4)
    if text != "":
        s.text(x + w / 2, y + h / 2 + size * 0.36, str(text), size=size, anchor="middle", family=MONO, weight=700,
               fill=tf)


def heap_positions(n, cx, y0, width, dy):
    """Positions of a complete tree with n nodes, by array index."""
    pos = []
    for i in range(n):
        level = int(math.log2(i + 1))
        first = 2 ** level - 1
        k = i - first
        slots = 2 ** level
        x = cx - width / 2 + (k + 0.5) * width / slots
        pos.append((x, y0 + level * dy))
    return pos


def draw_heap(s, a, cx, y0, width, dy, hi=None, r=18, size=15, arrows=None, ghost=()):
    """a: values by index; hi: {index: (fill, stroke)}."""
    hi = hi or {}
    pos = heap_positions(len(a), cx, y0, width, dy)
    for i in range(1, len(a)):
        p = (i - 1) // 2
        (x1, y1), (x2, y2) = pos[p], pos[i]
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        dash = "4 4" if i in ghost else None
        s.line(x1 + ux * r, y1 + uy * r, x2 - ux * r, y2 - uy * r, stroke=GRID if i in ghost else INK2, sw=1.5,
               dash=dash)
    for i, v in enumerate(a):
        x, y = pos[i]
        f, st = hi.get(i, (TINT1, S1))
        if i in ghost:
            f, st = SURFACE, GRID
        s.circle(x, y, r, f, stroke=st, sw=1.6)
        s.text(x, y + size * 0.36, str(v), size=size, anchor="middle", family=MONO, weight=700,
               fill=MUTED if i in ghost else INK)
    return pos


def draw_array(s, a, x0, y0, w=44, h=40, hi=None, idx=True, ghost=()):
    hi = hi or {}
    for i, v in enumerate(a):
        f, st = hi.get(i, (TINT1, S1))
        if i in ghost:
            f, st = SURFACE, GRID
        cell(s, x0 + i * w, y0, w - 4, h, v, fill=f, stroke=st, tf=MUTED if i in ghost else INK)
        if idx:
            s.text(x0 + i * w + (w - 4) / 2, y0 + h + 16, str(i), size=11, anchor="middle", fill=MUTED, family=MONO)


ORANGE = (TINT2, S2)
GREEN = (TINT3, S3)
RED_HI = (TINT_RED, RED)


# ---------------------------------------------------------------- heap

def heap_array():
    s = Svg(1100, 420, "Двоична пирамида: дърво и масив")
    s.text(40, 46, "Двоична пирамида: почти пълно дърво, записано в масив", size=24, weight=700)
    a = [9, 5, 8, 3, 4, 7, 6, 1, 2]
    pos = draw_heap(s, a, 300, 100, 440, 72)
    for i, (x, y) in enumerate(pos):
        s.text(x + 22, y - 14, f"[{i}]", size=11, fill=MUTED, family=MONO)
    draw_array(s, a, 600, 120)
    s.path("M 666,118 C 666,90 740,90 740,118", S2, sw=1.6, arrow=True)
    s.path("M 666,118 C 666,80 784,80 784,118", S2, sw=1.6, arrow=True)
    s.text(720, 74, "деца на 1: 3 и 4", size=13, anchor="middle", fill=S2)
    for k, t in enumerate(["деца на i:  2i + 1,  2i + 2", "родител на i:  (i − 1) / 2",
                           "max-heap: родителят ≥ децата си", "→ най-големият е в a[0]",
                           "височина ⌊log₂ n⌋ — винаги (формата е фиксирана)"]):
        s.text(600, 230 + k * 28, t, size=15, family=MONO if k < 2 else None, weight=600 if k == 3 else 400)
    s.text(40, 400, "Без указатели, без дупки, съседните нива — близо в паметта. Не е BST: 5 и 8 не са наредени.",
           size=14, fill=INK2)
    s.save(OUT / "heap-array.svg")


def push_frames():
    a = [9, 5, 8, 3, 4, 7, 6]
    a.append(10)
    i = len(a) - 1
    frames = [(list(a), i, "push(10): добавяме в края — на първото свободно място")]
    while i > 0 and a[(i - 1) // 2] < a[i]:
        p = (i - 1) // 2
        a[p], a[i] = a[i], a[p]
        i = p
        frames.append((list(a), i, f"10 > родителя → размяна (siftUp)"))
    frames[-1] = (frames[-1][0], frames[-1][1], "10 е в корена: готово. ≤ ⌊log₂ n⌋ размени")
    for k, (arr, cur, title) in enumerate(frames):
        s = Svg(1100, 420, title)
        s.text(40, 46, title, size=22, weight=700)
        draw_heap(s, arr, 300, 110, 440, 76, hi={cur: ORANGE})
        draw_array(s, arr, 600, 150, hi={cur: ORANGE})
        s.text(600, 260, f"стъпка {k + 1} от {len(frames)}", size=14, fill=MUTED)
        s.save(OUT / f"push-{k + 1}.svg")
    return len(frames)


def pop_frames():
    a = [10, 9, 8, 5, 4, 7, 6, 3]
    frames = [(list(a), {0: RED_HI}, (), "pop(): махаме a[0] — най-големия")]
    a[0] = a.pop()
    frames.append((list(a), {0: ORANGE}, (), "последният (3) отива в корена; масивът се скъсява"))
    i, n = 0, len(a)
    while True:
        l = 2 * i + 1
        if l >= n:
            break
        c = l + 1 if l + 1 < n and a[l] < a[l + 1] else l
        if not a[i] < a[c]:
            break
        a[i], a[c] = a[c], a[i]
        frames.append((list(a), {c: ORANGE, i: GREEN}, (), f"по-голямото дете ({a[i]}) > 3 → размяна (siftDown)"))
        i = c
    frames[-1] = (frames[-1][0], frames[-1][1], (), "3 няма по-големи деца: готово. ≤ 2 сравнения на ниво")
    for k, (arr, hi, ghost, title) in enumerate(frames):
        s = Svg(1100, 420, title)
        s.text(40, 46, title, size=22, weight=700)
        draw_heap(s, arr, 300, 110, 440, 76, hi=hi)
        draw_array(s, arr, 600, 150, hi=hi)
        s.text(600, 260, f"стъпка {k + 1} от {len(frames)}", size=14, fill=MUTED)
        s.save(OUT / f"pop-{k + 1}.svg")
    return len(frames)


def floyd():
    s = Svg(1100, 450, "Построяване на пирамида отдолу нагоре (Floyd)")
    s.text(40, 46, "makeHeap: siftDown на всеки вътрешен възел, отзад напред", size=24, weight=700)
    a = [3, 1, 6, 5, 2, 4, 8, 7, 9, 10, 11, 12, 13, 14, 15]
    n = len(a)
    pos = draw_heap(s, a, 300, 110, 500, 76, hi={i: (TINT1, S1) for i in range(n // 2)} |
                    {i: (PANEL, GRID) for i in range(n // 2, n)})
    order = list(range(n // 2 - 1, -1, -1))
    for k, i in enumerate(order):
        x, y = pos[i]
        s.circle(x + 20, y - 18, 10, S2, stroke=SURFACE)
        s.text(x + 20, y - 14, str(k + 1), size=11, anchor="middle", fill=SURFACE, weight=700)
    s.text(40, 420, "оранжевите числа — редът на siftDown; сивите (листата) — вече са пирамиди", size=14,
           fill=INK2)
    rows = [("ниво", "възли", "≤ слизания"), ("листа", "≈ n/2", "0"), ("над тях", "≈ n/4", "1"),
            ("", "≈ n/8", "2"), ("", "≈ n/16", "3"), ("…", "", "")]
    for k, (a1, b1, c1) in enumerate(rows):
        y = 120 + k * 30
        w = 700 if k else 700
        s.text(w, y, a1, size=14, weight=700 if k == 0 else 400)
        s.text(w + 110, y, b1, size=14, family=MONO, weight=700 if k == 0 else 400)
        s.text(w + 210, y, c1, size=14, family=MONO, weight=700 if k == 0 else 400)
    s.text(700, 320, "общо ≤ n · (1/4 + 2/8 + 3/16 + …) = n", size=15, family=MONO, weight=700)
    s.text(700, 348, "→ ≤ 2n сравнения: Θ(n)", size=15, weight=700, fill=S3)
    s.text(700, 380, "n поредни push: Θ(n log n)", size=14, fill=INK2)
    s.text(700, 402, "(повечето възли са долу, а там", size=14, fill=INK2)
    s.text(700, 422, "siftDown е евтин, siftUp — скъп)", size=14, fill=INK2)
    s.save(OUT / "floyd.svg")


def heapsort_trace():
    a = [5, 2, 4, 6, 1, 3]
    n = len(a)

    def sift_down(a, i, n):
        while True:
            l = 2 * i + 1
            if l >= n:
                return
            c = l + 1 if l + 1 < n and a[l] < a[l + 1] else l
            if not a[i] < a[c]:
                return
            a[i], a[c] = a[c], a[i]
            i = c
    rows = [(list(a), 0, "вход")]
    for i in range(n // 2 - 1, -1, -1):
        sift_down(a, i, n)
    rows.append((list(a), 0, "makeHeap — 6 е отпред"))
    for m in range(n, 1, -1):
        a[0], a[m - 1] = a[m - 1], a[0]
        sift_down(a, 0, m - 1)
        rows.append((list(a), n - m + 1, f"swap(a[0], a[{m - 1}]); siftDown в [0, {m - 1})"))
    s = Svg(1100, 80 + len(rows) * 50 + 40, "Heapsort стъпка по стъпка")
    s.text(40, 46, "Heapsort: най-големият отива в края, пирамидата се свива", size=24, weight=700)
    for k, (arr, done, label) in enumerate(rows):
        y = 80 + k * 50
        hi = {i: GREEN for i in range(n - done, n)}
        if k == 1:
            hi = {0: ORANGE}
        draw_array(s, arr, 40, y, w=50, h=38, hi=hi, idx=False)
        s.text(360, y + 25, label, size=14, family=MONO if "swap" in label else None, fill=INK2)
    s.text(40, 80 + len(rows) * 50 + 20, "зелено — на окончателното си място. На място, Θ(n log n) винаги, не е стабилен.",
           size=14, fill=INK2)
    s.save(OUT / "heapsort-trace.svg")


# ---------------------------------------------------------------- simple sorts

def trace_figure(name, title, rows, note, n):
    s = Svg(1100, 90 + len(rows) * 50 + 40, title)
    s.text(40, 46, title, size=24, weight=700)
    for k, (arr, hi, label) in enumerate(rows):
        y = 80 + k * 50
        draw_array(s, arr, 40, y, w=50, h=38, hi=hi, idx=False)
        s.text(40 + n * 50 + 30, y + 25, label, size=14, fill=INK2, family=MONO)
    s.text(40, 90 + len(rows) * 50 + 10, note, size=14, fill=INK2)
    s.save(OUT / name)


def simple_traces():
    start = [5, 2, 4, 6, 1, 3]
    n = len(start)
    # selection
    a, rows = list(start), [(list(start), {}, "вход")]
    for i in range(n - 1):
        m = min(range(i, n), key=lambda j: a[j])
        a[i], a[m] = a[m], a[i]
        hi = {j: GREEN for j in range(i)} | {i: ORANGE}
        if m != i:
            hi[m] = ORANGE
        rows.append((list(a), hi, f"i = {i}: най-малкият в [{i}, {n}) е {a[i]}" + ("" if m != i else " — на място")))
    trace_figure("selection-trace.svg", "Selection sort: намери минимума, сложи го отпред", rows,
                 "Винаги n(n−1)/2 сравнения — дори при сортиран вход. Но само n − 1 размени.", n)
    # bubble
    a, rows = list(start), [(list(start), {}, "вход")]
    end = n
    while end > 1:
        swapped = False
        for j in range(1, end):
            if a[j] < a[j - 1]:
                a[j], a[j - 1] = a[j - 1], a[j]
                swapped = True
        end -= 1
        rows.append((list(a), {j: GREEN for j in range(end, n)}, f"проход {n - end}: {a[end]} изплува до края"))
        if not swapped:
            break
    trace_figure("bubble-trace.svg", "Bubble sort: съседите, които не са наред, се разменят", rows,
                 "Най-големият „изплува“ на всеки проход. Проход без размяна → край (сортиран вход: n − 1 сравнения).",
                 n)
    # insertion
    a, rows = list(start), [(list(start), {0: GREEN}, "вход: [0, 1) е сортиран")]
    for i in range(1, n):
        v = a[i]
        j = i
        while j > 0 and v < a[j - 1]:
            a[j] = a[j - 1]
            j -= 1
        a[j] = v
        hi = {k: GREEN for k in range(i + 1)} | {j: ORANGE}
        rows.append((list(a), hi, f"вмъкваме {v}: {i - j} по-големи се изместват надясно"))
    trace_figure("insertion-trace.svg", "Insertion sort: вмъкни в сортираното начало", rows,
                 "Като подреждане на карти в ръката. Сравнения ≈ инверсии + n → почти сортиран вход: почти Θ(n).", n)


def shell_trace():
    a = [8, 3, 7, 1, 9, 2, 6, 4, 5, 0]
    n = len(a)
    colors = [(TINT1, S1), (TINT2, S2), (TINT3, S3), ("#fff4d6", S4)]
    s = Svg(1100, 400, "Shell sort: insertion sort през разстояние")
    s.text(40, 46, "Shell sort: insertion sort на елементи през gap, после gap = 1", size=24, weight=700)
    rows = [(list(a), "вход; gap = 4: четири групи (по цвят)")]
    gap = 4
    for i in range(gap, n):
        v, j = a[i], i
        while j >= gap and v < a[j - gap]:
            a[j] = a[j - gap]
            j -= gap
        a[j] = v
    rows.append((list(a), "след gap = 4: всяка група е сортирана"))
    inv = sum(1 for i in range(n) for j in range(i + 1, n) if a[j] < a[i])
    for i in range(1, n):
        v, j = a[i], i
        while j >= 1 and v < a[j - 1]:
            a[j] = a[j - 1]
            j -= 1
        a[j] = v
    rows.append((list(a), f"gap = 1 (обикновен insertion sort): само {inv} инверсии"))
    for k, (arr, label) in enumerate(rows):
        y = 90 + k * 80
        hi = {i: colors[i % 4] for i in range(n)} if k < 2 else {i: GREEN for i in range(n)}
        draw_array(s, arr, 40, y, w=50, h=40, hi=hi, idx=False)
        s.text(560, y + 26, label, size=14, fill=INK2)
    inv0 = sum(1 for i in range(n) for j in range(i + 1, n) if rows[0][0][j] < rows[0][0][i])
    s.text(40, 350, f"Входът има {inv0} инверсии. Голямото разстояние мести далечни елементи с един ход —",
           size=14, fill=INK2)
    s.text(40, 372, "последният проход получава почти сортиран масив. Пропуски на Ciura: 1, 4, 10, 23, 57, 132, 301, 701, …",
           size=14, fill=INK2)
    s.save(OUT / "shell-trace.svg")


def turtle():
    s = Svg(1100, 380, "Костенурки и зайци: bubble срещу shaker")
    s.text(40, 46, "Костенурка: малък елемент в края. Bubble я мести с една позиция на проход", size=22, weight=700)
    a = [2, 3, 4, 5, 6, 7, 8, 1]
    n = len(a)
    s.text(40, 96, "bubble sort", size=16, weight=700)
    b = list(a)
    for p in range(3):
        for j in range(1, n - p):
            if b[j] < b[j - 1]:
                b[j], b[j - 1] = b[j - 1], b[j]
        idx = b.index(1)
        draw_array(s, b, 40, 110 + p * 50, w=44, h=36, hi={idx: ORANGE}, idx=False)
        s.text(40 + n * 44 + 16, 110 + p * 50 + 24, f"след проход {p + 1}", size=13, fill=INK2)
    s.text(40, 290, "… още 4 прохода: n − 1 общо", size=14, fill=RED)
    s.text(600, 96, "shaker sort", size=16, weight=700)
    c = list(a)
    for j in range(1, n):
        if c[j] < c[j - 1]:
            c[j], c[j - 1] = c[j - 1], c[j]
    draw_array(s, c, 600, 110, w=44, h=36, hi={c.index(1): ORANGE}, idx=False)
    s.text(600 + n * 44 + 16, 134, "→ напред", size=13, fill=INK2)
    for j in range(n - 2, 0, -1):
        if c[j] < c[j - 1]:
            c[j], c[j - 1] = c[j - 1], c[j]
    draw_array(s, c, 600, 160, w=44, h=36, hi={0: GREEN}, idx=False)
    s.text(600 + n * 44 + 16, 184, "← назад", size=13, fill=INK2)
    s.text(600, 240, "Обратният проход носи 1 до началото наведнъж:", size=14, fill=S3, weight=600)
    s.text(600, 262, "3 прохода (третият — само проверка) вместо 7.", size=14, fill=S3, weight=600)
    s.text(40, 350, "Зайци (големи елементи в началото) не са проблем — изплуват за един проход. В най-лошия случай и двете са Θ(n²).",
           size=14, fill=INK2)
    s.save(OUT / "turtle.svg")


def stability():
    s = Svg(1100, 330, "Стабилно сортиране")
    s.text(40, 46, "Стабилност: равните ключове запазват реда си", size=24, weight=700)
    items = [(3, "a"), (1, "b"), (3, "c"), (2, "d"), (1, "e")]
    cols = {"a": S1, "b": S2, "c": S3, "d": S4, "e": S5}

    def row(y, seq, label):
        for k, (key, tag) in enumerate(seq):
            x = 220 + k * 90
            s.rect(x, y, 76, 46, PANEL, cols[tag], rx=6, sw=2)
            s.text(x + 26, y + 30, str(key), size=18, anchor="middle", family=MONO, weight=700)
            s.text(x + 56, y + 30, tag, size=15, anchor="middle", family=MONO, fill=cols[tag], weight=700)
        s.text(40, y + 30, label, size=15, weight=600)
    row(80, items, "вход")
    row(150, sorted(items, key=lambda t: t[0]), "стабилно")
    row(220, [(1, "e"), (1, "b"), (2, "d"), (3, "a"), (3, "c")], "нестабилно")
    s.text(700, 103, "сортираме само по числото", size=14, fill=INK2)
    s.text(700, 173, "b преди e, a преди c — като във входа", size=14, fill=S3)
    s.text(700, 243, "e преди b — също сортирано, но друго", size=14, fill=RED)
    s.text(40, 310, "Защо е важно: сортирайте по име, после стабилно по град → в един град имената остават по азбучен ред.",
           size=14, fill=INK2)
    s.save(OUT / "stability.svg")


# ---------------------------------------------------------------- charts

STYLE = {"selection": S5, "bubble": RED, "shaker": "#b13b3b", "insertion": S4, "shell": S3, "heap": S1,
         "std::sort": INK, "std::stable_sort": MUTED}
LABEL = {"selection": "selection", "bubble": "bubble", "shaker": "shaker", "insertion": "insertion", "shell": "Shell",
         "heap": "heapsort", "std::sort": "std::sort", "std::stable_sort": "std::stable_sort"}


def sort_chart(rows):
    s = Svg(1100, 520, "Време за сортиране, случаен вход")
    s.text(40, 44, "Време за сортиране на n случайни int (ms)", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина; квадратичните — само до 20 000", size=15, fill=INK2)
    x0, y0, w, h0 = 100, 100, 560, 340
    fx = lambda n: x0 + (math.log10(n) - 3) / 3 * w
    fy = lambda v: y0 + h0 - (math.log10(v) + 3) / 7 * h0
    axes(s, x0, y0, w, h0, [1000, 10000, 100000, 1000000], [0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000],
         "n (лог. ос)", "ms (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    for algo, col in STYLE.items():
        pts = [(fx(int(r["n"])), fy(float(r["ms"]))) for r in rows if r["algorithm"] == algo and r["input"] == "random"]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 700, 120, [(STYLE[a], LABEL[a]) for a in STYLE])
    get = lambda n, a: float(next(r["ms"] for r in rows if r["n"] == str(n) and r["algorithm"] == a
                                  and r["input"] == "random"))
    s.text(700, 330, f"n = 20 000: bubble {get(20000, 'bubble'):.0f} ms,", size=14, fill=INK2)
    s.text(700, 352, f"insertion {get(20000, 'insertion'):.0f} ms, std::sort {get(20000, 'std::sort'):.1f} ms", size=14,
           fill=INK2)
    s.text(700, 440, "(bubble и shaker почти съвпадат)", size=13, fill=MUTED)
    s.text(700, 388, "Наклон 2 (×10 по n → ×100 по време):", size=14, fill=INK2)
    s.text(700, 410, "квадратичните. Наклон ≈ 1: n log n.", size=14, fill=INK2)
    s.save(OUT / "sort-chart.svg")


def input_chart(rows):
    n = "20000"
    algos = ["selection", "bubble", "insertion", "shell", "heap", "std::sort"]
    kinds = [("random", "случаен", S1), ("sorted", "сортиран", S3), ("reversed", "обратен", RED),
             ("nearly", "почти сортиран", S4)]
    s = Svg(1100, 500, "Влияние на входа, n = 20 000")
    s.text(40, 44, "Един и същи алгоритъм, различен вход (n = 20 000, ms, лог. скала)", size=22, weight=700)
    x0, y0, w, h0 = 100, 100, 900, 300
    fy = lambda v: y0 + h0 - (math.log10(max(v, 0.001)) + 3) / 6 * h0
    for t in [0.001, 0.01, 0.1, 1, 10, 100, 1000]:
        y = fy(t)
        s.line(x0, y, x0 + w, y, stroke=GRID, sw=1)
        s.text(x0 - 10, y + 5, f"{t:g}", size=13, fill=INK2, anchor="end")
    gw = w / len(algos)
    for g, algo in enumerate(algos):
        for k, (kind, _, col) in enumerate(kinds):
            v = float(next(r["ms"] for r in rows if r["n"] == n and r["algorithm"] == algo and r["input"] == kind))
            bx = x0 + g * gw + 14 + k * (gw - 28) / 4
            bw = (gw - 28) / 4 - 4
            s.add(f'<rect x="{bx:.1f}" y="{fy(v):.1f}" width="{bw:.1f}" height="{y0 + h0 - fy(v):.1f}" '
                  f'rx="3" fill="{col}"/>')
        s.text(x0 + g * gw + gw / 2, y0 + h0 + 24, LABEL[algo], size=14, anchor="middle", weight=600)
    s.line(x0, y0 + h0, x0 + w, y0 + h0, stroke=MUTED, sw=1)
    for k, (_, lab, col) in enumerate(kinds):
        s.rect(120 + k * 200, 450, 16, 16, col, "none", rx=3)
        s.text(144 + k * 200, 463, lab, size=14)
    s.text(40, 490, "Selection не забелязва входа. Bubble и insertion са адаптивни: сортиран вход → Θ(n).", size=14,
           fill=INK2)
    s.save(OUT / "input-chart.svg")


if __name__ == "__main__":
    heap_array()
    print("push frames:", push_frames(), " pop frames:", pop_frames())
    floyd()
    heapsort_trace()
    simple_traces()
    shell_trace()
    turtle()
    stability()
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    sort_chart(rows)
    input_chart(rows)
