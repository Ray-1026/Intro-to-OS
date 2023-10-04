/*
Student No.: 110550093
Student Name: 蔡師睿
Email: srt093.cs10@nycu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define Pipe 1
#define Redirect_to 2
#define Redirect_from 3
#define BUFFER 256

using namespace std;

void sigchld_handler(int sig)
{
    int stat;
    pid_t pid = waitpid(0, &stat, WNOHANG);
}

void tokenize(char *input, char **left_cmd, char **right_cmd, bool &wait_child, int &cmd_status)
{
    bool div = false;
    string lastchar_l, lastchar_r;
    char *token = strtok(input, " ");

    while (token != NULL) {
        if (!strcmp(token, "|") || !strcmp(token, ">") || !strcmp(token, "<")) {
            cmd_status = (!strcmp(token, "|")) ? (Pipe) : ((!strcmp(token, ">")) ? (Redirect_to) : (Redirect_from));
            div = true;
            token = strtok(NULL, " ");
            continue;
        }

        if (!div) {
            lastchar_l = token;
            *left_cmd++ = token;
        }
        else {
            lastchar_r = token;
            *right_cmd++ = token;
        }
        token = strtok(NULL, " ");
    }
    if (lastchar_l == "&") {
        wait_child = false;
        *left_cmd--;
    }
    *left_cmd = NULL;

    if (div) {
        if (lastchar_r == "&")
            *right_cmd--;
        *right_cmd = NULL;
    }
}

void execute(pid_t &pid, char *input, bool &wait_child)
{
    int cmd_status = 0;
    char *left_cmd[BUFFER], *right_cmd[BUFFER];

    tokenize(input, left_cmd, right_cmd, wait_child, cmd_status);

    pid = fork();
    if (pid == 0) {
        // child
        switch (cmd_status) {
        case Pipe:
            break;
        case Redirect_to: {
            int fd = open(right_cmd[0], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            break;
        }
        case Redirect_from: {
            int fd = open(right_cmd[0], O_RDONLY, S_IRUSR | S_IWUSR);
            dup2(fd, STDIN_FILENO);
            close(fd);
            break;
        }
        default:
            break;
        }
        execvp(left_cmd[0], left_cmd);
        exit(1);
    }
    else if (pid > 0) {
        // parent
        (wait_child) ? (wait(NULL)) : (waitpid(-1, NULL, WNOHANG));
    }
    else {
        fprintf(stderr, "Fork Failed");
        exit(-1);
    }
}

int main()
{
    pid_t pid;
    char input[BUFFER];

    signal(SIGCHLD, sigchld_handler); // handle zombie processes with SIGCHLD

    printf(">");
    while (fgets(input, BUFFER, stdin)) {
        input[strlen(input) - 1] = '\0';
        if (strcmp(input, "exit") == 0)
            break;

        bool wait_child = true;
        execute(pid, input, wait_child);
        printf(">");
    }
}
