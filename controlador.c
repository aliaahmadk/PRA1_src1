#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <sys/wait.h>

#include <string.h>

#define STRLONG 1024

char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

int main(int argc, char *argv[])
{
    long int i;
    char *args[] = {"./calculador", "./calculador", "1", "20"};
    pid_t pid;
    int estatWait;
    char *nomPrograma;
    char cadena[STRLONG];

    if (argc != 4)
    {
        printf("Us: %s <1r terme> <darrer terme> <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }
    if ((atoll(argv[2]) - atoll(argv[1]) + 1) % atoll(argv[3]) != 0)
    {
        printf("Rang de valors no divisible entre el nombre de processos calculadors.\n");
        exit(2);
    }

    nomPrograma = argv[0];
    long long terme_1 = atoll(argv[1]);
    long long terme_n = atoll(argv[2]);
    unsigned int numCalculadors = atoi(argv[3]);

    long long termesPerCalculador = (terme_n - terme_1 + 1) / numCalculadors;

    for (i = 0; i < numCalculadors; i++)
    {
        long double terme_1_Calculador = i * termesPerCalculador + terme_1;
        char terme_1_Calculador_Str[STRLONG];
        sprintf(terme_1_Calculador_Str, "%.0Lf", terme_1_Calculador);

        long double terme_n_Calculador = (i + 1) * termesPerCalculador + terme_1 - 1;
        char terme_n_Calculador_Str[STRLONG];
        sprintf(terme_n_Calculador_Str, "%.0Lf", terme_n_Calculador);

        switch (pid = fork())
        {
        case -1:
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(3);

        case 0: /* Fill */
            execl(args[0], args[1], terme_1_Calculador_Str, terme_n_Calculador_Str, NULL);

        default:
            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] Termes: %.0Lf -> %.0Lf %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, terme_1_Calculador, terme_n_Calculador, color_end);
            write(1, cadena, strlen(cadena));

            pid = wait(&estatWait);

            if (pid == -1)
            {
                sprintf(cadena, "%s%s - %d> No existeixen fills.%s\n", color_blue, nomPrograma, getpid(), color_end);
                write(1, cadena, strlen(cadena));
                exit(4);
            }

            sprintf(cadena, "%s%s - %d> Fill %d finalitzat%s.\n\n", color_blue, nomPrograma, getpid(), pid, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    return EXIT_SUCCESS;
}
