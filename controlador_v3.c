/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: controlador_v3.c
 
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
#define FD_PIPE_RANGS_WRITE 10
#define FD_PIPE_RANGS_READ 11
#define FD_PIPE_SUMES_WRITE 20
#define FD_PIPE_SUMES_READ 21

typedef struct 
{   long long terme_1;
    long long terme_n;
} t_rang;


char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

int main(int argc, char *argv[]) 
{
  
    long int i;
    pid_t pid;
    int estatWait;
    char *nomPrograma;
    char cadena[STRLONG];
    char resposta[STRLONG];
    int pipeSumes[1];
    int pipeRangs[1];
    t_rang rang;

    if (argc != 2)
    {
        printf("Us: %s <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }
    
    nomPrograma = argv[0];

    long long terme_1, terme_n, total_termes;
    unsigned int numCalculadors = atoi(argv[2]);

    if (numCalculadors <= 0)
    {
        printf("El nombre de calculadors ha de ser major que 0.\n");
        exit(1);
    }

    long long termesPerCalculador = (terme_n - terme_1 + 1) / numCalculadors;

    if (pipe(pipeRangs) < 0 || pipe(pipeSumes) < 0) 
    {
        perror("Error al crear las tuberias");
        exit(2);
    }

    // PRIMER BUCLE: Creació dels processos calculadors
    for (i = 0; i < numCalculadors; i++) 
    {
        long double terme_1_Calculador = i * termesPerCalculador + terme_1;
        char terme_1_Calculador_Str[STRLONG];
        sprintf(terme_1_Calculador_Str, "%.0Lf", terme_1_Calculador);

        long double terme_n_Calculador = (i + 1) * termesPerCalculador + terme_1 - 1;
        char terme_n_Calculador_Str[STRLONG];
        sprintf(terme_n_Calculador_Str, "%.0Lf", terme_n_Calculador);

        pid = fork();  
        if (pid == -1) { // Control de errores en fork
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s\n", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(4);

        } 
        else if (pid == 0) 
        { 
            // Codi del Fill
            // Redireccionar extrem d'escriptura de pipeSumes al descriptor 20
            if (dup2(pipeSumes[1], 20) < 0) 
            {
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
        else 
        {
            // Codi del Pare
            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] Termes: %.0Lf -> %.0Lf %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, terme_1_Calculador, terme_n_Calculador, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    if (dup2(pipeSumes[0], 21) < 0) 
    { 
        perror("Error dup2 en el padre"); 
        exit(6); 
    }

    close(pipeSumes[0]);
    close(pipeSumes[1]);

    // Aquestes variables s'inicialitzen NOMÉS UNA VEGADA fora del bucle!
    long long sumTotal = 0;
    long long suma_parcial = 0;

    // SEGON BUCLE: Recepció de dades per la pipe i sincronització
    for (i = 0; i < numCalculadors; i++) 
    {
        if (read(21, &suma_parcial, sizeof(suma_parcial)) > 0) 
        {
            sumTotal += suma_parcial;
        }
        
        pid = wait(&estatWait);    
        if (pid == -1) 
        {
            sprintf(cadena, "%s%s - %d> No existeixen fills.%s\n", color_blue, nomPrograma, getpid(), color_end);
            write(1, cadena, strlen(cadena));
            exit(6);
        }

        // FIX: Una sola impressió correcta per cada fill que mor
        sprintf(cadena, "%s%s - %d> Fill %d finalitzat. Suma rebuda: %lld%s\n", color_blue, nomPrograma, getpid(), pid, suma_parcial, color_end);
        write(1, cadena, strlen(cadena));
    }

    // Tancar el descriptor 21 després de finalitzar la recepció 
    close(21);
    
    // COMPROVACIÓ FINAL
    long long n_terms = terme_n - terme_1 + 1; 
    long long sumFormula = (terme_1 + terme_n) * n_terms / 2;
    sprintf(cadena, "%s%s - %d&gt; Suma total acumulada: %lld | Comprobación fórmula: %lld%s\\n", color_blue, nomPrograma, getpid(), sumTotal, sumFormula, color_end); 
    write(1, cadena, strlen(cadena));
    
    return 0;
}
