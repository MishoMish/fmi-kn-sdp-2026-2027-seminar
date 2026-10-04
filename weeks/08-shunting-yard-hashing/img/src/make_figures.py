#!/usr/bin/env python3
"""Generates every SVG figure for week 08.

Run from anywhere:  python3 weeks/08-shunting-yard-hashing/img/src/make_figures.py
The timing chart reads bench_sample.csv (`w08_bench --csv`, release preset);
the hash-quality and probe-count charts are computed here, with the same
hash functions as starter/hashing.cpp.
"""

import csv
import math
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"


def fnv1a(s):
    h = 14695981039346656037
    for c in s.encode():
        h ^= c
        h = (h * 1099511628211) % 2 ** 64
    return h


def sum_hash(s):
    return sum(s.encode())


def cell(s, x, y, w, h, text, fill=TINT1, stroke=S1, tf=INK, size=15):
    s.rect(x, y, w, h, fill, stroke, rx=5, sw=1.4)
    if text != "":
        s.text(x + w / 2, y + h / 2 + 5, str(text), size=size, anchor="middle", family=MONO, weight=600, fill=tf)


# ---------------------------------------------------------------- expressions

def shunting_trace():
    steps = [
        ("3", "→ изход", "3", ""),
        ("+", "стекът е празен → push", "3", "+"),
        ("4", "→ изход", "3 4", "+"),
        ("*", "* е по-силно от + → push", "3 4", "+ *"),
        ("(", "push", "3 4", "+ * ("),
        ("2", "→ изход", "3 4 2", "+ * ("),
        ("-", "върхът е ( → push", "3 4 2", "+ * ( -"),
        ("1", "→ изход", "3 4 2 1", "+ * ( -"),
        (")", "pop до ( : - → изход; махаме (", "3 4 2 1 -", "+ *"),
        ("край", "pop всичко: *, после +", "3 4 2 1 - * +", ""),
    ]
    s = Svg(1100, 560, "Shunting-yard за 3 + 4 * (2 - 1)")
    s.text(40, 44, "Shunting-yard: 3 + 4 * (2 − 1)", size=24, weight=700)
    cols = [(40, "символ"), (140, "действие"), (560, "изход"), (860, "стек на операторите")]
    for x, t in cols:
        s.text(x, 86, t.upper(), size=12, fill=MUTED, weight=600)
    for k, (tok, act, out, ops) in enumerate(steps):
        y = 100 + k * 44
        s.rect(30, y, 1040, 38, PANEL if k % 2 == 0 else SURFACE, "none", rx=4)
        s.text(56, y + 25, tok, size=16, family=MONO, weight=700, anchor="middle" if tok != "край" else "start",
               fill=S2 if tok in "+-*" else (S3 if tok in "()" else INK))
        s.text(140, y + 25, act, size=14, fill=INK2)
        s.text(560, y + 25, out, size=15, family=MONO, weight=600 if k == len(steps) - 1 else 400)
        s.text(860, y + 25, ops, size=15, family=MONO)
    s.text(40, 548, "Числата отиват направо на изхода; операторите чакат на стека, докато не дойде по-слаб оператор, ) или краят.",
           size=14, fill=INK2)
    s.save(OUT / "shunting-trace.svg")


def rpn_eval():
    tokens = ["3", "4", "2", "1", "-", "*", "+"]
    stacks = [[3], [3, 4], [3, 4, 2], [3, 4, 2, 1], [3, 4, 1], [3, 4], [7]]
    notes = ["", "", "", "", "2−1", "4·1", "3+4"]
    s = Svg(1100, 320, "Пресмятане на постфиксен запис със стек")
    s.text(40, 46, "Пресмятане на 3 4 2 1 − * +: числа → push; оператор → pop два, push резултата", size=19,
           weight=700)
    for k, (t, st, note) in enumerate(zip(tokens, stacks, notes)):
        x = 60 + k * 145
        s.text(x + 45, 92, t, size=22, family=MONO, weight=700, anchor="middle", fill=S2 if t in "-*+" else INK)
        s.path(f"M {x},{110} L {x},{262} L {x + 90},{262} L {x + 90},{110}", MUTED, sw=1.5)
        for i, v in enumerate(st):
            top = i == len(st) - 1
            cell(s, x + 8, 222 - i * 38, 74, 34, v, fill=S1 if top else TINT1, tf=SURFACE if top else INK)
        if note:
            s.text(x + 45, 286, note, size=13, anchor="middle", fill=INK2, family=MONO)
    s.text(40, 312, "Първият изваден е ДЕСНИЯТ операнд: „8 3 −“ е 8 − 3. Накрая — точно едно число.", size=14,
           fill=INK2)
    s.save(OUT / "rpn-eval.svg")


