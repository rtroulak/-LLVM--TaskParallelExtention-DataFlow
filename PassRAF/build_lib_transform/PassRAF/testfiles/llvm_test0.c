#include <stdio.h>
#include <string.h>
// #include "funcs.c"
 // #include "../../../../llvm/include/llvm/Partee/src/runtime.c"
// #include "../../../../llvm/include/llvm/Partee/include/runtime.h"



/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/



void test(float * d)
{

  printf("<<  arguments : [%f]  >>" ,d);

}



void wrapper_llvm_test(struct _tpc_task_argument *t) 
{
  
  float * dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  test(dlt);
}




int main(int argc, char *argv[])
{
 
  int array_size;
  int nThreads;
  array_size = atoi(argv[1]);
  nThreads = atoi(argv[2]);
  
  int num = 878;
  double doub = 9.8;
  float floa = 8.9;
  char ch = 'K';
  float *nump = &floa;

  tpc_init(nThreads);
  test(num+8);
  spawn();
  
  
  
  tpc_shutdown();



  return 0;
}

