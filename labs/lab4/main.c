#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void run_program(const char *program) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        
        printf("Дочерний процесс: PID = %d, PPID = %d, запускает программу: %s\n", getpid(), getppid(), program);
        execlp(program, program, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { 
        printf("Родительский процесс: PID = %d, создал дочерний процесс с PID = %d\n", getpid(), pid);
    }
}

int main() {
    printf("Главный процесс: PID = %d\n", getpid());

    
    run_program("ls");
    run_program("date");
    run_program("whoami");

    
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Процесс с PID = %d завершился с кодом %d\n", wpid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Процесс с PID = %d завершился сигналом %d\n", wpid, WTERMSIG(status));
        }
    }

    printf("Главный процесс завершается\n");
    return 0;
}
