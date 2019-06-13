#! /usr/bin/python

# To change this template, choose Tools | Templates
# and open the template in the editor.

__author__="gary"
__date__ ="$Sep 2, 2009 1:26:58 PM$"


# ----------------------------------------------------------------------
# lua_lex.py
#
# A lexer for lua.
#
# Adapted from: http://www.juanjoconti.com.ar/files/python/ply-examples/ansic/clex.py
# ----------------------------------------------------------------------

import sys
import re
sys.path.insert(0,"../..")

import ply.lex as lex

# Reserved words, put stuff here to simplify regexes for keywords
reserved = (
    'DO','END','WHILE','REPEAT','UNTIL','IF','THEN',
    'ELSEIF','ELSE','FOR','IN','FUNCTION','LOCAL',
    'RETURN','BREAK','NIL','FALSE','TRUE',
    
    #Special Binops
    'AND','OR',
    
    #Special Unop
    'NOT'
    )

tokens = reserved + (
    # Literals (identifier, number, string)
    'ID', 'NUMBER', 'STRING',

    # Binary Operators (+,-,*,/,^,%, '..' ,<,<=,>,>=,==,~=)
    # defined elsewhere: (and, or)
    'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'EXP', 'MOD',
    'CONCAT', 'LT', 'LE', 'GT', 'GE', 'EQ', 'NE',

    # Unary Operators (#)
    # defined elsewhere: (-, not)
    'LENGTH',
    
    # Assignment (=)
    'EQUALS',
    

    # Delimeters ( ) [ ] { } , . ; :
    'LPAREN', 'RPAREN',
    'LBRACKET', 'RBRACKET',
    'LBRACE', 'RBRACE',
    'COMMA', 'PERIOD', 'SEMI', 'COLON',

    # Ellipsis (...)
    'ELLIPSIS',
    )

# Completely ignored characters
t_ignore           = ' \t\x0c'

