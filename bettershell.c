#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int pid = INT_MIN;

struct process {
    int pid;
    char cmd[100];
    int status;
};

struct process processes[100];
int n_processes = 0;

int myexec(char *argv[], char *path[], int or, int ora, int ir) {
	
	pid = fork(); // On success, the PID of the child process is returned in the parent, and 0  is returned in the child.  On failure, -1 is returned in the parent, no child process is created,
	

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
	else { 
		int wstatus;
		// idhar kuch problem
		waitpid(-1, &wstatus, WUNTRACED);
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

void sigint_handler(int sig) {
	kill(pid, SIGINT);
}

// void sigstop_handler(int sig) {
// 	// 2 bar kam nhi kar raha
// 	int i=0;
// 	int temp;
// 	for(i; i<n_processes; i++) {
// 		if(processes[i].pid == pid) {
// 			processes[i].status = 0;
// 		}
// 	}
// 	temp = i+1;
// 	if(i==n_processes) {
// 		processes[n_processes].pid = pid;
// 		processes[n_processes].status = 0;
// 		n_processes++;
// 	}

// 	kill(pid, SIGSTOP);
// 	printf("\n[%d]+	Stopped", temp);

// }

// void sigcont_handler(int sig) {
// 	int pid = processes[n_processes].pid;
// 	kill(pid, SIGCONT);
// 	for(int i=0; i<n_processes; i++) {
// 		if(processes[i].pid == pid) {
// 			processes[i].status = 1;
// 		}
// 	}
// 	int stat;
// 	waitpid(pid, &stat,0);
// }

void multipipe(char *argv[10][100], int num_pipes) {
	
	int myfds[num_pipes][2];
	
	for(int i = 0; i < num_pipes; i++) {
    	pipe(myfds[i]);
	}
	
	int n = 0;
	while(n < num_pipes + 1) {
    	
		pid = fork();

		if(pid == 0) {
        	if(n != 0) {
				close(0);
				dup(myfds[n-1][0]);
			}

			if(n != num_pipes-1) {
				close(1);
				dup(myfds[n][1]);
        	}
        
			execvp(argv[n][0], argv[n]);
    	}
	}

	for( int i = 0; i < num_pipes; i++ ) {
   		close(myfds[i][0]);
		close(myfds[i][1]);
	}

}

int main() {

	signal(SIGINT, sigint_handler);
	// signal(SIGTSTP, sigstop_handler);
	// signal(SIGCONT, sigcont_handler);

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
		
		// deafult prompt
		if(prompt == -1) {
			printf("\n__ransomware__:\n");
		}
		else if(prompt == 0) {
			printf("\n%s$", cwd);
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

		// if(!strncmp(str, "fg", 2)) {
		// 	// fg specific process?
		// 	sigcont_handler(0);
		// 	continue;
		// }
		// else if(!strncmp(str, "jobs", 4)) {
		// 	for(int i=0; i<n_processes; i++) {
		// 		printf("[%d]+	%d\n", i+1, processes[i].pid);
		// 	}
		// 	continue;
		// }

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
			if(!last) {
				PATH[1]=NULL;
			}
			while(last) {
				PATH[pi] = (char *)malloc(sizeof(char)*1000);
				strcpy(PATH[pi], last);
				strcat(PATH[pi], "/");
				pi++;
				last = strtok(NULL, ":");
			}
			PATH[pi]=NULL;
			continue;
		}
		else if(!strncmp(str, "PS1", 3)) {
			char *last = strtok(str, "\"");
			last = strtok(NULL, "\"");
			if(!strcmp(last, "\\w$")) {
				prompt=0;
			}
			else {
				prompt = 1;
				strcpy(PS1, last);
			}
			continue;
		}

		else if (!strncmp(str, "cd", 2)) {
			char *last = strtok(str, " ");
			last = strtok(NULL, " ");
			last = strtok(last, "\n");
			chdir(last);
			continue;
		}

		int i = 0, or=-1, ir=-1, ora=-1, ind = 0, mypipesloc[10];
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
			else if(!strcmp(topi, "|")) {
				mypipesloc[ind] = i;
				ind++;
			}

			argv[i] = topi;
			i++;
			topi = strtok(NULL, " ");
		}
		argv[i] = NULL;
		
		// if(ind != 0) {
		// 	multipipe({{ls | }}, ind);
		// }

		myexec(argv, PATH, or, ora, ir);
		fclose(his);
	}
}
