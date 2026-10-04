#!/usr/bin/env python3
"""Generates every SVG figure for week 02.

Run from anywhere:  python3 weeks/02-compiler-locality-containers/img/src/make_figures.py
The result charts read bench_sample.csv (output of `w02_bench --csv`,
release preset).
"""

import csv
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent


def bench():
    data = {}
    for row in csv.DictReader((HERE / "bench_sample.csv").open()):
        data.setdefault(row["part"], []).append((row["variant"], float(row["value"])))
    return data


# ---------------------------------------------------------------- memory

def memory_hierarchy():
    s = Svg(1100, 520, "Йерархия на паметта: размер и време за достъп")
    s.text(40, 46, "Йерархия на паметта", size=24, weight=700)
    s.text(40, 72, "Приблизителни стойности за съвременен настолен процесор. Оста на времето е логаритмична.",
           size=15, fill=INK2)
    levels = [
        ("регистри", "< 1 KB", 0.3, "0.3 s"),
        ("L1 кеш", "32–64 KB / ядро", 1.0, "1 s"),
        ("L2 кеш", "0.5–2 MB / ядро", 4.0, "4 s"),
        ("L3 кеш", "8–96 MB, общ", 15.0, "15 s"),
        ("RAM", "8–64 GB", 90.0, "1.5 мин"),
        ("SSD (NVMe)", "0.5–4 TB", 80000.0, "≈ 1 ден"),
    ]
    x0, w = 330, 520
    lo, hi = math.log10(0.1), math.log10(200000)
    fx = lambda ns: x0 + (math.log10(ns) - lo) / (hi - lo) * w
    s.text(40, 112, "НИВО", size=13, fill=MUTED, weight=600)
    s.text(170, 112, "РАЗМЕР", size=13, fill=MUTED, weight=600)
    s.text(x0, 112, "ВРЕМЕ ЗА ДОСТЪП", size=13, fill=MUTED, weight=600)
    s.text(1060, 112, "АКО L1 = 1 s", size=13, fill=MUTED, weight=600, anchor="end")
    for tick, lab in [(1, "1 ns"), (10, "10 ns"), (100, "100 ns"), (1000, "1 µs"), (10000, "10 µs"),
                      (100000, "100 µs")]:
        x = fx(tick)
        s.line(x, 124, x, 470, stroke=GRID, sw=1)
        s.text(x, 490, lab, size=13, fill=INK2, anchor="middle")
    for k, (name, size, ns, human) in enumerate(levels):
        y = 140 + k * 54
        s.text(40, y + 22, name, size=17, weight=600)
        s.text(170, y + 22, size, size=15, fill=INK2)
        col = S1 if ns < 50 else (S2 if ns < 1000 else S5)
        s.rect(x0, y + 6, fx(ns) - x0, 24, col, rx=4)
        lab = f"{ns:g} ns" if ns < 1000 else f"{ns / 1000:g} µs"
        s.text(fx(ns) + 10, y + 23, lab, size=14, weight=600)
        s.text(1060, y + 22, human, size=15, fill=INK2, anchor="end")
    s.text(40, 512, "Кешът е малък, но ~100 пъти по-бърз от RAM. Програма, която чете паметта подред, "
           "почти винаги уцелва кеша.", size=15, fill=INK2)
    s.save(OUT / "memory-hierarchy.svg")


def cache_line():
    s = Svg(1100, 400, "Кеш линия: при четене на един елемент се зарежда цял блок от 64 байта")
    s.text(40, 46, "Паметта се зарежда на кеш линии от 64 байта", size=24, weight=700)
    s.text(40, 72, "int е 4 байта → една линия = 16 последователни int-а", size=15, fill=INK2)
    cell, gx = 31, 40
    rows = [
        (120, "последователно: a[0], a[1], a[2], …", lambda i: True, "1 промах на 16 достъпа"),
        (250, "със стъпка 16: a[0], a[16], a[32], …", lambda i: i % 16 == 0, "1 промах на всеки достъп"),
    ]
    for y, title, touched, verdict in rows:
        s.text(gx, y - 14, title, size=16, weight=600)
        for i in range(32):
            line = i // 16
            x = gx + i * cell + line * 10
            fill = TINT1 if line == 0 else TINT3
            if touched(i):
                fill = S1 if line == 0 else S3
            s.rect(x, y, cell - 3, 40, fill, rx=4)
            s.text(x + (cell - 3) / 2, y + 26, str(i), size=12, anchor="middle",
                   fill=SURFACE if touched(i) else INK2, family=MONO)
        s.text(gx, y + 66, "→ " + verdict, size=15, weight=600)
    for line, col in [(0, S1), (1, S3)]:
        xa = gx + line * (16 * cell + 10)
        xb = xa + 16 * cell - 3
        s.path(f"M {xa},{338} L {xa},{346} L {xb},{346} L {xb},{338}", MUTED, sw=1.5)
        s.text((xa + xb) / 2, 368, f"кеш линия {line + 1}: 64 B", size=14, fill=INK2, anchor="middle")
    s.text(40, 394, "Оцветено = елементи, които програмата чете. Цялата линия идва от RAM при първия достъп до нея.",
           size=14, fill=INK2)
    s.save(OUT / "cache-line.svg")


