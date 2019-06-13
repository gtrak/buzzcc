/* Lua Parser */

%{ 

#include "exp.h"
#include "stmt.h"
#include "ast.h"

#include <iostream>
#include <cstring>
#include <string>

using namespace std; 
extern void yyerror(const char *msg);
extern int yylex();
extern int yylineno;
extern SyntaxTree *ast;

%} 

/* Bison Declarations */ 
%union 
{
	Chunk *chk;
	Statement *stmt;
	Exp *exp;
	PrefixExp *pexp;
	Args *args;
	FuncCall *fc;
	Name *nm;
	Var *var;
	Block *blk;
	ParList *pl;
	FuncBody *fb;
	Function *fx;
	VarList *vl;
	ExpList *el;
	FuncName *fn;
	NameList *nl;	
	FieldList *fl;
	TableCons *tc;
	Field *field;
	IfThenSt *ifs;

	struct { float val; int line_num; } f;
	struct { Name *str; int line_num; } s;
	struct { int val; int line_num; } i;
	int line_num;
}

/* operators */
%left  <line_num> OR
%left  <line_num> AND
%left  <line_num> LT GT LE GE NEQ EQ 
%right <line_num> CONCAT 
%left  <line_num> PLUS MINUS 
%left  <line_num> MULT DIV MOD
%left  <line_num> NOT POUND /* missing unary - operator */
%right <line_num> EXP 


/* keywords */
%token <line_num> DO END WHILE REPEAT UNTIL IF THEN ELSEIF ELSE FOR 
%token <line_num> IN FUNCTION LOCAL RETURN BREAK NIL FALSE TRUE 

/* java keywords */
%token <line_num> CLASS EXTENDS VIRTUAL PUBLIC PRIVATE STATIC THIS PROTECTED 

/* variables, constants */ 
%token <s> NAME 
%token <f> INT
%token <f> FLOAT
%token <s> STRING 

/* more terminals */
%token <line_num> SEMIC 	";"
%token <line_num> ASSIGN 	"="
%token <line_num> COMMA 	","
%token <line_num> PERIOD  	"."
%token <line_num> ELIPSIS  "..."

%token <line_num> LPAREN "("
%token <line_num> RPAREN ")"
%token <line_num> LSQRBRACK  "["
%token <line_num> RSQRBRACK  "]"
%token <line_num> LCURLBRACK "{"
%token <line_num> RCURLBRACK "}" 

/* non-terminal semantic values */
%type <chk> statrep;
%type <stmt> stat;
%type <stmt> laststat;
%type <chk> chunk;
%type <blk> block;
%type <ifs>  elseifrep; 
%type <fn> funcname;
%type <vl> varlist;
%type <var> var;
%type <nl> namelist;
%type <el> explist;
%type <exp> exp;
%type <pexp> prefixexp;
%type <fc> functioncall;
%type <args> args;
%type <fx> function;
%type <fb> funcbody;
%type <pl> parlist;
%type <tc> tableconstructor;
%type <fl> fieldlist;
%type <fl> fieldlistrep;
%type <field> field;
 /*%type <bo> binop; */
 /* %type <uo> unop; */

/* Language Grammar */ 
%start init 

%%  
init: 
	chunk 					{ ast = new SyntaxTree($1); $1->smart_set(); 				} 
;

chunk:		
	statrep 				{ $$ = $1; 													} |
	statrep laststat 		{ $$ = $1; $1->push_back($2); 								} |
	statrep laststat ";" 	{ $$ = $1; $1->push_back($2); 								}   
;

statrep:
	/* empty */ 			{ $$ = new Chunk(); 										} |
	statrep stat 			{ $$ = $1; $1->push_back($2); 								} |
	statrep stat ";"		{ $$ = $1; $1->push_back($2); 								} 	
;

block: 		
	chunk					{ $1->smart_set(); $$ = new Block($1, $1->get_line());		} 
;

