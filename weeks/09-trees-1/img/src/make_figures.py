#!/usr/bin/env python3
"""Generates every SVG figure for week 09.

Run from anywhere:  python3 weeks/09-trees-1/img/src/make_figures.py
The timing chart reads bench_sample.csv (`w09_bench --csv`, release preset);
the height chart is simulated here (random insertion orders, fixed seed).
"""

import csv
import math
import random
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"
R = 20  # node radius

# A binary tree is a nested tuple (label, left, right); None is empty.
SAMPLE = (50, (30, (20, None, None), (40, (35, None, None), (45, None, None))),
          (70, (60, None, None), (80, None, (90, None, None))))


def bst_from(keys):
    def ins(t, k):
        if t is None:
            return (k, None, None)
        v, l, r = t
        if k < v:
            return (v, ins(l, k), r)
        if k > v:
            return (v, l, ins(r, k))
        return t
    t = None
    for k in keys:
        t = ins(t, k)
    return t


def layout_bin(tree, x0, y0, dx, dy):
    """In-order x, depth y: a BST's keys line up left to right."""
    pos, edges = {}, []
    counter = [0]

    def go(t, d, parent):
        if t is None:
            return
        v, l, r = t
        go(l, d + 1, v)
        pos[v] = (x0 + counter[0] * dx, y0 + d * dy)
        counter[0] += 1
        go(r, d + 1, v)
        if parent is not None:
            edges.append((parent, v))
    go(tree, 0, None)
    return pos, edges


def draw_nodes(s, pos, edges, fill=None, stroke=None, edge_style=None, r=R, size=16):
    fill = fill or {}
    stroke = stroke or {}
    edge_style = edge_style or {}
    for a, b in edges:
        (x1, y1), (x2, y2) = pos[a], pos[b]
        col, sw, dash = edge_style.get((a, b), (INK2, 1.5, None))
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        s.line(x1 + ux * r, y1 + uy * r, x2 - ux * r, y2 - uy * r, stroke=col, sw=sw, dash=dash)
    for v, (x, y) in pos.items():
        s.circle(x, y, r, fill.get(v, TINT1), stroke=stroke.get(v, S1), sw=1.6)
        s.text(x, y + size * 0.36, str(v), size=size, anchor="middle", family=MONO, weight=700)


def draw_bin(s, tree, x0, y0, dx, dy, **kw):
    pos, edges = layout_bin(tree, x0, y0, dx, dy)
    draw_nodes(s, pos, edges, **kw)
    return pos, edges


def height(t):
    return -1 if t is None else 1 + max(height(t[1]), height(t[2]))


# ---------------------------------------------------------------- terminology

def tree_terms():
    s = Svg(1100, 470, "Дърво: основни понятия")
    s.text(40, 46, "Дърво: корен, деца, листа, дълбочина, височина", size=24, weight=700)
    kids = {"A": "BCD", "B": "EF", "C": "G", "D": "HIJ", "F": "KL"}
    # leaves get consecutive x; parents are centered above their children
    pos = {}
    nxt = [0]

    def place(v, d):
        ch = kids.get(v, "")
        for c in ch:
            place(c, d + 1)
        if ch:
            x = (pos[ch[0]][0] + pos[ch[-1]][0]) / 2
        else:
            x = 230 + nxt[0] * 62
            nxt[0] += 1
        pos[v] = (x, 110 + d * 90)
    place("A", 0)
    edges = [(p, c) for p, cs in kids.items() for c in cs]
    leaves = {v for v in pos if v not in kids}
    path = {("A", "B"), ("B", "F"), ("F", "K")}
    # subtree of D
    xs = [pos[v][0] for v in "DHIJ"]
    s.rect(min(xs) - 34, pos["D"][1] - 32, max(xs) - min(xs) + 68, 154, "none", S4, rx=14, sw=1.6, dash="6 5")
    s.text((min(xs) + max(xs)) / 2, pos["D"][1] + 146, "поддърво с корен D", size=14, fill=S4, anchor="middle",
           weight=600)
    draw_nodes(s, pos, edges,
               fill={v: (TINT3 if v in leaves else TINT1) for v in pos} | {"A": TINT2},
               stroke={v: (S3 if v in leaves else S1) for v in pos} | {"A": S2},
               edge_style={e: (S2, 3, None) for e in path})
    for d in range(4):
        s.text(60, 110 + d * 90 + 5, f"дълбочина {d}", size=14, fill=MUTED)
    s.text(pos["A"][0] + 30, pos["A"][1] - 18, "корен (root)", size=14, fill=S2, weight=600)
    s.text(pos["K"][0] - 10, pos["K"][1] + 44, "листа (leaves) — зелени", size=14, fill=S3, weight=600)
    x = 780
    rows = [("родител на F:", "B"), ("деца на B:", "E, F (братя — siblings)"), ("предци на K:", "F, B, A"),
            ("степен на D:", "3 деца"), ("път A → K:", "3 ребра = дълбочина на K"),
            ("височина:", "3 = най-голямата дълбочина"), ("n възела:", "n − 1 ребра (тук 12 и 11)")]
    for k, (a, b) in enumerate(rows):
        s.text(x, 112 + k * 40, a, size=14, fill=MUTED, weight=600)
        s.text(x + 120, 112 + k * 40, b, size=14)
    s.save(OUT / "tree-terms.svg")


