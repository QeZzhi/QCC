<p align="center">
  <img src="https://www.gitskins.com/api/section/wordmark?username=QeZzhi&theme=aurora&label=QCC%20(Qora%20Compiler%20Collection)" width="100%" alt="QCC Wordmark" />
</p>

# QCC (Qora Compiler Collection) 🛠️

**Компилятор для языка программирования Qora, написанный на C.**

> ⚠️ **Внимание:** Проект находится в активной стадии разработки. Код может быть нестабильным, а функции — отсутствовать. Следите за обновлениями!

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-lightgrey.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build: Make](https://img.shields.io/badge/Build-Makefile-blue.svg)]()
[![Platform: macOS / Linux](https://img.shields.io/badge/Platform-macOS%20%2F%20Linux-blue.svg)]()

---

## 📖 О проекте

**QCC** — это компилятор для моего собственного языка программирования **Qora**. 

**Главная цель:** Создать легковесный и понятный компилятор, который транслирует код на Qora в байт-код для виртуальной машины **[QezVM](https://github.com/QeZzhi/QezVM)**. 

Проект пишется с нуля на C (стандарт C11), чтобы полностью контролировать каждый этап компиляции — от чтения символов до генерации байт-кода.

## 🏗 Архитектура компилятора

Проект разбит на логические модули (классическая схема компилятора):
- 📂 `src/lexer` — Лексический анализатор (токенизация, поддержка строк и чисел).
- 📂 `src/parser` — Синтаксический анализатор (построение AST).
- 📂 `src/ast` — Абстрактное синтаксическое дерево (AST).
- 📂 `src/codegen` — Генератор кода (превращает AST в инструкции).
- 📂 `src/bytecode` — Работа с байт-кодом и запись файлов `.qc` для QezVM.
- 📂 `src/common` — Общие структуры и утилиты (безопасное выделение памяти).

## 📝 Пример кода на Qora

```qora
let x = 10;
let y = x * 2 + 3;
print(y);
print("Say Hello Qora!\n");