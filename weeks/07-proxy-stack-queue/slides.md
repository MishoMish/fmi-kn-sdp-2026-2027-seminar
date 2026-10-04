<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 7 · 19 ноември</p>

# Proxy, стек и опашка

Нови интерфейси върху стари структури — и обект, който се преструва на референция

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: двусвързан списък и итератори |
| 15 мин | Преговор: стек, опашка, адаптери, кръгов буфер |
| 20 мин | На живо: Proxy — `BitVector`, чийто `[]` връща прокси |
| ☕ | почивка |
| 35 мин | Практика: `Stack`, `RingQueue`, два стека, `BitVector`, приложения |
| 10 мин | Разбор, контролно 1 и какво следва |

---

## Загрявка

1. Защо `insert` в двусвързан списък с фиктивен възел няма специални случаи? <span class="fragment answer">всеки истински възел винаги има истински prev и next</span>
2. Какво е `*end()`? <span class="fragment answer">недефинирано поведение</span>
3. Защо `std::sort` не работи със `std::list`? <span class="fragment answer">иска random access итератор</span>
4. ASan казва „Indirect leak“. Какво значи? <span class="fragment answer">блокът е достижим само през друг изтекъл блок</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · стек, опашка, адаптери</p>

---

## LIFO и FIFO

![Стек и опашка](img/lifo-fifo.svg)

---

## Адаптер: стекът скрива останалото

![Адаптер](img/adaptor.svg)

---

## Реализации на опашка

| Реализация | `push` | `pop` |
|---|---|---|
| вектор с `erase(begin())` | $\Theta(1)$ аморт. | **$\Theta(n)$** |
| списък с `tail_` | $\Theta(1)$ | $\Theta(1)$ |
| кръгов буфер | $\Theta(1)$ аморт. | $\Theta(1)$ |
| два стека | $\Theta(1)$ | $\Theta(1)$ аморт. |
| `std::deque` | $\Theta(1)$ | $\Theta(1)$ |

---

## Кръгов буфер

<div class="r-stack">
<img src="img/ring-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/ring-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/ring-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/ring-4.svg" alt="стъпка 4" class="fragment current-visible" data-fragment-index="2">
<img src="img/ring-5.svg" alt="стъпка 5" class="fragment" data-fragment-index="3">
</div>

---

## Растеж: разгъване

![Разгъване при растеж](img/ring-grow.svg)

---

## Опашка от два стека

<div class="r-stack">
<img src="img/two-stacks-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/two-stacks-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/two-stacks-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/two-stacks-4.svg" alt="стъпка 4" class="fragment" data-fragment-index="2">
</div>

<p class="small fragment">Всеки елемент се прехвърля <strong>най-много веднъж</strong> → амортизирано $\Theta(1)$ (седмица 04).</p>

---

## Защо `std::stack::pop()` връща `void`?

```cpp
T pop() { T top = c_.back(); c_.pop_back(); return top; }   // изглежда удобно…
```

<div class="fragment">

…но връщането **копира**. Ако копирането хвърли, елементът е вече махнат — **изгубен**.

STL разделя: `top()` (референция, не копира) и `pop()` (не връща нищо).

</div>

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: Proxy

<p class="small">20 минути · обект, който се преструва на референция</p>

---

## Шаблонът Proxy

![Шаблонът Proxy](img/proxy-pattern.svg)

---

## Бит няма адрес

```cpp
std::vector<std::uint64_t> words_;   // 64 bool-а в една дума: 8× по-малко памет

bool& operator[](std::size_t i);     // невъзможно — няма bool в паметта
```

<div class="fragment">

```cpp
class BitRef {                       // прокси: „коя дума, кой бит“
    std::uint64_t* word_;
    std::uint64_t mask_;
public:
    operator bool() const;           // bool b = v[3];
    BitRef& operator=(bool);         // v[3] = true;
    BitRef& operator=(const BitRef&);// v[3] = v[5];   ← капан!
};
```

</div>

---

## `BitRef` отблизо

![BitRef](img/bitref.svg)

---

## Два капана

<div class="cols">
<div>

**Генерираният `operator=`**

```cpp
v[3] = v[5];
```

копира **полетата** на проксито → лявото прокси започва да сочи бит 5; бит 3 не се променя.

</div>
<div>

**`auto`**

```cpp
auto r = v[1];   // BitRef, не bool!
r = true;        // променя v
```

Същото е със `std::vector<bool>` — „контейнерът, който не е контейнер“.

</div>
</div>

---

## Proxy, Adapter, Decorator

| | Интерфейс | Цел | Пример днес |
|---|---|---|---|
| **Proxy** | същият | контролира достъпа | `BitRef`, `unique_ptr` |
| **Adapter** | различен | преобразува | `Stack` върху `vector` |
| **Decorator** | същият | добавя поведение | буфериран поток |

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 07 е в <code>weeks/07-proxy-stack-queue</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w07/"</code></p>

---

## Задачи 1–3 — стек и опашки

- **★ `Stack<T, Container>`** — върху `vector`, `deque`, `list`; `push(T&&)` с `unique_ptr`.
- **★★ `RingQueue<T>`** — `(head_ + i) % capacity_`; разгъване при растеж.
- **★★★ `TwoStackQueue<T>`** — тестът брои прехвърлянията: $\le$ броя `push`.

---

## Задачи 4–5

- **★★ `BitVector`** — `BitRef`: четене, запис, копиране на **стойност**, `flip`; `count()` само в използваните битове.
- **★ скоби**, **★★ стек с минимум**, **★★★ максимум в плъзгащ се прозорец** (монотонен дек).

![Проверка на скоби](img/brackets.svg)

---

## Задача 6 — една буква, 6× разлика

![Пет опашки](img/queue-chart.svg)

<p class="small">Капацитетът е степен на 2 → <code>x % cap == x &amp; (cap - 1)</code>. Пробвайте.</p>

---

## Обобщение

- **Proxy** — същият интерфейс, контролира достъпа; в C++: обект, върнат вместо референция.
- **Стек** — LIFO, върхът е краят на масива; **опашка** — FIFO, никога `erase(begin())`.
- **Кръгов буфер** — нищо не се мести; **два стека** — амортизирано $\Theta(1)$.
- **Адаптери** — `std::stack`, `std::queue`, `std::priority_queue`; `pop()` връща `void`.
- Асимптотиката избира алгоритъма; константите — реализацията.

---

## Контролно 1 и какво следва

- **Контролно 1** е седмицата 23–27 ноември — по лекции 01–09, **включително дървета** (които на семинар ще упражним чак на 3 декември). Вижте записките на лекцията за дървета и материалите за преговор в repo-то.
- **Следващия четвъртък:** Shunting-yard, хеш функции и хеш таблици.

<div class="box task">

**Изходен въпрос:** как бихте пресметнали `3 + 4 * (2 - 1)` с компютър, който може само да чете символите отляво надясно — и има един стек?

</div>