stat:
	varlist "=" explist 							{ $$ = new AssignSt($1,$3,$1->get_line()); 							} |
	functioncall 									{ $$ = new FuncCallSt($1, $1->get_line()); 							} |
	DO block END 									{ $$ = new DoEndSt($2, $1); 										} |
	WHILE exp DO block END 							{ $$ = new WhileLoopSt($2,$4,$1)									} |
	REPEAT block UNTIL exp 							{ $$ = new RepeatLoopSt($4,$2,$1);									} |
	IF exp THEN block elseifrep END 				{ $$ = $5; $5->add_if($2,$4); $5->set_line($1);						} |
	IF exp THEN block elseifrep ELSE block END 		{ $$ = $5; $5->add_if($2,$4); $5->add_else($7); $$->set_line($1);	} | 
	FOR NAME "=" exp "," exp DO block END 			{ $$ = new ForLoopSt($2.str, $4, $6, $8, $1); 						} | 
	FOR NAME "=" exp "," exp "," exp DO block END 	{ $$ = new ForLoopSt($2.str, $4, $6, $10, $1, $8);					} |
	FOR namelist IN explist DO block END 			{ $$ = new ForIterSt($2, $4, $6, $1);								} |
	FUNCTION funcname funcbody 						{ $$ = new FuncDeclSt($2,$3, $1);									} | 
	LOCAL FUNCTION NAME funcbody 					{ $$ = new LocFuncDeclSt($3.str, $4, $1); 							} | 
	LOCAL namelist 									{ $$ = new LocDefSt($2, $1); 										} |
	LOCAL namelist "=" explist 						{ $$ = new LocAssignSt($2, $4); 									} |
	/* java class additions */
	CLASS NAME classbody END						{} |
	PUBLIC CLASS NAME classbody END 				{} |
	CLASS NAME EXTENDS NAME classbody END			{} |
	PUBLIC CLASS NAME EXTENDS NAME classbody END	{} 
;

/* java class non-terminals */
classbody:
	classbody classmember							{} |
	classmember 									{}
;

classmember:
	accessperm FUNCTION NAME block END				{} |
	accessperm NAME 								{} |
	accessperm NAME "=" NAME 						{}
;

accessperm:
	PRIVATE 										{} |
	PUBLIC											{} |
	PROTECTED 										{} |
	/* empty */										{} 
;

/* end java class non-terminals */


elseifrep:
	/* empty */ 										{ $$ = new IfThenSt(); 				} |
	elseifrep ELSEIF exp THEN block 					{ $$ = $1; $1->add_elif($3,$5);	 	}  
;

laststat:
	RETURN 				{ $$ = new Return(); $$->set_line($1);								} |
	RETURN explist 		{ $$ = new Return($2, $1);											} |	
	BREAK				{ $$ = new Break($1);												} 
;

funcname:
	NAME 				{ $$ = new FuncName($1.line_num, $1.str); 						 	} |
	funcname "." NAME 	{ $$ = $1; $1->push_back($3.str);			 			 			} 
;

varlist:
	var 				{ $$ = new VarList($1->get_line(), $1); 							} | 
	varlist "," var 	{ $$ = $1; $1->push_back($3); 										} 
;

var: 
	NAME 				{ $$ = new NameVar($1.str, $1.line_num); 							} |
	prefixexp "[" exp "]"	{ $$ = new IndexedVar($1, $3, $1->get_line()); 					} |
	prefixexp "." NAME 	{ $$ = new SelectVar($1, $3.str, $3.line_num); 						}  
;

namelist:
	NAME 				{ $$ = new NameList($1.line_num, $1.str); 									} |
	namelist "," NAME 	{ $$ = $1; $1->push_back($3.str);			 								} 
;

explist:
	exp 				{ $$ = new ExpList($1->get_line(), $1);										} |
	explist "," exp		{ $$ = $1; $1->push_back($3);												} 
;

exp:
	NIL 				{ $$ = new NilExp($1);														} |
	FALSE 				{ $$ = new BoolExp(false, $1); 											 	} |
	TRUE 				{ $$ = new BoolExp(true, $1);												} |
	INT 				{ $$ = new NumberExp($1.val,-1, $1.line_num,NumberExp::NUM_INT);									} |
	FLOAT               { $$ = new NumberExp(-1,$1.val,$1.line_num,NumberExp::NUM_FLOAT);                                                                      } |
	STRING 				{ string tmp ($1.str->c_str()); $$ = new StringExp(tmp, $1.line_num); delete $1.str;} |
	"..."  				{ $$ = new ElipsisExp($1);													} |
	function 			{ $$ = new FuncExp($1, $1->get_line());										} |
	prefixexp 			{ $$ = new PrefixExpExp($1, $1->get_line())									} |
	tableconstructor 	{ $$ = new TableExp($1, $1->get_line());									} |
	MINUS exp 			{ $$ = new MinusExp($2,$1);													} |
	NOT exp 			{ $$ = new NotExp($2,$1);													} |
	POUND exp 			{ $$ = new LengthExp($2,$1);												} |
	exp PLUS exp 		{ $$ = new AddExp($1,$3,$2);												} |
	exp MINUS exp 		{ $$ = new SubExp($1,$3,$2);												} |
	exp MULT exp 		{ $$ = new MultExp($1,$3,$2);												} |
	exp DIV exp 		{ $$ = new DivExp($1,$3,$2);												} |
	exp EXP exp 		{ $$ = new PowExp($1,$3,$2);												} |
	exp MOD exp 		{ $$ = new ModExp($1,$3,$2);												} |
	exp CONCAT exp 		{ $$ = new ConcatExp($1,$3,$2);												} |
	exp LT exp 			{ $$ = new LTExp($1,$3,$2);													} |
	exp LE exp 			{ $$ = new LEExp($1,$3,$2);													} |
	exp GT exp 			{ $$ = new GTExp($1,$3,$2);													} |
	exp GE exp 			{ $$ = new GEExp($1,$3,$2);													} |
	exp EQ exp 			{ $$ = new EqualExp($1,$3,$2);									 			} |
	exp NEQ exp 		{ $$ = new NotEqualExp($1,$3,$2);											} |
	exp AND exp 		{ $$ = new AndExp($1,$3,$2);												} |
	exp OR exp 			{ $$ = new OrExp($1,$3,$2);													} 
