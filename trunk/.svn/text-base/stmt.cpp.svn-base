#include "stmt.h"
#include "symtable.h"
using namespace std;
typedef SymbolInfo::llvm_type lltype;

void AssignSt::gen_code(IRBuilder& builder)
{
	ExpList& exp = *exps;
    VarList& var = *vars;
	for (unsigned int i = 0; i < vars->size(); i++)
	{
		// SymbolInfo* info = get_scope()->find_in_table(var[i]->get_symbol_name());
		SymbolInfo* info = get_scope()->find_defined(var[i]->get_symbol_name());
		if (info == NULL) {
			info = get_scope()->find_in_table(var[i]->get_symbol_name());
		}
        if (info == NULL) {
			serror(get_line(), "Variable not in Symbol Table.");
		}
		else
		{
			int reg;
			lltype exp_type = info->get_type();
			if (!info->is_defined())
			{
				reg = builder.get_global_reg();
				info->define();
				info->set_reg(reg);
				builder.add_global(exp_type, reg);
			}
			else reg = info->get_reg();

			int reg_temp = exp[i]->gen_exp_code(builder);
			builder.add_ins(new Storeop(reg_temp, ARG_REG, convert(info->get_type()), reg, info->is_global()));
			/*
			if (exp[i]->is_constant())
			{
				llvmtype ir_type = convert(info->get_type());
				switch(exp_type)
				{
				case SymbolInfo::LL_INTEGER:
					builder.add_ins(new Storeop(((NumberExp*)exp[i])->getint(), ARG_IMM, ir_type, reg, info->is_global()));
					break;
				case SymbolInfo::LL_FLOAT:
					builder.add_ins(new Storeop(((NumberExp*)exp[i])->getfloat(), reg, info->is_global()));
					break;
				case SymbolInfo::LL_STRING:
					builder.add_ins(new Storeop(exp[i]->getint(), ARG_IMM, convert(LL_STRING), reg, info->is_global()));
					break;
				case SymbolInfo::LL_NIL:
					//
					break;
				case SymbolInfo::LL_BOOL:
					builder.add_ins(new Storeop(((BoolExp*)exp[i])->getBoolVal(), reg, info->is_global()));
					break;
				default: serror(get_line(), "Unrecognized constant type in expression");
					break;
				}
			}
			else
			{
				llvmtype ir_type = convert(info->get_type());
				builder.add_ins(new Storeop(exp[i]->gen_exp_code(builder), ARG_REG, ir_type, reg, info->is_global()));
			}
			*/
		}
	}
}

SymbolInfo::llvm_type AssignSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	ExpList& exp = *exps;
	VarList& var = *vars;
	for (unsigned int i = 0; i < vars->size(); i++)
	{
		SymbolInfo *info = builder.find(var[i]->get_symbol_name());
		//printf("Var: %s\nline %d\n", var[i]->get_symbol_name().c_str(), var[i]->get_line());
		if (info == NULL)
		{
			// if variable does not exist, create entry in symbol table

			if (i < exps->size())
				builder.add_global(var[i]->get_symbol_name(), new SymbolInfo(exp[i]->infer_type(builder)));
			else
				builder.add_global(var[i]->get_symbol_name(), new SymbolInfo(SymbolInfo::LL_NIL));
		}
		else
		{
			// if variable exists, ensure that type of explist matches varlist
			if (i < exps->size())
			{
				if ( var[i]->infer_type(builder) != exp[i]->infer_type(builder) )
				{
					//printf("var %d, exp %d\n", var[i]->infer_type(builder), exp[i]->infer_type(builder));
					serror(get_line(), "Existing Variable is assigned expression of different type.\n");
				}
			}
			// if no exp is given, assign var to nill
			else info->type = SymbolInfo::LL_NIL;
		}
	}
	return SymbolInfo::NO_TYPE;
}

SymbolInfo::llvm_type FuncCallSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	return fx->infer_type(builder);
}

void FuncCallSt::gen_code(IRBuilder& builder)
{
	fx->gen_exp_code(builder);
}

SymbolInfo::llvm_type DoEndSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	builder.new_scope();
	SymbolInfo::llvm_type t = blk->infer_type(builder);
	builder.ascend();
	return t;
}

void DoEndSt::gen_code(IRBuilder& builder){
	blk->gen_code(builder);
}

