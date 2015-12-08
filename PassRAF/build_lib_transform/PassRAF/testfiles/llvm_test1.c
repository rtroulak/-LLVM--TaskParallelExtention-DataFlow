#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

void task_init(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = i;
	}
}

void task_print(int * array, int size){
	int i;
	assert( array && size > 0);
	printf("array = [");
	for( int i = 0; i < size; i++ ){
		printf("%c%d ",(i==0)?' ':',',array[i]);
	}
	printf("]");
}


void wrapper_llvm_task_print(struct _tpc_task_argument *t) 
{
  
  int dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
  task_print(dlt,slt);
}

void wrapper_llvm_task_init(struct _tpc_task_argument *t) 
{
  
  int dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
  task_init(dlt,slt);
}

int main(int argc, char **argv)
{
  int *array;
  int array_size;
  int nThreads;
  int i;

  
  array_size = atoi(argv[1]);
  nThreads = atoi(argv[2]);
	assert(array_size > 0);
	
	tpc_init(nThreads);
  // alloc spaces for the option data

 	array = (int *) malloc(array_size * sizeof(int));
	tpc_sync();
	task_init(array, array_size);
	spawn();
	task_print(array, array_size);
	spawn();
	tpc_shutdown();
  return 0;
}

