#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

void task_init(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = 0;
	}
}

void task_set(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = i*size;
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

void task_unset(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = -1;
	}
}


void wrapper_llvm_task_unset(struct _tpc_task_argument *t) 
{
  
  int dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
  task_unset(dlt,slt);
}
void wrapper_llvm_task_print(struct _tpc_task_argument *t) 
{
  
  int dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
  task_print(dlt,slt);
}
void wrapper_llvm_task_set(struct _tpc_task_argument *t) 
{
  
  int dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
  task_set(dlt,slt);
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
  int *array, *block;
  int array_size, block_size;
  int nThreads;
  int i;


  array_size = atoi(argv[1]);
  nThreads = atoi(argv[2]);
	block_size = array_size/nThreads;
	assert(array_size > 0);
	assert(block_size > 0);
	tpc_init(nThreads);


   array = (int *) malloc(array_size * sizeof(int));
	tpc_sync();
	for(i = 0; i< nThreads; i++){
		block = array + i*block_size;

		task_init(block, block_size);
		spawn();
	}
	for(i = 0; i< nThreads; i++){
		block = array + i*block_size;

		task_set(block, block_size);
		spawn();
	}

	task_print(array, array_size);
	spawn();

/*#pragma css sync*/

	for(i = 0; i< nThreads; i++){
		block = array + i*block_size;

		task_unset(block, block_size);
		spawn();
	}


	task_print(array, array_size);
	spawn();
	tpc_shutdown();

		
  return 0;
}

