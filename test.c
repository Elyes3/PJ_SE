#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"
int main (){
char** tokenize_string(char* str) {
    char **words = NULL;
    int num_words = 0;
    char* word = strtok(str, " ");
    while (word != NULL) {
        words = (char**) realloc(words, sizeof(char*) * (num_words + 1));
        words[num_words] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
        strcpy(words[num_words], word);
        num_words++;
        word = strtok(NULL, " ");
    }

    // Append a null character at the end of the array
    words = (char**) realloc(words, sizeof(char*) * (num_words + 1));
    words[num_words] = NULL;

    return words;
}
char *input= (char *)malloc(300 * sizeof(char));
fgets(input, 300, stdin);


char **words=tokenize_string(input);
 for(int i = 0; words[i] != NULL; i++) {
        printf("%s\n", words[i]);
    }
return 1;
}

