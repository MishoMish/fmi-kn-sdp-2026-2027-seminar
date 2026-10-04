<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 10 · 10 декември</p>

# Дървета II: балансирани дървета

Два указателя на ротация — и $\Theta(\log n)$ за всеки вход.

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: BST, обхождания, изтриване |
| 15 мин | Преговор: ротации, AVL инвариант, четирите случая, изтриване |
| 20 мин | На живо: от BST до AVL — височина, две ротации, `rebalance` |
| ☕ | почивка |
| 35 мин | Практика: ротации, `rebalance`, `insert`, `erase`, `rank` / `select` |
| 10 мин | Червено-черни, B-дървета и какво следва |

---

## Загрявка

1. Каква е височината на BST след вмъкване на 1, 2, …, 1000 по ред? <span class="fragment answer">999 — свързан списък</span>
2. При изтриване на възел с две деца — кой заема мястото му? <span class="fragment answer">наследникът — най-левият в дясното поддърво</span>
3. Защо миналата седмица `inorderIterative` беше без рекурсия? <span class="fragment answer">изродено дърво с $10^6$ нива препълва стека</span>
4. Изходният въпрос: веригата 1 → 2 → 3. Как 2 да стане корен? <span class="fragment answer">една лява ротация около 1 → днес</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · ротации, AVL</p>

---

## Ротация

![Ротация](img/rotation.svg)

---

## AVL: $\lvert h(\text{ляво}) - h(\text{дясно}) \rvert \le 1$ за всеки възел

![Най-тънките AVL дървета](img/fib-trees.svg)

---

## Четирите случая

![Четирите случая](img/avl-cases.svg)

---

## Двойна ротация: защо две

<div class="r-stack">
<img src="img/lr-1.svg" alt="LR начало" class="fragment fade-out" data-fragment-index="0">
<img src="img/lr-2.svg" alt="LR стъпка 1" class="fragment current-visible" data-fragment-index="0">
<img src="img/lr-3.svg" alt="LR стъпка 2" class="fragment" data-fragment-index="1">
</div>

---

## Изтриване: ротации по целия път

![Каскада](img/erase-cascade.svg)

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: от BST до AVL

<p class="small">20 минути · миналата седмица + 3 функции</p>

---

## Всяка функция връща новия корен

```cpp
Node* insert(Node* n, int x) {
    if (!n) return new Node{x};
    if (x < n->value) n->left = insert(n->left, x);
    else if (n->value < x) n->right = insert(n->right, x);
    else return n;
    return rebalance(n);          // the only new line
}
```

```cpp
Node* rebalance(Node* n) {
    update(n);
    if (balance(n) > 1) {
        if (balance(n->left) < 0) n->left = rotateLeft(n->left);    // LR -> LL
        return rotateRight(n);
    }
    if (balance(n) < -1) {
        if (balance(n->right) > 0) n->right = rotateRight(n->right); // RL -> RR
        return rotateLeft(n);
    }
    return n;
}
```

---

## 1, 2, …, 7 — най-лошият вход от миналата седмица

![Вмъкване на 1…7](img/avl-sorted.svg)

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 10 е в <code>weeks/10-trees-2</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w10/"</code></p>

---

## Задачи 1–3

- **★ `update`, `rotateRight`, `rotateLeft`** — първо `update` на долния, после на горния.
- **★★ `rebalance`** — знакът на фактора на по-високото дете решава единична или двойна.
- **★ `insertAt`** — миналата седмица + `return rebalance(n);`.

<div class="box warn">

`checkInvariants()` проверява **всяка** записана височина. Забравен `update` някъде по пътя → `false`.

</div>

---

## Задачи 4–5

- **★★★ `extractMin`, `eraseAt`** — наследникът се **пренасочва**; `rebalance` на всяко ниво.
- **★★ `rank`, `select`, `countRange`** — полето `size`.

![select(6)](img/order-stat.svg)

---

## Задача 6 — бенчмарк

![AVL, std::set, BST](img/avl-bench.svg)

---

<!-- .slide: data-background-color="#e3eefb" -->

# Отвъд AVL

<p class="small">10 минути · какво има в std::map и в базите данни</p>

---

## Червено-черно дърво = 2-3-4 дърво

![Червено-черно дърво](img/rb-tree.svg)

---

## AVL или червено-черно?

| | AVL | червено-черно |
|---|---|---|
| височина | $\le 1.44 \log_2 n$ | $\le 2 \log_2 n$ |
| търсене | по-бързо (по-ниско) | малко по-бавно |
| ротации при вмъкване | $\le 1$ | $\le 2$ |
| ротации при изтриване | $O(\log n)$ | $\le 3$ |
| допълнително във възела | височина | 1 бит цвят |

<p class="small fragment">Измерено при $10^6$: търсене AVL 184 ns, <code>std::set</code> 235 ns; вмъкване — обратно (252 срещу 190).</p>

---

## B-дърво: един възел — един блок

![B-дърво](img/btree.svg)

---

## Височина: границите

![Височини](img/height-bounds.svg)

---

## Обобщение

- **Ротация**: два указателя, $\Theta(1)$, in-order се запазва.
- **AVL**: $\lvert bf \rvert \le 1$ навсякъде → $h < 1.44 \log_2 n$ (Фибоначи).
- **Вмъкване**: `rebalance` на връщане; LL/RR — единична, LR/RL — двойна; най-много една.
- **Изтриване**: същото, но до $O(\log n)$ ротации; $bf(\text{дете}) = 0$ → единична.
- **`size` във възела** → `select`, `rank` за $\Theta(\log n)$.
- `std::map` е червено-черно; базите данни — B+ дървета.

---

## До следващия път

- Довършете задачи 1–3; 4–6 за любопитните.
- Прочетете [notes.md](notes.md).
- **Следващия четвъртък:** двоична пирамида и сортиране — първа част.

<div class="box task">

**Изходен въпрос:** понякога не ви трябва търсене по ключ, а само **най-малкият** елемент — постоянно, докато добавяте нови. AVL го прави за $\Theta(\log n)$. Може ли по-просто — с масив и без указатели?

</div>
