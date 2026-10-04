#!/usr/bin/env python3
"""Generates every SVG figure for week 14.

Run from anywhere:  python3 weeks/14-graphs-2/img/src/make_figures.py
The chart reads bench_sample.csv (`w14_bench --csv`, release preset).
Dijkstra/Kruskal traces run the same algorithms as solutions/graphs2.h.
"""

import csv
import heapq
import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"
YELLOW = "#fff4d6"
R = 20

POS = {0: (100, 330), 1: (250, 160), 2: (250, 330), 3: (460, 160), 4: (460, 330), 5: (610, 245)}
EDGES = [(0, 1, 2), (0, 2, 5), (1, 2, 1), (1, 3, 4), (2, 4, 8), (3, 4, 2), (3, 5, 6), (4, 5, 1)]


def draw_wgraph(s, pos, edges, fill=None, stroke=None, edge_style=None, labels=None, directed=False, r=R):
    fill, stroke, edge_style, labels = fill or {}, stroke or {}, edge_style or {}, labels or {}
    for u, v, w in edges:
        (x1, y1), (x2, y2) = pos[u], pos[v]
        col, sw, dash = edge_style.get((u, v), edge_style.get((v, u), (INK2, 1.6, None)))
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        end = r + (4 if directed else 0)
        s.line(x1 + ux * r, y1 + uy * r, x2 - ux * end, y2 - uy * end, stroke=col, sw=sw, dash=dash, arrow=directed)
        mx, my = (x1 + x2) / 2, (y1 + y2) / 2
        s.circle(mx, my, 11, SURFACE, stroke=GRID, sw=1)
        s.text(mx, my + 5, str(w), size=13, anchor="middle", family=MONO, weight=700, fill=col if col != INK2 else INK)
    for v, (x, y) in pos.items():
        s.circle(x, y, r, fill.get(v, TINT1), stroke=stroke.get(v, S1), sw=1.8)
        s.text(x, y + 5, str(labels.get(v, v)), size=15, anchor="middle", family=MONO, weight=700)


def adj():
    a = {v: [] for v in POS}
    for u, v, w in EDGES:
        a[u].append((v, w))
        a[v].append((u, w))
    return a


# ---------------------------------------------------------------- why not BFS

def weighted():
    s = Svg(1100, 360, "Тегла: BFS вече не дава най-къс път")
    s.text(40, 46, "С тегла BFS греши: най-малко ребра ≠ най-къс път", size=24, weight=700)
    pos = {0: (120, 200), 1: (330, 110), 2: (540, 110), 3: (750, 200)}
    edges = [(0, 3, 10), (0, 1, 1), (1, 2, 1), (2, 3, 1)]
    draw_wgraph(s, pos, edges, labels={0: "A", 1: "B", 2: "C", 3: "D"},
                edge_style={(0, 3): (RED, 3, None), (0, 1): (S3, 3, None), (1, 2): (S3, 3, None), (2, 3): (S3, 3, None)})
    s.text(430, 250, "BFS: A → D (1 ребро) = 10 km", size=15, anchor="middle", fill=RED, weight=600)
    s.text(430, 82, "A → B → C → D (3 ребра) = 3 km", size=15, anchor="middle", fill=S3, weight=600)
    s.text(40, 320, "Дейкстра (1959): BFS, в който опашката е min-пирамида по разстояние — винаги вадим НАЙ-БЛИЗКИЯ непосетен връх.",
           size=14, fill=INK2)
    s.save(OUT / "weighted.svg")


# ---------------------------------------------------------------- Dijkstra frames

