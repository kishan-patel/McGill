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
  if(argc < 2){
    printf("Usage: ./output_file size_of_matrix\n");
    return 0;
  } 
  
  aSize = atoi(argv[1]); 

  A = malloc(aSize * sizeof *A);
  for (i=0; i<aSize; i++)
  {
    A[i] = malloc(aSize * sizeof *A[i]);
  }
  
  initialize(A);
  gettimeofday(&start, NULL);  
  solve(A);
  gettimeofday(&end, NULL);
  printf("Time of solve is: \n%lu us\n", (unsigned long)(end.tv_usec - start.tv_usec));
}

void initialize(double** A)
{
  srand(time(NULL));
  for(i=0; i<aSize; i++)
  {
    for(j=0; j<aSize; j++)
    {
      A[i][j] = (double)rand();
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
    
    for(j=k+1; j<aSize; j++)
    {
      A[k][j] = A[k][j] / A[k][k];
    }
  
    A[k][k] = 1;
   
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
