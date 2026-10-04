#!/usr/bin/env python3
"""Generates every SVG figure for week 13.

Run from anywhere:  python3 weeks/13-graphs-1/img/src/make_figures.py
The chart reads bench_sample.csv (`w13_bench --csv`, release preset).
BFS/DFS/Kahn traces run the same algorithms as solutions/graph.h, in Python.
"""

import csv
import math
import sys
from collections import deque
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "tools"))
from figlib import *  # noqa: E402,F401,F403

HERE = Path(__file__).resolve().parent
OUT = HERE.parent
TINT_RED = "#fbe3e3"
YELLOW = "#fff4d6"
DARK = "#3a3a3a"
R = 20


def adj_lists(n, edges, directed=False):
    adj = [[] for _ in range(n)]
    for u, v in edges:
        adj[u].append(v)
        if not directed and u != v:
            adj[v].append(u)
    return adj


def draw_graph(s, pos, edges, directed=False, fill=None, stroke=None, text_fill=None, edge_style=None, labels=None,
               r=R, size=15):
    fill, stroke, text_fill, edge_style = fill or {}, stroke or {}, text_fill or {}, edge_style or {}
    labels = labels or {}
    for u, v in edges:
        (x1, y1), (x2, y2) = pos[u], pos[v]
        col, sw, dash = edge_style.get((u, v), edge_style.get((v, u), (INK2, 1.6, None)) if not directed else
                                       (INK2, 1.6, None))
        L = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / L, (y2 - y1) / L
        end = r + (4 if directed else 0)
        s.line(x1 + ux * r, y1 + uy * r, x2 - ux * end, y2 - uy * end, stroke=col, sw=sw, arrow=directed, dash=dash)
    for v, (x, y) in pos.items():
        s.circle(x, y, r, fill.get(v, TINT1), stroke=stroke.get(v, S1), sw=1.8)
        s.text(x, y + size * 0.36, str(labels.get(v, v)), size=size, anchor="middle", family=MONO, weight=700,
               fill=text_fill.get(v, INK))


SAMPLE_POS = {0: (110, 140), 1: (250, 140), 2: (110, 270), 3: (250, 270), 4: (390, 270), 5: (520, 140),
              6: (520, 270), 7: (640, 200)}
SAMPLE_EDGES = [(0, 1), (0, 2), (1, 3), (2, 3), (3, 4), (5, 6)]


# ---------------------------------------------------------------- basics

def graph_terms():
    s = Svg(1100, 470, "Граф: основни понятия")
    s.text(40, 46, "Граф G = (V, E): върхове и ребра", size=24, weight=700)
    style = {(0, 1): (S2, 3.5, None), (1, 3): (S2, 3.5, None), (3, 4): (S2, 3.5, None)}
    draw_graph(s, SAMPLE_POS, SAMPLE_EDGES, edge_style=style, fill={3: TINT2}, stroke={3: S2})
    s.rect(70, 100, 360, 210, "none", S1, rx=18, sw=1.2, dash="6 5")
    s.rect(480, 100, 82, 210, "none", S3, rx=18, sw=1.2, dash="6 5")
    s.rect(600, 160, 80, 80, "none", S5, rx=18, sw=1.2, dash="6 5")
    s.text(70, 336, "три свързани компоненти", size=14, fill=INK2)
    s.text(270, 252, "deg(3) = 3", size=13, fill=S2, weight=600)
    s.text(40, 380, "път 0 → 1 → 3 → 4 (оранжево): дължина 3", size=14, fill=INK2)
    s.text(40, 402, "цикъл 0 – 1 – 3 – 2 – 0; 7 е изолиран връх", size=14, fill=INK2)
    s.text(40, 424, "n = |V| = 8, m = |E| = 6; ненасочен: ребро {u, v} = {v, u}", size=14, fill=INK2)
    # directed on the right
    pos = {0: (780, 140), 1: (900, 140), 2: (1020, 210), 3: (780, 280), 4: (900, 280)}
    edges = [(0, 1), (1, 2), (3, 4), (4, 2), (3, 0)]
    draw_graph(s, pos, edges, directed=True, fill={2: TINT2}, stroke={2: S2})
    s.text(780, 96, "насочен граф", size=16, weight=700)
    s.text(780, 336, "дъга (u, v) ≠ (v, u)", size=14, fill=INK2)
    s.text(780, 358, "вх. степен на 2: 2, изх. степен: 0", size=14, fill=S2)
    s.text(780, 380, "насочен цикъл? — няма", size=14, fill=INK2)
    s.save(OUT / "graph-terms.svg")


