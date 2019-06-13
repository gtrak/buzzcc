#ifndef AST_H
#define AST_H

#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <utility>
#include <cstdio>

#include "symtable.h"
#include "ir.h"

class SyntaxTree;
extern FILE *yyout, *yyin;
extern SyntaxTree *ast;

class AstNode {
	int line;
	SymbolTable* scope;
public:
	AstNode() : line(-1), scope(NULL) {} // Get rid of this later
	AstNode(int ln) : line(ln) {}
	int get_line() { return line; }
	void set_line(int ln) { line = ln; }
	void print_group(const char* name);
	virtual void print_children() {};  // Change to virtual void later to check for errors
	void set_scope(SymbolTable* scp) { scope = scp; }
	SymbolTable* get_scope() { return scope; }
	// Important stuff for all nodes for type-inference/symbol table construction

	//virtual SymbolInfo::llvm_type infer_type(TableBuilder& builder) { return SymbolInfo::LL_UNKNOWN; } ;
	//virtual void gen_code(IRBuilder& builder) { };
	static void serror(int ln, const char* msg) {
		fprintf(stderr, "Line %d: %s\n", ln, msg);
		//delete ast;
		exit(EXIT_FAILURE);
	}
};

// class for doing output indentation of ast printer
class Indenter {
    int level;
    static const int spaces = 2;
    Indenter() { level = 0; }
    Indenter(const Indenter&);
public:
    static Indenter& GetIndenter() {
        static Indenter sing;
        return sing;
    }
    void inc() { ++level; }
    void dec() { --level; }
    void indent() { for (int i = 0; i < level*spaces; i++) fputc(' ', yyout);  }
};

// template for list based tree nodes
template <typename T>
struct ListNode : public std::vector<T>, public virtual AstNode {
	ListNode() : AstNode(-1) {}
	ListNode(int line_num) : AstNode(line_num) {}
	~ListNode() { for (unsigned int i = 0; i < this->size(); i++) { delete this->operator[](i); } }
    void smart_set() { if (this->size() != 0) set_line(this->operator[](0)->get_line()); }
    void print_children_lines() {
		fputc('\n', yyout);
		Indenter& ind = Indenter::GetIndenter();
		ind.inc();
		for (unsigned int i = 0; i < this->size(); i++) {
    		ind.indent();
        	this->operator[](i)->print();
        	fputc('\n', yyout);

    	}
		ind.dec();
		ind.indent();
	}
	void print_children_spaces() {
        for (unsigned int i = 0; i < this->size(); i++) {
            fputc(' ', yyout);
            this->operator[](i)->print();
        }
    }
	void reverse() { std::reverse(this->begin(), this->end()); }
	void print_children() { print_children_spaces(); }
};

// Expression abstract class

class Exp : public AstNode {
	bool constant;
	SymbolInfo::llvm_type type;
public:
	void set_type(SymbolInfo::llvm_type t) { type = t; }
	SymbolInfo::llvm_type get_type() { return type; }
	Exp(int ln) : AstNode(ln), constant(false) {}
	Exp() : AstNode(-1), constant(false), type(SymbolInfo::LL_UNKNOWN) {}
	virtual void print() { print_group("Exp"); };
	virtual void print_children() = 0;
	virtual ~Exp() {}
	virtual SymbolInfo::llvm_type typecoercion(){
	  return get_type();
	}
	virtual bool getBoolVal(){
	    serror(get_line(),"Boolean Expression was not initialized (may be a dynamic-typing case)");
	    return false;
	}
	virtual int gen_exp_code(IRBuilder& builder) { return -1; };
	virtual SymbolInfo::llvm_type infer_type(TableBuilder& builder) = 0;
	bool is_constant() { return constant; }
	void set_constant() { constant = true; }
};

