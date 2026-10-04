#!/usr/bin/env python3
"""Generates every SVG figure for week 10.

Run from anywhere:  python3 weeks/10-trees-2/img/src/make_figures.py
The timing chart reads bench_sample.csv (`w10_bench --csv`, release preset).
Tree shapes come from a small AVL implementation below - the same
algorithm as solutions/avl_tree.h, so the figures match what the tests expect.
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
R = 18
BLACK_NODE = "#2b2b2b"


# ---------------------------------------------------------------- AVL model

class AvlNode:
    def __init__(self, v):
        self.v, self.l, self.r, self.h = v, None, None, 0


def h(n):
    return n.h if n else -1


def upd(n):
    n.h = 1 + max(h(n.l), h(n.r))


def bf(n):
    return h(n.l) - h(n.r)


LOG = []


def rot_right(y):
    x = y.l
    y.l, x.r = x.r, y
    upd(y)
    upd(x)
    LOG.append(("rotateRight", y.v))
    return x


def rot_left(x):
    y = x.r
    x.r, y.l = y.l, x
    upd(x)
    upd(y)
    LOG.append(("rotateLeft", x.v))
    return y


def rebalance(n):
    upd(n)
    if bf(n) > 1:
        if bf(n.l) < 0:
            n.l = rot_left(n.l)
        return rot_right(n)
    if bf(n) < -1:
        if bf(n.r) > 0:
            n.r = rot_right(n.r)
        return rot_left(n)
    return n


def avl_insert(n, x):
    if n is None:
        return AvlNode(x)
    if x < n.v:
        n.l = avl_insert(n.l, x)
    elif x > n.v:
        n.r = avl_insert(n.r, x)
    else:
        return n
    return rebalance(n)


def extract_min(n):
    if n.l is None:
        return n.r, n
    n.l, m = extract_min(n.l)
    return rebalance(n), m


def avl_erase(n, x):
    if n is None:
        return None
    if x < n.v:
        n.l = avl_erase(n.l, x)
    elif x > n.v:
        n.r = avl_erase(n.r, x)
    else:
        if n.l is None:
            return n.r
        if n.r is None:
            return n.l
        rest, s = extract_min(n.r)
        s.l, s.r = n.l, rest
        return rebalance(s)
    return rebalance(n)


def as_tuple(n):
    return None if n is None else (n.v, as_tuple(n.l), as_tuple(n.r))


def from_tuple(t):
    if t is None:
        return None
    n = AvlNode(t[0])
    n.l, n.r = from_tuple(t[1]), from_tuple(t[2])
    upd(n)
    return n


def tuple_height(t):
    return -1 if t is None else 1 + max(tuple_height(t[1]), tuple_height(t[2]))


def tuple_size(t):
    return 0 if t is None else 1 + tuple_size(t[1]) + tuple_size(t[2])


# ---------------------------------------------------------------- drawing

def layout_bin(tree, x0, y0, dx, dy):
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


def draw_nodes(s, pos, edges, fill=None, stroke=None, text_fill=None, edge_style=None, r=R, size=14):
    fill, stroke, text_fill, edge_style = fill or {}, stroke or {}, text_fill or {}, edge_style or {}
    for a, b in edges:
        (x1, y1), (x2, y2) = pos[a], pos[b]
        col, sw, dash = edge_style.get((a, b), (INK2, 1.5, None))
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        s.line(x1 + ux * r, y1 + uy * r, x2 - ux * r, y2 - uy * r, stroke=col, sw=sw, dash=dash)
    for v, (x, y) in pos.items():
        s.circle(x, y, r, fill.get(v, TINT1), stroke=stroke.get(v, S1), sw=1.6)
        s.text(x, y + size * 0.36, str(v), size=size, anchor="middle", family=MONO, weight=700,
               fill=text_fill.get(v, INK))


def draw_tree(s, tree, cx, y0, dx, dy, **kw):
    """Centered horizontally on cx."""
    n = tuple_size(tree)
    pos, edges = layout_bin(tree, cx - (n - 1) * dx / 2, y0, dx, dy)
    draw_nodes(s, pos, edges, **kw)
    return pos


def triangle(s, x, y, label, hgt=56, w=56, fill=PANEL, stroke=MUTED):
    s.path(f"M {x},{y} L {x - w / 2},{y + hgt} L {x + w / 2},{y + hgt} Z", stroke, sw=1.4, fill=fill)
    s.text(x, y + hgt * 0.72, label, size=15, anchor="middle", weight=700)


def node(s, x, y, label, fill=TINT1, stroke=S1, r=20):
    s.circle(x, y, r, fill, stroke=stroke, sw=1.6)
    s.text(x, y + 6, label, size=16, anchor="middle", family=MONO, weight=700)


def edge(s, x1, y1, x2, y2, r=20, col=INK2, sw=1.5):
    L = math.hypot(x2 - x1, y2 - y1)
    ux, uy = (x2 - x1) / L, (y2 - y1) / L
    s.line(x1 + ux * r, y1 + uy * r, x2 - ux * (r if y2 - y1 > 40 or abs(x2 - x1) > 40 else 0), y2 - uy * 2,
           stroke=col, sw=sw)


def link(s, a, b, col=INK2, sw=1.5):
    """Edge from node center a to the apex of a triangle (or a node) b."""
    (x1, y1), (x2, y2) = a, b
    L = math.hypot(x2 - x1, y2 - y1)
    ux, uy = (x2 - x1) / L, (y2 - y1) / L
    s.line(x1 + ux * 20, y1 + uy * 20, x2, y2, stroke=col, sw=sw)


# ---------------------------------------------------------------- rotations

def rotation():
    s = Svg(1100, 430, "Ротация надясно и наляво")
    s.text(40, 46, "Ротация: два указателя, наредбата се запазва", size=24, weight=700)

    def left_shape(ox):
        y, x = (ox + 200, 120), (ox + 120, 210)
        link(s, y, x)
        link(s, x, (ox + 70, 284))
        link(s, x, (ox + 170, 284), col=S2, sw=2.5)
        link(s, y, (ox + 270, 214))
        triangle(s, ox + 70, 284, "A")
        triangle(s, ox + 170, 284, "B", fill=TINT2, stroke=S2)
        triangle(s, ox + 270, 214, "C")
        node(s, *y, "y")
        node(s, *x, "x", fill=TINT3, stroke=S3)

    def right_shape(ox):
        x, y = (ox + 120, 120), (ox + 200, 210)
        link(s, x, y)
        link(s, x, (ox + 50, 214))
        link(s, y, (ox + 150, 284), col=S2, sw=2.5)
        link(s, y, (ox + 250, 284))
        triangle(s, ox + 50, 214, "A")
        triangle(s, ox + 150, 284, "B", fill=TINT2, stroke=S2)
        triangle(s, ox + 250, 284, "C")
        node(s, *x, "x", fill=TINT3, stroke=S3)
        node(s, *y, "y")

    left_shape(60)
    right_shape(700)
    s.line(450, 190, 640, 190, stroke=INK2, sw=2, arrow=True)
    s.text(545, 178, "rotateRight(y)", size=15, anchor="middle", family=MONO, weight=600)
    s.line(640, 236, 450, 236, stroke=INK2, sw=2, arrow=True)
    s.text(545, 262, "rotateLeft(x)", size=15, anchor="middle", family=MONO, weight=600)
    s.text(550, 380, "in-order и в двете: A  x  B  y  C", size=17, anchor="middle", family=MONO, weight=700)
    s.text(550, 408, "мести се само B (оранжево): от дясно дете на x става ляво дете на y; Θ(1)", size=14,
           anchor="middle", fill=INK2)
    s.save(OUT / "rotation.svg")


def avl_cases():
    s = Svg(1100, 490, "Четирите случая на дисбаланс в AVL")
    s.text(40, 46, "Четирите случая: височините се различават с 2", size=24, weight=700)
    cases = [
        ("LL", (3, (2, (1, None, None), None), None), "rotateRight(3)"),
        ("RR", (1, None, (2, None, (3, None, None))), "rotateLeft(1)"),
        ("LR", (3, (1, None, (2, None, None)), None), "rotateLeft(1),|rotateRight(3)"),
        ("RL", (1, None, (3, (2, None, None), None)), "rotateRight(3),|rotateLeft(1)"),
    ]
    for k, (name, before, how) in enumerate(cases):
        cx = 150 + k * 270
        s.text(cx, 96, name, size=20, anchor="middle", weight=700, fill=S2 if k < 2 else S5)
        pos, edges = layout_bin(before, cx - 50, 130, 50, 56)
        draw_nodes(s, pos, edges, fill={before[0]: TINT_RED}, stroke={before[0]: RED})
        rx, ry = pos[before[0]]
        s.text(rx + 26, ry - 14, "±2", size=13, fill=RED, weight=700)
        for j, line in enumerate(how.split("|")):
            s.text(cx, 278 + j * 20, line, size=13, anchor="middle", family=MONO)
        s.line(cx, 316, cx, 344, stroke=INK2, sw=1.6, arrow=True)
        draw_tree(s, (2, (1, None, None), (3, None, None)), cx, 370, 50, 56, fill={2: TINT3}, stroke={2: S3})
    s.text(550, 476, "LL / RR: единична ротация. LR / RL: двойна — първо детето, после възела.", size=14,
           anchor="middle", fill=INK2)
    s.save(OUT / "avl-cases.svg")


def lr_frames():
    titles = ["LR: височината идва от средата (y)", "Стъпка 1: rotateLeft(x) — сега е LL",
              "Стъпка 2: rotateRight(z) — балансирано"]
    for k in range(3):
        s = Svg(1100, 420, titles[k])
        s.text(40, 46, titles[k], size=24, weight=700)
        hi = dict(fill=TINT2, stroke=S2)
        if k == 0:
            z, x, yy = (560, 110), (420, 190), (520, 270)
            link(s, z, x)
            link(s, z, (700, 194))
            link(s, x, (340, 274))
            link(s, x, yy)
            link(s, yy, (470, 334))
            link(s, yy, (570, 334))
            triangle(s, 700, 194, "D", hgt=110)
            triangle(s, 340, 274, "A", hgt=110)
            triangle(s, 470, 334, "B", hgt=56, **hi)
            triangle(s, 570, 334, "C", hgt=56, **hi)
            node(s, *z, "z", fill=TINT_RED, stroke=RED)
            node(s, *x, "x")
            node(s, *yy, "y", fill=TINT3, stroke=S3)
            s.text(600, 100, "+2", size=15, fill=RED, weight=700)
            s.text(460, 182, "−1", size=15, fill=S1, weight=700)
        elif k == 1:
            z, yy, x = (560, 110), (440, 190), (360, 270)
            link(s, z, yy)
            link(s, z, (700, 194))
            link(s, yy, x)
            link(s, yy, (520, 274))
            link(s, x, (300, 334))
            link(s, x, (420, 334))
            triangle(s, 700, 194, "D", hgt=110)
            triangle(s, 300, 334, "A", hgt=56 + 0)
            triangle(s, 420, 334, "B", hgt=56, **hi)
            triangle(s, 520, 274, "C", hgt=56, **hi)
            node(s, *z, "z", fill=TINT_RED, stroke=RED)
            node(s, *yy, "y", fill=TINT3, stroke=S3)
            node(s, *x, "x")
        else:
            yy, x, z = (550, 110), (420, 200), (680, 200)
            link(s, yy, x)
            link(s, yy, z)
            link(s, x, (360, 264))
            link(s, x, (470, 264))
            link(s, z, (630, 264))
            link(s, z, (740, 264))
            triangle(s, 360, 264, "A", hgt=110)
            triangle(s, 470, 264, "B", hgt=56, **hi)
            triangle(s, 630, 264, "C", hgt=56, **hi)
            triangle(s, 740, 264, "D", hgt=110)
            node(s, *yy, "y", fill=TINT3, stroke=S3)
            node(s, *x, "x")
            node(s, *z, "z")
        notes = [
            ["z: +2 (лявото е с 2 по-високо)", "x: −1 (дясното му е по-високо)", "→ единична ротация надясно",
             "  НЕ стига: y би отишъл под z"],
            ["x и y сменят местата си", "in-order: A x B y C z D — същият"],
            ["y е новият корен", "A, B | C, D — по два от всяка страна", "височината е като преди вмъкването"],
        ][k]
        for j, t in enumerate(notes):
            s.text(800, 150 + j * 26, t, size=14, fill=INK2)
        s.save(OUT / f"lr-{k + 1}.svg")


def avl_sorted():
    s = Svg(1100, 520, "Вмъкване на 1…7 по ред в AVL дърво")
    s.text(40, 46, "Вмъкване на 1, 2, …, 7 по ред: ротация на стъпки 3, 5, 6, 7", size=24, weight=700)
    root = None
    for i in range(1, 8):
        LOG.clear()
        root = avl_insert(root, i)
        t = as_tuple(root)
        row, col = (0, i - 1) if i <= 4 else (1, i - 5)
        cx = 150 + col * 270 if row == 0 else 285 + col * 270
        y0 = 110 if row == 0 else 310
        rot = ", ".join(f"{a}({b})" for a, b in LOG)
        s.text(cx, y0 - 16, f"insert({i})", size=15, anchor="middle", family=MONO, weight=700)
        if rot:
            s.text(cx, y0 + 4, rot, size=13, anchor="middle", family=MONO, fill=RED)
        draw_tree(s, t, cx, y0 + 34, 30, 44, r=13, size=12, fill={i: TINT2}, stroke={i: S2})
    s.text(40, 506, "Сортираният вход — най-лошият за BST от миналата седмица — тук дава перфектно дърво.",
           size=14, fill=INK2)
    s.save(OUT / "avl-sorted.svg")


def fib_trees():
    s = Svg(1100, 420, "Най-тънките AVL дървета (дървета на Фибоначи)")
    s.text(40, 46, "Най-малко възли при височина h: N(h) = N(h−1) + N(h−2) + 1", size=24, weight=700)

    def fib(hh):
        if hh < 0:
            return None
        if hh == 0:
            return ("*", None, None)
        return ("*", fib(hh - 1), fib(hh - 2))

    def label(t, c):
        if t is None:
            return None
        l = label(t[1], c)
        c[0] += 1
        v = c[0]
        return (v, l, label(t[2], c))

    xs = [70, 150, 270, 450, 780]
    dxs = [0, 26, 26, 24, 24]
    for hh in range(5):
        t = label(fib(hh), [0])
        n = tuple_size(t)
        s.text(xs[hh], 96, f"h = {hh}", size=15, anchor="middle", weight=700)
        s.text(xs[hh], 118, f"N = {n}", size=14, anchor="middle", family=MONO, fill=S2)
        draw_tree(s, t, xs[hh], 150, dxs[hh], 50, r=10, size=0)
    s.text(40, 384, "1, 2, 4, 7, 12, 20, 33, … = F(h+3) − 1: числата на Фибоначи минус 1 → растат като 1.618^h",
           size=14, fill=INK2)
    s.text(40, 406, "Затова n ≥ 1.618^h, т.е. h ≤ 1.44 log₂ n: и най-тънкото AVL дърво е логаритмично", size=14,
           fill=INK2, weight=600)
    s.save(OUT / "fib-trees.svg")


def erase_cascade():
    def fib(hh):
        if hh < 0:
            return None
        if hh == 0:
            return ("*", None, None)
        return ("*", fib(hh - 1), fib(hh - 2))

    def label(t, c):
        if t is None:
            return None
        l = label(t[1], c)
        c[0] += 1
        v = c[0]
        return (v, l, label(t[2], c))

    before = label(fib(4), [0])
    root = from_tuple(before)
    LOG.clear()
    root = avl_erase(root, 12)
    after = as_tuple(root)
    rots = list(LOG)
    s = Svg(1100, 480, "Изтриването може да предизвика ротации по целия път")
    s.text(40, 46, "erase(12): две ротации — на две нива", size=24, weight=700)
    s.text(40, 76, "Най-тънкото AVL дърво с височина 4; изтриваме лист от по-късата страна", size=15, fill=INK2)
    rotated = {v for _, v in rots}
    draw_tree(s, before, 270, 130, 36, 62, r=15, size=12, fill={12: TINT_RED} | {v: TINT2 for v in rotated},
              stroke={12: RED} | {v: S2 for v in rotated})
    s.line(510, 240, 570, 240, stroke=INK2, sw=2, arrow=True)
    draw_tree(s, after, 820, 130, 36, 62, r=15, size=12, fill={v: TINT2 for v in rotated},
              stroke={v: S2 for v in rotated})
    for j, (a, b) in enumerate(rots):
        s.text(40, 430 + j * 22, f"{j + 1}. {a}({b})", size=14, family=MONO)
    s.text(330, 430, "Вмъкване: най-много ЕДНА (единична или двойна) ротация — височината се връща.", size=14,
           fill=INK2)
    s.text(330, 452, "Изтриване: до една на ниво — Θ(log n) ротации; поддървото може да остане по-ниско.",
           size=14, fill=INK2)
    print("erase(12) rotations:", rots, "height", tuple_height(before), "->", tuple_height(after))
    s.save(OUT / "erase-cascade.svg")


# ---------------------------------------------------------------- other trees

def rb_tree():
    s = Svg(1100, 520, "Червено-черно дърво и съответното 2-3-4 дърво")
    s.text(40, 46, "Червено-черно дърво = 2-3-4 дърво, записано като двоично", size=24, weight=700)
    t = (8, (4, (2, None, None), (6, None, None)),
         (16, (12, (10, None, None), (14, None, None)), (20, (18, None, None), None)))
    red = {12, 18}
    pos = draw_tree(s, t, 270, 120, 46, 76, fill={v: (TINT_RED if v in red else BLACK_NODE) for v in
                                                  [8, 4, 2, 6, 16, 12, 10, 14, 20, 18]},
                    stroke={v: (RED if v in red else BLACK_NODE) for v in [8, 4, 2, 6, 16, 12, 10, 14, 20, 18]},
                    text_fill={v: (INK if v in red else SURFACE) for v in [8, 4, 2, 6, 16, 12, 10, 14, 20, 18]})
    rules = ["① червен или черен; ② коренът е черен", "③ червеният няма червено дете",
             "④ еднакъв брой черни по всеки път", "→ най-дългият път ≤ 2 × най-късия",
             "→ h ≤ 2 log₂(n + 1)"]
    for k, line in enumerate(rules):
        s.text(40, 408 + k * 21, line, size=14, fill=INK2 if k < 3 else INK, weight=400 if k < 3 else 600)
    # 2-3-4 tree on the right
    def keybox(x, y, keys):
        w = 40 * len(keys)
        s.rect(x - w / 2, y - 18, w, 36, PANEL, INK2, rx=6)
        for i, k in enumerate(keys):
            xx = x - w / 2 + 20 + 40 * i
            s.text(xx, y + 6, str(k), size=14, anchor="middle", family=MONO, weight=700)
            if i:
                s.line(x - w / 2 + 40 * i, y - 18, x - w / 2 + 40 * i, y + 18, stroke=GRID, sw=1)
        return w
    s.text(830, 96, "2-3-4 дърво", size=16, anchor="middle", weight=700)
    keybox(830, 140, [8])
    keybox(700, 230, [4])
    keybox(930, 230, [12, 16])
    for x, keys in [(650, [2]), (750, [6]), (850, [10]), (930, [14]), (1030, [18, 20])]:
        keybox(x, 320, keys)
    for (x1, y1, x2, y2) in [(820, 158, 705, 212), (840, 158, 925, 212), (690, 248, 655, 302), (710, 248, 745, 302),
                             (900, 248, 855, 302), (930, 248, 930, 302), (960, 248, 1020, 302)]:
        s.line(x1, y1, x2, y2, stroke=INK2, sw=1.4)
    s.text(830, 408, "червеният възел е „в един възел“", size=14, anchor="middle", fill=INK2)
    s.text(830, 429, "с черния си родител: 12|16, 18|20", size=14, anchor="middle", fill=INK2)
    s.text(830, 460, "всички листа — на една дълбочина", size=14, anchor="middle", fill=INK2, weight=600)
    s.save(OUT / "rb-tree.svg")


def btree():
    s = Svg(1100, 400, "B-дърво: много ключове във възел")
    s.text(40, 46, "B-дърво: един възел = един блок памет, много ключове", size=24, weight=700)

    def keybox(x, y, keys, fill=PANEL):
        w = 44 * len(keys)
        s.rect(x - w / 2, y - 20, w, 40, fill, INK2, rx=6)
        for i, k in enumerate(keys):
            s.text(x - w / 2 + 22 + 44 * i, y + 6, str(k), size=15, anchor="middle", family=MONO, weight=700)
            if i:
                s.line(x - w / 2 + 44 * i, y - 20, x - w / 2 + 44 * i, y + 20, stroke=GRID, sw=1)
        return x - w / 2, w

    left, w = keybox(550, 120, [30, 60], fill=TINT1)
    kids = [(200, [5, 10, 20]), (550, [35, 40, 50]), (900, [70, 80, 90, 95])]
    for i, (x, keys) in enumerate(kids):
        keybox(x, 250, keys)
        s.line(left + 44 * i, 140, x, 230, stroke=INK2, sw=1.4)
    lines = ["Възел с k ключа има k + 1 деца; всички листа са на една дълбочина.",
             "Диск (блок 4–16 KB): стотици ключове във възел → при ~500 ключа 4 нива стигат за над 60 милиарда ключа.",
             "Бази данни (B+ дървета в PostgreSQL, MySQL/InnoDB, SQLite), файлови системи (NTFS, Btrfs, APFS)."]
    for k, t in enumerate(lines):
        s.text(40, 320 + k * 24, t, size=14, fill=INK2)
    s.save(OUT / "btree.svg")


def order_stat():
    root = None
    for v in [50, 10, 90, 30, 70, 20, 40, 60, 80, 100]:
        root = avl_insert(root, v)
    t = as_tuple(root)
    s = Svg(1100, 440, "Разширено дърво: размер на поддървото")
    s.text(40, 46, "Всеки възел помни размера на поддървото си → select и rank за Θ(log n)", size=24, weight=700)
    pos, edges = layout_bin(t, 80, 120, 60, 84)

    sizes = {}

    def walk(n):
        if n is None:
            return 0
        sz = 1 + walk(n[1]) + walk(n[2])
        sizes[n[0]] = sz
        return sz
    walk(t)
    # select(6): path
    path = []
    cur, k = t, 6
    while cur:
        left = sizes[cur[1][0]] if cur[1] else 0
        path.append((cur[0], k, left))
        if k < left:
            cur = cur[1]
        elif k == left:
            break
        else:
            k -= left + 1
            cur = cur[2]
    on = {p[0] for p in path}
    style = {}
    for a, b in zip([p[0] for p in path], [p[0] for p in path][1:]):
        style[(a, b)] = (S2, 3.5, None)
    draw_nodes(s, pos, edges, fill={v: TINT2 for v in on}, stroke={v: S2 for v in on}, edge_style=style, r=20)
    side = {b: ("left" if pos[b][0] < pos[a][0] else "right") for a, b in edges}
    for v, (x, y) in pos.items():
        if side.get(v) == "left":
            s.text(x - 24, y - 16, f"size {sizes[v]}", size=12, anchor="end", fill=MUTED, family=MONO)
        else:
            s.text(x + 24, y - 16, f"size {sizes[v]}", size=12, anchor="start", fill=MUTED, family=MONO)
    s.text(720, 120, "select(6) — 7-мият най-малък:", size=16, weight=700, fill=S2)
    for j, (v, kk, left) in enumerate(path):
        if kk < left:
            what = f"k={kk} < {left} → наляво"
        elif kk == left:
            what = f"k={kk} = {left} → намерен: {v}"
        else:
            what = f"k={kk} > {left} → надясно, k = {kk} − {left} − 1 = {kk - left - 1}"
        s.text(720, 150 + j * 26, f"при {v}: {what}", size=14, family=MONO)
    s.text(720, 300, "rank(x) — колко са < x: при всяко", size=14, fill=INK2)
    s.text(720, 322, "„надясно“ добавяме size(ляво) + 1", size=14, fill=INK2)
    s.text(720, 358, "Ротацията мени размерите само на", size=14, fill=INK2)
    s.text(720, 380, "двата си възела → update() ги оправя", size=14, fill=INK2)
    s.save(OUT / "order-stat.svg")
    return path


# ---------------------------------------------------------------- charts

def height_bounds():
    s = Svg(1100, 480, "Височина: граници и измерено")
    s.text(40, 44, "Височина: най-лошият случай за всеки вид дърво", size=24, weight=700)
    s.text(40, 70, "Логаритмична скала по n; линиите са горни граници, точките — измерени", size=15, fill=INK2)
    x0, y0, w, h0 = 100, 100, 560, 300
    fx = lambda n: x0 + (math.log10(n) - 3) / 6 * w
    fy = lambda v: y0 + h0 - v / 60 * h0
    axes(s, x0, y0, w, h0, [10 ** k for k in range(3, 10)], [0, 10, 20, 30, 40, 50, 60], "n (лог. ос)",
         "височина", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=str)
    ns = [10 ** (3 + i / 10) for i in range(61)]
    curves = [(S3, "перфектно: ⌊log₂ n⌋", lambda n: math.floor(math.log2(n))),
              (S1, "AVL, най-лошо: 1.44 log₂ n", lambda n: 1.4405 * math.log2(n + 2) - 0.3277),
              (RED, "червено-черно, най-лошо: 2 log₂ n", lambda n: 2 * math.log2(n + 1))]
    for col, _, f in curves:
        s.path(polyline([(fx(n), fy(f(n))) for n in ns]), col, sw=2)
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    for r in rows:
        if r["structure"] == "avl" and r["order"] == "random":
            s.circle(fx(int(r["n"])), fy(int(r["height"])), 5, S4)
    legend(s, 700, 130, [(c, l) for c, l, _ in curves] + [(S4, "вашето AVL, случаен вход")])
    s.text(700, 250, "При n = 10^9 (милиард):", size=14, weight=600)
    s.text(700, 272, "перфектно 29, AVL ≤ 42, ч-ч ≤ 59", size=14, fill=INK2)
    s.text(700, 294, "BST от миналата седмица: до 999 999 999", size=14, fill=RED)
    s.text(700, 330, "На практика AVL е близо до перфектното:", size=14, fill=INK2)
    s.text(700, 352, "23 при милион ключа (log₂ n ≈ 20)", size=14, fill=INK2)
    s.save(OUT / "height-bounds.svg")


def bench_chart():
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    s = Svg(1100, 500, "Време за вмъкване и търсене: AVL, std::set, обикновено BST")
    s.text(40, 44, "ns на операция, int ключове, случаен вход", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина. Червено: обикновеното BST при сортиран вход (спряно след 20 000)",
           size=15, fill=INK2)
    series = [(S1, "вашето AVL", "avl", "random"), (S4, "std::set (червено-черно)", "std::set", "random"),
              (S3, "обикновено BST", "plain_bst", "random"), (RED, "обикновено BST, сортиран", "plain_bst", "sorted")]
    for p, (title, key) in enumerate([("вмъкване", "insert_ns"), ("търсене", "find_ns")]):
        x0, y0, w, h0 = 100 + p * 420, 120, 320, 280
        fx = lambda n, x0=x0, w=w: x0 + (math.log10(n) - 3) / 3 * w
        fy = lambda v, y0=y0, h0=h0: y0 + h0 - (math.log10(v) - 1) / 4 * h0
        s.text(x0 + w / 2, 106, title, size=17, anchor="middle", weight=700)
        axes(s, x0, y0, w, h0, [1000, 10000, 100000, 1000000], [10, 100, 1000, 10000, 100000], "n (лог. ос)",
             "ns (лог. ос)" if p == 0 else "", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}",
             yfmt=lambda v: f"{v:g}")
        for col, _, name, order in series:
            pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows
                   if r["structure"] == name and r["order"] == order]
            s.path(polyline(pts), col, sw=2)
            for x, y in pts:
                s.circle(x, y, 4, col)
    legend(s, 880, 140, [(c, l) for c, l, _, _ in series])
    get = lambda name, order, n, key: float(next(r[key] for r in rows if r["structure"] == name
                                                 and r["order"] == order and r["n"] == str(n)))
    s.text(880, 260, "При 10^6, случаен вход:", size=13, weight=600)
    s.text(880, 280, f"вмъкване AVL {get('avl', 'random', 1000000, 'insert_ns'):.0f}, "
                     f"set {get('std::set', 'random', 1000000, 'insert_ns'):.0f}", size=13, fill=INK2)
    s.text(880, 300, f"търсене AVL {get('avl', 'random', 1000000, 'find_ns'):.0f}, "
                     f"set {get('std::set', 'random', 1000000, 'find_ns'):.0f}", size=13, fill=INK2)
    s.text(880, 336, "AVL: по-скъпо вмъкване,", size=13, fill=INK2)
    s.text(880, 354, "по-бързо търсене (по-ниско).", size=13, fill=INK2)
    ratio = get("plain_bst", "sorted", 20000, "insert_ns") / get("avl", "sorted", 20000, "insert_ns")
    s.text(880, 380, "Сортиран вход, n = 20 000:", size=13, fill=RED)
    s.text(880, 398, f"обикновеното BST вмъква ~{ratio:.0f}×", size=13, fill=RED)
    s.text(880, 416, "по-бавно от AVL.", size=13, fill=RED)
    s.save(OUT / "avl-bench.svg")


if __name__ == "__main__":
    rotation()
    avl_cases()
    lr_frames()
    avl_sorted()
    fib_trees()
    erase_cascade()
    rb_tree()
    btree()
    print("select(6) path:", order_stat())
    height_bounds()
    bench_chart()
