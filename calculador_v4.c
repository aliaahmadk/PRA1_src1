/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: calculador_v4.c
 
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
#include <signal.h>

#define STRLONG 1024
#define FD_PIPE_SUMES_WRITE 20 
#define FD_PIPE_RANGS_READ 11  

char *color_green = "\033[01;32m";
char *color_end = "\033[00m";


typedef struct {
    long long terme_1;
    long long terme_n;
} t_rang;

// Gestor de senyal SIGTERM (per tancar el calculador)
void senyal_SIGTERM(int sig) {
    write(1, "Calculador tancat per SIGTERM\n", 30);
    exit(0); 
}

int main(int argc, char *argv[]) {
    long int i;
    long long sum = 0, sumForm, terme_i, terme_i_mes_1, terme_1, terme_n;
    char *nomPrograma = argv[0];
    char cadena[STRLONG];
   
    t_rang rang;

    signal(SIGINT, SIG_IGN);

    if (signal(SIGTERM, senyal_SIGTERM) == SIG_ERR) {       // Control d'errors en la configuració del gestor de senyals
        perror("Signal");
        exit(-1);
    }

    
    // Bucle principal: llegim els rangs de la pipe i calculem la suma per a cada rang
    while (read(FD_PIPE_RANGS_READ, &rang, sizeof(rang)) > 0) {     
        terme_1 = rang.terme_1; 
        terme_n = rang.terme_n; 

    

    sprintf(cadena, "%s%s - %d> Calcula progressio termes: %lld -> %lld %s\n", color_green, nomPrograma, getpid(), terme_1, terme_n, color_end);            
    write(1, cadena, strlen(cadena));

        terme_i = terme_1;
        terme_i_mes_1 = terme_i + 1;
        sum = terme_i;

        for (i = terme_1; i < terme_n; i++) {
            sum = sum + terme_i_mes_1;
            terme_i = terme_i_mes_1;
            terme_i_mes_1 = terme_i_mes_1 + 1;
        }

        sumForm = ((terme_1 + terme_n) / 2.0) * (terme_n + 1 - terme_1);

    sprintf(cadena, "%s%s - %d> Suma amb bucle=%lld - Suma amb formula=%lld%s\n", color_green, nomPrograma, getpid(), sum, sumForm, color_end);        
    write(1, cadena, strlen(cadena));

        if (write(FD_PIPE_SUMES_WRITE, &sum, sizeof(sum)) < 0) {  // Enviament de resultats per la canonada
            perror("Error en escriure el resultat a la pipeSumes"); 
            exit(1); 
        }
    }
    
    // Quan el read dona 0 (el controlador no envia més feina), sortim del bucle.
    close(FD_PIPE_RANGS_READ);  // Tanquem el tub de lectura (11)
    close(FD_PIPE_SUMES_WRITE); // Tanquem el tub d'escriptura (20)
    
    exit(EXIT_SUCCESS);         // Macro oficial d'èxit exigida al document


}