// Prefix Expression abstract class
class PrefixExp : public AstNode {
public:
	PrefixExp(int ln) : AstNode(ln) {}
	virtual ~PrefixExp() {}
	virtual void print_children() = 0;
	virtual void print() { print_group("PrefixExp"); }
	virtual SymbolInfo::llvm_type infer_type(TableBuilder& builder) = 0;
    virtual int gen_exp_code(IRBuilder& builder) = 0;
	virtual std::string get_symbol_name() = 0;
};

class ExpList;
// Args abstract class
class Args : public AstNode {
public:
	typedef std::vector<Exp*> ArgVec;
	Args(int ln) : AstNode(ln) {}
	Args() : AstNode(-1) {}
	virtual ~Args() {}
	void print() { print_group("Args"); }
	virtual void print_children() = 0;
	virtual ArgVec* get_explist() { serror(get_line(), "Table Arguments are not supported"); return 0; }
};

// Function call class
class FuncCall : public AstNode {
	PrefixExp *pexp;
	Args *args;
public:
	FuncCall(PrefixExp* e1, Args* a1, int ln = -1) : AstNode(ln), pexp(e1), args(a1) {}
	~FuncCall() { delete pexp; delete args; }
	void print() { print_group("FunctionCall"); }
	void print_children() { pexp->print(); fputc(' ', yyout); args->print(); }
    virtual SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	int gen_exp_code(IRBuilder& builder);
};

// Name class
struct Name : public std::string, public AstNode {
	//Name(char*& str, int line) : std::string(str), AstNode(line) {}
	Name(std::string& ref, int line = -1) : std::string(ref), AstNode(line) {}
	Name(char* str, int line=-1) : std::string(str), AstNode(line) {}
	Name(const char* st, const char* end, int line=-1) : std::string(st,end), AstNode(line) {}
	~Name() {}
	void print() { print_group("Name"); }
	void print_children() { fputs(c_str(), yyout); }
    SymbolInfo::llvm_type infer_type(TableBuilder& builder);
    int name_gen_code_local(IRBuilder& builder);
    int name_gen_code_global(IRBuilder& builder);

};

// Var abstract class
struct Var : public AstNode {
	virtual std::string get_symbol_name() = 0;
	Var(int ln) : AstNode(ln) {}
	virtual ~Var() {};
	virtual void print() { print_group("Var"); };
    SymbolInfo::llvm_type infer_type(TableBuilder& builder) {
    	set_scope(builder.get_current_scope());
    	SymbolInfo *info = builder.find(get_symbol_name());
    	if (info == NULL) serror(get_line(), "Variable name undefined.");
    	return info->get_type();
	};
};

// Statement abstract class
class Statement : public virtual AstNode {
public:
	Statement(int line_num) : AstNode(line_num) {}
	Statement() : AstNode(-1) {} // Get rid of this later
	virtual void print() = 0;
	virtual void print_children() = 0;
	virtual ~Statement() {}
	virtual SymbolInfo::llvm_type infer_type(TableBuilder& builder) = 0;
	virtual void gen_code(IRBuilder& builder) { }
	virtual bool changes_scope() { return false; }
};

// Field abstract class
class Field : public AstNode {
public:
	Field(int ln) : AstNode(ln) {}
	virtual void print_children() = 0;
	void print() { print_group("Field"); }
	virtual ~Field() {}
};