# Newlines
def t_NEWLINE(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

# Binary Operators
t_PLUS             = r'\+'
t_MINUS            = r'-'
t_TIMES            = r'\*'
t_DIVIDE           = r'/'
t_EXP              = r'\^'
t_MOD              = r'%'
t_CONCAT           = r'\.\.'
t_LT               = r'<'
t_GT               = r'>'
t_LE               = r'<='
t_GE               = r'>='
t_EQ               = r'=='
t_NE               = r'!='

# Unary Operators
t_LENGTH           = r'\#'

# Assignment operators
t_EQUALS           = r'='

# Delimeters
t_LPAREN           = r'\('
t_RPAREN           = r'\)'
t_LBRACKET         = r'\['
t_RBRACKET         = r'\]'
t_LBRACE           = r'\{'
t_RBRACE           = r'\}'
t_COMMA            = r','
t_PERIOD           = r'\.'
t_SEMI             = r';'
t_COLON            = r':'
t_ELLIPSIS         = r'\.\.\.'

# Identifiers and reserved words

reserved_map = { }
for r in reserved:
    reserved_map[r.lower()] = r

def t_ID(t):
    r'[A-Za-z_][\w_]*'
    t.type = reserved_map.get(t.value,"ID")
    return t

# Number is just the definition for a float taken from below OR'd with my regex for int
t_NUMBER = r'(((\d+)(\.\d+)(e(\+|-)?(\d+))? | (\d+)e(\+|-)?(\d+))([lL]|[fF])? | \d+)'

# Integer literal
#t_ICONST = r'\d+([uU]|[lL]|[uU][lL]|[lL][uU])?'

# Floating literal
#t_FCONST = r'((\d+)(\.\d+)(e(\+|-)?(\d+))? | (\d+)e(\+|-)?(\d+))([lL]|[fF])?'



# String literal
t_STRING = r'[\"\']([^\\\n]|(\\.))*?[\'\"]'

# Character constant 'c' or L'c'
#t_CCONST = r'(L)?\'([^\\\n]|(\\.))*?\''

def t_error(t):
    print "Illegal character %s" % repr(t.value[0])
    t.lexer.skip(1)

###### COMMENTS
# Declare the state
states = (
  ('singlelinecomments','exclusive'),
  ('multilinecomments', 'exclusive'),
#  ('multilinestrings','exclusive'),
)

# Match the --. Enter comments state.
def t_singlelinecomments(t):
    r'--'
#    print 'matched single comment'
    t.lexer.code_start = t.lexer.lexpos        # Record the starting position
    t.lexer.level = 1                          # Initial comment level
    t.lexer.begin('singlelinecomments')        # Enter 'comments' state

# Rules for the comments state
def t_singlelinecomments_entermulti(t):     
    r'\[\['
#    print 'matched multi'
    t.lexer.begin('multilinecomments')

def t_singlelinecomments_voidmulti(t):     
    r'-\[\['
    t.lexer.begin('singlelinecomments')

def t_singlelinecomments_singleline(t):
    r'[^\n]+'
    pass

def t_singlelinecomments_NEWLINE(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")
    t.lexer.begin('INITIAL')

def t_singlelinecomments_error(t):
    print "Illegal character %s" % repr(t.value[0])
    t.lexer.skip(1)

def t_multilinecomments_end(t):
    r'--\]\]' 
    t.lexer.begin('INITIAL')
                   
def t_multilinecomments_commented(t):
    r'[^\n]+'
    pass

def t_multilinecomments_NEWLINE(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_multilinecomments_error(t):
    print "Illegal character %s" % repr(t.value[0])
    t.lexer.skip(1)

#######Multiline String
#def t_multilinestring(t):
#    r'[['
#    t.lexer.code_start = t.lexer.lexpos        # Record the starting position
#    t.lexer.string = ""                        # Initialize string
#    t.lexer.begin('multilinestring')           # Enter 'multiline string' state

#def t_multilinestring_exit(t):     
#    r'\]\]'
#    t.lexer.begin('INITIAL')

#def t_multilinestring_match(t):
#    r'[^\n]+'
#    t.lexer.string += t
    

#def t_multilinestring_NEWLINE(t):
#    r'[\n]+'
#    t.lexer.lineno += t.value.count("\n")
#    t.lexer.string += t


#def t_multilinestring_error(t):
#    print "Illegal character %s" % repr(t.value[0])
#    t.lexer.skip(1)



lexer = lex.lex(optimize=0,debug=0)

if __name__ == "__main__":
    lex.runmain(lexer)

# -----------------------------------------------------------------------------
# lua_parse.py
#
# Simple parser for lua.
# -----------------------------------------------------------------------------

import ply.yacc as yacc

# Set Precedence
precedence = (
    ('left', 'OR'),
    ('left', 'AND'),
    ('left', 'LT', 'GT', 'LE', 'GE', 'NE', 'EQ'),
    ('right', 'CONCAT'),
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE', 'MOD'),
    ('left', 'NOT', 'LENGTH'),
    ('right', 'UMINUS'),  #uminus right or left associative?
    ('right', 'EXP'),
)

# Define Class Structure for AST
class Node:
    def __init__(self,type,linenumber=None,children=None,leaf=None):
        self.type = type
        if children:
            self.children = children
        else:
            self.children = [ ]
        self.leaf = leaf
        if linenumber:
            self.linenumber = linenumber
                

# chunk ::= {stat [';']} [laststat [';']]
def p_chunk(p):
    '''chunk : stat chunk
             | stat SEMI chunk
             | stat
             | stat SEMI
             | laststat
             | laststat SEMI'''
    p[0] = ('chunk',p.lineno(1), tuple(p[1:]))

def p_block(p):
    'block : chunk'
    p[0] = ('block',p.lineno(1), tuple(p[1:]))

def p_Number(p):
    'Number : NUMBER'
    p[0] = ('Number', p.lineno(1), tuple(p[1:]))

def p_Name(p):
    'Name : ID'
    p[0] = ('Name', p.lineno(1), tuple(p[1:]))

#stat ::=  varlist '=' explist |
#                functioncall |
#                do block end |
#                while exp do block end |
#                repeat block until exp |
#                if exp then block {elseif exp then block} [else block] end |
#                for Name '=' exp ',' exp [',' exp] do block end |
#                for namelist in explist do block end |
#                function funcname funcbody |
#                local function Name funcbody |
#                local namelist ['=' explist]
def p_stat(p):
    '''stat : varlist EQUALS explist
            | functioncall
            | DO block END
            | WHILE exp DO block END
            | REPEAT block UNTIL exp
            | IF exp THEN block elseif_multiple END
            | IF exp THEN block elseif_multiple ELSE block END
            | FOR Name EQUALS exp COMMA exp DO block END
            | FOR Name EQUALS exp COMMA exp COMMA exp DO block END
            | FOR namelist IN explist DO block END
            | FUNCTION funcname funcbody
            | LOCAL FUNCTION Name funcbody
            | LOCAL namelist
            | LOCAL namelist EQUALS explist'''
    p[0] = ('stat',p.lineno(1), tuple(p[1:]))

def p_elseif_multiple(p):
    '''elseif_multiple : elseif_multiple ELSEIF exp THEN block
                       | ELSEIF exp THEN block
                       | empty'''
    print 'elseif_multiple'
    pass

# laststat ::= return [explist] | break
def p_laststat(p):
    '''laststat : RETURN
                | RETURN explist
                | BREAK'''
    p[0] = ('laststat',p.lineno(1), tuple(p[1:]))

# OO stuff was in this:
# funcname ::= Name {'.' Name} [':' Name]
def p_funcname(p):
    '''funcname : Name
                | Name PERIOD funcname'''
#    if len(p) == 3:
#        p[0] = ('funcname',p.lineno(1), (p[1], p[3]))
#    elif len(p) == 1:
#        p[0] = ('funcname',p.lineno(1), (p[1]))  
    p[0] = ('funcname',p.lineno(1), tuple(p[1:])) 

# varlist ::= var {',' var}
def p_varlist(p):
    '''varlist : var
               | var COMMA varlist'''
#    if len(p)==3:
#        p[0]= ('varlist',p.lineno(1),(p[1], p[3]))
#    elif len(p)==1:
#        p[0]= ('varlist',p.lineno(1),(p[1]))
    p[0]= ('varlist',p.lineno(1),tuple(p[1:]))

    
# var ::=  Name | prefixexp '[' exp ']' | prefixexp '.' Name
def p_var(p):
    '''var : Name
           | prefixexp LBRACKET exp RBRACKET 
           | prefixexp PERIOD Name'''
#    if len(p)==1:
#        p[0]= ('var',p.lineno(1),(p[1]))
#    elif len(p)==4:
#        p[0]= ('var',p.lineno(1),(p[1], p[3]))
#    elif len(p)==3:
#        p[0]= ('var',p.lineno(1),(p[1]), p[3])
    p[0]= ('var',p.lineno(1),tuple(p[1:]))

# namelist ::= Name {',' Name}
def p_namelist(p):
    '''namelist : Name
                | Name COMMA namelist'''
#    if len(p)==1:
#        p[0]= ('namelist',p.lineno(1))
#    elif len(p)==3:
#        p[0]= ('namelist',p.lineno(1),(p[3]))
    p[0]= ('namelist',p.lineno(1),tuple(p[1:]))

# explist ::= {exp ','} exp
def p_explist(p):
    '''explist : exp
               | exp COMMA explist'''
#    if len(p)==1:
#        p[0]= ('explist',p.lineno(1))
#    elif len(p)==3:
#        p[0]= ('explist',p.lineno(1),(p[3]))
    p[0]= ('explist',p.lineno(1),tuple(p[1:]))

# exp ::=  nil | false | true | Number | String | '...' | function |
#          prefixexp | tableconstructor | exp binop exp | unop exp
# I expanded out binops and unops later
def p_exp(p):
    '''exp : NIL 
           | FALSE 
           | TRUE 
           | Number
           | STRING 
           | ELLIPSIS 
           | FUNCTION
           | prefixexp 
           | tableconstructor 
           | binop 
           | unop'''
    p[0]= ('exp', p.lineno(1), (p[1]))

# prefixexp ::= var | functioncall | '(' exp ')'
def p_prefixexp(p):
    '''prefixexp : var 
                 | functioncall 
                 | LPAREN exp RPAREN'''
#    if len(p)==1:    
#        p[0]= ('prefixexp', p.lineno(1), (p[1]))
#    elif len(p)==3:
#        p[0]= ('prefixexp', p.lineno(1), (p[2]))
    p[0]= ('prefixexp', p.lineno(1), tuple(p[1:]))

# functioncall ::=  prefixexp args | prefixexp ':' Name args
def p_functioncall(p):
    'functioncall : prefixexp args'
    p[0]= ('functioncall', p.lineno(1), tuple(p[1:]))

# args ::=  '(' [explist] ')' | tableconstructor | String
def p_args(p):
    '''args : LPAREN RPAREN
            | LPAREN explist RPAREN
            | tableconstructor 
            | STRING'''
#    if len(p)==1:
#        p[0]= ('args', p.lineno(1), (p[1]))
#    elif len(p)==3:
#        p[0]= ('args', p.lineno(1), (p[2]))
    p[0]= ('args', p.lineno(1), tuple(p[1:]))

# Closure stuff?
# function ::= function funcbody
#
#def p_function(p):
#    'function : FUNCTION funcbody'
#    pass

# funcbody ::= '(' [parlist] ')' block end
def p_funcbody(p):
    '''funcbody : LPAREN RPAREN block END
                | LPAREN parlist RPAREN block END'''
#    if len(p)==4:
#        p[0]= ('funcbody', p.lineno(1), (p[3], p[4]))
#    elif len(p)==5:
#        p[0]= ('funcbody', p.lineno(1), (p[2], p[4], p[5]))
    p[0]= ('funcbody', p.lineno(1), tuple(p[1:]))


# parlist ::= namelist [',' '...'] | '...'
def p_parlist(p):
    '''parlist : namelist
               | namelist COMMA ELLIPSIS
               | ELLIPSIS'''
#    if len(p)==1:    
#        p[0]= ('parlist', p.lineno(1), (p[1]))
#    elif len(p)==3:
#        p[0]= ('parlist', p.lineno(1), [p[1], p[3]])
    p[0]= ('parlist', p.lineno(1), tuple(p[1:]))

# tableconstructor ::= '{' [fieldlist] '}'
def p_tableconstructor(p):
    '''tableconstructor : LBRACE RBRACE
                        | LBRACE fieldlist RBRACE'''
#    if len(p)==3:    
#        p[0]= ('tableconstructor', p.lineno(1), (p[2]))
    p[0]= ('tableconstructor', p.lineno(1), tuple(p[1:]))

# fieldlist ::= field {fieldsep field} [fieldsep]
def p_fieldlist(p):
    '''fieldlist : field
                 | field fieldsep fieldlist
                 | field fieldsep fieldlist fieldsep'''
#    if len(p)==1:
#        p[0]= ('fieldlist', p.lineno(1), (p[1]))
#    elif len(p)>=2:
#        p[0]= ('fieldlist', p.lineno(1), [p[1], p[3]])
    p[0]= ('fieldlist', p.lineno(1), tuple(p[1:]))

# field ::= '[' exp ']' '=' exp | Name '=' exp | exp
def p_field(p):
    '''field : LBRACKET exp RBRACKET EQUALS exp 
             | Name EQUALS exp
             | exp'''
#    if len(p)==5:
#        p[0]= ('funcbody', p.lineno(1), [p[2], p[5]])
#    elif len(p)==3:
#        p[0]= ('funcbody', p.lineno(1), [p[1], p[3]])
#    elif len(p)==1:
#        p[0]= ('funcbody', p.lineno(1), (p[1]))
    p[0]= ('funcbody', p.lineno(1), tuple(p[1:]))
    

# fieldsep ::= ',' | ';'
def p_fieldsep(p):
    '''fieldsep : COMMA 
                | SEMI'''
    p[0]=('fieldsep', p.lineno(1), tuple(p[1:]))

# binop ::= '+' | '-' | '*' | '/' | '^' | '%' | '..' |
#           '<' | '<=' | '>' | '>=' | '==' | '~=' |
#           and | or
def p_binop(p):
    '''binop : exp PLUS exp
             | exp MINUS exp
             | exp TIMES exp
             | exp DIVIDE exp
             | exp EXP exp
             | exp MOD exp
             | exp CONCAT exp
             | exp LT exp
             | exp LE exp
             | exp GT exp
             | exp GE exp
             | exp EQ exp
             | exp NE exp
             | exp AND exp
             | exp OR exp'''
    p[0]= ('binop', p.lineno(1), tuple(p[1:]))

# unop ::= '-' | not | '#'
def p_unop(p):
    '''unop : MINUS exp %prec UMINUS
            | NOT exp
            | LENGTH exp'''
#    p[0]= ('funcbody', p.lineno(1), (p[2]))
    p[0]= ('funcbody', p.lineno(1), (p[1:]))

def p_empty(p):
    'empty : '
    pass

def p_error(p):
    print "Syntax error at '%s'" % p.value


#import profile
# Build the grammar

yacc.yacc(method='LALR', debug='False')

###############################################################################
# Main
import sys
try:
    f = open(sys.argv[1])
    parse_output = yacc.parse(f.read(),debug=0)
    output=str(parse_output)
    #remove quotes
    output=output.replace("\'","")
    #remove commas
    output=output.replace(",","")
    print "\n\nSomewhat Simplified Concrete Syntax Tree:\n"
    print output

except EOFError:
    print "Could not open file %s." % sys.argv[1]

