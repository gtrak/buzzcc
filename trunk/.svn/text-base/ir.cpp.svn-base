#include "ir.h"
#include <sstream>
#include <cmath>
//#define DEBUG


using namespace std;

int PrintInt::whichstringconstant = 0;
int PrintFloat::whichstringconstant = 0;
extern FILE* yyout;

void add_nulls(std::string& ref, int num)
{
	for (int i = 0; i < num; i++)
	{
		ref.append("\\00");
	}
}

IRTree::IRTree()
{
	string nulls;
	add_nulls(nulls, 252);
	add_global(new Global(std::string("%d\\09\\00") + nulls, "ptr1000", 256));
	add_global(new Global(std::string("%s\\09\\00") + nulls, "ptr1001", 256));
	add_global(new Global(std::string("%f\\09\\00") + nulls, "ptr1002", 256));
	add_global(new Global(std::string("%d\\09\\00") + nulls, "ptr1004", 256));

	nulls.clear();
	add_nulls(nulls, 251);
	add_global(new Global(std::string("nil\\09\\00") + nulls, "ptr1003", 256));
	nulls.clear();
	add_nulls(nulls, 254);
	add_global(new Global(std::string("\n\\00") + nulls, "ptr1005", 256));
}

Sitofp::Sitofp(int dest_reg, int src_reg) : src(src_reg), dest(dest_reg) {}

void Sitofp::emit()
{
	fprintf(yyout, "%%r%d = sitofp i32 %%r%d to float\n", dest, src);
}

Fptosi::Fptosi(int dest_reg, int src_reg) : src(src_reg), dest(dest_reg) {}

void Fptosi::emit()
{
	fprintf(yyout, "%%r%d = fptosi float %%r%d to i32\n", dest, src);
}


Fpext::Fpext(int dest_reg, int src_reg) : src(src_reg), dest(dest_reg) { }

void Fpext::emit()
{
	fprintf(yyout, "%%r%d = fpext float %%r%d to double\n", dest, src);
}

//same as add global but user adds node
void IRBuilder::add_global_node(Global* node){

  Global* gl;
  gl = node;
  tree.add_global(gl);
}


void IRBuilder::add_global(SymbolInfo::llvm_type t, int nm) {
	std::string name("ptr");
	name.append(itos(nm));
	Global* gl;
	std::string contents;
	switch(t)
	{
	case SymbolInfo::LL_STRING:
		add_nulls(contents, 256);
		gl = new Global(contents,name,256);
		break;
	case SymbolInfo::LL_INTEGER:
		gl = new Global(0,name);
		break;
	case SymbolInfo::LL_FLOAT:
		gl = new Global(0.0f,name);
		break;
	case SymbolInfo::LL_BOOL:
		gl = new Global((bool)false,name);
		break;
	case SymbolInfo::LL_NIL:
		break;
    case SymbolInfo::LL_TABLE:
        add_nulls(contents,256);
        gl = new Global(contents,name,256);
        break;
	default: fprintf(stderr, "Unknown global type.\n"); exit(EXIT_FAILURE); break;
	}
	tree.add_global(gl);
}

llvmtype convert(SymbolInfo::llvm_type t) {
    switch (t) {
        case SymbolInfo::LL_BOOL:
			return ARG_BOOL;
            break;
        case SymbolInfo::LL_FLOAT:
            return ARG_FLOAT;
            break;
        case SymbolInfo::LL_INTEGER:
            return ARG_INT;
            break;
        case SymbolInfo::LL_STRING:
            return ARG_STRING;
            break;
		case SymbolInfo::LL_NIL:
			return ARG_BOOL;
			break;
		case SymbolInfo::NO_TYPE:
			return ARG_VOID;
			break;
		case SymbolInfo::LL_VOID:
			return ARG_VOID;
			break;
        case SymbolInfo::LL_TABLE:
            return ARG_TABLE;
        default:
            break;
    }
    abort();
}

void BinopInt::emit()
{
    //Initialize the string to store the function
    char fxn[16]= "change me";

    switch (binoptype)
    {
    case OP_ADD:
        strcpy(fxn,"add i32");
        break;
    case OP_SUB:
        strcpy(fxn,"sub i32");
        break;
    case OP_MUL:
        strcpy(fxn,"mul i32");
        break;
    case OP_DIV:
        strcpy(fxn,"sdiv i32");
        break;
    case OP_MOD:
        strcpy(fxn,"srem i32");
        break;
    case OP_SHL:
        strcpy(fxn,"shl i32");

        break;
    case OP_SHR:
        strcpy(fxn,"ashr i32");
        break;

        //Put something here?
    case OP_CONCAT:
        printf("No, don't use concat in binops");
        abort();
        break;

        //<result> = icmp <cond> <ty> <op1>, <op2>   ; yields {i1} or {<N x i1>}:result
    case OP_LT:
        strcpy(fxn,"icmp slt i32");
        break;
    case OP_GT:
        strcpy(fxn,"icmp sgt i32");
        break;
    case OP_LE:
        strcpy(fxn,"icmp sle i32");
        break;
    case OP_GE:
        strcpy(fxn,"icmp sge i32");
        break;
    case OP_EQ:
        strcpy(fxn,"icmp eq i32");
        break;
    case OP_NE:
        strcpy(fxn,"icmp ne i32");
        break;
    default:
        printf("incorrect op in binopint::emit");
        abort();
    }

    int temparg1 = arg1;
    int temparg2 = arg2;


  char arg1prefix[16];
    char arg2prefix[16];

    if(arg1>=0){
    //Will need to output to a file eventually
    //Default to reg on reg action
        strcpy(arg1prefix,"%r");
    }
    else{
        strcpy(arg1prefix,"%opt");
        temparg1= (-temparg1);
    }

    if(arg2>=0){
        strcpy(arg2prefix,"%r");
    }
    else{
        strcpy(arg2prefix,"%opt");
        temparg2 = (-temparg2);
    }



    //Change each to ARG_IMM as needed, maybe also memory
    if (isarg1reg==ARG_IMM)
    {
        strcpy(arg1prefix,"");
    }
    if (isarg2reg==ARG_IMM)
    {
        strcpy(arg2prefix,"");
    }


   if(result>=0){

        fprintf(yyout,"%%r%d = %s %s%d, %s%d \n",result, fxn, arg1prefix, temparg1, arg2prefix, temparg2);
   }
   else{
        fprintf(yyout,"%%opt%d = %s %s%d, %s%d \n",-result, fxn, arg1prefix, temparg1, arg2prefix, temparg2);
   }
}

