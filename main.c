#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <sys/wait.h>
#define MAX_HISTORY 100
#define KNRM "\x1B[0m"
#define KGRN "\x1B[31m"
#define COLOR_BOLD "\e[1m"
#define COLOR_OFF "\e[m"
struct Occurrence
{
  char ch[3];
  int index;
};
// Struct for indexing >>/>/&>/>& to change stdout into file
// >> APPEND TO FILE / > OVERRIDE OLD FILE CONTENT / &> || >& PUT BOTH STDOUT AND STDERR INTO FILE.
int main()
{
  int num_words; // number of words in the input string to tokenize
                 // num_words declared outside of tokenize_string for use inside another fct.


  char **tokenize_string(char *str)
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

  // Function to save both the index of the occurrence of the character used to write / append content
  // into the file and the character itself (>>/>...)
  struct Occurrence findOcc(char **words)
  {
    struct Occurrence occ;
    for (int i = 0; i < num_words; i++)
    {
      if (
          strcmp(words[i], ">") == 0 ||
          strcmp(words[i], "&>") == 0 ||
          strcmp(words[i], ">&") == 0 ||
          strcmp(words[i], ">>") == 0)
      // Check if it's one of the characters.
      {
        strcpy(occ.ch, words[i]);
        occ.index = i;
        return occ;
        // If it's one of them save it inside the occ struct.
      }
    }
    occ.index = -1;
    // Neither one of the characters stated above exists.
    return occ;
  }

  // Allocating string character by character to make a string with a dynamic length.
  char *concat(char *s1, char *s2)
  {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
  }

  int execute_command(char *input) {
      char **words;
      int status;
          words = tokenize_string(input);
          for (int i = 0; words != NULL && words[i] != NULL; i++)
          {
            printf("words[%d]=\"%s\"\n", i, words[i]);
          }
          if (strcmp(words[0], "cd") == 0)
          {
            printf("%s", words[1]);
            chdir(words[1]); // execute cd command using chdir
          }
          else
          {
            printf("%s", words[num_words - 1]);
            pid_t id = fork();
            if (id == 0)
            {
              struct Occurrence occ = findOcc(words); // Find the occurence of >/>>/&>/>&
              if (occ.index != -1)                    // There is the character we're looking for
              {
                if (occ.index != num_words - 2 || strcmp(words[num_words - 1], "") == 0)
                {
                  printf("Error in the command"); // if the file isn't after the character >>/>... / if the character isn't at the n-1 index ( n the length of the words string) -> show error message
                }
                else
                {
                  // The character exists in the right spot : we execute some conditional statements to check which character has been typed by the user to do the operations he needs.
                  char *fileName = words[occ.index + 1];
                  int f_id;
                  if (strcmp(occ.ch, ">") == 0)
                  {
                    f_id = open(fileName, O_WRONLY | O_CREAT, 0644);
                    // CREATE FILE + OVERRIDE OLD CONTENT
                    dup2(f_id, 1);
                  }
                  else if (strcmp(occ.ch, ">>") == 0)
                  {
                    f_id = open(fileName, O_APPEND | O_WRONLY);
                    // CREATE FILE + APPEND TO OLD CONTENT
                    dup2(f_id, 1);
                  }
                  else if (strcmp(occ.ch, "&>") == 0)
                  {
                    f_id = open(fileName, O_WRONLY | O_CREAT, 0644);
                    dup2(f_id, 1);
                    // CREATE FILE + SHOW BOTH STDERR AND STDOUT IN FILE
                    dup2(f_id, 2);
                  }
                  else if (strcmp(occ.ch, ">&") == 0)
                  {
                    f_id = open(fileName, O_WRONLY | O_CREAT, 0644);
                    // CREATE FILE + SHOW BOTH STDERR AND STDOUT IN FILE
                    dup2(f_id, 1);
                    dup2(f_id, 2);
                  }
                  else
                    printf("this operator doesn't exist");
                  for (int i = occ.index; i < num_words + 1; i++)
                  {
                    words[i] = NULL;
                    // REMOVE ALL THE CHARACTERS STARTING FROM THE OPERATOR TO MAKE THE EXECUTION OF THE FUNCTION WITH EXECVP.
                  }
                  return execvp(words[0], words);
                }
              }
              else
              {
                status = execvp(words[0], words);
                // execution if we don't have any operator ( >>/>/&>/>&)
                // print result in STDOUT
                perror("execvp");
                return status;
              }
            }
            else if (id < 0)
            {
              // fork failed
              printf("Fork failed\n");
              return -1;
            }
            else
            {
              wait(0);
              printf("command executed\n");
              return 1;
            }
          }
  }

  int execute_script_file(const char *file_path) {
      //read the file content
      strtok(file_path,"./");
      FILE *fp = fopen(file_path, "r");
      if (fp == NULL) {
          perror("Failed to open file");
          return -1;
      }
      char line[1024];
      while (fgets(line, sizeof(line), fp)) {
          if(execute_command(strtok(line,"\n"))!=0)
          return -1;
      }
      fclose(fp);
      return 0;
  }

  char cwd[PATH_MAX];
  int count=0;
  char *input;
  int len;
  char d = '$';
  rl_initialize();
    do
  {
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      strcat(cwd, &d);
      char *prompt = malloc(strlen(cwd) + 1 + 6); //allocate memory for the prompt
      sprintf(prompt, "\033[1;31m%s\033[0m", cwd); // change color to red
      input=readline(prompt);
      add_history(input);
      // Add font-color and font-weight to the terminal path
    }

    count++;
            if (count > MAX_HISTORY) {
            // remove the oldest command from the history
            HIST_ENTRY *entry = remove_history(0);
            free(entry->line);
            free(entry);
        }
    if (strcmp(input, "quit") == 0)
      break; // kill the process.

    strtok(input, "\n"); // fgets adds a '\n' at the end of the string
                         // what is this line for????
    if(input[0] == "." && input[1] == "/") {
        if(execute_script_file(input) != 0)
        printf("Error in the script!");
    }
    execute_command(input);
  } while (strcmp(input, "quit") != 0);
  exit(1);
  return 0;
}