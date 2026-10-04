#!/usr/bin/env python3
"""Generates every SVG figure for week 15 (bits and the course retrospective).

Run from anywhere:  python3 weeks/15-bitwise-retrospective/img/src/make_figures.py
The chart reads bench_sample.csv (`w15_bench --csv`, release preset; the
popcount_native rows were measured with the same program built with
-O2 -march=native).
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
YELLOW = "#fff4d6"
DARK = "#3a3a3a"


def bits_row(s, x0, y0, value, nbits=8, hi=None, label=None, w=40, h=38, show_index=False, fill=TINT1, stroke=S1):
    hi = hi or {}
    for k in range(nbits):
        bit = (value >> (nbits - 1 - k)) & 1
        idx = nbits - 1 - k
        f, st = hi.get(idx, (fill if bit else PANEL, stroke if bit else GRID))
        s.rect(x0 + k * w, y0, w - 4, h, f, st, rx=4)
        s.text(x0 + k * w + (w - 4) / 2, y0 + h / 2 + 6, str(bit), size=17, anchor="middle", family=MONO, weight=700,
               fill=INK if bit else MUTED)
        if show_index:
            s.text(x0 + k * w + (w - 4) / 2, y0 - 8, str(idx), size=11, anchor="middle", fill=MUTED, family=MONO)
    if label:
        s.text(x0 + nbits * w + 14, y0 + h / 2 + 6, label, size=15, family=MONO)


# ---------------------------------------------------------------- bits

def binary():
    s = Svg(1100, 440, "Двоична бройна система")
    s.text(40, 46, "Двоично: всеки бит е степен на 2", size=24, weight=700)
    v = 181
    bits_row(s, 40, 110, v, show_index=True, w=60, h=44)
    for k in range(8):
        p = 7 - k
        s.text(40 + k * 60 + 28, 178, f"2^{p}", size=13, anchor="middle", fill=INK2, family=MONO)
        s.text(40 + k * 60 + 28, 198, str(2 ** p), size=13, anchor="middle", fill=INK2, family=MONO)
    s.text(540, 138, "= 128 + 32 + 16 + 4 + 1 = 181", size=17, family=MONO, weight=700)
    s.text(540, 168, "бит 0 — най-младшият (вдясно)", size=14, fill=INK2)
    s.text(40, 250, "Отрицателни числа: допълнителен код (two's complement), 8 бита", size=16, weight=700)
    bits_row(s, 40, 270, 5, label="  5")
    bits_row(s, 40, 316, (~5) & 0xFF, label=" ~5 (обърни всички битове)")
    bits_row(s, 40, 362, (-5) & 0xFF, label=" −5 = ~5 + 1", fill=TINT2, stroke=S2)
    s.text(560, 392, "най-старшият бит „тежи“ −128: −128 + 64 + 32 + 16 + 8 + 2 + 1 = −5", size=13, fill=INK2)
    s.text(560, 414, "→ събирането работи еднакво за знакови и беззнакови числа", size=13, fill=INK2)
    s.save(OUT / "binary.svg")


def bitops():
    a, b = 0b11001010, 0b10101100
    s = Svg(1100, 470, "Побитови операции")
    s.text(40, 46, "Побитовите операции работят бит по бит, независимо", size=24, weight=700)
    rows = [("a", a), ("b", b), ("a & b", a & b), ("a | b", a | b), ("a ^ b", a ^ b), ("~a", (~a) & 0xFF),
            ("a << 2", (a << 2) & 0xFF), ("a >> 3", a >> 3)]
    notes = ["", "", "и двата — маскиране", "поне единият — слагане на битове", "точно единият — обръщане",
             "обръща всички", "× 4 (изпадналите битове се губят)", "÷ 8 (закръглено надолу)"]
    for k, ((name, v), note) in enumerate(zip(rows, notes)):
        y = 80 + k * 46
        s.text(130, y + 25, name, size=16, anchor="end", family=MONO, weight=700)
        bits_row(s, 150, y, v, w=40, h=36, fill=TINT2 if k >= 2 else TINT1, stroke=S2 if k >= 2 else S1)
        s.text(490, y + 25, note, size=14, fill=INK2)
        if k == 1:
            s.line(150, y + 42, 466, y + 42, stroke=INK2, sw=1)
    s.text(800, 120, "маски", size=16, weight=700)
    for k, t in enumerate(["x & (1u << i)   бит i?", "x | (1u << i)   слагане", "x & ~(1u << i)  махане",
                           "x ^ (1u << i)   обръщане"]):
        s.text(800, 150 + k * 26, t, size=14, family=MONO)
    s.text(800, 280, "Само за unsigned!", size=14, weight=700, fill=RED)
    s.text(800, 302, "1 << 31 за int — UB до C++20;", size=13, fill=INK2)
    s.text(800, 322, "−x >> 1 — зависи от реализацията", size=13, fill=INK2)
    s.save(OUT / "bitops.svg")


def tricks():
    s = Svg(1100, 400, "Два трика")
    s.text(40, 46, "Два трика с най-младшия единичен бит", size=24, weight=700)
    x = 0b01011000
    s.text(40, 96, "x & (x − 1): маха най-младшата единица", size=16, weight=700)
    bits_row(s, 40, 110, x, label="  x = 88", hi={3: (TINT2, S2)})
    bits_row(s, 40, 154, x - 1, label="  x − 1 = 87")
    bits_row(s, 40, 198, x & (x - 1), label="  x & (x − 1) = 80")
    s.text(40, 266, "→ брой единици (Kernighan): повтаряй, докато x ≠ 0", size=14, fill=INK2)
    s.text(40, 288, "→ степен на 2 ⇔ x ≠ 0 и x & (x − 1) == 0", size=14, fill=INK2)
    s.text(600, 96, "x & −x: оставя само нея", size=16, weight=700)
    bits_row(s, 600, 110, x, label="  x", hi={3: (TINT2, S2)})
    bits_row(s, 600, 154, (-x) & 0xFF, label="  −x = ~x + 1")
    bits_row(s, 600, 198, x & -x, label="  x & −x = 8", hi={3: (TINT3, S3)})
    s.text(600, 266, "→ обхождане на единиците една по една", size=14, fill=INK2)
    s.text(600, 288, "  (N цариците, задача 5)", size=14, fill=INK2)
    s.text(40, 360, "„−1“ превръща най-младшата единица в 0, а нулите под нея — в единици. Отрицанието (~x + 1) — обратното.",
           size=14, fill=INK2)
    s.save(OUT / "tricks.svg")


def subsets():
    s = Svg(1100, 420, "Подмножества като битови маски")
    s.text(40, 46, "Подмножествата на {a, b, c} = числата 0 … 7", size=24, weight=700)
    names = ["a", "b", "c"]
    for mask in range(8):
        y = 80 + mask * 40
        s.text(60, y + 24, str(mask), size=15, anchor="end", family=MONO, fill=INK2)
        for k in range(3):
            bit = (mask >> (2 - k)) & 1
            s.rect(80 + k * 40, y, 36, 32, TINT1 if bit else PANEL, S1 if bit else GRID, rx=4)
            s.text(98 + k * 40, y + 22, str(bit), size=15, anchor="middle", family=MONO, weight=700,
                   fill=INK if bit else MUTED)
        members = [names[i] for i in range(3) if mask & (1 << i)]
        s.text(220, y + 22, "{" + ", ".join(members) + "}", size=15, family=MONO)
    s.text(80, 72, "c  b  a", size=12, fill=MUTED, family=MONO)
    s.text(500, 110, "бит i вдигнат ⇔ v[i] е в подмножеството", size=15, weight=700)
    s.text(500, 140, "for (u32 mask = 0; mask < (1u << n); ++mask)", size=14, family=MONO)
    s.text(534, 162, "for (i = 0; i < n; ++i)", size=14, family=MONO)
    s.text(568, 184, "if (mask & (1u << i)) …", size=14, family=MONO)
    s.text(500, 230, "2^n подмножества: при n = 20 — около милион,", size=14, fill=INK2)
    s.text(500, 252, "при n = 40 — трилион. Пълно изброяване", size=14, fill=INK2)
    s.text(500, 274, "става само за малки n.", size=14, fill=INK2)
    s.text(500, 318, "Обединение: m1 | m2; сечение: m1 & m2;", size=14, fill=INK2)
    s.text(500, 340, "разлика: m1 & ~m2; размер: popcount(m).", size=14, fill=INK2)
    s.save(OUT / "subsets.svg")


def nqueens():
    n = 8
    sol = None
    cols = [0] * n

    def place(row, c, d1, d2):
        nonlocal sol
        if sol:
            return
        if row == n:
            sol = list(cols)
            return
        free = ~(c | d1 | d2) & ((1 << n) - 1)
        while free:
            bit = free & -free
            free ^= bit
            cols[row] = bit.bit_length() - 1
            place(row + 1, c | bit, ((d1 | bit) << 1) & ((1 << n) - 1), (d2 | bit) >> 1)
    place(0, 0, 0, 0)
    s = Svg(1100, 470, "N цариците с битови маски")
    s.text(40, 46, "8 царици: ред по ред, атакуваните колони — в три маски", size=24, weight=700)
    cw = 44
    x0, y0 = 40, 80
    for r in range(n):
        for c in range(n):
            dark = (r + c) % 2 == 1
            s.rect(x0 + c * cw, y0 + r * cw, cw, cw, "#e8e4da" if dark else SURFACE, GRID, rx=0, sw=0.5)
        qc = n - 1 - sol[r]  # bit i = column n-1-i, drawn left to right
        s.text(x0 + qc * cw + cw / 2, y0 + r * cw + 32, "♛", size=28, anchor="middle", fill=S2)
    x = 450
    s.text(x, 100, "за всеки ред — три маски от n бита:", size=15, weight=700)
    s.text(x, 128, "cols — заети колони", size=14, family=MONO)
    s.text(x, 150, "d1   — „/“ диагонали (местят се наляво с <<)", size=14, family=MONO)
    s.text(x, 172, "d2   — „\\“ диагонали (местят се надясно с >>)", size=14, family=MONO)
    s.text(x, 214, "free = ~(cols | d1 | d2) & ((1 << n) − 1);", size=14, family=MONO)
    s.text(x, 236, "while (free) {", size=14, family=MONO)
    s.text(x + 34, 258, "bit = free & −free;  free ^= bit;", size=14, family=MONO)
    s.text(x + 34, 280, "place(cols | bit, (d1 | bit) << 1, (d2 | bit) >> 1);", size=14, family=MONO)
    s.text(x, 302, "}", size=14, family=MONO)
    s.text(x, 346, "Никакви масиви, никакви проверки на диагонали —", size=14, fill=INK2)
    s.text(x, 368, "всяка стъпка е няколко инструкции.", size=14, fill=INK2)
    s.text(x, 404, "n = 8: 92 решения; n = 14: 365 596 за ~0.1 s.", size=14, weight=600)
    s.save(OUT / "nqueens.svg")


# ---------------------------------------------------------------- retrospective

WEEKS = [
    ("01", "сложност, тестове"), ("02", "компилатор, кеш"), ("03", "двоично търсене"),
    ("04", "динамичен масив"), ("05", "списък"), ("06", "итератори"),
    ("07", "стек, опашка"), ("08", "хеширане"), ("09", "BST"),
    ("10", "AVL, ч-ч, B-дърво"), ("11", "пирамида, сорт. I"), ("12", "сортиране II"),
    ("13", "BFS, DFS"), ("14", "Дейкстра, MST"), ("15", "битове"),
]


def course_map():
    s = Svg(1100, 590, "Курсът на една страница")
    s.text(40, 46, "Курсът на една страница: всяка тема стъпва на предишните", size=24, weight=700)
    blocks = [("основи", ["01", "02", "03"], S1, TINT1), ("линейни структури", ["04", "05", "06", "07", "08"], S3, TINT3),
              ("дървета", ["09", "10", "11"], S2, TINT2), ("сортиране", ["12"], S4, YELLOW),
              ("графи и битове", ["13", "14", "15"], S5, "#fbe7ef")]
    pos = {}
    y = 90
    for name, ws, col, tint in blocks:
        s.text(40, y + 26, name, size=14, weight=700, fill=col)
        for k, w in enumerate(ws):
            x = 210 + k * 176
            title = dict(WEEKS)[w]
            s.rect(x, y, 164, 42, tint, col, rx=8)
            s.text(x + 10, y + 26, w, size=14, family=MONO, weight=700, fill=col)
            s.text(x + 36, y + 26, title, size=12)
            pos[w] = (x, y)
        y += 72
    for (a, b) in [("01", "02"), ("02", "03"), ("04", "05"), ("05", "06"), ("06", "07"), ("07", "08"), ("09", "10"),
                   ("10", "11"), ("13", "14"), ("14", "15")]:
        (x1, y1), (x2, y2) = pos[a], pos[b]
        s.line(x1 + 166, y1 + 21, x2 - 2, y2 + 21, stroke=MUTED, sw=1.2, arrow=True)
    s.text(40, 446, "Мостове: 03 → 09 (двоично търсене → BST), 04 → 07 (вектор → стек), 07 → 13 (опашка → BFS),",
           size=13, fill=INK2)
    s.text(40, 464, "09 → 10 (BST → AVL), 11 → 14 (пирамида → Дейкстра), 05 → 12 (сливане на списъци → merge sort)", size=13,
           fill=INK2)
    themes = ["Повтарящи се идеи:",
              "• амортизирана сложност — 04 (растеж ×2), 07 (два стека), 09 (итератор), 14 (union-find)",
              "• локалност и кеш — 02, 04 (вектор срещу списък), 10 (B-дървета), 13 (CSR)",
              "• инварианти — 09 (BST), 10 (AVL), 11 (пирамида), 14 (Дейкстра: готовите са окончателни)",
              "• измервай, не гадай — бенчмарк всяка седмица; константите и асимптотиката заедно"]
    for k, t in enumerate(themes):
        s.text(40, 500 + k * 20, t, size=13, fill=INK if k == 0 else INK2, weight=700 if k == 0 else 400)
    s.save(OUT / "course-map.svg")


def which_container():
    s = Svg(1100, 520, "Коя структура?")
    s.text(40, 46, "Коя структура? Въпросите, които решават", size=24, weight=700)

    def q(x, y, t, w=300):
        s.rect(x, y, w, 40, TINT1, S1, rx=20)
        s.text(x + w / 2, y + 25, t, size=14, anchor="middle", weight=600)

    def a(x, y, t, sub, w=210):
        s.rect(x, y, w, 52, TINT3, S3, rx=6)
        s.text(x + w / 2, y + 22, t, size=14, anchor="middle", family=MONO, weight=700)
        s.text(x + w / 2, y + 42, sub, size=12, anchor="middle", fill=INK2)

    def arrow(x1, y1, x2, y2, lab):
        s.line(x1, y1, x2, y2, stroke=INK2, sw=1.4, arrow=True)
        s.text((x1 + x2) / 2 + 6, (y1 + y2) / 2 - 4, lab, size=12, fill=MUTED)
    q(40, 80, "търсене по ключ?")
    q(40, 200, "трябва ли наредба / интервал?", w=300)
    a(40, 320, "std::unordered_map", "Θ(1) средно — седм. 08")
    a(270, 320, "std::map / set", "Θ(log n) — седм. 10")
    arrow(190, 120, 190, 198, "да")
    arrow(150, 240, 145, 318, "не")
    arrow(250, 240, 370, 318, "да")
    q(460, 80, "само най-малкият / най-големият?", w=320)
    a(500, 200, "std::priority_queue", "Θ(log n) — седм. 11", w=240)
    arrow(620, 120, 620, 198, "да")
    arrow(340, 100, 458, 100, "не")
    q(820, 80, "само от краищата?", w=240)
    a(820, 200, "std::deque / stack / queue", "Θ(1) — седм. 07", w=240)
    arrow(940, 120, 940, 198, "да")
    arrow(780, 100, 818, 100, "не")
    q(820, 300, "иначе (индекс, обхождане)", w=240)
    a(820, 400, "std::vector", "почти винаги — седм. 04", w=240)
    s.path("M 1060,100 C 1090,100 1090,320 1062,320", INK2, sw=1.4, arrow=True)
    arrow(940, 340, 940, 398, "")
    s.text(40, 430, "Свързан списък (седм. 05–06) — рядко: когато трябват стабилни итератори при вмъкване в средата,", size=13,
           fill=INK2)
    s.text(40, 450, "или splice за Θ(1). Иначе векторът печели заради локалността (седм. 02).", size=13, fill=INK2)
    s.text(40, 486, "Граф: списъци на съседство (седм. 13). Най-къс път: BFS без тегла, Дейкстра с тегла ≥ 0 (седм. 14).",
           size=13, fill=INK2)
    s.save(OUT / "which-container.svg")


# ---------------------------------------------------------------- chart

def popcount_chart():
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    s = Svg(1100, 420, "Брой единици: четири начина")
    s.text(40, 44, "Брой единици в 64-битово число (ns на число)", size=24, weight=700)
    s.text(40, 70, "10 000 000 случайни числа; release; с и без -march=native", size=15, fill=INK2)
    methods = [("loop", "64 проверки"), ("kernighan", "x &= x − 1"), ("table", "таблица по байт"),
               ("bitset", "std::bitset::count")]
    x0, y0, w = 260, 100, 600
    vmax = 25
    for k, (m, lab) in enumerate(methods):
        y = y0 + k * 66
        s.text(x0 - 12, y + 20, lab, size=14, anchor="end")
        for j, (kind, col, name) in enumerate([("popcount", S1, "по подразбиране"),
                                               ("popcount_native", S2, "-march=native")]):
            v = float(next(r["value"] for r in rows if r["what"] == kind and r["method"] == m))
            yy = y + j * 24
            s.rect(x0, yy, max(2, v / vmax * w), 20, col, "none", rx=3)
            s.text(x0 + max(2, v / vmax * w) + 8, yy + 15, f"{v:.2f}", size=13, family=MONO)
    legend(s, 300, 380, [(S1, "по подразбиране (x86-64 без POPCNT)")])
    legend(s, 640, 380, [(S2, "-march=native")])
    s.text(880, 140, "С -march=native GCC", size=13, fill=INK2)
    s.text(880, 160, "разпознава цикъла на", size=13, fill=INK2)
    s.text(880, 180, "Kernighan и го заменя", size=13, fill=INK2)
    s.text(880, 200, "с една инструкция", size=13, fill=INK2)
    s.text(880, 220, "popcnt (проверено", size=13, fill=INK2)
    s.text(880, 240, "в асемблера).", size=13, fill=INK2)
    s.save(OUT / "popcount-chart.svg")


if __name__ == "__main__":
    binary()
    bitops()
    tricks()
    subsets()
    nqueens()
    course_map()
    which_container()
    popcount_chart()
