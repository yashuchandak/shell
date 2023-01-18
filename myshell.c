#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int myexec(char *argv[]) {
	char *path[] = {"/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	
	int pid = fork();
	if(pid == 0) {
		
		int i=0;
		while(1) {
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
	int pid;

	char str[1000];
		
	while(1) {
		printf("prompt>");
		
		fgets(str, 1000, stdin);

		if(!strcmp(str, "exit\n")) return 0;
		
		char *argv[10];
		
		for(int i=0; i<10; i++) {
			argv[i] = (char *)malloc(sizeof(char)*25);	
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
			myexec(argv);
	}
}



// printf("df");
		// int pid = fork();
		// if(pid == 0) {
			// printf("svf");
			// char comub[50] = "/usr/bin/";
			// strcpy(argv[0], strcat(comub, argv[0]));
			// if(execv(comub, argv) == -1) {
			// 	char comus[50] = "/usr/sbin/";
			// 	strcpy(argv[0], strcat(comus, argv[0]));
			// 	if(execv(comus, argv) == -1) {
			// 		char comsb[50] = "/usr/games/";
			// 		strcpy(argv[0], strcat(comus, argv[0]));
			// 		if(execv(comsb, argv) == -1) {
			// 			char comug[50] = "/snap/bin/";
			// 			strcpy(argv[0], strcat(comug, argv[0]));
			// 			if(execv(comug, argv) == -1) {
			// 				printf("command not found\n");
			// 			}
			// 		}
			// 	}
			// }
			// char *path[] = {"/usr/bin/"};
			// printf("dv");

			// execl("/usr/bin/ls", "-la", NULL);
		// }
		// else {
			// wait(0); // wait for child
		// } 