SymbolInfo::llvm_type WhileLoopSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	lltype t = pred->infer_type(builder);
	if (t != SymbolInfo::LL_BOOL) serror(get_line(), "Predicate of While statement is not a bool.");
	builder.new_scope();
	SymbolInfo::llvm_type t_return = blk->infer_type(builder);
	builder.ascend();
	//First, check to see if it has a return in it....if it does, then return its type!
	return t_return;
}

void WhileLoopSt::gen_code(IRBuilder& builder)
{
	int loop_label = builder.get_label_num();
	int test_label = builder.get_label_num();
	int after_label = builder.get_label_num();

	// non conditional jmp to test label
	builder.add_ins(new CondJump(test_label));

	builder.create_bb(loop_label);
	blk->gen_code(builder);

	// non conditional jmp to test label
	builder.add_ins(new CondJump(test_label));

	builder.create_bb(test_label);
	int test_register = pred->gen_exp_code(builder);

	// condition jmp to loop_label
	builder.add_ins(new CondJump(test_register, loop_label, after_label));
	builder.create_bb(after_label);
}

SymbolInfo::llvm_type RepeatLoopSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	//Infer lower levels just to ensure they do their appropriate checks
	lltype t = pred->infer_type(builder);
	if (t != SymbolInfo::LL_BOOL) serror(get_line(), "Predicate of Repeat statement is not a bool.");

	builder.new_scope();
	SymbolInfo::llvm_type t_return = blk->infer_type(builder);
	builder.ascend();
  	//First, check to see if it has a return in it....if it does, then return its type!

	return t_return;
}


void RepeatLoopSt::gen_code(IRBuilder& builder)
{
	int loop_label = builder.get_label_num();
	int after_label = builder.get_label_num();

	// non-conditional juimp to loop body
	builder.add_ins(new CondJump(loop_label));

	// Loop body
	builder.create_bb(loop_label);
	blk->gen_code(builder);

	int test_register = pred->gen_exp_code(builder);
	// condition jmp to loop_label
	builder.add_ins(new CondJump(test_register, after_label, loop_label));
	builder.create_bb(after_label);
}

