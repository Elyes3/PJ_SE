#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAX_HISTORY 100

int main() {
    char *input;
    int count = 0;

    while (1) {
        input = readline("$ ");

        if (!input)
            break;

        add_history(input);
        count++;

        if (count > MAX_HISTORY) {
            // remove the oldest command from the history
            HIST_ENTRY *entry = remove_history(0);
            free(entry->line);
            free(entry);
        }

        // process the command
        printf("You entered: %s\n", input);

        free(input);
    }
    return 0;
}