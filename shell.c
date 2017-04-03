#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>



#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_DELIM2 ";"
#define LSH_TOK_DELIM3 "|"

int lsh_cd(char **args);
int lsh_echo(char **args);
int lsh_exit(char **args);
int lsh_pwd(char **args);

int flag=0,l,k;
pid_t pid;
char **args;
int flag,background,st;
char *tmp1;
char *tmp2;    
char **pi;
int pipefds[20];


char *builtin_str[] = {
	"cd",
	"echo",
	"pwd",
	"exit"
};




void signalHandler(int signal)
{
	//    sleep(1);

	st=1;
	if (signal==SIGCHLD && background==1) {
		if (args[1]!=NULL)
			printf("Process %s %s with pid %d exited normally\n",args[0],args[1],pid);
		else
			printf("Process %s with pid %d exited normally\n",args[0],pid);
		wait(NULL);
	}
}



int (*builtin_func[]) (char **) = {
	&lsh_cd,
	&lsh_echo,
	&lsh_pwd,
	&lsh_exit
};

int lsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

/*
   Builtin function implementations.
 */
int lsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("lsh");
		}
	}
	return 1;
}

int lsh_echo(char **args)
{

	printf("%s ", args[1]);
	printf("\n");
	return 1;
}

int lsh_pwd(char **args)
{
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		fprintf(stdout, "%s", cwd);
	else
		perror("getcwd() error");
	printf("\n");
	return 1;
}


int lsh_exit(char **args)
{
	return 0;
}

/*void proc_exit()
  {
//	int wstat;
union wait wstat;
pid_t	pid;
if (flag==1)
{
while (1) {
pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
if (pid == 0)
return;
else if (pid == -1)
return;
else
printf ("exited normally\n");
}
}
}*/





int lsh_launch(char **args)
{

	int out,in,i;
	background = flag;
	signal(SIGCHLD,signalHandler);
	pid = fork();

	if(pid == -1) {
		perror("fork");
		exit(1);
	}

	if(pid == 0) {
		if (l==0)
		{
			dup2(pipefds[1],1);
		}
		else if (l!=0 && pi[l+1]!=NULL)
		{
			dup2(pipefds[2*i],0);
			dup2(pipefds[2*i+3],1);
		}
		else if (pi[l+1]==NULL)
		{
			dup2(pipefds[2*(k-1)-2],0);
		}
		for(i = 0; i < 2*(k-1); i++){
			close(pipefds[i]);
		}
		if (st ==1)
		{
			out = open(tmp1,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out,STDOUT_FILENO);
			close(out);
		}
		else if (st==2)
		{
			in = open(tmp1,O_RDONLY);
			dup2(in, 0);
			close(in);
		}

		else if (st==3)
		{
			in = open(tmp1, O_RDONLY);
			out= open(tmp2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

			dup2(in, 0);


			dup2(out, 1);


			close(in);
			close(out);
		}

		execvp(args[0], args);
		exit(0);
	}

//	if(!background) 
//		wait(NULL);

	//		exit(0);
	return 1;
}



int lsh_execute(char **args)
{
	int i;

	if (args[0] == NULL) {
		return 1;
	}

	for (i = 0; i < lsh_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return lsh_launch(args);
}


char **lsh_split_line2(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM3);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM3);
	}
	tokens[position] = NULL;
	return tokens;
}




char **lsh_split_line1(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM2);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM2);
	}
	tokens[position] = NULL;
	return tokens;
}



char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0,l,i;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}


	token = strtok(line, LSH_TOK_DELIM);
	/*	l=strlen(token);
		for(i=0;i<l;i++)
		{
		if (token[i] == '&')
		flag=1;

		}
		printf("%d\n",flag);   */     
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}



/*char *lsh_read_line(void)
  {
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
  fprintf(stderr, "lsh: allocation error\n");
  exit(EXIT_FAILURE);
  }

  while (1) {
  c = getchar();

  if (c == EOF || c == '\n') {
  buffer[position] = '\0';
  return buffer;
  } else {
  buffer[position] = c;
  }
  position++;

  if (position >= bufsize) {
  bufsize += LSH_RL_BUFSIZE;
  buffer = realloc(buffer, bufsize);
  if (!buffer) {
  fprintf(stderr, "lsh: allocation error\n");
  exit(EXIT_FAILURE);
  }
  }
  }
  }*/


char *lsh_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	getline(&line, &bufsize, stdin);
	return line;
}




void lsh_loop(void)
{
	char *line;
	char **args2;
	int status=1,i,j,p,stat;


	while (status!=0)
	{
		i=0;

		printf("> ");
		line = lsh_read_line();
		args2 = lsh_split_line1(line);

		while(args2[i]!= NULL)
		{

			flag=0;
			st=0;
			l=0;
			k=0;
			pi = lsh_split_line2(args2[i]);
			while(pi[k]!=NULL)
				k++;
			//	printf("%d\n",k);
			for( p = 0; p < k-1; p++ ){
				if( pipe(pipefds + p*2) < 0 ){
					perror("could'nt pipe"); 
					exit(EXIT_FAILURE);
				}
			}
			while(pi[l]!=NULL)
			{
				args = lsh_split_line(pi[l]);
				//		printf("%s\n",*args);
				j=0;
				while(args[j]!=NULL)
				{
					if (*args[j] == '>' && st==0)
						st=1;

					else if (*args[j] == '<' && st==0)
						st=2;

					//	else if (*args[j] == '<' && st==1)
					//		st=3;

					else if (*args[j] == '>' && st==2)
						st=3;
					if (st==1 && args[j+1]!=NULL && *args[j] == '>' || st==2 && args[j+1]!=NULL && *args[j] == '<')
					{
						tmp1 = args[j+1];
						args[j]=NULL;
					}

					if (st==3 && args[j+1]!=NULL && *args[j] == '>')
					{
						tmp2 = args[j+1];
						args[j]=NULL;
					}

					j++;
				}
				if (*args[j-1] == '&')
				{

					flag=1;
					args[j-1]=NULL;
				}
				status = lsh_execute(args);


				free(args);
				l++;
			}
			for(p = 0; p < 2 * (k-1); p++){
				close(pipefds[p]);
			}
			for(p = 0; p < k; p++)
				wait(NULL);
			i++;

		}
		//	free(line);
	}
}


int main(int argc, char **argv)
{
	lsh_loop();


	return EXIT_SUCCESS;
}

