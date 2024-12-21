#ifndef UTILS_H
#define UTILS_H

// Чтение содержимого файла в строку
char *read_file(const char *filename);

// Запись строки в файл
int write_file(const char *filename, const char *content);

// Замена текста в строке на основе команды
char *regex_replace(const char *content, const char *command);

// Удаление строк на основе регулярного выражения
char *regex_delete(const char *content, const char *command);

// Добавление префикса или суффикса к строкам
char *regex_add_prefix_suffix(const char *content, const char *command);

// Создание копии строки (реализация strdup, если не доступна)
char *strdup(const char *src);

#endif // UTILS_H
