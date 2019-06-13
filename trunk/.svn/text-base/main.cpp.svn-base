#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast.h"
//#include "parse.h"

//#define DEBUG
#define OPTIMIZE
//#undef OPTIMIZE


void yyerror(char *msg);
void yyparse();

extern int yydebug;
extern int yylineno;
extern FILE *yyin, *yyout;

SyntaxTree *ast;
void compile();

using namespace std;
int main(int argc, char *argv[])
{
	yydebug = 0;

	if (argc == 1) {
		yyin = stdin;
		yyout = stdout;
		compile();
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			char root[128];
			if (sscanf(argv[i], "%127[^.].lua", root) != 1)
			{
				fprintf(stderr, "Error with file name.\n");
				exit(EXIT_FAILURE);
			}
			yyin = fopen(argv[i], "r");
			if (yyin == NULL)
			{
				fprintf(stderr, "Error opening input file \"%s\"\n", argv[i]);
				exit(EXIT_FAILURE);
			}
			strcat(root, ".ll");
			yyout  = fopen(root, "w");
			if (yyout == NULL )
			{
				fprintf(stderr, "Error opening output file \"%s\"", root);
				exit(EXIT_FAILURE);
			}
			compile();
		}
	}

	return 0;
}

void yyerror(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	fprintf(stderr, "Current lexer line: %d\n", yylineno);
	exit(EXIT_FAILURE);
}


void compile()
{
	yylineno = 0;
	printf("Lexing & Parsing.\n");
	yyparse();
	//ast->lisp_print();
	fflush(yyout);

	printf("Performing type inference.\n");
	SymbolTable table;
	TableBuilder sbuilder(&table);
	ast->infer_type(sbuilder);

	printf("Generating llvm IR.\n");
	IRTree ir;
	IRBuilder irbuilder(ir,table);
	ast->gen_code(irbuilder);

#ifdef OPTIMIZE
//    printf("Optimizing!\n");
    IROptimizer iroptimizer(ir,table);
    ir.optimize(iroptimizer);
#endif

	printf("Finished compiling.\n");
	ir.print_assembly();
}
