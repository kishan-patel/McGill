#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

void initialize(double** A);
void print(double** A);
void solve(double** A);

double** A;
int aSize, noProc, i, j, k;
struct timeval start, end;

int main(int argc, char *argv[])
{
  if(argc < 3){
    printf("You need to specify the matrix size and no of processes.\n");
  } 
  
  aSize = atoi(argv[1]); 
  noProc = atoi(argv[2]);

  A = malloc(aSize * sizeof *A);
  for (i=0; i<aSize; i++)
  {
    A[i] = malloc(aSize * sizeof *A[i]);
  }
  
  initialize(A);
  gettimeofday(&start, NULL);  
  solve(A);
  gettimeofday(&end, NULL);
  printf("Time of solve is: \n%lu s\n", (unsigned long)(end.tv_sec - start.tv_sec));
}

void initialize(double** A)
{
  srand(time(NULL));
  for(i=0; i<aSize; i++)
  {
    for(j=0; j<aSize; j++)
    {
      A[i][j] = rand();
    }
  }
}

void print(double** A)
{
  for(i=0; i<aSize; i++)
  {
    for(j=0; j<aSize; j++)
    {
      printf("%.0f ", A[i][j]);
    }
    
    printf("\n");
  }
}

void solve(double** A)
{
  for(k=0; k<aSize; k++)
  {
    
    #pragma omp parallel for private(i,j) num_threads(noProc)
    for(j=k+1; j<aSize; j++)
    {
      A[k][j] = A[k][j] / A[k][k];
    }
  
      A[k][k] = 1;
   
    #pragma omp parallel for private(i,j) num_threads(noProc)
    for(i=k+1; i<aSize; i++)
    {
      for(j=k+1; j<aSize; j++)
      {
        A[i][j] = A[i][j] - A[i][k] * A[k][j];
      }
      A[i][k] = 0;
    }
  }
}