void ForLoopSt::gen_code(IRBuilder& builder){

  //For has a start, end, and optionally an increment
  //First, Store start in memory,


  /*  This is sort of what we need to do:
      entry:
      %b = alloca i32		; <i32*> [#uses=2]
      %a = alloca i32		; <i32*> [#uses=2]
	%i = alloca i32		; <i32*> [#uses=5]
	%"alloca point" = bitcast i32 0 to i32		; <i32> [#uses=0]
	store i32 0, i32* %i, align 4
	store i32 0, i32* %a, align 4
	store i32 2, i32* %b, align 4
	store i32 0, i32* %i, align 4
	br label %bb1

	bb:		; preds = %bb1
	store i32 1, i32* %a, align 4
	%0 = load i32* %i, align 4		; <i32> [#uses=1]
	%1 = load i32* %b, align 4		; <i32> [#uses=1]
	%2 = add i32 %0, %1		; <i32> [#uses=1]
	store i32 %2, i32* %i, align 4
	br label %bb1

	bb1:		; preds = %bb, %entry
	%3 = load i32* %i, align 4		; <i32> [#uses=1]
	%4 = icmp sle i32 %3, 9		; <i1> [#uses=1]
	br i1 %4, label %bb, label %bb2

	bb2:		; preds = %bb1
	br label %return

	return:		; preds = %bb2
	ret void

  */

  //I assume the entry was allocated by somebody else


  //First, prepare labels...we will need one for the main code, one for the predicate, and one for the exit

  int loop_label = builder.get_label_num();
  int test_label = builder.get_label_num();
  int exit_label = builder.get_label_num();



  //allocate memory for start
  int loopvar = nm->name_gen_code_local(builder);

  //Figure out the start value
  //We need to know if for is float or int

  //Initialize start

  llvmtype ir_type;
  if (start->get_type() == SymbolInfo::LL_FLOAT){
    ir_type = convert(SymbolInfo::LL_FLOAT);
  }
  else if (end->get_type() == SymbolInfo::LL_FLOAT) {
    ir_type = convert(SymbolInfo::LL_FLOAT);
  }
  else if ((inc != NULL) && (inc->get_type() == SymbolInfo::LL_FLOAT))
  {
	ir_type = convert(SymbolInfo::LL_FLOAT);
  }
  else {
    ir_type = convert(SymbolInfo::LL_INTEGER);
  }

  int startval = start->gen_exp_code(builder);
  builder.add_ins(new Storeop(startval, ARG_REG, ir_type, loopvar,false));


  //non conditional jmp to test label
  builder.add_ins(new CondJump(test_label));

  //create main block
  builder.create_bb(loop_label);
  blk->gen_code(builder);
  //load register we neeed to increment

  int loadreg = builder.get_local_register();

  builder.add_ins(new Loadop(loadreg,ir_type,loopvar,false));

  int addreg = builder.get_local_register();

  //and increment it
  if (ir_type == convert(SymbolInfo::LL_FLOAT))
  {
    if(inc!=NULL){
        builder.add_ins(new BinopFloat(addreg,OP_ADD,loadreg,inc->gen_exp_code(builder),ARG_REG,ARG_REG));
    }
    else{
        builder.add_ins(new BinopFloat(addreg,OP_ADD,loadreg,1.0,ARG_REG,ARG_IMM));
    }
  }
  else {
  	if(inc!=NULL){
    	builder.add_ins(new BinopInt(addreg,OP_ADD,loadreg,inc->gen_exp_code(builder),ARG_REG,ARG_REG));
  	}
  	else{
    	builder.add_ins(new BinopInt(addreg,OP_ADD,loadreg,1,ARG_REG,ARG_IMM));
  	}
  }
  //and store what we incremented
  builder.add_ins(new Storeop(addreg,ARG_REG,ir_type,loopvar,false));

  //unconditional branch to test
  builder.add_ins(new CondJump(test_label));

  //create test block
  builder.create_bb(test_label);
  //load what we incremented previously
  int loadreg2 = builder.get_local_register();
  builder.add_ins(new Loadop(loadreg2,ir_type,loopvar,false));
  //test what we incremented (conditional jump)

  //first test
  int testerreg = builder.get_local_register();
  if (ir_type == convert(SymbolInfo::LL_INTEGER))
  	builder.add_ins(new BinopInt(testerreg,OP_LE,loadreg2,end->gen_exp_code(builder),ARG_REG,ARG_REG));
  else
	builder.add_ins(new BinopFloat(testerreg, OP_LE, loadreg2, end->gen_exp_code(builder), ARG_REG, ARG_REG));

  builder.add_ins(new CondJump(testerreg,loop_label,exit_label));


  //create exit block
  builder.create_bb(exit_label);
}

SymbolInfo::llvm_type ForLoopSt::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  lltype tstart = start->infer_type(builder);
  lltype tend = end->infer_type(builder);
  lltype tinc = SymbolInfo::NO_TYPE;
  if(inc!=NULL){
    tinc = inc->infer_type(builder);
  }


  //Check types!   start, end and inc should be numbers!
  if(tstart != SymbolInfo::LL_INTEGER && tstart != SymbolInfo::LL_FLOAT && tend != SymbolInfo::LL_INTEGER && tend != SymbolInfo::LL_FLOAT)
    {
      serror(get_line(), "Using non-numeric expression for For loop iteration.\n");
    }

  if(inc!=NULL){
    if((tinc != SymbolInfo::LL_INTEGER) && (tinc != SymbolInfo::LL_FLOAT)){
      serror(get_line(), "Using non-numeric expression for For loop iteration.\n");

    }
  }


  //If nm does not exist, add

  /*
  if(info==NULL){
    builder.new_scope();
    //if start or increment are float, we want a float, else, we want an int..... i = 0.4,4,1 goes .4, 1.4..... i=0,4,.1 goes 0,.1,.2....
    if(tstart == SymbolInfo::LL_FLOAT || (inc!=NULL && tinc == SymbolInfo::LL_FLOAT)){
      builder.add_global(static_cast<std::string>(*nm), new SymbolInfo(SymbolInfo::LL_FLOAT));
    }
    else{
      builder.add_global(static_cast<std::string>(*nm), new SymbolInfo(SymbolInfo::LL_INTEGER));
    }
    builder.ascend();
 	nm->set_scope(builder.get_current_scope());

  }

  //but if it already exists....
  else{
    lltype tnm = nm->infer_type(builder);

    //we have to check to make sure its in accord to start or inc

    if(tstart == SymbolInfo::LL_FLOAT || (inc!=NULL && tinc == SymbolInfo::LL_FLOAT)){
      if(tnm!=SymbolInfo::LL_FLOAT){
	serror(get_line(), "Using different types for increment variable and increment expressions in For loop");
      }
    }
    else{
      if(tnm!=SymbolInfo::LL_INTEGER){
	serror(get_line(), "Using different types for increment variable and increment expressions in For loop");

      }
    }
  }
  */

  builder.new_scope();
  //if start or increment are float, we want a float, else, we want an int..... i = 0.4,4,1 goes .4, 1.4..... i=0,4,.1 goes 0,.1,.2....
  if(tstart == SymbolInfo::LL_FLOAT || (inc!=NULL && tinc == SymbolInfo::LL_FLOAT)){
  	builder.add_local(static_cast<std::string>(*nm), new SymbolInfo(SymbolInfo::LL_FLOAT));
  }
  else{
  	builder.add_local(static_cast<std::string>(*nm), new SymbolInfo(SymbolInfo::LL_INTEGER));
  }
  nm->set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type t = blk->infer_type(builder);
  builder.ascend();
  return t;
}

