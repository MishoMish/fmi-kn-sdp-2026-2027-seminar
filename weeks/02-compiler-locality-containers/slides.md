<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 2 · 15 октомври</p>

# Компилаторът, локалност, контейнери

Една и съща сложност — до 100 пъти разлика във времето

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a> · Примери: <a href="godbolt/README.md">godbolt/</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: сложност и `double` от миналия път |
| 15 мин | Преговор: кеш, локалност, контейнери, масиви |
| 20 мин | На живо: какво прави компилаторът с кода ни |
| ☕ | почивка |
| 35 мин | Практика: матрица, обхождания, erase-remove, бенчмарк |
| 10 мин | Разбор и какво следва |

---

## Загрявка — 1

Колко е сложността?

```cpp
for (std::size_t i = n; i > 0; i /= 2)
    for (std::size_t j = 0; j < i; ++j)
        ++count;
```

<div class="fragment">

<span class="answer">$\Theta(n)$</span> — не $\Theta(n \log n)$: работата е $n + \frac{n}{2} + \frac{n}{4} + \ldots < 2n$ (задача 1f).

</div>

---

## Загрявка — 2

Удвоявате $n$ и времето става **8 пъти** по-голямо. Каква е сложността?

<p class="fragment"><span class="answer">$\Theta(n^3)$</span> — $\frac{T(2n)}{T(n)} \approx 2^k = 8 \Rightarrow k = 3$</p>

Вярно или не?

- $n \in O(n^2)$ <span class="fragment answer">вярно (но безполезно)</span>
- $n^2 \in O(n)$ <span class="fragment answer">невярно</span>
- `0.1 * 3 == 0.3` <span class="fragment answer">false</span>
- `std::sort` с NaN в масива е безопасно <span class="fragment answer">не — UB</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · откъде идват константите, които асимптотиката крие</p>

---

## Паметта не е еднакво бърза

![Йерархия на паметта](img/memory-hierarchy.svg)

---

## Кеш линии: паметта идва на блокове от 64 B

![Кеш линия](img/cache-line.svg)

- **Пространствена локалност:** четем съседни адреси → една линия обслужва много четения.
- **Временна локалност:** четем едно и също отново → то още е в кеша.

---

## Матрица в един блок, по редове

![Матрица по редове](img/row-major.svg)

Елементът $(r, c)$ е на позиция $r \cdot \text{cols} + c$ → **задача 1**.

---

## Масив → указател

![Масив → указател](img/array-decay.svg)

---

## Контейнерите в STL

![Карта на контейнерите](img/container-map.svg)

---

## Цената на операциите

| Операция | `vector` | `deque` | `list` | `set` | `unordered_set` |
|---|---|---|---|---|---|
| достъп по индекс | $\Theta(1)$ | $\Theta(1)$ | $\Theta(n)$ | — | — |
| в края | $\Theta(1)$ аморт. | $\Theta(1)$ | $\Theta(1)$ | — | — |
| в началото | $\Theta(n)$ | $\Theta(1)$ | $\Theta(1)$ | — | — |
| в средата (по итератор) | $\Theta(n)$ | $\Theta(n)$ | $\Theta(1)$ | $\Theta(\log n)$ | $\Theta(1)$ ср. |
| търсене | $\Theta(n)$ | $\Theta(n)$ | $\Theta(n)$ | $\Theta(\log n)$ | $\Theta(1)$ ср. |
| локалност | отлична | добра | лоша | лоша | средна |

<div class="box good">

По подразбиране: `std::vector`. Последният ред често решава повече от асимптотиката.

</div>

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: Compiler Explorer

<p class="small">20 минути · <a href="https://godbolt.org">godbolt.org</a> · всички примери: <a href="godbolt/README.md">godbolt/</a></p>

---

## Как се чете асемблер (минимумът)

<div class="cols">
<div>

| Инструкция | Значение |
|---|---|
| `mov a, b` | `a = b` |
| `add` / `sub` / `imul` | `+=` / `-=` / `*=` |
| `lea a, [b + c*4]` | `a = b + c*4` |
| `cmp a, b` + `jl` … | сравни и скочи |
| `call` / `ret` | извикай / върни |

</div>
<div>