def representations():
    s = Svg(1100, 470, "Три представяния на граф")
    s.text(40, 46, "Представяне: матрица, списъци, списък на ребрата", size=24, weight=700)
    pos = {0: (90, 130), 1: (210, 130), 2: (90, 250), 3: (210, 250), 4: (300, 190)}
    edges = [(0, 1), (0, 2), (1, 3), (2, 3), (3, 4)]
    draw_graph(s, pos, edges)
    n = 5
    adj = adj_lists(n, edges)
    # matrix
    x0, y0, c = 380, 110, 30
    s.text(x0, 92, "матрица на съседство", size=15, weight=700)
    for i in range(n):
        s.text(x0 + 24 + i * c + c / 2, y0 + 10, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)
        s.text(x0 + 12, y0 + 34 + i * c, str(i), size=12, anchor="middle", fill=MUTED, family=MONO)
        for j in range(n):
            on = j in adj[i]
            s.rect(x0 + 24 + j * c, y0 + 16 + i * c, c - 2, c - 2, TINT1 if on else PANEL, S1 if on else GRID, rx=3)
            s.text(x0 + 24 + j * c + c / 2 - 1, y0 + 36 + i * c, "1" if on else "0", size=13, anchor="middle",
                   family=MONO, fill=INK if on else MUTED)
    # lists
    x1 = 620
    s.text(x1, 92, "списъци на съседство", size=15, weight=700)
    for i in range(n):
        y = y0 + 16 + i * c
        s.rect(x1, y, 28, c - 2, PANEL, GRID, rx=3)
        s.text(x1 + 14, y + 20, str(i), size=13, anchor="middle", family=MONO, fill=MUTED)
        for k, v in enumerate(adj[i]):
            s.line(x1 + 30 + k * 40, y + 14, x1 + 40 + k * 40, y + 14, stroke=INK2, sw=1.2)
            s.rect(x1 + 40 + k * 40, y, 30, c - 2, TINT1, S1, rx=3)
            s.text(x1 + 55 + k * 40, y + 20, str(v), size=13, anchor="middle", family=MONO, weight=700)
    # edge list
    x2 = 900
    s.text(x2, 92, "списък на ребрата", size=15, weight=700)
    for k, (u, v) in enumerate(edges):
        y = y0 + 16 + k * c
        s.rect(x2, y, 90, c - 2, TINT1, S1, rx=3)
        s.text(x2 + 45, y + 20, f"{u} – {v}", size=13, anchor="middle", family=MONO, weight=700)
    rows = [("", "матрица", "списъци", "ребра"), ("памет", "Θ(n²)", "Θ(n + m)", "Θ(m)"),
            ("има ли ребро u–v?", "Θ(1)", "Θ(deg u)", "Θ(m)"), ("съседите на u", "Θ(n)", "Θ(deg u)", "Θ(m)"),
            ("подходящо за", "гъсти графи", "почти всичко", "Kruskal (седм. 14)")]
    for k, row in enumerate(rows):
        y = 300 + k * 30
        for j, t in enumerate(row):
            x = [40, 260, 470, 690][j]
            s.text(x, y, t, size=14, weight=700 if k == 0 or j == 0 else 400, family=MONO if "Θ" in t else None,
                   fill=INK if k == 0 or j == 0 else INK2)
    s.save(OUT / "representations.svg")


# ---------------------------------------------------------------- BFS / DFS

BFS_POS = {0: (110, 230), 1: (260, 120), 2: (260, 230), 3: (260, 340), 4: (410, 120), 5: (410, 230),
           6: (410, 340), 7: (560, 175), 8: (560, 300)}
BFS_EDGES = [(0, 1), (0, 2), (0, 3), (1, 4), (2, 4), (2, 5), (3, 6), (4, 7), (5, 7), (6, 8), (7, 8), (1, 2)]


