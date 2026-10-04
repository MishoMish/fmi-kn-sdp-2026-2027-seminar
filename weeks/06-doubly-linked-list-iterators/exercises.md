# Задачи — седмица 06: двусвързан списък, итератори, изтичане на памет

> [Записки](notes.md) · [слайдове](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/slides/?w=06-doubly-linked-list-iterators) · [starter/](starter/) · [tests/](tests/) · [demo/](demo/)

**Трудност:** ★ — всеки трябва да може · ★★ — изисква малко мислене · ★★★ — за любопитните

```bash
git pull
cmake --build --preset default
ctest --preset default -R "^w06/"
./build/default/w06_tests "[task2]"
cmake --preset asan && cmake --build --preset asan && ctest --preset asan -R "^w06/"
```

Задачи 1–4: [`starter/list.h`](starter/list.h). Задача 5: [`starter/iter_algorithms.h`](starter/iter_algorithms.h).

Незавършените методи хвърлят `std::logic_error("TODO: …")` — така тестовете ви казват точно кое още липсва.

Тестовете ползват [`tests/counted.h`](tests/counted.h): тип, който брои живите си обекти. Ако след тест `Counted::alive != 0`, някой възел не е изтрит — това е първият от трите начина за откриване на изтичане, които разглеждаме днес.

---

## Задача 1 ★ — итераторът

Тестове: `Task 1+2` (итераторът и `insert` се тестват заедно — без `insert` няма какво да обхождаме)

В класа `List<T>::Iterator<IsConst>` реализирайте:

| Оператор | Поведение |
|---|---|
| `*it` | стойността на възела: `static_cast<Node*>(node_)->value` |
| `it->` | адресът на стойността |
| `++it`, `it++` | към `next`; постфиксната версия връща **старата** позиция |
| `--it`, `it--` | към `prev`; `--end()` е последният елемент |
| `a == b` | сочат ли същия възел |

Членовете-типове (`iterator_category`, `value_type`, …) и преобразуването `iterator → const_iterator` са готови. Благодарение на тях `std::find`, `std::reverse`, `std::distance` и range-based `for` работят с вашия списък.

---

## Задача 2 ★ — `insert`, `erase`, `clear`

Тестове: `Task 1+2`, `Task 2`

| Метод | Поведение |
|---|---|
| `insert(pos, x)` | вмъква **преди** `pos` (може `pos == end()`); връща итератор към новия |
| `erase(pos)` | изтрива елемента на `pos` (`pos != end()`); връща итератор към **следващия** |
| `clear()` | изтрива всички възли итеративно; списъкът остава празен и използваем |

Благодарение на фиктивния възел **няма специални случаи** — нито за празен списък, нито за първи или последен елемент. Ако пишете `if`, спрете и нарисувайте четирите указателя.

`pushFront`, `pushBack`, `popFront`, `popBack` са готови — те са `insert`/`erase` на `begin()` и `end()`.

Тестът `no node is leaked` проверява с `Counted::alive`, че `erase`, `clear` и деструкторът изтриват всичко.

<details>
<summary>Насока за insert</summary>

`next = pos.node_`, `prev = next->prev`. Създайте възела, свържете **неговите** `prev` и `next`, после пренасочете `prev->next` и `next->prev` към него.

</details>

---

## Задача 3 ★★ — правилото на петте

Тестове: `Task 3`

1. **Копиращ конструктор** — в същия ред. Ако копирането на елемент хвърли по средата, освободете вече създадените възли и хвърлете отново. Тестът `a copy that throws halfway leaks nothing` кара шестото копие да хвърли и проверява, че не е останал нито един излишен `Counted`.
2. **`stealNodes(other)`** (private) — `*this` е празен; вземете всички възли на `other`. Фиктивният възел е **поле** — първият и последният възел на `other` сочат **неговия** фиктивен възел; пренасочете ги към вашия. После `other` трябва да е празен (фиктивният му възел сочи себе си).
3. **Копиращо присвояване** — copy-and-swap (`swap` е готов и ползва `stealNodes`).

Преместващият конструктор и преместващото присвояване са готови — и двата ползват `stealNodes`.

<details>
<summary>Насока за stealNodes</summary>

Ако `other` е празен — нищо. Иначе: `sentinel_.next = other.sentinel_.next; sentinel_.prev = other.sentinel_.prev;` после двата крайни възела: `sentinel_.next->prev = &sentinel_; sentinel_.prev->next = &sentinel_;`. Накрая „изпразнете“ `other`.