void BinopFloat::emit()
{
    //Initialize the string to store the function
    char fxn[16]= "change me";

    switch (binoptype)
    {
    case OP_ADD:
        strcpy(fxn,"add float");
        break;
    case OP_SUB:
        strcpy(fxn,"sub float");
        break;
    case OP_MUL:
        strcpy(fxn,"mul float");
        break;
    case OP_DIV:
        strcpy(fxn,"div float");
        break;
    case OP_MOD:
        strcpy(fxn,"rem float");
        break;


        //Put something here?
    case OP_CONCAT:
        printf("No, don't use concat in binops");
        abort();
        break;

        //<result> = icmp <cond> <ty> <op1>, <op2>   ; yields {i1} or {<N x i1>}:result
    case OP_LT:
        strcpy(fxn,"fcmp olt float");
        break;
    case OP_GT:
        strcpy(fxn,"fcmp ogt float");
        break;
    case OP_LE:
        strcpy(fxn,"fcmp ole float");
        break;
    case OP_GE:
        strcpy(fxn,"fcmp oge float");
        break;
    case OP_EQ:
        strcpy(fxn,"fcmp oeq float");
        break;
    case OP_NE:
        strcpy(fxn,"fcmp one float");
        break;
    default:
        printf("incorrect op in binopfloat::emit");
        abort();

    }

    //Will need to output to a file eventually
    char arg1prefix[4]="";
    char arg2prefix[4]="";

    //Keeps track, so we can generate appropriate string
    int whichisreg=0;

    //Change each to ARG_IMM as needed, maybe also memory
    if (isarg1reg==ARG_REG)
    {
        strcpy(arg1prefix,"%r");
        whichisreg=whichisreg+1;
    }
    if (isarg2reg==ARG_REG)
    {
        strcpy(arg2prefix,"%r");
        whichisreg=whichisreg+2;
    }

    switch (whichisreg)
    {
    case 0:
       fprintf(yyout,"%%r%d = %s %s%f, %s%f \n",result, fxn, arg1prefix, arg1, arg2prefix, arg2);
        break;
    case 1:
       fprintf(yyout,"%%r%d = %s %s%d, %s%f \n",result, fxn, arg1prefix, (int)arg1, arg2prefix, arg2);
        break;
    case 2:
       fprintf(yyout,"%%r%d = %s %s%f, %s%d \n",result, fxn, arg1prefix, arg1, arg2prefix, (int)arg2);
        break;
    case 3:
       fprintf(yyout,"%%r%d = %s %s%d, %s%d \n",result, fxn, arg1prefix, (int)arg1, arg2prefix, (int)arg2);
        break;
    }
}

void BinopBool::emit()
{
    //Initialize the string to store the function
    char fxn[16]= "change me";

    switch (binoptype)
    {
    case OP_AND:
        strcpy(fxn,"and");
        break;
    case OP_OR:
        strcpy(fxn,"or");
        break;
    case OP_XOR:
        strcpy(fxn,"xor");
        break;
    default:
        printf("Binoptype must be OP_AND, OP_OR, or OP_XOR \n");
        abort();
    }






    //Will need to output to a file eventually
    char arg1prefix[8]="";
    char arg2prefix[8]="";

    //Keeps track, so we can generate appropriate string

    //Change each to ARG_IMM as needed, maybe also memory
    if (isarg1reg==ARG_REG)
    {
        if(arg1>=0){
            strcpy(arg1prefix,"%r");
        }
        else{
            strcpy(arg1prefix,"%opt");
        }
    }
    if (isarg2reg==ARG_REG)
    {
        if(arg2>=0){
            strcpy(arg2prefix,"%r");
        }
        else{
            strcpy(arg2prefix,"%opt");
        }
    }

    if(result>=0){
        fprintf(yyout,"%%r%d = %s i32 %s%d, %s%d \n",result, fxn, arg1prefix, arg1, arg2prefix, arg2);
    }
    else{
        fprintf(yyout,"%%opt%d = %s i32 %s%d, %s%d \n",-result, fxn, arg1prefix, arg1, arg2prefix, arg2);
    }

}

void PrintInt::emit()
{
    //looks like
    //%0 = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str, i32 0, i32 0), i32 %a) nounwind		; <i32> [#uses=0]
    //
    if (whichstringconstant==0)
    {
       fprintf(yyout,"you're a dummy, do emitstringconstant first\n");
        return;
    }
    switch (isargreg)
    {
    case ARG_IMM:
       fprintf(yyout,"%%r%d = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str%d, i32 0, i32 0), i32 %d) nounwind \n",firstresult,whichstringconstant,arg);
        break;
    case ARG_REG:
       fprintf(yyout,"%%r%d = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str%d, i32 0, i32 0), i32 %%r%d) nounwind \n",firstresult,whichstringconstant,arg);
        break;
    default:
       fprintf(yyout,"specify ARG_REG or ARG_IMM in the instantiation");
        break;
    }
}

void PrintFloat::emit()
{
    // Make sure it's initialized
    if (whichstringconstant==0)
    {
       fprintf(yyout,"you're a dummy, do emitstringconstant first\n");
        return;
    }
    //looks like
    //%0 = fpext float %a to double		; <double> [#uses=8]
    //%1 = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str, i32 0, i32 0), double %0) nounwind


    // Pulls in a register, or converts it to a float value
    switch (isargreg)
    {
    case ARG_IMM:
       fprintf(yyout,"%%r%d = fpext float %.14g to double \n",firstresult,arg);
       fprintf(yyout,"%%r%d = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str%d, i32 0, i32 0), double %%r%d) nounwind \n",firstresult+1,whichstringconstant,firstresult);
        break;
    case ARG_REG:
       fprintf(yyout,"%%r%d = fpext float %%r%d to double \n",firstresult,(int)arg);
       fprintf(yyout,"%%r%d = tail call i32 (i8*, ...)* @printf(i8* noalias getelementptr ([4 x i8]* @.str%d, i32 0, i32 0), double %%r%d) nounwind \n",firstresult+1,whichstringconstant,firstresult);
        break;
    default:
       fprintf(yyout,"specify ARG_REG or ARG_IMM in the instantiation");
        break;
    }
}

void DefineFunction::emitpre()
{
    //define i32 @stfu(i32 %b) nounwind {
    //entry:

   fprintf(yyout,"define %s @%s(",  returntypestring, funcname.c_str());

    std::vector<llvmtype>::const_iterator i;
    for (i=argtypes->begin(); (i<argtypes->end());i++)
    {
        char outstr[8];

        switch (*i)
        {
        case ARG_INT:
            strcpy(outstr,intstring);
            break;
        case ARG_FLOAT:
            strcpy(outstr,floatstring);
            break;
        case ARG_STRING:
            strcpy(outstr,stringstring);
            break;
		default: abort();
        }

       fprintf(yyout,"%s", outstr);

        // Comma separator except last
        if (i<argtypes->end())
        {
           fprintf(yyout,",");
        }
    }

   fprintf(yyout,") nounwind { \n");
   fprintf(yyout,"entry: \n");

}

