#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <readline/readline.h>
#include <errno.h>
#include <readline/history.h>
#include <fcntl.h>
#include "headers.h"
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
{ char *EXTRA;
  char *TOKEN;
  int cpt;
  char * prompt;
  int test = 0;
    char ** subMat2=NULL;
  int fd[2];
  struct Occurrence occ,occ1;
  int num_words; // number of words in the input string to tokenize
                 // num_words declared outside of tokenize_string for use inside another fct.



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
          strcmp(words[i], ">>") == 0 ||
          strcmp(words[i],"|") == 0)
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
  struct Occurrence getFirstDelim(char* ch)
  {	struct Occurrence occ1;
  	for (int i = 0;i<=strlen(ch);i++)
  	{  
  	   if (ch[i] == '|' && ch[i+1] == '|')
  	   {
  	   	
  	   	occ1.index = i;
  	   	strcpy(occ1.ch,"||");
  	   	return occ1;
  	   }
  	   else if (ch[i] == '&' && ch[i+1] == '&')
  	   {
  	   	occ1.index = i;
  	   	strcpy(occ1.ch,"&&");
  	   	return occ1;
  	   }
  	   else if (ch[i] == ';')
  	   {
  	   	occ1.index = i;
  	   	strcpy(occ1.ch,";");
  	   	return occ1;
  	   }
  	
  	}
  	   occ1.index = -1;
  	   return occ1;
  }
 int getNbDelim(char* ch){
  int cpt = 0;
  for (int i=0 ; i<=strlen(ch); i++)
  {
      if (ch[i] == '|' && ch[i+1] == '|')
      	cpt ++;
      else if (ch[i] == '&' && ch[i+1] == '&')
      	cpt++;
      else if (ch[i] == ';')
        cpt++;
      
  }
  return cpt;
}
  char* getSubstr(char *ch,struct Occurrence occ)
{
  char* res= (char *) malloc((strlen(ch) - occ.index)* sizeof(char*));
  if (occ.index != -1)
  {
  	if (strcmp(occ.ch,"||") == 0)
  	  res = strncpy(res,ch+occ.index+2,strlen(ch));
  	else if (strcmp(occ.ch,"&&") == 0)
  	  res = strncpy(res,ch+occ.index+2,strlen(ch));
  	else if (strcmp(occ.ch,";") == 0)
  	  res = strncpy(res,ch+occ.index+1,strlen(ch));
  	 return res; 
  }
  else
  return NULL;
}
  int execute_command(char *input) {
      char **words;
      int status;
    
         if (cpt == 0)
          words = tokenize_string(input,num_words);
         else 
          words = tokenize_string(TOKEN,num_words);
          do{
          for (int i = 0; words != NULL && words[i] != NULL; i++)
          {
            printf("words[%d]=\"%s\"\n", i, words[i]);
          }
          if (strcmp(words[0], "cd") == 0)
          {
            printf("%s", words[1]);
            int res = chdir(words[1]); // execute cd command using chdir
            if (occ1.index != -1)
            {
            if ((res == 0 && strcmp(occ1.ch,"&&") == 0 )||
                 (res == -1 && strcmp(occ1.ch,"||") == 0) ||
                 strcmp(occ1.ch,";") == 0)
                 {
                    
                    occ1 = getFirstDelim(EXTRA);
                    if (occ1.index != -1)
                    {
                      strcpy(TOKEN,EXTRA);
                      TOKEN = strtokm(TOKEN,occ1.ch);
                      EXTRA = getSubstr(EXTRA,occ1);
                      words = tokenize_string(TOKEN,num_words);
                    }
                    else
                     words = tokenize_string(EXTRA,num_words);
                 }
            else
            {  
                break;
            }
            }
            else break;

               

          }
          else
          {
            
            struct Occurrence occ = findOcc(words); 
            if (occ.index !=-1 && strcmp(occ.ch,"|") == 0){
            subMat2 = (char **)malloc( sizeof(char *) * (num_words-occ.index-1));
            for (int i=occ.index +1 ; i<=num_words-1;i++)
            {subMat2[i-occ.index-1] = (char *)malloc(sizeof(char) * ((strlen(words[i]))));
            subMat2[i-occ.index-1] = words[i];
            test = num_words-occ.index-2;
            subMat2[num_words-occ.index-1] = NULL;
            }
            
            }
            pid_t id = fork();
            if (id == 0)
            { 
              struct Occurrence occ = findOcc(words); // Find the occurence of >/>>/&>/>&
              if (occ.index != -1)                    // There is the character we're looking for
              {
                if (strcmp(occ.ch,"|") &&(occ.index != num_words - 2 || strcmp(words[num_words - 1], "") == 0) )
                {
                  perror("Error in the command"); // if the file isn't after the character >>/>... / if the character isn't at the n-1 index ( n the length of the words string) -> show error message
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
                  {perror("HERE");
                    f_id = open(fileName, O_WRONLY | O_CREAT, 0644);
                    // CREATE FILE + SHOW BOTH STDERR AND STDOUT IN FILE
                    dup2(f_id, 1);
                    dup2(f_id, 2);
                  }
                  else if (strcmp(occ.ch,"|") == 0)
                  {
                  if(pipe(fd) == -1)
                    {
                      perror("pipe");
                      exit(EXIT_FAILURE);
                    }
                   pid_t id2 = fork();
                    if (id2 == 0){

                    close(1);
                    dup2(fd[1],1);
                    close(fd[0]);
                  for (int i = occ.index; i < num_words + 1; i++)
                  {
                    words[i] = NULL;
                    // REMOVE ALL THE CHARACTERS STARTING FROM THE OPERATOR TO MAKE THE EXECUTION OF THE FUNCTION WITH EXECVP.
                  }
                  int res = execvp(words[0], words);
                  perror("execvp");
                  exit(EXIT_FAILURE);
                  }
                  else if (id2 == -1)
                  {
                    perror("Fork Failed");
                    exit(EXIT_FAILURE);
                  }
                  else
                  {
                    close(0);
                    dup2(fd[0],0);
                    close(fd[1]);
                    execvp(subMat2[0],subMat2);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                  }
                  }
                  else
                    perror("this operator doesn't exist");
                    int res;
                  if (strcmp(occ.ch,"|")!=0)
                  {
                  for (int i = occ.index; i < num_words + 1; i++)
                  {
                    words[i] = NULL;
                    // REMOVE ALL THE CHARACTERS STARTING FROM THE OPERATOR TO MAKE THE EXECUTION OF THE FUNCTION WITH EXECVP.
                  }

                  res = execvp(words[0], words);
                  if (res == -1)
                  perror("execvp");
                  return 0;
                }
                }
              }
              else
              { 

                status = execvp(words[0], words);
                // execution if we don't have any operator ( >>/>/&>/>&)
                // print result in STDOUT
                if (status == -1)
                  perror("execvp");
                return 0;
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
              waitpid(id,&status,0);
              printf("command executed\n");

            if (occ1.index != -1)
            {
            if ((status ==0 && strcmp(occ1.ch,"&&") == 0 )||
                 (status != 0 && strcmp(occ1.ch,"||") == 0) ||
                 strcmp(occ1.ch,";") == 0)
                 {
                    
                    occ1 = getFirstDelim(EXTRA);
                    if (occ1.index != -1)
                    {
                      strcpy(TOKEN,EXTRA);
                      TOKEN = strtokm(TOKEN,occ1.ch);
                      EXTRA = getSubstr(EXTRA,occ1);
                      words = tokenize_string(TOKEN,num_words);
                    }
                    else
                     words = tokenize_string(EXTRA,num_words);
                 }
            else
            {  
                break;
            }
            }
            else break;
            
            
            }
          }
      }while(cpt!=0);
  }

  int execute_script_file(char *file_path) {
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
  char d = '%';
  rl_initialize();
    do
  {
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      strcat(cwd, &d);
      prompt = malloc(strlen(cwd) + 1 + 6); //allocate memory for the prompt
      sprintf(prompt, "\033[1;31m%s\033[0m", cwd); // change color to red
      input=readline(prompt);
      cpt = getNbDelim(input);
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
    if (strcmp(input, "exit") == 0)
      break; // kill the process.

    strtok(input, "\n"); // fgets adds a '\n' at the end of the string

    if (cpt != 0)
    {occ1 = getFirstDelim(input);
    EXTRA = (char *) malloc((strlen(input)+1) * sizeof(char *));
    TOKEN = (char *) malloc(strlen(input) * sizeof(char *));
    strcpy(EXTRA,input);
    strcpy(TOKEN,EXTRA);
    TOKEN = strtokm(TOKEN,occ1.ch);
    EXTRA = getSubstr(EXTRA,occ1);
    }

    if(input[0] == '.' && input[1] == '/') {
        if(execute_script_file(input) != 0)
        printf("Error in the script!");
    }
    execute_command(input);
  } while (strcmp(input, "exit") != 0);
  free(TOKEN);
  free(EXTRA);
  free(subMat2);
  exit(1);
  return 0;
}
