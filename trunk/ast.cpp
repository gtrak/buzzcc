#include "node.h"
#include <string>

using namespace std;
typedef SymbolInfo::llvm_type lltype;
template<> void ListNode<IfPair>::print_children_spaces() {}
template<> ListNode<IfPair>::~ListNode() {
    for (unsigned int i = 0; i < this->size(); i++) {
        delete this->operator[](i).first;
        delete this->operator[](i).second;
    }
}
// String Arg 
StringArg::~StringArg()
{
 	if (t) delete t;
} 

// Print help functions in 
void AstNode::print_group(const char* name)
{
	fprintf(yyout, "(%s %d ", name, get_line());
	print_children();
	fprintf(yyout, ")");
}

// ExpList class
/*
SymbolInfo::llvm_type ExpList::infer_type(TableBuilder& builder)
{
}
*/
// Name class 
SymbolInfo::llvm_type Name::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo* info = builder.find((std::string)*this);
	if (info == NULL) {
		serror(get_line(), "Variable name not defined.");
	}
	return info->get_type();
}


// FuncCall class 
SymbolInfo::llvm_type FuncCall::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	std::string symbol( pexp->get_symbol_name() );
	if (symbol == "print") 
	{
        Args::ArgVec *arg_exps = args->get_explist();
		for (unsigned int i = 0; i < arg_exps->size(); i++)
			(*arg_exps)[i]->infer_type(builder);
		delete arg_exps;
		return SymbolInfo::NO_TYPE;
	}
	SymbolInfo* info = builder.find_global(symbol);
	if (info == NULL)
	{
		serror(get_line(), "Calling undeclared function.");		
		return SymbolInfo::NO_TYPE;
	}
	else 
	{
		// we must infer the return type of the function at this point 
		if (info->get_type() == SymbolInfo::LL_UNKNOWN) 
		{
		    Args::ArgVec *arg_exps = args->get_explist();
			for (unsigned int i = 0; i < arg_exps->size(); i++)
			{
				if ((*arg_exps)[i]->infer_type(builder) == SymbolInfo::LL_UNKNOWN) 
					return SymbolInfo::LL_UNKNOWN; 
			}

			if (info->is_defined()) return SymbolInfo::LL_UNKNOWN; 
			FuncDeclSt* node = info->get_func_decl(); 
			SymbolInfo::llvm_type t = node->infer_type_fcall(args, builder);
			info->set_type(t);
			return t;
		}
		// the return type has already been inferred, so we must check arguments 
		else 
		{
			Args::ArgVec *arg_exps = args->get_explist();
			SymbolInfo::ArgTypeList argtypes;
			for (unsigned int i = 0; i < arg_exps->size(); i++) 
			{
				argtypes.push_back((*arg_exps)[i]->infer_type(builder));		
			}
			if (info->equal_args(argtypes) == false)
			{
				serror(get_line(), "Function called twice with different argument types.");
			}
			
			delete arg_exps;
			return info->get_type();
		}
	}
}

