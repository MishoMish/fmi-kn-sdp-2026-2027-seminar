# Настройка на средата

За курса ви трябват три неща: **компилатор за C++17**, **CMake 3.21+** и **git**. Всичко останало (Catch2) е вътре в repo-то — не е нужен интернет след клонирането.

Проверка дали вече ги имате:

```bash
g++ --version     # или clang++ --version; на Windows с Visual Studio — пропуснете
cmake --version   # трябва да е 3.21 или по-нова
git --version
```

## Windows

**Препоръчително: Visual Studio 2022 Community** (безплатно).

1. Инсталирайте [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/community/).
2. В инсталатора изберете работното натоварване **„Desktop development with C++“**. То включва компилатора (MSVC) и CMake.
3. Инсталирайте [Git for Windows](https://git-scm.com/download/win).
4. Клонирайте repo-то, после във Visual Studio: **File → Open → Folder…** и изберете **корена** на repo-то. Visual Studio разпознава `CMakePresets.json`; изберете preset `default` от лентата горе.
5. Тестовете: **Test → Test Explorer**, или в терминала на Visual Studio: `ctest --preset default`.

> [!NOTE]
> С генератора на Visual Studio изпълнимите файлове са в `build\default\Debug\` (напр. `build\default\Debug\w01_tests.exe`), а не директно в `build\default\`.

**Алтернатива: VS Code + MSYS2 (GCC).** Инсталирайте [MSYS2](https://www.msys2.org/), в терминала *MSYS2 UCRT64*: `pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja`, добавете `C:\msys64\ucrt64\bin` в `PATH`. После следвайте указанията за VS Code по-долу.

**Алтернатива: WSL** (Ubuntu под Windows) — следвайте указанията за Ubuntu.

## macOS

```bash
xcode-select --install          # компилатор (Apple Clang) и git
brew install cmake              # нужен е Homebrew: https://brew.sh
```

Без Homebrew: изтеглете CMake от [cmake.org/download](https://cmake.org/download/) и след инсталиране изберете *Tools → How to Install For Command Line Use*.

## Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake git
cmake --version
```

Ubuntu 22.04+ има достатъчно нов CMake. При по-стара версия: `sudo snap install cmake --classic`.

## Редактор

Може да ползвате какъвто искате. Ето как се отваря проектът в най-честите:

| Редактор | Как |
|---|---|
| **VS Code** | инсталирайте разширенията *C/C++* и *CMake Tools*; **File → Open Folder** → корена на repo-то; при въпрос за preset изберете `default`. Build: `F7`; тестове: иконката с колба в лявата лента. |
| **CLion** | **Open** → корена на repo-то. В *Settings → Build, Execution, Deployment → CMake* включете preset-а `default` (и `release` при нужда). |
| **Visual Studio 2022** | вижте по-горе. |
| **Терминал** | командите по-долу. |

## Първи build

```bash
git clone https://github.com/MishoMish/fmi-kn-sdp-2026-2027-seminar.git
cd fmi-kn-sdp-2026-2027-seminar

cmake --preset default            # configure — веднъж (и при нов файл)
cmake --build --preset default    # build — след всяка промяна
ctest --preset default            # test
```

Първият build отнема около минута — компилира се Catch2. След това всеки build прекомпилира само промененото.

Очакван резултат при чисто repo: **повечето тестове падат**. Това е правилно — те чакат вашите решения.

## Полезни команди

```bash
ctest --preset default -R "Task 1"           # само тестовете, чието име съдържа "Task 1"
ctest --preset default -L w01                # само седмица 01
./build/default/w01_tests                    # тестовете на седмица 01 директно, с подробен изход
./build/default/w01_tests "[task2]"          # само тестове с таг [task2]
./build/default/w01_tests --list-tests       # списък с всички тестове

cmake --preset release && cmake --build --preset release   # оптимизиран build — за мерене на време
cmake --preset asan && cmake --build --preset asan && ctest --preset asan   # с AddressSanitizer (Linux/macOS)
```

## Резервен вариант: само `g++` (без CMake)

Ако на машината има компилатор, но няма CMake (или CMake отказва), от папката на седмицата:

```bash
cd weeks/01-intro-complexity-testing

# Catch2 — компилира се веднъж (~30 s), после се преизползва
g++ -std=c++17 -c ../../third_party/catch2/catch_amalgamated.cpp -o catch.o

# тестовете
g++ -std=c++17 -Wall -Wextra -I starter -I ../../third_party/catch2 \
    tests/*.cpp starter/*.cpp catch.o -o w01_tests
./w01_tests

# бенчмарк (ако седмицата има bench/)
g++ -std=c++17 -O2 -I starter bench/*.cpp starter/*.cpp -o w01_bench
./w01_bench
```

## Чести проблеми

| Симптом | Причина | Решение |
|---|---|---|
| `cmake: command not found` | няма CMake или не е в `PATH` | инсталирайте (по-горе); рестартирайте терминала |
| `No CMAKE_CXX_COMPILER could be found` | няма компилатор | Windows: workload „Desktop development with C++“; macOS: `xcode-select --install`; Ubuntu: `build-essential` |
| `CMake 3.21 or higher is required` | стар CMake | обновете (Ubuntu: `snap install cmake --classic`) |
| `No such preset ... "default"` | отворена е грешна папка | отворете **корена** на repo-то (там, където е `CMakePresets.json`) |
| `Could not create named generator ...` или грешки за стар cache след смяна на компилатор | остарял `build/` | изтрийте `build/` и пуснете configure отново |
| `ctest` казва `No tests were found` | тестовете не са билднати | първо `cmake --build --preset default` |
| VS Code не вижда preset-ите | липсва разширението CMake Tools | инсталирайте го, после *CMake: Select Configure Preset* от палитрата (`Ctrl+Shift+P`) |
| `preset "asan"` не съществува на Windows | ASan preset-ът е само за Linux/macOS | ползвайте `default` |

Не тръгва и след това? Пишете ми с **пълния текст на първата грешка** и операционната система.
