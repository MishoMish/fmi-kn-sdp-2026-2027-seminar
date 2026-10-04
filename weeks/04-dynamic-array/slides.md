<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 4 · 29 октомври</p>

# Динамичен масив

Колко да расте, защо `pushBack` е $\Theta(1)$, и как да местим вместо да копираме

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: масив, копиране, двоично търсене |
| 15 мин | Преговор: растеж, амортизиран анализ, свиване |
| 20 мин | На живо: `noexcept` променя какво прави `std::vector` |
| ☕ | почивка |
| 35 мин | Практика: `DynamicArray<T>` — растеж, правилото на петте, `move_if_noexcept` |
| 10 мин | Разбор и какво следва |

---

## Загрявка

1. `insertAt(0, x)` в масив с $n$ елемента — колко премествания? <span class="fragment answer">$n$</span>
2. Какво става при `FixedArray b = a;`, ако не сме написали копиращ конструктор? <span class="fragment answer">два обекта, един буфер → двойно <code>delete[]</code></span>
3. Двоично търсене със затворен интервал и `lo = mid` — какво се случва при `hi == lo + 1`? <span class="fragment answer">безкраен цикъл</span>
4. Миналия път: масивът е пълен, искаме още един елемент. Ако растем с **+1** — колко струват $n$ добавяния? <span class="fragment answer">$1 + 2 + \ldots + n = \Theta(n^2)$</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · растеж, амортизиран анализ, свиване</p>

---

## Растеж стъпка по стъпка

<div class="r-stack">
<img src="img/growth-1.svg" alt="стъпка 1" class="fragment fade-out" data-fragment-index="0">
<img src="img/growth-2.svg" alt="стъпка 2" class="fragment current-visible" data-fragment-index="0">
<img src="img/growth-3.svg" alt="стъпка 3" class="fragment current-visible" data-fragment-index="1">
<img src="img/growth-4.svg" alt="стъпка 4" class="fragment current-visible" data-fragment-index="2">
<img src="img/growth-5.svg" alt="стъпка 5" class="fragment" data-fragment-index="3">
</div>

<p class="small">Защо новият елемент е първи? <code>a.pushBack(a[0])</code>.</p>

---

## $+c$ или $\times k$?

![Премествания на pushBack](img/copies-chart.svg)

---

## Защо удвояването е $\Theta(1)$ амортизирано

$$1 + 2 + 4 + \ldots + C = 2C - 1 < 2n$$

<div class="cols">
<div>

**Агрегатно:** $n$ записа + $< 2n$ премествания $< 3n$ → **3 на операция**.

</div>
<div>

**Счетоводно:** всеки `pushBack` плаща 3 монети — 1 за себе си, 2 спестява за бъдещото преместване.

</div>
</div>

<div class="box warn">

Амортизирано ≠ средно. Няма вероятност: гаранцията е за **всяка** последователност от операции.

</div>

---

## Рядко скъпо, средно евтино

![Реална и амортизирана цена](img/amortized-bars.svg)

---

## Свиване: при ½ или при ¼?

![Свиване при ½ или ¼](img/thrashing-chart.svg)

---

## Преоразмеряването инвалидира

![Инвалидиране](img/invalidation.svg)

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: копиране или преместване?

<p class="small">20 минути · един клас, който брои копиранията си, и <code>std::vector</code></p>

---

## lvalue, rvalue, `std::move`

```cpp
std::string a = "tree";
std::string b = a;              // a е lvalue: копие
std::string c = a + "s";        // временна стойност (rvalue): преместване
std::string d = std::move(a);   // „свърших с a“: преместване
```

- `T&&` се свързва само с rvalue.
- `std::move` **не мести нищо** — само казва „третирай като rvalue“.
- Преместеният обект е **валиден, но неопределен**.

---

## Преместване на `DynamicArray`

![Копиране и преместване](img/move-vs-copy.svg)

```cpp
DynamicArray(DynamicArray&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)),
      size_(std::exchange(other.size_, 0)),
      capacity_(std::exchange(other.capacity_, 0)) {}
```

---

