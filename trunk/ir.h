#if !defined(IR_H)
#define IR_H
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "symtable.h"
#include <cstring>
#include <cassert>
#include <string>

extern FILE* yyout;
//Complete:
// Handling of Smost Binops
// PrintInt and PrintFloat

// TODO: Handling of NIL, and printstring
// Concat, and EXP

// Sometimes you need to pass in the type of argument
enum argtype {ARG_REG, ARG_IMM };
enum llvmtype {ARG_FLOAT, ARG_INT, ARG_STRING, ARG_BOOL, ARG_VOID, ARG_DOUBLE, ARG_TABLE };
enum cstylefunction {CNULL,CPRINTF,CSTRCAT,CPOW };

// Binops, need to add float stuff, maybe in a different binop struct?
enum ops {OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_CONCAT, OP_LT, OP_GT, OP_LE, OP_GE, OP_EQ, OP_NE, OP_AND, OP_OR, OP_XOR, OP_SHR, OP_SHL };


llvmtype convert(SymbolInfo::llvm_type t);
struct IRConstant;



struct BasicBlock;
class IRTree;
struct Instruction;
class IROptimizer;

// TODO: do exponent

// (type: llvm instruction), (operands a-d), (datatype: dtype)
struct Instruction {
    bool visit;
    std::vector<Instruction*> defininginstructions;
    std::vector<int> sourceregisters;

    // Use this to setup the static constants needed later
    virtual bool isjump(){return false;}
    virtual bool isbinop(){return false;}
    virtual int assigned_register(){return -1;}
    virtual bool assigns_register(){
        return false;
    }
    virtual bool iscritical(){return false;} //critical functions are conditional branches and stores
bool beenVisited(){
        return visit;
	}

     void visitInstruction(){
        visit=true;
        for(std::vector<Instruction*>::iterator mydef=(defininginstructions).begin(); mydef!=(defininginstructions).end(); mydef++){
                            if(*mydef!=NULL){
                                (*mydef)->visitInstruction();
                            }
        }
    }
 virtual void establishSources(){
   // sourceregisters.push_back();
    return;
  }

    void addDefiningInstruction(Instruction* mydefinst){
        defininginstructions.push_back(mydefinst);
    }

    virtual bool isstore(){return false;}
    virtual bool isload(){return false;}

    virtual bool isalloc(){return false;}
    virtual bool isfunccall(){return false;}

    virtual bool isgetpointer(){return false;}

    virtual int getintorfloat(){return -1;}

    // Constant folding specific
    // This will help us figure out what to extract out the result as, should be overridden only by binops
    virtual llvmtype binop_type(){return ARG_VOID;}
    virtual bool arebothimm(){return false;}


    virtual void replaceregwithimm(IRConstant & theconstant){
        printf("This virtual fxn should never be called, Instruction::replaceregwithimm, aborting");
        abort();
    }



	// Must be defined later
	virtual void emit(){
	};



	Instruction(){
       visit=false;
	};
	~Instruction(){
	};
};



//Just to keep some common stuff together

template <class T>
struct Binop : Instruction{
    T arg1;
    T arg2;

    ops binoptype;
    int result;
    int isarg1reg;
    int isarg2reg;
    int intorfloat;   //1 is int, 2 is float, 3 is bool
    int getintorfloat(){return intorfloat;}
    bool isbinop(){return true;}

    virtual bool assigns_register(){
        return true;
    }


    /*NOTES
    //alloc, load, getpointer, callfunction
            else if(itr->isalloc()){
                return;
            }

            else if(itr->isgetpointer()){
                return; //see later what to do w global?

            }
            else if(itr->iscallfunction()){
                return; //see later what to do w passed in vars?
            }
            else{
                return;
            }
            ENDNOTES*/
      void establishSources(){
          if(isarg1reg==ARG_REG){
   sourceregisters.push_back(arg1);
          }
          if(isarg2reg==ARG_REG){

   sourceregisters.push_back(arg2);
          }
    return;
  }

    bool arebothimm(){
        if ((isarg1reg==ARG_IMM) && (isarg2reg==ARG_IMM)){
            return true;}
        return false;
    }

    Binop(){
    };
    ~Binop(){
    };
};

//Need to add something similar for 1 register and immediate, and 2 immediate values
struct BinopInt : Binop<int>{
    //<result> = add <ty> <op1>, <op2>
    //<result> = add i32 4, %var

    void emit();
    void replaceregwithimm(IRConstant & theconstant);
    llvmtype binop_type(){return ARG_INT;}

    //Takes in register to store, the enum for the type and the two registers to operate on
    //arg1type is ARG_REG or ARG_IMM
    //Assumes contents are signed int 32-bit type
    BinopInt(int theresult, ops optype, int a, int b,int arg1type,int arg2type){
        arg1=a;
        arg2=b;
        binoptype=optype;
        result=theresult;
        isarg1reg=arg1type;
        isarg2reg=arg2type;
        intorfloat=1;

    };
    ~BinopInt(){
    };