def bfs_frames():
    adj = adj_lists(9, BFS_EDGES)
    dist = {0: 0}
    parent = {}
    q = deque([0])
    order = []
    snapshots = []
    level_done = -1
    while q:
        u = q.popleft()
        order.append(u)
        for v in adj[u]:
            if v not in dist:
                dist[v] = dist[u] + 1
                parent[v] = u
                q.append(v)
        nxt = dist[q[0]] if q else None
        if nxt is None or nxt > dist[u]:
            snapshots.append((dict(dist), dict(parent), list(q), dist[u]))
    titles = ["BFS от 0: разстояние 0 — началото; в опашката: съседите му",
              "Разстояние 1 обработено; в опашката: разстояние 2",
              "Разстояние 2 обработено; в опашката: разстояние 3",
              "Готово: дървото на BFS (оранжево) = най-кратки пътища"]
    level_cols = [S2, S1, S3, S5]
    for k, (d, par, queue, lvl) in enumerate(snapshots):
        s = Svg(1100, 440, titles[k])
        s.text(40, 46, titles[k], size=22, weight=700)
        fill, stroke, labels = {}, {}, {}
        for v in BFS_POS:
            if v in d and d[v] <= lvl:
                fill[v], stroke[v] = TINT3, S3
            elif v in d:
                fill[v], stroke[v] = YELLOW, S4
            else:
                fill[v], stroke[v] = PANEL, GRID
        style = {(par[v], v): (S2, 3.5, None) for v in par if d[v] <= lvl + 1}
        draw_graph(s, BFS_POS, BFS_EDGES, fill=fill, stroke=stroke, edge_style=style)
        for v, (x, y) in BFS_POS.items():
            if v in d:
                s.text(x + 24, y - 16, f"d={d[v]}", size=12, fill=INK2, family=MONO)
        s.text(680, 120, "опашка (отпред →):", size=15, weight=700)
        for i, v in enumerate(queue):
            cell = (680 + i * 44, 136)
            s.rect(cell[0], cell[1], 38, 34, YELLOW, S4, rx=4)
            s.text(cell[0] + 19, cell[1] + 23, str(v), size=15, anchor="middle", family=MONO, weight=700)
        if not queue:
            s.text(680, 160, "(празна)", size=14, fill=MUTED)
        s.rect(680, 220, 16, 16, TINT3, S3, rx=3)
        s.text(704, 233, "обработен", size=13)
        s.rect(680, 246, 16, 16, YELLOW, S4, rx=3)
        s.text(704, 259, "открит — в опашката", size=13)
        s.rect(680, 272, 16, 16, PANEL, GRID, rx=3)
        s.text(704, 285, "неоткрит", size=13)
        s.text(680, 330, "Маркираме връх, когато го слагаме", size=13, fill=INK2)
        s.text(680, 350, "в опашката — не когато го вадим.", size=13, fill=INK2)
        s.text(680, 384, f"стъпка {k + 1} от {len(snapshots)}", size=13, fill=MUTED)
        s.save(OUT / f"bfs-{k + 1}.svg")
    return len(snapshots), order


def dfs_figure():
    adj = adj_lists(9, BFS_EDGES)
    seen, order, parent = set(), [], {}
    deepest = []
    stack = []

    def visit(u):
        seen.add(u)
        order.append(u)
        stack.append(u)
        nonlocal deepest
        if len(stack) > len(deepest):
            deepest = list(stack)
        for v in adj[u]:
            if v not in seen:
                parent[v] = u
                visit(v)
        stack.pop()
    visit(0)
    s = Svg(1100, 500, "DFS: по-навътре, после назад")
    s.text(40, 46, "DFS от 0: колкото може по-навътре, после назад", size=24, weight=700)
    style = {(parent[v], v): (S2, 3.5, None) for v in parent}
    draw_graph(s, BFS_POS, BFS_EDGES, edge_style=style)
    for k, v in enumerate(order):
        x, y = BFS_POS[v]
        s.circle(x + 20, y - 18, 10, S2, stroke=SURFACE)
        s.text(x + 20, y - 14, str(k + 1), size=11, anchor="middle", fill=SURFACE, weight=700)
    s.text(680, 110, "ред на откриване:", size=15, weight=700)
    s.text(680, 136, " ".join(map(str, order)), size=16, family=MONO, weight=600)
    s.text(680, 186, "стекът на извикванията", size=15, weight=700)
    s.text(680, 206, "в най-дълбокия момент:", size=13, fill=INK2)
    for i, v in enumerate(reversed(deepest)):
        s.rect(680, 220 + i * 22, 150, 20, TINT2 if i == 0 else PANEL, S2 if i == 0 else GRID, rx=3)
        s.text(690, 235 + i * 22, f"dfsVisit({v})", size=12, family=MONO)
    s.text(840, 235, "← връх", size=13, fill=INK2)
    s.text(840, 257, f"дълбочина {len(deepest)}", size=13, fill=INK2)
    depth8, v = 0, 8
    while v in parent:
        v = parent[v]
        depth8 += 1
    s.text(40, 450, "Оранжево: дървото на DFS. Другите ребра водят до вече посетени върхове.", size=13, fill=INK2)
    s.text(40, 472, f"DFS не дава най-кратки пътища: в дървото 0 → 8 е {depth8} ребра, а BFS дава 3.", size=13,
           fill=INK2)
    s.save(OUT / "dfs.svg")
    return order


