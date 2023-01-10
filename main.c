#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#define KNRM  "\x1B[0m"
#define KGRN  "\x1B[32m"
int main (){
char input[300];
char cwd[PATH_MAX];
do
{
if (getcwd(cwd,sizeof(cwd)) != NULL) {
  printf("%s%s%%%s",KGRN,cwd,KNRM);
}
scanf("%s",input);
int id = fork();
if (id == 0 ){
execl("/bin/"+input,"/bin/"+input,"google.com","-c 4",NULL);
}
else{
wait(0);
}
}
while(strcmp(input,"quit"));
exit(1);
return 0;
}