def tree_shapes():
    s = Svg(1100, 400, "Видове двоични дървета")
    s.text(40, 46, "Видове двоични дървета", size=24, weight=700)
    panels = [
        ("перфектно (perfect)", bst_from([4, 2, 6, 1, 3, 5, 7]), "всички нива са пълни", "n = 2^(h+1) − 1"),
        ("почти пълно (complete)", bst_from([4, 2, 6, 1, 3, 5]), "пълно без последното ниво,", "то — отляво надясно"),
        ("строго двоично (full)", bst_from([2, 1, 5, 4, 6]), "всеки възел има 0 или 2 деца", "листа = вътрешни + 1"),
        ("изродено (degenerate)", bst_from([1, 2, 3, 4]), "по едно дете навсякъде", "h = n − 1: това е списък"),
    ]
    for k, (title, tree, l1, l2) in enumerate(panels):
        x = 40 + k * 265
        s.text(x + 115, 96, title, size=16, anchor="middle", weight=700)
        n = len(layout_bin(tree, 0, 0, 1, 1)[0])
        dx = 30 if k < 3 else 40
        draw_bin(s, tree, x + 115 - (n - 1) * dx / 2, 140, dx, 56, r=15, size=13)
        s.text(x + 115, 352, l1, size=13, anchor="middle", fill=INK2)
        s.text(x + 115, 374, l2, size=13, anchor="middle", fill=INK2, family=MONO if "=" in l2 else None)
    s.save(OUT / "tree-shapes.svg")


