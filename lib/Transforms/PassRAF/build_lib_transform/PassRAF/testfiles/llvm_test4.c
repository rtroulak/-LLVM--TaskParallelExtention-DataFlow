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

void task_split_init(int *array, int size, int block_size, int threads){
  int i, *block;
	assert( array && size > 0 && threads > 0);
	for(i = 0; i< threads; i++){
		block = array + i*block_size;

		task_init(block, block_size);
		spawn();
	}
/*#pragma css	sync*/
	;
}

void task_set(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = i*size;
	}
}

void task_split_set(int *array, int size, int block_size, int threads){
  int i, *block;
	assert( array && size > 0 && threads > 0);
	for(i = 0; i< threads; i++){
		block = array + i*block_size;

		task_set(block, block_size);
		spawn();
	}
/*#pragma css	sync*/
	;
}

void task_print(int * array, int size){
	int i;
	assert( array && size > 0);
	printf("array = [");
	for( int i = 0; i < size; i++ ){
		printf("%c%d ",(i==0)?' ':',',array[i]);
	}
	printf("] ");
}

void task_unset(int * array, int size){
	int i;
	assert( array && size > 0);
	for( int i = 0; i < size; i++ ){
		array[i] = -1;
	}
}

void task_split_unset(int *array, int size, int block_size, int threads){
  int i, *block;
	assert( array && size > 0 && threads > 0);
	for(i = 0; i< threads; i++){
		block = array + i*block_size;
		task_unset(block, block_size);
		spawn();
	}
/*#pragma css	sync*/
	;
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
void wrapper_llvm_task_split_unset(struct _tpc_task_argument *t) 
{
  
   int *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
   int slt1 ;
  memcpy(&slt1,&t[1].addr_in,sizeof(slt1));
   int slt2 ;
  memcpy(&slt2,&t[1].addr_in,sizeof(slt2));
  task_split_unset(dlt,slt,slt1,slt2);
}

void wrapper_llvm_task_split_set(struct _tpc_task_argument *t) 
{
  
  int *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
   int slt1 ;
  memcpy(&slt1,&t[1].addr_in,sizeof(slt1));
   int slt2 ;
  memcpy(&slt2,&t[1].addr_in,sizeof(slt2));
  task_split_set(dlt,slt,slt1,slt2);
}
void wrapper_llvm_task_split_init(struct _tpc_task_argument *t) 
{
  
   int *dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
   int slt ;
  memcpy(&slt,&t[1].addr_in,sizeof(slt));
   int slt1 ;
  memcpy(&slt1,&t[1].addr_in,sizeof(slt1));
   int slt2 ;
  memcpy(&slt2,&t[1].addr_in,sizeof(slt2));
  task_split_init(dlt,slt,slt1,slt2);
}

int main(int argc, char **argv)
{
  int *array;
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

	task_split_init(array, array_size, block_size, nThreads);
	spawn();
	task_split_set(array, array_size, block_size, nThreads);
	spawn();
	task_print(array, array_size);
	spawn();
	task_split_unset(array, array_size, block_size, nThreads);
	spawn();
	task_print(array, array_size);
	spawn();
	tpc_shutdown();

	
  return 0;
}

