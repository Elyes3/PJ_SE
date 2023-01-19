#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

int main() {
    char *variable_string = "My message: ";
    char *prompt = malloc(strlen(variable_string) + 1 + 6); //allocate memory for the prompt
    sprintf(prompt, "\033[1;31m%s\033[0m", variable_string); // change color to red
    char *input = readline(prompt);
    printf("You entered: %s\n", input);
    free(prompt); //free the memory
    return 0;
}