    int computeresult(llvmtype & returntype);

        int assigned_register(){
        return result;
    }
};

struct BinopFloat : Binop<float>{
    //<result> = add <ty> <op1>, <op2>
    //<result> = add i32 4, %var

    void emit();
    llvmtype binop_type(){return ARG_FLOAT;}

    //Takes in register to store, the enum for the type and the two registers to operate on
    //arg1type is ARG_REG or ARG_IMM
    //Assumes contents are signed int 32-bit type
    BinopFloat(int theresult, ops optype, float a, float b,int arg1type,int arg2type){
        //Will be cast to int if arguments are registers later
        arg1=a;
        arg2=b;

        binoptype=optype;
        result=theresult;
        isarg1reg=arg1type;
        isarg2reg=arg2type;
        intorfloat=2;
    };
    ~BinopFloat(){
    };
    float computeresult(llvmtype & returntype);

    void replaceregwithimm(IRConstant & theconstant);

    int assigned_register(){
        return result;
    }
};

struct BinopBool : Binop<int>{
    //<result> = add <ty> <op1>, <op2>
    //<result> = add i32 4, %var

    void emit();
    llvmtype binop_type(){return ARG_BOOL;}

    //Takes in register to store, the enum for the type and the two registers to operate on
    //arg1type is ARG_REG or ARG_IMM
    //Assumes contents are signed int 32-bit type
    BinopBool(int theresult, ops optype, int a, int b,int arg1type,int arg2type){
        //Will be cast to int if arguments are registers later
        arg1=a;
        arg2=b;

        binoptype=optype;
        result=theresult;
        isarg1reg=arg1type;
        isarg2reg=arg2type;
        intorfloat=3;
    };
    ~BinopBool(){
    };
    void replaceregwithimm(IRConstant & theconstant);

    bool computeresult();

    int assigned_register(){
        return result;
    }
};

template <class T>
struct Printop : Instruction{
    int firstresult;
    T arg;
    argtype isargreg;

    // Emits a printf with just a newline, needs to know which string constant to use
    Printop(){
    };
    ~Printop(){
    };

    static void emitnewline(int a){
        //%0 = tail call i32 @putchar(i32 10) nounwind		; <i32> [#uses=0]
        fprintf(yyout,"%%r%d = tail call i32 @putchar(i32 10) nounwind \n",a);
    }
};

struct PrintInt : Printop<int>{
    //Use this to determine which registers to allocate
    static const int requiresthismanyregs=1;
    //Call this for each at the beginning of code emission for every type of string constant

    static int whichstringconstant;

    static void emitstringconstant(int a){
        //char stringconstant[8]="%d\t";
        //
        //Looks like
        //@.str = internal constant [4 x i8] c"%d\09\00"
        fprintf(yyout,"@.str%d = internal constant [4 x i8] c\042%%d\\09\\00\042 \n",a);
        whichstringconstant=a;
    }

    void emit();

    PrintInt(int a, int b, argtype c){
        firstresult=a;
        arg=b;
        isargreg=c;
    };
    ~PrintInt(){
    };
};



struct PrintFloat : Printop<float>{
    static const int requiresthismanyregs=2;

    //We can keep track of which to use, MUST be initialized ~= 0
    static int whichstringconstant;

    //Call this at the beginning of code emission for every type of string constant
    static void emitstringconstant(int a){
        //char stringconstant[8]="%.14g\t%d\t%d\n";
        //
        //Looks like
        //@.str# = internal constant [8 x i8] c"%.14g\09\00"
        fprintf(yyout,"@.str%d = internal constant [4 x i8] c\042%%.14g\\09\\00\042 \n",a);

        //For later use
        whichstringconstant=a;
    }

    void emit();

    PrintFloat(int a, float b, argtype c) {
        firstresult=a;
        arg=b;
        isargreg=c;
    };
    ~PrintFloat(){
    };
};

struct MakeLabel : Instruction{
    int branchnum;
    void emit(){
        //bb#:
        fprintf(yyout,"bb%d: \n",branchnum);
    }


    MakeLabel(int a) : branchnum(a) {}
    ~MakeLabel(){}
};

struct CondJump : Instruction{
    //br i1 <cond>, label <iftrue>, label <iffalse>
    //br label <dest>          ; Unconditional branch


    bool isconditional;
    int condreg;
    int iftrue;
    int iffalse;

    void replaceregwithimm(IRConstant & theconstant);

	    void establishSources(){
          if(isconditional){
   sourceregisters.push_back(condreg);
          }


  }
   bool iscritical(){
            return isconditional;
    }
    void emit(){
        switch (isconditional){
            case false:
                fprintf(yyout,"br label %%bb%d \n",iftrue);
                break;
            case true:
                fprintf(yyout,"br i1 %%r%d, label %%bb%d, label %%bb%d \n",condreg,iftrue,iffalse);
                break;
        }

    }

    bool isjump(){
        return true;
    }



