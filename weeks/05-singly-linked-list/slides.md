<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 5 · 5 ноември</p>

# Едносвързан списък

Вмъкване за $\Theta(1)$ — и какво плащаме за него

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: динамичен масив, амортизиране, преместване |
| 15 мин | Преговор: възли, видове списъци, цена на операциите |
| 20 мин | На живо: вмъкване, изтриване, обръщане — и как се чупят |
| ☕ | почивка |
| 35 мин | Практика: `SinglyLinkedList<T>`, бърз/бавен указател, Йосиф |
| 10 мин | Разбор и какво следва |

---

## Загрявка

1. `pushBack` в динамичен масив понякога струва $\Theta(n)$. Защо казваме $\Theta(1)$? <span class="fragment answer">амортизирано: $n$ операции струват $< 3n$ общо</span>
2. Растеж с $+1000$ вместо $\times 2$ — какво става? <span class="fragment answer">$\Theta(n)$ на добавяне; константата не помага</span>
3. Какво прави `std::move(x)`? <span class="fragment answer">нищо — само cast до rvalue</span>
4. Защо преместващият конструктор трябва да е `noexcept`? <span class="fragment answer">иначе <code>std::vector</code> копира при всяко преоразмеряване</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · възли, видове, цена</p>

---

## Възли и указатели

![Едносвързан списък](img/list-anatomy.svg)

---

## Четири вида

![Видове свързани списъци](img/list-kinds.svg)

---

## Цената на операциите

| Операция | Едносвързан (с `tail_`) | Динамичен масив |
|---|---|---|
| в началото | $\Theta(1)$ | $\Theta(n)$ |
| добавяне в края | $\Theta(1)$ | $\Theta(1)$ аморт. |
| изтриване в края | $\Theta(n)$ (!) | $\Theta(1)$ |
| позиция $i$ | $\Theta(i)$ | $\Theta(1)$ |
| след даден възел | $\Theta(1)$ | — |
| памет за `int` | ~32 B | 4 B |

<p class="small fragment">Защо изтриването в края е $\Theta(n)$ дори с <code>tail_</code>? — новият <code>tail_</code> е предпоследният, а до него стигаме само отначало.</p>

---

## Паметта на един възел

![Памет на един int](img/node-memory.svg)

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: указателите

<p class="small">20 минути · рисуваме, преди да пишем</p>

---

## `pushFront`

<div class="r-stack">
<img src="img/push-front-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/push-front-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/push-front-3.svg" alt="стъпка 3" class="fragment" data-fragment-index="1">
</div>

```cpp
head_ = new Node{value, head_};
if (tail_ == nullptr) tail_ = head_;   // празен списък!
++size_;
```

---

## Вмъкване в средата: редът е важен

<div class="r-stack">
<img src="img/insert-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/insert-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/insert-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/insert-4.svg" alt="стъпка 4" class="fragment" data-fragment-index="2">
</div>

<p class="small fragment">Обратният ред (<code>prev->next = node</code> първо) губи останалата част от списъка.</p>

---

## Изтриване: трябва ни предишният

<div class="r-stack">
<img src="img/remove-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/remove-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/remove-3.svg" alt="стъпка 3" class="fragment" data-fragment-index="1">
</div>

---

## Обръщане: три указателя

<div class="r-stack">
<img src="img/reverse-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/reverse-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/reverse-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/reverse-4.svg" alt="стъпка 4" class="fragment current-visible" data-fragment-index="2">
<img src="img/reverse-5.svg" alt="край" class="fragment" data-fragment-index="3">
</div>

---

## Четирите въпроса

Преди **всяка** операция:

1. Празен ли е списъкът?
2. Ще стане ли празен?
3. Засяга ли се **първият** възел (`head_`)?
4. Засяга ли се **последният** възел (`tail_`)?

<div class="box bad">

Най-честият бъг тази седмица: `tail_`, който сочи изтрит възел. Тестовете проверяват `back()` след всяка операция — и с ASan виждате веднага къде.

</div>

---

## Защо не рекурсивно изтриване

```cpp
~Node() { delete next; }     // изглежда елегантно…
```

<p class="fragment">…рекурсия с дълбочина $n$. Милион възела = милион кадъра в стека → <strong>stack overflow</strong>. Итеративно:</p>

```cpp
while (head_ != nullptr) {
    Node* next = head_->next;   // запомни, ПРЕДИ да изтриеш
    delete head_;
    head_ = next;
}
```
<!-- .element: class="fragment" -->

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 05 е в <code>weeks/05-singly-linked-list</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w05/"</code> · <strong>и с ASan</strong></p>

---

## Задачи 1–2 — `SinglyLinkedList<T>`

| ★ Задача 1 | ★★ Задача 2 |
|---|---|
| `pushFront`, `pushBack` | `insertAt(i, x)` |
| `popFront` | `removeAt(i)` |
| `clear` — итеративно | `indexOf(x)` |

- 3000 случайни операции срещу `std::list`, проверка на `front()` и `back()` след всяка.
- `nodeAt(i)` е готов — стига до позиция $i$ за $\Theta(i)$.

---

## Задачи 3–4

- **★★ Задача 3:** правилото на петте. Копирането — $\Theta(n)$ (тестът е с 200 000 елемента).
- **★★ Задача 4:** `reverse()` — **същите** възли, обърнати; `removeAll(x)`.

<div class="box task">

🔬 `removeAll` без специален случай за началото: `Node** link = &head_;` — вижте записките, §4.5.

</div>

---

## Задача 5 ★★★ — бърз и бавен указател

![Костенурката и заекът](img/floyd.svg)

`middle`, `kthFromEnd`, `hasCycle` ($\Theta(1)$ памет), `mergeSorted` (без нови възли).

---

## Задача 6 ★★★ — Йосиф

![Йосиф](img/josephus.svg)

---

## Задача 7 — бенчмарк

<p class="small"><code>cmake --build --preset release --target w05_bench</code> → <code>./build/release/w05_bench</code></p>

![Вмъкване в началото](img/front-chart.svg)

---

## Обобщение

- Списъкът: $\Theta(1)$ там, където **вече сте**; $\Theta(i)$, за да стигнете.
- При вмъкване: първо новият хваща продължението, после предишният сочи новия.
- Четирите въпроса: празен, ще стане празен, `head_`, `tail_`.
- Унищожаване — итеративно.
- Бърз/бавен указател: среда, $k$-ти от края, цикъл (Флойд).
- Обхождането е ~×9 по-бавно от вектор; паметта — ~×8 повече.

---

## До следващия път

- Довършете задачи 1–4; 5–7 за любопитните.
- Прочетете [notes.md](notes.md) — кога списъкът наистина е правилният избор.
- **Следващия четвъртък:** двусвързан списък, итератори, изтичане на памет.

<div class="box task">

**Изходен въпрос:** в едносвързан списък изтриването в края е $\Theta(n)$. Какво бихте добавили във всеки възел, за да стане $\Theta(1)$? Колко струва това?

</div>
