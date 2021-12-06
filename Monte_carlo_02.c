#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char* argv[]) {
	int N, lhit;
	int i, id, np;
	double x, y;

		MPI_Init(&argc, &argv);
	// la creation de communicateur 
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	N = 1000;   // le nombre totales des point 	
    lhit = 0;     //  le nombre de points qui tombent dans le cercle.
	srand((unsigned)(time(0)));  //generé les nombre aleatoires 
	int lN = N / np;

	for (i = 0; i < lN; i++) {
		// generer le nombre x et y entre le 0 et le 1 
		x = ((double)rand()) / ((double)RAND_MAX);
		y = ((double)rand()) / ((double)RAND_MAX);
		// incrémenter lhit si le point est à l'intérieur du cercle
		if (((x * x) + (y * y)) <= 1)
			lhit++;
	}

	int hit = 0;
	MPI_Allreduce(&lhit, &hit, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	double est;
	est = (hit * 4) / ((double)N);

	if (id == 0) {
		printf("Nombre de points utilises:      %d\n", N);
		printf("Pi:         %24.16f\n", est);

	}

	MPI_Finalize();
	return 0;}