    int returnTrueDestination(){
        return iftrue;
    }
    int returnFalseDestination(){
        return iffalse;
    }

    // Takes in a register to check, label to go to if true, label to go to if false
    CondJump(int a,int b,int c) : isconditional(true), condreg(a), iftrue(b), iffalse(c) {}

    CondJump(int a) : isconditional(false), iftrue(a) {}

    ~CondJump(){
    }

};

struct DefineFunction : Instruction {
    char intstring[8];//="i32";
    char floatstring[8];//="float";
    char stringstring[8];//="i8 *";
    // How to handle nils?

    std::string funcname;
    llvmtype returntype;
    std::vector<llvmtype> * argtypes;
    char returntypestring[8];

    void emitpre();

    void emitpost(int returnregister){
    //return:		; preds = %entry
	//%retval1 = load i32* %retval		; <i32> [#uses=1]
	//ret i32 %retval1

        fprintf(yyout,"return: \n");
        fprintf(yyout,"ret %s %%r%d \n",returntypestring,returnregister);

    }

    //llvmtype is an enum defined up top
    DefineFunction(std::string a, llvmtype b, std::vector<llvmtype> * c){
        funcname=a;
        returntype=b;
        argtypes=c;

        strcpy(intstring,"i32");
        strcpy(floatstring,"float");
        strcpy(stringstring,"i8 *");

        switch (returntype){
            case ARG_INT:
                strcpy(returntypestring,intstring);
            break;
            case ARG_FLOAT:
                strcpy(returntypestring,floatstring);
            break;
            case ARG_STRING:
                strcpy(returntypestring,stringstring);
            break;
			default: abort();
        }

    }
    ~DefineFunction(){
    }

};

struct Alloc : Instruction {
    int theptr;
    llvmtype thetype;
    int length;
    bool isarray;
    bool isalloc(){return true;}

    void emit();




    Alloc(int a, llvmtype b){
        theptr=a;
        thetype=b;
        isarray=false;
    }


    virtual bool assigns_register(){
        return true;
    }
     int assigned_register(){
        return theptr;
    }

    //Needed for strings and arrays
    Alloc(int a, llvmtype b, int c){
        theptr=a;
        isarray=true;
        thetype=b;
        length=c;
    }


    ~Alloc(){
    }
};

struct Loadop : Instruction {
    //%val = load i32* %ptr
    int resultreg;
    llvmtype thetype;
    int theptr;
    bool isptrglobal;
    bool isload(){return true;}
    void emit();
        void establishSources(){
          if(!isptrglobal){
   sourceregisters.push_back(theptr);
          }

    return;
  }

    virtual bool assigns_register(){
        return true;
    }
     int assigned_register(){
        return resultreg;
    }
    Loadop(int a, llvmtype b, int c, bool d=false) : resultreg(a), thetype(b), theptr(c), isptrglobal(d) {}
    ~Loadop(){}
};

struct Storeop : Instruction {
    //store i32 3, i32* %ptr                          ; yields {void}
    union immediate{
        float f;
        int i;
        bool b;
        int reg;
    };

    immediate arg;

    argtype argkind;
    llvmtype thetype;
    int theptr;
    bool isptrglobal;

	void establishSources(){
	           if(argkind==ARG_REG){
                sourceregisters.push_back(arg.reg);
	           }

          if(!isptrglobal){
                sourceregisters.push_back(theptr);
          }


  }
    void emit();

    bool isstore() {return true;}
    bool iscritical(){return true;}
    void replaceregwithimm(IRConstant & theconstant);

    Storeop(float a, int d, bool e=false) : argkind(ARG_IMM), thetype(ARG_FLOAT), theptr(d), isptrglobal(e) {
    arg.f=a;
    }

    Storeop(int a, argtype b, llvmtype c, int d, bool e=false) : theptr(d), isptrglobal(e)
    {
        argkind=b;

        if (argkind == ARG_IMM){
            arg.i = a;
        }
        else {
            arg.reg = a;
        }

        thetype=c;

        if ((argkind == ARG_IMM) && (thetype!=ARG_INT)){
            printf("type error on storeop \n");
            abort();
        }

    }

    Storeop(bool a, int d, bool e=false) : argkind(ARG_IMM), thetype(ARG_BOOL), theptr(d), isptrglobal(e) {
    arg.b = a;
    }


    ~Storeop(){}
};

//takes a global address and loads it into a local register;
struct GetPointer : Instruction {
    //@ptr# to register
    //%r1 = getelementptr i32* %MyVar, i64 0

    int outputregister;
    llvmtype thetype;
    int globalptr;
    int offset;
    virtual bool isgetpointer(){return true;}
    void emit();
    virtual bool assigns_register(){
        return true;
    }
    int assigned_register(){
        return outputregister;
    }
    GetPointer(int a, llvmtype b, int c, int d=0) : outputregister(a), thetype(b), globalptr(c), offset(d) {}
};


