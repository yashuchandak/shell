#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int myexec(char *argv[], char *path[], int or, int ora, int ir, int kahatk)
{
	int pid = fork();
	if (pid == 0)
	{
		// setsid(); // agar child ise run karta to ye kam karao
		if (or != -1)
		{
			close(1);
			int fd = open(argv[or + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			argv[or] = NULL;
		}
		else if (ora != -1)
		{
			close(1);
			int fd = open(argv[ora + 1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			argv[ora] = NULL;
		}

		if (ir != -1)
		{
			close(0);
			int fd = open(argv[ir + 1], O_RDONLY, S_IRUSR | S_IWUSR);
			argv[ir] = NULL;
		}

		int i = 0;
		while (i < kahatk)
		{
			char pa[1000] = "";
			strcpy(pa, path[i]);
			strcpy(pa, strcat(pa, argv[0]));

			if (execv(pa, argv) == -1)
			{
				i++;
			}
			else
			{
				break;
			}
		}
		printf("command not found\n");
	}
	else
	{ // parent to wait karo child ke liye
		wait(0);
	}
	return 0;
}

void trim(char **strp)
{

	char *str = *strp;

	while (str[0] == ' ')
	{
		str++;
	}

	int abhika = strlen(str) - 2; // "  \n\0"
	while (str[abhika] == ' ')
	{
		abhika--;
	}
	str[abhika + 1] = '\n';
	str[abhika + 2] = '\0';
}

int multipipe(char *argv[], char *path[], int kahatk, int ispipe[])
{
	
}

int main()
{

	char fullhispath[100];
	getcwd(fullhispath, sizeof(fullhispath));

	char hisfilename[20] = "history.txt";
	strcat(fullhispath, hisfilename);

	char *PATH[10] = {"", "/usr/bin/", "/usr/sbin/", "/usr/games/", "/snap/bin/"};
	int pi = 5;
	char *str = (char *)malloc(sizeof(char) * 10000);
	int prompt = 0;
	char PS1[1000];

	while (1)
	{

		FILE *his = fopen("history.txt", "a+");

		char cwd[1000];
		getcwd(cwd, sizeof(cwd));

		if (prompt == 0)
		{
			// printf("fv\n");
			printf("\n__yash__%s__:\n$", cwd);
		}
		else
		{
			printf("%s", PS1);
		}

		char *ctrld = fgets(str, 10000, stdin); // fgets()  reads in at most one less than size characters from stream and stores them into the buffer pointed to by s.  Reading  stops  after  an EOF  or a newline.  If a newline is read, it is stored into the buffer. A terminating null byte ('\0') is stored after the  last  character  in the buffer.

		trim(&str);

		fprintf(his, "%s", str);

		if (!ctrld || !strcmp(str, "exit\n"))
		{
			return 0;
		}

		if (!strcmp(ctrld, "\n"))
		{
			continue;
		}

		if (!strcmp(ctrld, "history\n"))
		{
			fclose(his);
			int pidtemp = fork();
			if (pidtemp == 0)
			{
				execl("/usr/bin/cat", "/usr/bin/cat", "-n", "history.txt", NULL);
			}
			else
			{
				wait(0);
			}
			continue;
		}

		char *argv[1000]; // akhri null rahega
		// consider char *argv[2] aur  cat filename kara ton nhi chalega as expected
		// but char *argv[3] me aur cat f1 f2 kara to chalra 2no files print ho rahi !!
		// is case me argv[3] = NULL hota ; stack smashing undefined behaviour
		int dotslash = 0;
		if (!strncmp(str, "./", 2))
		{
			int i = 2;
			for (i; i < strlen(str); i++)
			{
				str[i - 2] = str[i];
			}
			str[i - 2] = '\0';
			PATH[0] = (char *)malloc(sizeof(char) * 1000);
			strcpy(PATH[0], cwd);
			strcat(PATH[0], "/");
			dotslash = 1;
		}
		else if (!strncmp(str, "../", 3))
		{
			int i = 3;
			for (i; i < strlen(str); i++)
			{
				str[i - 3] = str[i];
			}
			str[i - 3] = '\0';
			PATH[0] = (char *)malloc(sizeof(char) * 1000);
			char temppath[1000]; //(char *)malloc
			chdir("..");
			getcwd(temppath, sizeof(temppath));
			chdir(cwd);
			strcpy(PATH[0], temppath);
			strcat(PATH[0], "/");
			dotslash = 1;
		}

		else if (!strncmp(str, "PATH", 4))
		{
			pi = 1;
			char *last = strtok(str, "=");
			last = strtok(NULL, "=");
			last = strtok(last, "\n");
			last = strtok(last, ":");
			while (last)
			{
				PATH[pi] = (char *)malloc(sizeof(char) * 1000);
				strcpy(PATH[pi], last);
				strcat(PATH[pi], "/");
				pi++;
				last = strtok(NULL, ":");
			}
			continue;
		}
		else if (!strncmp(str, "PS1", 3))
		{
			char *last = strtok(NULL, "=");
			last = strtok(last, "\"");
			if (!strcmp(last, "\\w$"))
			{
				prompt = 0;
			}
			else
			{
				prompt = 1;
				strcpy(PS1, last);
			}
		}

		else if (!strncmp(str, "cd", 2))
		{
			char *last = strtok(str, " ");
			last = strtok(NULL, " ");
			last = strtok(last, "\n");
			chdir(last);
			continue;
		}

		// int i = 0, or=-1, ir=-1, ora=-1;
		// char *topi = strtok(str, "\n");
		// topi = strtok(str, " ");
		// while (topi) {
		// 	if(!strcmp(topi, ">")) {
		// 		or=i;
		// 	}
		// 	else if(!strcmp(topi, "<")) {
		// 		ir=i;
		// 	}
		// 	else if(!strcmp(topi, ">>")) {
		// 		ora=i;
		// 	}

		// 	argv[i] = topi;
		// 	i++;
		// 	topi = strtok(NULL, " ");
		// }
		// argv[i] = NULL;

		// if(dotslash == 1) {
		// 	myexec(argv, PATH, or, ora, ir, 1);
		// 	PATH[0] = "";
		// 	dotslash = 0;
		// }
		// else {
		// 	myexec(argv, PATH, or, ora, ir, pi);
		// }

		int i = 0, or = -1, ir = -1, ora = -1, ispipe[1];
		ispipe[0] = -1;
		char *topi = strtok(str, "\n");
		topi = strtok(str, " ");
		while (topi)
		{
			if (!strcmp(topi, ">"))
			{
				or = i;
			}
			else if (!strcmp(topi, "<"))
			{
				ir = i;
			}
			else if (!strcmp(topi, ">>"))
			{
				ora = i;
			}
			else if (!strcmp(topi, "|"))
			{
				ispipe[0] = i;
			}
			argv[i] = topi;
			i++;
			topi = strtok(NULL, " ");
		}
		argv[i] = NULL;

		if (ispipe[0] != -1)
		{
			multipipe(argv, PATH, pi, ispipe);
			// printf("step1\n");
			fclose(his);
			continue;
		} // pi ka jhamela

		if (dotslash == 1)
		{
			myexec(argv, PATH, or, ora, ir, 1);
			PATH[0] = "";
			dotslash = 0;
		}
		else
		{
			myexec(argv, PATH, or, ora, ir, pi);
		}

		fclose(his);
	}
}

// ispipes
// cat info.txt | grep e
//