void  Alloc::emit()
{
    //%ptr = alloca i32, i32 4
    char intstring[8]="i32";
    char floatstring[8]="float";
    char stringstring[8]="i8";
	char boolstring[8]="i1";
    char outstring[8];

    switch (thetype)
    {
    case ARG_INT:
        strcpy(outstring,intstring);
        break;
    case ARG_FLOAT:
        strcpy(outstring,floatstring);
        break;
    case ARG_STRING:
        strcpy(outstring,stringstring);
        break;
	case ARG_BOOL:
		strcpy(outstring,boolstring);
		break;
	default: abort();
    }

    // In order to handle our optimizer-generated registers in a possible way, use a different
    // prefix %opt# instead of %r#
    if (theptr>=0){
        switch (isarray)
        {
        case false:
            fprintf(yyout,"%%r%d = alloca %s \n",theptr,outstring);
            break;
        case true:
            fprintf(yyout,"%%r%d = alloca [%d x %s]  \n",theptr,length, outstring);
            break;
        }
    } else {
        switch (isarray)
        {
        case false:
            fprintf(yyout,"%%opt%d = alloca %s \n",(0-theptr),outstring);
            break;
        case true:
            fprintf(yyout,"%%opt%d = alloca [%d x %s]  \n",(0-theptr),length, outstring);
            break;
        }
    }


}

void Loadop::emit()
{
    char intstring[16]="i32";
    char floatstring[16]="float";
    char stringstring[16]="[256 x i8]";
    char boolstring[16]="i1";
    char outstring[16];

    const char localprefix[8]="%r";
    const char globalprefix[8]="@ptr";
    char prefix[8];

    switch (thetype)
    {
    case ARG_INT:
        strcpy(outstring,intstring);
        break;
    case ARG_FLOAT:
        strcpy(outstring,floatstring);
        break;
    case ARG_STRING:
        strcpy(outstring,stringstring);
        break;
    case ARG_BOOL:
        strcpy(outstring, boolstring);
        break;
    default:
        printf("incorrect type in loadop");
        abort();
    }

    // Once again, for our special optimizer generated pointers
    if (theptr>=0){
        switch (isptrglobal)
        {
        case true:
            strcpy(prefix,globalprefix);
            break;
        case false:
            strcpy(prefix,localprefix);
            break;
        }

        fprintf(yyout,"%%r%d = load %s * %s%d \n",resultreg,outstring, prefix, theptr);
    } else{
        strcpy(prefix,"%opt");
        fprintf(yyout,"%%r%d = load %s * %s%d \n",resultreg,outstring, prefix, (0-theptr));
    }


}

void Storeop::emit()
{
    const char intstring[12]="i32";
    const char floatstring[12]="float";
    const char stringstring[12]="[256 x i8]";
    const char boolstring[12]="i1";


    char outstring[12];

    const char localprefix[8]="%r";
    const char globalprefix[8]="@ptr";
    char prefix[8];

    switch (thetype)
    {
    case ARG_INT:
        strcpy(outstring,intstring);
        break;
    case ARG_FLOAT:
        strcpy(outstring,floatstring);
        break;
    case ARG_STRING:
        strcpy(outstring,stringstring);
        break;
    case ARG_BOOL:
        strcpy(outstring,boolstring);
        break;
    default:
        abort();
    }

    switch (isptrglobal)
    {
    case true:
        strcpy(prefix,globalprefix);
        break;
    case false:
        strcpy(prefix,localprefix);
        break;
    }

    switch (argkind){
        case ARG_REG:
            fprintf(yyout,"store %s %%r%d, %s* %s%d \n",outstring, arg.reg, outstring, prefix, theptr);
            break;
        case ARG_IMM:
        {
            if (theptr>=0){
                switch (thetype){
                    case ARG_BOOL:
                        fprintf(yyout,"store %s %d, %s* %s%d \n",outstring, arg.b, outstring, prefix, theptr);
                        break;
                    case ARG_INT:
                        fprintf(yyout,"store %s %d, %s* %s%d \n",outstring, arg.i, outstring, prefix, theptr);
                        break;
                    case ARG_FLOAT:
                        fprintf(yyout,"store %s %f, %s* %s%d \n",outstring, arg.f, outstring, prefix, theptr);
                        break;
                    default:
                        printf("wrong type in storeop");
                        abort();
                }
            } else {
                strcpy(prefix,"%opt");
                switch (thetype){
                    case ARG_BOOL:
                        fprintf(yyout,"store %s %d, %s* %s%d \n",outstring, arg.b, outstring, prefix, (0-theptr));
                        break;
                    case ARG_INT:
                        fprintf(yyout,"store %s %d, %s* %s%d \n",outstring, arg.i, outstring, prefix, (0-theptr));
                        break;
                    case ARG_FLOAT:
                        fprintf(yyout,"store %s %f, %s* %s%d \n",outstring, arg.f, outstring, prefix, (0-theptr));
                        break;
                    default:
                        printf("wrong type in storeop");
                        abort();
                }
            }
        }
        default:
            break;
    }
}

void GetPointer::emit()
{
    //i8* noalias getelementptr ([4 x i8]* @.str, i32 0, i32 0)
    char intstring[12]="i32 *";
    char floatstring[12]="float *";
    char boolstring[12]="i1 *";
    char stringstring[12]="[256 x i8]*";//"i8 *";

    switch (thetype)
    {
    case ARG_INT:
        fprintf(yyout,"%%r%d = getelementptr %s @ptr%d, i32 %d\n",outputregister,intstring, globalptr, offset);
        break;
    case ARG_FLOAT:
        fprintf(yyout,"%%r%d = getelementptr %s @ptr%d, i32 %d\n",outputregister,floatstring, globalptr, offset);
        break;
    case ARG_BOOL:
        fprintf(yyout,"%%r%d = getelementptr %s @ptr%d, i32 %d\n",outputregister,boolstring, globalptr, offset);
        break;
    case ARG_STRING:
        fprintf(yyout,"%%r%d = getelementptr %s @ptr%d, i32 %d\n",outputregister,stringstring, globalptr, offset);
        break;
    default:
        printf("incorrect use of getpointer, emission failed");
        abort();
    }
}