def tree_repr():
    s = Svg(1100, 470, "Три представяния на дърво")
    s.text(40, 46, "Представяне в паметта", size=24, weight=700)
    # (a) linked nodes
    s.text(40, 92, "① свързани възли", size=16, weight=700)
    s.text(40, 114, "Node{value, left, right, parent}", size=13, family=MONO, fill=INK2)

    def record(x, y, v):
        for i, (lab, w) in enumerate([("•", 30), (str(v), 40), ("•", 30)]):
            xx = x + (0 if i == 0 else 30 if i == 1 else 70)
            s.rect(xx, y, w, 34, TINT1 if i == 1 else PANEL, S1 if i == 1 else GRID, rx=4)
            s.text(xx + w / 2, y + 23, lab, size=15, anchor="middle", family=MONO, weight=700 if i == 1 else 400)
    record(120, 150, 4)
    record(40, 260, 2)
    record(200, 260, 6)
    s.line(135, 184, 95, 258, stroke=INK2, arrow=True)
    s.line(205, 184, 245, 258, stroke=INK2, arrow=True)
    s.path("M 75,260 C 70,220 110,200 140,186", S2, sw=1.3, dash="4 4", arrow=True)
    s.path("M 265,260 C 270,220 230,200 200,186", S2, sw=1.3, dash="4 4", arrow=True)
    s.text(40, 330, "плътно: left / right", size=13, fill=INK2)
    s.text(40, 350, "пунктир: parent (по избор)", size=13, fill=S2)
    s.text(40, 384, "+ всякаква форма", size=13, fill=INK2)
    s.text(40, 404, "− 2–3 указателя на възел,", size=13, fill=INK2)
    s.text(40, 424, "  възлите са пръснати в паметта", size=13, fill=INK2)
    # (b) array
    s.text(390, 92, "② масив (за почти пълни дървета)", size=16, weight=700)
    tree = bst_from([4, 2, 6, 1, 3, 5])
    order = [4, 2, 6, 1, 3, 5]
    pos, edges = layout_bin(tree, 450, 150, 40, 56)
    draw_nodes(s, pos, edges, r=15, size=13)
    for i, v in enumerate(order):
        x, y = pos[v]
        s.text(x, y - 20, f"[{i}]", size=11, anchor="middle", fill=MUTED, family=MONO)
    for i, v in enumerate(order):
        x = 400 + i * 50
        s.rect(x, 300, 46, 36, TINT1, S1, rx=4)
        s.text(x + 23, 324, str(v), size=15, anchor="middle", family=MONO, weight=700)
        s.text(x + 23, 352, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)
    s.text(390, 384, "деца на i: 2i + 1 и 2i + 2", size=14, family=MONO)
    s.text(390, 406, "родител на i: (i − 1) / 2", size=14, family=MONO)
    s.text(390, 432, "без указатели; двоичната пирамида (седм. 11)", size=13, fill=INK2)
    # (c) first child / next sibling
    s.text(770, 92, "③ първо дете / следващ брат", size=16, weight=700)
    s.text(770, 114, "произволно дърво → двоично", size=13, fill=INK2)
    P = {"A": (900, 160), "B": (820, 240), "C": (900, 240), "D": (980, 240), "E": (790, 320), "F": (850, 320)}
    for (a, b) in [("A", "B"), ("B", "E")]:
        (x1, y1), (x2, y2) = P[a], P[b]
        s.line(x1 - 10, y1 + 16, x2 + 6, y2 - 17, stroke=S1, sw=2, arrow=True)
    for (a, b) in [("B", "C"), ("C", "D"), ("E", "F")]:
        (x1, y1), (x2, y2) = P[a], P[b]
        s.line(x1 + 17, y1, x2 - 19, y2, stroke=S2, sw=2, arrow=True)
    for (a, b) in [("A", "C"), ("A", "D")]:
        (x1, y1), (x2, y2) = P[a], P[b]
        s.line(x1, y1 + 17, x2, y2 - 17, stroke=GRID, sw=1.3, dash="4 4")
    for v, (x, y) in P.items():
        s.circle(x, y, 16, TINT1, stroke=S1, sw=1.6)
        s.text(x, y + 5, v, size=14, anchor="middle", family=MONO, weight=700)
    s.line(770, 372, 800, 372, stroke=S1, sw=2)
    s.text(808, 377, "left = първо дете", size=13)
    s.line(770, 398, 800, 398, stroke=S2, sw=2)
    s.text(808, 403, "right = следващ брат", size=13)
    s.text(770, 432, "сиво: истинските ребра", size=13, fill=MUTED)
    s.save(OUT / "tree-repr.svg")


# ---------------------------------------------------------------- traversals

