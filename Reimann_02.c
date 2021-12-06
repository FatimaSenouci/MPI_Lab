#include "mpi.h" 
#include <stdio.h>
#include <corecrt_math.h>

int main(int argc, char** argv)

{
	// ************** Il faut definir le nombre n au prealable si on veut l'executer en utilisant CMD *****


	int  myid, numprocs, i,n=0;
	double mypi, pi, h, sum, x;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == 0) {
		printf("Entrer le nombre d intervalles: ");

		if (scanf_s("%d", &n) > 0) {
			MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		}
		else printf(" \n nbr d Intervalle invalide! \n");

	}

	h = 1.0 / (double)n;
	sum = 0.0;
	for (i = myid + 1; i <= n; i += numprocs) {
		x = h * ((double)i - 0.5);
		sum += 4.0 / (1.0 + x * x);

	}

	mypi = h * sum;
	


	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,
		MPI_COMM_WORLD);

	if (myid == 0)
		printf(" \n pi: %.16f: \n",pi);

	MPI_Finalize();
	return 0;
}


