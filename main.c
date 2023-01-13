#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#define KNRM "\x1B[0m"
#define KGRN "\x1B[31m"
#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"
int main()
{
  char **tokenize_string(char *str)
  {
    char **words = NULL; // words array of strings that will be returned
    int num_words = 0;   // number of words in the input string to tokenize
    char *word = strtok(str, " ");
    printf("the word is %s",word);
    while (word != NULL)
    {
      words = (char **)realloc(words, sizeof(char *) * (num_words +1));    // dynamically reallocate the array of strings for each iteration : size of pointer to character (string) * number of words tokenized
      words[num_words] = (char *)malloc(sizeof(char) * (strlen(word) + 1)); 
      // allocate size of tokenized string
      strcpy(words[num_words], word);
      num_words++;
      word = strtok(NULL, " ");
    }
    // Append a null character at the end of the array
    words = (char **)realloc(words, sizeof(char *) * (num_words + 1));
    words[num_words] = NULL;

    return words;
  }

  char *concat(char *s1, char *s2)
  {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
  }
  char *input = calloc(1,sizeof(char));
  char cwd[PATH_MAX];
  do
  {
     char *input = calloc(1,sizeof(char));
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf(COLOR_BOLD "%s%s%%%s" COLOR_OFF, KGRN, cwd, KNRM);
    }
    char t;
    int len;
    while(scanf("%c",&t)==1)
    {
          if(t=='\n')
             break;
          len = strlen(input);
          input= realloc(input,len+1);
         *(input+len) = t;
         *(input+len+1) = '\0';
     }
     fputs(input,stdout);
    strtok(input, "\n"); //fgets adds a '/n' at the end of the string 
    char **words;
    words = tokenize_string(input);
    for (int i = 0; words != NULL && words[i] != NULL; i++)
    {
      printf("words[%d]=\"%s\"\n", i, words[i]);
    }
    if (strcmp(words[0],"cd") == 0){
    	printf("%s",words[1]);
    	chdir(words[1]);
    }
    else{
    pid_t id = fork();
    if (id == 0)
    {
      execvp(words[0], words);
      perror("execvp");
    }
    else if (id < 0)
    {
      // fork failed
      printf("Fork failed\n");
    }
    else
    {
      wait(0);
      printf("command executed\n");
    }
    }
  } while (strcmp(input, "quit"));
  exit(1);
  return 0;
}