struct CallFunction : Instruction {
    char intstring[16];//="i32";
    char floatstring[16];//="float";
    char stringstring[16];//="i8 *";
    char boolstring[16];
    char llvmstringstring[16];
    bool cstyle;
    cstylefunction thecfunction;

    // How to handle nils?

    std::string funcname;
    llvmtype returntype;
    std::vector<llvmtype> * argtypes;
    std::vector<int> * argregs;
    int outregister;
    char returntypestring[32];



    void establishSources(){
        for(std::vector<int>::iterator myitr=argregs->begin();myitr!=argregs->end(); myitr++){
                     sourceregisters.push_back(*myitr);
        }

    }




    bool iscritical(){return true;}


    virtual bool assigns_register(){
        if(returntype!=ARG_VOID){
            return true;
        }
        else{
            return false;
        }
    }
    int assigned_register(){
        return outregister;
    }




    virtual bool iscallfunc(){return true;}


    void emit();
	bool isfunccall(){return true;}

    CallFunction(int a, std::string b, llvmtype c, std::vector<llvmtype> * d, std::vector<int> * e, bool f=false, cstylefunction g=CNULL){
        //%5 = call i32 @factorial(i32 %4) nounwind
        outregister=a;
        funcname=b;
        returntype=c;
        cstyle=f;
		argtypes = new std::vector<llvmtype>(*d);
		argregs = new std::vector<int>(*e);
		returntypestring[0] = '\0';
		thecfunction=g;
    }
    ~CallFunction(){
		delete argtypes; delete argregs;
    }
	bool searchforlocalreg(int theregister);
};

struct ReturnFromFunc : Instruction{
    llvmtype thetype;
    int argreg;
    argtype regorimm;

    void establishSources(){
        if(regorimm==ARG_REG){
                     sourceregisters.push_back(argreg);
        }

    }




    bool iscritical(){return true;}

    void emit(){
        char argprefix[4]="";
        if (regorimm==ARG_REG){
            strcpy(argprefix,"%r");
        }
		/*
        else if (regorimm!=ARG_IMM){
            printf("must specify ARG_REG or ARG_IMM \n");
            abort();
        }
		*/

        switch (thetype){

            case ARG_INT:
                fprintf(yyout, "ret i32 %s%d \n",argprefix,argreg);
				break;
            case ARG_BOOL:
                fprintf(yyout, "ret i1 %s%d \n",argprefix,argreg);
				break;
            case ARG_FLOAT:
                fprintf(yyout, "ret float %s%d \n",argprefix,argreg);
				break;
            case ARG_STRING:
                fprintf(yyout, "ret [256 x i8] %s%d \n",argprefix,argreg);
				break;
            case ARG_VOID:
                fprintf(yyout, "ret void \n");
				break;
            default:
                printf("something bad happened in return \n");
                abort();
				break;
        }

    }

    ReturnFromFunc(llvmtype a, int b, argtype c) :  thetype(a), argreg(b){
        regorimm=c;

        if ((regorimm!=ARG_IMM) && (regorimm!= ARG_REG)){
            printf("must specify ARG_REG or ARG_IMM \n");
            abort();
        }
    }

    // Just for voids
    ReturnFromFunc(llvmtype a){
        if (a==ARG_VOID){
            thetype=ARG_VOID;
        }
        else {
            printf("Must specify a register if non-void \n");
            abort();
        }
    }
    ~ReturnFromFunc();
};

// class Fpext
// convert single precision fp to double fp value
class Fpext : public Instruction
{
	int src, dest;
public:
	Fpext(int dest_reg, int src_reg);
	void emit();
    virtual int assigned_register(){return dest;}
    virtual bool assigns_register(){
        return true;
    }


 virtual void establishSources(){
    sourceregisters.push_back(src);
    return;
  }

};

// class Sitofp
// convert signed integer to single precision floating point
class Sitofp : public Instruction
{
	int src, dest;
public:
	Sitofp(int dest_reg, int src_reg);
	void emit();
	virtual int assigned_register(){return dest;}
    virtual bool assigns_register(){
        return true;
    }


 virtual void establishSources(){
    sourceregisters.push_back(src);
    return;
  }
};

// class Fptosi
// convert single precision floating point to signed integer
class Fptosi : public Instruction
{
	int src, dest;
public:
	Fptosi(int dest_reg, int src_reg);
	void emit();
	virtual int assigned_register(){return dest;}
    virtual bool assigns_register(){
        return true;
    }


 virtual void establishSources(){
    sourceregisters.push_back(src);
    return;
  }
};