def traversals():
    s = Svg(1100, 420, "Четирите обхождания на двоично дърво")
    s.text(40, 46, "Обхождания: обиколете дървото отвън, обратно на часовника", size=24, weight=700)
    tree = bst_from([4, 2, 6, 1, 3, 7])
    pos, edges = layout_bin(tree, 110, 120, 80, 100)
    D = 34
    pts = []

    def tour(t):
        if t is None:
            return
        v, l, r = t
        x, y = pos[v]
        pts.append((x - D, y))
        if l is None:
            pts.append((x - D * 0.75, y + D * 0.75))
        tour(l)
        pts.append((x, y + D))
        tour(r)
        if r is None:
            pts.append((x + D * 0.75, y + D * 0.75))
        pts.append((x + D, y))
    tour(tree)
    s.path(polyline(pts), MUTED, sw=1.5, dash="5 5")
    draw_nodes(s, pos, edges)
    for v, (x, y) in pos.items():
        s.circle(x - D, y, 6, S1, stroke=SURFACE)
        s.circle(x, y + D, 6, S3, stroke=SURFACE)
        s.circle(x + D, y, 6, S2, stroke=SURFACE)
    rows = [(S1, "pre-order", "отляво", "4 2 1 3 6 7", "копиране, сериализация"),
            (S3, "in-order", "отдолу", "1 2 3 4 6 7", "BST → сортиран ред"),
            (S2, "post-order", "отдясно", "1 3 2 7 6 4", "изтриване, размер, RPN"),
            (INK2, "level-order", "по нива", "4 2 6 1 3 7", "опашка (BFS)")]
    for k, (col, name, when, seq, use) in enumerate(rows):
        y = 120 + k * 66
        if k < 3:
            s.circle(620, y - 5, 7, col, stroke=SURFACE)
        else:
            s.rect(613, y - 12, 14, 14, PANEL, INK2, rx=2)
        s.text(640, y, name, size=16, weight=700)
        s.text(760, y, f"({when})", size=14, fill=INK2)
        s.text(640, y + 26, seq, size=17, family=MONO, weight=600)
        s.text(800, y + 26, use, size=14, fill=INK2)
    s.save(OUT / "traversals.svg")


# ---------------------------------------------------------------- BST

def insert_order():
    s = Svg(1100, 440, "Един и същи ключове, различен ред на вмъкване")
    s.text(40, 46, "Едни и същи ключове 1–7, различен ред на вмъкване", size=24, weight=700)
    panels = [("4 2 6 1 3 5 7", [4, 2, 6, 1, 3, 5, 7]), ("3 6 1 7 2 5 4", [3, 6, 1, 7, 2, 5, 4]),
              ("1 2 3 4 5 6 7", [1, 2, 3, 4, 5, 6, 7])]
    for k, (lab, keys) in enumerate(panels):
        x = 40 + k * 355
        tree = bst_from(keys)
        h = height(tree)
        s.text(x + 150, 92, lab, size=16, anchor="middle", family=MONO, weight=700)
        dy = 46 if h < 4 else 40
        draw_bin(s, tree, x + 150 - 3 * 38, 130, 38, dy, r=15, size=13)
        col = RED if h == 6 else (GREEN if h == 2 else INK2)
        s.text(x + 150, 418, f"височина {h}", size=15, anchor="middle", weight=700, fill=col)
    s.save(OUT / "insert-order.svg")


def bst_search():
    s = Svg(1100, 440, "Търсене в двоично наредено дърво")
    s.text(40, 46, "Търсене: на всяка стъпка — наляво или надясно", size=24, weight=700)
    found = [50, 30, 40, 45]
    miss = [50, 70, 60]
    pos, edges = layout_bin(SAMPLE, 70, 110, 62, 84)
    style = {}
    for a, b in zip(found, found[1:]):
        style[(a, b)] = (S2, 3.5, None)
    for a, b in zip(miss, miss[1:]):
        style[(a, b)] = (S5, 3.5, "6 5")
    fill = {45: TINT2, 60: TINT5, 50: TINT2}
    stroke = {45: S2, 60: S5, 50: S2}
    draw_nodes(s, pos, edges, fill=fill, stroke=stroke, edge_style=style)
    x, y = pos[60]
    s.line(x + 12, y + 16, x + 28, y + 46, stroke=S5, sw=2, dash="4 4")
    s.text(x + 30, y + 64, "nullptr", size=13, anchor="middle", family=MONO, fill=S5)
    s.text(720, 110, "find(45)", size=17, family=MONO, weight=700, fill=S2)
    for k, t in enumerate(["45 < 50 → наляво", "45 > 30 → надясно", "45 > 40 → надясно", "45 = 45 → намерен"]):
        s.text(720, 138 + k * 24, t, size=14, family=MONO)
    s.text(720, 262, "find(55)", size=17, family=MONO, weight=700, fill=S5)
    for k, t in enumerate(["55 > 50 → надясно", "55 < 70 → наляво", "55 < 60 → наляво: nullptr"]):
        s.text(720, 290 + k * 24, t, size=14, family=MONO)
    s.text(720, 380, "по пътя: floor(55) = 50,", size=14, fill=INK2)
    s.text(720, 402, "ceiling(55) = 60", size=14, fill=INK2)
    s.text(720, 428, "Θ(h) сравнения: h = височината", size=14, weight=600)
    s.save(OUT / "bst-search.svg")


