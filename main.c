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
    char **words = NULL; //words array of strings that will be returned
    int num_words = 0; //number of words in the input string to tokenize
    char* word = strtok(str, " ");
    while (word != NULL) {
        words = (char**) realloc(words, sizeof(char*) * (num_words + 1)); //dynamically reallocate the array of strings for each iteration : size of pointer to character (string) * number of words tokenized
        words[num_words] = (char*) malloc(sizeof(char) * (strlen(word) + 1)); //allocate size of tokenized string 
        strcpy(words[num_words], word);
        num_words++;
        word = strtok(NULL, " ");
    }
    // Append a null character at the end of the array
    words = (char**) realloc(words, sizeof(char*) * (num_words + 1));
    words[num_words] = NULL;

    return words;
}
char* concat (char *s1,char *s2){
char *result = malloc(strlen(s1) + strlen(s2) +1);
strcpy(result,s1);
strcat(result,s2);
return result;
}

char *input= (char *)malloc(300 * sizeof(char));
char cwd[PATH_MAX];
do
{
if (getcwd(cwd,sizeof(cwd)) != NULL) {
  printf("%s%s%%%s",KGRN,cwd,KNRM);
}
fgets(input,300,stdin); //fgets instead of scanf to read input with spaces


int id = fork();
if (id == 0 ){

  char **words=tokenize_string(input);
   for(int i = 0; words[i] != NULL; i++) {
        printf("%s\n", words[i]);
    }

        execvp(words[0], words);
        perror("execvp failed");
}
else{
wait(0);
}
}
while(strcmp(input,"quit"));
exit(1);
return 0;
}

