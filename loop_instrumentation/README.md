```markdown
# Инструментирование циклов

Компиляция пасса:
```bash
mkdir build && cd build
cmake ../src
make
```

Тестирование:
```bash
cd tests
chmod +x run_tests.sh
./run_tests.sh
```

Результат должен содержать вызовы:
```llvm
call void @cycle_begin()
; тело цикла
call void @cycle_finish()
```

Для использования с вашим кодом:
```bash
clang++ -S -emit-llvm ваш_код.cpp -o out.ll
opt -load-pass-plugin ../build/src/libLoopWrapperPass.dylib -passes="wrap-loops" out.ll -o instrumented.ll
clang++ instrumented.ll -o программа
```