- аргументи: `edi`, `esi`, `edx`… (`rdi`, `rsi`… за 64 бита)
- резултат: `eax` / `rax`
- `[rdi]` = паметта на адрес `rdi`
- цветът свързва ред от C++ с инструкциите му

</div>
</div>

---

## A — къде отиде цикълът?

```cpp
int sumTo(int n) {
    int total = 0;
    for (int i = 0; i < n; ++i) total += i;
    return total;
}
```

[▶ Clang -O2 срещу GCC -O0](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gQ29tcGFyZTogY2xhbmcgLU8yIHZzIGdjYyAtTzAuIFdoZXJlIGRpZCB0aGUgbG9vcCBnbz9cbmludCBzdW1UbyhpbnQgbikge1xuICAgIGludCB0b3RhbCA9IDA7XG4gICAgZm9yIChpbnQgaSA9IDA7IGkgPCBuOyArK2kpIHtcbiAgICAgICAgdG90YWwgKz0gaTtcbiAgICB9XG4gICAgcmV0dXJuIHRvdGFsO1xufVxuIiwgImNvbXBpbGVycyI6IFt7ImlkIjogImNsYW5nMTgxMCIsICJvcHRpb25zIjogIi1PMiJ9LCB7ImlkIjogImcxNTEiLCAib3B0aW9ucyI6ICItTzAifV19XX0=)

<div class="fragment">

Clang -O2: **няма цикъл** — `imul` + `shr` = $\frac{n(n-1)}{2}$. Формулата от задача 1c миналата седмица. $\Theta(n) \to \Theta(1)$.

</div>

---

## B, C — вграждане и мъртъв код

<div class="cols">
<div>

```cpp
static int square(int x) { return x * x; }
int answer() {
    return square(6) + square(2) + 2;
}
```

[▶ -O0 срещу -O2](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gLU8wIHZzIC1PMjogaG93IG1hbnkgY2FsbHMgdG8gc3F1YXJlKCkgYXJlIGxlZnQ/XG5zdGF0aWMgaW50IHNxdWFyZShpbnQgeCkge1xuICAgIHJldHVybiB4ICogeDtcbn1cblxuaW50IGFuc3dlcigpIHtcbiAgICByZXR1cm4gc3F1YXJlKDYpICsgc3F1YXJlKDIpICsgMjtcbn1cbiIsICJjb21waWxlcnMiOiBbeyJpZCI6ICJnMTUxIiwgIm9wdGlvbnMiOiAiLU8wIn0sIHsiaWQiOiAiZzE1MSIsICJvcHRpb25zIjogIi1PMiJ9XX1dfQ==)

<p class="fragment">-O2: <code>mov eax, 42</code></p>

</div>
<div>

```cpp
void work() {
    int scratch[1000];
    for (int i = 0; i < 1000; ++i)
        scratch[i] = i * i;
}
```

[▶ -O0 срещу -O2](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gLU8wIHZzIC1PMjogdGhlIHJlc3VsdCBpcyBuZXZlciB1c2VkLiBXaGF0IGlzIGxlZnQgb2YgdGhlIGxvb3A/XG52b2lkIHdvcmsoKSB7XG4gICAgaW50IHNjcmF0Y2hbMTAwMF07XG4gICAgZm9yIChpbnQgaSA9IDA7IGkgPCAxMDAwOyArK2kpIHtcbiAgICAgICAgc2NyYXRjaFtpXSA9IGkgKiBpO1xuICAgIH1cbn1cbiIsICJjb21waWxlcnMiOiBbeyJpZCI6ICJnMTUxIiwgIm9wdGlvbnMiOiAiLU8wIn0sIHsiaWQiOiAiZzE1MSIsICJvcHRpb25zIjogIi1PMiJ9XX1dfQ==)

<p class="fragment">-O2: само <code>ret</code> — затова бенчмарките „ползват“ резултата си</p>

</div>
</div>

---

## D — недефинираното поведение е разрешение

```cpp
bool plusOneIsBigger(int x)              { return x + 1 > x; }
bool plusOneIsBiggerUnsigned(unsigned x) { return x + 1 > x; }
```

