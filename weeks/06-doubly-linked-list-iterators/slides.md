<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 6 · 12 ноември</p>

# Двусвързан списък, итератори, изтичане на памет

Без специални случаи, без $N \times M$ функции, без изгубени възли

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: едносвързан списък |
| 15 мин | Преговор: фиктивен възел, итератори, категории |
| 20 мин | На живо: три начина да намерим изтичане на памет |
| ☕ | почивка |
| 35 мин | Практика: `List<T>` с итератор, `splice`, алгоритми с итератори |
| 10 мин | Разбор и какво следва |

---

## Загрявка

1. Защо изтриването в края на едносвързан списък е $\Theta(n)$? <span class="fragment answer">новият <code>tail_</code> е предпоследният</span>
2. Кой ред е първи при вмъкване след `prev`? <span class="fragment answer"><code>node->next = prev->next</code> — иначе губим продължението</span>
3. Какво не е наред: `~Node() { delete next; }`? <span class="fragment answer">рекурсия с дълбочина $n$ → stack overflow</span>
4. Как засичаме цикъл с $\Theta(1)$ памет? <span class="fragment answer">Флойд: бавен и бърз указател</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · фиктивен възел, итератори, категории</p>

---

## Фиктивният възел: кръг без специални случаи

![Двусвързан списък с фиктивен възел](img/dll-sentinel.svg)

---

## Празният списък

![Празен списък](img/dll-empty.svg)

`begin() == end()` — и `insert(end(), x)` работи без нито един `if`.

---

## `insert`: четири указателя

<div class="r-stack">
<img src="img/dll-insert-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/dll-insert-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/dll-insert-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/dll-insert-4.svg" alt="стъпка 4" class="fragment" data-fragment-index="2">
</div>

---

## `erase`: възелът знае съседите си

<div class="r-stack">
<img src="img/dll-erase-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/dll-erase-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/dll-erase-3.svg" alt="стъпка 3" class="fragment" data-fragment-index="1">
</div>

---

## Итераторите: $N + M$ вместо $N \times M$

![Итераторите свързват контейнери и алгоритми](img/iterator-bridge.svg)

---

## `[begin, end)`

![Полуотвореният интервал](img/half-open.svg)

---

## Итераторът на списъка

```cpp
template <bool IsConst>
class Iterator {
    NodeBase* node_;                       // end() → фиктивният възел
public:
    using iterator_category = std::bidirectional_iterator_tag;
    reference operator*() const { return static_cast<Node*>(node_)->value; }
    Iterator& operator++()      { node_ = node_->next; return *this; }
    Iterator& operator--()      { node_ = node_->prev; return *this; }
    // ==, !=, it++, it--, ->
};
```

<p class="small">Членовете-типове → <code>std::find</code>, <code>std::reverse</code>, <code>std::distance</code> и <code>for (x : list)</code> работят без допълнителен код.</p>

---

## Категории итератори

![Категории итератори](img/iterator-categories.svg)

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: къде изтече паметта?

<p class="small">20 минути · брояч, AddressSanitizer, Valgrind — върху <a href="demo/leak_demo.cpp">demo/leak_demo.cpp</a></p>

---

## Демото

```cpp
// BUG: unlinks the node but never deletes it.
void popFront() {
    Node* victim = sentinel.next;
    victim->prev->next = victim->next;
    victim->next->prev = victim->prev;
    // delete victim;
}
```

Програмата **не крашва**. Резултатът е правилен. Паметта просто расте.

---

## Три инструмента

| Инструмент | Как | Цена |
|---|---|---|
| брояч в типа | `++alive` / `--alive` | нищо; само за този тип |
| AddressSanitizer + LSan | `-fsanitize=address` (preset `asan`) | ~2× бавно, прекомпилиране |
| Valgrind (Linux) | `valgrind --leak-check=full ./prog` | ~20–50× бавно, без прекомпилиране |

<p class="small">macOS: <code>leaks --atExit -- ./prog</code> · Windows: CRT debug heap (<code>_CrtSetDbgFlag</code>)</p>

---

## Как се чете отчетът

```text
==12091==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 24 byte(s) in 1 object(s) allocated from:
    #1 0x… in TinyList::pushBack(int) leak_demo.cpp:22
    #2 0x… in main leak_demo.cpp:50

Indirect leak of 24 byte(s) in 1 object(s) allocated from:
    #1 0x… in TinyList::pushBack(int) leak_demo.cpp:22
```

- Стекът показва **къде е заделена** паметта — не къде е трябвало да се освободи.
- **Direct** — никой не сочи блока. **Indirect** — сочи го само друг изтекъл блок.

---

## По-добре: да няма какво да изтече

- **RAII** — всеки ресурс има собственик с деструктор.
- **Правило на петте / нулата.**
- **`std::unique_ptr`** — в приложен код почти никога не пишете `delete`.

<div class="box warn">

Ние пишем `delete`, защото реализираме **контейнера** — нивото, на което се пишат точно тези абстракции. Затова тестовете броят обектите.

</div>

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 06 е в <code>weeks/06-doubly-linked-list-iterators</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w06/"</code> · <strong>и с ASan</strong></p>

---

## Задачи 1–2 ★ — итератор, `insert`, `erase`

- Итератор: `*`, `->`, `++`/`--` (префиксни и постфиксни), `==`.
- `insert(pos, x)` — преди `pos`; `erase(pos)` — връща следващия; `clear()`.
- Недовършеното хвърля `std::logic_error("TODO: …")`.

<div class="box good">

Фиктивният възел → **никакви** специални случаи. Ако пишете `if` в `insert` — нарисувайте четирите указателя.

</div>

---

## Задача 3 ★★ — преместване: фиктивният възел не се мести

![Преместване](img/dll-move.svg)

---

## Задачи 4–5

- **★★ `splice(pos, other)`** — $\Theta(1)$, нито един нов възел (тестът брои с `Counted`).
- **★★ `reverse()`** — размяна на `prev` и `next` навсякъде.
- **★/★★★ Задача 5** — `findValue`, `reverseRange`, `isPalindrome`, `distanceBetween` — само с итератори.

```cpp
if constexpr (std::is_base_of_v<std::random_access_iterator_tag, Category>)
    return last - first;     // Θ(1)
else  /* брои ++ */          // Θ(n)
```

---

## Задача 7 — опитът на Страуструп

![Сортирано вмъкване](img/sorted-insert-chart.svg)

---

## Обобщение

- Двусвързан списък + фиктивен възел → insert/erase без специални случаи.
- Фиктивният възел е поле → при преместване пренасочете крайните възли.
- Итераторът: обхождане без разкриване на представянето; $[\text{begin}, \text{end})$.
- Категориите определят кои алгоритми работят и колко струват.
- Изтичане: брояч, ASan/LSan, Valgrind. Още по-добре — RAII.
- Векторът печели, освен ако вече държите итератора към мястото.

---

## До следващия път

- Довършете задачи 1–3; 4–7 за любопитните.
- Прочетете [notes.md](notes.md).
- **Следващия четвъртък:** шаблонът Proxy, стек и опашка.

<div class="box task">

**Изходен въпрос:** искате контейнер, който позволява **само** добавяне и премахване от един и същ край. Кой от досегашните контейнери бихте ползвали отдолу — и как бихте **скрили** всичко останало?

</div>
