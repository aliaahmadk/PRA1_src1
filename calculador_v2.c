/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: calculador_v2.c
 
 Alia Ahmad Kousar.
 Arantxa Ballester Ibarz.
 ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>

#define STRLONG 1024
#define FD_PIPE_SUMES_WRITE 20 // Descriptor assignat al Pas 2 per escriure sumes parcials

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

    nomPrograma = argv;
    terme_1 = atoll(argv[4]);
    terme_n = atoll(argv[5]);

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

    //PASO 2:
    if (write(FD\_PIPE\_SUMES\_WRITE, &amp;sum, sizeof(sum)) &lt; 0) 
    { 
        perror("Error en escriure el resultat a la pipeSumes"); 
        exit(1); 
    }

    close(FD\_PIPE\_SUMES\_WRITE);

    exit(0);
}