// Not Correct, can't do immediates like that
//struct Assign : Instruction{
//    int outputregister;
//    llvmtype thetype;
//    int intvalue;
//    float floatvalue;
//    bool boolvalue;
//
//
//    void emit(){
//        //%ptr = alloca i32, i32 4
//        char intstring[8]="i32";
//        char floatstring[8]="float";
//        char boolstring[8]="i1";
//
//        switch (thetype){
//            case ARG_INT:
//                fprintf(yyout,"%%r%d = %s %d \n",outputregister,intstring, intvalue);
//                break;
//            case ARG_FLOAT:
//                fprintf(yyout,"%%r%d = %s %f \n",outputregister,floatstring, floatvalue);
//                break;
//            case ARG_BOOL:
//                fprintf(yyout,"%%r%d = %s %d \n",outputregister,boolstring, boolvalue);
//                break;
//            default:
//                abort();
//        }
//    }
//
//
//    Assign(int a, llvmtype b, int c) : outputregister(a), thetype(b), intvalue(c) {}
//    Assign(int a, llvmtype b, float c) : outputregister(a), thetype(b), floatvalue(c) {}
//    Assign(int a, llvmtype b, bool c) : outputregister(a), thetype(b), boolvalue(c) {}
//
//
//
//
//    ~Assign(){
//    }
//};


//
//
//      http://www.lua.org/source/5.1/print.c.html
/*
static void PrintString(const Proto* f, int n)
{
 const char* s=svalue(&f->k[n]);
 putchar('"');
 for (; *s; s++)
 {
  switch (*s)
  {
   case '"': fprintf(yyout,"\\\""); break;
   case '\a': fprintf(yyout,"\\a"); break;
   case '\b': fprintf(yyout,"\\b"); break;
   case '\f': fprintf(yyout,"\\f"); break;
   case '\n': fprintf(yyout,"\\n"); break;
   case '\r': fprintf(yyout,"\\r"); break;
   case '\t': fprintf(yyout,"\\t"); break;
   case '\v': fprintf(yyout,"\\v"); break;
   default:     if (isprint((unsigned char)*s))
                        fprintf(yyout,"%c",*s);
                else
                        fprintf(yyout,"\\%03u",(unsigned char)*s);
  }
 }
 putchar('"');
}

static void PrintConstant(const Proto* f, int i)
{
 const TValue* o=&f->k[i];
 switch (ttype(o))
 {
  case LUA_TNIL:
        fprintf(yyout,"nil");
        break;
  case LUA_TBOOLEAN:
        fprintf(yyout,bvalue(o) ? "true" : "false");
        break;
  case LUA_TNUMBER:
        fprintf(yyout,LUA_NUMBER_FMT,nvalue(o));
        break;
  case LUA_TSTRING:
        PrintString(f,i);
        break;
  default:                              //cannot happen
        fprintf(yyout,"? type=%d",ttype(o));
        break;
 }
}
*/

std::string itos(int i);

// register naming convetion r(#number)

struct Arg {
	SymbolInfo::llvm_type type;
	std::string name;
	int reg;
	Arg(const std::string& nm, SymbolInfo::llvm_type t, int r) : type(t), name(nm),  reg(r){}
	Arg(const std::string& nm, SymbolInfo::llvm_type t) : type(t), name(nm) {}
	void print();
};

struct BasicBlock; //necessary for next line
class bblist: public std::vector<BasicBlock*>{};

struct BasicBlock : public std::vector<Instruction*> {
	std::string label;  // name of the label
	std::vector<BasicBlock*> source; //list of Source Basic Blocks in the CFG
	std::vector<BasicBlock*> destination; //list of Destination Basic Blocks in the CFG



public:
	void set_label(const std::string& ref ) { label = ref; }
	std::string& get_label() { return label; }
	BasicBlock(std::string nm ) : label(nm) {}
	~BasicBlock() {
		for (iterator itr = begin(); itr != end(); itr++) {
			delete *itr;
		}
	}
	void print();

    void addSourceBasicBlock(BasicBlock* newsource){
        source.push_back(newsource);
    }
    void addDestinationBasicBlock(BasicBlock* newdestination){
        destination.push_back(newdestination);
    }
    int getBlockID(){    //Helper Get Function for whichoneami
        return whichoneami;
    }
    void setBlockID(int whois){ //Helper Set Function for whichoneami
        whichoneami = whois;
    }

	int whichoneami;   //BlockID with a weird name

    bblist bbsfrom;
    bblist bbsto;

};






// stores global string constant information (naming scheme will be string0, string1, etc.)
class Global {
	std::string name;
	int size;
	std::string str;
	union {
		int i32;
		float fp;
		bool bl;
	} content;
	SymbolInfo::llvm_type t;
public:
	Global(const std::string& cont, const std::string& nm, int sz = -1) : name(nm), t(SymbolInfo::LL_STRING) {
		if (sz == -1) size = cont.size();
		else size = sz;
		str = cont;
	}
	Global(const int i, const std::string& nm) : name(nm), t(SymbolInfo::LL_INTEGER) {
		content.i32 = i;
	}
	Global(float f, const std::string& nm) : name(nm), t(SymbolInfo::LL_FLOAT) {
		content.fp = f;
	}
	Global(bool b, const std::string& nm) :  name(nm), t(SymbolInfo::LL_BOOL) {
		content.bl = b;
	}
	void print();
};

// Helps us do constant propogation
struct IRConstant{
    int theregister;
    Storeop::immediate thevalue;
    llvmtype thetype;