;

prefixexp:
	var 				{ $$ = new VarPrefix($1,$1->get_line());									} | 
	functioncall 		{ $$ = new FuncPrefix($1, $1->get_line());									} |
	"(" exp ")" 		{ $$ = new ExpPrefix($2, $2->get_line());									} 
;

functioncall:
	prefixexp args 		{ $$ = new FuncCall($1, $2, $1->get_line()); 								} 
;

args:
	"(" ")" 			{ $$ = new EmptyArgs($1);													} |
	"(" explist ")" 	{ $$ = new ExpArgs($2, $1);													} |
	tableconstructor 	{ $$ = new TableArg($1, $1->get_line());									} |
	STRING 				{ string tmp($1.str->c_str()); $$ = new StringArg(tmp, $1.line_num); delete $1.str;	} 
;

function:
	FUNCTION funcbody  	{ $$ = new Function($2, $1);												} 	
;

funcbody: 
	"(" ")" block END 			{ $$ = new FuncBody($3, $1, new NameParList(new NameList($1),$1));	} |
	"(" parlist ")" block END 	{ $$ = new FuncBody($4, $1, $2);									} 
;

parlist:
	namelist 					{ $$ = new NameParList($1, $1->get_line());				 			} |
	namelist "," "..." 			{ $$ = new NameVAParList($1, $1->get_line());						} |
	"..."						{ $$ = new VAParList($1);											}
;

tableconstructor:
"{" "}" 					{ $$ = new TableCons($1); 											} |
"{" fieldlist "}"			{ $$ = new TableCons($1, $2);										} 
;

fieldlist:
	fieldlistrep 				{ $$ = $1;  														} |
	fieldlistrep fieldsep 		{ $$ = $1;															} 
;

fieldlistrep:
	field 							{ $$ = new FieldList($1->get_line(), $1); 						} | 
	fieldlistrep fieldsep field 	{ $$ = $1; $1->push_back($3);									}  
;

field:
	"[" exp "]" "=" exp 			{ $$ = new BracketField($2, $5, $2->get_line());				} |
	NAME "=" exp 					{ $$ = new NameField($1.str, $3, $1.line_num); 					} |
	exp 							{ $$ = new ExpField($1, $1->get_line());						}
;

fieldsep:
	"," | ";" 
;

/*
binop: 
	PLUS 	{ $$ = new BinOp(BinOp::OP_PLUS, $1);											} |
	MINUS 	{ $$ = new BinOp(BinOp::OP_MINUS, $1);											} | 
	MULT 	{ $$ = new BinOp(BinOp::OP_MULT, $1);											} | 
	DIV 	{ $$ = new BinOp(BinOp::OP_DIV, $1);											} | 
	EXP		{ $$ = new BinOp(BinOp::OP_EXP, $1);											} | 
	MOD 	{ $$ = new BinOp(BinOp::OP_MOD, $1);											} | 
	CONCAT 	{ $$ = new BinOp(BinOp::OP_CONCAT, $1);										} | 
	LT		{ $$ = new BinOp(BinOp::OP_LT, $1);											} |
	LE		{ $$ = new BinOp(BinOp::OP_LE, $1);											} | 
	GT 		{ $$ = new BinOp(BinOp::OP_GT, $1);											} | 
	GE 		{ $$ = new BinOp(BinOp::OP_GE, $1);											} |
	EQ 		{ $$ = new BinOp(BinOp::OP_EQ, $1);											} | 
	NEQ 	{ $$ = new BinOp(BinOp::OP_NEQ, $1);											} |
	AND 	{ $$ = new BinOp(BinOp::OP_AND, $1);											} |
	OR 		{ $$ = new BinOp(BinOp::OP_OR, $1);											} 
;

unop:
	MINUS 	{ $$ = new UnOp(UnOp::UO_MINUS, $1);													} |
	NOT   	{ $$ = new UnOp(UnOp::UO_NOT, $1);													} |	
	POUND 	{ $$ = new UnOp(UnOp::UO_POUND, $1);													} 
;
*/

%% 


