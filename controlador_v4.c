/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: controlador_v4.c
 
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
#include <signal.h>         // Per a gestionar senyals

#define STRLONG 1024
#define FD_PIPE_RANGS_WRITE 10
#define FD_PIPE_RANGS_READ 11
#define FD_PIPE_SUMES_WRITE 20
#define FD_PIPE_SUMES_READ 21

// Estructura per especificar els termes de la seqüència
typedef struct {   
    long long terme_1;
    long long terme_n;
} 

t_rang;

char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

pid_t pids_fills[100]; // <-- Array per guardar els PIDs dels fills
int numFillsGlobal = 0;

//Gestor de senyal SIGINT (Ctrl+C)
void senyal_SIGINT(int sig) {
    char msg[] = "\nRebut SIGINT (Ctrl+C). Tancant calculadors...\n";
    write(1, msg, strlen(msg));

    for (int i = 0; i < numFillsGlobal; i++) {
        kill(pids_fills[i], SIGTERM);
    }
}

// Funció per esperar que tots els fills finalitzin
void esperar_calculadors() {
    int estatWait;
    pid_t pid;
    char cadena[STRLONG];

    for (int i = 0; i < numFillsGlobal; i++) {
        pid = wait(&estatWait); // Espera que un fill finalitzi
        if (pid != -1) {
            sprintf(cadena, "%s%s - %d> Fill %d finalitzat.%s\n", color_blue, "./controlador_v4", getpid(), pid, color_end);
            write(1, cadena, strlen(cadena)); // Mostra missatge de finalització del fill
        }
    }
}

