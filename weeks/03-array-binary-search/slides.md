<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 3 · 22 октомври</p>

# Масив и двоично търсене

Първият ни контейнер и първият ни „истински“ алгоритъм — и двата около инварианти

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: кеш, компилатор, итератори |
| 15 мин | Преговор: линейни структури, масивът като клас, копиране |
| 20 мин | На живо: двоично търсене от инварианта, с тестове |
| ☕ | почивка |
| 35 мин | Практика: `FixedArray`, `lowerBound`, търсене по отговора |
| 10 мин | Разбор и какво следва |

---

## Загрявка

1. Сума на матрица $4096^2$ по редове: 10 ms. По колони — горе-долу колко?
   <span class="fragment answer">~×30 по-бавно: всеки достъп е в нова кеш линия</span>
2. Какво остава от функция, която попълва локален масив и не го ползва, при `-O2`?
   <span class="fragment answer">само <code>ret</code></span>
3. Къде е бъгът?

```cpp
for (auto it = v.begin(); it != v.end(); ++it)
    if (*it == x) v.erase(it);
```

<p class="fragment"><span class="answer"><code>erase</code> инвалидира <code>it</code></span> — ползвайте <code>it = v.erase(it)</code> без <code>++it</code>; още по-добре: erase-remove</p>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · линейни структури, масивът като клас, копиране</p>

---

## Линейни структури: две представяния

![Линейни структури](img/linear-structures.svg)

---

## Масивът: цената на операциите

| Операция | Сложност | Защо |
|---|---|---|
| достъп `a[i]` | $\Theta(1)$ | адрес = начало + $i \cdot$ sizeof |
| в края | $\Theta(1)$ | никой не се мести |
| вмъкване/изтриване на позиция $i$ | $\Theta(n - i)$ | всички отдясно се местят |
| търсене, несортиран | $\Theta(n)$ | |
| търсене, сортиран | $\Theta(\log n)$ | двоично търсене |

---

## Вмъкване: всичко отдясно се мести

<div class="r-stack">
<img src="img/insert-shift-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/insert-shift-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/insert-shift-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/insert-shift-4.svg" alt="стъпка 4" class="fragment current-visible" data-fragment-index="2">
<img src="img/insert-shift-5.svg" alt="стъпка 5" class="fragment" data-fragment-index="3">
</div>

Отзад напред — иначе презаписваме, преди да сме преместили.

---

## `FixedArray<T>`: обект + буфер

![Обектът и буферът](img/array-object.svg)

**Инвариант:** $0 \le$ `size_` $\le$ `capacity_`, `data_` сочи към собствения буфер. Конструкторът го установява, всеки метод го пази → данните са `private`.

---

## Копиране: плитко срещу дълбоко

![Плитко срещу дълбоко копиране](img/shallow-deep-copy.svg)

**Правилото на трите:** собствен деструктор ⇒ собствен копиращ конструктор и `operator=`.

---

## `operator=`: copy-and-swap

<div class="cols">
<div>

```cpp
// наивно
delete[] data_;               // a = a ?!
data_ = new T[other.capacity_];   // ако хвърли?
std::copy(...);
```

</div>
<div>

```cpp
FixedArray& operator=(const FixedArray& other) {
    FixedArray copy(other);   // може да хвърли
    swap(copy);               // noexcept
    return *this;             // старото умира с copy
}
```

</div>
</div>

- `a = a` — работи без специален случай.
- Ако копирането хвърли, `*this` е непокътнат (**силна гаранция**).

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: двоично търсене

<p class="small">20 минути · от инварианта към кода, тестовете първо</p>

---

## Инвариантът първо, кодът после

![Инвариантът](img/binary-search-invariant.svg)

- Начало: `lo = first`, `hi = last` — двете известни области са празни.
- Стъпка: `*mid < x` → `lo = mid + 1`, иначе `hi = mid`. Интервалът **строго** намалява.
- Край: `lo == hi` → това е отговорът.

---

## `lowerBound(…, 61)` стъпка по стъпка

<div class="r-stack">
<img src="img/binary-search-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/binary-search-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/binary-search-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/binary-search-4.svg" alt="стъпка 4" class="fragment current-visible" data-fragment-index="2">
<img src="img/binary-search-5.svg" alt="край" class="fragment" data-fragment-index="3">
</div>

---

## Кодът

```cpp [|4|5|6-7|9]
template <typename It, typename T>
It lowerBound(It first, It last, const T& value) {
    It lo = first, hi = last;
    while (lo != hi) {
        It mid = lo + (hi - lo) / 2;
        if (*mid < value) lo = mid + 1;
        else              hi = mid;
    }
    return lo;
}
```

