#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"
int main (){
char* concat (const char *s1,const char *s2){
char *result = malloc(strlen(s1) + strlen(s2) +1);
strcpy(result,s1);
strcat(result,s2);
return result;
}
char* input=(char *) malloc(300);
char cwd[PATH_MAX];
do
{
if (getcwd(cwd,sizeof(cwd)) != NULL) {
  printf("%s%s%%%s",KGRN,cwd,KNRM);
}
scanf("%s",input);
int id = fork();
if (id == 0 ){
execl(concat("/bin/",input),concat("/bin/",input),"google.com","-c 4",NULL);
}
else{
wait(0);
}
}
while(strcmp(input,"quit"));
exit(1);
return 0;
}
