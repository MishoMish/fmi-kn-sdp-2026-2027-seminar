#!/usr/bin/env python3
"""Generates every SVG figure for week 07.

Run from anywhere:  python3 weeks/07-proxy-stack-queue/img/src/make_figures.py
The chart reads bench_sample.csv (`w07_bench --csv`) and mask_sample.csv
(the same benchmark with a power-of-two mask instead of % in RingQueue).
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


def cell(s, x, y, w, h, text, fill=TINT1, stroke=S1, tf=INK, size=16):
    s.rect(x, y, w, h, fill, stroke, rx=6, sw=1.5)
    if text != "":
        s.text(x + w / 2, y + h / 2 + 6, str(text), size=size, anchor="middle", family=MONO, weight=600, fill=tf)


# ---------------------------------------------------------------- concepts

def lifo_fifo():
    s = Svg(1100, 400, "Стек (LIFO) и опашка (FIFO)")
    s.text(40, 46, "Стек: последен влязъл — пръв излязъл. Опашка: пръв влязъл — пръв излязъл.", size=21,
           weight=700)
    # stack
    s.text(170, 96, "стек", size=18, weight=700, anchor="middle")
    for i, v in enumerate(["A", "B", "C", "D"]):
        cell(s, 110, 300 - i * 50, 120, 44, v, fill=TINT1 if i < 3 else S1, stroke=S1,
             tf=SURFACE if i == 3 else INK)
    s.path("M 110,120 L 110,346 L 230,346 L 230,120", MUTED, sw=2)
    s.path("M 300,170 C 270,170 260,160 240,160", S3, sw=2, arrow=True)
    s.text(310, 175, "push", size=15, family=MONO, weight=600)
    s.path("M 240,180 C 270,190 290,210 300,214", S2, sw=2, arrow=True)
    s.text(310, 222, "pop / top", size=15, family=MONO, weight=600)
    s.text(170, 380, "един край: top", size=14, fill=INK2, anchor="middle")
    # queue
    s.text(760, 96, "опашка", size=18, weight=700, anchor="middle")
    for i, v in enumerate(["A", "B", "C", "D"]):
        cell(s, 560 + i * 100, 200, 90, 50, v, fill=S1 if i == 0 else TINT1, stroke=S1,
             tf=SURFACE if i == 0 else INK)
    s.line(560, 194, 950, 194, stroke=MUTED, sw=2)
    s.line(560, 256, 950, 256, stroke=MUTED, sw=2)
    s.line(1050, 225, 958, 225, stroke=S3, sw=2, arrow=True)
    s.text(1000, 214, "push", size=15, family=MONO, weight=600, anchor="middle")
    s.line(552, 225, 470, 225, stroke=S2, sw=2, arrow=True)
    s.text(505, 214, "pop", size=15, family=MONO, weight=600, anchor="middle")
    s.text(605, 290, "front", size=14, family=MONO, anchor="middle", fill=INK2)
    s.text(905, 290, "back", size=14, family=MONO, anchor="middle", fill=INK2)
    s.text(760, 380, "два края: влиза отзад, излиза отпред", size=14, fill=INK2, anchor="middle")
    s.save(OUT / "lifo-fifo.svg")


def adaptor():
    s = Svg(1000, 340, "Адаптер: стек върху вектор")
    s.text(40, 46, "Адаптер: стекът скрива всичко, което векторът може", size=22, weight=700)
    s.rect(60, 90, 880, 200, PANEL, GRID, rx=12)
    s.text(84, 120, "Stack<T, std::vector<T>>", size=16, weight=700, family=MONO)
    for k, (op, real) in enumerate([("push(x)", "c_.push_back(x)"), ("pop()", "c_.pop_back()"),
                                    ("top()", "c_.back()")]):
        y = 140 + k * 44
        s.rect(84, y, 180, 34, TINT3, S3, rx=6)
        s.text(174, y + 23, op, size=15, anchor="middle", family=MONO, weight=600)
        s.line(268, y + 17, 390, y + 17, stroke=INK2, sw=1.4, arrow=True)
        s.text(400, y + 23, real, size=15, family=MONO)
    s.rect(620, 136, 290, 136, SURFACE, MUTED, rx=8, dash="5 4")
    s.text(765, 162, "private: Container c_", size=14, anchor="middle", family=MONO, weight=600)
    for k, hidden in enumerate(["operator[]", "insert / erase", "begin / end"]):
        s.text(765, 192 + k * 24, hidden, size=14, anchor="middle", family=MONO, fill=MUTED)
        s.line(700, 187 + k * 24, 830, 187 + k * 24, stroke=RED, sw=1.2)
    s.text(40, 320, "Същото за std::stack, std::queue (върху deque) и std::priority_queue (върху vector, седмица 11).",
           size=15, fill=INK2)
    s.save(OUT / "adaptor.svg")


def ring_frames():
    cap = 8
    frames = [
        ("празен, capacity 8", 0, 0, {}),
        ("push a, b, c, d, e", 0, 5, {0: "a", 1: "b", 2: "c", 3: "d", 4: "e"}),
        ("pop ×3: head_ се мести, нищо не се копира", 3, 2, {3: "d", 4: "e"}),
        ("push f, g, h, i: стигаме края — продължаваме от 0", 3, 6, {3: "d", 4: "e", 5: "f", 6: "g", 7: "h", 0: "i"}),
        ("push j, k: пълен (head_ = 3, size_ = 8)", 3, 8,
         {3: "d", 4: "e", 5: "f", 6: "g", 7: "h", 0: "i", 1: "j", 2: "k"}),
    ]
    for k, (caption, head, size, contents) in enumerate(frames):
        s = Svg(1000, 290, f"Кръгов буфер, стъпка {k + 1}")
        s.text(40, 44, "Кръгов буфер: позиция i е на индекс (head_ + i) % capacity_", size=20, weight=700)
        for i in range(cap):
            v = contents.get(i, "")
            logical = (i - head) % cap
            is_front = size > 0 and i == head
            fill = S1 if is_front else (TINT1 if v else PANEL)
            cell(s, 60 + i * 110, 110, 100, 52, v, fill=fill, stroke=S1 if v else GRID,
                 tf=SURFACE if is_front else INK)
            s.text(110 + i * 110, 182, str(i), size=13, anchor="middle", fill=MUTED, family=MONO)
            if v:
                s.text(110 + i * 110, 100, f"#{logical}", size=12, anchor="middle", fill=INK2, family=MONO)
        if size:
            s.text(110 + head * 110, 210, "head_", size=14, anchor="middle", family=MONO, weight=700)
        if size < cap:
            nxt = (head + size) % cap
            s.text(110 + nxt * 110, 230, "следващ push", size=13, anchor="middle", fill=S3, weight=600)
        suffix = "" if "size_" in caption else f"   (size_ = {size})"
        s.text(40, 268, caption + suffix, size=16, weight=600)
        s.save(OUT / f"ring-{k + 1}.svg")


def ring_grow():
    s = Svg(1100, 340, "Растеж на кръгов буфер: разгъване")
    s.text(40, 44, "Растеж при пълен буфер: копираме в логически ред — „разгъваме“", size=20, weight=700)
    old = ["i", "j", "k", "d", "e", "f", "g", "h"]
    s.text(40, 92, "стар (head_ = 3)", size=14, fill=MUTED, weight=600)
    for i, v in enumerate(old):
        cell(s, 40 + i * 64, 102, 58, 44, v, fill=S1 if i == 3 else TINT1, tf=SURFACE if i == 3 else INK)
    s.text(40, 280, "нов, capacity 16 (head_ = 0)", size=14, fill=MUTED, weight=600)
    order = old[3:] + old[:3]
    for i in range(16):
        v = order[i] if i < 8 else ""
        cell(s, 40 + i * 64, 210, 58, 44, v, fill=TINT3 if v else PANEL, stroke=S3 if v else GRID)
    for logical in range(8):
        src = (3 + logical) % 8
        s.line(40 + src * 64 + 29, 150, 40 + logical * 64 + 29, 206, stroke=S3 if logical < 5 else S2, sw=1.4,
               arrow=True)
    s.text(40, 320, "Копираме data[(head_ + i) % capacity] → fresh[i]. Нов елемент отива на индекс 8.", size=15,
           fill=INK2)
    s.save(OUT / "ring-grow.svg")


def two_stacks_frames():
    frames = [
        ("push 1, 2, 3 → в in", [1, 2, 3], [], None),
        ("front(): out е празен → преместваме всичко от in в out (обърнато!)", [], [3, 2, 1], "move"),
        ("pop() → от out (1 излиза пръв); push 4 → в in", [4], [3, 2], None),
        ("pop(), pop() → 2, 3; out е празен → следващият pop премества 4", [], [4], "move"),
    ]
    for k, (caption, ins, outs, mark) in enumerate(frames):
        s = Svg(1000, 330, f"Опашка с два стека, стъпка {k + 1}")
        s.text(40, 44, "Опашка от два стека", size=22, weight=700)
        for col, (title, items, x) in enumerate([("in (push тук)", ins, 180), ("out (front / pop оттук)", outs, 620)]):
            s.text(x + 60, 86, title, size=15, weight=600, anchor="middle")
            s.path(f"M {x},{100} L {x},{280} L {x + 120},{280} L {x + 120},{100}", MUTED, sw=2)
            for i, v in enumerate(items):
                top = i == len(items) - 1
                cell(s, x + 6, 236 - i * 46, 108, 40, v, fill=(S3 if col == 0 else S1) if top else
                     (TINT3 if col == 0 else TINT1), stroke=S3 if col == 0 else S1, tf=SURFACE if top else INK)
        if mark:
            s.path("M 320,190 C 420,150 500,150 600,190", S2, sw=2.4, arrow=True)
            s.text(460, 140, "по един, обърнато", size=14, anchor="middle", fill=S2, weight=600)
        s.text(40, 312, caption, size=15, weight=600)
        s.save(OUT / f"two-stacks-{k + 1}.svg")


def call_stack():
    s = Svg(1000, 360, "Стекът на извикванията при рекурсия")
    s.text(40, 46, "Стекът на извикванията — стекът, който ползвате всеки ден", size=22, weight=700)
    s.text(60, 96, "int fact(int n) { return n <= 1 ? 1 : n * fact(n - 1); }", size=16, family=MONO)
    frames = ["main()", "fact(4)", "fact(3)", "fact(2)", "fact(1)"]
    for i, f in enumerate(frames):
        y = 300 - i * 42
        top = i == len(frames) - 1
        cell(s, 120, y, 220, 36, f, fill=S1 if top else TINT1, tf=SURFACE if top else INK, size=15)
    s.text(380, 132, "← връх: изпълнява се сега", size=14, fill=INK2)
    s.text(380, 320, "← дъно", size=14, fill=INK2)
    s.text(560, 170, "Всяко извикване = push на кадър", size=15)
    s.text(560, 194, "(аргументи, локални, адрес за връщане).", size=15, fill=INK2)
    s.text(560, 230, "Всяко return = pop.", size=15)
    s.text(560, 266, "Дълбочина n → n кадъра: затова", size=15, fill=INK2)
    s.text(560, 290, "рекурсивният delete на милион", size=15, fill=INK2)
    s.text(560, 314, "възела препълва (седмица 05).", size=15, fill=INK2)
    s.save(OUT / "call-stack.svg")


def brackets_trace():
    text = "( [ ] { ( ) } )"
    steps = [("(", ["("]), ("[", ["(", "["]), ("]", ["("]), ("{", ["(", "{"]), ("(", ["(", "{", "("]),
             (")", ["(", "{"]), ("}", ["("]), (")", [])]
    s = Svg(1100, 300, "Проверка на скоби със стек")
    s.text(40, 46, "Скоби: отваряща → push; затваряща → трябва да съвпада с върха → pop", size=20, weight=700)
    for k, (ch, stack) in enumerate(steps):
        x = 60 + k * 128
        s.text(x + 50, 92, ch, size=22, family=MONO, weight=700, anchor="middle",
               fill=S3 if ch in "([{" else S2)
        for i, b in enumerate(stack):
            cell(s, x + 15, 216 - i * 36, 70, 32, b, fill=TINT1, size=15)
        s.path(f"M {x + 10},110 L {x + 10},250 L {x + 90},250 L {x + 90},110", MUTED, sw=1.5)
    s.text(40, 284, "Накрая стекът е празен → балансирани. „([)]“: при ) върхът е [ → не съвпада → false.", size=15,
           fill=INK2)
    s.save(OUT / "brackets.svg")


# ---------------------------------------------------------------- proxy

def proxy_pattern():
    s = Svg(1100, 360, "Шаблонът Proxy")
    s.text(40, 46, "Proxy: заместител със същия интерфейс, който контролира достъпа", size=22, weight=700)
    s.rect(60, 150, 180, 70, PANEL, GRID, rx=10)
    s.text(150, 192, "клиент", size=17, anchor="middle", weight=700)
    s.rect(420, 130, 240, 110, TINT2, S2, rx=10, sw=2)
    s.text(540, 170, "Proxy", size=18, anchor="middle", weight=700)
    s.text(540, 196, "същият интерфейс", size=14, anchor="middle", fill=INK2)
    s.text(540, 218, "+ проверка / отлагане / брояч", size=13, anchor="middle", fill=INK2)
    s.rect(840, 150, 200, 70, TINT1, S1, rx=10)
    s.text(940, 192, "истинският обект", size=16, anchor="middle", weight=700)
    s.line(244, 185, 414, 185, stroke=INK2, sw=2, arrow=True)
    s.line(664, 185, 834, 185, stroke=INK2, sw=2, arrow=True, dash="6 5")
    s.text(330, 172, "вика", size=14, anchor="middle", fill=INK2)
    s.text(750, 172, "препраща (може и да не)", size=14, anchor="middle", fill=INK2)
    kinds = [("виртуален", "създава скъпия обект едва при нужда"), ("защитен", "проверява права преди достъп"),
             ("отдалечен", "обектът е на друга машина"), ("умна референция", "брои, заключва, освобождава")]
    for k, (name, desc) in enumerate(kinds):
        x = 60 + k * 255
        s.text(x, 290, name, size=15, weight=700)
        s.text(x, 312, desc, size=13, fill=INK2)
    s.save(OUT / "proxy-pattern.svg")


def bitref():
    s = Svg(1100, 360, "BitRef: прокси за един бит")
    s.text(40, 46, "v[70] = true; — operator[] връща прокси, не bool&", size=22, weight=700)
    s.text(40, 92, "words_[0]", size=14, family=MONO, fill=MUTED)
    s.text(560, 92, "words_[1]  (битове 64…127)", size=14, family=MONO, fill=MUTED)
    for w in range(2):
        for b in range(16):
            x = 40 + w * 520 + b * 31
            hot = (w == 1 and b == 6)
            s.rect(x, 104, 28, 34, S2 if hot else PANEL, S2 if hot else GRID, rx=3)
            s.text(x + 14, 127, "1" if hot else "0", size=13, anchor="middle", family=MONO,
                   fill=SURFACE if hot else INK2)
        s.text(40 + w * 520 + 16 * 31 + 4, 127, "…", size=16)
    s.rect(560, 200, 330, 90, TINT2, S2, rx=10, sw=2)
    s.text(580, 230, "BitRef", size=16, weight=700, family=MONO)
    s.text(580, 254, "word_ = &words_[70 / 64]", size=14, family=MONO)
    s.text(580, 276, "mask_ = 1 << (70 % 64)", size=14, family=MONO)
    s.path("M 600,200 C 600,170 610,150 752,142", S2, sw=2, arrow=True)
    s.text(40, 230, "operator bool()   → (*word_ & mask_) != 0", size=15, family=MONO)
    s.text(40, 256, "operator=(bool)   → *word_ |= mask_  /  &= ~mask_", size=15, family=MONO)
    s.text(40, 282, "operator=(BitRef) → копира СТОЙНОСТТА на бита", size=15, family=MONO)
    s.text(40, 336, "Бит няма адрес → няма bool&. Проксито „се държи като референция“ — точно както std::vector<bool>.",
           size=15, fill=INK2)
    s.save(OUT / "bitref.svg")


# ---------------------------------------------------------------- results

def queue_chart():
    rows = {int(r["n"]): r for r in csv.DictReader((HERE / "bench_sample.csv").open())}
    mask = {int(r["n"]): r for r in csv.DictReader((HERE / "mask_sample.csv").open())}
    n = 1000
    bars = [
        ("RingQueue с & (маска)", float(mask[n]["ring"]), S3),
        ("std::queue<deque>", float(rows[n]["std_deque"]), S5),
        ("TwoStackQueue", float(rows[n]["two_stacks"]), S4),
        ("RingQueue с %", float(rows[n]["ring"]), S1),
        ("std::queue<list>", float(rows[n]["std_list"]), S2),
        ("vector + erase(begin)", float(rows[n]["vector_erase"]), RED),
    ]
    s = Svg(1100, 470, "Пет опашки при еднакво натоварване")
    s.text(40, 44, "ns на (pop + push), опашка с 1000 елемента", size=24, weight=700)
    s.text(40, 70, "Release, примерна машина. Дължината на стълбчето е в логаритмична скала.", size=15, fill=INK2)
    vmax = max(v for _, v, _ in bars)
    for k, (lab, v, col) in enumerate(bars):
        y = 100 + k * 54
        s.text(40, y + 26, lab, size=15, family=MONO)
        bw = max(4, (math.log10(v) + 1) / (math.log10(vmax) + 1) * 520)
        s.rect(330, y + 8, bw, 28, col, rx=4)
        s.text(330 + bw + 10, y + 28, f"{v:.2f}" if v < 10 else f"{v:.1f}", size=15, weight=700)
    r_pct = float(rows[n]["ring"])
    r_and = float(mask[n]["ring"])
    s.text(40, 432, f"Една промяна — % на & — прави кръговия буфер {r_pct / r_and:.0f}× по-бърз: делението струва "
           f"десетки такта, маската — един.", size=15, fill=INK2)
    s.text(40, 456, "(Стълбчето „с & (маска)“ е от отделно пускане на същия бенчмарк с тази единствена промяна.)",
           size=12, fill=MUTED)
    s.save(OUT / "queue-chart.svg")


if __name__ == "__main__":
    lifo_fifo()
    adaptor()
    ring_frames()
    ring_grow()
    two_stacks_frames()
    call_stack()
    brackets_trace()
    proxy_pattern()
    bitref()
    queue_chart()
