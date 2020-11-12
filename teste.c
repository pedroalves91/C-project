#include <unistd.h>    /* declarações de chamadas ao sistema */
#include <sys/types.h> /* definições de tipos do sistema */
#include <sys/stat.h>  /* decls e defs necessárias para named pipes */
#include <stdlib.h>    /* EXIT_* */
#include <stdio.h>     /* perror(), puts() */
#include <fcntl.h>     /* O_* */
#include <string.h>
#include <time.h>
#include <signal.h>

#define C2S "/tmp/c2s" /* pipe de comando */
#define S2C "/tmp/s2c"

int main()
{
    unlink(C2S);
    unlink(S2C);
    //printf("PROGRAMA TESTE\n");
    //while(1){}
    return 0;
}