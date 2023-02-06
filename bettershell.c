#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int myexec(char *argv[], char *path[], int or, int ora, int ir) {
	
	int pid = fork();
	if (pid == 0) {
		// setsid(); // agar child ise run karta to ye kam karao 
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
		while (path[i])
		{	
			char pa[1000] = "";
			strcpy(pa, path[i]);
			strcpy(pa, strcat(pa, argv[0]));
			if(execv(pa, argv) == -1) {
				i++;
			}
			else {
				break; // koi jarurat?
			}
		}
		
		printf("command not found\n");
	}
	else { // parent to wait karo child ke liye
		wait(0);
	}
	return 0;
}

void trim(char **strp) {
	
	char *str = *strp;	
	
	while(str[0]==' ') {
		str++;
	}

	int abhika = strlen(str)-2; // "  \n\0"
	while(str[abhika]==' ') {
		abhika--;
	}
	str[abhika+1]='\n';
	str[abhika+2]='\0';
}

int main() {

	char *PATH[10] = {"", "/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};

	// path kuch nhi? /bin/ls chalna chahiye

	// ./../yash/ls chal raha

	int pi = 5;

	// string of 10000 chars
	char *str = (char *)malloc(sizeof(char)*10000);
	
	int prompt = -1;
	char PS1[1000];

	// hisfile path
	char hisfilepath[100];
	strcat(getcwd(hisfilepath, sizeof(hisfilepath)), "/history.txt");

	while (1) {
		
		FILE *his = fopen(hisfilepath, "a+");
		
		char cwd[100];
		getcwd(cwd, sizeof(cwd));
		
		//deafult prompt
		if(prompt == -1) {
			printf("\n__ransomware__:\n");
		}
		else if(prompt == 0) {
			printf("\n%s", cwd);
		}
		else {
			printf("%s",PS1);
		}
		
		char *ctrld = fgets(str, 10000, stdin); // fgets()  reads in at most one less than size characters from stream and stores them into the buffer pointed to by s.  Reading  stops  after  an EOF  or a newline.  If a newline is read, it is stored into the buffer. A terminating null byte ('\0') is stored after the  last  character  in the buffer.
	
		trim(&str);

		fprintf(his, "%s", str);
		
		if(!ctrld || !strcmp(str, "exit\n")) {
			return 0;
		}
		
		if(!strcmp(ctrld,"\n")) {
			continue;
		}
		
		// history command pe sirf 1000 print hone chahiye
		if(!strcmp(ctrld, "history\n")) {
			fclose(his);
			int pidtemp = fork();
			if(pidtemp == 0) {
				execl("/usr/bin/cat", "/usr/bin/cat", "-n", hisfilepath, NULL);
			}
			else {
				wait(0); // wait ke andar pid pass karte child process ki; If 0 is passed as the argument, the wait system call waits for any child process to terminate
			}
			continue;
		}

		char *argv[100]; // 100 args akhri null rahega 
		// consider char *argv[2] aur  cat filename kara ton nhi chalega as expected
		// but char *argv[3] me aur cat f1 f2 kara to chalra 2no files print ho rahi !!
		// is case me argv[3] = NULL hota ; stack smashing undefined behaviour

		if(!strncmp(str, "PATH", 4)) {
			pi=1;
			char *last = strtok(str, "=");
			last = strtok(NULL, "=");
			last = strtok(last, "\n");
			last = strtok(last, ":");
			while(last) {
				PATH[pi] = (char *)malloc(sizeof(char)*1000);
				strcpy(PATH[pi], last);
				strcat(PATH[pi], "/");
				pi++;
				last = strtok(NULL, ":");
			}
			continue;
		}
		else if(!strncmp(str, "PS1", 3)) {
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

		else if (!strncmp(str, "cd", 2)) {
			char *last = strtok(str, " ");
			last = strtok(NULL, " ");
			last = strtok(last, "\n");
			chdir(last);
			continue;
		}

		int i = 0, or=-1, ir=-1, ora=-1;
		char *topi = strtok(str, "\n");
		topi = strtok(str, " ");
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
			
			argv[i] = topi;
			i++;
			topi = strtok(NULL, " ");
		}
		argv[i] = NULL;
		
		myexec(argv, PATH, or, ora, ir);
		fclose(his);
	}
}