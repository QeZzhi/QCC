// ============================================================
// main.c — точка входа в компилятор QCC
// Читает файл .q, компилирует, сохраняет в .qc или выводит в консоль.
// main.c — entry point of QCC compiler
// Reads .q file, compiles, saves to .qc or prints to console.
// ============================================================

#include <stdio.h>  // printf, fprintf, fopen, fread | printf, fprintf, fopen, fread
#include <stdlib.h> // free, atoi | free, atoi
#include <string.h> // strcmp | strcmp

#include "bytecode/bytecode.h"   // Bytecode, bytecode_dump, bytecode_free | Bytecode, bytecode_dump, bytecode_free
#include "bytecode/writer.h"     // bytecode_write_file | bytecode_write_file
#include "common/xalloc.h"       // xmalloc | xmalloc
#include "qcc.h"                 // qcc_compile, QCC_VERSION | qcc_compile, QCC_VERSION

// ------------------------------------------------------------
// print_usage — напечатать справку по использованию
// ------------------------------------------------------------
static void print_usage(const char* program) { // Справка | Usage
    printf("Qora compiler (QCC) %s\n\n", QCC_VERSION); // Версия | Version
    printf("usage: %s <file.q> [-o <output.qc>]\n\n", program); // Использование | Usage
    printf("options:\n"); // Опции | Options
    printf("  -h, --help     show this help\n"); // Помощь | Help
    printf("      --version  show version\n"); // Версия | Version
    printf("  -o <file>      save bytecode to file (binary)\n"); // Сохранить в файл | Save to file
    printf("\nIf -o is not specified, bytecode is printed to console as text.\n"); // Если нет -o — вывод в консоль | If no -o — print to console
    printf("Exit code is 1 if the file contains errors.\n"); // Код выхода | Exit code
}

// ------------------------------------------------------------
// read_file — прочитать файл целиком в строку
// Возвращает NULL при ошибке, иначе — строку (владение у вызывающего)
// ------------------------------------------------------------
static char* read_file(const char* path) { // Чтение файла | Read file
    FILE* file = fopen(path, "rb"); // Открываем файл на чтение | Open file for reading
    if (!file) return NULL; // Если не открылся — NULL | If failed — NULL

    if (fseek(file, 0, SEEK_END) != 0) { // Переходим в конец файла | Seek to end
        fclose(file); // Закрываем файл | Close file
        return NULL; // Ошибка | Error
    }

    long size = ftell(file); // Размер файла | File size
    if (size < 0) { // Если размер отрицательный | If size is negative
        fclose(file); // Закрываем файл | Close file
        return NULL; // Ошибка | Error
    }
    rewind(file); // Возвращаемся в начало | Rewind to start

    char* buffer = xmalloc((size_t)size + 1); // Выделяем память под файл + '\0' | Allocate memory for file + '\0'
    size_t read = fread(buffer, 1, (size_t)size, file); // Читаем файл | Read file
    buffer[read] = '\0'; // Завершаем строку | Terminate string

    fclose(file); // Закрываем файл | Close file
    return buffer; // Возвращаем содержимое | Return content
}

// ------------------------------------------------------------
// main — точка входа программы
// ------------------------------------------------------------
int main(int argc, char** argv) { // Главная функция | Main function
    if (argc < 2) { // Если нет аргументов | If no arguments
        print_usage(argv[0]); // Печатаем справку | Print usage
        return 2; // Код выхода 2 (неправильное использование) | Exit code 2 (misuse)
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) { // Если -h или --help | If -h or --help
        print_usage(argv[0]); // Печатаем справку | Print usage
        return 0; // Успех | Success
    }

    if (strcmp(argv[1], "--version") == 0) { // Если --version | If --version
        printf("qcc %s\n", QCC_VERSION); // Печатаем версию | Print version
        return 0; // Успех | Success
    }

    const char* path = argv[1]; // Путь к файлу | File path
    char* source = read_file(path); // Читаем файл | Read file
    if (!source) { // Если не прочитался | If failed to read
        fprintf(stderr, "error: cannot read file '%s'\n", path); // Ошибка | Error
        return 1; // Код выхода 1 | Exit code 1
    }

    Bytecode* bc = qcc_compile(source); // Компилируем | Compile
    free(source); // Освобождаем исходник | Free source

    if (!bc) return 1; // Если ошибка компиляции — код 1 | If compile error — code 1

    // === НОВАЯ ЛОГИКА: Сохранение в файл или вывод в консоль ===
    // Если передан флаг -o (argc >= 4 и argv[2] == "-o")
    if (argc >= 4 && strcmp(argv[2], "-o") == 0) {
        const char* out_path = argv[3];
        // Сохраняем байткод в бинарный файл .qc
        if (bytecode_write_file(bc, out_path)) {
            printf("Compiled successfully: %s -> %s\n", path, out_path);
        } else {
            fprintf(stderr, "error: cannot write to file '%s'\n", out_path);
        }
    } else {
        // Если флага -o нет — выводим байткод в консоль (как раньше)
        bytecode_dump(bc);
    }

    bytecode_free(bc); // Освобождаем байткод | Free bytecode
    return 0; // Успех | Success
}