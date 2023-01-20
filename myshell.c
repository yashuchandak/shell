#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int myexec(char *argv[], char *path[], int or, int ir) {
	int pid = fork();
	if (pid == 0) { // agar child ise run karta to ye kam karao 
		if(or!=-1) {
			close(1);
			int fd = open(argv[or+1], O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
			argv[or] = NULL;
		}
		if(ir!=-1) {
			close(0);
			int fd = open(argv[ir+1], O_RDONLY, S_IRUSR);
			argv[ir] = NULL;
		}
		
		int i = 0;
		while (i < 5)
		{
			char pa[50] = "";
			strcpy(pa, path[i]);
			strcpy(pa, strcat(pa, argv[0]));

			if (execv(pa, argv) == -1) {
				i++;
			}
			else {
				break;
			}
		}
		printf("command not found\n");
	}
	else { // parent to wait karo child ke liye
		wait(0);
	}
}

int main() {
	char *PATH[10] = {"/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	int pi = 0;
	char str[1000];
	char str2[1000];

	int prompt = 0;
	char *PS1 = (char *)malloc(sizeof(char)*50);

	while (1) {

		char cwd[50];
		getcwd(cwd, sizeof(cwd));
		if(prompt == 0) {
			printf("\n__yash__%s__:\n$", cwd);
		}
		else {
			printf("%s",PS1);
		}
		char *ctrld = fgets(str, 1000, stdin);

		if(!ctrld || !strcmp(str, "exit\n")) {
			return 0;
		}

		strcpy(str2, str);

		char *argv[10];

		char *first = strtok(str, "=");
		if(!strcmp(first, "PATH")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\n");
			last = strtok(last, ":");
			while(last) {
				PATH[pi] = (char *)malloc(sizeof(char) * 50);
				strcpy(PATH[pi], last);
				printf("%s ", PATH[pi]);
				pi++;
				last = strtok(NULL, ":");
			}
			continue;
		}
		else if(!strcmp(first, "PS1")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\n");
			if(!strcmp(last, "\"\\w$\"")) {
				prompt=0;
			}
			else {
				prompt = 1;
				strcpy(PS1, last);
			}
		}

		first = strtok(str, " ");
		if (!strcmp(first, "cd")) {
			char *last = strtok(NULL, " ");
			last = strtok(last, "\n");
			chdir(last);
			continue;
		}

		strcpy(str, str2);



		for (int i = 0; i < 10; i++) {
			argv[i] = (char *)malloc(sizeof(char) * 100);
		}

		int i = 0, j = 0, k = 0, or=-1, ir=-1;
		while (1) {
			while (str2[i] == ' ') {
				i++;
			}
			while (str2[i] != ' ' && str2[i] != '\n') {
				if(str2[i]=='>') {
					or = j;
				}
				else if(str2[i]=='<') {
					ir = j;
				}
				argv[j][k] = str2[i];
				i++;
				k++;
			}
			argv[j][k] = '\0';
			j++;
			if (str2[i] == '\n')
				break;
			k = 0;
		}
		argv[j] = NULL;

		myexec(argv, PATH, or, ir);
	}
}
