/**
* @file PassRAF.cpp
* @author Troulakis Georgios Rafail <rtroulak@ics.forth.gr>,<rtroulak@gmail.com>
* @version v1.0
* @date 2015-21-10
*/


#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include <iostream>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <string>
#include <memory>
#include <cxxabi.h>
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm//ExecutionEngine/ExecutionEngine.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm//IR/DataLayout.h"
#include "llvm/Partee/include/partee.h"


#include "llvm/IR/InlineAsm.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/Support/Casting.h"


#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace llvm;

#define DEBUG_TYPE "mypass"


int flag=0;

 
  class ExprAST {
public:
  virtual ~ExprAST() {}
  virtual Value *Codegen() = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;
public:
  NumberExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

Value *ErrorV(const char *Str) 
{
  fprintf(stderr, "Error: %s\n", Str); return 0; 
}


Value*  *FPrintF, *messagePtr;
llvm::LLVMContext* context;


namespace {
  // MyPass - The first implementation, without getAnalysisUsage.
  struct MyPass : public FunctionPass 
  {
  
  static char ID; // Pass identification, replacement for typeid
  MyPass() : FunctionPass(ID) {}
    

  		int ins_counter=0;
    	//GlobalStruct
  		struct structData
		{
    		Instruction *inst;
    		struct structData* next;
		}structData_t;

		struct structData* headg;
		struct structData* tail = NULL;


		void insert_s(struct structData* head,Instruction * inst)
		{

			ins_counter++;
			struct structData* newnode = (struct structData *)malloc(sizeof(struct structData_t*));
			if(head == NULL)
			{
				newnode->inst = inst;
				newnode->next = tail;
				headg=newnode;
			}	
			else
			{
				newnode->inst = inst;
				newnode->next = head;
				headg=newnode;
			}

		}

		void delete_func_calls()
		{
			struct structData *current;
  			current = headg;
  			
  			if(current == NULL)
  				return ;
  
  			int i = 0;
  			while(i < ins_counter)
  			{
    
    			errs() << "Instruction = " << *(current->inst) << " [deleted!]\n";
    			current->inst->eraseFromParent();

    			current = current->next;
    			
    			i++;
  			}

		}

		struct structData * tmp_struct;
  		//GlobalVariables
  		Instruction* I2;
  		Instruction* I1;
  		std::vector<Instruction *> InstForDelete;
  		std::string name,prevname;
  		std::vector<Value*> Params;
  		std::vector<Type*> ParamsType;
  		std::string nameofvalue;
  		StructType* MYFILE;
  		StructType* MYFILE1;
  		Type* FILEPTR;
  		Type* FILEPTR1;
  		Value *GVstderr;
  		int numofargs,counter=0;
  		bool IsInMain=false,deldefret=true,onetime=true;
  		BasicBlock* label_entry_basic ;
  		Function* func_tpc_task_descriptor_alloc;
  		Function* func_malloc;
  		Function* func_tpc_call;
  		CallInst* ptr_mall_call;
  		CallInst* prevcallInst;
  		StructType *StructTy;
		StructType *StructTy_struct__tpc_task_argument;
		Instruction* nextInstruction;
		CallInst* ptr_call_public ;
		LoadInst* ptr_par;
		PointerType* PointerTy_4;


  
   
	bool runOnMachineFunction(Function &F)
  	{
    	return false;
  	}
    
    
  	//return next inst
  	Instruction* getNextInstruction(Instruction& i) 
  	{
    	BasicBlock::iterator it(&i);
   		return it;
  	}
   


  	//erase Instruction
  	void EraseInst(Instruction &I) 
  	{
    	assert(I.use_empty() && "Cannot erase used instructions!");
		I.eraseFromParent();
	      
  	}
  	//create wrapper
  	Function* create_wrapper_func(Module *M,std::string name_func,PointerType* PointerTy_10)
  	{
  		
  		std::vector<Type*>FuncTy_9_args;
  		FuncTy_9_args.push_back(PointerTy_10);
 		FunctionType* FuncTy_9 = FunctionType::get(
 		 /*Result=*/Type::getVoidTy(M->getContext()),
 		 /*Params=*/FuncTy_9_args,
 		 /*isVarArg=*/false);
 		std::string wrapper = "wrapper_llvm_";
 		std::string called_wrapper = wrapper + name_func;
 		

  		Function* func_wrapper_llvm_functiontest = 	M->getFunction(called_wrapper);
 		if (!func_wrapper_llvm_functiontest) 
 		{
 			func_wrapper_llvm_functiontest = Function::Create(
 		 	/*Type=*/FuncTy_9,
 		 	/*Linkage=*/GlobalValue::ExternalLinkage,
 			 /*Name=*/called_wrapper, M); 
 			func_wrapper_llvm_functiontest->setCallingConv(CallingConv::C);
 		}
 		AttributeSet func_wrapper_llvm_functiontest_PAL;
 		{
 		 	SmallVector<AttributeSet, 4> Attrs;
 		 	AttributeSet PAS;
 		  	{
 		   		AttrBuilder B;
 		   		B.addAttribute(Attribute::NoUnwind);
 		   		PAS = AttributeSet::get(M->getContext(), ~0U, B);
 		  	}
 		 
 			Attrs.push_back(PAS);
 			func_wrapper_llvm_functiontest_PAL = AttributeSet::get(M->getContext(), Attrs);
 		 
 		}
 		func_wrapper_llvm_functiontest->setAttributes(func_wrapper_llvm_functiontest_PAL);

 	   	return func_wrapper_llvm_functiontest;
  	}

  	//store wrapper
  	void wrapper_llvm(Module *M,PointerType* PointerTy_10,PointerType* PointerTy_7,CallInst* ptr_mall_call,AllocaInst* ptr_y,ConstantInt* const_int32_26,std::string name_func,Instruction *label_entry)
  	{
		
		//delcare wrapper_llvm_functiontest
 		Function* func_wrapper_llvm_functiontest = create_wrapper_func(M,name_func,PointerTy_10);
  		LoadInst* ptr_40 = new LoadInst(ptr_y, "", false, label_entry);
 		ptr_40->setAlignment(8);
 		std::vector<Value*> ptr_task_indices;
 		ptr_task_indices.push_back(const_int32_26);
 		ptr_task_indices.push_back(const_int32_26);
 		Instruction* ptr_task = GetElementPtrInst::Create(ptr_40, ptr_task_indices, "task", label_entry);
 		StoreInst* void_41 = new StoreInst(func_wrapper_llvm_functiontest, ptr_task, false, label_entry);
 		void_41->setAlignment(8);



		errs() << "wrapper_llvm [NOT] done!\n";
  	}
    
  	//malloc struct
  	void mallon_struct	(Module *M,PointerType* StrctPtr,ConstantInt* const_int64_17,AllocaInst* str,Instruction *nextInstruction)
  	{ 
  		PointerType* PointerTy_3 = PointerType::get(IntegerType::get(M->getContext(), 8), 0);
  		std::vector<Type*>FuncTy_151_args;
 		FuncTy_151_args.push_back(IntegerType::get(M->getContext(), 64));
 		FunctionType* FuncTy_151 = FunctionType::get(
 		   /*Result=*/PointerTy_3,
 		   /*Params=*/FuncTy_151_args,
                    /*isVarArg=*/false);
 		  
 		  
 		func_malloc = M->getFunction("malloc");
 		if (!func_malloc) 
 		{
	
			func_malloc = Function::Create(
 			/*Type=*/FuncTy_151,
 			/*Linkage=*/GlobalValue::ExternalLinkage,
 			/*Name=*/"malloc", M); // (external, no body)
 			func_malloc->setCallingConv(CallingConv::C);
 		}
 		  
 		AttributeSet func_malloc_PAL;
 		{
 			SmallVector<AttributeSet, 4> Attrs;
 		    AttributeSet PAS;
 		    {
 		    	AttrBuilder B;
 		    	PAS = AttributeSet::get(M->getContext(), ~0U, B);
 		    }
   
 		    Attrs.push_back(PAS);
 			func_malloc_PAL = AttributeSet::get(M->getContext(), Attrs);
   
 		}
 		func_malloc->setAttributes(func_malloc_PAL);
 		  
		ptr_mall_call = CallInst::Create(func_malloc, const_int64_17, "call3", nextInstruction);
  		ptr_mall_call->setCallingConv(CallingConv::C);
  		ptr_mall_call->setTailCall(false);
  		AttributeSet ptr_call_PAL;
  		ptr_mall_call->setAttributes(ptr_call_PAL);
    
  		CastInst* ptr_21 = new BitCastInst(ptr_mall_call, StrctPtr, "", nextInstruction);
  		StoreInst* void_22 = new StoreInst(ptr_21, str, false, nextInstruction);
  		void_22->setAlignment(8);
		errs() << "malloc Done!\n";

	}


    //tmp=tpc_task_descriptor_alloc()
    void func_tpc_alloca_do(Module *M,PointerType* StrctPtr)
    {
		  std::vector<Type*>FuncTy_15_args;
		  FuncTy_15_args.push_back(IntegerType::get(M->getContext(), 32));
		  FunctionType* FuncTy_15 = FunctionType::get(
		   /*Result=*/StrctPtr,
		   /*Params=*/FuncTy_15_args,
                   /*isVarArg=*/false);
		  
		  
		  func_tpc_task_descriptor_alloc = M->getFunction("tpc_task_descriptor_alloc");
		  if (!func_tpc_task_descriptor_alloc) 
		  {func_tpc_task_descriptor_alloc = Function::Create(
		  /*Type=*/FuncTy_15,
		  /*Linkage=*/Function::ExternalLinkage,
		  /*Name=*/"tpc_task_descriptor_alloc", M); // (external, no body)
		   func_tpc_task_descriptor_alloc->setCallingConv(CallingConv::C);
		  }
 
		  AttributeSet func_tpc_task_descriptor_alloc_PAL;
		  {
		    SmallVector<AttributeSet, 4> Attrs;
		    AttributeSet PAS;
		  {
		    AttrBuilder B;
		    PAS = AttributeSet::get(M->getContext(), ~0U, B);
		  }
  
		  Attrs.push_back(PAS);
		  func_tpc_task_descriptor_alloc_PAL = AttributeSet::get(M->getContext(), Attrs);
  
		  }
		  func_tpc_task_descriptor_alloc->setAttributes(func_tpc_task_descriptor_alloc_PAL);
		  errs() << "Func Tpc Alloca Done!\n";
    }
    
    //tpc call(tmp)
    void tpc_call_do(Module *M,PointerType* StrctPtr, AllocaInst* str,Instruction *label_entry_27)
    {
		

		  std::vector<Type*>FuncTy_18_args;
		  FuncTy_18_args.push_back(StrctPtr);
		  FunctionType* FuncTy_18 = FunctionType::get(
		   /*Result=*/Type::getVoidTy(M->getContext()),
		   /*Params=*/FuncTy_18_args,
		   /*isVarArg=*/false);
		  
		  
		  func_tpc_call = M->getFunction("tpc_call");
		  if (!func_tpc_call) 
		  {
		    func_tpc_call = Function::Create(
		  /*Type=*/FuncTy_18,
		  /*Linkage=*/GlobalValue::ExternalLinkage,
		  /*Name=*/"tpc_call", M); // (external, no body)
		   func_tpc_call->setCallingConv(CallingConv::C);
		  }
 
		  AttributeSet func_tpc_call_PAL;
		  {
		    SmallVector<AttributeSet, 4> Attrs;
		    AttributeSet PAS;
		  {
		    AttrBuilder B;
		    PAS = AttributeSet::get(M->getContext(), ~0U, B);
		  }
  
		  Attrs.push_back(PAS);
		  func_tpc_call_PAL = AttributeSet::get(M->getContext(), Attrs);
  
		  }
		  func_tpc_call->setAttributes(func_tpc_call_PAL);

		  // CastInst* ptr_32 = new BitCastInst(ptr_mall_call, StrctPtr, "", label_entry_27);
 		 //  StoreInst* void_33 = new StoreInst(ptr_32, str, false, label_entry_27);
 		 //  void_33->setAlignment(8);
 		  LoadInst* ptr_34 = new LoadInst(str, "", false, label_entry_27);
 		  ptr_34->setAlignment(8);
 		  CallInst* void_35 = CallInst::Create(func_tpc_call, ptr_34, "", label_entry_27);
 		  void_35->setCallingConv(CallingConv::C);
 		  void_35->setTailCall(false);
 		  AttributeSet void_35_PAL;
 		  void_35->setAttributes(void_35_PAL);

		  errs() << "tpc_call Done!\n";
    }
    
    
    
    //alloca_do
    void alloca_do(ConstantInt* const_int32_2,AllocaInst* str,Instruction * label_entry)
    {
		  ptr_call_public = CallInst::Create(func_tpc_task_descriptor_alloc, const_int32_2, "call4", label_entry);
		  ptr_call_public->setCallingConv(CallingConv::C);
		  ptr_call_public->setTailCall(false);
		  AttributeSet ptr_call_public_PAL;
		  ptr_call_public->setAttributes(ptr_call_public_PAL);
  
 		  StoreInst* void_21 = new StoreInst(ptr_call_public, str, false, label_entry);
 		  void_21->setAlignment(8); 
		  errs() << "Alloca_do Done!\n";
    }
    
    //tmp->args = (tpc_task_argument* )(tmp + 1)
    void StoreInst_args( CallInst* ptr_mall_call, PointerType* StrctPtr,Instruction * nextInstruction,AllocaInst* str,ConstantInt* const_int64_1,ConstantInt* const_int32_0,ConstantInt* const_int32_1,PointerType* PointerTy_9)
    {
    	  CastInst* ptr_23 = new BitCastInst(ptr_mall_call, StrctPtr, "", nextInstruction);
		  StoreInst* void_24 = new StoreInst(ptr_23, str, false, nextInstruction);
		  void_24->setAlignment(8);
		  LoadInst* ptr_25 = new LoadInst(str, "", false, nextInstruction);
		  ptr_25->setAlignment(8);
		  GetElementPtrInst* ptr_add_ptr = GetElementPtrInst::Create(ptr_25, const_int64_1, "add.ptr", nextInstruction);
		  CastInst* ptr_26 = new BitCastInst(ptr_add_ptr, PointerTy_9, "", nextInstruction);
		  LoadInst* ptr_277 = new LoadInst(str, "", false, nextInstruction);
		  ptr_277->setAlignment(8);
		  std::vector<Value*> ptr_args_indices;
		  ptr_args_indices.push_back(const_int32_0);
		  ptr_args_indices.push_back(const_int32_1);
		  Instruction* ptr_args = GetElementPtrInst::Create(ptr_277, ptr_args_indices, "args", nextInstruction);
		  StoreInst* void_288 = new StoreInst(ptr_26, ptr_args, false, nextInstruction);
		  void_288->setAlignment(8);

    }
    
    //args
    void args_do(ConstantInt* const_int64_1,ConstantInt* const_int32_0,ConstantInt* const_int32_1,
		 PointerType* PointerTy_9,LoadInst* ptr_19,Instruction * label_entry)
    {
		  GetElementPtrInst* ptr_add_ptr = GetElementPtrInst::Create(ptr_19, const_int64_1, "add.ptr", label_entry);
		  CastInst* ptr_21 = new BitCastInst(ptr_add_ptr, PointerTy_9, "", label_entry);
		  std::vector<Value*> ptr_args_indices0;
		  ptr_args_indices0.push_back(const_int32_0);
		  ptr_args_indices0.push_back(const_int32_1);
		  Instruction* ptr_args0 = GetElementPtrInst::Create(ptr_19, ptr_args_indices0, "args", label_entry);
 		  StoreInst* void_23 = new StoreInst(ptr_21, ptr_args0, false, label_entry); 
  		  void_23->setAlignment(8); 
		  errs() << "args_do Done!\n";
    }
    
   
    
    //tmp->parent = parent
    void parent_do(Module *M,PointerType *PointerTy_9,Instruction * label_entry)
    {
		  GlobalVariable* gvar_ptr_parent = new GlobalVariable(/*Module=*/*M, 
		  /*Type=*/PointerTy_9,
		  /*isConstant=*/false,
		  /*Linkage=*/GlobalValue::ExternalLinkage,
		  /*Initializer=*/0, 
		  /*Name=*/"parent");
		  gvar_ptr_parent->setAlignment(8);
		  gvar_ptr_parent->setThreadLocalMode(GlobalVariable::GeneralDynamicTLSModel);
		  
		  
		   //load ptr var
		  ptr_par = new LoadInst(gvar_ptr_parent, "", false, label_entry);
		  ptr_par->setAlignment(8);
		  errs() << "parent_do Done!\n";
    }
    

//StoreInst parent funtion in action (!)
    void StoreInst_parent(ConstantInt* const_int32_1,ConstantInt* const_int32_2, LoadInst* ptr_19, LoadInst* ptr_par,std::string nameofvalue,Instruction * label_entry,int align,int flag)
    {
            std::vector<Value*> ptr_args_num_indices3;
		 	ptr_args_num_indices3.push_back(const_int32_1);
		  	ptr_args_num_indices3.push_back(const_int32_2);
 		  	Instruction* ptr_args_num3 = GetElementPtrInst::Create(ptr_19, ptr_args_num_indices3, nameofvalue, label_entry);
            StoreInst* void_3 = new StoreInst(ptr_par, ptr_args_num3, false, label_entry);
            void_3->setAlignment(8);
            errs() << "StoreInst_do [" << nameofvalue <<"] Done!\n";
       
    }

    
    //StoreInst funtion in action (!)
    void StoreInst_do(ConstantInt* const_int32_1,ConstantInt* const_int32_2, LoadInst* ptr_19,std::string nameofvalue,Instruction * label_entry,int align,int flag)
    {
             std::vector<Value*> ptr_args_num_indices3;
		 	 ptr_args_num_indices3.push_back(const_int32_1);
		  	 ptr_args_num_indices3.push_back(const_int32_2);
 		  	 Instruction* ptr_args_num3 = GetElementPtrInst::Create(ptr_19, ptr_args_num_indices3, nameofvalue, label_entry);
             if(flag)
             {
                StoreInst* void_3 = new StoreInst(const_int32_1, ptr_args_num3, false, label_entry); 
                void_3->setAlignment(align);
             }
             else
             {
                StoreInst* void_3 = new StoreInst(ptr_19, ptr_args_num3, false, label_entry);
                void_3->setAlignment(align);
             }
                ptr_args_num_indices3.clear();
		  		errs() << "StoreInst_do [" << nameofvalue <<"] Done!\n";
    }
    //StoreInst funtion in action args(!)
    //tmp->args_num
    void StoreInst_args_do(ConstantInt* const_int32_1,ConstantInt* const_int32_2, LoadInst* ptr_19,std::string nameofvalue,Instruction * label_entry,int align)
    {
                  std::vector<Value*> ptr_args_num_indices3;
		  ptr_args_num_indices3.push_back(const_int32_1);
		  ptr_args_num_indices3.push_back(const_int32_2);
 		  Instruction* ptr_args_num3 = GetElementPtrInst::Create(ptr_19, ptr_args_num_indices3, nameofvalue, label_entry);
                  StoreInst* void_3 = new StoreInst(const_int32_2, ptr_args_num3, false, label_entry); 
                  void_3->setAlignment(align);
		  ptr_args_num_indices3.clear();
		  errs() << "StoreInst_do [" << nameofvalue <<"] Done!\n";
    }
    
    //(tmp->args)++;
    void  arg_sinsin(ConstantInt* const_int32_0,ConstantInt* const_int32_1,AllocaInst* str,std::string prevname,int counter,Instruction * label_entry)
    {
		   LoadInst* ptr_199 = new LoadInst(str, "", false, label_entry);
		   ptr_199->setAlignment(8);
		   std::vector<Value*> ptr_args_indices;
		   ptr_args_indices.push_back(const_int32_0);
		   ptr_args_indices.push_back(const_int32_1);
		   Instruction* ptr_args = GetElementPtrInst::Create(ptr_199, ptr_args_indices, "args", label_entry);
		   LoadInst* ptr_20 = new LoadInst(ptr_args, "", false, label_entry);
		   ptr_20->setAlignment(8);
		   GetElementPtrInst* ptr_incdec_ptr = GetElementPtrInst::Create(ptr_20, const_int32_1, "incdec.ptr", label_entry);
		   StoreInst* void_21 = new StoreInst(ptr_incdec_ptr, ptr_args, false, label_entry);
		   void_21->setAlignment(8);
		   errs() << "[" << prevname << "]" << "arg No" << counter+1 << "(++;) Done!\n";
    }
    
    //(tmp->args)->stride
    void stride_do(ConstantInt* const_int32_0,ConstantInt* const_int32_1,ConstantInt* const_int32_4,
		 AllocaInst* str,Instruction * label_entry)
     {
		    LoadInst* ptr_20 = new LoadInst(str, "", false, label_entry);
		    ptr_20->setAlignment(8);
		    std::vector<Value*> ptr_args_indices;
		    ptr_args_indices.push_back(const_int32_0);
		    ptr_args_indices.push_back(const_int32_1);
		    Instruction* ptr_args = GetElementPtrInst::Create(ptr_20, ptr_args_indices, "args", label_entry);
		    LoadInst* ptr_21 = new LoadInst(ptr_args, "", false, label_entry);
		    ptr_21->setAlignment(8);
		    std::vector<Value*> ptr_stride_indices;
		    ptr_stride_indices.push_back(const_int32_0);
		    ptr_stride_indices.push_back(const_int32_4);
		    Instruction* ptr_stride = GetElementPtrInst::Create(ptr_21, ptr_stride_indices, "stride", label_entry);
		    StoreInst* void_22 = new StoreInst(const_int32_0, ptr_stride, false, label_entry);
		    void_22->setAlignment(4);
		    errs() << "stride Done!\n";
     }
     
    //(tmp->args)->element_num
    void element_num_do(ConstantInt* const_int32_0,ConstantInt* const_int32_1,ConstantInt* const_int32_5,
		 AllocaInst* str,Instruction * label_entry)
     {
		    LoadInst* ptr_24 = new LoadInst(str, "", false, label_entry);
		    ptr_24->setAlignment(8);
		    std::vector<Value*> ptr_args1_indices;
		    ptr_args1_indices.push_back(const_int32_0);
		    ptr_args1_indices.push_back(const_int32_1);
		    Instruction* ptr_args1 = GetElementPtrInst::Create(ptr_24, ptr_args1_indices, "args1", label_entry);
		    LoadInst* ptr_25 = new LoadInst(ptr_args1, "", false, label_entry);
		    ptr_25->setAlignment(8);
		    std::vector<Value*> ptr_element_num_indices;
		    ptr_element_num_indices.push_back(const_int32_0);
		    ptr_element_num_indices.push_back(const_int32_5);
		    Instruction* ptr_element_num = GetElementPtrInst::Create(ptr_25, ptr_element_num_indices, "element_num", label_entry);
		    StoreInst* void_26 = new StoreInst(const_int32_0, ptr_element_num, false, label_entry);
		    void_26->setAlignment(4);
		    errs() << "element_num Done!\n";
     }
    
    //(tmp->args)->type
    void type_do(ConstantInt* const_int32_0,ConstantInt* const_int32_1,ConstantInt* const_int32_5,ConstantInt* const_int32_type,
		 AllocaInst* str,Instruction * label_entry)
     {
		    LoadInst* ptr_24 = new LoadInst(str, "", false, label_entry);
		    ptr_24->setAlignment(8);
		    std::vector<Value*> ptr_args1_indices;
		    ptr_args1_indices.push_back(const_int32_0);
		    ptr_args1_indices.push_back(const_int32_1);
		    Instruction* ptr_args1 = GetElementPtrInst::Create(ptr_24, ptr_args1_indices, "args2", label_entry);
		    LoadInst* ptr_25 = new LoadInst(ptr_args1, "", false, label_entry);
		    ptr_25->setAlignment(8);
		    std::vector<Value*> ptr_element_num_indices;
		    ptr_element_num_indices.push_back(const_int32_0);
		    ptr_element_num_indices.push_back(const_int32_5);
		    Instruction* ptr_element_num = GetElementPtrInst::Create(ptr_25, ptr_element_num_indices, "type", label_entry);
		    StoreInst* void_26 = new StoreInst(const_int32_type, ptr_element_num, false, label_entry);
		    void_26->setAlignment(4);
		    errs() << "type Done!\n";
     }





     //(tmp->args)->addr_in
    void addr_in(Module* M,ConstantInt* const_int32_0,ConstantInt* const_int32_1,Value*  argument_operand,unsigned numtype,uint64_t typesize,AllocaInst* str, PointerType* PointerTy_6,Instruction * label_entry)
     {

     	//ConstantInt* const_int32_24 = ConstantInt::get(M->getContext(), APInt(32, StringRef("8"), 10));
     	ConstantInt* const_int1_25 = ConstantInt::get(M->getContext(), APInt(1, StringRef("0"), 10));
     	ConstantInt* const_int64_23 = ConstantInt::get(M->getContext(), APInt(64, StringRef("8"), 10));
		ConstantInt* const_int32_24 = ConstantInt::get(M->getContext(), APInt(32, StringRef("4"), 10));
    	PointerTy_4 = PointerType::get(IntegerType::get(M->getContext(), 8), 0);
    	PointerType* PointerTy_14 = PointerType::get(IntegerType::get(M->getContext(), 32), 0);
    	PointerType* PointerTy_12 = PointerType::get(IntegerType::get(M->getContext(), 8), 0);
    	AllocaInst* ptr_x;
    	CastInst* ptr_33;
    	CastInst* ptr_32;
    	CastInst* int64_conv;
    	AllocaInst* ptr_numv = new AllocaInst(PointerTy_4, "numv", label_entry);
 	    ptr_numv->setAlignment(8);

		
    	if(numtype == 10 && typesize == 4)
    	{
    		
    		ptr_x = new AllocaInst(IntegerType::get(M->getContext(), 32), "x", label_entry);
  			ptr_x->setAlignment(4);
  		}
  		else if(numtype == 10 && typesize == 1)
  		{
  			
  			ptr_x = new AllocaInst(IntegerType::get(M->getContext(), 8), "x", label_entry);
  			ptr_x->setAlignment(1);
  		}
  		else if(numtype == 14)
  		{
  			
  			ptr_x = new AllocaInst(PointerTy_14, "x", label_entry);
  			ptr_x->setAlignment(4);
  		}
  		else 
  		{
  			if(numtype == 2 && typesize == 4)
  			{

  				ptr_x = new AllocaInst(Type::getFloatTy(M->getContext()), "x", label_entry);
 				ptr_x->setAlignment(4);
  				
  			}
  			if(numtype == 3 && typesize == 8)
  			{
  				ptr_x = new AllocaInst(Type::getDoubleTy(M->getContext()), "x", label_entry);
 				ptr_x->setAlignment(8);
  				
  			}
  		
			/*|------memcopy------|*/

  			std::vector<Type*>FuncTy_18_args;
 			FuncTy_18_args.push_back(PointerTy_4);
 			FuncTy_18_args.push_back(PointerTy_4);
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 64));
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 32));
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 1));
 			FunctionType* FuncTy_18 = FunctionType::get(
 			 /*Result=*/Type::getVoidTy(M->getContext()),
 			 /*Params=*/FuncTy_18_args,
 			 /*isVarArg=*/false);
	
	  		Function* func_llvm_memcpy_p0i8_p0i8_i64 = M->getFunction("llvm.memcpy.p0i8.p0i8.i64");
	 		if (!func_llvm_memcpy_p0i8_p0i8_i64) {
	 		func_llvm_memcpy_p0i8_p0i8_i64 = Function::Create(
	 		 /*Type=*/FuncTy_18,
	 		 /*Linkage=*/GlobalValue::ExternalLinkage,
	 		 /*Name=*/"llvm.memcpy.p0i8.p0i8.i64", M); // (external, no body)
	 		func_llvm_memcpy_p0i8_p0i8_i64->setCallingConv(CallingConv::C);
	 		}
	 		AttributeSet func_llvm_memcpy_p0i8_p0i8_i64_PAL;
	 		{
	 		 SmallVector<AttributeSet, 4> Attrs;
	 		 AttributeSet PAS;
	 		  {
	 		   AttrBuilder B;
	 		   B.addAttribute(Attribute::NoCapture);
	 		   PAS = AttributeSet::get(M->getContext(), 1U, B);
	 		  }
	 		 
	 		 Attrs.push_back(PAS);
	 		 {
	 		  AttrBuilder B;
	 		  B.addAttribute(Attribute::ReadOnly);
	 		  B.addAttribute(Attribute::NoCapture);
	 		  PAS = AttributeSet::get(M->getContext(), 2U, B);
	 		 }
	 		
	 		Attrs.push_back(PAS);
	 		{
	 		 AttrBuilder B;
	 		 B.addAttribute(Attribute::NoUnwind);
	 		 PAS = AttributeSet::get(M->getContext(), ~0U, B);
	 		}
			Attrs.push_back(PAS);
			func_llvm_memcpy_p0i8_p0i8_i64_PAL = AttributeSet::get(M->getContext(), Attrs);
			
			}
			func_llvm_memcpy_p0i8_p0i8_i64->setAttributes(func_llvm_memcpy_p0i8_p0i8_i64_PAL);


			StoreInst* void_31 = new StoreInst(argument_operand, ptr_x, false, label_entry);
 			void_31->setAlignment(4);
	 		ptr_32 = new BitCastInst(ptr_numv, PointerTy_4, "", label_entry);
	 		ptr_33 = new BitCastInst(ptr_x, PointerTy_4, "", label_entry);
	 		std::vector<Value*> void_34_params;
	 		void_34_params.push_back(ptr_32);
	 		void_34_params.push_back(ptr_33);
	 		void_34_params.push_back(const_int64_23);
	 		void_34_params.push_back(const_int32_24);
	 		void_34_params.push_back(const_int1_25);
	 		CallInst* void_34 = CallInst::Create(func_llvm_memcpy_p0i8_p0i8_i64, void_34_params, "", label_entry);
	 		void_34->setCallingConv(CallingConv::C);
	 		void_34->setTailCall(false);
	 		AttributeSet void_34_PAL;
 			void_34->setAttributes(void_34_PAL);
	 	
 		}	

		/*|---------end_memcopy----------|*/


		StoreInst* void_31 ;

  		LoadInst* int32_32 ;
  		
  		if(numtype == 10 )
  		{
  			void_31 = new StoreInst(argument_operand, ptr_x, false, label_entry);
  			void_31->setAlignment(4);
			int32_32 = new LoadInst(ptr_x, "", false, label_entry);
 			int32_32->setAlignment(4);
    		int64_conv = new SExtInst(int32_32, IntegerType::get(M->getContext(), 64), "conv", label_entry);
  			ptr_33 = new IntToPtrInst(int64_conv, PointerTy_4, "", label_entry);
  			LoadInst* ptr_34 = new LoadInst(str, "", false, label_entry);
  			ptr_34->setAlignment(8);
  			std::vector<Value*> ptr_args_indices;
  			ptr_args_indices.push_back(const_int32_0);
  			ptr_args_indices.push_back(const_int32_1);
  			Instruction* ptr_args = GetElementPtrInst::Create(ptr_34, ptr_args_indices, "args", label_entry);
  			LoadInst* ptr_35 = new LoadInst(ptr_args, "", false, label_entry);
  			ptr_35->setAlignment(8);
  			std::vector<Value*> ptr_addr_in_indices;
  			ptr_addr_in_indices.push_back(const_int32_0);
  			ptr_addr_in_indices.push_back(const_int32_0);
  			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_35, ptr_addr_in_indices, "addr_in", label_entry);
  			StoreInst* void_36 = new StoreInst(ptr_33, ptr_addr_in, false, label_entry);
  			void_36->setAlignment(8);
		}
		else if(numtype == 14)
		{
			StoreInst* void_27 = new StoreInst(argument_operand, ptr_x, false, label_entry);
  			void_27->setAlignment(8);
  			LoadInst* ptr_28 = new LoadInst(ptr_x, "", false, label_entry);
  			ptr_28->setAlignment(8);
  			CastInst* ptr_29 = new BitCastInst(ptr_28, PointerTy_12, "", label_entry);
  			LoadInst* ptr_30 = new LoadInst(str, "", false, label_entry);
  			ptr_30->setAlignment(8);
  			std::vector<Value*> ptr_args_indices11;
  			ptr_args_indices11.push_back(const_int32_0);
  			ptr_args_indices11.push_back(const_int32_1);
  			Instruction* ptr_args = GetElementPtrInst::Create(ptr_30, ptr_args_indices11, "args", label_entry);
  			LoadInst* ptr_31 = new LoadInst(ptr_args, "", false, label_entry);
  			ptr_31->setAlignment(8);
  			std::vector<Value*> ptr_addr_in_indices;
  			ptr_addr_in_indices.push_back(const_int32_0);
  			ptr_addr_in_indices.push_back(const_int32_0);
  			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_31, ptr_addr_in_indices, "addr_in", label_entry);
  			StoreInst* void_32 = new StoreInst(ptr_29, ptr_addr_in, false, label_entry);
  			void_32->setAlignment(8);
		}
		else
		{
			LoadInst* ptr_35 = new LoadInst(ptr_numv, "", false, label_entry);
 			ptr_35->setAlignment(8);
 			LoadInst* ptr_36 = new LoadInst(str, "", false, label_entry);
 			ptr_36->setAlignment(8);
 			std::vector<Value*> ptr_args_indices;
 			ptr_args_indices.push_back(const_int32_0);
 			ptr_args_indices.push_back(const_int32_1);
 			Instruction* ptr_args = GetElementPtrInst::Create(ptr_36, ptr_args_indices, "args", label_entry);
 			LoadInst* ptr_37 = new LoadInst(ptr_args, "", false, label_entry);
 			ptr_37->setAlignment(8);
 			std::vector<Value*> ptr_addr_in_indices;
 			ptr_addr_in_indices.push_back(const_int32_0);
 			ptr_addr_in_indices.push_back(const_int32_0);
 			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_37, ptr_addr_in_indices, "addr_in", label_entry);
 			StoreInst* void_38 = new StoreInst(ptr_35, ptr_addr_in, false, label_entry);
 			void_38->setAlignment(8);
		}
     	

    	 	errs() << "addr_in done!\n";
     }

	//(tmp->args)->addr_out
    void addr_out(Module* M,ConstantInt* const_int32_0,ConstantInt* const_int32_1,Value*  argument_operand,unsigned numtype,uint64_t typesize,AllocaInst* str, PointerType* PointerTy_6,Instruction * label_entry)
     {

     		//ConstantInt* const_int32_24 = ConstantInt::get(M->getContext(), APInt(32, StringRef("8"), 10));
     	ConstantInt* const_int1_25 = ConstantInt::get(M->getContext(), APInt(1, StringRef("0"), 10));
     	ConstantInt* const_int64_23 = ConstantInt::get(M->getContext(), APInt(64, StringRef("8"), 10));
		ConstantInt* const_int32_24 = ConstantInt::get(M->getContext(), APInt(32, StringRef("4"), 10));
    	PointerTy_4 = PointerType::get(IntegerType::get(M->getContext(), 8), 0);
    	PointerType* PointerTy_14 = PointerType::get(IntegerType::get(M->getContext(), 32), 0);
    	PointerType* PointerTy_12 = PointerType::get(IntegerType::get(M->getContext(), 8), 0);
    	AllocaInst* ptr_x;
    	CastInst* ptr_33;
    	CastInst* ptr_32;
    	CastInst* int64_conv;
    	AllocaInst* ptr_numv = new AllocaInst(PointerTy_4, "numv", label_entry);
 	    ptr_numv->setAlignment(8);

		
    	if(numtype == 10 && typesize == 4)
    	{
    		
    		ptr_x = new AllocaInst(IntegerType::get(M->getContext(), 32), "x", label_entry);
  			ptr_x->setAlignment(4);
  		}
  		else if(numtype == 10 && typesize == 1)
  		{
  			
  			ptr_x = new AllocaInst(IntegerType::get(M->getContext(), 8), "x", label_entry);
  			ptr_x->setAlignment(1);
  		}
  		else if(numtype == 14)
  		{
  			
  			ptr_x = new AllocaInst(PointerTy_14, "x", label_entry);
  			ptr_x->setAlignment(4);
  		}
  		else 
  		{
  			if(numtype == 2 && typesize == 4)
  			{

  				ptr_x = new AllocaInst(Type::getFloatTy(M->getContext()), "x", label_entry);
 				ptr_x->setAlignment(4);
  				
  			}
  			if(numtype == 3 && typesize == 8)
  			{
  				ptr_x = new AllocaInst(Type::getDoubleTy(M->getContext()), "x", label_entry);
 				ptr_x->setAlignment(8);
  				
  			}
  		
			/*|------memcopy------|*/

  			std::vector<Type*>FuncTy_18_args;
 			FuncTy_18_args.push_back(PointerTy_4);
 			FuncTy_18_args.push_back(PointerTy_4);
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 64));
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 32));
 			FuncTy_18_args.push_back(IntegerType::get(M->getContext(), 1));
 			FunctionType* FuncTy_18 = FunctionType::get(
 			 /*Result=*/Type::getVoidTy(M->getContext()),
 			 /*Params=*/FuncTy_18_args,
 			 /*isVarArg=*/false);
	
	  		Function* func_llvm_memcpy_p0i8_p0i8_i64 = M->getFunction("llvm.memcpy.p0i8.p0i8.i64");
	 		if (!func_llvm_memcpy_p0i8_p0i8_i64) {
	 		func_llvm_memcpy_p0i8_p0i8_i64 = Function::Create(
	 		 /*Type=*/FuncTy_18,
	 		 /*Linkage=*/GlobalValue::ExternalLinkage,
	 		 /*Name=*/"llvm.memcpy.p0i8.p0i8.i64", M); // (external, no body)
	 		func_llvm_memcpy_p0i8_p0i8_i64->setCallingConv(CallingConv::C);
	 		}
	 		AttributeSet func_llvm_memcpy_p0i8_p0i8_i64_PAL;
	 		{
	 		 SmallVector<AttributeSet, 4> Attrs;
	 		 AttributeSet PAS;
	 		  {
	 		   AttrBuilder B;
	 		   B.addAttribute(Attribute::NoCapture);
	 		   PAS = AttributeSet::get(M->getContext(), 1U, B);
	 		  }
	 		 
	 		 Attrs.push_back(PAS);
	 		 {
	 		  AttrBuilder B;
	 		  B.addAttribute(Attribute::ReadOnly);
	 		  B.addAttribute(Attribute::NoCapture);
	 		  PAS = AttributeSet::get(M->getContext(), 2U, B);
	 		 }
	 		
	 		Attrs.push_back(PAS);
	 		{
	 		 AttrBuilder B;
	 		 B.addAttribute(Attribute::NoUnwind);
	 		 PAS = AttributeSet::get(M->getContext(), ~0U, B);
	 		}
			Attrs.push_back(PAS);
			func_llvm_memcpy_p0i8_p0i8_i64_PAL = AttributeSet::get(M->getContext(), Attrs);
			
			}
			func_llvm_memcpy_p0i8_p0i8_i64->setAttributes(func_llvm_memcpy_p0i8_p0i8_i64_PAL);


			StoreInst* void_31 = new StoreInst(argument_operand, ptr_x, false, label_entry);
 			void_31->setAlignment(4);
	 		ptr_32 = new BitCastInst(ptr_numv, PointerTy_4, "", label_entry);
	 		ptr_33 = new BitCastInst(ptr_x, PointerTy_4, "", label_entry);
	 		std::vector<Value*> void_34_params;
	 		void_34_params.push_back(ptr_32);
	 		void_34_params.push_back(ptr_33);
	 		void_34_params.push_back(const_int64_23);
	 		void_34_params.push_back(const_int32_24);
	 		void_34_params.push_back(const_int1_25);
	 		CallInst* void_34 = CallInst::Create(func_llvm_memcpy_p0i8_p0i8_i64, void_34_params, "", label_entry);
	 		void_34->setCallingConv(CallingConv::C);
	 		void_34->setTailCall(false);
	 		AttributeSet void_34_PAL;
 			void_34->setAttributes(void_34_PAL);
	 	
 		}	

		/*|---------end_memcopy----------|*/


		StoreInst* void_31 ;

  		LoadInst* int32_32 ;
  		
  		if(numtype == 10 )
  		{
  			void_31 = new StoreInst(argument_operand, ptr_x, false, label_entry);
  			void_31->setAlignment(4);
			int32_32 = new LoadInst(ptr_x, "", false, label_entry);
 			int32_32->setAlignment(4);
    		int64_conv = new SExtInst(int32_32, IntegerType::get(M->getContext(), 64), "conv", label_entry);
  			ptr_33 = new IntToPtrInst(int64_conv, PointerTy_4, "", label_entry);
  			LoadInst* ptr_34 = new LoadInst(str, "", false, label_entry);
  			ptr_34->setAlignment(8);
  			std::vector<Value*> ptr_args_indices;
  			ptr_args_indices.push_back(const_int32_0);
  			ptr_args_indices.push_back(const_int32_1);
  			Instruction* ptr_args = GetElementPtrInst::Create(ptr_34, ptr_args_indices, "args", label_entry);
  			LoadInst* ptr_35 = new LoadInst(ptr_args, "", false, label_entry);
  			ptr_35->setAlignment(8);
  			std::vector<Value*> ptr_addr_in_indices;
  			ptr_addr_in_indices.push_back(const_int32_0);
  			ptr_addr_in_indices.push_back(const_int32_0);
  			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_35, ptr_addr_in_indices, "addr_out", label_entry);
  			StoreInst* void_36 = new StoreInst(ptr_33, ptr_addr_in, false, label_entry);
  			void_36->setAlignment(8);
		}
		else if(numtype == 14)
		{
			StoreInst* void_27 = new StoreInst(argument_operand, ptr_x, false, label_entry);
  			void_27->setAlignment(8);
  			LoadInst* ptr_28 = new LoadInst(ptr_x, "", false, label_entry);
  			ptr_28->setAlignment(8);
  			CastInst* ptr_29 = new BitCastInst(ptr_28, PointerTy_12, "", label_entry);
  			LoadInst* ptr_30 = new LoadInst(str, "", false, label_entry);
  			ptr_30->setAlignment(8);
  			std::vector<Value*> ptr_args_indices11;
  			ptr_args_indices11.push_back(const_int32_0);
  			ptr_args_indices11.push_back(const_int32_1);
  			Instruction* ptr_args = GetElementPtrInst::Create(ptr_30, ptr_args_indices11, "args", label_entry);
  			LoadInst* ptr_31 = new LoadInst(ptr_args, "", false, label_entry);
  			ptr_31->setAlignment(8);
  			std::vector<Value*> ptr_addr_in_indices;
  			ptr_addr_in_indices.push_back(const_int32_0);
  			ptr_addr_in_indices.push_back(const_int32_0);
  			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_31, ptr_addr_in_indices, "addr_out", label_entry);
  			StoreInst* void_32 = new StoreInst(ptr_29, ptr_addr_in, false, label_entry);
  			void_32->setAlignment(8);
		}
		else
		{
			LoadInst* ptr_35 = new LoadInst(ptr_numv, "", false, label_entry);
 			ptr_35->setAlignment(8);
 			LoadInst* ptr_36 = new LoadInst(str, "", false, label_entry);
 			ptr_36->setAlignment(8);
 			std::vector<Value*> ptr_args_indices;
 			ptr_args_indices.push_back(const_int32_0);
 			ptr_args_indices.push_back(const_int32_1);
 			Instruction* ptr_args = GetElementPtrInst::Create(ptr_36, ptr_args_indices, "args", label_entry);
 			LoadInst* ptr_37 = new LoadInst(ptr_args, "", false, label_entry);
 			ptr_37->setAlignment(8);
 			std::vector<Value*> ptr_addr_in_indices;
 			ptr_addr_in_indices.push_back(const_int32_0);
 			ptr_addr_in_indices.push_back(const_int32_0);
 			Instruction* ptr_addr_in = GetElementPtrInst::Create(ptr_37, ptr_addr_in_indices, "addr_out", label_entry);
 			StoreInst* void_38 = new StoreInst(ptr_35, ptr_addr_in, false, label_entry);
 			void_38->setAlignment(8);
		}
     	

    	 	errs() << "addr_out done!\n";
     }
   
     //(tmp->args)->size
     void size_el(ConstantInt* const_int32_0,ConstantInt* const_int32_1,ConstantInt* const_int32_5,ConstantInt* const_int32_argsize,
		 AllocaInst* str,Instruction * label_entry)
     {

     		LoadInst* ptr_24 = new LoadInst(str, "", false, label_entry);
		    ptr_24->setAlignment(8);
		    std::vector<Value*> ptr_args1_indices;
		    ptr_args1_indices.push_back(const_int32_0);
		    ptr_args1_indices.push_back(const_int32_1);
		    Instruction* ptr_args1 = GetElementPtrInst::Create(ptr_24, ptr_args1_indices, "args2", label_entry);
		    LoadInst* ptr_25 = new LoadInst(ptr_args1, "", false, label_entry);
		    ptr_25->setAlignment(8);
		    std::vector<Value*> ptr_element_num_indices;
		    ptr_element_num_indices.push_back(const_int32_0);
		    ptr_element_num_indices.push_back(const_int32_5);
		    Instruction* ptr_element_num = GetElementPtrInst::Create(ptr_25, ptr_element_num_indices, "size", label_entry);
		    StoreInst* void_26 = new StoreInst(const_int32_argsize, ptr_element_num, false, label_entry);
		    void_26->setAlignment(4);
		   
     		errs() << "size_el done!\n";
     }


    
    //IR code printing
    void IRcode(Function &F,int flag,Module *M)
    {
     errs() << "\n|----------------Analysis Of IR code--------------------------|\n";
	  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
	  {
	  	errs() << *I << "\n";
	  }
	  errs() << "\n|---------------End of Analysis Of IR code------------------|\n";
	}
    

 

    bool runOnFunction(Function &F) override 
    {
	std::string str1=F.getName();
	bool PrevIsInt=false;
	Module *M=F.getParent();
	PointerType* StrctPtr;
	PointerType* PointerTy_9;
	AllocaInst* str;
	flag++;

	

	
	
	
	
	  Function* func_main = M->getFunction("main");
	  if (!func_main) 
	  {
		errs() << "func_main fail\n";
	  }
	
	
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){

	      Function* calledFunction = NULL;
	      Function* calledFunction1 = NULL;
	      Instruction* currentInst = &*I;

	      Instruction* prevInst;
	      IRBuilder<> builder(currentInst);
	

	  if (dyn_cast < ReturnInst > (currentInst)) 
	  {
		   	if(IsInMain && deldefret)
		      {
				deldefret=false;
		      }
		      
		
	  }else if (CallInst *callInst = dyn_cast <CallInst> (currentInst)) 
	  {


		//errs()<<"NameOfCalledFunctionIs=	[";
		calledFunction = callInst->getCalledFunction();
		name=calledFunction->getName().str();

		//errs()<<name;
		//errs()<<"]\n";


	 	if(str1.compare("main")==0 && onetime==true)
		{ 


		  IRcode(F,flag,M);
		  onetime=false;
		  //structs declare
		  StructTy = M->getTypeByName("struct._tpc_task_descriptor");
		  StructTy_struct__tpc_task_argument = M->getTypeByName("struct._tpc_task_argument");
		  nextInstruction = getNextInstruction(*currentInst);
		  //declare struct
		  StrctPtr = PointerType::get(StructTy, 0);
		  PointerTy_9 = PointerType::get(StructTy_struct__tpc_task_argument, 0);
		  str = new AllocaInst(StrctPtr, "tmp", nextInstruction);
		  str->setAlignment(8);
		  errs() << "declare struct Done!\n";
		  
	
		  }

		
	/*
	 *	spawn when we find a call func
	 */
	
	  if(name.compare("spawn")== 0)
	  {	

	  

	  		
		
		  
		  MYFILE = llvm::StructType::create(F.getContext(),ParamsType, "struct.foo");
		  FILEPTR=PointerType::get(MYFILE,0);
		  Value *p;
		  p = new GlobalVariable(*M,
                                        FILEPTR,
                                        false,
                                        GlobalValue::ExternalLinkage,
                                        NULL,
                                        "foo");
		  
		  /*----------------------------stencil code------------------------------*/
		 

		  /*--------------------------arguments-----------------------------*/
		  errs() << "||----------------------------------------------------------------{";
		  calledFunction1 = prevcallInst->getCalledFunction();
		  prevname=calledFunction1->getName().str();
		  errs() << prevname << "}----------------------------------------------------------------\n"; 

		  unsigned num = prevcallInst->getNumOperands();
		  unsigned rnum = num-1;
   		  std::cerr << "num - CI->getNumOperands() " << num << std::endl;
   		  errs () << "\n";
	
    	 errs() << "----------------------------------\n";
    	/*--------------------------------------------------------------------------*/

		 
		  
		if(str1.compare("main")==0)
		{ 
		  IsInMain=true;
		  


		 
		  //structs declare
		  StructTy = M->getTypeByName("struct._tpc_task_descriptor");
		  StructTy_struct__tpc_task_argument = M->getTypeByName("struct._tpc_task_argument");
		  
		  //save args to string to create ConstantInt
          std::string s_args = std::to_string(numofargs);
		  //ConstantInt declare
		  ConstantInt* const_int32_0 = ConstantInt::get(M->getContext(), APInt(32, StringRef("0"), 10));
		  ConstantInt* const_int32_1 = ConstantInt::get(M->getContext(), APInt(32, StringRef("1"), 10));
		  ConstantInt* const_int32_2 = ConstantInt::get(M->getContext(), APInt(32, StringRef("2"), 10));
		  ConstantInt* const_int32_4 = ConstantInt::get(M->getContext(), APInt(32, StringRef("4"), 10));
		  ConstantInt* const_int32_5 = ConstantInt::get(M->getContext(), APInt(32, StringRef("5"), 10));
		  //ConstantInt* const_int32_args = ConstantInt::get(M->getContext(), APInt(32, StringRef(s_args), 10));
		  //ConstantInt* const_int32_type = ConstantInt::get(M->getContext(), APInt(32, StringRef("10"), 10));
          ConstantInt* const_int64_1 = ConstantInt::get(M->getContext(), APInt(64, StringRef("1"), 10));
		  ConstantInt* const_int64_17 = ConstantInt::get(M->getContext(), APInt(64, StringRef("72"), 10));

                  
		  nextInstruction = getNextInstruction(*currentInst);
		//malloc struct
		   mallon_struct	(M,StrctPtr,const_int64_17,str,nextInstruction);		  
		  
	  	
		  
		  // // //func_tpc_task_descriptor_alloc
		  func_tpc_alloca_do(M,StrctPtr);
		  
		  //Alloca 
		  alloca_do(const_int32_2,str,nextInstruction);

		  //wrapper_Scoop 
		  wrapper_llvm(M,PointerTy_9,StrctPtr,ptr_mall_call,str,const_int32_0,prevname,nextInstruction);
		  
		 
		    //load ptr struct
		  LoadInst* ptr_19 = new LoadInst(str, "", false, nextInstruction);
		  ptr_19->setAlignment(8);
		  //args 
		  	args_do(const_int64_1,const_int32_0,const_int32_1,PointerTy_9,ptr_19,nextInstruction);
		  
		 

		  std::string typeargnum = std::to_string(rnum);
		  ConstantInt* const_int32_argsnum = ConstantInt::get(M->getContext(), APInt(32, StringRef(typeargnum), 10));
	      //tmp->args_num
		  nameofvalue="args_num";
		  StoreInst_args_do(const_int32_argsnum,const_int32_2,ptr_19,nameofvalue,nextInstruction,4);
		  
		
		  
		  //tmp->number_of_children
          nameofvalue="number_of_children";
		  StoreInst_do(const_int32_0,const_int32_2,ptr_19,nameofvalue,nextInstruction,4,1);
		  

		    //tmp->status
		  nameofvalue="status";
		  StoreInst_do(const_int32_0,const_int32_2,ptr_19,nameofvalue,nextInstruction,4,1);
		  
		  //parent declare
		  parent_do(M,StrctPtr,nextInstruction);
		 
		  //tmp->parent
          nameofvalue="parent";
		  // StoreInst_parent(const_int32_0,const_int32_5,ptr_19,ptr_par,nameofvalue,nextInstruction,8,0);
		  /*  */
		  
		  errs()<< "\n-----------------elements---------------------\n";


		  


		  for(unsigned i=0; i < num -1 ; i++)
		  {
		

            Value *e = prevcallInst->getArgOperand(i);
            assert(e && "no value!");
		    Type* eType = e->getType();
            assert(eType && "no type found!");
            std::cerr << "Arg: " << e->getName().str() << std::endl;

            std::cerr << "Arg: " << e->getValueID()<< std::endl;
            unsigned numtype=eType->getTypeID();
            std::cerr << "ArgType: " << numtype << std::endl;
		    DataLayout* dl = new llvm::DataLayout(M); 
   			uint64_t typesize = dl->getTypeStoreSize(eType); 
		    std::cerr << "SizeofArg: " << typesize << "%" << std::endl;


		    std::string typearg = std::to_string(numtype);
			ConstantInt* const_int32_argtype = ConstantInt::get(M->getContext(), APInt(32, StringRef(typearg), 10));

			//(tmp->args)->addr_in;(NOT YET)
		    addr_in(M,const_int32_0,const_int32_1,prevInst->getOperand(i),numtype,typesize,str,PointerTy_9,nextInstruction);
		    addr_out(M,const_int32_0,const_int32_1,prevInst->getOperand(i),numtype,typesize,str,PointerTy_9,nextInstruction);
			
			//(tmp->args)->type;
		    type_do(const_int32_0,const_int32_1,const_int32_2,const_int32_argtype,str,nextInstruction);
		    
			//(tmp->args)->size;
		    std::string sizearg = std::to_string(typesize);
		    ConstantInt* const_int32_argsize = ConstantInt::get(M->getContext(), APInt(32, StringRef(sizearg), 10));
			size_el(const_int32_0,const_int32_1,const_int32_2,const_int32_argsize,str,nextInstruction);
		    
		    //(tmp->args)->stride(NOT CORRECT ALWAYS 0);
		    stride_do(const_int32_0,const_int32_1,const_int32_4,str,nextInstruction);
		    
		    //(tmp->args)->element_num(NOT CORRECT ALWAYS 0);
		    element_num_do(const_int32_0,const_int32_1,const_int32_5,str,nextInstruction);
		    
		    //(tmp->arg)->++;
		    arg_sinsin(const_int32_0,const_int32_1,str,prevname,i,nextInstruction);
		    
		    errs()<< "\n--------------end_element_" << i + 1 << "-----------------\n";

		  }
		  
		 
		  
		 // // //  //(tmp->args) = (tpc_task_arguments *)(tmp + 1)
		  args_do(const_int64_1,const_int32_0,const_int32_1,PointerTy_9,ptr_19,nextInstruction);
		  
		 // //  //tpc_call(temp)
		//printllvm(M,const_int32_0,const_int32_1,str,nextInstruction);
		
		 tpc_call_do(M,StrctPtr,str,nextInstruction);
		  /*---------------------------------------------------------------------*/
		  

		  //insert spawns and call funcs in a list
          insert_s(headg,prevInst);
          insert_s(headg,currentInst);

          //replace all uses with undef type
    	  currentInst->replaceAllUsesWith(UndefValue::get(currentInst->getType()));
    	  prevInst->replaceAllUsesWith(UndefValue::get(prevInst->getType()));

	      ParamsType.clear();
		  Params.clear();
		  PrevIsInt=true;
		}
		  
	        }
	/*--------*/
	        else
		{
		  calledFunction = callInst->getCalledFunction();
		  prevname=calledFunction->getName().str();
		  CallSite w(callInst);

		  prevcallInst=callInst;
		
		}
	  
	    
	  } 
	  prevInst = &*I;

	  	if (F.getName()=="main" &&  dyn_cast < ReturnInst > (currentInst)) 
	  	{
			delete_func_calls();
	  	}
	  

	}
	

	


	return false;
      }

  };
}

char MyPass::ID = 0;
static RegisterPass<MyPass> X("PassRAF", "R A F",true,true);