def maze():
    grid = ["S.#.....", ".##.###.", "....#T#.", ".##.#.#.", "....#..."]
    rows, cols = len(grid), len(grid[0])
    sr, sc = 0, 0
    dist = {(sr, sc): 0}
    par = {}
    q = deque([(sr, sc)])
    while q:
        r, c = q.popleft()
        for dr, dc in ((-1, 0), (1, 0), (0, -1), (0, 1)):
            nr, nc = r + dr, c + dc
            if 0 <= nr < rows and 0 <= nc < cols and grid[nr][nc] != "#" and (nr, nc) not in dist:
                dist[(nr, nc)] = dist[(r, c)] + 1
                par[(nr, nc)] = (r, c)
                q.append((nr, nc))
    t = next((r, c) for r in range(rows) for c in range(cols) if grid[r][c] == "T")
    path = set()
    cur = t
    while cur in par:
        path.add(cur)
        cur = par[cur]
    path.add((sr, sc))
    s = Svg(1100, 420, "BFS в лабиринт")
    s.text(40, 46, f"Лабиринт: неявен граф; BFS дава разстоянието до всяка клетка (S → T: {dist[t]})", size=22,
           weight=700)
    cw = 58
    x0, y0 = 40, 80
    for r in range(rows):
        for c in range(cols):
            x, y = x0 + c * cw, y0 + r * cw
            ch = grid[r][c]
            if ch == "#":
                s.rect(x, y, cw - 4, cw - 4, DARK, DARK, rx=4)
                continue
            on = (r, c) in path
            s.rect(x, y, cw - 4, cw - 4, TINT2 if on else TINT1, S2 if on else S1, rx=4)
            label = ch if ch in "ST" else str(dist.get((r, c), ""))
            s.text(x + (cw - 4) / 2, y + cw / 2 + 2, label, size=16, anchor="middle", family=MONO, weight=700)
            if ch in "ST":
                s.text(x + cw - 10, y + 14, str(dist[(r, c)]), size=10, anchor="end", fill=INK2, family=MONO)
    x = 560
    for k, t2 in enumerate(["Връх = свободна клетка (r, c).", "Ребра = до 4 съседа: горе, долу, ляво, дясно.",
                            "Списъци на съседство НЕ се строят —", "съседите се смятат при нужда.", "",
                            "Числата: разстоянието от S (BFS).", "Оранжево: един най-кратък път.",
                            "Задача 3 — 1000 × 1000 клетки за милисекунди."]):
        s.text(x, 110 + k * 26, t2, size=15, fill=INK2)
    s.save(OUT / "maze.svg")
    return dist[t]


# ---------------------------------------------------------------- DAGs

COURSES = ["УП", "ООП", "ДС", "СДП", "ДАА", "БД"]
COURSE_EDGES = [(0, 1), (1, 3), (2, 3), (3, 4), (2, 4), (1, 5)]


