  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  char **tokenize_string(char *str,int num_words)
  {
    num_words = 0;       // reinitialize -> 0 for each read
    char **words = NULL; // words array of strings that will be returned

    char *word = strtok(str, " ");
    while (word != NULL)
    {
      words = (char **)realloc(words, sizeof(char *) * (num_words + 1)); // dynamically reallocate the array of strings for each iteration : size of pointer to character (string) * number of words tokenized
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