// Chunk class
class Chunk : public ListNode<Statement*> {
public:
	Chunk() : AstNode(-1) {}
	Chunk(int lnum) : AstNode(lnum) {}
	void print() { print_group("Chunk"); }
	void print_children() { print_children_lines(); }
	void gen_code(IRBuilder& builder);
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

// Base Tree class
class SyntaxTree  {
	Chunk *root;
public:
	SyntaxTree(Chunk* blk) : root(blk) {}
	~SyntaxTree() { delete root; }
	void lisp_print();
	void infer_type(TableBuilder& builder) { root->infer_type(builder); }
	void gen_code(IRBuilder& builder) {
		root->gen_code(builder);
		builder.add_ins(new ReturnFromFunc(convert(SymbolInfo::LL_INTEGER), 0, ARG_IMM));
	}
};

// Block class
class Block : public AstNode {
	Chunk* chk;
public:
	Block(int li) : AstNode(li) {}
	Block(Chunk *ck, int li = -1) : AstNode(li), chk(ck) {}
	void print() { print_group("Block"); }
	void print_children();
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	bool return_at_end();
	Chunk* getChunkPointer(){return chk;}
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

typedef std::pair<Exp*, Block*> IfPair;
template<> void ListNode<IfPair>::print_children_spaces();
template<> ListNode<IfPair>::~ListNode();
// ParList abstract class
class NameList;
class ParList : public AstNode {
public:
	ParList(int ln) : AstNode(ln) {}
	void print() { print_group("Parlist"); }
	virtual void print_children() = 0;
	virtual ~ParList() {}
	virtual NameList* get_namelist() { serror(get_line(), "Variable arguments are not supported."); return NULL; }
};

// Funcbody class
class FuncBody : public AstNode {
	ParList* lst; Block* code;
public:
	FuncBody(Block *blk, int ln = -1, ParList* list = 0) : AstNode(ln), lst(list), code(blk) { }
	~FuncBody() { if (lst) delete lst; delete code; }
	void print() { print_group("Funcbody"); }
	void print_children() { if (lst) { lst->print(); fputc(' ', yyout);} code->print(); }
	ParList* get_params() { return lst; }
	Block* get_block() { return code; }
	Chunk* getChunkPointer(){return code->getChunkPointer();}
};

// Function class
class Function : public AstNode {
	FuncBody *bdy;
public:
	Function(FuncBody *b, int ln = -1) : AstNode(ln), bdy(b) {}
	~Function() { delete bdy; }
	void print() { print_group("Function"); }
	void print_children() { bdy->print(); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder) {
		serror(get_line(), "Closures are not allowed.");
		return SymbolInfo::LL_UNKNOWN;
	}

};

// List based subclasses
class VarList : public ListNode<Var*>  {
public:
	VarList(int ln) : AstNode(ln) {}
	VarList(int ln, Var* var) : AstNode(ln) { push_back(var); }
	void print() { print_group("VarList"); }
};

class ExpList : public ListNode<Exp*> {
public:
	ExpList(int ln) : AstNode(ln) {}
	ExpList(int ln, Exp* var) : AstNode(ln) { push_back(var); }
	void print() { print_group("ExpList"); }
	void print_children() { print_children_lines(); }
	//SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

class FuncName : public ListNode<Name*> {
public:
	FuncName(int ln) : AstNode(ln) {}
	FuncName(int ln, Name *nm) : AstNode(ln) { push_back(nm); }
	void print() { print_group("FuncName"); }
	std::string get_symbol_name() {
		if (size() > 1) serror(get_line(), "Nested Functions are not supported.");
		return *(front());
	}
};

class NameList : public ListNode<Name*> {
public:
	NameList(int ln) : AstNode(ln) {}
	NameList(int ln, Name *nm) : AstNode(ln) { push_back(nm); }
	void print() { print_group("NameList"); }
};

class FieldList : public ListNode<Field*> {
public:
	FieldList(int ln) : AstNode(ln) {}
	FieldList(int ln, Field* fl) : AstNode(ln) { push_back(fl); }
	void print() { print_group("FieldList"); }
	void print_children() { print_children_lines(); }
};
//~ End List based subclasses

// TableConstructor class
class TableCons : public AstNode {
	FieldList* list;
public:
	//	TableCons(int ln = -1) : AstNode(ln){}
	TableCons(int ln = -1, FieldList *l = 0) : AstNode(ln), list(l) {}
	void print() { print_group("TableConstructor"); }
	void print_children() { if (list) list->print(); }
	~TableCons() { if (list) delete list; }
};

// Args subclasses
class EmptyArgs : public Args {
public:
	EmptyArgs(int ln = -1) : Args(ln) {}
	~EmptyArgs() {}
	void print_children() {}
	std::vector<Exp*>* get_explist() { return new ArgVec; }
};

class ExpArgs : public Args {
	ExpList* list;
public:
	ExpArgs(ExpList *l, int ln) : Args(ln), list(l) {}
	~ExpArgs() { delete list; }
	void print_children() { list->print(); }
	std::vector<Exp*>* get_explist() {
		ArgVec* ptr =  new ArgVec;
		for (unsigned int i = 0; i < list->size(); i++) (*ptr).push_back((*list)[i]);
		return ptr;
	}
};

class TableArg : public Args {
	TableCons *cons;
public:
	TableArg(TableCons *tb, int ln) : Args(ln), cons(tb) {}
	~TableArg() { delete cons; }
	void print_children() { cons->print(); }
};
class StringExp;
class StringArg : public Args {
	std::string str;
	StringExp* t;
public:
	StringArg(std::string& s, int ln = -1) : Args(ln), str(s), t(0) {}
	~StringArg();
	void print_children() { fprintf(yyout, "(String %d \"%s\")", get_line(), str.c_str()); }
	ArgVec* get_explist();
};
//~ End Arguments classes

// Parameter List classes
class NameParList : public ParList {
	NameList *names;
public:
	NameParList(NameList *list, int ln = -1) : ParList(ln), names(list) {}
	~NameParList() { delete names; }
	void print_children() { names->print(); }
	NameList* get_namelist() { return names; }
};

class NameVAParList : public ParList {
	NameList *names;
public:
	NameVAParList(NameList *list, int ln = -1) : ParList(ln), names(list) {}
	~NameVAParList() { delete names; }
	void print_children() { names->print(); fputs(" ... ", yyout); }
};

class VAParList : public ParList {
public:
	VAParList(int ln = -1) : ParList(ln) {}
	void print_children() { fputs(" ... ", yyout); }
};
//~ End Parameter List classes

// Field Classes
class BracketField : public Field {
	Exp *ind; Exp *val;
public:
	BracketField(Exp *i, Exp *v, int ln = -1) : Field(ln), ind(i), val(v) {}
	~BracketField() { delete ind; delete val; }
	void print_children() { ind->print(); fputc(' ', yyout); val->print(); }
};

class NameField : public Field {
	Name *nm; Exp *val;
public:
	NameField(Name* n, Exp *e, int ln = -1) : Field(ln), nm(n), val(e) {}
	~NameField() { delete nm; delete val; }
	void print_children() { nm->print(); fputc(' ', yyout); val->print(); }
};

class ExpField : public Field {
	Exp *exp;
public:
	ExpField(Exp *e, int ln = -1) : Field(ln), exp(e) {}
	~ExpField() { delete exp; }
	void print_children() { exp->print(); }
};
//~ End Field Classes

// Var Classes
class NameVar : public Var {
	Name *name;
public:
	std::string get_symbol_name() { return *name; }
	NameVar(Name* nm, int ln) : Var(ln), name(nm) {}
	~NameVar() { delete name; }
	void print_children() { name->print(); }
    SymbolInfo::llvm_type infer_type(TableBuilder& builder);
};

class IndexedVar : public Var {
public:
	PrefixExp* pre; Exp *index_exp;
	IndexedVar(PrefixExp *ex1, Exp *ex2, int ln) : Var(ln), pre(ex1), index_exp(ex2) {}
	~IndexedVar() { delete pre; delete index_exp; }
	void print_children() { pre->print(); fputc(' ', yyout); index_exp->print(); }
    SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string get_symbol_name()  { serror(get_line(), "No support for Indexed Variables."); return std::string(); }
};

class SelectVar : public Var {
public:
	PrefixExp* pre; Name *element;
	SelectVar(PrefixExp *ex1, Name *nm, int ln) : Var(ln), pre(ex1), element(nm) {}
	~SelectVar() { delete pre; delete element; }
	void print_children() { pre->print(); fputc(' ', yyout); element->print(); }
    SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	std::string get_symbol_name()  { serror(get_line(), "No support for Select Variables."); return std::string(); }
};
//~ End Var Classes

#endif