SymbolInfo::llvm_type ForIterSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	serror(get_line(), "For Iteration statements not supported.");
	return SymbolInfo::NO_TYPE;
}

void ForIterSt::gen_code(IRBuilder& builder)
{
  serror(get_line(),"For Iteration statements not supported.");
}


SymbolInfo::llvm_type FuncDeclSt::infer_type(TableBuilder& builder)
{
	builder.new_scope();
	set_scope(builder.get_current_scope());
	if (name->size() > 1) {
		serror(get_line(), "Compiler does not support classes.");
	}
	else if (name->size() == 0) {
		serror(get_line(), "No name given to function call.");
	}

	SymbolInfo* info = new SymbolInfo(SymbolInfo::LL_UNKNOWN, this);
	builder.add_global((std::string)*(name->operator[](0)), info);
	builder.ascend();
	return SymbolInfo::NO_TYPE;
}

// special function call to type check only when function call is found
SymbolInfo::llvm_type FuncDeclSt::infer_type_fcall(Args* args, TableBuilder& builder)
{
	ParList* params = body->get_params();
	NameList* nmlist = params->get_namelist();
	if (!nmlist) { serror(get_line(), "Function Arguments not present"); }
	Block* blk = body->get_block();

	SymbolTable* previous_scope = builder.get_current_scope();
	Args::ArgVec *explist = args->get_explist();
	if (explist->size() != nmlist->size()) {
		serror(get_line(), "Incorrect Number of arguments given to function.");
	}
	vector<SymbolInfo*> symbols;

	// infer types of all arguments in previous scope  &
	// adds function types to symbol taable
	SymbolInfo* info = builder.find_global((std::string)(name->get_symbol_name()));
	for (unsigned int i = 0; i < explist->size(); i++) {
		//SymbolInfo::llvm_type t = (*explist)[i]->infer_type(builder);
		SymbolInfo::llvm_type t = (*explist)[i]->get_type();
		//printf("Argument %d type %d\n", i, t);
		info->add_func_arg(t);
		symbols.push_back(new SymbolInfo(t));
	}

	// defines function and sets return type to unknown
	info->set_type(SymbolInfo::LL_UNKNOWN);
	info->define();

	// switch to functions local scope and add new variables
	builder.set_current_scope(get_scope());
	for (unsigned int i = 0; i < nmlist->size(); i++) {
		builder.add_local(*((*nmlist)[i]), symbols[i]);
	}
	delete explist;

	// infer functions twice to take care of recursive functions
	builder.new_scope();
	builder.record_global_writes();

	// infer return types
	//printf("First infer call: %s\n", name->get_symbol_name().c_str());
	SymbolInfo::llvm_type ret_type = blk->infer_type(builder);
	builder.destroy_scope();
	builder.revert_global_writes();
	builder.stop_record();

	//printf("Second infer call: %s\n", name->get_symbol_name().c_str());
	// infer again using previously discovered return type
	assert(ret_type != SymbolInfo::LL_UNKNOWN);
	info->set_type(ret_type);
	blk->infer_type(builder);
	builder.set_current_scope(previous_scope);
	builder.resume_record();

	// returns return type of block
	return ret_type;
}

