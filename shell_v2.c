#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
int argv_position = 0;
int process_counter = 0;
int exec (char** argv);

char* read_line(void){
	int read_status = 0;
	char* user_input = NULL;
	size_t buffer_size = 0;

	read_status = getline(&user_input, &buffer_size, stdin);

	if (read_status < 0){
		printf("Error occured reading user input! \nTerminating..");
		exit(read_status);
	}
	return user_input;
}

char** parse_line(char* user_input){
	int buffer_size = 512;
	char** argv = malloc(buffer_size * sizeof(char*));
	char* arg;
	argv_position =0;
	if(!argv){
		printf("Allocation Error!\n");
		exit(-1);
	}

	arg = strtok(user_input," \t\a\n\r");

	while (arg != NULL){
		argv[argv_position] = arg;
		argv_position++;

		if (argv_position >= buffer_size){
			printf("HEY! TOO MUCH INPUT!\n");
			exit(-1);
		}

		arg = strtok(NULL," \t\a\n\r");
	}

	argv[argv_position] = NULL;
return argv;
}

int change_directory (char** argv){
	int cd_status;
	//printf("you entered Change_Directory!\n");

	if (argv[1] == NULL)
		printf("Expected argument\n");
	else {
		cd_status = chdir(argv[1]);
		if (cd_status != 0)
			printf("Not a directory!\n");
	}
	return 1;
}

int exit_shell (char** args){
	exit(0);
}

int exec_switch (char** args){

	int status;
	if (args[0] == NULL){
		// Empty command!!!!
		return 1;
	}
	if (strcmp(args[0],"cd") == 0){
		//printf("you found cd !!\n");
		change_directory(args);
		return 1;
	}
	else if (strcmp(args[0], "exit") == 0){
		exit_shell(args);
	}

	status = exec(args);
	
	
	return status;
}

int exec (char** argv){
	pid_t pid, wait_pid;
	int status,exec_status, flag = 0;

	
	//printf("%s\n", argv[argv_position - 1] );
	if (strcmp ((argv[argv_position-1]), "&") == 0){
		argv[argv_position -1] = NULL;
		flag = 1;
	}
		
	process_counter ++;
	pid = fork();
	if (pid == 0){ //This is the child
		exec_status = execvp(argv[0], argv);
		if (exec_status == -1){
			printf("There is no such command.\n");
		}
	
		exit(0);
	}
	else if (pid < 0){ // Error forking
		printf("Forking Error!\n");
	}
	else { //Parent process
		if (flag == 1){
			printf("Background Process initiated. \n");
			printf("[%d] %d\n",process_counter,pid);
			return 1; //do nothing
		}
		else {
		do {
				wait_pid = waitpid(pid, &status, WUNTRACED);

		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	
		if (wait_pid == pid){
			if (WIFEXITED(status)){
				//printf("Child ended normally.\n");
				process_counter --;
			}
			else if (WIFSIGNALED(status))
				printf("Child ended due to uncaught signal.\n");
			else if (WIFSTOPPED(status))
				printf("Child process stopped.\n");
		}
	}
}
	
return 1;
}


int main(){
	int status;
	char *user_input;
	char** args;

	do {
		printf("Shell> ");
		user_input = read_line();
		args = parse_line(user_input);
		status = exec_switch(args);

		free(user_input);
		free(args);
	}
	while (status);

	return 0;
}