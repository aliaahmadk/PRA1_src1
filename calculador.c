#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>

#define STRLONG 1024

char *color_green = "\033[01;32m";
char *color_end = "\033[00m";

int main(int argc, char *argv[])
{
    long int i;
    long long sum = 0, sumForm, terme_i, terme_i_mes_1, terme_1, terme_n;
    char *nomPrograma;
    char cadena[STRLONG];

    if (argc != 3)
    {
        printf("Us: %s <1r terme> <darrer terme>\n\n", argv[0]);
        exit(1);
    }

    nomPrograma = argv[0];
    terme_1 = atoll(argv[1]);
    terme_n = atoll(argv[2]);

    sprintf(cadena, "%s%s - %d> Calcula progressio termes: %.0lld -> %.0lld %s\n", color_green, nomPrograma, getpid(), terme_1, terme_n, color_end);
    write(1, cadena, strlen(cadena));

    terme_i = terme_1;
    terme_i_mes_1 = terme_i + 1;
    sum = terme_i;

    for (i = terme_1; i < terme_n; i++)
    {
        sum = sum + terme_i_mes_1;
        terme_i = terme_i_mes_1;
        terme_i_mes_1 = terme_i_mes_1 + 1;
    }

    sumForm = ((terme_1 + terme_n) / 2.0) * (terme_n + 1 - terme_1);

    sprintf(cadena, "%s%s - %d> Suma amb bucle=%.0lld - Suma amb formula=%.0lld%s\n", color_green, nomPrograma, getpid(), sum, sumForm, color_end);
    write(1, cadena, strlen(cadena));

    return EXIT_SUCCESS;
}