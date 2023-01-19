#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int myexec(char *argv[], char *path[]) {
	int pid = fork();
	if(pid == 0) {
		
		int i=0;
		while(i<4) {
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
		printf("command not found\n");
	}
	else {
		wait(0);
	}	
}

int main() {
	// int pid;
	char *PATH[10] = {"", "/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	int pi=4;
	char str[1000];
	char str2[1000];
		
	while(1) {

		char cwd[50];
		getcwd(cwd, sizeof(cwd));
		printf("\n__yash__%s__:\n$", cwd);
		
		fgets(str, 1000, stdin);
		strcpy(str2, str);

		PATH[0] = (char *)malloc(sizeof(char)*50);
		strcat(cwd, "/");
		strcpy(PATH[0], cwd);

		if(!strcmp(str, "exit\n")) return 0;
		char *argv[10];
		
		char *first = strtok(str, "=");
		if(!strcmp(first, "PATH")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\n");
			PATH[pi] = (char *)malloc(sizeof(char)*50);
			strcpy(PATH[pi], last);
		}

		// printf("%s\ndf", str);

		first = strtok(str, " ");
		if(!strcmp(first, "cd")) {
			char *last = strtok(NULL, " ");
			last = strtok(last, "\n");
			chdir(last);
			continue;
		}
		
		// printf("%s\ndf", str);

		for(int i=0; i<10; i++) {
			argv[i] = (char *)malloc(sizeof(char)*100);	
		}
		
		int i=0, j=0, k=0;
		while(1) {
			while(str2[i] == ' ') {
				i++;
			}
			while(str2[i] != ' ' && str2[i] != '\n') {
				argv[j][k] = str2[i];
				i++;
				k++;
			}
			argv[j][k] = '\0';
			j++;
			if(str2[i]=='\n') break;
			k=0;
		}
		argv[j] = NULL;
		myexec(argv, PATH);
	}
}
