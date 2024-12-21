#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void print_usage(const char *program_name) {
    printf("Usage:\n");
    printf("  %s <file> 's/old_text/new_text/'\n", program_name);
    printf("  %s <file> '/regex/d'\n", program_name);
    printf("  %s <file> 's/^/prefix/'\n", program_name);
    printf("  %s <file> 's/$/suffix/'\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *command = argv[2];

    printf("Reading file: %s\n", filename);

    char *file_content = read_file(filename);
    if (!file_content) {
        fprintf(stderr, "Error: Unable to read file %s\n", filename);
        return 1;
    }

    printf("Original file content:\n%s\n", file_content);

    char *result = NULL;
    if (strncmp(command, "s/", 2) == 0) {
        printf("Applying regex_replace with command: %s\n", command);
        result = regex_replace(file_content, command);
    } else if (strncmp(command, "/", 1) == 0 && strstr(command, "/d")) {
        printf("Applying regex_delete with command: %s\n", command);
        result = regex_delete(file_content, command);
    } else if (strncmp(command, "s/^", 3) == 0 || strncmp(command, "s/$", 3) == 0) {
        printf("Applying regex_add_prefix_suffix with command: %s\n", command);
        result = regex_add_prefix_suffix(file_content, command);
    } else {
        fprintf(stderr, "Error: Invalid command\n");
        free(file_content);
        return 1;
    }

    if (!result) {
        fprintf(stderr, "Error: No changes were made.\n");
        free(file_content);
        return 1;
    }

    printf("Modified file content:\n%s\n", result);

    printf("Writing changes back to file: %s\n", filename);
    if (write_file(filename, result) != 0) {
        fprintf(stderr, "Error: Unable to write to %s\n", filename);
    } else {
        printf("File %s has been updated successfully.\n", filename);
    }

    free(file_content);
    free(result);
    return 0;
}
