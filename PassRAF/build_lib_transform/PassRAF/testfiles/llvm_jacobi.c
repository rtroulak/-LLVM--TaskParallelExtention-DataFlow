#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"

static int DIM;
static int BDIM;
static int REALDIM;

// See charm++ jacobi reference implementation
// static void jacobi_block(double inMatrix[REALDIM][REALDIM], double outMatrix[REALDIM][REALDIM],
static void jacobi_block(double *inMatrix, double *outMatrix,
		double *west_ghostblock, double *east_ghostblock,
		double *north_ghostline, double *south_ghostline)
{
	int N=DIM;
	int BN=BDIM;

	// Update all values in the block ("inner-loop")
	for (int x = 0; x < BN; x++) {
		///	__builtin_prefetch((void*)&outMatrix[x][0], 1 , 2);
		//	__builtin_prefetch((void*)&outMatrix[x][8], 1 , 2);
		//	__builtin_prefetch((void*)&outMatrix[x][16], 1 , 2);
		for (int y = 0; y < BN; y++) {

			// Start with "self" value
			double sum;
			sum = inMatrix[x*REALDIM+y] + inMatrix[(x-1)*REALDIM+y] + inMatrix[(x+1)*REALDIM+y] +inMatrix[x*REALDIM+y-1] + inMatrix[x*REALDIM+y+1] ;
			// Calculate new value, difference, and update maxDiff
			outMatrix[x*REALDIM+y] = sum * (double)0.2;
		}
	}
}

void wrapper_llvm_jacobi_block(struct _tpc_task_argument *t) 
{
  
  double * dlt;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  double * dlt1;
  memcpy(&dlt1,&t[1].addr_in,sizeof(dlt1));
  double * dlt2;
  memcpy(&dlt2,&t[2].addr_in,sizeof(dlt2));
  double * dlt3;
  memcpy(&dlt3,&t[3].addr_in,sizeof(dlt3));
  double * dlt4;
  memcpy(&dlt4,&t[4].addr_in,sizeof(dlt4));
  double * dlt5;
  memcpy(&dlt5,&t[5].addr_in,sizeof(dlt5));
 
  jacobi_block(dlt,dlt1,dlt2,dlt3,dlt4,dlt5);
}

void init_block(double *matrix)
{
	unsigned int i, j;
	unsigned int block_dim = BDIM;
	for(i=0; i<block_dim; i++) {
		for(j=0; j<block_dim; j++) {
			matrix[i*REALDIM+j] = (i*j) % 1345;
		}
	}
}

void wrapper_llvm_init_block(struct _tpc_task_argument *t) 
{
  
   double *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
 
  init_block(dlt);
}

// void parallel_jacobi(unsigned int jacobi_iterations, unsigned int N, unsigned int BN, double inMatrix[N][N], double outMatrix[N][N])
void parallel_jacobi(unsigned int jacobi_iterations, unsigned int N, unsigned int BN, double *inMatrix, double *outMatrix)
{
	unsigned int row_length = N*sizeof(double);
	double *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	for(int iters=0; iters<jacobi_iterations; iters++) {
		for (int x = BN; x < N-BN; x+=BN) {
			for (int y = BN; y < N-BN; y+=BN) {
				//jacobi_block(&inMatrix[x][y], &outMatrix[x][y], &inMatrix[x][y-BN], &inMatrix[x][y+BN], &inMatrix[x-1][y], &inMatrix[x+BN][y]);
				arg1=&inMatrix[x*N+y];
				arg2=&outMatrix[x*N+y];
				arg3=&inMatrix[x*N+y-BN];
				arg4=&inMatrix[x*N+y+BN];
				arg5=&inMatrix[(x-1)*N+y];
				arg6=&inMatrix[(x+BN)*N+y];

				jacobi_block(arg1, arg2, arg3, arg4, arg5, arg6);
				spawn();
			}
		}
		// Swap in and out
		double *tmp = inMatrix;
		inMatrix = outMatrix;
		outMatrix = tmp;
	}
	/*#pragma css sync*/
	;
}

void wrapper_llvm_parallel_jacobi(struct _tpc_task_argument *t) 
{
  
   int dlt;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  int dlt1;
  memcpy(&dlt1,&t[1].addr_in,sizeof(dlt1));
  int dlt2;
  memcpy(&dlt2,&t[2].addr_in,sizeof(dlt2));
  double * dlt3;
  memcpy(&dlt3,&t[3].addr_in,sizeof(dlt3));
  double * dlt4;
  memcpy(&dlt4,&t[4].addr_in,sizeof(dlt4));
 
  parallel_jacobi(dlt,dlt1,dlt2,dlt3,dlt4);
}

void init_matrix(unsigned int matrix_dim, unsigned int block_dim, double *matrix)
{
	int i,j;
	unsigned int row_length = matrix_dim*sizeof(double);
	double* matr;
	for(i=0; i<matrix_dim; i+=block_dim) {
		for(j=0; j<matrix_dim; j+=block_dim) {
			matr = &matrix[i*matrix_dim+j];

			init_block(matr);
			spawn();
		}
	}
	/*#pragma css sync*/
	;
}

void print_matrix(int matrix_dim, double *matrix)
{
	int i,j;
	for(i=0; i<matrix_dim; i++) {
		for(j=0; j<matrix_dim; j++) {
			printf("%lf ", matrix[i*matrix_dim+j]);
		}
	}

}

unsigned long ceil_to_pow2(unsigned long x)
{
	unsigned long leeding_zeros = __builtin_clzl(x);
	unsigned long next_pow2 = 1L <<(64-leeding_zeros) ;
	/*printf("clzl: %lu
", leeding_zeros );*/
	/*printf("x-> %lx | R -> %lx
", x, next_pow2 );*/
	return next_pow2;
}

int main(int argc, char *argv[])
{
	int nthreads;
	unsigned int jacobi_iterations;

	
		DIM = atoi(argv[1]);
		BDIM = atoi(argv[2]);
		nthreads = atoi(argv[3]);
		jacobi_iterations = atoi(argv[4]);
	

	printf( "	|Jacobi: %4ux%4u matrix"
			"        %4ux%4u blocks"
			"        %4d threads"
			"        %4u jacobi_iterations	|", DIM,DIM, BDIM,BDIM, nthreads, jacobi_iterations);


	int matrix_dim = DIM + (2*BDIM);

	REALDIM = matrix_dim;

	unsigned long grid_size = ceil_to_pow2(matrix_dim*matrix_dim*sizeof(double)*3);

tpc_init(nthreads);
	;
	double *inMatrix,*outMatrix;

	inMatrix = (double*)malloc(matrix_dim*matrix_dim*sizeof(double));

	outMatrix = (double*) malloc(matrix_dim*matrix_dim*sizeof(double));
	init_matrix(matrix_dim, BDIM, inMatrix);

	/*print_matrix(matrix_dim, inMatrix);*/
	printf("Init done");
	START_TIME();

	parallel_jacobi(jacobi_iterations, matrix_dim, BDIM, inMatrix, outMatrix);
	spawn();
	STOP_TIME();
	tpc_shutdown();
	PRINT_TIME();

	/*print_matrix(matrix_dim, inMatrix);*/

	return 0;
}

