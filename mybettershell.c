// fg me specific
// bg

// ctrl c 
// 2 bar ctrlz

// ctrl z jobs fg bg

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

char *command_arr[100];
int co_arr_ind = 0;

int bgflag = 0;

struct process {
	int pid;	
    char *cmd;
    int status;
};

struct process processes[100];
int n_processes = 0;
// int job_ind = 1;

// typedef struct Node{   //Node struct for storing jobs and pid and status
// 	int no;
// 	int pid;
// 	int stat; //0 - pause , 1-running
// 	char * com;
// 	struct Node * next;
// }Node;
// Node * list = NULL;
// Node * templist;
// Node * addList(Node * list,int pid,char * com)
// {
// 	if(list==NULL)
// 	{
// 		list = (Node*)malloc(sizeof(Node));
// 		list->no = job_ind++;
// 		list->pid = pid;
// 		list->com = com;
// 		list->stat = 0;
// 		list->next = NULL;
// 		return list;
// 	}
// 	Node*temp = list;
// 	while (temp && temp->next)
// 	{
// 		 temp = temp->next;
// 	}
// 	Node *p = (Node*)malloc(sizeof(Node));
// 	p->no = job_ind++;
// 	p->pid = pid;
// 	p->com = com;
// 	p->stat = 0;
// 	p->next = NULL;
// 	temp->next = p;
// 	return list;
// }
char *str;

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

int occ = 0;

void sigint_handler(int sig) {
	if(!strcmp(str, "")) {
		return;
	}
	for(int i=0; i<n_processes; i++) {
		if(processes[i].pid == pid) {
			processes[i].status = -1;
		}
	}
	kill(pid, SIGINT);
}

void sigstop_handler(int sig) {
	// 2 bar kam nhi kar raha
	int i=0;
	int temp = 0;
	for(i; i<n_processes; i++) {
		if(processes[i].status != -1) {
			temp++;
			if(processes[i].pid == pid) {
				processes[i].status = 0;
			}
		}
	}
	temp++;
	if(i==n_processes) {
		processes[n_processes].pid = pid;
		processes[n_processes].status = 0;
		processes[n_processes].cmd = (char *)malloc(sizeof(char)*100);
		strcpy(processes[n_processes].cmd, command_arr[co_arr_ind-1]);  
		n_processes++;
	}
	// printf("%dstop \n", pid);
	kill(pid, SIGSTOP);
	printf("\n[%d]+	Stopped\n", temp);

}

// void sigstop_handler(int sig) {
// 	char * temp = command_arr[co_arr_ind - 1];
	
// 	// printf("%s\n", temp);
// 	if(strncmp(temp,"fg",2) == 0 || strncmp(temp,"bg", 2) == 0) {
// 		templist->stat = 0; // 0 state will get printed in jobs 1 will not
// 	}
// 	else{
// 		list = addList(list, pid, command_arr[co_arr_ind-1]);
// 	}
// 	printf("\n");
// }

void sigcont_handler(int sig) {
	int tp = 0;
	if(sig == 0) {
		pid = processes[n_processes-1].pid; // prblm
	}
	else {
		for(int i=0; i<n_processes; i++) {
			if(processes[i].status != -1) {
				tp++;
			}
			if(tp == sig) {
				// printf("%d\n", processes[i].pid);
				pid = processes[i].pid;
				break;
			}
		}	
	}
	// printf("%dcont \n", pid);
	kill(pid, SIGCONT);
	for(int i=0; i<n_processes; i++) {
		if(processes[i].pid == pid) {
			processes[i].status = 1;
		}
	}
	int stat;
	if(!bgflag) {
		// printf("djhv\n");
		waitpid(pid, &stat, 0);
	}
}

// void printlist(Node * list) { // for jobs 
// 	Node * p = list;
// 	while(p) {
// 		if(p->stat==0) {
// 			printf("[%d]     %s\n",p->no,p->com);
// 		}
// 		p = p->next;
// 	}
// 	return ;
// }

void multipipe(int num_pipes, char *argv[11][10], char *path[]) {

	int myfds[num_pipes][2];
	
	int n = 0;

	while(n < num_pipes+1) {

		pipe(myfds[n]);

		pid = fork();

		if(pid == 0) {
        	if(n != 0) {
				close(0);
				dup(myfds[n-1][0]);
			}

			if(n != num_pipes) {
				close(1);
				dup(myfds[n][1]);
        	}

			close(myfds[n][0]);

			// printf("%s\n %s\n", argv[1][0], argv[1][1]);

			int i = 0;
			while (path[i])
			{	
				char pa[1000] = "";
				strcpy(pa, path[i]);
				strcpy(pa, strcat(pa, argv[n][0]));
				if(execv(pa, argv[n]) == -1) {
					i++;
				}
				else {
					break; // koi jarurat?
				}
			}
			printf("command not found\n");	
			// execvp(argv[n][0], argv[n]);
    	}
		else
		{
			wait(0);
			close(myfds[n][1]);
		}
		n++;
	}

}	



