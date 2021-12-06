#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <time.h>
#include <sys/timeb.h>
#include <chrono> 

/*
  Ici, le `` processus o '' qui distribue les taches de travail à d'autres processus est considéré
  comme processus racine (master ) et les processus qui effectuent le calcul est considéré
  en tant que processus de tâches esclaves.


*/

// Nombre de lignes et de colonnes dans une matrice
#define N 10

MPI_Status status;

// Les squelette de matrice sont créés
double matrix_a[N][N], matrix_b[N][N], matrix_c[N][N];

int main(int argc, char** argv)
{


    int processCount, processId, slaveTaskCount, source, dest, rows, offset;
    // l'envirenement MPI est inisialiser 
    MPI_Init(&argc, &argv);
    // Chaque processus obtient un identifiant unique (rang)
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    // Le nombre de processus dans le communicateur sera attribué à la variable -> processCount
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    double t1, t2;


    if (processId == 0) {

        t1 = MPI_Wtime();
    }
    // Le nombre de tâches esclaves sera affecté à la variable -> slaveTaskCount
    slaveTaskCount = processCount - 1;


    // Root (Master) process





    if (processId == 0) {

        // La matrice A et la matrice B seront toutes deux remplies de nombres aléatoires
       // srand(time(NULL));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix_a[i][j] = rand() % 10;
                matrix_b[i][j] = rand() % 10;
            }
        }

        printf("\n\t\tMatrix - Matrix Multiplication using MPI\n");

        // Print Matrix A
        printf("\nMatrix A\n\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%.0f\t", matrix_a[i][j]);
            }
            printf("\n");
        }

        // Print Matrix B
        printf("\nMatrix B\n\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%.0f\t", matrix_b[i][j]);
            }
            printf("\n");
        }

        // Déterminez le nombre de lignes de la matrice A, qui est envoyé à chaque processus esclave


        if (slaveTaskCount != 0)
        {
            rows = N / slaveTaskCount;
        }
        // La variable offset détermine le point de départ de la ligne envoyée au processus esclave
        offset = 0;

        // Les détails du calcul sont affectés aux tâches esclaves. a partir de processus 1; 
        // le tag de chaque message est 
        for (dest = 1; dest <= slaveTaskCount; dest++)
        {
            // Reconnaître offset de la matrice A
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // Reconnaître le nombre de lignes
            MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // Envoyer des lignes de la matrice A qui seront affectées au processus esclave pour les calculer
            MPI_Send(&matrix_a[offset][0], rows * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            // La matrice B est envoyée
            MPI_Send(&matrix_b, N * N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);

            // offset est modifié en fonction du nombre de lignes envoyées à chaque processus
            offset = offset + rows;
        }

        // Le processus master attend que chaque processus esclave envoie son résultat calculé avec le tag de message 2
        for (int i = 1; i <= slaveTaskCount; i++)
        {
            source = i;
            // Recevoir l'offset d'un processus esclave particulier
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            // Recevoir le nombre de lignes traitées par chaque processus esclave
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            // Les lignes calculées de chaque processus seront stockées dans la matrice C en fonction de leur offset et
            //le nombre de lignes traitées
            MPI_Recv(&matrix_c[offset][0], rows * N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
        }

        // Print la matrice des résultats
        printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                printf("%.0f\t", matrix_c[i][j]);
            printf("\n");
        }
        printf("\n");
    }

    // Processus esclaves

    if (processId > 0) {

        // L'ID de processus source est défini
        source = 0;

        // Le processus esclave attend les tampons de messages avec le tag 1, que le processus master a envoyé
        // Chaque processus recevra et exécutera ceci séparément de leurs processus

        // Le processus esclave reçoit la valeur de l'offset  envoyée par le processus master 
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // Le processus esclave reçoit le nombre de lignes envoyées par le processus master
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // Le processus esclave reçoit la sous-partie de la matrice A qui est assignée par Root
        MPI_Recv(&matrix_a, rows * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        // Le processus esclave reçoit la matrice B
        MPI_Recv(&matrix_b, N * N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        // Multiplication matricielle

        for (int k = 0; k < N; k++) {
            for (int i = 0; i < rows; i++) {
                // Définir la valeur initiale de la somme des lignes
                matrix_c[i][k] = 0.0;
                // L'élément de la matrice A (i, j) sera multiplié par l'élément de la matrice B (j, k)
                for (int j = 0; j < N; j++)
                    matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
            }
        }

        // Le résultat calculé sera renvoyé au processus racine (processus 0) avec le tag  de message 2

        // offset sera envoyé à la racine, qui détermine le point de départ du calcul
        // valeur dans la matrice C
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // Nombre de lignes que le processus a calculé sera envoyé au processus racine
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        // La matrice résultante avec les lignes calculées sera envoyée au processus master
        MPI_Send(&matrix_c, rows * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }


    if (processId == 0) {
        t2 = MPI_Wtime();
     printf("Temps d\'execution %f\n", t2 - t1);

    }



    MPI_Finalize();
}
