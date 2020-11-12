#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define C2S "/tmp/c2s"
#define S2C "/tmp/s2c"

int c2s()
{
    int c2s;
    if (access(C2S, F_OK) == -1)
    {
        if (mkfifo(C2S, 0666) == -1)
        {
            perror(C2S);
            return EXIT_FAILURE;
        }
    }
    if ((c2s = open(C2S, O_RDONLY)) == -1)
    {
        perror(C2S);
        return EXIT_FAILURE;
    }
    return c2s;
}

int s2c()
{
    int s2c;
    if (access(S2C, F_OK) == -1)
    {
        if (mkfifo(S2C, 0666) == -1)
        { /* criar fifo */
            perror(S2C);
            return EXIT_FAILURE;
        }
    }
    if ((s2c = open(S2C, O_WRONLY)) == -1)
    { /* abrir fifo para escrita */
        perror(S2C);
        return EXIT_FAILURE;
    }
    return s2c;
}

void clear()
{
    for (int i = 0; i < 5; i++)
    {
        printf("\n");
    }
}

void tempo(int signo)
{
    //int pid = getpid();
    //printf("PID %d",pid);
    puts("Time\n");
}

void proc_exit()
{
    int wstat;
    union wait wstat2;
    pid_t pid;
    while (1)
    {
        pid = wait3(&wstat, WNOHANG, (struct rusage *)NULL);
        if (pid == 0)
            return;
        else if (pid == -1)
            return;
    }
}

void historico(char *action, int size)
{
    int fd1;
    fd1 = open("history.txt", O_CREAT | O_WRONLY | O_APPEND, 0777);

    write(fd1, action, size);
    if ((strcmp(action, "Execute:") != 0) && (strcmp(action, "Exit:") != 0))
    {
        write(fd1, "\n", sizeof("\n"));
    }
    close(fd1);
}

