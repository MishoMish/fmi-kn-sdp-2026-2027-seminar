#!/usr/bin/env python3
"""Generates every SVG figure for week 04.

Run from anywhere:  python3 weeks/04-dynamic-array/img/src/make_figures.py
The timing chart reads bench_sample.csv (`w04_bench --csv`, release preset);
the growth-cost charts are computed exactly here.
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


def cells(s, x, y, values, w=56, h=44, fills=None, strokes=None, text_fill=None, idx=True):
    for i, v in enumerate(values):
        fill = fills[i] if fills else PANEL
        stroke = strokes[i] if strokes else GRID
        s.rect(x + i * w, y, w - 4, h, fill, stroke, rx=6, sw=1.5)
        if v != "":
            col = text_fill[i] if text_fill else INK
            s.text(x + i * w + (w - 4) / 2, y + h / 2 + 6, str(v), size=16, anchor="middle", family=MONO,
                   fill=col)
        if idx:
            s.text(x + i * w + (w - 4) / 2, y + h + 17, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)


# ---------------------------------------------------------------- growth frames

def growth_frames():
    steps = [
        ("пълен: size = capacity = 4, идва pushBack(E)", ["A", "B", "C", "D"], None, False, False),
        ("1. нов буфер с двоен капацитет (8)", ["A", "B", "C", "D"], ["", "", "", "", "", "", "", ""], False, False),
        ("2. новият елемент — ПЪРВИ (value може да сочи в стария буфер)", ["A", "B", "C", "D"],
         ["", "", "", "", "E", "", "", ""], False, False),
        ("3. старите елементи се преместват (move_if_noexcept)", ["·", "·", "·", "·"],
         ["A", "B", "C", "D", "E", "", "", ""], False, False),
        ("4. старият буфер се освобождава; data_ сочи новия; size = 5", None,
         ["A", "B", "C", "D", "E", "", "", ""], True, True),
    ]
    for k, (caption, old, new, freed, final) in enumerate(steps):
        s = Svg(1000, 330, f"Растеж на динамичен масив, стъпка {k + 1}")
        s.text(40, 44, "pushBack при пълен масив", size=22, weight=700)
        s.text(40, 96, "стар буфер", size=14, fill=MUTED, weight=600)
        if old is not None:
            fills = [TINT1 if v not in ("", "·") else PANEL for v in old]
            cells(s, 40, 106, old, w=70, h=46, fills=fills, strokes=[S1] * 4)
        else:
            s.rect(40, 106, 276, 46, SURFACE, GRID, rx=6, dash="5 4")
            s.text(178, 135, "освободен", size=15, anchor="middle", fill=MUTED)
        if new is not None:
            s.text(650, 235, "нов буфер (capacity 8)", size=14, fill=MUTED, weight=600)
            fills = []
            strokes = []
            for i, v in enumerate(new):
                if v == "E":
                    fills.append(S3 if k == 2 else TINT3)
                    strokes.append(S3)
                elif v:
                    fills.append(TINT1)
                    strokes.append(S1)
                else:
                    fills.append(PANEL)
                    strokes.append(GRID)
            tf = [SURFACE if f == S3 else INK for f in fills]
            cells(s, 40, 206, new, w=70, h=46, fills=fills, strokes=strokes, text_fill=tf)
        if k == 3:
            for i in range(4):
                x = 40 + i * 70 + 33
                s.line(x, 180, x, 204, stroke=S2, sw=2, arrow=True)
        s.text(620, 120, "data_", size=15, family=MONO, weight=600)
        target_y = 230 if final else 130
        s.path(f"M 668,116 C 720,116 720,{target_y} 604,{target_y}" if final else "M 614,124 L 330,128",
               S1, sw=2, arrow=True)
        s.text(40, 306, caption, size=17, weight=600)
        s.save(OUT / f"growth-{k + 1}.svg")


# ---------------------------------------------------------------- cost charts

def regrowth_copies(n_max, grow):
    """copies[n] = total element transfers caused by regrowth after n pushes."""
    cap, total, out = 0, 0, [0]
    for size in range(n_max):
        if size == cap:
            total += size
            cap = grow(cap)
        out.append(total)
    return out


def copies_chart():
    n_max = 2048
    strategies = [
        (S1, "×2", lambda c: max(1, 2 * c)),
        (S3, "×1.5", lambda c: max(c + 1, (3 * c) // 2)),
        (S2, "+64", lambda c: c + 64),
    ]
    s = Svg(1160, 520, "Преместени елементи на един pushBack при различни стратегии на растеж")
    s.text(40, 44, "Колко елемента се местят средно на един pushBack", size=24, weight=700)
    s.text(40, 70, "Общо преместени при преоразмеряване ÷ брой pushBack, за n = 1…2048", size=15, fill=INK2)
    x0, y0, w, h = 100, 100, 720, 320
    ymax = 5.0
    fx = lambda n: x0 + n / n_max * w
    fy = lambda v: y0 + h - min(v, ymax) / ymax * h
    axes(s, x0, y0, w, h, [0, 512, 1024, 1536, 2048], [0, 1, 2, 3, 4, 5], "n — брой pushBack",
         "премествания на pushBack", fx, fy)
    for col, lab, grow in strategies:
        copies = regrowth_copies(n_max, grow)
        pts = [(fx(n), fy(copies[n] / n)) for n in range(1, n_max + 1, 2)]
        clipped = [p for p in pts if p[1] > y0 + 0.5]
        s.path(polyline(clipped), col, sw=2)
        ex, ey = clipped[-1]
        if lab == "+64":
            s.text(ex + 6, ey + 4, "+64 → n/128, расте без граница", size=14, weight=600)
        else:
            s.text(fx(n_max) + 8, ey + 5, lab, size=15, weight=600)
    legend(s, 960, 130, [(c, l) for c, l, _ in strategies])
    s.text(960, 230, "×2: между 1 и 2", size=14, fill=INK2)
    s.text(960, 252, "×1.5: между 2 и 3", size=14, fill=INK2)
    s.text(960, 274, "→ Θ(1) амортизирано", size=14, weight=600)
    s.text(960, 316, "+c: n / 2c", size=14, fill=INK2)
    s.text(960, 338, "→ Θ(n) амортизирано", size=14, weight=600)
    s.text(40, 505, "Мултипликативният растеж държи средната цена ограничена; адитивният — не, колкото и голямо да е c.",
           size=15, fill=INK2)
    s.save(OUT / "copies-chart.svg")


def amortized_bars():
    n = 33
    cap, costs = 0, []
    for size in range(n):
        cost = 1
        if size == cap:
            cost += size
            cap = max(1, 2 * cap)
        costs.append(cost)
    s = Svg(1000, 460, "Реална и амортизирана цена на всеки pushBack")
    s.text(40, 44, "Цената на всеки pushBack: рядко скъпо, средно евтино", size=24, weight=700)
    s.text(40, 70, "1 = записът на новия елемент; + k = преместените при преоразмеряване", size=15, fill=INK2)
    x0, y0, w, h = 90, 100, 860, 270
    ymax = 34
    bw = w / n
    fy = lambda v: y0 + h - v / ymax * h
    for t in [0, 8, 16, 24, 32]:
        s.line(x0, fy(t), x0 + w, fy(t), stroke=GRID, sw=1)
        s.text(x0 - 10, fy(t) + 5, str(t), size=13, fill=INK2, anchor="end")
    for i, c in enumerate(costs):
        x = x0 + i * bw + 2
        s.rect(x, fy(c), bw - 4, y0 + h - fy(c), S2 if c > 1 else S1, rx=2)
        if c > 1:
            s.text(x + (bw - 4) / 2, fy(c) - 6, str(c), size=12, anchor="middle", weight=600)
        if (i + 1) in (1, 2, 3, 5, 9, 17, 33):
            s.text(x + (bw - 4) / 2, y0 + h + 18, str(i + 1), size=12, anchor="middle", fill=INK2, family=MONO)
    s.line(x0, fy(3), x0 + w, fy(3), stroke=INK, sw=2, dash="6 5")
    s.text(x0 + w - 4, fy(3) - 8, "амортизирано: ≤ 3 на операция", size=14, weight=700, anchor="end")
    s.line(x0, y0 + h, x0 + w, y0 + h, stroke=MUTED, sw=1)
    s.text(x0 + w / 2, y0 + h + 44, "номер на pushBack", size=14, fill=INK2, anchor="middle")
    total = sum(costs)
    s.text(40, 450, f"Общо за {n} операции: {total} ≤ 3·{n} = {3 * n}. Скъпите операции са все по-редки — "
           f"точно колкото по-скъпи стават.", size=15, fill=INK2)
    s.save(OUT / "amortized-bars.svg")


def thrashing_chart():
    def simulate(shrink_at):
        size, cap, caps, reallocs = 8, 8, [], 0
        for op in range(24):
            if op % 2 == 0:
                if size == cap:
                    cap *= 2
                    reallocs += 1
                size += 1
            else:
                size -= 1
                if cap > 1 and size <= cap * shrink_at:
                    cap //= 2
                    reallocs += 1
            caps.append(cap)
        return caps, reallocs

    s = Svg(1000, 470, "Капацитет при редуване на pushBack и popBack около границата")
    s.text(40, 44, "Свиване: при ½ или при ¼ запълване?", size=24, weight=700)
    s.text(40, 70, "Масив с 8 елемента и капацитет 8; редуваме pushBack, popBack, pushBack, …", size=15,
           fill=INK2)
    x0, y0, w, h = 90, 110, 620, 230
    fx = lambda i: x0 + i / 23 * w
    fy = lambda c: y0 + h - (c - 4) / 14 * h
    axes(s, x0, y0, w, h, [0, 6, 12, 18, 23], [4, 8, 12, 16], "операция", "капацитет", fx, fy)
    for col, lab, at in [(RED, "свиване при size ≤ cap/2", 0.5), (S1, "свиване при size ≤ cap/4", 0.25)]:
        caps, reallocs = simulate(at)
        pts = []
        for i, c in enumerate(caps):
            if i:
                pts.append((fx(i), fy(caps[i - 1])))
            pts.append((fx(i), fy(c)))
        s.path(polyline(pts), col, sw=2.5 if at == 0.25 else 2)
        y_lab = fy(caps[-1]) - 8 if at == 0.25 else fy(4) + 26
        word = "преоразмеряване" if reallocs == 1 else "преоразмерявания"
        s.text(fx(23) + 12, fy(caps[-1]) + 5, f"{reallocs} {word}", size=14, weight=600)
    legend(s, 90, 420, [(RED, "свиване при size ≤ cap/2: всяка операция преоразмерява (Θ(n) всяка!)")])
    legend(s, 90, 444, [(S1, "свиване при size ≤ cap/4: след първия растеж — нито едно")])
    s.save(OUT / "thrashing-chart.svg")


def timing_chart():
    rows = [r for r in csv.DictReader((HERE / "bench_sample.csv").open()) if int(r["n"]) >= 4096]
    s = Svg(1100, 520, "Време за n pushBack при различни стратегии")
    s.text(40, 44, "Време за n pushBack на int", size=24, weight=700)
    s.text(40, 70, "Release build, примерна машина; най-доброто от няколко пускания", size=15, fill=INK2)
    x0, y0, w, h = 110, 100, 620, 330
    fx = lambda n: x0 + (math.log2(n) - 12) / 10 * w
    fy = lambda ms: y0 + h - (math.log10(ms) + 3) / 5 * h
    axes(s, x0, y0, w, h, [2 ** k for k in range(12, 23, 2)], [0.001, 0.01, 0.1, 1, 10, 100], "n (лог. ос)",
         "ms (лог. ос)", fx, fy, xfmt=lambda n: f"2^{int(math.log2(n))}", yfmt=lambda v: f"{v:g}")
    series = [(S2, "+64 (адитивно)", "plus64_ms"), (S1, "×2 (вашият)", "doubling_ms"),
              (S5, "std::vector", "std_vector_ms"), (S3, "×2 + reserve(n)", "reserve_ms")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 770, 130, [(c, l) for c, l, _ in series])
    s.text(770, 250, "На лог-лог графика наклонът", size=14, fill=INK2)
    s.text(770, 270, "е степента: +64 се изкачва", size=14, fill=INK2)
    s.text(770, 290, "двойно по-стръмно → Θ(n²).", size=14, fill=INK2)
    s.text(770, 326, "reserve(n): нула преоразме-", size=14, fill=INK2)
    s.text(770, 346, "рявания, ~2–3× по-бързо.", size=14, fill=INK2)
    s.save(OUT / "timing-chart.svg")


# ---------------------------------------------------------------- diagrams

def move_vs_copy():
    s = Svg(1100, 380, "Копиране срещу преместване на DynamicArray")
    s.text(40, 46, "Копиране и преместване", size=24, weight=700)
    for px, title, moved in [(40, "DynamicArray b(a);  — копиране", False),
                             (580, "DynamicArray b(std::move(a));  — преместване", True)]:
        s.text(px, 96, title, size=16, weight=600, family=MONO)
        for k, (name, ptr, sz) in enumerate([("a", "nullptr" if moved else "→", "0" if moved else "3"),
                                             ("b", "→", "3")]):
            y = 120 + k * 110
            s.rect(px, y, 150, 60, TINT1, S1, rx=6)
            s.text(px + 12, y + 24, f"{name}.data_ {ptr if ptr != '→' else ''}", size=14, family=MONO)
            s.text(px + 12, y + 46, f"{name}.size_ = {sz}", size=14, family=MONO, fill=INK2)
        if moved:
            cells(s, px + 260, 230, ["1", "2", "3"], w=60, h=40, fills=[TINT2] * 3, strokes=[S2] * 3, idx=False)
            s.line(px + 150, 250, px + 258, 250, stroke=INK2, sw=1.6, arrow=True)
            s.rect(px, 316, 470, 46, TINT3, GREEN, rx=8)
            s.text(px + 14, 344, "3 присвоявания на указатели. Без алокация, Θ(1).", size=15, weight=600)
        else:
            cells(s, px + 260, 120, ["1", "2", "3"], w=60, h=40, fills=[TINT2] * 3, strokes=[S2] * 3, idx=False)
            cells(s, px + 260, 230, ["1", "2", "3"], w=60, h=40, fills=[TINT3] * 3, strokes=[S3] * 3, idx=False)
            s.line(px + 150, 140, px + 258, 140, stroke=INK2, sw=1.6, arrow=True)
            s.line(px + 150, 250, px + 258, 250, stroke=INK2, sw=1.6, arrow=True)
            s.rect(px, 316, 470, 46, PANEL, GRID, rx=8)
            s.text(px + 14, 344, "нова алокация + n копирания: Θ(n).", size=15, weight=600)
    s.save(OUT / "move-vs-copy.svg")


def invalidation():
    s = Svg(1000, 360, "Указател към елемент след преоразмеряване")
    s.text(40, 46, "Преоразмеряването инвалидира указатели и референции", size=24, weight=700)
    s.text(40, 92, "int& first = a[0];   a.pushBack(99);   first = 7;   // UB", size=17, family=MONO)
    s.text(40, 140, "стар буфер (освободен)", size=14, fill=MUTED, weight=600)
    s.rect(40, 150, 4 * 70 - 4, 46, TINT_RED, RED, rx=6, dash="5 4")
    s.text(40 + 138, 179, "освободена памет", size=15, anchor="middle", fill=RED)
    s.text(40, 236, "нов буфер", size=14, fill=MUTED, weight=600)
    cells(s, 40, 246, ["1", "2", "3", "4", "99", "", "", ""], w=70, h=46,
          fills=[TINT1] * 5 + [PANEL] * 3, strokes=[S1] * 5 + [GRID] * 3, idx=False)
    s.text(660, 150, "first", size=16, family=MONO, weight=700)
    s.path("M 656,164 C 560,170 360,170 260,170", RED, sw=2, arrow=True)
    s.text(40, 330, "Същото важи за итератори и за T* от data(). След pushBack, insert или reserve — вземете ги наново.",
           size=15, fill=INK2)
    s.save(OUT / "invalidation.svg")


if __name__ == "__main__":
    growth_frames()
    copies_chart()
    amortized_bars()
    thrashing_chart()
    timing_chart()
    move_vs_copy()
    invalidation()