def bst_trap():
    s = Svg(1000, 400, "Проверка дали дърво е BST: капанът")
    s.text(40, 46, "isBST: сравнението само с децата не стига", size=24, weight=700)
    tree = (5, (3, (1, None, None), (6, None, None)), (8, None, None))
    pos, edges = layout_bin(tree, 110, 120, 100, 100)
    draw_nodes(s, pos, edges, fill={6: TINT_RED}, stroke={6: RED})
    ivals = {5: "(−∞, +∞)", 3: "(−∞, 5)", 8: "(5, +∞)", 1: "(−∞, 3)", 6: "(3, 5) ✗"}
    for v, t in ivals.items():
        x, y = pos[v]
        s.text(x, y + 44, t, size=14, anchor="middle", family=MONO, fill=RED if v == 6 else INK2,
               weight=700 if v == 6 else 400)
    s.text(560, 120, "Локално: всеки възел спрямо децата си", size=15, weight=700)
    s.text(560, 146, "при 5: 3 < 5 < 8 ✓", size=14, family=MONO)
    s.text(560, 168, "при 3: 1 < 3 < 6 ✓", size=14, family=MONO)
    s.text(560, 194, "→ „валидно“ — но не е!", size=14, fill=RED, weight=600)
    s.text(560, 236, "6 е в ЛЯВОТО поддърво на 5,", size=15)
    s.text(560, 260, "значи трябва да е < 5.", size=15)
    s.text(560, 296, "Правилно: пренасяйте интервала", size=15, weight=700)
    s.text(560, 320, "(lo, hi) надолу. Наляво: hi = възела;", size=14, fill=INK2)
    s.text(560, 342, "надясно: lo = възела.", size=14, fill=INK2)
    s.save(OUT / "bst-trap.svg")


def erase_frames():
    full = SAMPLE

    def remove_leaf(t, k):
        if t is None:
            return None
        v, l, r = t
        if v == k:
            return None
        return (v, remove_leaf(l, k), remove_leaf(r, k))

    after1 = remove_leaf(full, 35)
    after2 = (50, full[1], (70, (60, None, None), (90, None, None)))
    after3 = (50, (35, (20, None, None), (40, None, (45, None, None))), full[2])
    frames = [
        ("erase-1.svg", "Случай 1: лист — просто го откачаме", "erase(35)", 35, after1, {}, []),
        ("erase-2.svg", "Случай 2: едно дете — то заема мястото", "erase(80)", 80, after2, {90: "S3"},
         ["детето 90 се качва с цялото си поддърво"]),
        ("erase-3.svg", "Случай 3: две деца — наследникът заема мястото", "erase(30)", 30, after3, {35: "S3"},
         ["наследник = най-левият в дясното поддърво (35)", "той няма ляво дете → махаме го по случай 1 или 2",
          "и го поставяме на мястото на 30"]),
    ]
    for name, title, call, victim, after, moved, notes in frames:
        s = Svg(1100, 470, title)
        s.text(40, 46, title, size=24, weight=700)
        s.text(40, 80, call, size=17, family=MONO, fill=RED, weight=700)
        fill = {victim: TINT_RED}
        stroke = {victim: RED}
        for v in moved:
            fill[v] = TINT3
            stroke[v] = S3
        draw_bin(s, full, 50, 130, 47, 72, fill=fill, stroke=stroke, r=18, size=14)
        s.line(530, 230, 590, 230, stroke=INK2, sw=2, arrow=True)
        fill2 = {v: TINT3 for v in moved}
        stroke2 = {v: S3 for v in moved}
        draw_bin(s, after, 620, 130, 47, 72, fill=fill2, stroke=stroke2, r=18, size=14)
        for k, t in enumerate(notes):
            s.text(40, 404 + k * 22, t, size=14, fill=INK2)
        s.save(OUT / name)