int FuncCall::gen_exp_code(IRBuilder& builder)
{
	string func_name = pexp->get_symbol_name();
	//printf("Getting called! %s\n", func_name.c_str());
	int ret_val;
	if (func_name == "print")
	{
		Args::ArgVec* explist = args->get_explist();

		for (unsigned int i = 0; i < explist->size(); i++)
		{
			int loc_reg = builder.get_local_register();
			int format_reg = builder.get_local_register();
			SymbolInfo::llvm_type type = (*explist)[i]->get_type();
            std::vector<llvmtype> converted_args;
            std::vector<int> arg_registers;

			int exp_register = (*explist)[i]->gen_exp_code(builder);
			arg_registers.push_back(format_reg);
			converted_args.push_back(convert(SymbolInfo::LL_STRING));
			arg_registers.push_back(exp_register);
			converted_args.push_back(convert((*explist)[i]->get_type()));
			
			int alloc_reg;	
			int fp_arg, double_reg;
		 	switch (type)
    		{
    		case SymbolInfo::LL_BOOL:
				builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1004));
        		break;
    		case SymbolInfo::LL_STRING:
				alloc_reg = builder.get_local_register();
				arg_registers.pop_back();
				builder.add_ins(new Alloc(alloc_reg, convert(SymbolInfo::LL_STRING), 256));
				arg_registers.push_back(alloc_reg);	
				builder.add_ins(new Storeop(exp_register, ARG_REG, convert(SymbolInfo::LL_STRING), alloc_reg, false));
				builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1001));
        		break;
    		case SymbolInfo::LL_FLOAT:
				fp_arg = arg_registers.back();
				double_reg = builder.get_local_register();
				arg_registers.back() = double_reg;
				converted_args.back() = ARG_DOUBLE;
				builder.add_ins(new Fpext(double_reg, fp_arg));
				builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1002));
        		break;
    		case SymbolInfo::LL_INTEGER:
				builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1000));
        		break;
    		case SymbolInfo::LL_NIL:
				builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1003));
        		break;
   			case SymbolInfo::NO_TYPE:
        		serror(get_line(), "NO_TYPE type in function argument.");
        		break;
    		case SymbolInfo::LL_UNKNOWN:
        		serror(get_line(), "LL_UNKNOWN type in function argument.");
        		break;
    		default:
        		serror(get_line(), "Unrecognized type in function argument.");
       		 	break;
			}
			builder.add_ins(new CallFunction(loc_reg, "printf", convert(SymbolInfo::LL_INTEGER), &converted_args, &arg_registers, true, CPRINTF));
		}
        std::vector<llvmtype> converted_args;
        std::vector<int> arg_registers;
		int loc_reg = builder.get_local_register();
		int format_reg = builder.get_local_register();
		converted_args.push_back(convert(SymbolInfo::LL_STRING));
		arg_registers.push_back(format_reg);
		
		builder.add_ins(new GetPointer(format_reg, convert(SymbolInfo::LL_STRING), 1005));
		builder.add_ins(new CallFunction(loc_reg, "printf", convert(SymbolInfo::LL_INTEGER), &converted_args, &arg_registers, true, CPRINTF));
		delete explist;	
		ret_val = loc_reg;
	}
	else 
	{
		SymbolInfo* info = get_scope()->find_defined( pexp->get_symbol_name() );
		if (info == NULL) 
		{
			serror(get_line(), "Function not found in symbol table.");
		}
		else 
		{
			int local_register = builder.get_local_register();
			std::vector<SymbolInfo::llvm_type>& arg_types = info->get_args();
			std::vector<llvmtype> converted_args;
			std::vector<int> arg_registers;
			//builder.add_ins(new 
			Args::ArgVec* explist = args->get_explist();	
			for (unsigned int i = 0; i < arg_types.size(); i++) {
				converted_args.push_back(convert(arg_types[i]));	
				arg_registers.push_back((*explist)[i]->gen_exp_code(builder));
			}
			builder.add_ins(new CallFunction(local_register, func_name, convert(info->get_type()), &converted_args, &arg_registers));
			delete explist;
			ret_val = local_register;
		}
	}
	return ret_val;	
}


// StringArg
Args::ArgVec* StringArg::get_explist()
{
	ArgVec *list = new ArgVec(get_line());
	StringExp* exp = new StringExp(str, get_line());
	list->push_back(exp);
	exp->set_type(SymbolInfo::LL_STRING);
	t = exp;
	return list;
}

// Chunk class 
void Chunk::gen_code(IRBuilder& builder)
{	
	for (unsigned int i = 0; i < size(); i++) {
		operator[](i)->gen_code(builder);
	}
}

SymbolInfo::llvm_type Chunk::infer_type(TableBuilder& builder)
{
	for (unsigned int i = 0; i < size(); i++)
	{
		if (operator[](i)->changes_scope()) {
			serror(operator[](i)->get_line(), "No return/break statement allowed in Chunk.");	
		}
		operator[](i)->infer_type(builder);
	}
	return SymbolInfo::NO_TYPE;
}

int Name::name_gen_code_local(IRBuilder& builder){
  
  int loc_reg;
  SymbolInfo* info = get_scope()->find_in_table((std::string)*this);
  if (info == NULL) {
    serror(get_line(), "Variable not in Symbol Table.");
  }
  else
    {
      lltype t = info->get_type();
      
      if (!info->is_defined()) 
	{
	  loc_reg = builder.get_local_register();
	  info->set_reg(loc_reg);
	  info->define();
	  if (t == SymbolInfo::LL_STRING) 
	    builder.add_ins(new Alloc(loc_reg, convert(t), 256));
	  else 
	    builder.add_ins(new Alloc(loc_reg, convert(t)));
	}
      else loc_reg=info->get_reg();
      
      
    }

  return loc_reg;
  
}


