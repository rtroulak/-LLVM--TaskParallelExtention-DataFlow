#include <stdio.h>
#include <string.h>
// #include "funcs.c"
 // #include "../../../../llvm/include/llvm/Partee/src/runtime.c"
// #include "../../../../llvm/include/llvm/Partee/include/runtime.h"



/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/



void functiontest(int x,char c,float f,double d)
{

  printf("<<  arguments : [%d]-[%c]-[%f]-[%f]  >>" , x,c,f,d);

}
void test(double d)
{

  printf("<<  arguments : [%f]  >>" ,d);

}
void test1(int * d)
{

  printf("<<  arguments : [%d]  >>" ,d);

}


void wrapper_llvm_functiontest(struct _tpc_task_argument *t) 
{
  char flt ;
  memcpy(&flt,&t[1].addr_in,sizeof(flt));
  int ilt ;
  memcpy(&ilt,&t[0].addr_in,sizeof(ilt));
  float thlt ;
  memcpy(&thlt,&t[2].addr_in,sizeof(thlt));
  double dlt ;
  memcpy(&dlt,&t[3].addr_in,sizeof(dlt));
  functiontest(ilt,flt,thlt,dlt);
}
void wrapper_llvm_test(struct _tpc_task_argument *t) 
{
  
  double dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  test(dlt);
}
void wrapper_llvm_test1(struct _tpc_task_argument *t) 
{
  
  int * dlt ;
  memcpy(&dlt,&t[0].addr_in,sizeof(dlt));
  test1(dlt);
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
  int *nump = &num;

  tpc_init(nThreads);
  functiontest(num,ch,floa,doub);
  spawn();
  test(num+8);
  spawn();
  test1(nump);
  spawn();
  
  tpc_shutdown();



  return 0;
}

