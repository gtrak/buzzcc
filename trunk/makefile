CCX = g++ 
CXXFLAGS = -Wall -g  
#CXXFLAGS = -O3 -march=core2 -s 
LIBS = -lfl -ly 
OBJECTS = main.o parse.o ast.o lex.yy.o symtable.o exp.o stmt.o ir.o
.PHONY: clean all 

all : luac

luac : $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@  $(LIBS)

%.o : %.cpp 
	$(CXX) $(CXXFLAGS) -c $< 

%.o : %.c 
	$(CXX) $(CXXFLAGS) -c $< 

%.c : %.y 
	bison -tdv $^ -o $@ 

lex.yy.c : scan.l 
	flex  $^

clean : 
	rm *.o luac parse.h parse.output lex.yy.c