Няма `==`, няма `-1`, няма специален случай за празен масив.

---

## Четири класически бъга

| Бъг | Последствие |
|---|---|
| `mid = (lo + hi) / 2` с индекси | препълване при $n > 2^{30}$ |
| `lo = mid` вместо `mid + 1` | безкраен цикъл при интервал с дължина 2 |
| `hi = n - 1` с `while (lo < hi)` | смесени интервали → изпуснат елемент |
| несортиран вход / друг компаратор | грешен отговор, без грешка |

<p class="small fragment">Java: <code>Arrays.binarySearch</code> имаше бъг 1 около 9 години (открит 2006). Първото напълно коректно публикувано двоично търсене е от 1962 г. — 16 години след първото.</p>

---

## `lower_bound`, `upper_bound`, `equal_range`

![lower_bound и upper_bound](img/lower-upper-bound.svg)

---

## Двоично търсене по отговора

![Двоично търсене по отговора](img/first-true.svg)

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 03 е в <code>weeks/03-array-binary-search</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w03/"</code> · с ASan: <code>ctest --preset asan -R "^w03/"</code></p>

---

## Задачи 1–2 — `FixedArray<T>`

`starter/fixed_array.h` — конструкторите, деструкторът, `[]` и итераторите са готови.

| ★ Задача 1 | ★★ Задача 2 |
|---|---|
| `at(i)` → `std::out_of_range` | `insertAt(i, x)` — местене отзад напред |
| `pushBack(x)` → `std::length_error` | `removeAt(i)` — местене отпред назад |
| `popBack()` → `std::out_of_range` | 2000 случайни операции срещу `std::vector` |

<div class="box warn">

Неуспешна операция **не променя** масива. `at()` проверява спрямо **размера**, не капацитета.

</div>

---

## Задача 3 ★★ — правилото на трите

- Копиращ конструктор: **нов** буфер, копирани елементи.
- `operator=`: различни капацитети, `a = a`, `a = b = c`.

<div class="box task">

**Експеримент:** `FixedArray(const FixedArray&) = default;` → `ctest --preset asan -R "Task 3"`. Какво казва ASan?

</div>

---

## Задача 4 ★★ — `lowerBound`, `upperBound`, `contains`

`starter/binary_search.h` — шаблони върху итератора, с компаратор `less`.

- Тестовете ги сравняват с `std::lower_bound` върху 500 случайни масива.
- Работят с `vector`, C масив, `FixedArray`, `std::greater<>`.
- **Само** `less(a, b)` — без `==`. Как казвате „равно“ само с `<`?

<p class="fragment"><span class="answer"><code>!less(a, b) &amp;&amp; !less(b, a)</code></span></p>

---

## Задача 5 ★★★ — по отговора

- `firstTrue(lo, hi, pred)` — за **целия** диапазон на `uint64_t`.
- `integerSqrt(n)` — $\lfloor\sqrt n\rfloor$ без `double`.

```cpp
(std::uint64_t)std::sqrt((double)18446744073709551615ULL)   // 4294967296 — грешно
```

<p class="small">Насоки: <code>lo + (hi - lo) / 2</code>; <code>x &gt; n / x</code> вместо <code>x * x &gt; n</code>.</p>

---

## Задача 6 — бенчмарк

<p class="small"><code>cmake --build --preset release --target w03_bench</code> → <code>./build/release/w03_bench</code></p>

![Линейно срещу двоично търсене](img/search-chart.svg)

---

## Обобщение

- Линейна структура: масив (непрекъснато) или списък (свързано).
- Масивът: $\Theta(1)$ достъп, $\Theta(n)$ вмъкване в средата.
- Клас = **инвариант** + RAII; притежава ресурс ⇒ правило на трите, copy-and-swap.
- Двоично търсене: напишете инварианта, после кода. $[lo, hi)$, `mid = lo + (hi − lo) / 2`.
- Монотонен предикат ⇒ двоично търсене и без масив.

---

## До следващия път

- Довършете задачи 1–4; 5 и 6 за любопитните.
- Прочетете [notes.md](notes.md).
- **Следващия четвъртък:** динамичен масив — растеж, амортизиран анализ, преместване и правилото на петте.

<div class="box task">

**Изходен въпрос:** `FixedArray` е пълен и трябва да добавим още един елемент. Какво бихте направили? Колко ще струва, ако го правим при всяко добавяне?

</div>