def topo():
    n = len(COURSES)
    adj = adj_lists(n, COURSE_EDGES, directed=True)
    indeg = [0] * n
    for u, v in COURSE_EDGES:
        indeg[v] += 1
    import heapq
    ready = [u for u in range(n) if indeg[u] == 0]
    heapq.heapify(ready)
    order, steps = [], []
    deg = list(indeg)
    while ready:
        u = heapq.heappop(ready)
        order.append(u)
        for v in adj[u]:
            deg[v] -= 1
            if deg[v] == 0:
                heapq.heappush(ready, v)
        steps.append((u, list(deg), sorted(ready)))
    s = Svg(1100, 520, "Топологична подредба на курсове")
    s.text(40, 46, "Топологична подредба: всяка стрелка сочи надясно", size=24, weight=700)
    pos = {0: (100, 140), 1: (260, 140), 2: (100, 270), 3: (420, 205), 4: (580, 270), 5: (420, 110)}
    draw_graph(s, pos, COURSE_EDGES, directed=True, labels={i: c for i, c in enumerate(COURSES)}, r=26, size=13)
    s.text(40, 322, "УП — увод в програмирането, ДС — дискретни структури, ДАА — дизайн и анализ на алгоритми, БД — бази данни",
           size=12, fill=MUTED)
    s.text(720, 96, "Kahn: опашка от върхове с входна степен 0", size=15, weight=700)
    for k, (u, deg, ready) in enumerate(steps):
        y = 126 + k * 30
        s.text(720, y, f"{k + 1}. вземи {COURSES[u]:<4}", size=14, family=MONO)
        s.text(870, y, "готови: " + (", ".join(COURSES[v] for v in ready) or "—"), size=13, fill=INK2)
    # the order as a row with arrows
    y = 440
    xs = {u: 60 + k * 170 for k, u in enumerate(order)}
    for u, v in COURSE_EDGES:
        x1, x2 = xs[u], xs[v]
        h = 22 + 12 * abs(order.index(v) - order.index(u))
        s.path(f"M {x1 + 30},{y} C {x1 + 30},{y - h} {x2 - 10},{y - h} {x2 - 2},{y - 6}", INK2, sw=1.3, arrow=True)
    for u, x in xs.items():
        s.rect(x - 4, y, 68, 32, TINT3, S3, rx=6)
        s.text(x + 30, y + 21, COURSES[u], size=14, anchor="middle", weight=700)
    s.text(40, 505, "Ако накрая останат върхове — те са в цикъл и подредба няма. Θ(n + m).", size=14, fill=INK2)
    s.save(OUT / "topo.svg")
    return [COURSES[u] for u in order]


def cycle_colours():
    s = Svg(1100, 400, "Откриване на цикъл с три цвята")
    s.text(40, 46, "Цикъл в насочен граф: ребро към СИВ връх", size=24, weight=700)
    pos = {0: (110, 150), 1: (260, 150), 2: (410, 150), 3: (260, 290)}
    edges = [(0, 1), (1, 2), (2, 3), (3, 1)]
    draw_graph(s, pos, edges, directed=True, fill={0: PANEL, 1: PANEL, 2: PANEL, 3: PANEL},
               stroke={0: DARK, 1: DARK, 2: DARK, 3: DARK},
               edge_style={(3, 1): (RED, 3, None)})
    for v in pos:
        x, y = pos[v]
        s.circle(x, y, R, "#d8d8d8", stroke=DARK, sw=1.8)
        s.text(x, y + 5, str(v), size=15, anchor="middle", family=MONO, weight=700)
    s.text(300, 300, "3 → 1: 1 е СИВ → цикъл 1 → 2 → 3 → 1", size=14, fill=RED, weight=600)
    s.text(40, 360, "Пътят на DFS 0 → 1 → 2 → 3 — всички сиви (на стека).", size=14, fill=INK2)
    pos2 = {0: (640, 150), 1: (790, 100), 2: (790, 220), 3: (940, 160)}
    edges2 = [(0, 1), (0, 2), (1, 3), (2, 3)]
    draw_graph(s, pos2, edges2, directed=True, fill={3: DARK, 1: DARK, 0: "#d8d8d8", 2: "#d8d8d8"},
               stroke={v: DARK for v in pos2}, text_fill={3: SURFACE, 1: SURFACE},
               edge_style={(2, 3): (S3, 3, None)})
    s.text(640, 290, "2 → 3: 3 е ЧЕРЕН (завършен) → НЕ е цикъл,", size=14, fill=S3, weight=600)
    s.text(640, 312, "просто два пътя до 3.", size=14, fill=S3, weight=600)
    s.rect(640, 345, 16, 16, PANEL, DARK, rx=3)
    s.text(662, 358, "бял — непосетен", size=13)
    s.rect(790, 345, 16, 16, "#d8d8d8", DARK, rx=3)
    s.text(812, 358, "сив — на стека", size=13)
    s.rect(930, 345, 16, 16, DARK, DARK, rx=3)
    s.text(952, 358, "черен — завършен", size=13)
    s.save(OUT / "cycle-colours.svg")


