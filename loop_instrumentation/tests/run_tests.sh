#!/bin/zsh

# Установка путей для LLVM 15
export PATH="/opt/homebrew/opt/llvm@15/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm@15/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm@15/include"

# Проверка наличия инструментов
command -v clang++ >/dev/null 2>&1 || { echo >&2 "Ошибка: clang++ не установлен"; exit 1; }
command -v opt >/dev/null 2>&1 || { echo >&2 "Ошибка: opt (LLVM) не установлен"; exit 1; }

# Создаем папку для результатов тестов
mkdir -p test_results

# Генерация IR
echo "Компиляция тестового файла в LLVM IR..."
clang++ -S -emit-llvm -O0 -Xclang -disable-O0-optnone tests/loop_test.cpp -o test_results/loop_test.ll

# Применение пасса
echo "Применение пасса к коду..."
opt -S -load-pass-plugin ../build/libLoopWrapperPass.so \
    -passes="wrap-loops" \
    test_results/loop_test.ll \
    -o test_results/instrumented.ll

# Проверка результата
echo -e "\n=== Проверка инструментирования ==="
if grep -q 'call void @cycle_begin()' test_results/instrumented.ll; then
  echo "cycle_begin() обнаружен ✓"
else
  echo "ОШИБКА: cycle_begin() не найден ❌"
fi

if grep -q 'call void @cycle_finish()' test_results/instrumented.ll; then
  echo "cycle_finish() обнаружен ✓"
else
  echo "ОШИБКА: cycle_finish() не найден ❌"
fi

# Сборка исполняемого файла
echo -e "\nСборка исполняемого файла..."
clang++ test_results/instrumented.ll -o test_results/test_program

# Запуск программы
echo -e "\n=== Вывод программы ==="
./test_results/test_program

echo -e "\nТестирование завершено! Результаты в папке test_results/"