    IRConstant(int a, Storeop::immediate b,llvmtype c):theregister(a), thevalue(b),thetype(c){}
};



class IRTree;
struct IRFunction {
private:
	typedef std::vector<Arg*> ArgList;
	typedef std::vector<BasicBlock*> BlockList;

	std::string name;

	SymbolInfo::llvm_type rettype;
public:
    BlockList blocks;
    ArgList args;
	IRFunction(const std::string& nm) : name(nm) {}
	IRFunction(const std::string& nm, SymbolInfo::llvm_type ret) : name(nm), rettype(ret) {}
	~IRFunction() {
		for (ArgList::iterator itr = args.begin(); itr != args.end(); itr++) {
			delete *itr;
		}
		for (BlockList::iterator itr = blocks.begin(); itr != blocks.end(); itr++) {
			delete *itr;
		}
	}
	void add_arg(Arg* farg) { args.push_back(farg); }
	void add_blk(BasicBlock* blk) { blocks.push_back(blk); }
	void print();
	void print_decl();

};

class IROptimizer
{
    typedef std::vector<BasicBlock*> BlockList;
    typedef std::vector<Instruction*> Inslist;
    typedef std::vector<Arg*> ArgList;

    typedef std::pair<int, Instruction*> RegDefPair;

    std::vector<RegDefPair> reg_dir;

    IRTree& tree;
    SymbolTable& table;

    void replaceregwithconstant(IRFunction* irfunc, IRConstant& theconstant);

    //Used for Post-IRbuilder generated registers
    //Will be negative, and Instructions that use this must check for negative value and emit properly
    //So far, only Alloc, Load, and Store are meant to check check.
    int IROptimizergeneratedregister;

    public:
        int getregister(){return (--IROptimizergeneratedregister);}

        IROptimizer(IRTree& tr, SymbolTable& tab) : tree(tr), table(tab)
        {
            IROptimizergeneratedregister=0;
        }

        BasicBlock* getBlockPointer(IRFunction* irfunc, int blockID){   //given a BlockID, returns a pointer to the given Basic Block....if not found, return NULL!!!
            for(BlockList::iterator bb=irfunc->blocks.begin(); bb!=irfunc->blocks.end(); bb++){
                //Iterate through all the blocks looking for the given ID
                if((*bb)->getBlockID()==blockID){
                //If we found it, return the pointer!
                    return *bb;
                }
            }
            return NULL;
        }

        bool constant_propagation(IRFunction* irfunc);

        bool constant_folding(IRFunction* irfunc);
        void replacebinop_bool(BlockList::iterator bb,int offset, bool value);
        void replacebinop_int(BlockList::iterator bb,int offset, int value);
        void replacebinop_float(BlockList::iterator bb,int offset, float value);
        void strengthReduceMul(BasicBlock* bb, BinopInt* instr, int argtoreduce);
        void strengthReduceDiv(BasicBlock* bb, BinopInt* instr);
        void strengthReduceMod(BasicBlock* bb, BinopInt* instr);

        bool regiswithinfunccall(IRFunction * irfunc, int theregister);

