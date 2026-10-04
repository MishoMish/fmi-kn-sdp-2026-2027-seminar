#!/usr/bin/env python3
"""Generates every SVG figure for week 06.

Run from anywhere:  python3 weeks/06-doubly-linked-list-iterators/img/src/make_figures.py
The chart reads bench_sample.csv (`w06_bench --csv`, release preset).
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
W3, H3 = 120, 46


def dnode(s, x, y, value, fill=TINT1, stroke=S1, label=None, sentinel=False):
    """prev | value | next box. Returns dict of anchor points."""
    s.rect(x, y, W3, H3, PANEL if sentinel else fill, MUTED if sentinel else stroke, rx=6, sw=1.6,
           dash="5 4" if sentinel else None)
    s.line(x + 28, y, x + 28, y + H3, stroke=MUTED if sentinel else stroke, sw=1.1)
    s.line(x + 92, y, x + 92, y + H3, stroke=MUTED if sentinel else stroke, sw=1.1)
    s.text(x + 60, y + 29, "•" if sentinel else str(value), size=16, anchor="middle", family=MONO, weight=600,
           fill=MUTED if sentinel else INK)
    if label:
        s.text(x + 60, y + H3 + 20, label, size=13, anchor="middle", fill=MUTED, family=MONO)
    return {"prev": (x + 14, y + H3 * 0.68), "next": (x + 106, y + H3 * 0.32), "left": (x, y + H3 * 0.68),
            "right": (x + W3, y + H3 * 0.32), "x": x, "y": y}


def link_fwd(s, a, b, col=INK2, sw=1.5, dash=None):
    s.line(a["next"][0], a["next"][1], b["x"] - 2, b["y"] + H3 * 0.32, stroke=col, sw=sw, arrow=True, dash=dash)


def link_back(s, a, b, col=INK2, sw=1.5, dash=None):
    """b.prev -> a (arrow from b's prev cell back to a's right edge)."""
    s.line(b["prev"][0], b["prev"][1], a["x"] + W3 + 2, a["y"] + H3 * 0.68, stroke=col, sw=sw, arrow=True, dash=dash)


# ---------------------------------------------------------------- structure

def anatomy():
    s = Svg(1100, 360, "Двусвързан цикличен списък с фиктивен възел")
    s.text(40, 46, "Двусвързан списък с фиктивен възел (като std::list)", size=24, weight=700)
    xs = [60, 290, 520, 750]
    nodes = [dnode(s, xs[0], 150, "", sentinel=True, label="sentinel_ = end()")]
    for i, v in enumerate("abc"):
        nodes.append(dnode(s, xs[i + 1], 150, v, label="begin()" if i == 0 else None))
    for a, b in zip(nodes, nodes[1:]):
        link_fwd(s, a, b)
        link_back(s, a, b)
    last, sent = nodes[-1], nodes[0]
    s.path(f"M {last['next'][0]},{last['next'][1]} C 960,120 960,96 520,96 C 160,96 120,110 120,148", S1, sw=1.6,
           arrow=True)
    s.path(f"M {sent['prev'][0]},{sent['prev'][1] + 6} C 74,260 300,262 520,262 C 820,262 830,250 830,198", S2, sw=1.6,
           arrow=True)
    s.text(530, 88, "c.next = sentinel", size=13, fill=S1, anchor="middle", family=MONO)
    s.text(530, 282, "sentinel.prev = c  (затова --end() е последният)", size=13, fill=S2, anchor="middle", family=MONO)
    s.text(40, 330, "Всеки истински възел винаги има истински prev и next → insert и erase нямат специални случаи.",
           size=15, fill=INK2)
    s.save(OUT / "dll-sentinel.svg")


def empty_list():
    s = Svg(700, 230, "Празен списък: фиктивният възел сочи себе си")
    s.text(40, 46, "Празен списък", size=22, weight=700)
    n = dnode(s, 240, 100, "", sentinel=True, label="sentinel_")
    s.path(f"M {n['next'][0]},{n['next'][1]} C 420,80 420,60 300,60 C 230,60 240,80 250,98", S1, sw=1.6, arrow=True)
    s.path(f"M {n['prev'][0]},{n['prev'][1]} C 200,190 260,196 320,196 C 400,196 380,170 350,148", S2, sw=1.6,
           arrow=True)
    s.text(470, 128, "next = prev = &sentinel_", size=14, family=MONO)
    s.text(470, 152, "begin() == end()", size=14, family=MONO, weight=700)
    s.save(OUT / "dll-empty.svg")


def insert_frames():
    steps = [
        ("insert(pos, x): x застава ПРЕД pos", 0),
        ("1. node->prev = prev; node->next = next   (новият е готов, никой още не го вижда)", 1),
        ("2. prev->next = node", 2),
        ("3. next->prev = node   — 4 указателя, Θ(1), без специални случаи", 3),
    ]
    for k, (caption, stage) in enumerate(steps):
        s = Svg(1100, 340, f"Вмъкване в двусвързан списък, стъпка {k + 1}")
        s.text(40, 44, "insert: четири указателя", size=22, weight=700)
        a = dnode(s, 120, 110, "a", fill=TINT2, stroke=S2, label="prev")
        b = dnode(s, 760, 110, "b", label="next = pos")
        if stage < 2:
            link_fwd(s, a, b)
        if stage < 3:
            link_back(s, a, b)
        if stage >= 1:
            n = dnode(s, 440, 220, "x", fill=TINT3, stroke=S3)
            s.line(n["prev"][0], n["prev"][1], a["x"] + W3 - 10, a["y"] + H3 + 2, stroke=S3, sw=2, arrow=True)
            s.line(n["next"][0], n["next"][1], b["x"] + 10, b["y"] + H3 + 2, stroke=S3, sw=2, arrow=True)
            if stage >= 2:
                s.line(a["next"][0], a["next"][1], n["x"] + 30, n["y"] - 2, stroke=S3, sw=2.4, arrow=True)
            if stage >= 3:
                s.line(b["prev"][0], b["prev"][1], n["x"] + 90, n["y"] - 2, stroke=S3, sw=2.4, arrow=True)
        s.text(40, 318, caption, size=16, weight=600)
        s.save(OUT / f"dll-insert-{k + 1}.svg")


def erase_frames():
    steps = [
        ("erase(pos): victim = pos", 0),
        ("1. victim->prev->next = victim->next;   victim->next->prev = victim->prev", 1),
        ("2. delete victim; return iterator(next) — итераторите към a и c остават валидни", 2),
    ]
    for k, (caption, stage) in enumerate(steps):
        s = Svg(1100, 300, f"Изтриване от двусвързан списък, стъпка {k + 1}")
        s.text(40, 44, "erase: не ни трябва нищо освен самия възел", size=22, weight=700)
        a = dnode(s, 80, 130, "a")
        if stage < 2:
            v = dnode(s, 440, 130, "b", fill=TINT_RED, stroke=RED, label="victim")
        else:
            s.rect(440, 130, W3, H3, SURFACE, GRID, rx=6, dash="5 4")
            s.text(500, 158, "delete", size=14, anchor="middle", fill=MUTED)
        c = dnode(s, 800, 130, "c")
        if stage == 0:
            link_fwd(s, a, v)
            link_back(s, a, v)
            link_fwd(s, v, c)
            link_back(s, v, c)
        else:
            s.path(f"M {a['next'][0]},{a['next'][1]} C 300,70 680,70 {c['x'] + 4},{c['y'] - 2}", S3, sw=2.2, arrow=True)
            s.path(f"M {c['prev'][0]},{c['prev'][1] + 10} C 680,240 300,240 {a['x'] + W3 - 4},{a['y'] + H3 + 2}", S3,
                   sw=2.2, arrow=True)
            if stage == 1:
                link_fwd(s, v, c, dash="4 4")
                link_back(s, a, v, dash="4 4")
        s.text(40, 284, caption, size=16, weight=600)
        s.save(OUT / f"dll-erase-{k + 1}.svg")


def move_relink():
    s = Svg(1100, 400, "Защо преместването трябва да пренасочи двата крайни възела")
    s.text(40, 46, "Преместване: фиктивният възел НЕ се мести с възлите", size=24, weight=700)
    s.text(40, 72, "sentinel_ е поле на обекта; първият и последният възел сочат адреса на стария sentinel_", size=15,
           fill=INK2)
    oa = dnode(s, 60, 120, "", sentinel=True, label="other.sentinel_")
    na = dnode(s, 60, 270, "", sentinel=True, label="this->sentinel_")
    first = dnode(s, 400, 190, "a")
    last = dnode(s, 760, 190, "c")
    link_fwd(s, first, last, dash="3 5")
    s.text(640, 182, "…", size=20, anchor="middle")
    s.path(f"M {first['prev'][0]},{first['prev'][1]} C 340,230 260,170 182,150", RED, sw=2, arrow=True, dash="5 4")
    s.path(f"M {last['next'][0]},{last['next'][1]} C 960,120 600,110 182,128", RED, sw=2, arrow=True, dash="5 4")
    s.path(f"M {first['prev'][0]},{first['prev'][1] + 4} C 340,280 260,300 182,300", S3, sw=2.4, arrow=True)
    s.path(f"M {last['next'][0]},{last['next'][1] + 20} C 980,330 600,320 182,284", S3, sw=2.4, arrow=True)
    s.line(74, 370, 104, 370, stroke=RED, sw=2, dash="5 4")
    s.text(112, 375, "преди: сочат стария sentinel → висящи указатели след преместването", size=14)
    s.line(600, 370, 630, 370, stroke=S3, sw=2.4)
    s.text(638, 375, "stealNodes: пренасочваме ги към новия", size=14)
    s.save(OUT / "dll-move.svg")


def splice_fig():
    s = Svg(1100, 380, "splice: преместване на всички възли за Θ(1)")
    s.text(40, 46, "splice(pos, other): Θ(1), без копиране", size=24, weight=700)
    s.text(40, 92, "преди", size=14, fill=MUTED, weight=600)
    a1 = dnode(s, 60, 104, "1")
    a2 = dnode(s, 260, 104, "5", label="pos")
    link_fwd(s, a1, a2)
    link_back(s, a1, a2)
    b1 = dnode(s, 560, 104, "2", fill=TINT3, stroke=S3)
    b2 = dnode(s, 760, 104, "4", fill=TINT3, stroke=S3)
    link_fwd(s, b1, b2, dash="3 5")
    s.text(700, 96, "other", size=14, anchor="middle", fill=S3, weight=700)
    s.text(40, 230, "след", size=14, fill=MUTED, weight=600)
    xs = [60, 260, 460, 660]
    vals = ["1", "2", "4", "5"]
    nodes = [dnode(s, x, 242, v, fill=TINT3 if v in "24" else TINT1, stroke=S3 if v in "24" else S1)
             for x, v in zip(xs, vals)]
    for i, (a, b) in enumerate(zip(nodes, nodes[1:])):
        col = S3 if i != 1 else INK2
        link_fwd(s, a, b, col=col, dash="3 5" if i == 1 else None)
        link_back(s, a, b, col=col, dash="3 5" if i == 1 else None)
    s.text(880, 270, "4 указателя се сменят;", size=14, fill=INK2)
    s.text(880, 292, "other остава празен.", size=14, fill=INK2)
    s.save(OUT / "dll-splice.svg")


# ---------------------------------------------------------------- iterators

def bridge():
    s = Svg(1100, 400, "Итераторите свързват N контейнера с M алгоритъма")
    s.text(40, 46, "Без итератори: N × M функции. С итератори: N + M.", size=24, weight=700)
    conts = ["List<T>", "std::vector", "C масив", "forward_list", "std::deque"]
    algs = ["find", "count", "reverse", "sort*", "accumulate"]
    for i, c in enumerate(conts):
        s.rect(60, 100 + i * 54, 180, 40, TINT1, S1, rx=6)
        s.text(150, 126 + i * 54, c, size=15, anchor="middle", family=MONO)
        s.line(242, 120 + i * 54, 446, 230, stroke=S1, sw=1.3, arrow=True)
    s.rect(450, 190, 200, 80, TINT2, S2, rx=10, sw=2)
    s.text(550, 224, "итератори", size=18, anchor="middle", weight=700)
    s.text(550, 248, "*, ++, ==, (--, +n)", size=14, anchor="middle", family=MONO, fill=INK2)
    for i, a in enumerate(algs):
        s.rect(860, 100 + i * 54, 180, 40, TINT3, S3, rx=6)
        s.text(950, 126 + i * 54, a, size=15, anchor="middle", family=MONO)
        s.line(652, 230, 856, 120 + i * 54, stroke=S3, sw=1.3, arrow=True)
    s.text(40, 384, "Контейнерът казва КАК се върви; алгоритъмът — КАКВО се прави. * sort изисква произволен достъп.",
           size=15, fill=INK2)
    s.save(OUT / "iterator-bridge.svg")


def categories():
    s = Svg(1100, 470, "Категории итератори")
    s.text(40, 46, "Категории итератори: всяка може всичко от предишната и още", size=24, weight=700)
    levels = [
        ("input / forward", "*it, ++it, ==", "forward_list, unordered_map, istream", TINT1, S1),
        ("bidirectional", "+ --it", "List (днес), std::list, set, map", TINT3, S3),
        ("random access", "+ it + n, it[n], it2 − it1, <", "deque", TINT2, S2),
        ("contiguous", "+ елементите са един до друг в паметта", "vector, array, C масив, string", TINT5, S5),
    ]
    for k, (name, ops, ex, fill, col) in enumerate(levels):
        x, y = 40 + k * 30, 80 + k * 76
        w = 1020 - 2 * k * 30
        s.rect(x, y, w, 340 - k * 76, fill, col, rx=12, sw=1.6)
        s.text(x + 18, y + 28, name, size=17, weight=700)
        s.text(x + 230, y + 28, ops, size=15, family=MONO)
        s.text(x + w - 18, y + 28, ex, size=14, fill=INK2, anchor="end")
    s.text(40, 456, "Алгоритъм обявява какво му трябва: std::find — forward, std::reverse — bidirectional, std::sort — random access.",
           size=14, fill=INK2)
    s.save(OUT / "iterator-categories.svg")


def half_open():
    s = Svg(1000, 260, "Полуотвореният интервал [begin, end)")
    s.text(40, 46, "[begin(), end()): end() е „след последния“", size=22, weight=700)
    xs = [80, 260, 440, 620]
    for i, x in enumerate(xs):
        if i < 3:
            dnode(s, x, 110, "abc"[i])
        else:
            dnode(s, x, 110, "", sentinel=True)
    for x, lab in [(140, "begin()"), (680, "end()"), (500, "std::prev(end())")]:
        s.line(x, 92, x, 106, stroke=INK, sw=2, arrow=True)
        s.text(x, 86, lab, size=14, anchor="middle", family=MONO, weight=700)
    s.text(40, 210, "Празен интервал: begin() == end(). Брой елементи: distance(begin, end). Цикълът е винаги",
           size=15, fill=INK2)
    s.text(40, 234, "for (it = begin; it != end; ++it) — никога <= и никога *end().", size=15, fill=INK2)
    s.save(OUT / "half-open.svg")


# ---------------------------------------------------------------- results

def sorted_chart():
    rows = list(csv.DictReader((HERE / "bench_sample.csv").open()))
    sorted_rows = [r for r in rows if r["part"] == "sorted"]
    middle = [r for r in rows if r["part"] == "middle"]
    s = Svg(1100, 520, "Опитът на Страуструп: сортирано вмъкване във вектор и списък")
    s.text(40, 44, "Сортирано вмъкване: търсене + вмъкване", size=24, weight=700)
    s.text(40, 70, "n случайни числа, всяко вмъкнато на мястото си (линейно търсене). И двете са Θ(n²).",
           size=15, fill=INK2)
    x0, y0, w, h = 100, 100, 520, 330
    nmax = 32000
    ymax = 1400
    fx = lambda n: x0 + n / nmax * w
    fy = lambda ms: y0 + h - ms / ymax * h
    axes(s, x0, y0, w, h, [0, 8000, 16000, 24000, 32000], [0, 350, 700, 1050, 1400], "n", "ms", fx, fy)
    series = [(S5, "std::list", "std_list"), (S2, "вашият List", "your_list"), (S1, "std::vector", "vector")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in sorted_rows]
        s.path(polyline([(fx(0), fy(0))] + pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 120, 130, [(c, l) for c, l, _ in series])
    last = sorted_rows[-1]
    s.text(x0 + w - 4, fy(float(last["vector"])) - 12, f"{float(last['vector']):.0f} ms", size=13, anchor="end",
           weight=600)
    s.text(x0 + w - 4, fy(float(last["your_list"])) - 12, f"{float(last['your_list']):.0f} ms", size=13,
           anchor="end", weight=600)

    px = 700
    s.text(px, 120, "Позицията вече е известна", size=16, weight=700)
    s.text(px, 142, "(1000 вмъквания в средата, ns всяко)", size=13, fill=INK2)
    vmax = max(float(r["vector"]) for r in middle)
    for j, r in enumerate(middle):
        y = 168 + j * 72
        n = int(r["n"])
        s.text(px, y + 14, f"размер {n:,}".replace(",", " "), size=13, fill=INK2)
        for k, (key, col) in enumerate([("vector", S1), ("your_list", S2)]):
            v = float(r[key])
            bw = max(3, math.log10(v + 1) / math.log10(vmax + 1) * 260)
            s.rect(px, y + 20 + k * 22, bw, 16, col, rx=3)
            s.text(px + bw + 8, y + 33 + k * 22, f"{v:,.0f}".replace(",", " "), size=12, weight=600)
    s.text(px, 470, "(дължина на стълбчето — лог. скала)", size=12, fill=MUTED)
    s.rect(px + 230, 460, 14, 12, S1, rx=2)
    s.text(px + 250, 470, "vector", size=12)
    s.rect(px + 310, 460, 14, 12, S2, rx=2)
    s.text(px + 330, 470, "вашият List", size=12)
    s.text(40, 504, "Когато мястото трябва да се НАМЕРИ, търсенето доминира — и векторът търси много по-бързо (кеш).",
           size=15, fill=INK2)
    s.save(OUT / "sorted-insert-chart.svg")


if __name__ == "__main__":
    anatomy()
    empty_list()
    insert_frames()
    erase_frames()
    move_relink()
    splice_fig()
    bridge()
    categories()
    half_open()
    sorted_chart()