def row_major():
    rows, cols = 3, 4
    s = Svg(1100, 470, "Матрица 3×4 в паметта по редове и два реда на обхождане")
    s.text(40, 46, "Матрица по редове (row-major) и два начина да я обходим", size=24, weight=700)
    cell = 52
    gx, gy = 60, 110
    for r in range(rows):
        for c in range(cols):
            idx = r * cols + c
            s.rect(gx + c * cell, gy + r * cell, cell - 4, cell - 4, [TINT1, TINT3, TINT2][r], rx=6)
            s.text(gx + c * cell + 24, gy + r * cell + 30, f"{r},{c}", size=14, anchor="middle", family=MONO)
    s.text(gx + cols * cell / 2 - 2, gy + rows * cell + 28, "m(r, c)", size=15, fill=INK2, anchor="middle",
           family=MONO)
    s.text(gx + cols * cell / 2 - 2, gy + rows * cell + 50, "адрес = r · cols + c", size=15, weight=600,
           anchor="middle", family=MONO)

    lx, cw = 340, 56
    for y0, title, order, verdict, col in [
        (120, "по редове: r навън, c навътре", [r * cols + c for r in range(rows) for c in range(cols)],
         "стъпка 1 → кешът работи", S3),
        (300, "по колони: c навън, r навътре", [r * cols + c for c in range(cols) for r in range(rows)],
         "стъпка cols → почти всеки достъп е промах", RED),
    ]:
        s.text(lx, y0 - 16, title, size=16, weight=600)
        for i in range(rows * cols):
            r = i // cols
            s.rect(lx + i * cw, y0, cw - 4, 40, [TINT1, TINT3, TINT2][r], rx=5)
            s.text(lx + i * cw + 26, y0 + 25, f"{r},{i % cols}", size=13, anchor="middle", family=MONO)
        for k in range(len(order) - 1):
            a, b = order[k], order[k + 1]
            xa, xb = lx + a * cw + 26, lx + b * cw + 26
            if b > a:
                lift = 14 + (b - a) * 5
                s.path(f"M {xa:.0f},{y0 + 44} Q {(xa + xb) / 2:.0f},{y0 + 44 + lift} {xb:.0f},{y0 + 44}", col,
                       sw=1.6, arrow=True)
        s.text(lx, y0 + 112, verdict, size=15, fill=INK2, italic=True)
    s.save(OUT / "row-major.svg")


def pointer_chasing():
    s = Svg(1100, 400, "Масив срещу свързани възли в паметта")
    s.text(40, 46, "Масив срещу свързани възли", size=24, weight=700)
    s.text(40, 72, "И двете обхождания са Θ(n). Разликата е къде в паметта е следващият елемент.",
           size=15, fill=INK2)
    s.text(40, 120, "std::vector — елементите са един до друг", size=16, weight=600)
    for i in range(12):
        s.rect(40 + i * 62, 134, 58, 40, S1, rx=5)
        s.text(40 + i * 62 + 29, 159, str(i * 10), size=14, anchor="middle", fill=SURFACE, family=MONO)
    s.text(800, 159, "следващият е в същата кеш линия", size=15, fill=INK2)

    s.text(40, 230, "свързани възли в случаен ред — всеки сочи някъде другаде", size=16, weight=600)
    slots = [7, 2, 10, 4, 0, 9, 5, 11, 1, 8, 3, 6]  # slot of node k
    for k, slot in enumerate(slots):
        x = 40 + slot * 62
        s.rect(x, 250, 40, 40, S2, rx=5)
        s.rect(x + 40, 250, 16, 40, TINT2, S2, rx=3, sw=1)
        s.text(x + 20, 275, str(k * 10), size=14, anchor="middle", fill=SURFACE, family=MONO)
    for k in range(len(slots) - 1):
        xa = 40 + slots[k] * 62 + 48
        xb = 40 + slots[k + 1] * 62 + 20
        lift = 26 + abs(xb - xa) * 0.12
        s.path(f"M {xa:.0f},{294} Q {(xa + xb) / 2:.0f},{294 + lift:.0f} {xb:.0f},{294}", INK2, sw=1.3,
               arrow=True)
    s.text(800, 275, "всяка стъпка: нов промах в кеша", size=15, fill=INK2)
    s.text(40, 390, "Процесорът не може да зареди предварително следващия възел — адресът му е известен "
           "едва след като прочете текущия.", size=15, fill=INK2)
    s.save(OUT / "pointer-chasing.svg")