int main() {

	signal(SIGINT, sigint_handler);


	signal(SIGTSTP, sigstop_handler);
	// signal(SIGCONT, sigcont_handler);

	char *PATH[10] = {"", "/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};

	// path kuch nhi? /bin/ls chalna chahiye

	// ./../yash/ls chal raha

	int pi = 5;

	// string of 10000 chars
	str = (char *)malloc(sizeof(char)*10000);
	
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


		// if(ctrld != NULL) {
		// 	printf("dfdf\n");
		// 	continue;
		// }

		trim(&str);

		command_arr[co_arr_ind] = (char *)malloc(sizeof(char)*100);
		strcpy(command_arr[co_arr_ind++], str);

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

		if(!strncmp(str, "fg", 2)) {
			// fg specific process?
			if(str[3] == '%') {
				sigcont_handler(str[5] - '0');
			}
			else {
				sigcont_handler(0);
			}
			continue;
		}
		if(!strncmp(str, "bg", 2)) {
			bgflag = 1;
			if(str[3] == '%') {
				sigcont_handler(str[5] - '0');
			}
			else {
				sigcont_handler(0);
			}
			bgflag = 0;
			continue;
		}
		// if(strncmp(str,"fg",2)==0)
		// {
		// 	int nom = 0;
		// 	int i;
		// 	if (str[3]=='%')
		// 		i = 4;
		// 	else
		// 		i = 3;
		// 	int glopid;
		// 	while(str[i]!='\0')
		// 	{
		// 		int temp = str[i]-'0';
		// 		nom = nom*10 + temp;
		// 		i++;
		// 	}
		// 	templist = list;
		// 	while (templist)
		// 	{
		// 		if(templist->no == nom && templist->stat==0)
		// 		{
		// 			glopid = templist->pid;
		// 			templist->stat = 1;	
		// 			break;
		// 		}
		// 		templist = templist->next;
		// 	}
		// 	int pr;
		// 	kill(glopid, SIGCONT);
		// 	waitpid(glopid, &pr,WUNTRACED);
		// 	continue;
		// }
		// else if(strncmp(str,"bg",2) == 0)
		// {
		// 	int nom = 0;
		// 	int i;
		// 	if (str[3]=='%')
		// 		i = 4;
		// 	else
		// 		i = 3;
		// 	int glopid;
		// 	while(str[i]!='\0')
		// 	{
		// 		int temp = str[i]-'0';
		// 		nom = nom*10 + temp;
		// 		i++;
		// 	}
		// 	templist = list;
		// 	while (templist) {
		// 		if(templist->no == nom && templist->stat==0)
		// 		{
		// 			glopid = templist->pid;
		// 			templist->stat = 1;	
		// 			break;
		// 		}
		// 		templist = templist->next;
		// 	}
		// 	kill(glopid, SIGCONT);
		// 	bgflag = 1;
		// 	wait(0);
		// 	continue;
		// }
		else if(!strncmp(str, "jobs", 4)) {
			// printf("%dn_proc", n_processes);
			int tempjobind = 0;
			for(int i=0; i<n_processes; i++) {
				char *temp;
				if(processes[i].status == -1) {
					continue;
				}
				else if(processes[i].status==0) {
					temp = "Stopped";
				} 
				else {
					temp = "Running";
				}
				tempjobind++;
				printf("[%d]+	%s		%s\n", tempjobind, temp, processes[i].cmd);
			}
			continue;
		}
		// else if(strcmp(str, "jobs\n") == 0) {
		// 	// printf("sdv\n");
		// 	printlist(list);
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
		
		// char *tempargs[11][10] = {
		// 	{"cat","info.txt",NULL},
		// 	{"head", "-4",NULL}
		// };


		if(ind != 0) {
			char *newargs[11][10];
			
			int k=0, i=0;
			for(int k=0; k<ind+1; k++) {
				int j=0;
				// while(argv[i] != "|" /* not working */ && argv[i] != NULL) {
				// 	printf("%s\n", argv[i]);
				// 	newargs[k][j] = argv[i];
				// 	i++;
				// 	j++;
				// }

				while(i != mypipesloc[k] && argv[i] != NULL) {
					newargs[k][j] = argv[i];
					i++;
					j++;
				}
				newargs[k][j]=NULL;
				i++;
			}
			
			multipipe(ind, newargs, PATH);
			// multipipe(ind, tempargs);
		}
		else {
			myexec(argv, PATH, or, ora, ir);
		}
		fclose(his);
	}
}

