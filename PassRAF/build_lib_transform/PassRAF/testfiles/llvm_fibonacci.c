#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

void Fibonacci(int *n_out, int n){


	if ( n == 0 ){
		*n_out = 0;
	}
	else if( n == 1){
		*n_out = 1;
	}
	else{

		int tmp_n_1 = n - 1;
		int tmp_n_2 = n - 2;
		int tmp_n_1_out = 42;
		int tmp_n_2_out = 42;
		int * __tmp_n_1_out = &tmp_n_1_out;
		int * __tmp_n_2_out = &tmp_n_2_out;

		Fibonacci(__tmp_n_1_out,tmp_n_1);
		spawn();
		Fibonacci(__tmp_n_2_out,tmp_n_2);
		spawn();
		tpc_sync();
		*n_out = (tmp_n_1_out + tmp_n_2_out);
	}

}

void wrapper_llvm_Fibonacci(struct _tpc_task_argument *t) 
{
  
   int * dlt;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  int dlt1;
  memcpy(&dlt1,&t[1].addr_in,sizeof(dlt1));
 
 
  Fibonacci(dlt,dlt1);
}


int main(int argc, char ** argv){

	int n, i = 0, c, threads;
	int n_out = 42;
	int * __tmp_n_out = &n_out;


	n = atoi(argv[1]);
	threads = atoi(argv[2]);

tpc_init(threads);
	;
	START_TIME();

	Fibonacci(__tmp_n_out,n);
	spawn();

	STOP_TIME();
tpc_shutdown();
	PRINT_TIME();
	printf("Fibonacci series: %d", n_out);

	return 0;
}