def array_decay():
    s = Svg(1000, 380, "Масивът се превръща в указател при подаване на функция")
    s.text(40, 46, "Масив → указател: размерът се губи", size=24, weight=700)
    s.text(40, 86, "int a[5] = {1, 2, 3, 4, 5};", size=17, family=MONO)
    for i in range(5):
        s.rect(40 + i * 86, 120, 82, 48, TINT1, S1, rx=6)
        s.text(81 + i * 86, 150, str(i + 1), size=17, anchor="middle", family=MONO)
        s.text(81 + i * 86, 190, f"a[{i}]", size=13, anchor="middle", fill=INK2, family=MONO)
    s.text(40, 222, "sizeof(a) == 20   — компилаторът знае, че са 5 int-а", size=16, family=MONO)
    s.text(40, 280, "void f(int arr[]) { sizeof(arr); }   // == sizeof(int*) == 8", size=16, family=MONO)
    s.rect(600, 120, 90, 48, TINT2, S2, rx=6)
    s.text(645, 150, "arr", size=16, anchor="middle", family=MONO)
    s.path("M 645,120 C 645,100 130,100 81,117", S2, sw=1.8, arrow=True)
    s.text(710, 150, "указател: само адрес,", size=15, fill=INK2)
    s.text(710, 172, "без размер", size=15, fill=INK2)
    s.text(40, 330, "Решение: std::array<int, 5> (размерът е част от типа) или подавайте размера отделно.",
           size=15, fill=INK2)
    s.text(40, 356, "a[i] е просто *(a + i): адрес = начало + i · sizeof(int) → Θ(1) достъп.", size=15,
           fill=INK2)
    s.save(OUT / "array-decay.svg")


def container_map():
    s = Svg(1100, 470, "Карта на контейнерите в стандартната библиотека")
    s.text(40, 46, "Контейнерите в STL (std::) и кога ги реализираме", size=24, weight=700)
    groups = [
        ("Последователни", "ред = редът на вмъкване", TINT1, S1,
         [("array", "02–03"), ("vector", "04"), ("deque", "07"), ("list", "06"), ("forward_list", "05")]),
        ("Асоциативни, наредени", "балансирано дърво, Θ(log n)", TINT3, S3,
         [("set", "09–10"), ("map", "09–10"), ("multiset", ""), ("multimap", "")]),
        ("Асоциативни, хеширани", "хеш таблица, Θ(1) средно", TINT2, S2,
         [("unordered_set", "08"), ("unordered_map", "08")]),
        ("Адаптери", "интерфейс над друг контейнер", TINT5, S5,
         [("stack", "07"), ("queue", "07"), ("priority_queue", "11")]),
    ]
    x = 40
    for title, sub, tint, col, items in groups:
        s.rect(x, 90, 245, 340, tint, col)
        s.text(x + 16, 122, title, size=17, weight=700)
        s.text(x + 16, 144, sub, size=13, fill=INK2)
        for k, (name, week) in enumerate(items):
            y = 166 + k * 50
            s.rect(x + 14, y, 217, 38, SURFACE, GRID, rx=6)
            s.text(x + 26, y + 25, name, size=14, family=MONO)
            if week:
                s.text(x + 220, y + 25, "седм. " + week, size=12, fill=MUTED, anchor="end")
        x += 260
    s.text(40, 458, "„седм.“ = седмицата от курса, в която реализираме подобна структура сами.", size=14,
           fill=INK2)
    s.save(OUT / "container-map.svg")


