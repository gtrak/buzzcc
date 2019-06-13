D 			[0-9]
L 			[A-Za-z]
A			[a-zA-Z0-9]
NAME 		[a-zA-Z0-9_]
%x multistring
%x multicomment
%x singlequote
%x doublequote

%{
#include "ast.h"
#include "stmt.h"
#include "exp.h"
#include "parse.h" 
#include <cstdio>
#include <cstring> 

extern void yyerror(const char *msg);
void scan_error();
void rec_line_num();
using namespace std;
int equals_count;
std::string contents("");
%} 

 /* %option debug */
%option yylineno

%% 
^"#!"[^\n]*				{ /* remove shabang*/ }
"--"[^\[][^\n]* 		{ /* remove comment*/ }
"--["=*"["				{ 
							BEGIN(multicomment); 
							equals_count = 0;
							int i = 3;
							while (yytext[i] == '=') { i++; equals_count++; }
						}


<multicomment>[^\]]		{
							/*Throw away comment */
						}
<multicomment>"]"		{

						}
<multicomment>"]"=*"]" {
							int num_eq = 0;	
							int i = 1;
							while (yytext[i] == '=') { i++; num_eq++;}
							if (num_eq == equals_count) BEGIN(INITIAL);
						}


"do" 		{ rec_line_num(); return DO;		}
"end"		{ rec_line_num(); return END;		}
"while"		{ rec_line_num(); return WHILE; 	}
"repeat" 	{ rec_line_num(); return REPEAT;	}
"until"		{ rec_line_num(); return UNTIL;		}
"if"		{ rec_line_num(); return IF;		}
"then"		{ rec_line_num(); return THEN; 		}
"elseif"	{ rec_line_num(); return ELSEIF;	}
"else"		{ rec_line_num(); return ELSE;		}
"for"		{ rec_line_num(); return FOR;		}
"in"		{ rec_line_num(); return IN;		}
"function"	{ rec_line_num(); return FUNCTION;	}
"local"		{ rec_line_num(); return LOCAL; 	}
"return"	{ rec_line_num(); return RETURN;	}
"break" 	{ rec_line_num(); return BREAK; 	}
"nil" 		{ rec_line_num(); return NIL;		}
"false"		{ rec_line_num(); return FALSE;		}
"true"		{ rec_line_num(); return TRUE;		}

 /* Java keywords used in project 3 */
"class"  	{ rec_line_num(); return CLASS; 	}
"extends"	{ rec_line_num(); return EXTENDS; 	}
"virtual"	{ rec_line_num(); return VIRTUAL; 	}
"public"	{ rec_line_num(); return PUBLIC;	}
"private"	{ rec_line_num(); return PRIVATE;	}
"protected"	{ rec_line_num(); return PROTECTED;	}
"static"	{ rec_line_num(); return STATIC;	}
"this"		{ rec_line_num(); return THIS;		}

"+" 		{ rec_line_num(); return PLUS; 		} 
"-" 		{ rec_line_num(); return MINUS; 	}
"*" 		{ rec_line_num(); return MULT; 		}
"/" 		{ rec_line_num(); return DIV; 		}
"^" 		{ rec_line_num(); return EXP; 		}
"%"			{ rec_line_num(); return MOD; 		}
".."		{ rec_line_num(); return CONCAT; 	}
"<"			{ rec_line_num(); return LT;		}
"<="		{ rec_line_num(); return LE; 		}
">"			{ rec_line_num(); return GT; 		}
">="		{ rec_line_num(); return GE; 		}
"=="		{ rec_line_num(); return EQ; 		}
"and"		{ rec_line_num(); return AND; 		}
"or"		{ rec_line_num(); return OR; 		} 
"not" 		{ rec_line_num(); return NOT; 		} 

"."			{ rec_line_num(); return PERIOD;	}
"," 		{ rec_line_num(); return COMMA; 	} 
";"			{ rec_line_num(); return SEMIC; 	} 
"="			{ rec_line_num(); return ASSIGN; 	}
"..."		{ rec_line_num(); return ELIPSIS; 	}
"#"			{ rec_line_num(); return POUND; 	}

"("			{ rec_line_num(); return LPAREN;	}
")"			{ rec_line_num(); return RPAREN;	}
"["			{ rec_line_num(); return RSQRBRACK;	}
"]"			{ rec_line_num(); return LSQRBRACK;	}
"{"			{ rec_line_num(); return LCURLBRACK;}
"}"			{ rec_line_num(); return RCURLBRACK;}

{D}+ 					{ yylval.f.val = atof(yytext); yylval.f.line_num = yylineno; return INT; 	}
{D}+"."{D}* 			{ yylval.f.val = atof(yytext); yylval.f.line_num = yylineno; return FLOAT; 	} 
{D}+("."{D}+)?(E|e)-?{D}+ {
							yylval.f.val = atof(yytext); yylval.f.line_num = yylineno; return FLOAT; 	
						} 
\"							{
								BEGIN(doublequote);
								contents = "";
							}
<doublequote>[^\\\"]*\\.  	{ contents.append(yytext); }
<doublequote>[^\\\"]*\"		{
								contents.append(yytext, yytext+yyleng-1);
								BEGIN(INITIAL);
								yylval.s.str = new Name(contents, yylineno); 
								yylval.s.line_num = yylineno;
								return STRING;
							}
\'							{
								BEGIN(singlequote);
								contents = "";
							}
<singlequote>[^\'\\]*\\.	{ contents.append(yytext); }
<singlequote>[^\'\\]*\' 	{
                                contents.append(yytext, yytext+yyleng-1);
                                BEGIN(INITIAL);
                                yylval.s.str = new Name(contents, yylineno);
                                yylval.s.line_num = yylineno;
                                return STRING;
							}
"["[=]*"["				{
							BEGIN(multistring);
                            equals_count = 0;
							contents = "";
                            int i = 1;
                            while (yytext[i] == '=') { i++; equals_count++; }
						}
<multistring>[^\]]     	{
							contents.append(yytext);
                        }
<multistring>"]"       	{
							contents.append("]");
                        }
<multistring>"]"=*"]" 	{
							int eq_count = 0;
							int i = 1;	
							while (yytext[i++] == '=') eq_count++;
							if (eq_count == equals_count) 
							{
								yylval.s.str = new Name(contents, yylineno);
								yylval.s.line_num = yylineno;	
								BEGIN(INITIAL);
								return STRING;
							}
							else contents += yytext;
						}
[a-zA-Z_]{NAME}* 		{ 
							yylval.s.str = new Name(yytext, yylineno);	
							yylval.s.line_num = yylineno;
							return NAME; 
						}
[ \t\n]*				{ /* catch whitespace */ }
.						{ fprintf(stderr, "Uncaught character %s", yytext); }

%% 

void rec_line_num()
{
	yylval.line_num = yylineno;
}

void scan_error() 
{
	fprintf(stderr, "Scanner Error! -- line %d\n", yylineno);
	exit(EXIT_FAILURE);
}







