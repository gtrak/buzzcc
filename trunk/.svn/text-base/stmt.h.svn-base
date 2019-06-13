#ifndef STMT_H
#define STMT_H
#include "node.h"

// Statement sub-classes 
class AssignSt : public Statement { 
	VarList* vars; ExpList* exps;
public:
	AssignSt(VarList* var, ExpList* exp, int num) : AstNode(num), vars(var), exps(exp) {}  
	void print() { print_group("Assignment"); }
	void print_children() { vars->print(); fputc(' ',yyout); exps->print(); }
	~AssignSt() { delete vars; delete exps; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
};

class FuncCallSt : public Statement {
	FuncCall* fx; 
public:
	FuncCallSt(FuncCall* f, int ln) : AstNode(ln), fx(f) {} 
	void print() { print_group("FunctionCallSt"); }
	void print_children() { fx->print(); } 
	~FuncCallSt() { delete fx; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder&);
};

class DoEndSt : public Statement {
	Block *blk;
public:
	DoEndSt(Block *bl, int ln = -1) : AstNode(ln), blk(bl) {} 
	void print() { print_group("DoEnd"); } 
	void print_children() { blk->print(); }
	~DoEndSt() { delete blk; }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
    void gen_code(IRBuilder& biulder);
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

class WhileLoopSt : public Statement {
	Exp *pred; Block *blk; 
public:
	WhileLoopSt(Exp *pre, Block *bl, int ln = -1) : AstNode(ln), pred(pre), blk(bl) {}
	void print() { print_group("WhileLoop"); }
	void print_children() { pred->print(); fputc(' ',yyout); blk->print(); } 
	~WhileLoopSt() { delete pred; delete blk; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

class RepeatLoopSt : public Statement {
	Exp *pred; Block *blk;  
public:
	RepeatLoopSt(Exp *pre, Block *bl, int ln = -1) : AstNode(ln), pred(pre), blk(bl) {}
	void print() { print_group("RepeatLoop"); }
	void print_children() { blk->print(); fputc(' ', yyout); pred->print(); }
	~RepeatLoopSt() { delete blk; delete pred; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

class IfThenSt : public ListNode<IfPair>, public Statement {
	Block *final;
public:
    IfThenSt();
	void print_children();
    void print(); 
	void add_else(Block* ptr);
	void add_elif(Exp *ex, Block *blk);
	void add_if(Exp *ex, Block *blk);
	~IfThenSt(); 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

class ForLoopSt : public Statement {
	Name* nm; Exp *start, *end, *inc; Block *blk; 
public:
	void print() { print_group("ForLoop"); } 
	void print_children(); 
	ForLoopSt(Name *nm, Exp *st, Exp* end, Block* code, int ln, Exp* inc = 0); 
	~ForLoopSt();
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	SymbolInfo::llvm_type return_type(TableBuilder& builder);
};

class ForIterSt : public Statement {
	NameList *names; ExpList *exps; Block *blk; 
public:
	ForIterSt(NameList* nm, ExpList *exp, Block *code, int ln = -1) : 
		AstNode(ln), names(nm), exps(exp), blk(code) {}
	void print() { print_group("IterLoop"); } 
	void print_children(); 
	~ForIterSt();
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
};

class FuncDeclSt : public Statement {
	FuncName* name; FuncBody* body;
public:
	FuncDeclSt(FuncName* nm, FuncBody* bdy, int ln = -1) : AstNode(ln), name(nm), body(bdy) {} 
	void print() { print_group("FuncDecl"); }
	void print_children() { name->print(); fputc(' ', yyout); body->print(); } 
	~FuncDeclSt() { delete name; delete body; }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	SymbolInfo::llvm_type infer_type_fcall(Args* arg, TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	Chunk* getChunkPointer(){return body->getChunkPointer();} 
};

class LocFuncDeclSt : public Statement {
	Name* name; FuncBody* body; 
public:
	LocFuncDeclSt(Name* nm, FuncBody* bdy, int ln = -1) : AstNode(ln), name(nm), body(bdy) {}
	void print() { print_group("LocalFuncDecl"); } 
    void print_children() { name->print(); fputc(' ', yyout); body->print(); }
	~LocFuncDeclSt() { delete name; delete body; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
};

class LocAssignSt : public Statement {
	NameList* nlist; ExpList* elist; 
public:
	LocAssignSt(NameList* nm, ExpList* exp, int ln = 1) : AstNode(ln), nlist(nm), elist(exp) {}
	void print() { print_group("LocalAssignment"); } 
	void print_children() { nlist->print(); fputc(' ', yyout); elist->print(); } 
	~LocAssignSt() { delete nlist; delete elist; }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
};

class LocDefSt : public Statement {
	NameList* nlist;
public:
	LocDefSt(NameList* nm, int ln = -1) : AstNode(ln), nlist(nm) {}
	void print() { print_group("LocalDef"); } 
	void print_children() { nlist->print(); } 	
	~LocDefSt() { delete nlist; } 
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
};
//~ End Statement subclasses

// Last statement subclasses 
class Return : public Statement {
	ExpList *ret;
public:	
	Return(ExpList *list = 0, int ln = -1) : AstNode(ln), ret(list) {} 
	~Return() { if (ret) delete ret; }
	void print() { print_group("Return"); }
	void print_children() { if (ret) ret->print(); }
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	SymbolInfo::llvm_type infer_return_type(TableBuilder& builder);
	void gen_code(IRBuilder& builder);
	bool changes_scope() { return true; } 
};

class Break : public Statement {
public:
	Break(int ln = -1) : Statement(ln) {} 
	void print() { print_group("Break"); }
	void print_children() {} 
	~Break() {}
	SymbolInfo::llvm_type infer_type(TableBuilder& builder);
	bool changes_scope() { return true; }
};
//~ End Last Statement subclasses 

#endif