void CallFunction::emit()
{
    char intstring[]="i32";
    char floatstring[]="float";
    char stringstring[]="[256 x i8]*";
    char boolstring[]="i1";
    char llvmstringstring[]="[256 x i8]";
	char voidstring[] = "void";
	char doublestring[] = "double";
	bool voidt = false;
        // Do Bool, do llvm style strings
    switch (returntype){
        case ARG_INT:
            strcpy(returntypestring,intstring);
            break;
        case ARG_FLOAT:
            strcpy(returntypestring,floatstring);
            break;
        case ARG_STRING:
            if (cstyle==true){
                strcpy(returntypestring,stringstring);
            }
            else {
                strcpy(returntypestring,llvmstringstring);
            }
            break;
        case ARG_BOOL:
            strcpy(returntypestring, boolstring);
            break;
		case ARG_VOID:
			voidt = true;
			strcpy(returntypestring, voidstring);
			break;
        case ARG_TABLE:
           if (cstyle==true){
                strcpy(returntypestring,stringstring);
            }
            else {
                strcpy(returntypestring,llvmstringstring);
            }
            break;
        default:
            printf("incorrect return type in callfunction::emit");
            abort();
    }

	//printf("argtype %p regs %p\n", argtypes, argregs);
	//printf("type %d\n", (*argtypes)[0]);

	char functionprefix[256]="";

    if (cstyle==true){
        switch (thecfunction){
            case CPRINTF:
                strcpy(functionprefix,"([256 x i8]*,...)*");
                break;
            case CSTRCAT:
                break;
			case CPOW:
				break;
            default:
                printf("wrong c-style function call function");
                abort();
        }
    }


    if (!voidt) fprintf(yyout,"%%r%d = call %s %s @%s(", outregister, returntypestring, functionprefix, funcname.c_str());
    else fprintf(yyout,"call %s %s @%s(", returntypestring, functionprefix, funcname.c_str());


    for (unsigned int i=0; (i<(argtypes->size()));i++)
    {
        char outstr[512];

        switch ((*argtypes)[i])
        {
        case ARG_INT:
            strcpy(outstr,intstring);
            break;
        case ARG_FLOAT:
            strcpy(outstr,floatstring);
            break;
        case ARG_STRING:
            if (cstyle==true){
                strcpy(outstr,stringstring);
            }
            else {
                strcpy(outstr,llvmstringstring);
            }
            break;
        case ARG_BOOL:
            strcpy(outstr,boolstring);
            break;
		case ARG_DOUBLE:
			strcpy(outstr, doublestring);
			break;
        default:
			//printf("type %d\n", (*argtypes)[i]);
            printf("incorrect type in std::vector argtypes in callfunction::emit");
            abort();
        }

       fprintf(yyout,"%s %%r%d", outstr, (*argregs)[i]);

        // Comma separator except last
        if (((i+1)<(argtypes->size())) && ((argtypes->size())!=1))
        {
           fprintf(yyout,",");
        }
    }

   fprintf(yyout,") nounwind  \n");
    return;
}



string itos(int i)
{
    stringstream sstream(stringstream::out);
    sstream << i;
    return sstream.str();
}

void print_type(SymbolInfo::llvm_type t)
{
    switch (t)
    {
    case SymbolInfo::LL_BOOL:
        fprintf(yyout, "i1");
        break;
    case SymbolInfo::LL_STRING:
        fprintf(yyout, "[256 x i8]");
        break;
    case SymbolInfo::LL_FLOAT:
        fprintf(yyout, "float");
        break;
    case SymbolInfo::LL_INTEGER:
        fprintf(yyout, "i32");
        break;
    case SymbolInfo::LL_TABLE:
        fprintf(stderr, "Unrecognized type in function argument.");
        exit(EXIT_FAILURE);
        break;
    case SymbolInfo::LL_NIL:
        fprintf(stderr, "NIL type in function argument.");
        exit(EXIT_FAILURE);
        break;
	case SymbolInfo::LL_VOID:
	    fprintf(yyout, "void");
        break;
	case SymbolInfo::NO_TYPE:
		fprintf(yyout, "void");
		break;
    case SymbolInfo::LL_UNKNOWN:
        fprintf(stderr, "UNKNOWN type in function argument.");
        exit(EXIT_FAILURE);
        break;
    default:
        fprintf(stderr, "Unrecognized type in function argument.");
        exit(EXIT_FAILURE);
        break;
    }
}

void Global::print()
{
	switch(t)
	{
	case SymbolInfo::LL_BOOL:
        fprintf(yyout, "@%s = global i1 %d\n" , name.c_str(), content.bl);
        break;
    case SymbolInfo::LL_STRING:
		fprintf(yyout, "@%s = global [%d x i8] c\"%s\"\n", name.c_str(), size, str.c_str());
        break;
    case SymbolInfo::LL_FLOAT:
        fprintf(yyout, "@%s = global float %f\n" , name.c_str(), content.fp);
        break;
    case SymbolInfo::LL_INTEGER:
        fprintf(yyout, "@%s = global i32 %d\n" , name.c_str(), content.i32);
        break;
	case SymbolInfo::LL_NIL:
	    fprintf(yyout, "@%s = global i1 %d\n" , name.c_str(), content.bl);
		break;
	default:
		fprintf(stderr, "Trying to print unknown type.\n");
		exit(EXIT_FAILURE);
		break;
	}
}

void Arg::print()
{
    print_type(type);
    fprintf(yyout, " %%%s", name.c_str());
}

void BasicBlock::print()
{
    fprintf(yyout, "%s:\n", label.c_str());
    for (unsigned int i = 0; i < size(); i++)
    {
        fputs("\t", yyout); operator[](i)->emit();
    }
}

void IRFunction::print()
{
    fprintf(yyout, "define ");
    print_type(rettype);
    fprintf(yyout, " @%s(", name.c_str());
    for (unsigned int i = 0; i < args.size(); i++)
    {
        args[i]->print();
        if (i != (args.size()-1)) fprintf(yyout, ", ");
    }
    fprintf(yyout, ") {\n");

    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        blocks[i]->print();
    }
    fprintf(yyout, "\n}\n");
}

void IRFunction::print_decl()
{
    fprintf(yyout, "declare");
    print_type(rettype);
    fprintf(yyout, " @%s(", name.c_str());
    for (unsigned int i = 0; i < args.size(); i++)
    {
        args[i]->print();
        if (i != (args.size()-1))fprintf(yyout, ", ");
    }
    fprintf(yyout, ") \n");
}

void IRTree::print_assembly()
{
    //fprintf(yyout, "target triple = \"i386-pc-gnu-linux\"\n");
    for (unsigned int i = 0; i < globals.size(); i++)
    {
        globals[i]->print();
    }
	IRFunction* main = functions[0];
	functions[0] = functions[functions.size()-1];
	functions[functions.size()-1] = main;
    for (unsigned int i = 0; i < functions.size(); i++)
    {
        functions[i]->print();
    }
    fprintf(yyout, "declare i32 @printf(i8* noalias, ...)\n");
	fprintf(yyout, "declare i8* @strcat(i8* , i8* )\n");
	fprintf(yyout, "declare float @powf(float, float)\n");
}


bool IROptimizer::dead_code_elimination(IRFunction* irfunc){

    setDefiningInstructions(irfunc);


    for(BlockList::iterator bb=irfunc->blocks.begin(); bb!=irfunc->blocks.end(); bb++){
        for(Inslist::iterator itr = (*bb)->begin(); itr != (( (*bb)->end() )); itr++){
            Instruction &instr = **itr;

            if(instr.iscritical()){
               visitUD(*itr);
            }
        }
    }

    return deleteUnvisited(irfunc);

}


