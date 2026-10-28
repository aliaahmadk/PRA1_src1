/* -----------------------------------------------------------------------
PRA1: Suma seqüència d’enters
Codi font: controlador_v2.c

Alia Ahmad Kousar.
Arantxa Ballester Ibarz.
---------------------------------------------------------------------- */

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

int main(int argc, char *argv[]) {

    long int i;
    pid_t pid;
    int estatWait;
    char *nomPrograma;
    char cadena[STRLONG];
    int pipeSumes[2];   // Pipe per connectar el controlador amb els calculadors

    if (argc != 4) {
        printf("Us: %s <1r terme> <darrer terme> <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }

    nomPrograma = argv[0];
    long long terme_1 = atoll(argv[1]);
    long long terme_n = atoll(argv[2]);
    unsigned int numCalculadors = atoi(argv[3]);

    if ((atoll(argv[2]) - atoll(argv[1]) + 1) % atoll(argv[3]) != 0) {
        printf("Rang de valors no divisible entre el nombre de processos calculadors.\n");
        exit(2);
    }

    long long termesPerCalculador = (terme_n - terme_1 + 1) / numCalculadors;

    if (pipe(pipeSumes) == -1) {
        perror("Error al crear la tuberia pipeSumes");
        exit(3);
    }

    // PRIMER BUCLE: Creació dels processos calculadors
    for (i = 0; i < numCalculadors; i++) {
        long double terme_1_Calculador = i * termesPerCalculador + terme_1;
        char terme_1_Calculador_Str[STRLONG];
        sprintf(terme_1_Calculador_Str, "%.0Lf", terme_1_Calculador);

        long double terme_n_Calculador = (i + 1) * termesPerCalculador + terme_1 - 1;
        char terme_n_Calculador_Str[STRLONG];
        sprintf(terme_n_Calculador_Str, "%.0Lf", terme_n_Calculador);

        pid = fork();
        if (pid == -1)
        { // Control de errores en fork
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s\n", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(4);

        }
        else if (pid == 0) {
            // Codi del Fill
            // Redireccionar extrem d'escriptura de pipeSumes al descriptor 20
            if (dup2(pipeSumes[1], 20) < 0) {
                perror("Error dup2 en el hijo");
                exit(5);
            }

            close(pipeSumes[0]);
            close(pipeSumes[1]);

            // Executar el nou calculador_v2
            execl("./calculador_v2", "./calculador_v2", terme_1_Calculador_Str, terme_n_Calculador_Str, NULL);
            perror("Error exec calculador_v2");
            exit(5);

        }
        else {
            // Codi del Pare
            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] Termes: %.0Lf -> %.0Lf %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, terme_1_Calculador, terme_n_Calculador, color_end);
            write(1, cadena, strlen(cadena));
        }
    }
