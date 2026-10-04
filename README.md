# СДП 2026/27 — семинарни упражнения

Материали за семинарните упражнения по **Структури от данни и програмиране** — специалност Компютърни науки, поток 1, ФМИ, СУ „Св. Климент Охридски“, зимен семестър 2026/27.

**Семинар:** всеки четвъртък, 16:00–18:00. Всеки семинар упражнява темата от лекцията през **предходната** седмица.

**🖥 Слайдове и списък на темите: [mishomish.github.io/fmi-kn-sdp-2026-2027-seminar](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/)**

## График

| № | Дата | Тема | Материали |
|---|---|---|---|
| 1 | 8 окт | Увод: сложност, тестване и `double` | [слайдове](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/slides/?w=01-intro-complexity-testing) · [записки](weeks/01-intro-complexity-testing/notes.md) · [задачи](weeks/01-intro-complexity-testing/exercises.md) |
| 2 | 15 окт | Компилаторът, локалност, контейнери | [слайдове](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/slides/?w=02-compiler-locality-containers) · [записки](weeks/02-compiler-locality-containers/notes.md) · [задачи](weeks/02-compiler-locality-containers/exercises.md) |
| 3 | 22 окт | Масив и двоично търсене | [слайдове](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/slides/?w=03-array-binary-search) · [записки](weeks/03-array-binary-search/notes.md) · [задачи](weeks/03-array-binary-search/exercises.md) |
| 4 | 29 окт | Динамичен масив | [слайдове](https://mishomish.github.io/fmi-kn-sdp-2026-2027-seminar/slides/?w=04-dynamic-array) · [записки](weeks/04-dynamic-array/notes.md) · [задачи](weeks/04-dynamic-array/exercises.md) |
| 5 | 5 ное | Едносвързан списък | предстои |
| 6 | 12 ное | Двусвързан списък, итератори, изтичане на памет | предстои |
| 7 | 19 ное | Proxy, стек и опашка | предстои |
| 8 | 26 ное | Shunting-yard, хеширане, хеш таблици | предстои |
| 9 | 3 дек | Дървета I: двоични и двоични наредени дървета | предстои |
| 10 | 10 дек | Дървета II: балансирани дървета | предстои |
| 11 | 17 дек | Двоична пирамида, сортировки I | предстои |
| — | 24, 31 дек | *коледна ваканция* | |
| 12 | 7 яну | Сортировки II | предстои |
| 13 | 14 яну | Графи I: представяне и обхождане | предстои |
| 14 | 21 яну | Графи II; побитови операции; ретроспекция | предстои |

Датите на контролните и домашните, както и схемата за оценяване, са по правилата на лектора.

## Бърз старт

```bash
git clone https://github.com/MishoMish/fmi-kn-sdp-2026-2027-seminar.git
cd fmi-kn-sdp-2026-2027-seminar
cmake --preset default
cmake --build --preset default
ctest --preset default
```

Нужни са компилатор за C++17, CMake 3.21+ и git. Подробно за Windows, macOS, Linux и машините в лабораторията: **[SETUP.md](SETUP.md)**.

## Как е организирано

```text
weeks/NN-тема/
├── slides.md      слайдовете (четат се и тук, в GitHub)
├── notes.md       пълни записки по темата от лекцията
├── exercises.md   задачи с трудност ★ / ★★ / ★★★
├── img/           фигурите (img/src/ — скриптът, който ги генерира)
├── starter/       кодът, който попълвате
├── tests/         тестове — падат, докато задачата не е решена
├── bench/         програми за измерване на време (някои седмици)
└── solutions/     еталонни решения — публикуват се в понеделника след семинара
```

**Работният цикъл:** пишете решението в `starter/` → `cmake --build --preset default` → `ctest --preset default`. Задачата е решена, когато тестовете ѝ са зелени.

**Искате мнение за решението си?** Направете fork на repo-то, качете решението и ми пишете с линк.

## Лиценз

- Записки, слайдове, задачи и фигури: [CC BY-NC-SA 4.0](LICENSE) — може да ги ползвате и преработвате с посочване на автора, некомерсиално и под същия лиценз.
- Код: [MIT](LICENSE-code).
- [Catch2](https://github.com/catchorg/Catch2) (`third_party/catch2/`) — Boost Software License 1.0; [reveal.js](https://revealjs.com) (`slides/vendor/reveal/`) — MIT.
