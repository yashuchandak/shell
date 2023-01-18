#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int myexec(char *argv[], char *path[]) {
	int pid = fork();
	if(pid == 0) {
		
		int i=0;
		while(i<5) {
			char pa[50]="";
			strcpy(pa, path[i]);
			strcpy(pa, strcat(pa, argv[0]));
			
			if(execv(pa, argv) == -1) {
				i++;
			}
			else {	
				break;
			}
		}
	}
	else {
		wait(0);
	}	
}

int main() {
	// int pid;
	char *PATH[10] = {"/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	int pi=4;
	char str[1000];
		
	while(1) {
		printf("prompt>");
		
		fgets(str, 1000, stdin);

		if(!strcmp(str, "exit\n")) return 0;
		char *argv[10];
		
		char *first = strtok(str, "=");
		if(!strcmp(first, "PATH")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\n");
			PATH[pi] = (char *)malloc(sizeof(char)*50);
			strcpy(PATH[pi], last);
		}

		for(int i=0; i<10; i++) {
			argv[i] = (char *)malloc(sizeof(char)*100);	
		}
		
		int i=0, j=0, k=0;
		while(1) {
			while(str[i] == ' ') {
				i++;
			}
			while(str[i] != ' ' && str[i] != '\n') {
				argv[j][k] = str[i];
				i++;
				k++;
			}
			argv[j][k] = '\0';
			j++;
			if(str[i]=='\n') break;
			k=0;
		}
		argv[j] = NULL;
		myexec(argv, PATH);
	}
}