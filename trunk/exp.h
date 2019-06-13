#ifndef EXP_H
#define EXP_H

#include "ast.h"
#include "string.h"

// Expression Classes 
struct NilExp : public Exp {
	NilExp(int ln) : Exp(ln) { set_constant(); } 
	void print_children() { fprintf(yyout, "(Nil %d)", get_line()); }  
	void print() { print_group("NilExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};



class BoolExp : public Exp {
	bool val;
public:
	BoolExp(bool vl, int ln) : Exp(ln), val(vl) { set_constant(); } 
	//void print() { if (val) print_group("True"); else print_group("False") } 
	void print_children() { fprintf(yyout, "(Bool %d %s)", get_line(), val ? "true" : "false"); } 
	void print() { print_group("BoolExp"); }
    SymbolInfo::llvm_type infer_type(TableBuilder& builder) {
		set_type(SymbolInfo::LL_BOOL); return get_type(); 
	}
    bool getBoolVal(){ return val; }
    int gen_exp_code(IRBuilder& builder);
};



class NumberExp : public Exp {
	float floatnum;
	int   intnum;
public:
	enum number_type {NUM_INT, NUM_FLOAT};
 NumberExp(int intval, float floatval, int ln, number_type tp) : Exp(ln), floatnum(floatval), intnum(intval), type(tp) { set_constant(); } 
	//void print() { print_group("Number"); }
	void print_children() { 
	  if(type==0){ // is int
	    fprintf(yyout, "(Number %d %d)", get_line(), intnum);
	  }
	  else{ //is float
	    fprintf(yyout, "(Number %d %.2f)", get_line(), floatnum);
	  }

	}
	void print() { print_group("NumberExp"); }
	number_type getNumberType(){return type;}
	float getfloat() { return floatnum; } 
	int getint() { return intnum; }

	int gen_exp_code(IRBuilder& builder);
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
private:
	number_type type;
};




class StringExp : public Exp {
	std::string str; 
public:
	StringExp(std::string& ref, int ln) : Exp(ln), str(ref) { set_constant(); } 
	
	int gen_exp_code(IRBuilder& builder);
	
	void print() { print_group("StringExp"); }
	void print_children() { fprintf(yyout, "(String %d \"%s\")", get_line(), str.c_str()); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string getstring() { return str; } 

//Coercion check code
//When this function is called, it looks at its type it looks at its content,
        //if it contains a decimal or exponential notation, it attempts
        //to convert to numeric and returns float, if it does not contain
        //a decimal or exp notation, it attempts to convert to numeric and
        //returns integer.  If conversion fails, returns String.
SymbolInfo::llvm_type typecoercion(){

	  //Obtain C style string (because that is easier for me and 
	  //I dont want to spend much mroe time)
	    char* mystring = (char*)(str).c_str();
          char* tempstring =(char*) malloc(sizeof(strlen(mystring)+1));
	  //Is the string EXACTLY "0" ?
	  
	  tempstring = strcpy(tempstring,mystring);

	  if(!strcmp(tempstring,"0")){         //strcmp returns 0 if match
            free(tempstring);
	    return SymbolInfo::LL_INTEGER;
	  }
	  


	  //are there any characters that indicate it may be a float?
	  char* chardot =strchr(tempstring,'.');
	  char* chare =strchr(tempstring,'e');
	  char* charE =strchr(tempstring,'E');

	  if(chardot!=NULL || chare!=NULL || charE!=NULL){   //strchr returns NULL if not found
	    //Enter if a character indicates float
	  
	    float floatval;
	    float zeroval;
	    //get the float value of string
	    floatval = atof(tempstring);

	    //but wait....what if "000.0" or "0.0e0" in that case atof returns
            //0...............we need to account for those cases


	    if(chardot!=NULL){
	      *chardot = '1';
	    }
	    if(chare !=NULL){
	      *chare = '1';
	    }
	    if(charE !=NULL){
	      *charE = '1';
	    }

	    zeroval = atof(tempstring);  //if it were as above, it should give a non-zero value

	    //if floatval is 0 and zeroval is not zero, return float
            //because that means "00.0" or "0.0e000"
            //else if floatval is 0, return string
            //else return float

	    if(floatval==0 && zeroval!=0){
	      free(tempstring);
	      return SymbolInfo::LL_FLOAT;
	    }
	    else if(floatval==0){
	      free(tempstring);
	      return SymbolInfo::LL_STRING;
	    }
	    else{
	      free(tempstring);
	      return SymbolInfo::LL_FLOAT;
	    }
	    
	   
	    

	  }

			  //If it is not the case that the float characters exist....may be int
	  else{
	    //We already accounted for the case where "0"...but what if "00" or "000000".......change last zero to a 1...
            //If atoi gives 1 where before it gave 0, it was a 0 and return int, else, if it still is 0, return string
            // if atoi gives non 0 from beginning, return int
	    int intval;
	    char* firstzero;

	  

	    intval = atoi(tempstring);

	    if(intval!=0){
	      free(tempstring);
	      return SymbolInfo::LL_INTEGER;
	    }
	    else{
  //Is there a zero to begin with?
	      firstzero = strchr(tempstring,'0');
	      //if not, return string
	      if(firstzero==NULL){
		free(tempstring);
		return SymbolInfo::LL_STRING;
	      }
	      //if yes, convert first instance to a 1
	      else{
		*firstzero = '1';
		intval = atoi(tempstring);
		//is intval still a 0, then string, else, int
		if(intval==0){
		  free(tempstring);
		  return SymbolInfo::LL_STRING;
		}
		else{
		  free(tempstring);
		  return SymbolInfo::LL_INTEGER;
		}
	      }
	      
	    }

	    

	  }
          
	  
    }
};






class ElipsisExp : public Exp {
public:
	ElipsisExp(int ln) : Exp(ln) {} 
	void print() { print_group("ElipsisExp"); }
	void print_children() { fprintf(yyout, "(Elipsis %d ...)", get_line()); } 

	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

class FuncExp : public Exp {
	Function *func;
public:
	FuncExp(Function *f, int ln = -1) : Exp(ln), func(f) {} 
	~FuncExp() { delete func; } 
	void print_children() { func->print(); }  
	void print() { print_group("FunctionExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

class PrefixExpExp : public Exp {
	PrefixExp *exp; 
public:
	PrefixExpExp(PrefixExp* e, int ln = -1) : Exp(ln), exp(e) {} 
	~PrefixExpExp() { delete exp; } 
	void print_children() { exp->print(); } 
	void print() { print_group("PrefixExp-Exp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder) { return exp->gen_exp_code(builder); }
};

class TableExp : public Exp {
	TableCons* table;
public:	
	TableExp(TableCons* cons, int ln = -1) : Exp(ln), table(cons) {} 
	~TableExp() { delete table; } 
	void print_children() { table->print(); } 
	void print() { print_group("TableExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

// keep this class (It's still being used by all the classes below)
class BinOpExp : public Exp {
protected:
	Exp *ex1; Exp *ex2;
public:
	BinOpExp(Exp *e1, Exp *e2, int ln = -1) : Exp(ln), ex1(e1), ex2(e2) {} 
	~BinOpExp() { delete ex1; delete ex2; }
	int gen_code_binop(IRBuilder& builder, ops operation);
	int gen_code_logop(IRBuilder& builder, ops operation);
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

class AddExp : public BinOpExp {
public:
	AddExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" + ", yyout);  ex2->print(); } 
	void print() { print_group("AddExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class SubExp : public BinOpExp {
public:
	SubExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" - ", yyout);  ex2->print(); } 
	void print() { print_group("SubExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class MultExp : public BinOpExp {
public:
	MultExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" * ", yyout);  ex2->print(); } 
	void print() { print_group("MultExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class DivExp : public BinOpExp {
public:
	DivExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" / ", yyout);  ex2->print(); } 
	void print() { print_group("DivExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class PowExp : public BinOpExp {
public:
	PowExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" ^ ", yyout);  ex2->print(); } 
	void print() { print_group("PowExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class ModExp : public BinOpExp {
public:
	ModExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" % ", yyout);  ex2->print(); } 
	void print() { print_group("ModExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class ConcatExp : public BinOpExp {
public:
	ConcatExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" .. ", yyout);  ex2->print(); } 
	void print() { print_group("ConcatExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class LTExp : public BinOpExp {
public:
	LTExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" < ", yyout);  ex2->print(); } 
	void print() { print_group("LessThanExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class LEExp : public BinOpExp {
public:
	LEExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" <= ", yyout);  ex2->print(); } 
	void print() { print_group("LessThanEqExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class GTExp : public BinOpExp {
public:
	GTExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" > ", yyout);  ex2->print(); } 
	void print() { print_group("GreaterThanExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class GEExp : public BinOpExp {
public:
	GEExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" >= ", yyout);  ex2->print(); } 
	void print() { print_group("GreaterThanEqExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class EqualExp : public BinOpExp {
public:
	EqualExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" == ", yyout);  ex2->print(); } 
	void print() { print_group("EqualExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class NotEqualExp : public BinOpExp {
public:
	NotEqualExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" ~= ", yyout);  ex2->print(); } 
	void print() { print_group("NotEqualExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class AndExp : public BinOpExp {
public:
	AndExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" and ", yyout);  ex2->print(); } 
	void print() { print_group("AndExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class OrExp : public BinOpExp {
public:
	OrExp(Exp *e1, Exp *e2, int ln = -1) : BinOpExp(e1,e2,ln) {}
	void print_children() { ex1->print(); fputs(" or ", yyout);  ex2->print(); } 
	void print() { print_group("OrExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class MinusExp : public Exp {
	Exp *ex;
public:
	MinusExp(Exp *e, int ln = -1) : Exp(ln), ex(e) {} 
	~MinusExp() { delete ex; }
	void print_children() { fputs(" - ", yyout);  ex->print(); } 
	void print() { print_group("MinusExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class NotExp : public Exp {
	Exp *ex;
public:
	NotExp(Exp *e, int ln = -1) : Exp(ln), ex(e) {} 
	~NotExp() { delete ex; }
	void print_children() { fputs(" not ", yyout);  ex->print(); } 
	void print() { print_group("NotExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

class LengthExp : public Exp {
	Exp *ex;
public:
	LengthExp(Exp *e, int ln = -1) : Exp(ln), ex(e) {} 
	~LengthExp() { delete ex; }
	void print_children() { fputs(" # ", yyout);  ex->print(); } 
	void print() { print_group("LengthExp"); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};
//~ End Expression Classes 


/*
class BinOpExp : public Exp {
	BinOp* op; Exp *ex1, *ex2;  
public:
	BinOpExp(BinOp *bo, Exp *e1, Exp *e2, int ln = -1) : Exp(ln), op(bo), ex1(e1), ex2(e2) {} 
	~BinOpExp() { delete op; delete ex1; delete ex2; } 
	void print_children() { ex1->print(); fputc(' ', yyout); op->print(); fputc(' ', yyout); ex2->print(); } 
	void print() { print_group("BinOpExp"); }
}; 

class UnOpExp : public Exp {
	UnOp *op; Exp *exp;
public:
	UnOpExp(UnOp *uo, Exp *ex, int ln = -1) : Exp(ln), op(uo), exp(ex) {} 
	~UnOpExp() { delete op; delete exp; } 
	void print_children() { op->print(); fputc(' ', yyout); exp->print(); } 
	void print() { print_group("UnOpExp"); }
};
*/

// Prefix Expression Classes 
class VarPrefix : public PrefixExp {
	Var* var;
public:
	VarPrefix(Var* v, int ln = -1) : PrefixExp(ln), var(v) {} 
	~VarPrefix() { delete var; } 
	void print_children() { var->print(); } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string get_symbol_name() { return var->get_symbol_name(); } 
	int gen_exp_code(IRBuilder& builder);
};

class FuncPrefix : public PrefixExp {
	FuncCall* call;
public:
	FuncPrefix(FuncCall* f, int ln = -1) : PrefixExp(ln), call(f) {} 
	~FuncPrefix() { delete call; } 
	void print_children() { call->print(); } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string get_symbol_name() { 
		serror(get_line(), "Compiler does not support functions as a return type.");
		return std::string();
	}
	int gen_exp_code(IRBuilder& builder);
};

class ExpPrefix : public PrefixExp {
	Exp *exp;
public:
	ExpPrefix(Exp *ex, int ln = -1) : PrefixExp(ln), exp(ex) {}
	~ExpPrefix() { delete exp; } 
	void print_children() { exp->print(); }	
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string get_symbol_name() { 
		serror(get_line(), "Compiler does not support closures.");
		return std::string();
	}
	int gen_exp_code(IRBuilder& builder);
};
//~ End Prefix Expression Classes 


#endif