int Name::name_gen_code_global(IRBuilder& builder)
{
  int reg;
  //0 is local, 1 is global
  SymbolInfo* info = get_scope()->find_defined(static_cast<std::string>(*this)); 
		if (info == NULL) {
		  info = get_scope()->find_in_table(static_cast<std::string>(*this));
		}
		if (info == NULL) {
		  serror(get_line(), "Variable not in Symbol Table.");
		}
		else 
		  {
		       
			lltype exp_type = info->get_type();
			if (!info->is_defined()) 
			  {
			    reg = builder.get_global_reg();
				info->define();
				info->set_reg(reg);
				builder.add_global(exp_type, reg);
			  }
			else reg = info->get_reg();
			
		
		  }
		return reg;
}




SymbolInfo::llvm_type Block::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	Chunk& chunk = *chk;
	SymbolInfo::llvm_type t_return = SymbolInfo::NO_TYPE;
	bool unknown = false;
	for (unsigned int i = 0; i < chunk.size(); i++)
	{
		SymbolInfo::llvm_type type = chunk[i]->infer_type(builder);
		//printf("Return type of block statement %d: %d\n", i, type);
		if (chunk[i]->changes_scope() && (i != chunk.size() - 1)) {
			serror(get_line(), "Break or Return statement not allowed outside end of block.");
		}
		if (type == SymbolInfo::NO_TYPE) {
			continue;
		}
		else if (type == SymbolInfo::LL_UNKNOWN) {
			unknown = true;
			continue;	
		}
		else 
		{
			if (t_return == SymbolInfo::NO_TYPE)
			{
				t_return = type;
			}
			else if (t_return == type)
			{
				continue;
			}
			else serror(get_line(), "Multiple return types not allowed in a single block.");
		}
	}
	if ( unknown && (t_return == SymbolInfo::LL_UNKNOWN))
		serror(get_line(), "Function Return type cannot be deduced.");
	//printf("Return type of block %d: %d\n", get_line(), t_return);
	return t_return;
}

bool Block::return_at_end()
{
	Chunk& chunk = *chk;
	if (chunk.size() == 0) return false;
	if (chunk[chunk.size()-1]->changes_scope()) return true;
	else return false;
}


// Block Class 
void Block::print_children() 
{
	fputc('\n', yyout);
    Indenter& ind = Indenter::GetIndenter();
    ind.inc();
	ind.indent();
	chk->print();	
	fputc('\n', yyout);
    ind.dec();
    ind.indent();
}

void Block::gen_code(IRBuilder& builder)
{	
	chk->gen_code(builder);
}

// BinOp class

/*
void BinOp::print_children() 
{
	switch(opr) {
		case OP_PLUS: 	fputs("+ ", yyout);
			break;
		case OP_MINUS: fputs("- ", yyout);
			break;
		case OP_MULT: 	fputs("* ", yyout); 
			break;
		case OP_DIV:	fputs("/ ", yyout);
			break;
		case OP_EXP:	fputs("^ ", yyout);
			break;
		case OP_MOD:	fputs("% ", yyout);
			break;
		case OP_CONCAT:fputs(".. ", yyout);
			break; 
		case OP_LT:	fputs("< ", yyout);
			break;
		case OP_LE:	fputs("<= ", yyout);
			break;
		case OP_GT:	fputs("> ", yyout);
			break;
		case OP_GE:	fputs(">= ", yyout);
			break;
		case OP_EQ: 	fputs("== ", yyout);
			break;
		case OP_NEQ:	fputs("~=", yyout);
			break;
		case OP_AND:	fputs("and ", yyout);
			break;
		case OP_OR:	fputs("or ", yyout);
			break;
		default: break;
	}
}

void UnOp::print_children()
{
	switch(opr) {
		case UO_MINUS: fputs("- ", yyout); 
			break;
		case UO_NOT:	fputs("not ", yyout);
			break;
		case UO_POUND:	fputs("# ", yyout); 
			break;
		default: break;
	}

}
*/

// Statement classes 

void SyntaxTree::lisp_print()
{
	root->print();
	fputc('\n', yyout);
}

/*
void Assignment::print() 
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(Assignment %d xxxx)\n", get_line());
}

void FuncCall::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(funccall %d xxxx)\n", get_line());
}

void DoEnd::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(DoEnd %d xxxx)\n", get_line());
}

void WhileLoop::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(WhileLoop %d xxxx)\n", get_line());
}

void RepeatLoop::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(n, *yyout;RepeatLoop %d xxxx)\n", get_line());
}

void IfThen::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(IfThen %d xxxx)\n", get_line());
}

void ForLoop::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(ForLoop %d xxxx)\n", get_line());
}

void ForIter::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(ForIter %d xxxx)\n", get_line());
}

void FuncDecl::print()
{
	Indenter& ind = Indenter::GetIndenter();
	ind.indent();
	fprintf(yyout, "(FuncDecl %d xxxx)\n", get_line());
}
*/