def expression_tree():
    s = Svg(1000, 380, "Дървото на израза и трите записа")
    s.text(40, 46, "Един израз — три записа = три обхождания на дървото", size=22, weight=700)
    nodes = {"+": (300, 100), "3": (180, 190), "*": (420, 190), "4": (340, 280), "-": (500, 280),
             "2": (450, 350), "1": (550, 350)}
    edges = [("+", "3"), ("+", "*"), ("*", "4"), ("*", "-"), ("-", "2"), ("-", "1")]
    for a, b in edges:
        (x1, y1), (x2, y2) = nodes[a], nodes[b]
        s.line(x1, y1 + 18, x2, y2 - 18, stroke=INK2, sw=1.5)
    for name, (x, y) in nodes.items():
        op = name in "+*-"
        s.circle(x, y, 20, TINT2 if op else TINT1, stroke=S2 if op else S1, sw=1.6)
        s.text(x, y + 6, name.replace("-", "−"), size=16, anchor="middle", family=MONO, weight=700)
    rows = [("инфиксен (in-order)", "3 + 4 * (2 − 1)", "нужни са скоби"),
            ("постфиксен (post-order)", "3 4 2 1 − * +", "RPN — без скоби"),
            ("префиксен (pre-order)", "+ 3 * 4 − 2 1", "полски запис — без скоби")]
    for k, (name, expr, note) in enumerate(rows):
        y = 140 + k * 70
        s.text(640, y, name, size=14, fill=MUTED, weight=600)
        s.text(640, y + 26, expr, size=18, family=MONO, weight=600)
        s.text(640, y + 48, note, size=13, fill=INK2)
    s.save(OUT / "expression-tree.svg")


# ---------------------------------------------------------------- hashing

def hash_concept():
    s = Svg(1100, 400, "Хеш таблица: ключ → хеш → индекс")
    s.text(40, 46, "Ключ → хеш функция → индекс в масива", size=24, weight=700)
    keys = ["\"tree\"", "\"heap\"", "\"graph\""]
    idx = [5, 1, 5]
    for k, (key, i) in enumerate(zip(keys, idx)):
        y = 100 + k * 64
        s.rect(40, y, 140, 42, TINT1, S1, rx=6)
        s.text(110, y + 27, key, size=15, anchor="middle", family=MONO)
        s.line(184, y + 21, 280, y + 21, stroke=INK2, sw=1.4, arrow=True)
        s.rect(284, y, 120, 42, TINT2, S2, rx=6)
        s.text(344, y + 27, "hash()", size=15, anchor="middle", family=MONO)
        s.line(408, y + 21, 500, y + 21, stroke=INK2, sw=1.4, arrow=True)
        s.text(560, y + 27, f"% 8 = {i}", size=15, anchor="middle", family=MONO, weight=600)
        s.path(f"M 616,{y + 21} C 700,{y + 21} {721 + i * 46},{y + 60} {721 + i * 46},{276}",
               S1 if k < 2 else RED, sw=1.4, arrow=True, dash=None if k < 2 else "5 4")
    for i in range(8):
        cell(s, 700 + i * 46, 280, 42, 42, "", fill=PANEL, stroke=GRID)
        s.text(721 + i * 46, 342, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)
    s.text(40, 330, "\"tree\" и \"graph\" → 5: КОЛИЗИЯ", size=16, weight=700, fill=RED)
    s.text(40, 380, "Колизиите са неизбежни (повече възможни ключове, отколкото клетки). Въпросът е само как ги обработваме.",
           size=15, fill=INK2)
    s.save(OUT / "hash-concept.svg")


def chaining():
    s = Svg(1000, 380, "Разрешаване на колизии чрез списъци (separate chaining)")
    s.text(40, 46, "Отделни вериги (separate chaining): всяка клетка е списък", size=22, weight=700)
    chains = {0: [], 1: ["heap"], 2: [], 3: ["list", "queue", "stack"], 4: [], 5: ["tree", "graph"], 6: [], 7: ["set"]}
    for i in range(8):
        y = 80 + i * 36
        cell(s, 60, y, 50, 30, str(i), fill=PANEL, stroke=GRID, tf=MUTED, size=13)
        for j, key in enumerate(chains[i]):
            x = 160 + j * 150
            cell(s, x, y, 120, 30, key, fill=TINT1, size=14)
            s.line(x - 36 if j else 114, y + 15, x - 2, y + 15, stroke=INK2, sw=1.3, arrow=True)
    s.text(640, 120, "insert: в началото/края на веригата", size=14)
    s.text(640, 146, "find: обхождаме само една верига", size=14)
    s.text(640, 196, "load factor α = n / m", size=15, weight=700, family=MONO)
    s.text(640, 222, "средна дължина на верига = α", size=14, fill=INK2)
    s.text(640, 248, "α ≤ 1 → Θ(1) средно", size=14, fill=INK2)
    s.text(640, 274, "при α > 1 → rehash с 2m клетки", size=14, fill=INK2)
    s.save(OUT / "chaining.svg")