int main(int argc, char *argv[]) {
    long int i;
    pid_t pid;
    int estatWait;
    char *nomPrograma;
    char cadena[STRLONG];
    char resposta[STRLONG];

    int pipeRangs[2]; // Pipe per enviar els rangs (10/11)
    int pipeSumes[2]; // Pipe per rebre les sumes (20/21)

    long long terme_1, terme_n, totalTermes, termesPerCalculador;
    long long sumTotal, suma_parcial, sumFormula;
    unsigned int numCalculadors;
    t_rang rang;
    
   if (argc != 2) {
        printf("Us: %s <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }

    nomPrograma = argv[0];
    numCalculadors = atoi(argv[1]);
    signal(SIGINT, senyal_SIGINT);  // Configuració del gestor de senyal SIGINT (Ctrl+C)

    if (numCalculadors <= 0) {
        sprintf(cadena, "El nombre de calculadors ha de ser major que 0.\n");
        write(1, cadena, strlen(cadena));
        exit(1);
    }

    // Creació de les dues tuberies
    if (pipe(pipeRangs) == -1 || pipe(pipeSumes) == -1) {
        perror("Error al crear les tuberíes");
        exit(3);
    }

    // Creació dels processos calculadors
    for (i = 0; i < numCalculadors; i++) {
        pid = fork();
        if (pid == -1) {
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s\n", color_red, nomPrograma, getpid(), i + 1, color_end);
            write(1, cadena, strlen(cadena));
            exit(4);

        }
        else if (pid == 0) {
            // Codi del Fill v3
            // Redireccionar lectura de pipeRangs al descriptor 11
            if (dup2(pipeRangs[0], 11) < 0) {
                perror("Error dup2 pipeRangs en el hijo");
                exit(5);
            }
            // Redireccionar escriptura de pipeSumes al descriptor 20
            if (dup2(pipeSumes[1], 20) < 0) {
                perror("Error dup2 pipeSumes en el hijo");
                exit(5);
            }

           // Tancar tots els descriptores de les pipes originals al fill
            close(pipeRangs[0]);
            close(pipeRangs[1]);
            close(pipeSumes[0]);
            close(pipeSumes[1]);

            // Executar el calculador_v4
            execl("./calculador_v4", "./calculador_v4", (char *)NULL);
            perror("Error exec calculador_v4");
            exit(5);

        }
        else {
            // Codi del Pare
            pids_fills[i] = pid;
            numFillsGlobal++;

            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] .%s\n", color_blue, nomPrograma, getpid(), i + 1, pid, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    // Redireccionar descriptors al pare
    // Redireccionar escriptura de pipeRangs al descriptor 10
    if (dup2(pipeRangs[1], 10) < 0) {
        perror("Error dup2 pipeRangs en el padre");
        exit(6);
    }
    // Redireccionar lectura de pipeSumes al descriptor 21
    if (dup2(pipeSumes[0], 21) < 0) {
        perror("Error dup2 pipeSumes en el padre");
        exit(6);
    }

    // Tancar els descriptors originals al pare
    close(pipeRangs[0]);
    close(pipeRangs[1]);
    close(pipeSumes[0]);
    close(pipeSumes[1]);

    // Bucle principal (Processament de seqüències)
    do {
        // Demanar i validar la seqüència
        do {
            sprintf(cadena, "\n%s%s - %d> ----------- INTRODUCCIO TERMES RANG ------------%s\n", color_blue, nomPrograma, getpid(), color_end);
            write(1, cadena, strlen(cadena));

            sprintf(cadena, "%s%s - %d> Introduceix el 1r terme de la progressio: %s", color_blue, nomPrograma, getpid(), color_end);
            write(1, cadena, strlen(cadena));
            memset(cadena, 0, STRLONG);
            read(0, cadena, STRLONG);
            terme_1 = atoll(cadena);

            sprintf(cadena, "%s%s - %d> Introduceix el darrer terme de la progressio: %s", color_blue, nomPrograma, getpid(), color_end);
            write(1, cadena, strlen(cadena));
            memset(cadena, 0, STRLONG);
            read(0, cadena, STRLONG);
            terme_n = atoll(cadena);

            totalTermes = terme_n - terme_1 + 1;
           if (totalTermes <= 0 || totalTermes % numCalculadors != 0) {
                sprintf(cadena, "Rang de valors no divisible entre el nombre de processos calculadors.\n");
                write(1, cadena, strlen(cadena));
            }
        } while (totalTermes <= 0 || totalTermes % numCalculadors != 0);

        termesPerCalculador = totalTermes / numCalculadors;

        // Enviar els rangs als calculadors utilitzant l'estructura t_rang pel descriptor 10
        for (i = 0; i < numCalculadors; i++) {
            rang.terme_1 = terme_1 + (i * termesPerCalculador);
            rang.terme_n = rang.terme_1 + termesPerCalculador - 1;

            write(10, &rang, sizeof(rang));
        }

        sumTotal = 0;
        // Recollir les sumes parcials del descriptor 21
        for (i = 0; i < numCalculadors; i++) {
            if (read(21, &suma_parcial, sizeof(suma_parcial)) > 0) {
                sumTotal += suma_parcial;
            }
        }

        sumFormula = (terme_1 + terme_n) * totalTermes / 2;
        sprintf(cadena, "%s%s - %d> Suma total acumulada: %lld | Comprobación fórmula: %lld%s\n", color_blue, nomPrograma, getpid(), sumTotal, sumFormula, color_end);
        write(1, cadena, strlen(cadena));

        // Demanar si se vol introduir una nova seqüència
        sprintf(cadena, "\n%s%s - %d> Vols introduir una nova sequencia? (s/n): %s", color_blue, nomPrograma, getpid(), color_end);
        write(1, cadena, strlen(cadena));
        memset(resposta, 0, STRLONG);
        read(0, resposta, STRLONG);

    } while (resposta[0] == 's' || resposta[0] == 'S');
    
    close(10);           // Tanquem el descriptor d'escriptura de la pipe de rangs (10) per indicar als fills que no hi ha més feina.
    esperar_calculadors();  // Esperem que tots els calculadors finalitzin abans de tancar el descriptor de lectura de la pipe de sumes (21).
    close(21);           // Tanquem el descriptor de lectura de la pipe de sumes (21) després que tots els calculadors hagin finalitzat.

    return 0;
}
