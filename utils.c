#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

char *strdup(const char *src) {
    size_t len = strlen(src) + 1;
    char *dest = (char *)malloc(len);
    if (dest) {
        memcpy(dest, src, len);
    }
    return dest;
}

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = (char *)malloc(size + 1);
    if (!content) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);
    return content;
}

int write_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return -1;
    }

    size_t written = fwrite(content, 1, strlen(content), file);
    if (written < strlen(content)) {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

// Реализация команды: s/old_text_regex/new_text/
char *regex_replace(const char *content, const char *command) {
    const char *pattern_start = strchr(command + 2, '/');
    const char *replace_start = strchr(pattern_start + 1, '/');
    if (!pattern_start || !replace_start) {
        printf("Invalid regex_replace command format: %s\n", command);
        return NULL;
    }

    char pattern[256], replacement[256];
    strncpy(pattern, command + 2, pattern_start - (command + 2));
    strncpy(replacement, pattern_start + 1, replace_start - (pattern_start + 1));
    pattern[pattern_start - (command + 2)] = '\0';
    replacement[replace_start - (pattern_start + 1)] = '\0';

    printf("Pattern: %s, Replacement: %s\n", pattern, replacement);

    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED)) {
        printf("Failed to compile regex: %s\n", pattern);
        return NULL;
    }

    size_t result_size = strlen(content) * 2; // Результат может быть больше исходного
    char *result = (char *)malloc(result_size);
    if (!result) {
        perror("Error allocating memory");
        regfree(&regex);
        return NULL;
    }

    result[0] = '\0'; // Инициализация результата

    const char *cursor = content;
    regmatch_t match;

    while (regexec(&regex, cursor, 1, &match, 0) == 0) {
        // Копируем до совпадения
        strncat(result, cursor, match.rm_so);
        // Добавляем замену
        strncat(result, replacement, result_size - strlen(result) - 1);
        // Продвигаем указатель
        cursor += match.rm_eo;
    }

    // Копируем остаток строки
    strncat(result, cursor, result_size - strlen(result) - 1);

    regfree(&regex);
    return result;
}


// Реализация команды: /regex/d
char *regex_delete(const char *content, const char *command) {
    const char *pattern_start = command + 1;
    const char *pattern_end = strchr(pattern_start, '/');
    if (!pattern_end) {
        printf("Invalid regex_delete command format: %s\n", command);
        return NULL;
    }

    char pattern[256];
    strncpy(pattern, pattern_start, pattern_end - pattern_start);
    pattern[pattern_end - pattern_start] = '\0';

    printf("Pattern for deletion: %s\n", pattern);

    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED)) {
        printf("Failed to compile regex: %s\n", pattern);
        return NULL;
    }

    char *result = (char *)malloc(strlen(content) + 1);
    if (!result) {
        perror("Error allocating memory");
        regfree(&regex);
        return NULL;
    }

    result[0] = '\0';

    char *line_start = strdup(content); // Создаём копию содержимого
    char *line = strtok(line_start, "\n");

    while (line) {
        if (regexec(&regex, line, 0, NULL, 0) != 0) { // Если строка не совпадает
            strcat(result, line);
            strcat(result, "\n");
        }
        line = strtok(NULL, "\n");
    }

    free(line_start);
    regfree(&regex);
    return result;
}

// Реализация команды: s/^/prefix/ или s/$/suffix/
char *regex_add_prefix_suffix(const char *content, const char *command) {
    const char *prefix = (command[2] == '^') ? command + 3 : "";
    const char *suffix = (command[2] == '$') ? command + 3 : "";

    printf("Prefix: %s, Suffix: %s\n", prefix, suffix);

    // Подсчитываем необходимый размер памяти
    size_t line_count = 0;
    for (const char *c = content; *c; c++) {
        if (*c == '\n') {
            line_count++;
        }
    }

    size_t result_size = strlen(content) + line_count * (strlen(prefix) + strlen(suffix)) + 1;
    char *result = (char *)malloc(result_size);
    if (!result) {
        perror("Error allocating memory");
        return NULL;
    }

    result[0] = '\0'; // Инициализация пустого результата

    // Построчная обработка
    char *line_start = strdup(content);
    if (!line_start) {
        perror("Error duplicating content");
        free(result);
        return NULL;
    }

    char *line = strtok(line_start, "\n");
    while (line) {
        strncat(result, prefix, result_size - strlen(result) - 1);
        strncat(result, line, result_size - strlen(result) - 1);
        strncat(result, suffix, result_size - strlen(result) - 1);
        strncat(result, "\n", result_size - strlen(result) - 1);

        line = strtok(NULL, "\n");
    }

    free(line_start);
    return result;
}