def blocked_transpose():
    n, cell, b = 8, 34, 4
    s = Svg(1000, 430, "Транспониране на блокове")
    s.text(40, 46, "Транспониране на блокове (tiling)", size=24, weight=700)
    for panel, (gx, title) in enumerate([(60, "източник m"), (560, "резултат t = mᵀ")]):
        s.text(gx + n * cell / 2, 100, title, size=16, weight=600, anchor="middle")
        for r in range(n):
            for c in range(n):
                tile = (r // b, c // b) if panel == 0 else (c // b, r // b)
                hot = tile == (0, 1)
                s.rect(gx + c * cell, 116 + r * cell, cell - 3, cell - 3, S2 if hot else PANEL, rx=3)
        for k in range(1, n // b):
            s.line(gx + k * b * cell - 1.5, 116, gx + k * b * cell - 1.5, 116 + n * cell, stroke=INK2, sw=2)
            s.line(gx, 116 + k * b * cell - 1.5, gx + n * cell, 116 + k * b * cell - 1.5, stroke=INK2, sw=2)
    s.path("M 340,200 C 420,170 480,170 560,280", INK2, sw=1.8, arrow=True)
    s.text(450, 160, "блок B×B", size=15, weight=600, anchor="middle")
    s.text(40, 410, "Блок от източника и съответният блок от резултата се побират в кеша едновременно, "
           "затова разпръснатите записи не са промахи.", size=15, fill=INK2)
    s.save(OUT / "blocked-transpose.svg")


# ---------------------------------------------------------------- results

def stride_chart(data):
    pts = [(int(v), t) for v, t in data["stride"]]
    s = Svg(1000, 500, "Време за достъп спрямо стъпката на обхождане")
    s.text(40, 44, "Едни и същи 16M четения, различна стъпка", size=24, weight=700)
    s.text(40, 70, "Всеки елемент на 64 MB масив се чете точно веднъж. Release build, примерна машина.",
           size=15, fill=INK2)
    x0, y0, w, h = 110, 100, 760, 300
    ymax = max(t for _, t in pts) * 1.15
    fx = lambda st: x0 + math.log2(st) / 8 * w
    fy = lambda t: y0 + h - t / ymax * h
    axes(s, x0, y0, w, h, [p for p, _ in pts], [0, 0.25, 0.5, 0.75, 1.0], "стъпка (в брой int-а, лог. ос)",
         "ns на достъп", fx, fy, yfmt=lambda v: f"{v:g}")
    s.line(fx(16), y0, fx(16), y0 + h, stroke=INK2, sw=1, dash="4 4")
    s.text(fx(16) + 8, y0 + 18, "стъпка 16 int = 64 B = 1 кеш линия", size=14, weight=600)
    s.path(polyline([(fx(p), fy(t)) for p, t in pts]), S1, sw=2)
    for p, t in pts:
        s.circle(fx(p), fy(t), 4.5, S1)
    first, worst = pts[0][1], max(t for _, t in pts)
    s.text(fx(pts[0][0]) + 10, fy(first) + 20, f"{first:.2f} ns", size=14, weight=600)
    s.text(40, 484, f"Под 64 B няколко поредни четения падат в една кеш линия; от 64 B нагоре всяко четене е в "
           f"нова линия (~{worst / first:.0f}× по-бавно).", size=15, fill=INK2)
    s.save(OUT / "stride-chart.svg")


def results_chart(data):
    s = Svg(1100, 440, "Три експеримента: еднаква сложност, различен достъп до паметта")
    s.text(40, 44, "Еднаква сложност, различно време", size=24, weight=700)
    s.text(40, 70, "Release build, примерна машина (AMD Ryzen 7 7800X3D, 96 MB L3)", size=15, fill=INK2)
    d = {k: dict(v) for k, v in data.items()}
    panels = [
        ("Сума на матрица 4096², ms", [("по редове", d["traversal"]["row_major"], S1),
                                        ("по колони", d["traversal"]["col_major"], S2)]),
        ("Транспониране 2048², ms", [("наивно", d["transpose"]["naive"], S2),
                                     ("на блокове", d["transpose"]["blocked32"], S1)]),
        ("Сума на 4M елемента, ns/елемент", [("vector", d["chasing"]["vector"], S1),
                                              ("list", d["chasing"]["list"], S3),
                                              ("разбъркани възли", d["chasing"]["shuffled_nodes"], S2)]),
    ]
    for k, (title, bars, ) in enumerate(panels):
        px = 40 + k * 350
        s.text(px, 116, title, size=16, weight=600)
        vmax = max(v for _, v, _ in bars)
        for j, (lab, v, col) in enumerate(bars):
            y = 136 + j * 76
            s.text(px, y + 18, lab, size=14, fill=INK2)
            bw = max(3, v / vmax * 250)
            s.rect(px, y + 28, bw, 24, col, rx=4)
            fmt = f"{v:.1f}" if v >= 1 else f"{v:.2f}"
            s.text(px + bw + 8, y + 46, fmt, size=14, weight=600)
        ratio = vmax / min(v for _, v, _ in bars)
        s.text(px, 400, f"разлика ×{ratio:.0f}", size=16, weight=700)
    s.save(OUT / "results-chart.svg")


if __name__ == "__main__":
    data = bench()
    memory_hierarchy()
    cache_line()
    row_major()
    pointer_chasing()
    array_decay()
    container_map()
    blocked_transpose()
    stride_chart(data)
    results_chart(data)