def successor():
    s = Svg(1100, 450, "Следващият по големина: двата случая")
    s.text(40, 46, "++it: следващият по големина (in-order successor)", size=24, weight=700)
    pos, edges = layout_bin(SAMPLE, 70, 110, 62, 84)
    style = {(30, 40): (S1, 3.5, None), (40, 35): (S1, 3.5, None)}
    draw_nodes(s, pos, edges, fill={30: TINT1, 35: TINT1, 45: TINT2, 50: TINT2},
               stroke={30: S1, 35: S1, 45: S2, 50: S2}, edge_style=style)
    # climbing arrows for 45 -> 40 -> 30 -> 50, drawn beside the edges
    chain = [45, 40, 30, 50]
    for a, b in zip(chain, chain[1:]):
        (x1, y1), (x2, y2) = pos[a], pos[b]
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        nx, ny = uy, -ux  # normal pointing to the upper-left of an up-right edge
        if ux < 0:
            nx, ny = -nx, -ny
        o = 13
        s.line(x1 + ux * (R + 4) + nx * o, y1 + uy * (R + 4) + ny * o,
               x2 - ux * (R + 6) + nx * o, y2 - uy * (R + 6) + ny * o, stroke=S2, sw=2.2, arrow=True, dash="6 4")
    s.text(720, 110, "① има дясно поддърво:", size=16, weight=700, fill=S1)
    s.text(720, 136, "най-левият възел в него", size=15)
    s.text(720, 162, "30 → 40 → 35", size=15, family=MONO)
    s.text(720, 222, "② няма дясно поддърво:", size=16, weight=700, fill=S2)
    s.text(720, 248, "качвай се, докато идваш отдясно;", size=15)
    s.text(720, 272, "първият родител, до когото", size=15)
    s.text(720, 296, "стигнеш отляво, е следващият", size=15)
    s.text(720, 322, "45 → 40 → 30 → 50", size=15, family=MONO)
    s.text(720, 372, "Едно ++ е O(h), но цялото обхождане", size=14, fill=INK2)
    s.text(720, 394, "минава всяко ребро 2 пъти: Θ(n)", size=14, fill=INK2)
    s.text(720, 416, "→ амортизирано Θ(1) на ++", size=14, fill=INK2, weight=600)
    s.save(OUT / "successor.svg")


# ---------------------------------------------------------------- charts

def simulate_heights():
    rng = random.Random(9)
    out = []
    for k in range(4, 19, 2):
        n = 2 ** k
        trials = 20 if n <= 4096 else (5 if n <= 65536 else 2)
        hs, ds = [], []
        for _ in range(trials):
            keys = list(range(n))
            rng.shuffle(keys)
            left = [-1] * n
            right = [-1] * n
            root = keys[0]
            hmax, dsum = 0, 0
            for key in keys[1:]:
                cur, d = root, 0
                while True:
                    d += 1
                    if key < cur:
                        if left[cur] < 0:
                            left[cur] = key
                            break
                        cur = left[cur]
                    else:
                        if right[cur] < 0:
                            right[cur] = key
                            break
                        cur = right[cur]
                hmax = max(hmax, d)
                dsum += d
            hs.append(hmax)
            ds.append(dsum / n)
        out.append((n, sum(hs) / len(hs), sum(ds) / len(ds)))
    return out


