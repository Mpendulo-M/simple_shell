#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#define ARGS 100
#define MAX_LEN 1024


int main()
{
    char userInput[MAX_LEN];
    char *args[ARGS];
    int i = 0;
    int i_fd, o_fd;
    int r_input = 0, r_output = 0;

    while (1) {
        printf("$ ");
        if (fgets(userInput, sizeof(userInput), stdin) == NULL)
        {
            if (feof(stdin)) {
                printf("\n");
                break;
            } else {
                perror("fgets");
                exit(1);
            }
        }


        userInput[strcspn(userInput, "\n")] = '\0';

        
        
        args[i] = strtok(userInput, " ");
        while (args[i] != NULL && i < ARGS - 1)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        
        char *input_file = NULL, *output_file = NULL;

        for (int j = 0; j < i; j++) {
            if (strcmp(args[j], "<") == 0) {
                r_input = 1;
                input_file = args[j + 1];
                args[j] = NULL;
            } else if (strcmp(args[j], ">") == 0) {
                r_output = 1;
                output_file = args[j + 1];
                args[j] = NULL;
            }
        }

        if (r_input) {
            i_fd = open(input_file, O_RDONLY);
            if (i_fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(i_fd, STDIN_FILENO);
            close(i_fd);
        }

        if (r_output) {
            o_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (o_fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(o_fd, STDOUT_FILENO);
            close(o_fd);
        }


        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
        } else if (child_pid == 0)
        {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(1);
            }
        }
        else
        {
        
            int status;
            waitpid(child_pid, &status, 0);
        }
    }

    return 0;
}
