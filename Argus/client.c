#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define C2S "/tmp/c2s"
#define S2C "/tmp/s2c"

int c2s()
{
    int c2s;
    if ((c2s = open(C2S, O_WRONLY)) == -1)
    {
        perror(C2S);
        return EXIT_FAILURE;
    }
    return c2s;
}

int s2c()
{
    int s2c;
    if ((s2c = open(S2C, O_RDONLY)) == -1)
    {
        perror(S2C);
        return EXIT_FAILURE;
    }
    return s2c;
}

int main()
{
    int fd, fd2, n, n2;
    char buffer[512], buffer2[512], c[512];
    int dados;

    fd = c2s();
    fd2 = s2c();

    while (1)
    {
        puts("Write a command:");
        n = read(STDIN_FILENO, buffer, sizeof(buffer));
        write(fd, buffer, n);
        memset(buffer, 0, sizeof(buffer));

        //puts("Read:");
        n2 = read(fd2, buffer2, sizeof(buffer2));
        write(STDOUT_FILENO, buffer2, n2);
        //close(fd2);
    }

    close(fd);
    close(fd2);
    return 0;
}