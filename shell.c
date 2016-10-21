#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define FALSE 0
#define MAX 256

// init all variables 
char** init(int *len, int *bg, int *in, int *out);
// split command into an array of strings 
void parse(char **argv, char *cmd, int *len);
// get the index of '&', '>' and/or '<' in the array
void checkArgs(char **argv, int len, int *bg, int *in, int *out);
// execute the command correctly
void exec(char **argv, int *bgProcesses, int background, int fileIn, int fileOut);
// free the array of arguments
void clear(char **argv);

int main() {
  char cmd[MAX];
  char **argv;
  int len, bgProcesses=0;
  int fileIn, fileOut, background;

  while (1) {
  	// read the command
    printf("> ");
    scanf(" %[^\n]",cmd);

    // init all variables
    argv = init(&len, &background, &fileIn, &fileOut);

    // split command into an array of strings 
    parse(argv, cmd, &len);

	// get the index of '&', '>' and/or '<' in the array
    checkArgs(argv, len, &background, &fileIn, &fileOut);

    // execute the command correctly
    exec(argv, &bgProcesses, background, fileIn, fileOut);

  	// free the array of arguments
    clear(argv);
  }
}

// init all variables
char** init(int *len, int *bg, int *in, int *out) {
	int i;
	*len=0;
    *bg=FALSE;
    *in=FALSE;
    *out=FALSE;
	char **argv = (char**)malloc(MAX * sizeof(char*));
	for (i = 0; i < MAX; i++) 
	    argv[i] = (char*)malloc((MAX) * sizeof(char)); 

	return argv;
}

// split command into an array of strings 
void parse(char **argv, char *cmd, int *len) {
	char *pch;
	// break the string command in a array of arguments
    pch = strtok (cmd," ");
    while (pch != NULL){
    	strcpy(argv[*len], pch);
    	(*len)++;
    	pch = strtok (NULL, " ");
    }
    argv[*len] = NULL;
}

// get the index of '&', '>' and/or '<' in the array
void checkArgs(char **argv, int len, int *bg, int *in, int *out) {
    int i;
    for(i=0;i<len;i++) {
		if(strcmp(argv[i], "&") == 0) {
			*bg = i;
			argv[i] = NULL;
		} else 
		if(strcmp(argv[i], ">") == 0) {
			*out = i;
			argv[i] = NULL;
		} else
		if(strcmp(argv[i], "<") == 0) {
			*in = i;
			argv[i] = NULL;
		}
	}
}

// execute the command correctly
void exec(char **argv, int *bgProcesses, int background, int fileIn, int fileOut) {
   	// if first arg is 'exit'
    if (!strcmp(argv[0], "exit")) {
      exit(EXIT_SUCCESS);
    }

	// process creates a copy of itself (the parent process has pid=0)
    int pid = fork();

	// parent actions
	if (pid) {
		if(background) {
			// print the number of background processes and the pid of the new process 
			(*bgProcesses)++;
			printf("[%d] %d\n", *bgProcesses, pid);
			// since it should run in background, parent does not wait until child process ends
  		} else {
	  		// parent wait until child process ends
  			waitpid(pid, NULL, 0);
  		}
	}
	// child actions
	else {
		// if the output should be written in a file, associates the stream from stdout to the file
	    if(fileOut) {
			freopen(argv[fileOut+1], "w", stdout);
	    }
	    // if the input should be read from a file, associates the stream from stdin to the file
	    if(fileIn) {
			freopen(argv[fileIn+1] , "r", stdin);
	    }
		execvp(argv[0], argv);
		printf("Erro ao executar comando!\n");
		exit(EXIT_FAILURE);
	}
}

// free the array of arguments
void clear(char **argv) {
	int i;
	for (i = 0; i < MAX; i++)
	    free(argv[i]);
    free(argv);
}