void FuncDeclSt::gen_code(IRBuilder& builder)
{
	string nm = name->get_symbol_name();
	SymbolInfo* info = get_scope()->find_in_table(nm);
	assert(info != NULL);
	//info->define();
	if (info->get_type() == SymbolInfo::LL_UNKNOWN) return;

	IRFunction* function = new IRFunction(name->get_symbol_name(), info->get_type());
    ParList* params = body->get_params();
    NameList* nmlist = params->get_namelist();

	SymbolInfo::ArgTypeList& args =  info->get_args();
	builder.add_function(function);
	for (unsigned int i = 0; i < args.size(); i++)
	{
		SymbolInfo* arg_info = get_scope()->find_in_table(std::string(*(*nmlist)[i]));
		if (arg_info == NULL) {
			serror(get_line(), "Argument name not inserted into local symbol table.");
		}
		int loc_reg = builder.get_local_register();
		arg_info->set_reg(loc_reg);
		arg_info->define();

		string arg_name("r");
		arg_name.append(itos(loc_reg));
		Arg* param = new Arg(arg_name,args[i],loc_reg);
		function->add_arg(param);

		int loc_reg2 = builder.get_local_register();
		SymbolInfo::llvm_type t = arg_info->get_type();
		if (t == SymbolInfo::LL_STRING)
        	builder.add_ins(new Alloc(loc_reg2, convert(t), 256));
        else
        	builder.add_ins(new Alloc(loc_reg2, convert(t)));
        builder.add_ins(new Storeop(loc_reg, ARG_REG, convert(arg_info->get_type()), loc_reg2, false ));
        arg_info->set_reg(loc_reg2);
	}

	Block *blk = body->get_block();
	blk->gen_code(builder);
	if (info->get_type() == SymbolInfo::NO_TYPE) builder.add_ins(new ReturnFromFunc(ARG_VOID));
	else if (! blk->return_at_end())
	{
		SymbolInfo::llvm_type ret_type = info->get_type();
		int ret_reg = builder.get_local_register();
		int alloc_reg = builder.get_local_register();
		if (ret_type == SymbolInfo::LL_STRING) {
	    	builder.add_ins(new Alloc(alloc_reg, convert(ret_type), 256 ));
		}
		else builder.add_ins(new Alloc(alloc_reg, convert(ret_type)));
		builder.add_ins(new Loadop(ret_reg, convert(ret_type), alloc_reg, false));
		builder.add_ins(new ReturnFromFunc(convert(ret_type), ret_reg, ARG_REG));
	}
	builder.exit_function();
}

SymbolInfo::llvm_type LocFuncDeclSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	serror(get_line(), "Local function declarations are not supported.");

	return SymbolInfo::NO_TYPE;
}

void LocFuncDeclSt::gen_code(IRBuilder& builder){
  serror(get_line(), "Local function declarations are not supported.");
}


void LocAssignSt::gen_code(IRBuilder& builder)
{
    // continue here tomorrow
    ExpList& exp = *elist;
    NameList& var = *nlist;
    for (unsigned int i = 0; i < var.size(); i++)
    {
        SymbolInfo* info = get_scope()->find_in_table((std::string)*var[i]);
        if (info == NULL) {
            serror(get_line(), "Variable not in Symbol Table.");
        }
        else
        {
            lltype t = info->get_type();
           	int loc_reg;
			if (!info->is_defined())
			{
            	loc_reg = builder.get_local_register();
            	info->set_reg(loc_reg);
				info->define();
				if (t == SymbolInfo::LL_STRING)
					builder.add_ins(new Alloc(loc_reg, convert(t), 256));
				else
					builder.add_ins(new Alloc(loc_reg, convert(t)));
				info->set_reg(loc_reg);
			}
			else loc_reg = info->get_reg();
			builder.add_ins(new Storeop(exp[i]->gen_exp_code(builder), ARG_REG, convert(info->get_type()), loc_reg, info->is_global()));

			/*
			if (exp[i]->is_constant())
			{
				llvmtype ir_type = convert(info->get_type());
				switch(t)
				{
				case SymbolInfo::LL_INTEGER:
					builder.add_ins(new Storeop(((NumberExp*)exp[i])->getint(), ARG_IMM, ir_type, loc_reg, info->is_global()));
					break;
				case SymbolInfo::LL_FLOAT:
					builder.add_ins(new Storeop(((NumberExp*)exp[i])->getfloat(), loc_reg, info->is_global()));
					break;
				case SymbolInfo::LL_STRING:
					//builder.add_ins(new Storeop(exp[i]->getint(), ARG_IMM, convert(LL_STRING), reg, info->is_global());
					break;
				case SymbolInfo::LL_NIL:
					//
					break;
				case SymbolInfo::LL_BOOL:
					builder.add_ins(new Storeop(((BoolExp*)exp[i])->getBoolVal(), loc_reg, info->is_global()));
					break;
				default: serror(get_line(), "Unrecognized constant type in expression");
					break;
				}
			}
			else
			{
				llvmtype ir_type = convert(info->get_type());
				builder.add_ins(new Storeop(exp[i]->gen_exp_code(builder), ARG_REG, ir_type, loc_reg, info->is_global()));
			}
			*/
        }
    }
}