int command(char *cmd, int size, int timer, int fd)
{
    pid_t pid;

    signal(SIGALRM, tempo);
    signal(SIGCHLD, proc_exit);
    //alarm(timer);
    //puts(cmd);
    switch (pid = fork())
    {
    case -1:
        return -1;
    case 0:
        //alarm(timer);
        if (cmd[0] == '-')
        {
            //puts("server aqui");
            if (cmd[1] == 'l')
            {
                clear();
                puts("Listing tasks request");
                // Listar
                write(fd, "Listing tasks!\n", sizeof("Listing tasks!!\n"));
                execlp("ps", "ps", "-l", NULL);
                perror("Error in execlp\n");
                _exit(1);
            }

            else if (cmd[1] == 'e')
            {
                clear();
                puts("Execute task request");
                write(fd, "Executing task!\n", sizeof("Executing task!\n"));

                // Executar

                char tasks[size];

                // Eliminar o -e do comando
                int i = 3;
                int j = 0;
                while (cmd[i] != '\0')
                {
                    tasks[j] = cmd[i];
                    i++;
                    j++;
                }
                tasks[j] = '\0';

                char r[] = "Run:";

                historico(r, sizeof(r) - 1);
                historico(tasks, sizeof(tasks) - 4);

                //puts(tasks);

                // contar pipes
                int num_pipes = 0;
                for (int x = 0; x < sizeof(tasks); x++)
                {
                    if (tasks[x] == '|')
                    {
                        num_pipes++;
                    }
                }

                //printf("Pipes: %d\n", num_pipes);

                char *execs[10][5] = {NULL};

                char *token, *cmmd[10], *token2;
                char s[2] = "|", s2[2] = " ";
                int h = 0, t;
                int w, aux2;

                // Separar pelo caracter: |
                token = strtok(tasks, s);
                while (token != NULL)
                {
                    //Remove o primeiro espaço se necessário
                    if (token[0] == ' ')
                    {
                        for (w = 0; w < strlen(token); w++)
                        {
                            token[w] = token[w + 1];
                        }
                    }
                    //Remove o último espaço se necessário
                    if (token[strlen(token) - 1] == ' ' || token[strlen(token) - 1] == '\n')
                    {
                        token[strlen(token) - 1] = '\0';
                    }

                    // cada token tem um comando completo exemplo: ls -l
                    // e é guardado numa posição do array
                    cmmd[h] = token;
                    h++;
                    token = strtok(NULL, s);
                }

                // Para cada comando completo guardado separar pelo espaço
                for (t = 0; t < h; t++)
                {
                    token2 = strtok(cmmd[t], s2);
                    aux2 = 0;
                    while (token2 != NULL)
                    {
                        // Exemplo linha 0: ls na coluna 0; -l na coluna 1
                        // Cada linha tem um comando completo
                        execs[t][aux2] = token2;
                        aux2++;
                        token2 = strtok(NULL, s2);
                    }
                }

                int fdesc[2], fdAux = 0;
                int npipes = num_pipes + 1;
                pid_t p;

                for (int x = 0; x < npipes + 1; x++)
                {
                    //criar pipe para cada |
                    //printf("x: %d\n",x);
                    pipe(fdesc);
                    p = fork();
                    if (p == 0)
                    {
                        alarm(timer);
                        //puts("IF");
                        if (x < npipes - 1)
                        {
                            close(fdesc[0]);   //fecha fd de leitura
                            dup2(fdesc[1], 1); //stdout para fd de escrita no ultimo nao entra aqui
                        }
                        dup2(fdAux, 0); //stdin para fd de leitura
                        if (execvp(execs[x][0], execs[x]) == -1)
                        {
                            perror(execs[x][0]);
                        }
                    }
                    else
                    {
                        //puts("ELSE");
                        close(fdesc[1]);  //fechar fd de escrita
                        fdAux = fdesc[0]; //fdaux toma valor do fd de leitura
                        wait(&p);
                    }
                }

                //write(fd, "Executando tarefa!\n", sizeof("Executando tarefa!\n"));
            }

            else if (cmd[1] == 't')
            {
                clear();
                puts("Finish task request");

                // Terminar

                int pid_pr = 0;
                char id[512];
                int i = 3;
                int j = 0;

                while (cmd[i] != '\0')
                {
                    id[j] = cmd[i];
                    i++;
                    j++;
                }
                pid_pr = atoi(id);

                if (kill(pid_pr, 9) == -1)
                {
                    perror("Error ending process!\n");
                    _exit(1);
                }
                else
                {
                    char r[] = "Finish:";
                    historico(r, sizeof(r) - 1);
                    historico(id, sizeof(id) - 1);
                    write(fd, "Process finished!\n", sizeof("Process finished!\n"));
                }
            }

            else if (cmd[1] == 'r')
            {
                clear();
                puts("Check history request");

                // Histórico
                int file = open("history.txt", O_RDONLY);
                if (file == -1)
                {
                    printf("ERROR\n");
                }
                char c[500];
                int n;
                memset(c, 0, sizeof(c));
                while ((n = read(file, c, sizeof(c))) > 0)
                {
                    write(STDOUT_FILENO, c, n);
                }
                close(file);

                write(fd, "Checking history!\n", sizeof("Checking history!\n"));
            }

            else if (cmd[1] == 'h')
            {
                clear();

                // Manual de ajuda
                write(fd, "Access user manual!!\n", sizeof("Access user manual!!\n"));

                puts("\t\t\tUSER MANUAL\n");
                puts("Communication inactivity time \t Command: -i n (n=time)\n");
                puts("Execution time \t Command: -m n (n=time)\n");
                puts("Run a task \t Command: -e p1|p2...|pn\n");
                puts("Listing tasks \t Command: -l\n");
                puts("Listing history \t Command: -r\n");
                puts("Exiting a task \t Command: -t n (n == task id)\n");
            }

            else
            {
                //puts("inv1");
                // Comando Inválido
                write(fd, "Invalid command, type '-h' for help!!", sizeof("Invalid command, type '-h' for help!!"));
            }
        }
        else
        {
            //puts("inv2");
            // Comando Inválido
            write(fd, "Invalid command, type '-h' for help!!", sizeof("Invalid command, type '-h' for help!!"));
        }
    }
}

int main()
{
    int fd, fd2, n, indice;
    char buffer[512];
    int timer = 0;

    fd = c2s();
    fd2 = s2c();

    while (1)
    {
        //puts("Waiting for a command:");
        n = read(fd, buffer, sizeof(buffer));
        //write(STDOUT_FILENO, buffer, n);
        if (n > 0)
        {
            //printf("N: %d\n", n);

            if (buffer[0] == '-' && buffer[1] == 'm')
            {
                char timeAux[512];
                int i = 3;
                int j = 0;
                while (buffer[i] != '\0')
                {
                    timeAux[j] = buffer[i];
                    i++;
                    j++;
                }
                timer = atoi(timeAux);
                if (timer > 0)
                {
                    write(fd2, "Maximum time defined\n", sizeof("Maximum time defined\n"));
                }
                else
                {
                    write(fd2, "You need to define a execution time!! Type '-h' for help!!\n", sizeof("You need to define a execution time!! Type '-h' for help!!\n"));
                }
            }
            else
            {
                if (timer > 0)
                {
                    command(buffer, n, timer, fd2);
                }
                else
                {
                    write(fd2, "Execution time not defined!!!\n", sizeof("Execution time not defined!!!\n"));
                }
            }

            //printf("MAX TEMPO: %d\n", timer);
            memset(buffer, 0, sizeof(buffer));
        }
    }

    close(fd);
    close(fd2);
    return 0;
}