void IROptimizer::strengthReduceMul(BasicBlock* bb, BinopInt* instr, int argtoreduce){   // argtoreduce is either 1 or 2, as to whether we shoudl reduce arg 1 or we should reduce arg 2
    if(argtoreduce!=1 && argtoreduce!=2){
        printf("Error: Multiply argument number should be 1 or 2 (because it takes 2 arguments)\n");
        abort();
    }

    int shiftdir=0;  // positive is shift right, negative is shift left
    int shiftamount=0; // 0 is no shift
    int addamount=0;   //0 is no add
    int addorsub=0; //positive is add, negative is subtract
    typedef std::vector<Instruction*> Inslist;
    double lowbound;
    double upbound;

    //Figure out what to do........
    if(argtoreduce==1){
    //use arg1
    lowbound = int(log2(instr->arg1));
    upbound = lowbound+1;

        if(pow(2,lowbound)==instr->arg1){
        //Is exact shift, no add
            shiftdir=-1;
            shiftamount=lowbound;
            addamount=0;
            addorsub=0;


        }
        else{
            int lowdiff = instr->arg1-pow(2,lowbound);
            int updiff = pow(2,upbound)-instr->arg1;
            if(lowdiff<updiff){
                shiftdir=-1;
                shiftamount=lowbound;
                addamount = lowdiff;
                addorsub = 1;
            }
            else{
                shiftdir=-1;
                shiftamount = upbound;
                addamount = updiff;
                addorsub = -1;
            }
        }

    }
    else if(argtoreduce==2){
    //use arg2
     lowbound = int(log2(instr->arg2));
    upbound = lowbound+1;

        if(pow(2,lowbound)==instr->arg2){
        //Is exact shift, no add
            shiftdir=-1;
            shiftamount=lowbound;
            addamount=0;
            addorsub=0;


        }
        else{
            int lowdiff = instr->arg2-pow(2,lowbound);
            int updiff = pow(2,upbound)-instr->arg2;
            if(lowdiff<updiff){
                shiftdir=-1;
                shiftamount=lowbound;
                addamount = lowdiff;
                addorsub = 1;
            }
            else{
                shiftdir=-1;
                shiftamount = upbound;
                addamount = updiff;
                addorsub = -1;
            }
        }

    }








        Inslist::iterator start_itr = (bb)->begin();
        for (int i=0; start_itr+i != (( (bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            //printf("%d",i);
            start_itr = (bb)->begin();
            Inslist::iterator itr = start_itr+i;






 //   for(Inslist::iterator itr=bb->begin(); itr!=(( bb->end() )); itr++){
        Instruction* instrpointer = *itr;
        if(instrpointer==instr){
                //Add Addition (if necessary)
                int newregister = getregister();
                Instruction* themult;
                Instruction* theadd;
                Instruction* theshift;
                int addreg = getregister();
                if(addorsub!=0){
                if(argtoreduce==1){
                    themult = new BinopInt(addreg,OP_MUL,addamount,instr->arg2,ARG_IMM,instr->isarg2reg);
                }
                else if(argtoreduce==2){
                    themult = new BinopInt(addreg,OP_MUL,addamount,instr->arg1,ARG_IMM,instr->isarg1reg);
                }
                else{
                    printf("ERROR, INVALID ARGUMENT TO REDUCE IN STRENGTH REDUCTION!\n");
                    abort();
                }
                }

                if(addorsub>0){

                    theadd = new BinopInt(instrpointer->assigned_register(),OP_ADD,newregister,addreg,ARG_REG, ARG_REG);
                }
                else if(addorsub<0){
                    theadd = new BinopInt(instrpointer->assigned_register(),OP_SUB,newregister,addreg,ARG_REG,ARG_REG);   //DONT FORGET TO MODIFY EMITS!
                }
                else{
                    //Do nothing
                }
                if(addorsub!=0){

                    itr = bb->insert(itr,theadd);
                    itr= bb->insert(itr,themult);
                }




                //Add Shift
                int destinationregister;
                if(addorsub!=0){
                    //if we will be adding, then shift into a temp register, else, into final register
                    destinationregister = newregister;

                }
                else{
                    destinationregister = instrpointer->assigned_register();
                }

                if(shiftdir>0){
                    //shift right
                    if(argtoreduce==1){  //reduce arg 1, apply on 2
                        theshift = new BinopInt(destinationregister, OP_SHR, instr->arg2,shiftamount,instr->isarg2reg,ARG_IMM);
                    }
                    else if(argtoreduce==2){
                        theshift = new BinopInt(destinationregister, OP_SHR, instr->arg1,shiftamount, instr->isarg1reg,ARG_IMM);
                    }
                    else{
                        printf("Error: unspecified arg to reduce!");
                        abort();

                    }

                }
                else if(shiftdir<0){
                     //shift left
                      if(argtoreduce==1){  //reduce arg 1, apply on 2
                        theshift = new BinopInt(destinationregister, OP_SHL, instr->arg2,shiftamount,instr->isarg2reg,ARG_IMM);
                    }
                    else if(argtoreduce==2){
                        theshift = new BinopInt(destinationregister, OP_SHL, instr->arg1,shiftamount, instr->isarg1reg,ARG_IMM);
                    }
                    else{
                        printf("Error: unspecified arg to reduce!");
                        abort();

                    }
                }
                else{
                    //printf("ERROR! NO SHIFT SPECIFIED!");
                    //abort();
                }

                itr = bb->insert(itr,theshift);



                //Move iterator +2
                itr = itr+1;
                if(addorsub!=0){
                    itr=itr+2;
                }
                //Remove Old Mul
                bb->erase(itr);
        }

    }
}
void IROptimizer::strengthReduceDiv(BasicBlock* bb, BinopInt* instr){

    int shiftdir=0;  // positive is shift right, negative is shift left
    int shiftamount=0; // 0 is no shift
    int addamount=0;   //0 is no add
    int addorsub=0; //positive is add, negative is subtract

    double lowbound;
  //  double upbound;

    //Figure out what to do........

    //use arg1
    lowbound = int(log2(instr->arg2));


        if(pow(2,lowbound)==instr->arg2){
        //Is exact shift, no add
            shiftdir=1;
            shiftamount=lowbound;
            addamount=0;
            addorsub=0;

        }

       /* else{
            int lowdiff = instr->arg2-pow(2,lowbound);
            int updiff = pow(2,upbound)-instr->arg2;
            if(lowdiff<updiff){
                shiftdir=1;
                shiftamount=lowbound;
                addamount = lowdiff;
                addorsub = -1;
            }
            else{
                shiftdir=1;
                shiftamount = upbound;
                addamount = updiff;
                addorsub = 1;
            }
        }*/









        Inslist::iterator start_itr = (bb)->begin();
        for (int i=0; start_itr+i != (( (bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            //printf("%d",i);
            start_itr = (bb)->begin();
            Inslist::iterator itr = start_itr+i;




   // for(Inslist::iterator itr=bb->begin(); itr!=(( bb->end() )); itr++){
        Instruction* instrpointer = *itr;
        if(instrpointer==instr){
                //Add Addition (if necessary)

                Instruction* theshift;

                             //Add Shift
                int destinationregister;

                    destinationregister = instrpointer->assigned_register();


                if(shiftdir>0){

                        theshift = new BinopInt(destinationregister, OP_SHR, instr->arg1,shiftamount, instr->isarg1reg,ARG_IMM);


                }
                else if(shiftdir<0){
                     //shift left

                        theshift = new BinopInt(destinationregister, OP_SHL, instr->arg1,shiftamount, instr->isarg1reg,ARG_IMM);

                }
                else{
                  //  printf("ERROR! NO SHIFT SPECIFIED!");
                   // abort();
                }

                if(shiftdir!=0){

                    itr = bb->insert(itr,theshift);

                    //Move iterator +2
                    itr = itr+1;

                //Remove Old Mul
                bb->erase(itr);
                }
        }

    }
}
void IROptimizer::strengthReduceMod(BasicBlock* bb, BinopInt* instr){

//Have to finish Mod reduction
//have to modify emit on Binop to account for -ve registers (the ones we add)
//have to test

   //use arg1
    int lowbound = int(log2(instr->arg2));
    int mask;
    Instruction* theand;
    if(pow(2,lowbound)==instr->arg2){
          mask = pow(2,lowbound)-1;


    }
    else{
        return;
    }




        Inslist::iterator start_itr = (bb)->begin();
        for (int i=0; start_itr+i != (( (bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            //printf("%d",i);
            start_itr = (bb)->begin();
            Inslist::iterator itr = start_itr+i;


//    for(Inslist::iterator itr=bb->begin(); itr!=(( bb->end() )); itr++){


        Instruction* instrpointer = *itr;
        if(instrpointer==instr){
         theand = new BinopBool(instr->assigned_register(),OP_AND,instr->arg1,mask,instr->isarg1reg,ARG_IMM);

        itr=bb->insert(itr,theand);
        itr=itr+1;
        bb->erase(itr);
        break;
        }
    }


}

bool IROptimizer::strength_reduction(IRFunction* irfunc){
    bool returnval=false;
    //Parse through all the instructions looking for Multiply, Divide, or Remainder operations

   // for(BlockList::iterator bb=irfunc->blocks.begin(); bb!=irfunc->blocks.end();bb++){
    //    for(Inslist::iterator itr=(*bb)->begin(); itr!=(( (*bb)->end() )); itr++){


 for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
        Inslist::iterator start_itr = (*bb)->begin();
        for (int i=0; start_itr+i != (( (*bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            start_itr = (*bb)->begin();
            Inslist::iterator itr = start_itr+i;



            if((**itr).isbinop() && (**itr).getintorfloat() == 1){
                BinopInt &instr=*(static_cast<BinopInt*>(*itr));
                switch(instr.binoptype){
                    case OP_MUL:
                       //Find if one is an immediate, either.....if one is exact pow of 2, choose that one, else, choose larger number
                       if(instr.isarg1reg==ARG_IMM && instr.isarg2reg==ARG_IMM){
                           double arg1log = log2(instr.arg1);
                           double arg2log = log2(instr.arg2);
                           bool arg1exact = false;
                           bool arg2exact =false;
                           if(instr.arg1 == pow(2,int(arg1log))){ //This means it was an exact power of 2
                            arg1exact = true;
                           }
                           if(instr.arg2==pow(2,int(arg2log))){
                            arg2exact = true;
                           }
                           if(arg1exact && arg2exact){
                            if(arg2exact<arg1exact){
                                //work on arg2
                                strengthReduceMul(*bb,&instr,2);
                                returnval=true;
                            }
                            else{
                                //work on arg1
                                strengthReduceMul(*bb,&instr,1);
                                returnval=true;
                            }
                           }
                           else if(arg1exact){
                             //work on arg1
                             strengthReduceMul(*bb,&instr,1);
                             returnval=true;
                           }
                           else if(arg2exact){
                              //work on arg2
                              strengthReduceMul(*bb,&instr,2);
                              returnval=true;
                           }
                           else{
                             //work on arg2
                             strengthReduceMul(*bb,&instr,2);
                             returnval=true;
                           }
                       }
                       else if(instr.isarg1reg==ARG_IMM){
                        //work on arg1
                        strengthReduceMul(*bb,&instr,1);
                        returnval=true;
                       }
                       else if(instr.isarg2reg==ARG_IMM){
                        //work on arg2
                        strengthReduceMul(*bb,&instr,2);
                        returnval=true;
                       }
                    break;
                    case OP_DIV:
                      //Find if 2nd is an immediate, if yes, work w that
                      if(instr.isarg2reg==ARG_IMM){
                        //work on arg2
                        strengthReduceDiv(*bb,&instr);
                        returnval=true;
                      }
                    break;
                    case OP_MOD:
                      //Find if 2nd is an immediate, if yes, work w that
                        if(instr.isarg2reg==ARG_IMM){
                            strengthReduceMod(*bb,&instr);
                            returnval=true;
                        }
                    break;
                   default:
                        break;
                }
            }

        }
    }
    return returnval;


}


bool IROptimizer::constant_propagation(IRFunction* irfunc){
	bool doneanything=false;


    //at some point delete everything or you'll have a memory leak
    std::vector<IRConstant *> constants;

    // Find all constants
    for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
    //do something
        if ((*bb)->size()>=3){
        for (Inslist::iterator itr = (*bb)->begin(); itr != (( (*bb)->end() )-2); itr++){
        //to help our syntax
            Instruction &firstinstr = **(itr);
            Instruction &secondinstr = **(itr+1);
            Instruction &thirdinstr= **(itr+2); //subtract 1 from end of for loop to prevent segfault

            //if there's a alloc, store immediate followed by a load from same pointer, then that register is a constant
            // store the register and its value, account for type
            if (firstinstr.isalloc()){
                Alloc & thealloc = (Alloc &) firstinstr;
                if (secondinstr.isstore()){
                    Storeop & thestoreop = (Storeop &) secondinstr;
                    if (thestoreop.argkind==ARG_IMM){
                        if (thirdinstr.isload()){
                            Loadop & theloadop = (Loadop &) thirdinstr;
                            if ( ((theloadop.theptr)==(thestoreop.theptr)) &&
                                 ((theloadop.theptr)==(thealloc.theptr)) ) {
                                IRConstant * newconstant=new IRConstant(theloadop.resultreg,thestoreop.arg,thestoreop.thetype);

                                constants.push_back(newconstant);

                                //Now delete these silly instructions, once we've gotten the constant out
                                if (!regiswithinfunccall(irfunc, newconstant->theregister)){
                                    (*bb)->erase(itr,itr+3);
                                    // Go back one so we don't skip any
                                    itr--;
                                    doneanything=true;
                                }
                            }
                        }
                    }
                }
            }

        }
        }
    }

#ifdef DEBUG
    //Print the constants
    printf("Found Constants:\n");
    for (std::vector<IRConstant *>::iterator itr = constants.begin(); itr!= constants.end(); itr++){
        IRConstant & theconstant=**itr;
        switch (theconstant.thetype){
        case ARG_INT:
            printf("R%d = %d \n",theconstant.theregister,theconstant.thevalue.i);
            break;
        case ARG_BOOL:
            printf("R%d = boolean %d \n",theconstant.theregister,theconstant.thevalue.b);
            break;
        case ARG_FLOAT:
            printf("R%d = %f \n",theconstant.theregister,theconstant.thevalue.f);
            break;
        //case ARG_DOUBLE:  Not sure what to do about this
        //    break;
        default:
        printf("Something went wrong in constant propogation \n");
        abort();

        }
    }
#endif

    //For each constant, iterate through and search for instructions that use it
    //Instructions can be binops, stores, or jumps.
    for (std::vector<IRConstant *>::iterator itr = constants.begin(); itr!= constants.end(); itr++){
        IRConstant & theconstant=**itr;

        replaceregwithconstant(irfunc, theconstant);
    }

    return doneanything;
}

void IROptimizer::replaceregwithconstant(IRFunction * irfunc, IRConstant & theconstant){
    for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
    //do something
        for (Inslist::iterator itr = (*bb)->begin(); itr != (( (*bb)->end() )-1); itr++){
        //to help our syntax
            Instruction &instr = **itr;

            if (instr.isbinop() || instr.isstore() || instr.isjump()){
                instr.replaceregwithimm(theconstant);
            }
        }
    }
}

//These are the virtual fxn overrides that actually do the work for constant propagation
void BinopBool::replaceregwithimm(IRConstant & theconstant){
    if (isarg1reg==ARG_REG) {
        if (theconstant.theregister==arg1){
            arg1=theconstant.thevalue.b;
            isarg1reg=ARG_IMM;
        }
    }
    if (isarg2reg==ARG_REG) {
        if (theconstant.theregister==arg2){
            arg2=theconstant.thevalue.b;
            isarg2reg=ARG_IMM;
        }
    }
}

void BinopInt::replaceregwithimm(IRConstant & theconstant){
    if (isarg1reg==ARG_REG) {
        if (theconstant.theregister==arg1){
            arg1=theconstant.thevalue.i;
            isarg1reg=ARG_IMM;
        }
    }
    if (isarg2reg==ARG_REG) {
        if (theconstant.theregister==arg2){
            arg2=theconstant.thevalue.i;
            isarg2reg=ARG_IMM;
        }
    }
}

void BinopFloat::replaceregwithimm(IRConstant & theconstant){
    if (isarg1reg==ARG_REG) {
        if (theconstant.theregister==arg1){
            arg1=theconstant.thevalue.f;
            isarg1reg=ARG_IMM;
        }
    }
    if (isarg2reg==ARG_REG) {
        if (theconstant.theregister==arg2){
            arg2=theconstant.thevalue.f;
            isarg2reg=ARG_IMM;
        }
    }
}

void Storeop::replaceregwithimm(IRConstant & theconstant){
    if ((argkind==ARG_REG)){
        if (theconstant.theregister==arg.reg){
            if(theconstant.thetype==thetype){
                switch (theconstant.thetype){
                    case ARG_BOOL:
                        arg.b=theconstant.thevalue.b;
                        break;
                    case ARG_INT:
                        arg.i=theconstant.thevalue.i;
                        break;
                    case ARG_FLOAT:
                        arg.f=theconstant.thevalue.f;
                        break;
                    default:
                        printf("something bad happened in storeop::replaceregwithimm, aborting \n");
                        abort();
                }
                argkind=ARG_IMM;
            }
        }
    }
}

void CondJump::replaceregwithimm(IRConstant & theconstant){
//This one is special.  Do work if it's a conditional branch, and idea is to turn it into
// an unconditional one.
    if (isconditional){
        if ( (theconstant.theregister==condreg) && (theconstant.thetype==ARG_BOOL) ) {
            // Figure out which condition to take
            if (theconstant.thevalue.b){
                isconditional=false;
            }
            else {
                //if propagating a false, move iffalse into iftrue
                iftrue=iffalse;
                isconditional=false;
            }
        }
    }

}

bool IROptimizer::regiswithinfunccall(IRFunction * irfunc, int theregister){
    bool foundit=false;

    for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
        for (Inslist::iterator itr = (*bb)->begin(); itr != (( (*bb)->end() )); itr++){
            Instruction & instr = **(itr);
            if (instr.isfunccall()){
                CallFunction & thefunccall = (CallFunction &) instr;
                if (thefunccall.searchforlocalreg(theregister)){
                    foundit=true;
                    return foundit;
                }
            }
        }
    }
    return foundit;
}

bool CallFunction::searchforlocalreg(int theregister){
    for (unsigned int i=0; (i<(argtypes->size()));i++)
    {
        if ((*argregs)[i]==theregister){
            return true;
        }
    }
    return false;
}

bool IROptimizer::constant_folding(IRFunction* irfunc){
    bool doneanything=false;
    // Basically, if we have a binop of immediates, reduce it to an alloc, store and load
    // then it will get optimized out by next constant propagation pass.  I will create special pointers
    // prefixes within alloc/load/store to denote pointers created by this method instead of the standard
    // %r.  We have a guarantee that no other instructions will use this pointer prefix, as they
    // will just use the register from the load instead.
    for (BlockList::iterator bb = irfunc->blocks.begin(); bb != irfunc->blocks.end(); bb++) {
        Inslist::iterator start_itr = (*bb)->begin();
        for (int i=0; start_itr+i != (( (*bb)->end() )); i++){
            //Just in case, reinitialize start_iterator, causes a nasty bug otherwise
            //where a removal or insert causes iterator invalidation.  This is also why
            //I'm passing in offsets instead of iterators to my work functions, and doing start_itr+i
            //all over the place.  Should have used std::list in the first place.
            start_itr = (*bb)->begin();
            Inslist::iterator itr = start_itr+i;
            Instruction & instr = **(itr);
            if (instr.isbinop()){
                if (instr.arebothimm()){
                // If both are immediates, go ahead and compute the result, delete the instruction, and insert instructions
                // in the right place.
                    switch (instr.binop_type()){
                        case ARG_INT:
                        {
                            BinopInt & thebinop = (BinopInt &) instr;
                            //int and float binops can output bools or ints/floats
                            llvmtype returntype;
                            int result=thebinop.computeresult(returntype);
                            //Now the magic, I created instructions to replace the binop with alloc/load/store with
                            //the special ptr type inplace on the basic block.  New registers created by IROptimizer
                            //will have negative value and need to be handled correctly by anything that uses them.
                            //
                            if (returntype==ARG_BOOL){
                                replacebinop_bool(bb,i,(bool)result);
                                doneanything=true;
                            }else if (returntype==ARG_INT){
                                replacebinop_int(bb,i,result);
                                doneanything=true;
                            }
                        }
                        break;
                        case ARG_BOOL:
                        {
                            BinopBool & thebinop = (BinopBool &) instr;
                            bool result=thebinop.computeresult();
                            replacebinop_bool(bb,i,result);
                            doneanything=true;
                        }
                        break;
                        case ARG_FLOAT:
                        {
                            BinopFloat & thebinop = (BinopFloat &) instr;
                            llvmtype returntype;
                            float result=thebinop.computeresult(returntype);
                            if (returntype==ARG_BOOL){
                                replacebinop_bool(bb,i,(bool)result);
                                doneanything=true;
                            } else if (returntype==ARG_FLOAT){
                                replacebinop_float(bb,i,result);
                                doneanything=true;
                            }
                        }
                        break;
                        case ARG_VOID:
                        default:
                        printf("something went wrong in constant folding, aborting");
                        abort();
                }
                }
            }
        }
    }
    return doneanything;
}

int BinopInt::computeresult(llvmtype & returntype){
    //this assumes that both args are immediates.
    int result;
    returntype=ARG_INT;

    switch (binoptype){
    case OP_ADD:
        result=arg1+arg2;
        break;
    case OP_SUB:
        result=arg1-arg2;
        break;
    case OP_MUL:
        result=arg1*arg2;
        break;
    case OP_DIV:
        if (arg2==0){
            printf("divide by zero occurred during constant folding, aborting");
            abort();
        }
        result=arg1/arg2;
        break;
    case OP_MOD:
        result=arg1%arg2;
        break;

        //Put something here?
    case OP_CONCAT:
        printf("No, don't use concat in binops");
        abort();
        break;

        //<result> = icmp <cond> <ty> <op1>, <op2>   ; yields {i1} or {<N x i1>}:result
    case OP_LT:
        result=arg1<arg2;
        returntype=ARG_BOOL;
        break;
    case OP_GT:
        result=arg1>arg2;
        returntype=ARG_BOOL;
        break;
    case OP_LE:
        result=arg1<=arg2;
        returntype=ARG_BOOL;
        break;
    case OP_GE:
        result=arg1>=arg2;
        returntype=ARG_BOOL;
        break;
    case OP_EQ:
        result=arg1==arg2;
        returntype=ARG_BOOL;
        break;
    case OP_NE:
        result=arg1!=arg2;
        returntype=ARG_BOOL;
        break;
    default:
        printf("incorrect op in binopint::computeresult");
        abort();
    }

    return result;
}

float BinopFloat::computeresult(llvmtype & returntype){
    //this assumes that both args are immediates.
    float result;
    returntype=ARG_FLOAT;

    switch (binoptype){
    case OP_ADD:
        result=arg1+arg2;
        break;
    case OP_SUB:
        result=arg1-arg2;
        break;
    case OP_MUL:
        result=arg1*arg2;
        break;
    case OP_DIV:
        if (arg2==(float)0){
            printf("divide by zero occurred during constant folding, aborting");
            abort();
        }
        result=arg1/arg2;
        break;
    case OP_MOD:
        result=fmod(arg1,arg2);
        break;

        //Put something here?
    case OP_CONCAT:
        printf("No, don't use concat in binops");
        abort();
        break;

        //<result> = icmp <cond> <ty> <op1>, <op2>   ; yields {i1} or {<N x i1>}:result
    case OP_LT:
        result=arg1<arg2;
        returntype=ARG_BOOL;
        break;
    case OP_GT:
        result=arg1>arg2;
        returntype=ARG_BOOL;
        break;
    case OP_LE:
        result=arg1<=arg2;
        returntype=ARG_BOOL;
        break;
    case OP_GE:
        result=arg1>=arg2;
        returntype=ARG_BOOL;
        break;
    case OP_EQ:
        result=arg1==arg2;
        returntype=ARG_BOOL;
        break;
    case OP_NE:
        result=arg1!=arg2;
        returntype=ARG_BOOL;
        break;
    default:
        printf("incorrect op in binopint::computeresult");
        abort();
    }

    return result;
}

bool BinopBool::computeresult(){
    //this assumes that both args are immediates.
    bool result;

    switch (binoptype){
    case OP_AND:
        result= arg1 && arg2;
        break;
    case OP_OR:
        result=arg1||arg2;
        break;
    case OP_XOR:
        // xor=AB'+A'B
        result= (arg1&&(!arg2))||((!arg1)&&arg2);
        break;
    default:
        printf("incorrect op in binopbool::computeresult");
        abort();
    }

    return result;
}

void IROptimizer::replacebinop_bool(BlockList::iterator bb,int offset, bool value){
    // Create new Alloc, Store and Load
    // delete current
    // add them in the same place
    Inslist::iterator itr = (*bb)->begin()+offset;

    int newpointer=getregister();
    int resultregister=(*itr)->assigned_register();

    Instruction * thealloc= new Alloc(newpointer,ARG_BOOL);
    Instruction * thestore= new Storeop(value,newpointer);
    Instruction * theload=new Loadop(resultregister,ARG_BOOL,newpointer);


    // Insert in reverse order
    (*bb)->erase((*bb)->begin()+offset);

    (*bb)->insert((*bb)->begin()+offset,theload);
    (*bb)->insert((*bb)->begin()+offset,thestore);
    (*bb)->insert((*bb)->begin()+offset,thealloc);
}
void IROptimizer::replacebinop_int(BlockList::iterator bb,int offset, int value){

    Inslist::iterator itr = (*bb)->begin()+offset;
    int newpointer=getregister();
    int resultregister=(*itr)->assigned_register();

    Instruction * thealloc= new Alloc(newpointer,ARG_INT);
    Instruction * thestore= new Storeop(value,ARG_IMM,ARG_INT,newpointer);
    Instruction * theload=new Loadop(resultregister,ARG_INT,newpointer);


    // Insert in reverse order
    (*bb)->erase((*bb)->begin()+offset);

    (*bb)->insert((*bb)->begin()+offset,theload);
    (*bb)->insert((*bb)->begin()+offset,thestore);
    (*bb)->insert((*bb)->begin()+offset,thealloc);
}
void IROptimizer::replacebinop_float(BlockList::iterator bb,int offset, float value){

    Inslist::iterator itr = (*bb)->begin()+offset;
    int newpointer=getregister();
    int resultregister=(*itr)->assigned_register();

    Instruction * thealloc= new Alloc(newpointer,ARG_FLOAT);
    Instruction * thestore= new Storeop(value,newpointer);
    Instruction * theload=new Loadop(resultregister,ARG_FLOAT,newpointer);

    // Insert in reverse order
    (*bb)->erase((*bb)->begin()+offset);

    (*bb)->insert((*bb)->begin()+offset,theload);
    (*bb)->insert((*bb)->begin()+offset,thestore);
    (*bb)->insert((*bb)->begin()+offset,thealloc);
}
