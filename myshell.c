#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main() {
	int pid;

	char co[50];
		
	while(1) {
		printf("prompt> ");
		
		char buf[] = "/usr/bin/";
	
		fgets(co, 50, stdin);
		
		char *com = strtok(co, " ");	
		char *arg1 = strtok(NULL, "\n");
		// char *arg2 = strtok(NULL, " ");

		// char *arg1 = (char *)malloc(sizeof(char)*50);
		// strcpy(arg1, strtok(NULL, " ")); 
		
		// printf("\n%sb\n", arg1);

		strcat(buf, com);
		// printf("\n%saa\n", buf);
		// buf = trim(buf);

		pid = fork();

		if(pid == 0) {
			execl(buf, buf, arg1, NULL);
		}
		else {
			wait(0);
		}
	}
}