def bipartite():
    s = Svg(1100, 380, "Двуделен граф")
    s.text(40, 46, "Двуделен граф: оцветяване в 2 цвята, всяко ребро между различни", size=22, weight=700)

    def ring(cx, cy, n, rad):
        return {i: (cx + rad * math.cos(2 * math.pi * i / n - math.pi / 2),
                    cy + rad * math.sin(2 * math.pi * i / n - math.pi / 2)) for i in range(n)}
    p6 = ring(260, 200, 6, 110)
    e6 = [(i, (i + 1) % 6) for i in range(6)]
    draw_graph(s, p6, e6, fill={i: (TINT1 if i % 2 == 0 else TINT2) for i in range(6)},
               stroke={i: (S1 if i % 2 == 0 else S2) for i in range(6)})
    s.text(260, 345, "четен цикъл: да", size=15, anchor="middle", weight=700, fill=S3)
    p5 = ring(760, 200, 5, 110)
    e5 = [(i, (i + 1) % 5) for i in range(5)]
    cols = {0: (TINT1, S1), 1: (TINT2, S2), 2: (TINT1, S1), 3: (TINT2, S2), 4: (TINT1, S1)}
    draw_graph(s, p5, e5, fill={i: cols[i][0] for i in range(5)}, stroke={i: cols[i][1] for i in range(5)},
               edge_style={(4, 0): (RED, 3.5, None)})
    s.text(760, 345, "нечетен цикъл: не — 4 и 0 са в един цвят", size=15, anchor="middle", weight=700, fill=RED)
    s.text(470, 110, "BFS: коренът — синьо,", size=13, fill=INK2)
    s.text(470, 130, "съседите — оранжево, …", size=13, fill=INK2)
    s.text(470, 160, "Граф е двуделен ⇔", size=13, fill=INK2, weight=600)
    s.text(470, 180, "няма нечетен цикъл", size=13, fill=INK2, weight=600)
    s.save(OUT / "bipartite.svg")


# ---------------------------------------------------------------- chart

def bench_chart():
    rows = list(csv.DictReader((HERE.parent / "bench_sample.csv").open()))
    s = Svg(1100, 500, "BFS: матрица, списъци, CSR")
    s.text(40, 44, "Едно BFS, случаен граф с n върха и 4n ребра (ms)", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина; матрицата — само до 16 000 (256 MB)", size=15, fill=INK2)
    x0, y0, w, h0 = 100, 100, 560, 320
    fx = lambda n: x0 + (math.log10(n) - 3) / (math.log10(4e6) - 3) * w
    fy = lambda v: y0 + h0 - (math.log10(v) + 3) / 6 * h0
    axes(s, x0, y0, w, h0, [1000, 10000, 100000, 1000000], [0.001, 0.01, 0.1, 1, 10, 100, 1000], "n (лог. ос)",
         "ms (лог. ос)", fx, fy, xfmt=lambda n: f"10^{int(math.log10(n))}", yfmt=lambda v: f"{v:g}")
    series = [(RED, "матрица на съседство", "matrix"), (S1, "списъци (вашият Graph)", "lists"),
              (S3, "CSR (един масив)", "csr")]
    for col, _, key in series:
        pts = [(fx(int(r["n"])), fy(float(r["ms"]))) for r in rows if r["structure"] == key]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 700, 130, [(c, l) for c, l, _ in series])
    g = lambda n, k, f: float(next(r[f] for r in rows if r["n"] == str(n) and r["structure"] == k))
    s.text(700, 230, f"n = 16 000: матрица {g(16000, 'matrix', 'ms'):.0f} ms / {g(16000, 'matrix', 'mb'):.0f} MB,",
           size=14, fill=INK2)
    s.text(700, 252, f"списъци {g(16000, 'lists', 'ms'):.2f} ms / {g(16000, 'lists', 'mb'):.1f} MB", size=14,
           fill=INK2)
    s.text(700, 288, f"n = 4·10^6: списъци {g(4000000, 'lists', 'ms'):.0f} ms, CSR {g(4000000, 'csr', 'ms'):.0f} ms",
           size=14, fill=INK2)
    s.text(700, 310, f"памет {g(4000000, 'lists', 'mb'):.0f} MB срещу {g(4000000, 'csr', 'mb'):.0f} MB", size=14,
           fill=INK2)
    s.text(700, 346, "Матрицата: Θ(n²) — наклон 2.", size=14, fill=INK2)
    s.text(700, 368, "Списъци и CSR: Θ(n + m) — наклон 1;", size=14, fill=INK2)
    s.text(700, 390, "CSR — по-малко памет, по-малко промахи.", size=14, fill=INK2)
    s.save(OUT / "bench-chart.svg")


if __name__ == "__main__":
    graph_terms()
    representations()
    k, order = bfs_frames()
    print("bfs frames:", k, "order", order)
    print("dfs order:", dfs_figure())
    print("maze S->T:", maze())
    print("topo:", topo())
    cycle_colours()
    bipartite()
    bench_chart()