SymbolInfo::llvm_type LocAssignSt::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	ExpList& exp = *elist;
    NameList& var = *nlist;
    for (unsigned int i = 0; i < nlist->size(); i++)
    {
        SymbolInfo *info = builder.find_in_current_scope(*var[i]);
        if (info == NULL)
        {
            // if variable does not exist, create entry in local symbol table
            if (i < elist->size())
                builder.add_local(*var[i], new SymbolInfo(exp[i]->infer_type(builder)));
            else
                builder.add_local(*var[i], new SymbolInfo(SymbolInfo::LL_NIL));
        }
        else
        {
            // if variable exists, ensure that type of explist matches varlist
            if (i < elist->size())
            {
                if ( var[i]->infer_type(builder) != exp[i]->infer_type(builder) ) {
                    serror(get_line(), "Existing Variable is assigned expression of different type in same scope.\n");
                }
            }
            // if no exp is given, assign var to nill
            else if (info->type != SymbolInfo::LL_NIL)
				{
            	serror(get_line(), "Existing Variable is assigned expression of different type in same scope.\n");
				}
        }
    }
    return SymbolInfo::NO_TYPE;
}


SymbolInfo::llvm_type LocDefSt::infer_type(TableBuilder& builder)
{
  NameList& name = *nlist;

  set_scope(builder.get_current_scope());


  for (unsigned int i=0; i<nlist->size(); i++){

    //Check if it exists already....
    // std::string& nameref2 = static_cast<std::string>(*name[i]);

    SymbolInfo* info = builder.find_in_current_scope(static_cast<std::string>(*name[i]));


    //if not found...
    if(info==NULL){
      //  std::string& nameref = static_cast<std::string>(*name[i]);

      //Add to Local Symbol Table
      builder.add_local(static_cast<std::string>(*name[i]),new SymbolInfo(SymbolInfo::LL_NIL));
    }
    //else, if fouund
    else{
      //Give error! Already exists!
      serror(get_line(),"Variable was defined already!\n");
    }




  }

  return SymbolInfo::NO_TYPE;
}

void LocDefSt::gen_code(IRBuilder& builder){
  NameList& name = *nlist;
  for(unsigned int i=0; i<nlist->size(); i++){
    name[i]->name_gen_code_local(builder);
  }
}

SymbolInfo::llvm_type Return::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	if(ret!=NULL)
	{
		if (ret->size() > 1) serror(get_line(), "Compiler does not support returning multiple values.");
		SymbolInfo::llvm_type t = (*ret)[0]->infer_type(builder);
		return t;
	}
	else
	{
		return SymbolInfo::LL_VOID;
	}
}


/*
SymbolInfo::llvm_type Return::infer_return_type(TableBuilder& builder)
{
	Exp* val = ret->operator[](0);
	return val->get_type();
}
*/

void Return::gen_code(IRBuilder& builder){
  int retval=-1;
  if(ret!=NULL){
    Exp* val = ret->operator[](0);
    retval = val->gen_exp_code(builder);
    builder.add_ins(new ReturnFromFunc(convert(val->get_type()), retval, ARG_REG));
  }
  else{
    builder.add_ins(new ReturnFromFunc(ARG_VOID));
  }


}


SymbolInfo::llvm_type Break::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  return SymbolInfo::NO_TYPE;
}



