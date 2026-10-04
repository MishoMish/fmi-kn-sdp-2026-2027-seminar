<!-- .slide: class="title-slide" -->

<p class="kicker">СДП 2026/27 · Семинар 12 · 7 януари</p>

# Сортиране II

Разделяй и владей, долната граница — и как да я заобиколим.

<p class="small">Записки: <a href="notes.md">notes.md</a> · Задачи: <a href="exercises.md">exercises.md</a></p>

---

## План за днес

| Време | Какво правим |
|---|---|
| 10 мин | Загрявка: пирамида, простите сортировки |
| 15 мин | Преговор: merge sort, quicksort, долна граница, броене и radix |
| 20 мин | На живо: merge sort и quicksort с брояч — най-лошият случай на живо |
| ☕ | почивка |
| 35 мин | Практика: сливане, инверсии, тристранно разделяне, quickselect, radix |
| 10 мин | Какво има в `std::sort`, бенчмарк, какво следва |

---

## Загрявка

1. Колко сравнения прави insertion sort на сортиран масив? <span class="fragment answer">$n - 1$</span>
2. Heapsort — стабилен ли е? На място? <span class="fragment answer">не; да — $O(1)$ памет</span>
3. Защо Floyd строи пирамида за $\Theta(n)$, а не $\Theta(n \log n)$? <span class="fragment answer">повечето възли са долу, `siftDown` за тях е евтин</span>
4. Изходният въпрос: два сортирани масива → един сортиран за $\Theta(n)$? <span class="fragment answer">по-малкият от двата „върха“ → изхода. Днес: merge sort</span>

---

<!-- .slide: data-background-color="#e3eefb" -->

# Преговор

<p class="small">15 минути · merge sort, quicksort, долна граница, без сравнения</p>

---

## Сливане

![Сливане](img/merge-trace.svg)

---

## Merge sort: $T(n) = 2T(n/2) + \Theta(n)$

![Merge sort](img/mergesort-tree.svg)

---

## Тристранно разделяне

![Тристранно разделяне](img/partition3.svg)

---

## Pivot-ът решава

![Избор на pivot](img/pivot-choice.svg)

---

## Долна граница: $\log_2 n! \approx n \log_2 n - 1.44n$

![Дърво на решенията](img/decision-tree.svg)

---

## Без сравнения: броене и radix

<div class="r-stack">
<img src="img/counting-sort.svg" alt="сортиране чрез броене" class="fragment fade-out" data-fragment-index="0">
<img src="img/radix-trace.svg" alt="поразрядно сортиране" class="fragment" data-fragment-index="0">
</div>

---

<!-- .slide: data-background-color="#dcf3ea" -->

# На живо: най-лошият случай

<p class="small">20 минути · merge sort и quicksort с брояч</p>

---

## Брояч на сравненията

```cpp
static long long g_comparisons = 0;
static bool less(int a, int b) { ++g_comparisons; return a < b; }
```

```text
merge sort, random                       260995 comparisons =   0.91 n log2 n
merge sort, sorted                       139216 comparisons =   0.49 n log2 n
quicksort first pivot, random            321454 comparisons =   1.12 n log2 n
quicksort first pivot, sorted         199990000 comparisons = 699.87 n log2 n
quicksort random pivot, sorted           334159 comparisons =   1.17 n log2 n
quicksort random pivot, all equal     199990000 comparisons = 699.87 n log2 n
```
<!-- .element: class="fragment" -->

<p class="small fragment">n = 20 000. Сортиран вход + първият за pivot: <strong>600×</strong>. Еднакви елементи — дори случаен pivot не помага → тристранно разделяне.</p>

---

## Колко близо до долната граница

![Брой сравнения](img/comparisons.svg)

---

<!-- .slide: data-background-color="#f6f5f2" -->

# ☕ Почивка

<p class="small"><code>git pull</code> · седмица 12 е в <code>weeks/12-sorting-2</code></p>

---

<!-- .slide: data-background-color="#fde8df" -->

# Практика <span class="timer">35 мин</span>

<p class="small">По двойки. <a href="exercises.md">exercises.md</a> · <code>ctest --preset default -R "^w12/"</code></p>

---

## Задачи

| | | |
|---|---|---|
| ★ 1 | `mergeRanges`, `mergeSort` | стабилно; буферът — веднъж |
| ★★ 2 | `countInversions` | `mid - i` при всеки десен; `uint64_t` |
| ★★ 3 | `partition3`, `quickSort` | pivot — **копие**; рекурсия: по-малката |
| ★★ 4 | `nthElement` | само в частта с `nth` |
| ★★ / ★ 5 | `countingSortByKey`, `radixSort` | префиксни суми; 4 прохода по байт |
| ★★★ 6 | `bucketSort` | `x * n` може да стане `n` |

<p class="small">Ред: 1 → 3 → 5 за всички; 2, 4, 6 — после.</p>

---

## Капани

```cpp
auto p = randomPivot(first, last);
partition3(first, last, *p, comp);   // BUG: *p moves during the partition
const auto pivot = *randomPivot(first, last);   // a copy
```

```cpp
if (comp(pivot, *i)) std::iter_swap(i++, --gt);  // BUG: the new *i is unseen
```

```cpp
inv += mid - i;   // std::uint64_t - 200 000 descending elements: 2 * 10^10 inversions
```

---

<!-- .slide: data-background-color="#e3eefb" -->

# В реалния свят

<p class="small">10 минути · std::sort, Timsort, бенчмарк</p>

---

## Хибриди

| | |
|---|---|
| `std::sort` | **introsort**: quicksort (медиана от три) → heapsort при дълбочина $> 2 \log_2 n$ → insertion sort под 16 |
| `std::stable_sort` | merge sort с буфер |
| `std::nth_element` | introselect |
| Python, Java (обекти) | **Timsort**: естествен merge sort, серии, двоично вмъкване |
| Go, Rust (до 2024) | **pdqsort** |

<p class="small fragment">💡 2015: опит за формално доказателство на Timsort в Java намира бъг, скрит 13 години.</p>

---

## Бенчмарк

<div class="r-stack">
<img src="img/time-chart.svg" alt="време" class="fragment fade-out" data-fragment-index="0">
<img src="img/input-chart.svg" alt="вход" class="fragment" data-fragment-index="0">
</div>

---

## Обобщение

- **Merge sort**: $\Theta(n \log n)$ винаги, стабилен, $\Theta(n)$ памет; на 1% от долната граница.
- **Quicksort**: очаквано $\Theta(n \log n)$, на място; pivot — случаен; тристранно при повторения.
- **Quickselect**: $\Theta(n)$ очаквано — `std::nth_element`.
- **Долна граница**: $\Omega(n \log n)$ сравнения — дърво с $n!$ листа.
- **Без сравнения**: броене $\Theta(n + k)$; radix $\Theta(d(n + b))$ — 10× по-бърз от `std::sort` при $10^7$.
- Реалните библиотеки са **хибриди**.

---

## До следващия път

- Довършете задачи 1, 3, 5; останалите за любопитните.
- Прочетете [notes.md](notes.md).
- **Следващия четвъртък:** графи — представяне, обхождане в ширина и в дълбочина.

<div class="box task">

**Изходен въпрос:** дърветата имат точно един път между всеки два възела. Ако позволите **цикли** и **много** пътища (пътна мрежа, приятелства, зависимости между задачи) — как ще представите структурата в паметта? И как ще обходите всичко, без да минете два пъти през едно място?

</div>
