#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Verifier.h"/*
#include "llvm/Assembly/PrintModulePass.h"*/
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
#include "llvm/IR/LLVMContext.h"

#include "llvm/Bitcode/ReaderWriter.h"


#include "llvm/IR/Operator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"

#include <vector>
#include <list>
#include <stdio.h>
using namespace llvm;

#define DEBUG_TYPE "mypass"

STATISTIC(HelloCounter, "Counts number of functions greeted");

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
  struct MyPass : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    MyPass() : FunctionPass(ID) {
  
    }
    Function *PrintfFunc;
    std::string name;
    StructType* MYFILE;
    Type* FILEPTR;
    StructType* marker;
    PointerType* marker_ptr ;
    Value *GVstderr;
    std::vector<Value*> Params;
    std::vector<Type*> ParamsType;
    LLVMContext& globalcontext = getGlobalContext();

   
   

   
    bool runOnFunction(Function &F) override 
    {
	flag++;
	std::string str1=F.getName();
	bool PrevIsInt=false;
	bool IsInMain=false;
	
	++HelloCounter;
	errs() << "\n|---------------Analysis Of IR code--------------------------|\n";
	errs() << flag;
	errs() << ')';
	errs() << "Function: ";
	errs().write_escaped(F.getName()) << '\n';
      
	
        errs() << "\nIR Code of Function: \n";
	
	  
	// F is a pointer to a Function instance
	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
	  errs() << *I << "\n";

	for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
	      Function* calledFunction = NULL;
	      Instruction* currentInst = &*I;
	      Instruction* prevInst;
	      
	   
	      IRBuilder<> builder(currentInst);
	      
	        Module *FC = F.getParent();
		
		if(str1.compare("main")==0 && IsInMain==false)
		 {
		  std::vector<Value*> Params;
		  std::vector<Type*> ParamsType;
		  std::vector<Type*> parameters;
		  std::vector<Type*> parameters1;
			errs() << "\n|--------------------Transform part-------------------------|\n";
			
			
			/*FILE * DECLERATION*/
	
			MYFILE = StructType::create(F.getContext(), "struct._IO_FILE");
			FILEPTR=PointerType::getUnqual(MYFILE);
	
			marker = StructType::create(F.getContext(), "struct._IO_marker");
			marker_ptr = PointerType::getUnqual(marker);
			
			//push back for parameters
			parameters.push_back(marker_ptr);
			parameters.push_back(FILEPTR);
			MYFILE->setBody(parameters, false);

			//push back for parameters
			parameters1.push_back(marker_ptr);
			parameters1.push_back(FILEPTR);
			parameters1.push_back(Type::getInt32Ty(FC->getContext()));;
			marker->setBody(parameters1, false);
			
			
			  GVstderr = new GlobalVariable(*FC,FILEPTR,false,GlobalValue::ExternalLinkage, NULL, "logfile");
			  errs()<<"\ncreate a FILE variable : ("<<*GVstderr;
			  errs()<<")\n";
			/*END OF DECLERATION*/
			 
			  
			/*INSERT FOPEN FUNCTION*/
			
			Constant *cfo = FC->getOrInsertFunction("fopen", FILEPTR
								    , Type::getInt8PtrTy(F.getContext())
								  , Type::getInt8PtrTy(F.getContext()),NULL);
		   
			  if(!cfo)
			  {
			      errs() << "\nfopen FAIL\n";
			      exit(1);
			  }
			  else
			  {
			      errs() << "\nadd a fopen call !\n"; 
			  }
		    
			  Function *ffopen = dyn_cast<Function>(cfo);
		
			  Value *filename = builder.CreateGlobalStringPtr("log.txt");
			  Value *mode = builder.CreateGlobalStringPtr("a+");
			  
			  std::vector<Value*> args;
			  args.push_back(filename);
			  args.push_back(mode);
			  
			  
			 GVstderr=builder.CreateCall(ffopen,args,"fopen");
	 
			  IsInMain=true;
			  errs() << "\n|---------------------------------------------|\n";
			  
			  /*END OF FOPEN*/
	          }
	       
		 //put fprintf in the next inst of print (if we find a print inst)
		 if(PrevIsInt==true)
		   {
		    /*INSERT A FPRINTF FUNCTION*/
		   
		   Module *FPR=F.getParent();
		   
		 //collect arguments of print
		    std::vector<Value *> argsforfprintf;
		    argsforfprintf.push_back(GVstderr);
		   for (int i = 1, e = ParamsType.size(); i != e; i++) {
		    argsforfprintf.push_back(Params[i]);
		   }
		   
		   std::vector<Type*> argsTypes;

		 
		  argsTypes.push_back(FILEPTR);
		//collect typeargs of print
		  for (unsigned i = 1, e = argsforfprintf.size(); i != e; i++) {
		      argsTypes.push_back(argsforfprintf[i]->getType());
		}
		 
		   Constant *fpr = FPR->getOrInsertFunction("fprintf",FunctionType::get(Type::getInt32Ty(F.getContext()),
                                                        argsTypes,
                                                        true)
							    );
		   
		 if(!fpr)
		    {
		      errs() << "fprintf FAIL\n";
		      exit(1);
		    }
		    else
		    {
		      errs() << "add a fprintf inst !\n"; 
		    }
		    
		    Function *fprintfC = cast<Function>(fpr);
		   
 		    builder.CreateCall(fprintfC,argsforfprintf,"fprintf");
		    
		    
		   
		    errs()<< "call fprintf inst :"<<*fprintfC;
		
		    PrevIsInt=false;
		    
	    
		   
		  /*END OF FPRINTF*/

		}
		//fclose if we find return inst of main
		    if (dyn_cast < ReturnInst > (currentInst)) 
		    {
		      
		      BasicBlock &B=F.getEntryBlock();
		      Instruction *inst = B.getTerminator();
		      
		      
		      
		      if(IsInMain==true){
			errs() << "\n|---------------------------------------------|\n";
			errs() << "\nfound an return inst :("<< *inst<<")";
			Value *cfc =FC->getOrInsertFunction("fclose",
							    Type::getInt32Ty(B.getContext()),
                                                        FILEPTR,
                                                        NULL);
			if(!cfc)
			  {
			      errs() << "\fclose FAIL\n";
			      exit(1);
			  }
			  else
			  {
			      errs() << "\nadd a fclose call!"; 
			  }
			
			Function *ffclose = dyn_cast<Function>(cfc);
			
			

			builder.CreateCall(ffclose,GVstderr,"fclose");
			errs() << "\nWe call fclose!"; 
			
		      
			errs() << "\n|---------------------------------------------|\n";
		      }
		    }

	  

	  if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) 
	  {
		errs()<<"NameOfCalledFunctionIs=	[";
		calledFunction = callInst->getCalledFunction();
		name=calledFunction->getName().str();

		errs()<<name;
		errs()<<"]\n";
		
		
		//check for print inst
		if(name.compare("printf")==0) {
		  
		
		errs()<<"Found a printf inst !\n";
		CallSite w(callInst);
		Params.push_back(GVstderr);
		errs()<<"with parameters:\n";
		for(User::op_iterator BEG = w->op_begin(), END = w->op_end (); BEG != END ; ++BEG)
		{
			    Params.push_back(BEG->get());
			    ParamsType.push_back(BEG->get()->getType());
                            errs()<< BEG->get() << "\n";
		}
		errs()<<"\n";
		
		  PrevIsInt=true;
		}
	  } 

	  prevInst = &*I;
	}
	return false;
      }

  };
}

char MyPass::ID = 0;
static RegisterPass<MyPass> X("MyPass", "This Is My Pass",true,true);