// IfThen Statement class
SymbolInfo::llvm_type IfThenSt::infer_type(TableBuilder& builder)
{
	IfThenSt& list = *this;
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type t_return = SymbolInfo::NO_TYPE;
	bool unknown_found = false;
	for (unsigned int i=0; i < this->size(); i++)
	{
		if ((list[i].first)->infer_type(builder)!=SymbolInfo::LL_BOOL){
    		//if one of the predicates is not a bool, gotta exit with error!
			serror(get_line(), "Non-Boolean used in If Statemetn Predicate");
    	}
    	SymbolInfo::llvm_type t = list[i].second->infer_type(builder);
		if (t == SymbolInfo::LL_UNKNOWN) unknown_found = true;
		else if (t == SymbolInfo::NO_TYPE) continue;
		else
		{
			if (t == t_return) continue;
			else if (t_return == SymbolInfo::NO_TYPE) t_return = t;
			else serror(list[i].first->get_line(), "Multiple return types in if statement.");
		}
	}

    if (list.final!=NULL)
	{
		SymbolInfo::llvm_type t = list.final->infer_type(builder);
		if (t == SymbolInfo::LL_UNKNOWN) unknown_found = true;
		else if (t != SymbolInfo::NO_TYPE)
		{
			if (t_return == SymbolInfo::NO_TYPE) t_return = t;
			else if (t != t_return)  serror(list.final->get_line(), "Multiple return types in if statement.");
		}
    }
	return t_return;
}

void IfThenSt::gen_code(IRBuilder& builder)
{
	unsigned int after_label;
	after_label = builder.get_local_register();

	int next_test_label;
	int block_label;
	int exp_out;
	for (unsigned int i = 0; i < size(); i++)
	{
		// generate predicate code
		next_test_label = builder.get_local_register();
		block_label = builder.get_local_register();
		exp_out = operator[](i).first->gen_exp_code(builder);

		builder.add_ins(new CondJump(exp_out, block_label, next_test_label));

		// generate block code
		builder.create_bb(block_label);
		operator[](i).second->gen_code(builder);
		builder.add_ins(new CondJump(after_label));
		builder.create_bb(next_test_label);
	}
	if (final)
	{
		final->gen_code(builder);
	}
	builder.add_ins(new CondJump(after_label));
	builder.create_bb(after_label);


}

IfThenSt::IfThenSt() : AstNode(-1), final(0) {}

void IfThenSt::print()
{
	print_group("IfThen");
}

void IfThenSt::add_else(Block* ptr)
{
	final = ptr;
}

void IfThenSt::add_elif(Exp *ex, Block *blk)
{
	push_back(IfPair(ex,blk));
}

void IfThenSt::add_if(Exp *ex, Block *blk)
{
	insert(begin(), IfPair(ex,blk));
}

void IfThenSt::print_children() {
	fputc('\n', yyout);
	Indenter& ind = Indenter::GetIndenter();
	ind.inc();
    for (unsigned int i = 0; i < this->size(); i++) {
    	ind.indent();
    	operator[](i).first->print();
    	fputc('\n', yyout);
		ind.indent();
		operator[](i).second->print();
    	fputc('\n', yyout);
    }
	if (final) {
		ind.indent();
    	final->print();
    	fputc('\n', yyout);
	}
	ind.dec();
	ind.indent();
}

IfThenSt::~IfThenSt()
{
	for (unsigned int i = 0; i < size(); i++)
	{
		delete operator[](i).first;
		delete operator[](i).second;
	}
}

// For Iter Loop

ForIterSt::~ForIterSt()
{
	delete names; delete exps; delete blk;
}

void ForIterSt::print_children()
{
	names->print();
	fputc(' ', yyout);
	exps->print();
	fputc(' ', yyout);
	blk->print();
}

// ForLoop class
ForLoopSt::ForLoopSt(Name *name, Exp *st, Exp* en, Block* code, int ln, Exp* in) :
	AstNode(ln), nm(name), start(st), end(en), inc(in), blk(code)
{}

ForLoopSt::~ForLoopSt()
{
	delete nm;
	delete start;
	delete end;
	if (inc) delete inc;
	delete blk;
}

void ForLoopSt::print_children()
{
	nm->print();
	fputc(' ', yyout);
	start->print();
	fputc(' ', yyout);
	end->print();
	if (inc) inc->print();
	blk->print();
}