## `noexcept` решава дали `std::vector` мести

<div class="cols">
<div>

```cpp
struct Widget {
    Widget(Widget&&) noexcept;   // ← махнете го
    Widget(const Widget&);
};
std::vector<Widget> v;
for (int i = 0; i < 1000; ++i)
    v.push_back(Widget(i));
```

</div>
<div>

| | копирания | премествания |
|---|---|---|
| с `noexcept` | 0 | 2023 |
| без `noexcept` | 1023 | 1000 |

<p class="small fragment">Без <code>noexcept</code> векторът не може да даде силна гаранция при местене → копира при всяко преоразмеряване.</p>

</div>
</div>

---

## Правилото на петте — и на нулата

| | Подпис |
|---|---|
| деструктор | `~T()` |
| копиращ конструктор / оператор | `T(const T&)`, `T& operator=(const T&)` |
| преместващ конструктор / оператор | `T(T&&) noexcept`, `T& operator=(T&&) noexcept` |

- Класът **управлява ресурс** → и петте.
- Полетата са `std::vector`, `std::string`, `std::unique_ptr` → **нито една** (правило на нулата).

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 04 е в <code>weeks/04-dynamic-array</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w04/"</code> · <strong>и с ASan:</strong> <code>ctest --preset asan -R "^w04/"</code></p>

---

## Задача 1 ★ — растеж

`starter/dynamic_array.h`: `reallocate`, `reserve`, `pushBack(const T&)`, `popBack`.

- Нов капацитет: 1 за празен, иначе $2 \times$.
- Тестът проверява капацитетите: 1, 2, 4, 4, 8, … , 32.
- Тестът брои копирания + премествания: $\le 3n$.

<div class="box bad">

`a.pushBack(a[0])` при пълен масив: прочетете `value` **преди** да местите или освобождавате стария буфер. Без ASan бъгът често не се вижда.

</div>

---

## Задача 2 ★★ — правилото на петте

| | Тестът проверява |
|---|---|
| копиране | собствен буфер; `a = a` |
| преместващ конструктор | **същият** `data()`; 0 копирания и 0 премествания на елементи; оригиналът е празен и използваем |
| преместващ оператор | взима буфера; другият остава празен |
| `noexcept` | `STATIC_REQUIRE` — проверка при компилация |

---

## Задачи 3–4 — преместване навсякъде

- **★★ Задача 3:** `pushBack(T&&)` мести; `resize(n)`; `shrinkToFit()`.
- **★★★ Задача 4:** при преоразмеряване — `std::move_if_noexcept`.

```cpp
fresh[i] = std::move_if_noexcept(data_[i]);
```

| Тип | Очаквано при преоразмеряване на 8 елемента |
|---|---|
| `Tracked` (`noexcept` преместване) | 0 копирания |
| `ThrowingMove` | 8 копирания |

---

## Задача 5 — бенчмарк

<p class="small"><code>cmake --build --preset release --target w04_bench</code> → <code>./build/release/w04_bench</code></p>

![Време за n pushBack](img/timing-chart.svg)

---

## Обобщение

- Растеж: нов буфер → **новият елемент първо** → прехвърляне → освобождаване.
- $\times k$ → $\Theta(1)$ амортизирано; $+c$ → $\Theta(n)$ на добавяне.
- Амортизирано = гаранция за всяка последователност, не вероятност.
- Свиване при ¼, не при ½. `reserve`, когато знаете $n$.
- Преоразмеряването инвалидира всички указатели и итератори.
- Преместването краде ресурса за $\Theta(1)$; `noexcept` е задължителен.

---

## До следващия път

- Довършете задачи 1–3; 4 и 5 за любопитните.
- Прочетете [notes.md](notes.md) — там е и методът на потенциала.
- **Следващия четвъртък:** едносвързан списък — вмъкване за $\Theta(1)$, но на каква цена?

<div class="box task">

**Изходен въпрос:** динамичният масив вмъква в **началото** за $\Theta(n)$. Как бихте подредили паметта, така че вмъкването в началото да е $\Theta(1)$? Какво губите?

</div>