def probing_frames():
    m = 10
    frames = [
        ("insert 13: 13 % 10 = 3 — свободно", {3: "13"}, [3], None),
        ("insert 23: 3 е заето → 4", {3: "13", 4: "23"}, [3, 4], None),
        ("insert 33: 3, 4 са заети → 5. Образува се клъстер.", {3: "13", 4: "23", 5: "33"}, [3, 4, 5], None),
        ("erase 23: НЕ изпразваме клетката — слагаме надгробен камък †", {3: "13", 4: "†", 5: "33"}, [4], None),
        ("contains 33: 3 (не) → 4 (†, продължаваме!) → 5 (намерено)", {3: "13", 4: "†", 5: "33"}, [3, 4, 5], 5),
    ]
    for k, (caption, cells_, path, found) in enumerate(frames):
        s = Svg(1000, 260, f"Линейно пробване, стъпка {k + 1}")
        s.text(40, 44, "Отворено адресиране, линейно пробване: (h + i) % m", size=21, weight=700)
        for i in range(m):
            v = cells_.get(i, "")
            if v == "†":
                fill, stroke = TINT_RED, RED
            elif v:
                fill, stroke = (S3 if found == i else TINT1), S1 if found != i else S3
            else:
                fill, stroke = PANEL, GRID
            cell(s, 60 + i * 88, 100, 80, 48, v, fill=fill, stroke=stroke, tf=SURFACE if found == i else INK)
            s.text(100 + i * 88, 170, str(i), size=13, anchor="middle", fill=MUTED, family=MONO)
        for a, b in zip(path, path[1:]):
            xa, xb = 100 + a * 88, 100 + b * 88
            s.path(f"M {xa},96 Q {(xa + xb) / 2},70 {xb},96", S2, sw=2, arrow=True)
        s.text(40, 230, caption, size=16, weight=600)
        s.save(OUT / f"probing-{k + 1}.svg")


def bad_hash_chart():
    keys = [f"key{i}" for i in range(10_000)]
    m = 16384
    results = []
    for name, f, col in [("FNV-1a", fnv1a, S1), ("сума на символите", sum_hash, RED)]:
        counts = Counter(f(k) % m for k in keys)
        lengths = Counter(counts.values())
        results.append((name, col, len(counts), max(counts.values()), lengths))
    s = Svg(1100, 500, "Добра и лоша хеш функция върху 10 000 ключа key0…key9999")
    s.text(40, 44, "10 000 ключа „key0“ … „key9999“ в 16 384 клетки", size=24, weight=700)
    s.text(40, 70, "Колко клетки имат верига с дадена дължина (FNV-1a) — и какво прави сумата на символите", size=15,
           fill=INK2)
    name, col, used, longest, lengths = results[0]
    x0, y0, w, h = 100, 110, 480, 280
    ymax = 10000
    fy = lambda v: y0 + h - min(v, ymax) / ymax * h
    for t in [0, 2500, 5000, 7500, 10000]:
        s.line(x0, fy(t), x0 + w, fy(t), stroke=GRID, sw=1)
        s.text(x0 - 10, fy(t) + 5, f"{t:,}".replace(",", " "), size=12, fill=INK2, anchor="end")
    total_buckets = m
    empty = total_buckets - used
    bars = [(0, empty)] + sorted(lengths.items())
    bw = w / 8
    for i, (length, count) in enumerate(bars[:8]):
        x = x0 + i * bw + 6
        s.rect(x, fy(count), bw - 12, y0 + h - fy(count), col, rx=3)
        s.text(x + (bw - 12) / 2, fy(count) - 6, str(count), size=12, anchor="middle", weight=600)
        s.text(x + (bw - 12) / 2, y0 + h + 18, str(length), size=13, anchor="middle", fill=INK2, family=MONO)
    s.text(x0 + w / 2, y0 + h + 42, "дължина на веригата (FNV-1a)", size=14, fill=INK2, anchor="middle")
    s.text(x0, y0 - 10, f"FNV-1a: заети {used} клетки, най-дълга верига {longest}", size=14, weight=700)
    _, _, used_s, longest_s, _ = results[1]
    px = 650
    s.rect(px, 120, 410, 240, TINT_RED, RED, rx=12)
    s.text(px + 20, 156, "сума на символите", size=17, weight=700)
    s.text(px + 20, 196, f"заети клетки: {used_s} от {m}", size=16, family=MONO)
    s.text(px + 20, 226, f"най-дълга верига: {longest_s}", size=16, family=MONO)
    s.text(px + 20, 266, "„key123“, „key132“, „key213“, …", size=14, fill=INK2)
    s.text(px + 20, 288, "имат еднаква сума → един и същ индекс.", size=14, fill=INK2)
    s.text(px + 20, 326, f"Търсене: до {longest_s} сравнения вместо ~1.", size=14, weight=700)
    s.text(40, 484, "Хеш таблицата е бърза само толкова, колкото е равномерна хеш функцията.", size=15, fill=INK2)
    s.save(OUT / "bad-hash.svg")
    return results