[▶ GCC -O2](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gLU8yOiB3aHkgZG9lcyBvbmUgZnVuY3Rpb24gY29tcGFyZSBhbmQgdGhlIG90aGVyIG5vdD9cbi8vIEhpbnQ6IHNpZ25lZCBvdmVyZmxvdyBpcyB1bmRlZmluZWQgYmVoYXZpb3VyLCB1bnNpZ25lZCBvdmVyZmxvdyB3cmFwcy5cbmJvb2wgcGx1c09uZUlzQmlnZ2VyKGludCB4KSB7XG4gICAgcmV0dXJuIHggKyAxID4geDtcbn1cblxuYm9vbCBwbHVzT25lSXNCaWdnZXJVbnNpZ25lZCh1bnNpZ25lZCB4KSB7XG4gICAgcmV0dXJuIHggKyAxID4geDtcbn1cbiIsICJjb21waWxlcnMiOiBbeyJpZCI6ICJnMTUxIiwgIm9wdGlvbnMiOiAiLU8yIn1dfV19)

<div class="fragment">

- `int`: `mov eax, 1` — препълването е UB, „не се случва“, значи винаги `true`.
- `unsigned`: `cmp edi, -1` — препълването превърта до 0, проверката остава.

</div>

---

## E, F — векторизация и цената на `at()`

<div class="cols">
<div>

```cpp
void addArrays(float* __restrict out,
               const float* a,
               const float* b, int n) {
    for (int i = 0; i < n; ++i)
        out[i] = a[i] + b[i];
}
```

[▶ -O3 -march=x86-64-v3](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gLU8zIC1tYXJjaD14ODYtNjQtdjM6IGxvb2sgZm9yIHZhZGRwcyBhbmQgeW1tIHJlZ2lzdGVycyAtXG4vLyA4IGZsb2F0cyBhZGRlZCBieSBPTkUgaW5zdHJ1Y3Rpb24gKFNJTUQpLlxudm9pZCBhZGRBcnJheXMoZmxvYXQqIF9fcmVzdHJpY3Qgb3V0LCBjb25zdCBmbG9hdCogYSwgY29uc3QgZmxvYXQqIGIsIGludCBuKSB7XG4gICAgZm9yIChpbnQgaSA9IDA7IGkgPCBuOyArK2kpIHtcbiAgICAgICAgb3V0W2ldID0gYVtpXSArIGJbaV07XG4gICAgfVxufVxuIiwgImNvbXBpbGVycyI6IFt7ImlkIjogImcxNTEiLCAib3B0aW9ucyI6ICItTzMgLW1hcmNoPXg4Ni02NC12MyJ9XX1dfQ==)

<p class="fragment"><code>vaddps ymm</code> — 8 float-а с една инструкция</p>

</div>
<div>

```cpp
int byIndex(const std::vector<int>& v,
            std::size_t i) { return v[i]; }
int byAt(const std::vector<int>& v,
         std::size_t i) { return v.at(i); }
```

[▶ GCC -O2](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6IFt7ImlkIjogMSwgImxhbmd1YWdlIjogImMrKyIsICJzb3VyY2UiOiAiLy8gLU8yOiB3aGF0IGRvZXMgYXQoKSBjb3N0IGNvbXBhcmVkIHRvIG9wZXJhdG9yW10/XG4jaW5jbHVkZSA8Y3N0ZGRlZj5cbiNpbmNsdWRlIDx2ZWN0b3I+XG5cbmludCBieUluZGV4KGNvbnN0IHN0ZDo6dmVjdG9yPGludD4mIHYsIHN0ZDo6c2l6ZV90IGkpIHtcbiAgICByZXR1cm4gdltpXTtcbn1cblxuaW50IGJ5QXQoY29uc3Qgc3RkOjp2ZWN0b3I8aW50PiYgdiwgc3RkOjpzaXplX3QgaSkge1xuICAgIHJldHVybiB2LmF0KGkpO1xufVxuIiwgImNvbXBpbGVycyI6IFt7ImlkIjogImcxNTEiLCAib3B0aW9ucyI6ICItTzIifV19XX0=)

<p class="fragment"><code>[]</code>: 2 инструкции. <code>at()</code>: + сравнение и хвърляне</p>