def dijkstra_frames():
    a = adj()
    dist = {v: math.inf for v in POS}
    parent = {}
    dist[0] = 0
    heap = [(0, 0)]
    done = set()
    frames = []
    frames.append((dict(dist), set(done), sorted(heap), None, dict(parent), "старт: dist[0] = 0; в пирамидата (0, 0)"))
    while heap:
        d, u = heapq.heappop(heap)
        if d > dist[u]:
            frames.append((dict(dist), set(done), sorted(heap), None, dict(parent),
                           f"вадим ({d}, {u}) — остаряло (dist[{u}] = {dist[u]}): пропускаме"))
            continue
        done.add(u)
        relaxed = []
        for v, w in a[u]:
            if d + w < dist[v]:
                dist[v] = d + w
                parent[v] = u
                heapq.heappush(heap, (dist[v], v))
                relaxed.append(f"{v}→{dist[v]}")
        msg = f"вадим ({d}, {u}): {u} е готов" + (f"; обновяваме {', '.join(relaxed)}" if relaxed else "")
        frames.append((dict(dist), set(done), sorted(heap), u, dict(parent), msg))
    for k, (dist_, done_, heap_, cur, par, msg) in enumerate(frames):
        s = Svg(1100, 470, "Дейкстра стъпка по стъпка")
        s.text(40, 46, "Дейкстра: вади най-близкия, отпусни съседите му", size=24, weight=700)
        s.text(40, 82, msg, size=15, family=MONO, fill=INK2)
        fill, stroke = {}, {}
        for v in POS:
            if v == cur:
                fill[v], stroke[v] = TINT2, S2
            elif v in done_:
                fill[v], stroke[v] = TINT3, S3
            elif dist_[v] != math.inf:
                fill[v], stroke[v] = YELLOW, S4
            else:
                fill[v], stroke[v] = PANEL, GRID
        style = {(par[v], v): (S2, 3.5, None) for v in par}
        draw_wgraph(s, POS, EDGES, fill=fill, stroke=stroke, edge_style=style)
        for v, (x, y) in POS.items():
            t = "∞" if dist_[v] == math.inf else str(dist_[v])
            s.text(x, y - 28 if v in (1, 3) else y + 40, f"d={t}", size=13, anchor="middle", family=MONO,
                   fill=INK2, weight=600)
        s.text(720, 140, "min-пирамида (dist, връх):", size=15, weight=700)
        for i, (d, v) in enumerate(heap_):
            s.rect(720 + (i % 4) * 82, 156 + (i // 4) * 42, 74, 34, YELLOW, S4, rx=4)
            s.text(757 + (i % 4) * 82, 179 + (i // 4) * 42, f"({d}, {v})", size=14, anchor="middle", family=MONO)
        if not heap_:
            s.text(720, 180, "(празна)", size=14, fill=MUTED)
        for j, (f, st, lab) in enumerate([(TINT3, S3, "готов (dist е окончателно)"),
                                          (YELLOW, S4, "открит — dist е горна граница"),
                                          (PANEL, GRID, "неоткрит (∞)")]):
            s.rect(720, 280 + j * 26, 16, 16, f, st, rx=3)
            s.text(744, 293 + j * 26, lab, size=13)
        s.text(720, 380, "оранжево: най-кратките пътища досега", size=13, fill=INK2)
        s.text(720, 420, f"стъпка {k + 1} от {len(frames)}", size=13, fill=MUTED)
        s.save(OUT / f"dijkstra-{k + 1}.svg")
    return len(frames), [frames[-1][0][v] for v in POS]


def negative():
    s = Svg(1100, 330, "Отрицателно ребро чупи Дейкстра")
    s.text(40, 46, "Отрицателни тегла: Дейкстра бърза да обяви връх за готов", size=24, weight=700)
    pos = {0: (120, 200), 1: (330, 110), 2: (330, 280)}
    edges = [(0, 1, 2), (0, 2, 5), (2, 1, -4)]
    draw_wgraph(s, pos, edges, labels={0: "s", 1: "a", 2: "b"}, directed=True,
                edge_style={(2, 1): (RED, 3, None)}, fill={1: TINT_RED}, stroke={1: RED})
    for k, t in enumerate(["Дейкстра: вади a с dist = 2 и го обявява за готов.",
                           "После вади b (5) — но s → b → a = 5 − 4 = 1 < 2. Твърде късно.",
                           "",
                           "Доказателството ползва: всяко продължение на пътя само го удължава.",
                           "С отрицателни ребра това не е вярно.",
                           "",
                           "За напреднали — Bellman–Ford: n − 1 прохода по всички ребра, Θ(n · m); открива и",
                           "отрицателни цикли (тогава „най-къс път“ няма)."]):
        s.text(480, 110 + k * 26, t, size=14, fill=INK2 if k > 2 else INK)
    s.save(OUT / "negative.svg")


# ---------------------------------------------------------------- union-find

def union_find():
    s = Svg(1100, 470, "Обединение на множества: дървета, размер, свиване на пътя")
    s.text(40, 46, "Union-find: всяко множество е дърво; коренът е представителят", size=24, weight=700)

    def node(x, y, label, fill=TINT1, stroke=S1):
        s.circle(x, y, 18, fill, stroke=stroke, sw=1.6)
        s.text(x, y + 5, str(label), size=14, anchor="middle", family=MONO, weight=700)

    def up(x1, y1, x2, y2, col=INK2):
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        s.line(x1 + ux * 18, y1 + uy * 18, x2 - ux * 22, y2 - uy * 22, stroke=col, sw=1.6, arrow=True)
    # union by size
    s.text(40, 92, "① unite(4, 7): по-малкото дърво под по-голямото", size=15, weight=700)
    big = {1: (150, 140), 2: (100, 220), 3: (200, 220), 4: (200, 300)}
    for v, (x, y) in big.items():
        node(x, y, v)
    up(100, 220, 150, 140)
    up(200, 220, 150, 140)
    up(200, 300, 200, 220)
    small = {6: (340, 140), 7: (340, 220)}
    for v, (x, y) in small.items():
        node(x, y, v, TINT2, S2)
    up(340, 220, 340, 140)
    s.path("M 322,132 C 260,90 200,100 170,124", S2, sw=2.2, dash="6 4", arrow=True)
    s.text(40, 360, "размер 4 срещу 2: коренът 6 сочи 1.", size=13, fill=INK2)
    s.text(40, 380, "Височината расте само когато се слеят", size=13, fill=INK2)
    s.text(40, 400, "две еднакво големи → ≤ log₂ n.", size=13, fill=INK2)
    # path compression
    s.text(470, 92, "② find(5) със свиване на пътя", size=15, weight=700)
    chain = [(560, 140, 1), (560, 210, 2), (560, 280, 3), (560, 350, 5)]
    for x, y, v in chain:
        node(x, y, v, TINT2 if v == 5 else TINT1, S2 if v == 5 else S1)
    for (x1, y1, _), (x2, y2, _) in zip(chain[1:], chain[:-1]):
        up(x1, y1, x2, y2)
    s.text(560, 400, "преди", size=13, anchor="middle", fill=MUTED)
    s.line(620, 245, 690, 245, stroke=INK2, sw=2, arrow=True)
    node(820, 140, 1)
    for k, v in enumerate([2, 3, 5]):
        x = 740 + k * 80
        node(x, 260, v, TINT3, S3)
        up(x, 260, 820, 140, S3)
    s.text(820, 400, "след: всички сочат корена", size=13, anchor="middle", fill=MUTED)
    s.text(470, 440, "И двете заедно: m операции за O(m · α(n)); α(n) ≤ 4 за всяко n във Вселената.", size=14,
           fill=INK2, weight=600)
    s.save(OUT / "union-find.svg")


# ---------------------------------------------------------------- MST

def kruskal_figure():
    parent = list(range(6))

    def find(x):
        while parent[x] != x:
            x = parent[x]
        return x
    taken = []
    log = []
    for u, v, w in sorted(EDGES, key=lambda e: e[2]):
        ru, rv = find(u), find(v)
        ok = ru != rv
        if ok:
            parent[ru] = rv
            taken.append((u, v))
        log.append((u, v, w, ok))
    s = Svg(1100, 470, "Kruskal: ребрата по тегло, без цикли")
    s.text(40, 46, "Kruskal: сортирай ребрата, взимай всяко, което не затваря цикъл", size=22, weight=700)
    style = {}
    for u, v, w in EDGES:
        style[(u, v)] = (S3, 4, None) if (u, v) in taken or (v, u) in taken else (GRID, 1.4, "4 4")
    draw_wgraph(s, POS, EDGES, edge_style=style)
    total = sum(w for u, v, w, ok in log if ok)
    s.text(720, 110, "ребро   тегло", size=14, family=MONO, weight=700)
    for k, (u, v, w, ok) in enumerate(log):
        y = 140 + k * 30
        s.text(720, y, f"{u}–{v}     {w}", size=14, family=MONO)
        s.text(860, y, "✓ взимаме" if ok else "✗ цикъл", size=14, fill=S3 if ok else RED, weight=600)
    s.text(720, 140 + len(log) * 30 + 10, f"общо: {total}  ({len(taken)} = n − 1 ребра)", size=15, weight=700)
    s.text(40, 430, "„Цикъл ли е?“ = „в едно множество ли са краищата?“ — union-find. Θ(m log m) заради сортирането.",
           size=14, fill=INK2)
    s.save(OUT / "kruskal.svg")
    return total


def cut_property():
    s = Svg(1100, 400, "Свойството на среза")
    s.text(40, 46, "Защо работи: най-лекото ребро през всеки срез е в някое МПД", size=22, weight=700)
    left = {0: (120, 180), 1: (220, 120), 2: (220, 260)}
    right = {3: (560, 120), 4: (560, 260), 5: (680, 190)}
    pos = left | right
    inner = [(0, 1, 2), (0, 2, 5), (1, 2, 1), (3, 4, 2), (3, 5, 6), (4, 5, 1)]
    cross = [(1, 3, 4), (2, 4, 8)]
    s.path("M 390,80 C 360,180 420,260 390,340", RED, sw=2, dash="8 6")
    s.text(390, 362, "срез", size=14, anchor="middle", fill=RED, weight=600)
    draw_wgraph(s, pos, inner + cross, edge_style={(1, 3): (S3, 4, None), (2, 4): (INK2, 1.6, None)})
    s.text(760, 120, "Разделете върховете на две групи.", size=14, fill=INK2)
    s.text(760, 146, "От ребрата между групите — най-лекото", size=14, fill=INK2)
    s.text(760, 168, "(тук 1–3, тегло 4) е безопасно.", size=14, fill=INK2)
    s.text(760, 210, "Kruskal: срезът е „компонентата на u“", size=14, fill=INK2)
    s.text(760, 232, "срещу „всичко друго“.", size=14, fill=INK2)
    s.text(760, 266, "Prim: срезът е „дървото досега“", size=14, fill=INK2)
    s.text(760, 288, "срещу „останалите“.", size=14, fill=INK2)
    s.save(OUT / "cut-property.svg")


# ---------------------------------------------------------------- chart

def bench_chart():
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    s = Svg(1100, 560, "Дейкстра: пирамида срещу масив")
    s.text(40, 44, "Дейкстра: пирамида Θ((n + m) log n) срещу масив Θ(n²)", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина; ms за едно пускане", size=15, fill=INK2)
    x0, y0, w, h0 = 100, 110, 420, 280
    fx = lambda n: x0 + (math.log10(n) - 3) / 3 * w
    fy = lambda v: y0 + h0 - (math.log10(v) + 1) / 6 * h0
    s.text(x0 + w / 2, 100, "разреден граф, m = 4n", size=15, anchor="middle", weight=700)
    axes(s, x0, y0, w, h0, [1000, 10000, 100000, 1000000], [0.1, 1, 10, 100, 1000, 10000, 100000],
         "n (лог. ос)", "ms (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    for col, key in [(S1, "heap"), (RED, "array")]:
        pts = [(fx(int(r["n"])), fy(float(r["ms"]))) for r in rows if r["kind"] == "sparse" and r["algorithm"] == key]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    # dense: bars
    xb, yb, wb, hb = 640, 110, 400, 280
    s.text(xb + wb / 2, 100, "пълен граф, m ≈ n²/2", size=15, anchor="middle", weight=700)
    dense = [r for r in rows if r["kind"] == "dense"]
    ns = sorted({int(r["n"]) for r in dense})
    vmax = max(float(r["ms"]) for r in dense) * 1.15
    gw = wb / len(ns)
    for t in [0, 5, 10, 15, 20, 25]:
        y = yb + hb - t / vmax * hb
        if y < yb:
            continue
        s.line(xb, y, xb + wb, y, stroke=GRID, sw=1)
        s.text(xb - 8, y + 5, str(t), size=12, anchor="end", fill=INK2)
    for g, n in enumerate(ns):
        for k, (key, col) in enumerate([("heap", S1), ("array", RED)]):
            v = float(next(r["ms"] for r in dense if int(r["n"]) == n and r["algorithm"] == key))
            bx = xb + g * gw + 12 + k * (gw - 24) / 2
            bw = (gw - 24) / 2 - 4
            top = yb + hb - v / vmax * hb
            s.add(f'<rect x="{bx:.1f}" y="{top:.1f}" width="{bw:.1f}" height="{yb + hb - top:.1f}" rx="3" fill="{col}"/>')
            s.text(bx + bw / 2, top - 5, f"{v:.1f}", size=11, anchor="middle", fill=INK2)
        s.text(xb + g * gw + gw / 2, yb + hb + 20, f"n = {n}", size=12, anchor="middle")
    s.line(xb, yb + hb, xb + wb, yb + hb, stroke=MUTED, sw=1)
    legend(s, 120, 480, [(S1, "пирамида (вашият dijkstra)")])
    legend(s, 440, 480, [(RED, "масив (Дейкстра, 1959)")])
    g = lambda kind, n, a: float(next(r["ms"] for r in rows if r["kind"] == kind and r["n"] == str(n)
                                      and r["algorithm"] == a))
    s.text(40, 520, f"Разреден, n = 10^5: {g('sparse', 100000, 'heap'):.0f} ms срещу {g('sparse', 100000, 'array') / 1000:.1f} s. "
                    "Пълен: пирамидата пак печели — при случайни тегла малко ребра подобряват разстояние,", size=13,
           fill=INK2)
    s.text(40, 540, "затова в пирамидата влизат много по-малко от m двойки. Учебникарското „масивът е по-добър за гъсти графи“ е за най-лошия случай.",
           size=13, fill=INK2)
    s.save(OUT / "bench-chart.svg")


if __name__ == "__main__":
    weighted()
    print("dijkstra frames, final dist:", dijkstra_frames())
    negative()
    union_find()
    print("kruskal total:", kruskal_figure())
    cut_property()
    bench_chart()