def probes_chart():
    s = Svg(1000, 480, "Очакван брой проби спрямо запълването")
    s.text(40, 44, "Очакван брой проверени клетки спрямо α = n / m", size=24, weight=700)
    s.text(40, 70, "Класическите оценки (Knuth) при равномерна хеш функция", size=15, fill=INK2)
    x0, y0, w, h = 100, 100, 560, 300
    ymax = 10
    fx = lambda a: x0 + a / 0.95 * w
    fy = lambda v: y0 + h - min(v, ymax) / ymax * h
    axes(s, x0, y0, w, h, [0, 0.25, 0.5, 0.75, 0.95], [0, 2, 4, 6, 8, 10], "α (запълване)", "проби", fx, fy,
         xfmt=lambda v: f"{v:g}")
    curves = [
        (S1, "вериги, успешно: 1 + α/2", lambda a: 1 + a / 2),
        (S3, "вериги, неуспешно: α", lambda a: max(a, 1e-9)),
        (S2, "линейно, успешно: ½(1 + 1/(1−α))", lambda a: 0.5 * (1 + 1 / (1 - a))),
        (RED, "линейно, неуспешно: ½(1 + 1/(1−α)²)", lambda a: 0.5 * (1 + 1 / (1 - a) ** 2)),
    ]
    for col, lab, f in curves:
        pts = []
        a = 0.0
        while a <= 0.95:
            pts.append((fx(a), fy(f(a))))
            if f(a) > ymax:
                break
            a += 0.005
        s.path(polyline(pts), col, sw=2)
    s.line(fx(0.5), y0, fx(0.5), y0 + h, stroke=INK2, sw=1, dash="4 4")
    s.text(fx(0.5) + 6, y0 + 16, "α = 0.5 (нашият праг)", size=13, weight=600)
    legend(s, 690, 130, [(c, l) for c, l, _ in curves])
    s.text(690, 260, "При α = 0.5 линейното", size=14, fill=INK2)
    s.text(690, 280, "пробване прави 1.5 / 2.5 проби;", size=14, fill=INK2)
    s.text(690, 300, "при α = 0.9 — 5.5 / 50.5.", size=14, fill=INK2)
    s.text(690, 336, "Затова отвореното адресиране", size=14, fill=INK2)
    s.text(690, 356, "се преоразмерява много по-рано.", size=14, fill=INK2)
    s.save(OUT / "probes-chart.svg")


def bench_chart():
    rows = list(csv.DictReader((HERE / "bench_sample.csv").open()))
    s = Svg(1100, 500, "Време на операция в хеш таблици и в дърво")
    s.text(40, 44, "ns на операция, string ключове", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина; вмъкване + търсене на присъстващи + търсене на липсващи", size=15,
           fill=INK2)
    x0, y0, w, h = 100, 100, 560, 320
    fx = lambda n: x0 + (math.log10(n) - 3) / 3 * w
    fy = lambda v: y0 + h - (math.log10(v) - 1) / 2 * h
    axes(s, x0, y0, w, h, [1000, 10000, 100000, 1000000], [10, 30, 100, 300, 1000], "n (лог. ос)",
         "ns (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    series = [(RED, "вериги + сума на символите", "chained_sum"), (S5, "std::map (дърво)", "map"),
              (S1, "вериги + std::hash (вашата)", "chained_std"), (S3, "линейно пробване (вашето)", "probing"),
              (S4, "std::unordered_map", "unordered_map")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 700, 130, [(c, l) for c, l, _ in series])
    s.text(700, 280, "Лошата хеш функция расте", size=14, fill=INK2)
    s.text(700, 300, "линейно с n — по-бавна от", size=14, fill=INK2)
    s.text(700, 320, "дървото още при 10 000.", size=14, fill=INK2)
    s.text(700, 356, "Добрите хеш таблици: почти", size=14, fill=INK2)
    s.text(700, 376, "плоски (Θ(1) средно); леко", size=14, fill=INK2)
    s.text(700, 396, "нагоре заради кеша.", size=14, fill=INK2)
    s.save(OUT / "hash-bench.svg")


if __name__ == "__main__":
    shunting_trace()
    rpn_eval()
    expression_tree()
    hash_concept()
    chaining()
    probing_frames()
    res = bad_hash_chart()
    for name, _, used, longest, _ in res:
        print(f"{name}: {used} buckets used, longest chain {longest}")
    probes_chart()
    bench_chart()