def height_chart():
    data = simulate_heights()
    s = Svg(1100, 500, "Височина на двоично наредено дърво при случаен ред на вмъкване")
    s.text(40, 44, "Височина при случаен ред на вмъкване", size=24, weight=700)
    s.text(40, 70, "Симулация: случайни пермутации на 0 … n−1, средно от няколко опита", size=15, fill=INK2)
    x0, y0, w, h = 100, 100, 560, 320
    fx = lambda n: x0 + (math.log2(n) - 4) / 14 * w
    fy = lambda v: y0 + h - v / 50 * h
    axes(s, x0, y0, w, h, [2 ** k for k in range(4, 19, 2)], [0, 10, 20, 30, 40, 50], "n (лог. ос)",
         "нива", fx, fy, xfmt=lambda n: f"2^{int(math.log2(n))}", yfmt=str)
    series = [(RED, "височина (случаен ред)", 1), (S1, "средна дълбочина (случаен ред)", 2)]
    for col, _, idx in series:
        pts = [(fx(r[0]), fy(r[idx])) for r in data]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    pts = [(fx(2 ** k), fy(k)) for k in range(4, 19)]
    s.path(polyline(pts), S3, sw=2, dash="6 5")
    legend(s, 700, 130, [(RED, "височина, случаен ред"), (S1, "средна дълбочина, случаен ред"),
                         (S3, "log₂ n — най-ниското възможно")])
    last = data[-1]
    s.text(700, 236, f"При n = 2^18 ≈ 262 000:", size=14, weight=600)
    s.text(700, 258, f"височина ≈ {last[1]:.0f}, средна дълбочина ≈ {last[2]:.0f}", size=14, fill=INK2)
    s.text(700, 280, "(log₂ n = 18)", size=14, fill=INK2)
    s.text(700, 316, "Теория: средна дълбочина ≈ 2 ln n ≈ 1.39 log₂ n,", size=14, fill=INK2)
    s.text(700, 338, "височина → 4.31 ln n асимптотично (Devroye, 1986)", size=14, fill=INK2)
    s.text(700, 382, "Сортиран вход: височина n − 1", size=14, weight=700, fill=RED)
    s.text(700, 404, "(262 143 — далеч извън графиката)", size=14, fill=INK2)
    s.save(OUT / "bst-height.svg")
    return data


def bench_chart():
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    s = Svg(1100, 500, "Време на операция: BST и std::set, случаен и сортиран вход")
    s.text(40, 44, "ns на операция, int ключове", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина; вмъкване + търсене на присъстващи + търсене на липсващи", size=15,
           fill=INK2)
    x0, y0, w, h = 100, 100, 560, 320
    fx = lambda n: x0 + (math.log10(n) - 3) / 3 * w
    fy = lambda v: y0 + h - (math.log10(v) - 1) / 4 * h
    axes(s, x0, y0, w, h, [1000, 10000, 100000, 1000000], [10, 100, 1000, 10000, 100000], "n (лог. ос)",
         "ns (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    series = [(RED, "вашето BST, сортиран вход", "bst_sorted"), (S1, "вашето BST, случаен вход", "bst_random"),
              (S4, "std::set, случаен вход", "set_random"), (S3, "std::set, сортиран вход", "set_sorted")]
    for col, _, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 700, 130, [(c, l) for c, l, _ in series])
    r20 = next(r for r in rows if r["n"] == "20000")
    ratio = float(r20["bst_sorted"]) / float(r20["set_sorted"])
    s.text(700, 254, "Сортиран вход: дървото е списък,", size=14, fill=INK2)
    s.text(700, 274, f"при n = 20 000 — ~{ratio:.0f}× по-бавно", size=14, fill=INK2)
    s.text(700, 294, "от std::set (спряно след 20 000).", size=14, fill=INK2)
    s.text(700, 330, "Случаен вход: височина ~2.5 log₂ n —", size=14, fill=INK2)
    s.text(700, 350, "почти наравно със std::set (червено-", size=14, fill=INK2)
    s.text(700, 370, "черно дърво, гарантирано балансирано).", size=14, fill=INK2)
    s.save(OUT / "bst-bench.svg")


if __name__ == "__main__":
    tree_terms()
    tree_shapes()
    tree_repr()
    traversals()
    insert_order()
    bst_search()
    bst_trap()
    erase_frames()
    successor()
    for n, h, d in height_chart():
        print(f"n = {n:7d}: height {h:5.1f}, average depth {d:5.1f}, log2 n = {math.log2(n):4.1f}")
    bench_chart()
