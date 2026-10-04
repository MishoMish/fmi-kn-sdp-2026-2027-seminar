#!/usr/bin/env python3
"""Generates every SVG figure for week 05.

Run from anywhere:  python3 weeks/05-singly-linked-list/img/src/make_figures.py
The chart reads bench_sample.csv (`w05_bench --csv`, release preset).
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
NW, NH = 92, 44  # node box: value part + next part


def node(s, x, y, value, fill=TINT1, stroke=S1, null=False, label=None):
    """Draws a node at (x, y); returns the anchor of its next-pointer."""
    s.rect(x, y, NW, NH, fill, stroke, rx=6, sw=1.6)
    s.line(x + 58, y, x + 58, y + NH, stroke=stroke, sw=1.2)
    s.text(x + 29, y + 28, str(value), size=16, anchor="middle", family=MONO, weight=600)
    if null:
        s.line(x + 62, y + NH - 6, x + NW - 6, y + 6, stroke=MUTED, sw=1.5)
    else:
        s.circle(x + 75, y + NH / 2, 3.5, INK2, stroke=INK2, sw=0)
    if label:
        s.text(x + NW / 2, y + NH + 20, label, size=13, anchor="middle", fill=MUTED, family=MONO)
    return (x + 75, y + NH / 2)


def arrow(s, a, b, col=INK2, sw=1.6, dash=None):
    s.line(a[0], a[1], b[0], b[1], stroke=col, sw=sw, arrow=True, dash=dash)


def pointer_label(s, x, y, name, target, col=S1):
    s.text(x, y, name, size=15, family=MONO, weight=700, anchor="middle")
    arrow(s, (x, y + 6), target, col=col)


# ---------------------------------------------------------------- anatomy

def anatomy():
    s = Svg(1000, 330, "Едносвързан списък: възли, head, tail, size")
    s.text(40, 46, "Едносвързан списък", size=24, weight=700)
    xs = [120, 300, 480, 660]
    vals = ["a", "b", "c", "d"]
    for i, (x, v) in enumerate(zip(xs, vals)):
        p = node(s, x, 150, v, null=(i == 3))
        if i < 3:
            arrow(s, p, (xs[i + 1] - 2, 172))
    pointer_label(s, 160, 104, "head_", (160, 146))
    pointer_label(s, 700, 104, "tail_", (700, 146))
    s.text(850, 178, "size_ = 4", size=16, family=MONO, weight=600)
    s.text(120, 232, "value", size=13, fill=MUTED, anchor="start")
    s.text(178, 232, "next", size=13, fill=MUTED, anchor="start")
    s.text(40, 280, "Всеки възел е отделна алокация в heap-а — някъде в паметта. Знаем само как да стигнем",
           size=15, fill=INK2)
    s.text(40, 302, "от един възел до следващия: позиция i струва i стъпки. Последният сочи nullptr.", size=15,
           fill=INK2)
    s.save(OUT / "list-anatomy.svg")


def push_front_frames():
    steps = [
        "pushFront(x): нов възел",
        "1. node->next = head_   (новият сочи стария първи)",
        "2. head_ = node   (списъкът започва от новия) — Θ(1), без значение колко е дълъг",
    ]
    for k, caption in enumerate(steps):
        s = Svg(1000, 300, f"pushFront, стъпка {k + 1}")
        s.text(40, 44, "pushFront: два указателя", size=22, weight=700)
        xs = [320, 500, 680]
        for i, x in enumerate(xs):
            p = node(s, x, 150, "abc"[i], null=(i == 2))
            if i < 2:
                arrow(s, p, (xs[i + 1] - 2, 172))
        newp = node(s, 100, 150, "x", fill=TINT3, stroke=S3, null=(k == 0))
        if k >= 1:
            arrow(s, newp, (318, 172), col=S3, sw=2)
        if k < 2:
            pointer_label(s, 360, 100, "head_", (360, 146))
        else:
            pointer_label(s, 146, 100, "head_", (146, 146), col=S3)
        s.text(40, 270, caption, size=17, weight=600)
        s.save(OUT / f"push-front-{k + 1}.svg")


def insert_frames():
    steps = [
        ("insertAt(2, x): стигаме до prev = възел 1 — Θ(i)", 0),
        ("1. node = new Node{x}", 1),
        ("2. node->next = prev->next   — ПЪРВО новият хваща продължението", 2),
        ("3. prev->next = node   — после prev сочи новия", 3),
    ]
    for k, (caption, stage) in enumerate(steps):
        s = Svg(1000, 330, f"Вмъкване след възел, стъпка {k + 1}")
        s.text(40, 44, "Вмъкване в средата: редът на присвояванията е важен", size=22, weight=700)
        xs = [80, 260, 620, 800]
        vals = ["a", "b", "c", "d"]
        anchors = []
        for i, (x, v) in enumerate(zip(xs, vals)):
            fill, stroke = (TINT2, S2) if i == 1 else (TINT1, S1)
            anchors.append(node(s, x, 120, v, fill=fill, stroke=stroke, null=(i == 3),
                                label="prev" if i == 1 else None))
        arrow(s, anchors[0], (258, 142))
        arrow(s, anchors[2], (798, 142))
        if stage < 3:
            arrow(s, anchors[1], (618, 142))
        if stage >= 1:
            p = node(s, 440, 220, "x", fill=TINT3, stroke=S3, null=(stage < 2))
            if stage >= 2:
                arrow(s, p, (640, 166), col=S3, sw=2)
            if stage >= 3:
                arrow(s, anchors[1], (460, 218), col=S3, sw=2)
        s.text(40, 310, caption, size=17, weight=600)
        s.save(OUT / f"insert-{k + 1}.svg")


def remove_frames():
    steps = [
        ("removeAt(2): prev = възел 1, victim = prev->next", 0),
        ("1. prev->next = victim->next   — victim вече не е в списъка", 1),
        ("2. delete victim   (и ако victim == tail_: tail_ = prev)", 2),
    ]
    for k, (caption, stage) in enumerate(steps):
        s = Svg(1000, 300, f"Премахване на възел, стъпка {k + 1}")
        s.text(40, 44, "Премахване: трябва ни предишният възел", size=22, weight=700)
        xs = [80, 280, 480, 680]
        anchors = []
        for i, x in enumerate(xs):
            if i == 2 and stage == 2:
                s.rect(x, 140, NW, NH, SURFACE, GRID, rx=6, dash="5 4")
                s.text(x + NW / 2, 168, "delete", size=14, anchor="middle", fill=MUTED)
                anchors.append(None)
                continue
            fill, stroke = (TINT2, S2) if i == 1 else ((TINT_RED, RED) if i == 2 else (TINT1, S1))
            label = "prev" if i == 1 else ("victim" if i == 2 else None)
            anchors.append(node(s, x, 140, "abcd"[i], fill=fill, stroke=stroke, null=(i == 3), label=label))
        arrow(s, anchors[0], (278, 162))
        if stage == 0:
            arrow(s, anchors[1], (478, 162))
        else:
            s.path(f"M {anchors[1][0]},{anchors[1][1]} C {anchors[1][0] + 80},90 600,90 690,136", S3, sw=2,
                   arrow=True)
        if anchors[2] is not None:
            arrow(s, anchors[2], (678, 162), dash="4 4" if stage else None)
        s.text(40, 270, caption, size=17, weight=600)
        s.save(OUT / f"remove-{k + 1}.svg")


def reverse_frames():
    n = 4
    frames = []
    for done in range(n + 1):
        frames.append(done)
    for k, done in enumerate(frames):
        s = Svg(1000, 330, f"Обръщане на списък, стъпка {k + 1}")
        s.text(40, 44, "reverse(): три указателя, едно минаване", size=22, weight=700)
        xs = [220, 400, 580, 760]
        anchors = [node(s, x, 140, "abcd"[i], null=(i == 0 and done >= 1) or (i == 3 and done < 4),
                        fill=TINT3 if i < done else TINT1, stroke=S3 if i < done else S1)
                   for i, x in enumerate(xs)]
        for i in range(n - 1):
            if i < done - 1:   # already reversed: i+1 -> i
                s.path(f"M {xs[i + 1] + 10},{186} C {xs[i + 1] - 40},230 {xs[i] + 120},230 {xs[i] + 75},188", S3,
                       sw=2, arrow=True)
            elif i >= done:     # still forward
                arrow(s, anchors[i], (xs[i + 1] - 2, 162))
        labels = []
        prev = done - 1
        cur = done
        if prev >= 0:
            labels.append(("prev", xs[prev] + NW / 2))
        else:
            labels.append(("prev = nullptr", 40))
        if cur < n:
            labels.append(("cur", xs[cur] + NW / 2))
            if cur + 1 < n:
                labels.append(("next", xs[cur + 1] + NW / 2))
        for name, x in labels:
            s.text(x, 112, name, size=14, family=MONO, weight=700, anchor="middle" if "=" not in name else "start")
        if done < n:
            cap = (f"стъпка {done + 1}: next = cur->next; cur->next = prev; prev = cur; cur = next"
                   if done < n else "")
        else:
            cap = "cur == nullptr → head_ = prev. Θ(n) време, Θ(1) памет, нито един нов възел."
        s.text(40, 300, cap, size=16, weight=600)
        s.save(OUT / f"reverse-{k + 1}.svg")


def list_kinds():
    s = Svg(1100, 520, "Видове свързани списъци")
    s.text(40, 46, "Видове свързани списъци", size=24, weight=700)
    rows = [
        (100, "едносвързан"),
        (210, "двусвързан (седмица 06)"),
        (320, "цикличен едносвързан"),
        (430, "с фиктивен възел (sentinel)"),
    ]
    for y, title in rows:
        s.text(40, y - 12, title, size=16, weight=600)
    # singly
    xs = [60, 220, 380]
    for i, x in enumerate(xs):
        p = node(s, x, 100, "abc"[i], null=(i == 2))
        if i < 2:
            arrow(s, p, (xs[i + 1] - 2, 122))
    s.text(560, 128, "напред; вмъкване/изтриване след даден възел — Θ(1)", size=14, fill=INK2)
    # doubly
    for i, x in enumerate(xs):
        s.rect(x, 210, 120, NH, TINT2, S2, rx=6, sw=1.6)
        s.line(x + 26, 210, x + 26, 254, stroke=S2, sw=1.2)
        s.line(x + 94, 210, x + 94, 254, stroke=S2, sw=1.2)
        s.text(x + 60, 238, "abc"[i], size=16, anchor="middle", family=MONO, weight=600)
        if i < 2:
            arrow(s, (x + 107, 222), (xs[i + 1] - 2, 222))
            arrow(s, (xs[i + 1] + 13, 244), (x + 122, 244))
    s.text(560, 238, "в двете посоки; изтриване на даден възел — Θ(1)", size=14, fill=INK2)
    # circular
    for i, x in enumerate(xs):
        p = node(s, x, 320, "abc"[i], fill=TINT3, stroke=S3)
        if i < 2:
            arrow(s, p, (xs[i + 1] - 2, 342))
    s.path("M 455,342 C 520,342 520,392 300,392 C 80,392 30,392 58,350", S3, sw=1.6, arrow=True)
    s.text(560, 348, "последният сочи първия; удобен за „кръгове“ (Йосиф, задача 6)", size=14, fill=INK2)
    # sentinel
    s.rect(60, 430, NW, NH, PANEL, MUTED, rx=6, dash="5 4")
    s.text(90, 458, "—", size=16, anchor="middle", family=MONO, fill=MUTED)
    s.circle(135, 452, 3.5, INK2, stroke=INK2, sw=0)
    arrow(s, (135, 452), (218, 452))
    for i, x in enumerate([220, 380]):
        p = node(s, x, 430, "ab"[i], null=(i == 1))
        if i < 1:
            arrow(s, p, (378, 452))
    s.text(560, 458, "винаги има „възел преди първия“ → без специален случай за началото", size=14, fill=INK2)
    s.save(OUT / "list-kinds.svg")


def floyd():
    s = Svg(1000, 420, "Алгоритъмът на Флойд: костенурката и заекът")
    s.text(40, 46, "Има ли цикъл? Костенурката и заекът (Floyd)", size=24, weight=700)
    tail = [(80, 215), (230, 215), (380, 215)]
    cx, cy, r = 600, 215, 115
    ring = []
    m = 8
    for j in range(m):
        a = math.pi + 2 * math.pi * j / m
        ring.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    pts = tail + ring
    for i, (x, y) in enumerate(pts):
        s.circle(x, y, 22, TINT1, stroke=S1, sw=1.6)
        s.text(x, y + 6, str(i), size=15, anchor="middle", family=MONO, weight=600)
    order = list(range(len(pts))) + [3]
    for a, b in zip(order, order[1:]):
        (x1, y1), (x2, y2) = pts[a], pts[b]
        d = math.hypot(x2 - x1, y2 - y1)
        ux, uy = (x2 - x1) / d, (y2 - y1) / d
        s.line(x1 + ux * 24, y1 + uy * 24, x2 - ux * 26, y2 - uy * 26, stroke=INK2, sw=1.5, arrow=True)
    slow_i, fast_i = 6, 9
    for idx, lab, col in [(slow_i, "slow", S3), (fast_i, "fast", S2)]:
        x, y = pts[idx]
        s.circle(x, y, 22, col, stroke=col, sw=1.6)
        s.text(x, y + 6, str(idx), size=15, anchor="middle", family=MONO, weight=700, fill=SURFACE)
        if y < cy:
            s.text(x, y - 32, lab, size=14, anchor="middle", weight=700, family=MONO)
        else:
            s.text(x + 32, y + 6, lab, size=14, anchor="start", weight=700, family=MONO)
    s.text(40, 380, "slow прави 1 стъпка, fast — 2. Ако няма цикъл, fast стига nullptr. Ако има — вътре в цикъла",
           size=15, fill=INK2)
    s.text(40, 404, "fast настига slow с по една позиция на стъпка, значи го застига за ≤ дължината на цикъла. Θ(1) памет.",
           size=15, fill=INK2)
    s.save(OUT / "floyd.svg")


def node_memory():
    s = Svg(1000, 330, "Колко памет струва един int в списък и във вектор")
    s.text(40, 46, "Цената на един int: 4 байта във вектор, 32 в списък", size=24, weight=700)
    s.text(40, 96, "std::vector<int>", size=15, family=MONO, weight=600)
    for i in range(8):
        s.rect(40 + i * 44, 108, 40, 36, TINT1, S1, rx=4)
    s.text(40 + 8 * 44 + 10, 132, "4 B на елемент, един до друг", size=14, fill=INK2)
    s.text(40, 190, "Node<int> в heap-а (glibc, x86-64)", size=15, family=MONO, weight=600)
    parts = [(8, "заглавие на malloc", PANEL, GRID), (4, "value", TINT1, S1), (4, "подравняване", PANEL, GRID),
             (8, "next", TINT2, S2), (8, "неизползвани", PANEL, GRID)]
    x = 40
    for size, lab, fill, stroke in parts:
        w = size * 26
        s.rect(x, 202, w - 3, 40, fill, stroke, rx=4)
        s.text(x + (w - 3) / 2, 227, f"{size} B", size=13, anchor="middle", family=MONO)
        s.text(x + (w - 3) / 2, 262, lab, size=12, anchor="middle", fill=INK2)
        x += w
    s.text(x + 14, 227, "= 32 B", size=16, weight=700, family=MONO)
    s.text(40, 306, "sizeof(Node<int>) = 16, но malloc заделя поне 32 байта на блок. 8× повече памет — и 8× по-малко",
           size=15, fill=INK2)
    s.text(40, 326, "елементи в една кеш линия.", size=15, fill=INK2)
    s.save(OUT / "node-memory.svg")


def pointer_to_pointer():
    s = Svg(1000, 300, "Указател към указател: link")
    s.text(40, 46, "🔬 Node** link: един код за началото и за средата", size=22, weight=700)
    s.rect(60, 140, 100, 44, PANEL, MUTED, rx=6)
    s.text(110, 168, "head_", size=15, anchor="middle", family=MONO, weight=600)
    xs = [260, 440, 620]
    anchors = []
    for i, x in enumerate(xs):
        anchors.append(node(s, x, 140, "abc"[i], null=(i == 2)))
        if i < 2:
            arrow(s, anchors[-1], (xs[i + 1] - 2, 162))
    arrow(s, (160, 162), (258, 162))
    s.text(110, 112, "link = &head_", size=14, anchor="middle", family=MONO, weight=700)
    s.text(xs[0] + 75, 112, "link = &a->next", size=14, anchor="middle", family=MONO, weight=700)
    s.text(40, 240, "*link е указателят, който води към текущия възел. Премахването е винаги *link = cur->next —",
           size=15, fill=INK2)
    s.text(40, 262, "без значение дали *link е head_ или next на предишния. Linus Torvalds нарича това „добър вкус“.",
           size=15, fill=INK2)
    s.save(OUT / "pointer-to-pointer.svg")


def josephus_circle():
    n, k = 7, 3
    order = []
    people = list(range(1, n + 1))
    idx = 0
    while len(people) > 1:
        idx = (idx + k - 1) % len(people)
        order.append(people.pop(idx))
    survivor = people[0]
    s = Svg(1000, 430, "Задачата на Йосиф за n = 7, k = 3")
    s.text(40, 46, f"Йосиф: n = {n}, k = {k}", size=24, weight=700)
    cx, cy, r = 260, 250, 115
    for i in range(n):
        a = -math.pi / 2 + 2 * math.pi * i / n
        x, y = cx + r * math.cos(a), cy + r * math.sin(a)
        person = i + 1
        if person == survivor:
            fill, stroke, tf = S3, S3, SURFACE
        else:
            fill, stroke, tf = TINT_RED, RED, INK
        s.circle(x, y, 26, fill, stroke=stroke, sw=2)
        s.text(x, y + 6, str(person), size=17, anchor="middle", weight=700, fill=tf)
        if person != survivor:
            rank = order.index(person) + 1
            s.text(cx + (r + 50) * math.cos(a), cy + (r + 50) * math.sin(a) + 5, f"#{rank}", size=14,
                   anchor="middle", fill=INK2, weight=600)
    s.text(520, 130, "Ред на излизане:", size=16, weight=600)
    s.text(520, 160, " → ".join(map(str, order)), size=18, family=MONO)
    s.text(520, 200, f"Оцелява: {survivor}", size=18, weight=700)
    s.text(520, 250, "Симулация с цикличен списък: Θ(n·k).", size=15, fill=INK2)
    s.text(520, 274, "Рекурентна формула: J(1) = 0,", size=15, fill=INK2)
    s.text(520, 298, "J(n) = (J(n−1) + k) mod n → Θ(n).", size=15, fill=INK2)
    s.save(OUT / "josephus.svg")


def front_chart():
    rows = [r for r in csv.DictReader((HERE / "bench_sample.csv").open()) if r["part"] == "front"]
    sums = [r for r in csv.DictReader((HERE / "bench_sample.csv").open()) if r["part"] == "sum"]
    s = Svg(1100, 520, "Вмъкване в началото: списък, вектор, дек")
    s.text(40, 44, "n вмъквания в началото", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина. Списъкът е Θ(1) на вмъкване, vector::insert(begin) — Θ(n).",
           size=15, fill=INK2)
    x0, y0, w, h = 110, 100, 600, 330
    fx = lambda n: x0 + (math.log2(n) - 10) / 10 * w
    fy = lambda ms: y0 + h - (math.log10(ms) + 3) / 6 * h
    axes(s, x0, y0, w, h, [2 ** k for k in range(10, 21, 2)], [0.001, 0.01, 0.1, 1, 10, 100, 1000],
         "n (лог. ос)", "ms (лог. ос)", fx, fy, xfmt=lambda n: f"2^{int(math.log2(n))}", yfmt=lambda v: f"{v:g}")
    series = [(S2, "vector::insert(begin)", "vector_ms"), (S1, "SinglyLinkedList::pushFront", "list_ms"),
              (S3, "std::deque::push_front", "deque_ms")]
    for col, lab, key in series:
        pts = [(fx(int(r["n"])), fy(float(r[key]))) for r in rows if float(r[key]) > 0]
        s.path(polyline(pts), col, sw=2)
        for x, y in pts:
            s.circle(x, y, 4, col)
    legend(s, 750, 130, [(c, l) for c, l, _ in series])
    sl = float(sums[-1]["list_ms"])
    sv = float(sums[-1]["vector_ms"])
    s.text(750, 230, "Но обхождането (сума):", size=14, weight=600)
    s.text(750, 252, f"списък  {sl:.2f} ns на елемент", size=14, family=MONO)
    s.text(750, 274, f"vector  {sv:.2f} ns на елемент", size=14, family=MONO)
    s.text(750, 296, f"→ ×{sl / sv:.0f} в полза на вектора", size=14, weight=600)
    s.text(750, 340, "std::deque е и Θ(1) в началото,", size=14, fill=INK2)
    s.text(750, 360, "и ~×17 по-бърз от списъка:", size=14, fill=INK2)
    s.text(750, 380, "алокира блокове, не по един", size=14, fill=INK2)
    s.text(750, 400, "възел (седмица 07).", size=14, fill=INK2)
    s.save(OUT / "front-chart.svg")


if __name__ == "__main__":
    anatomy()
    push_front_frames()
    insert_frames()
    remove_frames()
    reverse_frames()
    list_kinds()
    floyd()
    node_memory()
    pointer_to_pointer()
    josephus_circle()
    front_chart()
