#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[])
{
  int npres, rank;
  double D; 
  
  MPI_Init(&argc, &argv);
  MPI_Status status;

  MPI_Comm_size(MPI_COMM_WORLD, &npres);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
               
  if (rank == 0) {
    srand(time(NULL) - rank*2);
    double A = rand()%100;
    double multExp;
    double divExp;

    printf("A = %f\n", A);
    MPI_Send(&A, 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
    MPI_Send(&A, 1, MPI_DOUBLE, 2, 1, MPI_COMM_WORLD);
    MPI_Recv(&multExp, 1, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD, &status);
    MPI_Recv(&divExp, 1, MPI_DOUBLE, 2, 3, MPI_COMM_WORLD, &status);
    
    D = multExp - divExp;
    printf("D = A*B - A/C = %f\n",D);
  }

  else if (rank == 1) {
    srand(time(NULL) - rank*2);
    double B = rand()%100;
    double A;
    double multExp;

    printf("B = %f\n", B);
    MPI_Recv(&A, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status); 
    multExp = A*B;
    MPI_Send(&multExp, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD); 
  }
  
  else if (rank == 2) {
    srand(time(NULL) - rank*2);
    double C = rand()%100;
    double A;
    double divExp;
    
    printf("C = %f\n",C);
    MPI_Recv(&A, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
    divExp = A/C;
    MPI_Send(&divExp, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