</div>
</div>

---

## Какво компилаторът **не може**

- Да смени алгоритъма (освен в тесни случаи като `sumTo`).
- Да промени **къде са данните** в паметта.
- Да пренареди събиране на `double` — затова в бенчмарка матрицата е от `double`: с `int` GCC -O3 **сам размени циклите** и разликата изчезна.

<div class="box warn">

Мерим **само** в Release. В Debug мерим дебъгера, не алгоритъма.

</div>

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small">Ако не сте: <code>git pull</code> и <code>cmake --build --preset default</code> — новата седмица е в <code>weeks/02-…</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. Задачите: <a href="exercises.md">exercises.md</a>. Тестовете: <code>ctest --preset default -R "^w02/"</code></p>

---

## Задача 1 ★ — един ред код

`starter/matrix.h` — матрица в **един** непрекъснат блок:

```cpp
std::size_t index(std::size_t row, std::size_t col) const noexcept {
    // TODO: row-major formula
}
```

Всичко останало в класа е готово и стъпва на `index()`.

<p class="small">Ако тестът мине от първия път — проверете защо е верен за 1 × 3 и за 4 × 1.</p>

---

## Задача 2 ★ — един и същ брой операции

`starter/traversal.h`

| Функция | Обхождане |
|---|---|
| `sumRowMajor` | `r` навън, `c` навътре |
| `sumColMajor` | `c` навън, `r` навътре |
| `transpose` | `result(c, r) = m(r, c)` |

Предскажете: колко пъти по-бавно ще е по колони за $4096 \times 4096$?

---

## Задача 3 ★★ — изтриване на всички срещания

`starter/remove_all.h` — шаблони за **всеки** контейнер: `vector`, `deque`, `list`, `string`…

```cpp
for (auto it = c.begin(); it != c.end(); ++it)
    if (*it == value) c.erase(it);          // къде е бъгът?
```

<div class="fragment">

- `erase` **инвалидира** `it` → после `++it` е UB. Ползвайте върнатия итератор.
- `removeAllNaive`: $\Theta(n^2)$ за `vector` — защо?
- `removeAllLinear`: $\Theta(n)$ — erase-remove: `std::remove` + един `erase`.

</div>

---

## Задача 4 ★★★ — транспониране на блокове

![Транспониране на блокове](img/blocked-transpose.svg)

`transposeBlocked(m, block)` — същият резултат, по блокове $B \times B$. Внимавайте с краищата, когато размерът не се дели на $B$.

---

## Задача 5 ★★ — пуснете бенчмарка

```bash
cmake --preset release
cmake --build --preset release --target w02_bench
./build/release/w02_bench
```

1. Колко пъти по-бавно е по колони? Отговаря ли на предсказанието ви?
2. Колко ускорява транспонирането на блокове? Пробвайте `block` = 8, 64, 256.
3. Защо `std::list` е по-бърз от „разбърканите възли“, щом и двете са свързани?

---

## Резултатът на примерна машина

![Три експеримента](img/results-chart.svg)

---

## Резултатът: стъпка и кеш линии

![Време спрямо стъпката](img/stride-chart.svg)

---

## Обобщение

- Компилаторът прави **всичко**, което не променя наблюдаемото поведение — включително да изтрие кода ви.
- UB не е „каквото прави процесорът“ — компилаторът приема, че няма UB.
- Кешът зарежда **64 B** наведнъж: последователният достъп е почти безплатен, разпръснатият — не.
- Същата сложност, ×32 (матрица по колони) и ×94 (разбъркани възли) разлика.
- `erase` инвалидира итератори; erase-remove е $\Theta(n)$.
- По подразбиране: `std::vector`.

---

## До следващия път

- Довършете задачи 1–3; 4 и 5 — ако сте любопитни.
- Прочетете [notes.md](notes.md) — там са и примерите от Compiler Explorer с пълния асемблер.
- **Следващия четвъртък:** масивът като клас и двоично търсене.

<div class="box task">

**Изходен въпрос:** `std::vector<std::vector<int>>` с 1000 реда по 1000 елемента — колко алокации на памет прави? Защо обхождането му може да е по-бавно от един блок `Matrix`?

</div>