Без двата средни реда всичко изглежда наред при обхождане напред, но `--end()` скача в стария (вече празен) списък. Тестът ползва `checkLinks`, който обхожда и назад.

</details>

---

## Задача 4 ★★ — `splice` и `reverse`

Тестове: `Task 4`

1. **`splice(pos, other)`** — премества **всички** възли на `other` преди `pos`, за $\Theta(1)$: нито един възел не се създава, копира или изтрива (тестът проверява с `Counted::alive` и с адресите на елементите). `other` остава празен.
2. **`reverse()`** — разменете `prev` и `next` във **всеки** възел, включително фиктивния.

<details>
<summary>Насока за reverse</summary>

Обхождайте кръга от фиктивния възел, докато не се върнете в него. След размяната на `n->prev` и `n->next`, следващият възел е в `n->prev` (старият `next`).

</details>

---

## Задача 5 ★★/★★★ — алгоритми само с итератори

Файл: [`starter/iter_algorithms.h`](starter/iter_algorithms.h) · тестове: `Task 5`

| Функция | Изисква | Трудност |
|---|---|---|
| `findValue(first, last, x)` | `*`, `++`, `!=` | ★ |
| `reverseRange(first, last)` | + `--` | ★★ |
| `isPalindrome(first, last)` | + `--` | ★★ |
| `distanceBetween(first, last)` | $\Theta(1)$ за random access, $\Theta(n)$ за останалите — избор при компилация | ★★★ |

Тестовете ги пускат върху вашия `List`, `std::vector`, C масив, `std::string` и `std::forward_list`. Тестът за `distanceBetween` ползва итератор, който **брои** колко пъти е извикан `++`: при random access трябва да са 0.

<details>
<summary>Насока за reverseRange</summary>

Двата итератора не бива да се разминат. Условието `while (first != last && first != --last)` покрива и четна, и нечетна дължина. Размяна: `std::iter_swap(first, last)`.

</details>

<details>
<summary>Насока за distanceBetween</summary>

```cpp
using Category = typename std::iterator_traits<It>::iterator_category;
if constexpr (std::is_base_of_v<std::random_access_iterator_tag, Category>) { … }
else { … }
```

`if constexpr` компилира само единия клон — затова `last - first` не дава грешка за итератор на списък.

</details>

---

## Задача 6 ★★ — намерете изтичането

Файл: [`demo/leak_demo.cpp`](demo/leak_demo.cpp) — малък списък с бъг в `popFront`.

1. Компилирайте и пуснете без нищо: `g++ -std=c++17 -g demo/leak_demo.cpp -o demo && ./demo`. Какво казва броячът?
2. С AddressSanitizer: добавете `-fsanitize=address`. Кой ред сочи отчетът? Защо има един **direct** и един **indirect** leak, след като изтичат два възела?
3. (Linux) С Valgrind: компилирайте без ASan и пуснете `valgrind --leak-check=full ./demo`. Сравнете с ASan.
4. (macOS) `leaks --atExit -- ./demo`.
5. Поправете бъга. Какво казват трите инструмента сега?

---

## Задача 7 ★ — бенчмарк: опитът на Страуструп

```bash
cmake --preset release
cmake --build --preset release --target w06_bench
./build/release/w06_bench
```

1. Част 1: сортирано вмъкване. Кой печели — `std::vector` или списъкът? С колко? Защо, след като вмъкването в списък е $\Theta(1)$?
2. Част 2: вмъкване на **известна** позиция. Как се променя картината?
3. Колко пъти е по-бавен `std::list` от вашия `List` в част 1? Защо изобщо има разлика? (Насока: какво друго има във възела на `std::list`?)

---

## Въпроси за размисъл (без код)

1. Защо `insert` и `erase` приемат `const_iterator`, а не `iterator`?
2. Какво става, ако извикате `*list.end()`? А `--list.begin()`?
3. Защо `std::sort` не работи с `std::list`, а `std::reverse` — работи?
4. Двусвързаният списък пази по два указателя на възел. Има ли начин да пази **един** и пак да може да се обхожда в двете посоки? (🔬 Насока: XOR.)
5. Програма заделя памет при старта и никога не я освобождава, защото я ползва до самия край. Изтичане ли е това? Какво ще каже Valgrind?

Отговорите се публикуват заедно с решенията.