        void create_cfg(IRFunction* irfunc){

            for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
			//do something
                for (Inslist::iterator itr = (*bb)->begin(); itr != (*bb)->end(); itr++){
                //to help our syntax
                    Instruction &instr = **itr;
                   // int currentbb = (*bb)->getBlockID();   //each bb has a number, this tells me what number we work with

                    // how do we know what type of instruction it is?  implement some sort of reflection
                    // if it's a branch or a jump.  Implemented a virtual function Instruction::isjump() to tell us.
                    if (instr.isjump()){
                        //OK, so the instruciton is a jump!  So now we have to follow it.....find the destinations!
                        CondJump &thejump = (CondJump &)instr;

                        if (thejump.isconditional){
                            //If it is conditional, it has 2 destinations
                            int firstdest = thejump.returnTrueDestination();
                            int seconddest = thejump.returnFalseDestination();
                            BasicBlock* firstdest_pointer;
                            BasicBlock* seconddest_pointer;

                            firstdest_pointer = getBlockPointer(irfunc, firstdest);
                            seconddest_pointer = getBlockPointer(irfunc, seconddest);

                            //First Build Destination Array
                            (*bb)->addDestinationBasicBlock(firstdest_pointer);
                            (*bb)->addDestinationBasicBlock(seconddest_pointer);

                            //Now Build Source Array
                            firstdest_pointer->addSourceBasicBlock(*bb);
                            seconddest_pointer->addSourceBasicBlock(*bb);
                        }
                        else {
                            //If it is unconditional, it has 1 destination
                            int firstdest = thejump.returnTrueDestination();
                            BasicBlock* firstdest_pointer;

                            firstdest_pointer = getBlockPointer(irfunc, firstdest);

                            //First Build Destination Array
                            (*bb)->addDestinationBasicBlock(firstdest_pointer);

                            //Now Build Source Array
                            firstdest_pointer->addSourceBasicBlock(*bb);
                        }


                    }


                }




            }



        }
         void build_register_dir(IRFunction* irfunc){

             //Also Add the ARGVAR list!!!!!!!  They will add a NULL pointer as the defining; thus, have to modify all visit functions to return if they encounter null, else, recurse
             for(ArgList::iterator al = irfunc->args.begin(); al != irfunc->args.end(); al++){
                //Arg &myarg = *al;
                RegDefPair* mypairpointer = new std::pair<int,Instruction*>();
                mypairpointer->first=(*al)->reg;
                mypairpointer->second=NULL;
                reg_dir.push_back(*mypairpointer);
             }


            for(BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++){



                 Inslist::iterator start_itr = (*bb)->begin();
            for (int i=0; start_itr+i != (( (*bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            //printf("%d",i);
            start_itr = (*bb)->begin();
            Inslist::iterator itr = start_itr+i;




               // for(Inslist::iterator itr = (*bb)->begin(); itr != (*bb)->end(); itr++){

                    Instruction &instr = **itr;


                    //Looping through instructions
                    //Is Instruction one that ASSIGNS a value to a register?  If so, What is the REGISTER a value is assigned to?
                    if(instr.assigns_register()){
                        //If it is a funciton which may assign a register, get the register this particular object is asigning to, and add to vector
                        int myassignedregister = instr.assigned_register();
                        RegDefPair* mypairpointer = new std::pair<int,Instruction *>();
                        //(myassignedregister,*itr);
                        mypairpointer->first = myassignedregister;
                        mypairpointer->second = *itr;
                        reg_dir.push_back(*mypairpointer);

                    }

                }
            }
        }

        void init_dead_code_elimination(IRFunction* irfunc){
            //This function initializes dead code elimination by:
              //CLEAN REG-INSTRUCTION VECTOR
            //INITIALIZE VISITED
            //INITIALIZE SOURCE AND DEST VECTORS
            // erase(begin(),end())

             //visited=false

             //in pair vector, first free pair, then vector erase, dont free vector



             //This part does the clean reg-instruction vector
             //dont forget to free up data!
          //   for(unsigned int i=0; i<reg_dir.size();i++){

            //    RegDefPair* mypairpointer = &(reg_dir.operator[](i));
            //    delete mypairpointer;

                reg_dir.erase(reg_dir.begin(),reg_dir.end());

             //}

             //Initialize Visited and source and dest vectors
             for(unsigned int i=0; i<(irfunc->blocks).size(); i++){
                for(unsigned int j=0; j<((irfunc->blocks).operator[](i))->size(); j++){
                    Instruction &instr = *(((irfunc->blocks).operator[](i))->operator[](j));
                    instr.visit=false;
                    (instr.defininginstructions).erase((instr.defininginstructions).begin(),(instr.defininginstructions).end());
                   (instr.sourceregisters).erase((instr.sourceregisters).begin(),(instr.sourceregisters).end());                }
             }


        }

        void optimize_function(IRFunction* irfunc){

            int minnumberofopts = 100;  //Optimize how many times if changes keep getting detected
            bool constant_prop_change = true;
            bool constant_folding_change = true;
            bool dead_code_elimination_change = true;
            bool strength_reduction_change = true;


            int i=0; //loop counter
            while(i<minnumberofopts && (constant_prop_change || constant_folding_change || dead_code_elimination_change || strength_reduction_change)){

            //INITIALIZE DEAD_CODE_ELIM DATASTRUCTS

            init_dead_code_elimination(irfunc);

                       //In here simply call the different optimizations


            constant_prop_change = constant_propagation(irfunc);


            constant_folding_change = constant_folding(irfunc);




 //First create our data structs

            create_cfg(irfunc);   //adds the linkds to source and destination basic blocks according to jump instructions

            build_register_dir(irfunc);    //builds an array of pairs  register#,pointer to instruciton which defines register#   since SSA, theres only one definition per reg#....need a function to search for a pointer given a # from reg#, thats the purpose of this


            strength_reduction_change = strength_reduction(irfunc);
            dead_code_elimination_change = dead_code_elimination(irfunc);
            i++;

            }

        }


        bool strength_reduction(IRFunction* irfunc);

        bool dead_code_elimination(IRFunction* irfunc);


        bool deleteUnvisited(IRFunction* irfunc){
            bool deletedstuff=false;
            for(BlockList::iterator bb=irfunc->blocks.begin(); bb!=irfunc->blocks.end(); bb++){
                for(Inslist::iterator itr = (*bb)->begin(); itr != (( (*bb)->end() )); itr++){
                    Instruction &instr = **itr;

                    if((!instr.beenVisited()) && (instr.assigns_register())&&(!instr.isfunccall())){
                        (*bb)->erase(itr);
                        itr--;
                        deletedstuff=true;
                    }
                }
            }
            return deletedstuff;
        }

  Instruction* getInstructionDefiningReg(int reg){
            for(std::vector<RegDefPair>::iterator myiterator=reg_dir.begin(); myiterator!=reg_dir.end(); myiterator++){
                if((*myiterator).first == reg){
                    return (*myiterator).second;
                }
            }
            return NULL;
        }

        void setDefiningInstructions(IRFunction* irfunc){
            for(BlockList::iterator bb=irfunc->blocks.begin(); bb!=irfunc->blocks.end(); bb++){
                for(Inslist::iterator itr=(*bb)->begin(); itr !=  (*bb)->end() ; itr++){
                    Instruction &instr = **itr;
                        instr.establishSources();
                        for(std::vector<int>::iterator mysource=(instr.sourceregisters).begin(); mysource!=(instr.sourceregisters).end(); mysource++){
                            instr.addDefiningInstruction(getInstructionDefiningReg(*mysource));
                        }



                }
            }
        }

        void visitUD(Instruction* itr){
            if(itr!=NULL){
                itr->visitInstruction();
            }




        }










};

// IRTree: structure to hold all instructions
class IRTree
{
	typedef std::vector<IRFunction*> FuncList;
	typedef std::vector<Global*> ConstList;
	ConstList globals;
	FuncList functions;
public:
	void print_assembly();
	IRTree();
	~IRTree() {
		for (FuncList::iterator itr = functions.begin(); itr != functions.end(); itr++) {
			delete *itr;
		}
		for (ConstList::iterator itr = globals.begin(); itr != globals.end(); itr++) {
			delete *itr;
		}

	}
	void add_function(IRFunction* fx) { functions.push_back(fx); }
	void add_global(Global* g) { globals.push_back(g); }
    void optimize(IROptimizer& iroptimizer){
        for (unsigned int i = 0; i < functions.size(); i++)
        {
            iroptimizer.optimize_function(functions.operator[](i));  //Passes an IRFUnciton pointer as argument  //could have opt all (organize procedures or functions), opt basic block.....opt trace, etc   //each one can have types...opt function can have constant propagation local, global, folding, etc

        }
    }
};

struct fhist { IRFunction* fx; BasicBlock* bb; int offset; int lnum; int func_reg; };


// IRBuilder : helper class that will be passed through ast to generate code
class IRBuilder
{
	IRTree& tree;
	SymbolTable& table;

	int function_reg_count;
	int global_reg_count;
	int label_num;

	// current place in ir tree
	IRFunction* cur_func;
	BasicBlock* cur_bb;
	int bb_offset;

	std::vector<fhist> function_stack;
	void save_fx_status() {
		fhist x = {cur_func, cur_bb, bb_offset, label_num, function_reg_count};
		function_stack.push_back(x);
	}
	void restore_fx_status() {
		fhist& x = function_stack.back();
		cur_func = x.fx; cur_bb = x.bb; bb_offset = x.offset;
		function_reg_count = x.func_reg;
		label_num = x.lnum;
		function_stack.pop_back();
	}
public:
	IRBuilder(IRTree& tr, SymbolTable& tab) : tree(tr), table(tab)
	{
		reset_local_reg();
		reset_global_reg();
		reset_label_num();
		IRFunction* fx = new IRFunction("main", SymbolInfo::LL_INTEGER);
		add_function(fx);
		bb_offset = 0;
	}

	// returns integer to name registers in functions (eg %0, %1, %2)
    // * count is reset after every function definition
	int get_local_register() { return function_reg_count++; }

	// resets the generator for local register names
	void reset_local_reg() 	 { function_reg_count = 0; }

	// returns numbers to name constant registers (eg %string0, %string1, etc.)
	// * count is not reset after each
	int get_global_reg() 	 { return global_reg_count++; }
	void reset_global_reg()	 { global_reg_count = 0; }

	// returns numbers to name labels for blocks (eg %label0, %label2, etc)
	int get_label_num() 	 { return label_num++; }
	void reset_label_num() 	 { label_num = 0; }

 	// stores IRFunction you have created in the tree and
	// makes certain that all instruction/basic block additions go into that function
	void add_function(IRFunction* fx) {
		save_fx_status();
		tree.add_function(fx);
		reset_local_reg();
		reset_label_num();
		cur_func = fx;
		create_bb();
	}

	void add_global(SymbolInfo::llvm_type t, int nm);
	void add_global_node(Global* node);
	// adds instruction into previously created basic block
	void add_ins(Instruction* ins) { cur_bb->push_back(ins); }

	// stops code from being written into a function
	void exit_function() {
		restore_fx_status();
	}

	// inserts basic block into a tree and ensures that next instruction additions will
 	// go into that new block ( returns the number of label that was assigned)
	int create_bb(int lab = -1) {
		lab = lab == -1 ? get_label_num(): lab;
		std::string label("bb");
		label.append(itos(lab));
		cur_bb = new BasicBlock(label);
		cur_bb->setBlockID(lab);
		cur_func->add_blk(cur_bb);
		return lab;
	}
	int get_six_flags() { return 0; }
	SymbolTable* get_global_scope() { return &table; }
};

void add_nulls(std::string& ref, int num);








#endif

