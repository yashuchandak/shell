#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int myexec(char *argv[], char *path[], int or, int ora, int ir, int kahatk) {
	int pid = fork();
	if (pid == 0) { // agar child ise run karta to ye kam karao 
		if(or!=-1) {
			close(1);
			int fd = open(argv[or+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			argv[or] = NULL;
		}
		else if(ora!=-1) {
			close(1);
			int fd = open(argv[ora+1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			argv[ora] = NULL;
		}
		
		if(ir!=-1) {
			close(0);
			int fd = open(argv[ir+1], O_RDONLY, S_IRUSR | S_IWUSR);
			argv[ir] = NULL;
		}
		
		int i = 0;
		while (i < kahatk)
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
	char *PATH[10] = {"", "/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	int pi = 1;
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

		if(!strcmp(ctrld,"\n")) {
			continue;
		}

		strcpy(str2, str);

		char *argv[10];

		char *first=NULL;
		int dotslash = 0;
		if(!strncmp(str, "./", 2)) {
			int i=2;
			for(i; i<strlen(str); i++) {
				str2[i-2] = str[i];
			}
			str2[i-2] = '\0';
			PATH[0] = (char *)malloc(sizeof(char)*50);
			strcpy(PATH[0], cwd);
			strcat(PATH[0], "/");
			dotslash = 1;
		}
		else if(!strncmp(str, "../", 3)) {
			int i=3;
			for(i; i<strlen(str); i++) {
				str2[i-3] = str[i];
			}
			str2[i-3] = '\0';
			PATH[0] = (char *)malloc(sizeof(char)*50);
			char temppath[50]; //(char *)malloc
			chdir("..");
			getcwd(temppath, sizeof(temppath));
			chdir(cwd);
			strcpy(PATH[0], temppath);
			strcat(PATH[0], "/");
			dotslash = 1;
		}	

		first = strtok(str, "=");
		if(!strcmp(first, "PATH")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\n");
			last = strtok(last, ":");
			while(last) {
				PATH[pi] = (char *)malloc(sizeof(char) * 50);
				strcpy(PATH[pi], last);
				strcat(PATH[pi], "/");
				pi++;
				last = strtok(NULL, ":");
			}
			continue;
		}
		else if(!strcmp(first, "PS1")) {
			char *last = strtok(NULL, "=");
			last = strtok(last, "\"");
			if(!strcmp(last, "\\w$")) {
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

		int i = 0, or=-1, ir=-1, ora=-1;
		char *topi = strtok(str2, "\n");
		topi = strtok(str2, " ");
		while (topi) {
			if(!strcmp(topi, ">")) {
				or=i;
			}
			else if(!strcmp(topi, "<")) {
				ir=i;
			}
			else if(!strcmp(topi, ">>")) {
				ora=i;
			}
			
			strcpy(argv[i], topi);
			i++;
			topi = strtok(NULL, " ");
		}
		argv[i] = NULL;
		if(dotslash == 1) {
			myexec(argv, PATH, or, ora, ir, 1);
			PATH[0] = "";
			dotslash = 0;
		}
		else {
			myexec(argv, PATH, or, ora, ir, 5);
		}
	}
}
