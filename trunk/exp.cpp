#include "exp.h"
#include <cassert>

int ConcatExp::gen_exp_code(IRBuilder& builder)
{
	int dest_reg = builder.get_local_register();
	int ret_reg = builder.get_local_register();
	int lreg = ex1->gen_exp_code(builder);
	int rreg = ex2->gen_exp_code(builder);
	int rreg_loc = builder.get_local_register();
	int return_val = builder.get_local_register();


    std::vector<llvmtype> converted_args;
    std::vector<int> arg_registers;
	
	builder.add_ins(new Alloc(dest_reg, convert(SymbolInfo::LL_STRING), 256));
	builder.add_ins(new Alloc(rreg_loc, convert(SymbolInfo::LL_STRING), 256));
	builder.add_ins(new Storeop(lreg,  ARG_REG, convert(SymbolInfo::LL_STRING), dest_reg));
	builder.add_ins(new Storeop(rreg, ARG_REG, convert(SymbolInfo::LL_STRING), rreg_loc));
	
	arg_registers.push_back(dest_reg); arg_registers.push_back(rreg_loc);
	converted_args.push_back(convert(SymbolInfo::LL_STRING));
	converted_args.push_back(convert(SymbolInfo::LL_STRING));
    builder.add_ins(new CallFunction(ret_reg, "strcat", convert(SymbolInfo::LL_STRING), &converted_args, &arg_registers, true, CSTRCAT));
	builder.add_ins(new Loadop(return_val, convert(SymbolInfo::LL_STRING), dest_reg, false));
	return return_val;
}

int BoolExp::gen_exp_code(IRBuilder& builder){
  	int local_register = builder.get_local_register();
	int val_register = builder.get_local_register();
	
	builder.add_ins(new Alloc(local_register, convert(SymbolInfo::LL_BOOL)));
	builder.add_ins(new Storeop((bool)getBoolVal(), local_register, false));
	builder.add_ins(new Loadop(val_register, convert(get_type()), local_register, false));
	return val_register;
}


SymbolInfo::llvm_type NilExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	set_type(SymbolInfo::LL_NIL);
	return get_type();
}

int NilExp::gen_exp_code(IRBuilder& builder)
{
  	int local_register = builder.get_local_register();
	int val_register = builder.get_local_register();
	builder.add_ins(new Alloc(local_register, convert(SymbolInfo::LL_BOOL)));
	builder.add_ins(new Storeop(false, local_register, false));
	builder.add_ins(new Loadop(val_register, convert(SymbolInfo::LL_BOOL), local_register, false));
	
	return val_register;
}

// Number Exp class 
SymbolInfo::llvm_type NumberExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	if (NumberExp::getNumberType()){   // Is 1, Float
	  set_type(SymbolInfo::LL_FLOAT); 
  	}
  	else {    //is 0, Int
	  set_type(SymbolInfo::LL_INTEGER);  
  	}
	return get_type();
}

int NumberExp::gen_exp_code(IRBuilder& builder)
{
	int local_register = builder.get_local_register();
	int val_register = builder.get_local_register();
	if (type == NUM_INT)
	{
		builder.add_ins(new Alloc(local_register, convert(SymbolInfo::LL_INTEGER)));
		builder.add_ins(new Storeop(getint(), ARG_IMM, convert(get_type()), local_register, false));
		builder.add_ins(new Loadop(val_register, convert(get_type()), local_register, false));
	}
	else 
	{
		builder.add_ins(new Alloc(local_register, convert(SymbolInfo::LL_FLOAT)));
		builder.add_ins(new Storeop(getfloat(), local_register, false));
		builder.add_ins(new Loadop(val_register, convert(get_type()), local_register, false));
	}
	return val_register;
}


/*
int NumberExp::gen_exp_code(IRBuilder& builder)
{
	int reg = builder.get_local_register();
	Instruction* ins;
	if (type == NUM_INT) 
	{
		ins = new Assign(reg, convert(SymbolInfo::LL_INTEGER), (int)num);
	}
	else if (type == NUM_FLOAT) 
	{
		ins = new Assign(reg, convert(SymbolInfo::LL_FLOAT), num);
	}
	builder.add_ins(ins);
	return reg;
}
*/


SymbolInfo::llvm_type StringExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	set_type(SymbolInfo::LL_STRING);
	return get_type();
}

int StringExp::gen_exp_code(IRBuilder& builder)
{
  int globalreg = builder.get_global_reg();
  std::string name("ptr");
  std::string mystring(str);
  add_nulls(mystring,256 - mystring.size());

  name.append(itos(globalreg));
  builder.add_global_node(new Global(mystring,name,256));

  int local_register = builder.get_local_register();
  builder.add_ins(new Loadop(local_register, convert(SymbolInfo::LL_STRING), globalreg, true));
	
  return local_register;
}


SymbolInfo::llvm_type ElipsisExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  set_type(SymbolInfo::NO_TYPE);
  return get_type();
}

SymbolInfo::llvm_type FuncExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  serror(get_line(),"Invalid Function Definition: Closure");
  return SymbolInfo::LL_UNKNOWN;
}

SymbolInfo::llvm_type PrefixExpExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  set_type(exp->infer_type(builder));
  return get_type();
}

SymbolInfo::llvm_type TableExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  set_type(SymbolInfo::LL_TABLE);
  return get_type();
}

int BinOpExp::gen_code_binop(IRBuilder& builder, ops operation)
{
    bool ex1_constant = ex1->is_constant();
    bool ex2_constant = ex2->is_constant();
	float src1, src2;
	argtype arg1_type, arg2_type;
	//printf("constant:%b %b\n", ex1_constant, ex2_constant);

	if (!ex1_constant)
	{
		src1 = (float) ex1->gen_exp_code(builder);
		arg1_type = ARG_REG;		
	}
	else 
	{
		if (ex1->get_type() == SymbolInfo::LL_FLOAT) 
			src1 = static_cast<NumberExp*>(ex1)->getfloat();
		else if (ex1->get_type() == SymbolInfo::LL_INTEGER) 
			src1 = static_cast<NumberExp*>(ex1)->getint();
		else { serror(get_type(), "Unsupported type in binop."); } 
		arg1_type = ARG_IMM;
	}

	if (!ex2_constant)
	{
		src2 = (float) ex2->gen_exp_code(builder);
		arg2_type = ARG_REG;
	}
	else 
	{
		if (ex2->get_type() == SymbolInfo::LL_FLOAT) 
			src2 = static_cast<NumberExp*>(ex2)->getfloat();
		else if (ex2->get_type() == SymbolInfo::LL_INTEGER) 
			src2 = static_cast<NumberExp*>(ex2)->getint();
		else { serror(get_type(), "Unsupported type in binop."); } 
		arg2_type = ARG_IMM;
	}

	//printf("registers: %f %f\n", src1, src2);	
  	SymbolInfo::llvm_type type = get_type(); 
	int t_reg = builder.get_local_register();
	SymbolInfo::llvm_type ex1_type = ex1->get_type();
	SymbolInfo::llvm_type ex2_type = ex2->get_type();
	if (type == SymbolInfo::LL_FLOAT)
	{
		builder.add_ins(new BinopFloat(t_reg, operation, src1, src2, arg1_type, arg2_type));
	}
	else if (type == SymbolInfo::LL_INTEGER)
	{
		builder.add_ins(new BinopInt(t_reg, operation, src1, src2, arg1_type, arg2_type));
	}
	else if (type == SymbolInfo::LL_BOOL) // logical operations and comparisons 
	{	
		if (ex1_type == SymbolInfo::LL_FLOAT && ex2_type == SymbolInfo::LL_FLOAT)
			builder.add_ins(new BinopFloat(t_reg, operation, src1, src2, arg1_type, arg2_type));
		else if (ex1_type == SymbolInfo::LL_INTEGER && ex2_type == SymbolInfo::LL_INTEGER)
			builder.add_ins(new BinopInt(t_reg, operation, src1, src2, arg1_type, arg2_type));
		assert(ex1_type == ex2_type);
	}
	else 
	{
		serror(get_type(), "Unsupported type in binop."); 
	}
	return t_reg;
}

int AddExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_ADD); 
}

SymbolInfo::llvm_type AddExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = (ex1)->infer_type(builder);
  ex2_type = (ex2)->infer_type(builder);

  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }
  
  //If either ex1 or ex2 are neither float nor integer nor string, error!
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either is string, have to make sure that what is in quotes is a numeric value!
  
  
  //This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
  ex1_type = ex1->typecoercion();        
  
  ex2_type = ex2->typecoercion();   
  
  
  //Now that we did coercion, they should all be either int or string, erlse, error
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }
  
  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }
  return get_type();
}


int SubExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_SUB); 
}

SymbolInfo::llvm_type SubExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = (ex1)->infer_type(builder);
  ex2_type = (ex2)->infer_type(builder);
  
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }

  //If either ex1 or ex2 are neither float nor integer nor string, error!
  if (ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either is string, have to make sure that what is in quotes is a numeric value!
  
  
  //This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
  ex1_type = ex1->typecoercion();        
  
  ex2_type = ex2->typecoercion();   
  
  
  //Now that we did coercion, they should all be either int or string, erlse, error
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }
  
  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }
  return get_type();
}

int MultExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_MUL); 
}

SymbolInfo::llvm_type MultExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = (ex1)->infer_type(builder);
  ex2_type = (ex2)->infer_type(builder);

  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }
  
  //If either ex1 or ex2 are neither float nor integer nor string, error!
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either is string, have to make sure that what is in quotes is a numeric value!
  
  
  //This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
  ex1_type = ex1->typecoercion();        
  
  ex2_type = ex2->typecoercion();   
  
  
  //Now that we did coercion, they should all be either int or string, erlse, error
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }
  
  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }
  return get_type();
}

int DivExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_DIV); 
}

SymbolInfo::llvm_type DivExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type ex1_type;
	SymbolInfo::llvm_type ex2_type;
  
	ex1_type = (ex1)->infer_type(builder);
	ex2_type = (ex2)->infer_type(builder);
    
    // if any type cannot be inferred, expression cannot be inferred 
    if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
    {
	  set_type(SymbolInfo::LL_UNKNOWN);
      return SymbolInfo::LL_UNKNOWN;
    }
  
	//If either ex1 or ex2 are neither float nor integer nor string, error!
	if (ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
		serror(get_line(),"Non-numeric expression passed");
	}
  
  
	//If either is string, have to make sure that what is in quotes is a numeric value!
	//This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
	ex1_type = ex1->typecoercion();        
	ex2_type = ex2->typecoercion();   
  
	//Now that we did coercion, they should all be either int or string, erlse, error
	if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
		serror(get_line(),"Non-numeric expression passed");
	}  
  
	//If either ex1 is float or ex2 is float, the result is float
  	if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
		set_type(SymbolInfo::LL_FLOAT);
	}
  
	//If both are int, then return int
  	else{
		set_type(SymbolInfo::LL_INTEGER);
	}
	return get_type();
}

int PowExp::gen_exp_code(IRBuilder& builder)
{
	int base_reg = ex1->gen_exp_code(builder);
	int pow_reg  = ex2->gen_exp_code(builder);
	int dest_reg = builder.get_local_register();
	
	// does not support type conversion of its operands 	
	std::vector<llvmtype> arg_types;
	std::vector<int> arg_reg;
	
	arg_types.push_back(convert(SymbolInfo::LL_FLOAT));
	arg_types.push_back(convert(SymbolInfo::LL_FLOAT));
	if (get_type() == SymbolInfo::LL_FLOAT)
	{
		arg_reg.push_back(base_reg);
		arg_reg.push_back(pow_reg);	
		builder.add_ins(new CallFunction(dest_reg, "powf", convert(SymbolInfo::LL_FLOAT), &arg_types, &arg_reg, true, CPOW));
	}
	else 
	{
		int fp_base = builder.get_local_register();
		int fp_pow 	= builder.get_local_register();
		int fp_result = builder.get_local_register();
		arg_reg.push_back(fp_base);
		arg_reg.push_back(fp_pow);

		builder.add_ins(new Sitofp(fp_base, base_reg));
		builder.add_ins(new Sitofp(fp_pow, pow_reg));
		builder.add_ins(new CallFunction(fp_result, "powf", convert(SymbolInfo::LL_FLOAT), &arg_types, &arg_reg, true, CPOW));
		builder.add_ins(new Fptosi(dest_reg, fp_result));
	}
	return dest_reg;
}

SymbolInfo::llvm_type PowExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type ex1_type;
	SymbolInfo::llvm_type ex2_type;
  
	ex1_type = (ex1)->infer_type(builder);
	ex2_type = (ex2)->infer_type(builder);
  
    // if any type cannot be inferred, expression cannot be inferred 
    if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
    {
	  set_type(SymbolInfo::LL_UNKNOWN);
      return SymbolInfo::LL_UNKNOWN;
    }

	//If either ex1 or ex2 are neither float nor integer nor string, error!
	if (ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
		serror(get_line(),"Non-numeric expression passed");
 	}
  
  
	//If either is string, have to make sure that what is in quotes is a numeric value!
  
  
	//This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
	ex1_type = ex1->typecoercion();        
	ex2_type = ex2->typecoercion();   
  
	//Now that we did coercion, they should all be either int or string, erlse, error
	if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
		serror(get_line(),"Non-numeric expression passed");
	}  
  
	//If either ex1 is float or ex2 is float, the result is float
	if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
		set_type(SymbolInfo::LL_FLOAT);
	}
  
	//If both are int, then return int
	else{
		set_type(SymbolInfo::LL_INTEGER);
	}
	return get_type();
}

int ModExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_MOD); 
}

SymbolInfo::llvm_type ModExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = (ex1)->infer_type(builder);
  ex2_type = (ex2)->infer_type(builder);
  
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }

  //If either ex1 or ex2 are neither float nor integer nor string, error!
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  
  //If either is string, have to make sure that what is in quotes is a numeric value!
  
  
  //This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
  ex1_type = ex1->typecoercion();        
  
  ex2_type = ex2->typecoercion();   
  
  
  //Now that we did coercion, they should all be either int or string, erlse, error
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }
  
  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }
  return get_type();
}

SymbolInfo::llvm_type ConcatExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  //Accepts numbers as inputs also because of corecion.....but returns
  //string type at all times....soo........
  //check if inputs are number (int float) or string...else error
  //always return string
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = ex1->infer_type(builder);
  ex2_type = ex2->infer_type(builder);

  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }
  
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER && ex1_type != SymbolInfo::LL_STRING && ex2_type != SymbolInfo::LL_STRING){
    serror(get_line(),"Non-numeric expression passed");
  }
  
  set_type(SymbolInfo::LL_STRING);
  return get_type();
}


int LTExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_LT);
}

SymbolInfo::llvm_type LTExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  ex1_type = ex1->infer_type(builder);        
  ex2_type = ex2->infer_type(builder);   
  
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }

  //if they are the same, set type to that
  if(ex1_type == ex2_type){
    set_type(SymbolInfo::LL_BOOL);
  }
  //else it is error
  else{
    serror(get_line(),"Different Types on Less Than Comparison");
  }
  return get_type();
  
  
}

int LEExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_LE);
}

SymbolInfo::llvm_type LEExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  ex1_type = ex1->infer_type(builder);        
  ex2_type = ex2->infer_type(builder);   
  
  
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }
  //if they are the same, set type to that
  if(ex1_type == ex2_type){
    set_type(SymbolInfo::LL_BOOL);
  }
  //else it is error
  else{
    serror(get_line(),"Different Types on Less Than Equal Comparison");
  }
  return get_type();
  
}


int GTExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_GT);
}

SymbolInfo::llvm_type GTExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  ex1_type = ex1->infer_type(builder);        
  ex2_type = ex2->infer_type(builder);   
  
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }

  //if they are the same, set type to that
  if(ex1_type == ex2_type){
    set_type(SymbolInfo::LL_BOOL);
  }
  //else it is error
  else{
    serror(get_line(),"Different Types on Greater Than Comparison");
  }
  return get_type();
}

int GEExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_GE);
}

SymbolInfo::llvm_type GEExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  ex1_type = ex1->infer_type(builder);        
  ex2_type = ex2->infer_type(builder);   
 
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
	 set_type(SymbolInfo::LL_UNKNOWN);
     return SymbolInfo::LL_UNKNOWN;
  }

  
  //if they are the same, set type to that
  if(ex1_type == ex2_type){
    set_type(SymbolInfo::LL_BOOL);
  }
  //else it is error
  else{
    serror(get_line(),"Different Types on Greater Than Equal Comparison");
  }
  return get_type();
  
}


int EqualExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_EQ);
}

SymbolInfo::llvm_type EqualExp::infer_type(TableBuilder& builder)
{ 
	SymbolInfo::llvm_type ex1_type = ex1->infer_type(builder);
	SymbolInfo::llvm_type ex2_type = ex2->infer_type(builder);
    // if any type cannot be inferred, expression cannot be inferred 
    if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
    {
		set_type(SymbolInfo::LL_UNKNOWN);
    	return SymbolInfo::LL_UNKNOWN;
  	}
	if (ex1_type != ex2_type) 
		serror(get_line(), "Arguments to equality expression are not the same type.");
	set_scope(builder.get_current_scope());
	set_type(SymbolInfo::LL_BOOL);
	return get_type();
}

int NotEqualExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_binop(builder, OP_NE);
}

SymbolInfo::llvm_type NotEqualExp::infer_type(TableBuilder& builder)
{
	SymbolInfo::llvm_type ex1_type = ex1->infer_type(builder);
	SymbolInfo::llvm_type ex2_type = ex2->infer_type(builder);
    // if any type cannot be inferred, expression cannot be inferred 
    if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
    {
		set_type(SymbolInfo::LL_UNKNOWN);
    	return SymbolInfo::LL_UNKNOWN;
  	}
	if (ex1_type != ex2_type) 
		serror(get_line(), "Arguments to equality expression are not the same type.");
	set_scope(builder.get_current_scope());
	set_type(SymbolInfo::LL_BOOL);
	return get_type();
}

int BinOpExp::gen_code_logop(IRBuilder& builder, ops operation)
{
    bool ex1_constant = ex1->is_constant();
    bool ex2_constant = ex2->is_constant();
	float src1, src2;
	argtype arg1_type, arg2_type;

	if (!ex1_constant)
	{
		src1 = (float) ex1->gen_exp_code(builder);
		arg1_type = ARG_REG;		
	}
	else 
	{
		if (ex1->get_type() == SymbolInfo::LL_BOOL) 
			src1 =  ex1->getBoolVal() ? 0.0 : 1.0;
		else { serror(get_type(), "Unsupported type in boolean bin op."); } 
		arg1_type = ARG_IMM;
	}

	if (!ex2_constant)
	{
		src2 = (float) ex2->gen_exp_code(builder);
		arg2_type = ARG_REG;
	}
	else 
	{
		if (ex2->get_type() == SymbolInfo::LL_BOOL) 
			src2 =  ex2->getBoolVal() ? 0.0 : 1.0;
		else { serror(get_type(), "Unsupported type in boolean bin op."); } 
		arg2_type = ARG_IMM;
	}
	
	int t_reg = builder.get_local_register();
	builder.add_ins(new BinopBool(t_reg, operation, src1, src2, arg1_type, arg2_type));
	return t_reg;
}

int AndExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_logop(builder, OP_AND);
}

SymbolInfo::llvm_type AndExp::infer_type(TableBuilder& builder)
{
  set_scope(builder.get_current_scope());
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  ex1_type = ex1->infer_type(builder);        
  ex2_type = ex2->infer_type(builder);   
  // if any type cannot be inferred, expression cannot be inferred 
  if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  {
  	set_type(SymbolInfo::LL_UNKNOWN);
    return SymbolInfo::LL_UNKNOWN;
  }
  
  if(ex1_type == SymbolInfo::LL_NIL || (ex1_type == SymbolInfo::LL_BOOL && ex1->getBoolVal()==false)){
    set_type(ex1_type);
  }
  else{
    set_type(ex2_type);
  }
  
  return get_type();
}

int OrExp::gen_exp_code(IRBuilder& builder)
{
	return gen_code_logop(builder, OP_OR);
}

SymbolInfo::llvm_type OrExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type ex1_type;
	SymbolInfo::llvm_type ex2_type;
	ex1_type = ex1->infer_type(builder);        
	ex2_type = ex2->infer_type(builder);   
  	// if any type cannot be inferred, expression cannot be inferred 
  	if (ex1_type == SymbolInfo::LL_UNKNOWN || ex2_type == SymbolInfo::LL_UNKNOWN)
  	{
  	  set_type(SymbolInfo::LL_UNKNOWN);
      return SymbolInfo::LL_UNKNOWN;
    }
  
	if (ex1_type  != SymbolInfo::LL_NIL && !(ex1_type == SymbolInfo::LL_BOOL && ex1->getBoolVal()==false)){
    	set_type(ex1_type);
  	}
  	else {
    	set_type(ex2_type);
	}
  	return get_type();
}

int MinusExp::gen_exp_code(IRBuilder& builder)
{
	int dest_reg = builder.get_local_register();
	int src_reg = ex->gen_exp_code(builder);	
	SymbolInfo::llvm_type type = ex->get_type();
	assert((type == SymbolInfo::LL_FLOAT) || (type == SymbolInfo::LL_INTEGER));
	if (type == SymbolInfo::LL_FLOAT)
	{
		// float dest_reg = 0.0 - src_reg
		builder.add_ins(new BinopFloat(dest_reg, OP_SUB, 0.0, (float)src_reg, ARG_IMM, ARG_REG));
	}
	else 
	{
		// int dest_reg = 0 - src_reg
		builder.add_ins(new BinopInt(dest_reg, OP_SUB, 0, src_reg, ARG_IMM, ARG_REG));
	}
	return dest_reg;
}

SymbolInfo::llvm_type MinusExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type ex_type;
	ex_type = (ex)->infer_type(builder);
  
  	// if any type cannot be inferred, expression cannot be inferred 
  	if (ex_type == SymbolInfo::LL_UNKNOWN)
  	{
  	  set_type(SymbolInfo::LL_UNKNOWN);
      return SymbolInfo::LL_UNKNOWN;
    }
  
	//If ex is neither float nor integer nor string, error!
	if (ex_type != SymbolInfo::LL_FLOAT && ex_type != SymbolInfo::LL_INTEGER && ex_type != SymbolInfo::LL_STRING){
		serror(get_line(),"Non-numeric expression passed");
	}
  
	//If either is string, have to make sure that what is in quotes is a numeric value!
	//This function retunrs LL_STRING, LL_INTEGER, or LL_FLOAT
	ex_type = ex->typecoercion();        
  
  
	//If ex is float, the result is float
	if(ex_type == SymbolInfo::LL_FLOAT){
    	set_type(SymbolInfo::LL_FLOAT);
  	}
  
	//If int, then return int
	else if (ex_type==SymbolInfo::LL_INTEGER){
    	set_type(SymbolInfo::LL_INTEGER);
  	}
	else
	{
    	serror(get_line(),"Unary negative operand applied in non-number");
	}
	return get_type();
}

int NotExp::gen_exp_code(IRBuilder& builder)
{
	int dest_reg = builder.get_local_register();
	int src_reg = ex->gen_exp_code(builder);
	builder.add_ins(new BinopBool(dest_reg, OP_XOR, src_reg, 1, ARG_REG, ARG_IMM));
	return dest_reg;
}

SymbolInfo::llvm_type NotExp::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
  	// if any type cannot be inferred, expression cannot be inferred 
	SymbolInfo::llvm_type t = ex->infer_type(builder);
  	if (t == SymbolInfo::LL_UNKNOWN)
  	{
  	  set_type(SymbolInfo::LL_UNKNOWN);
      return SymbolInfo::LL_UNKNOWN;
    }
	set_type(SymbolInfo::LL_BOOL);
	if (t != SymbolInfo::LL_BOOL) serror(get_line(), "Non-boolean expression used with not operator.");
	return get_type();
}

SymbolInfo::llvm_type LengthExp::infer_type(TableBuilder& builder)
{ 
	serror(get_line(), "The length operator is not supported now.");
	set_scope(builder.get_current_scope());
	set_type(SymbolInfo::LL_INTEGER);
	return get_type();
}

SymbolInfo::llvm_type FuncPrefix::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type type = call->infer_type(builder);
	return type;	
}

int FuncPrefix::gen_exp_code(IRBuilder& builder)
{
	return call->gen_exp_code(builder);
}

SymbolInfo::llvm_type VarPrefix::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	return var->infer_type(builder);
}

int VarPrefix::gen_exp_code(IRBuilder& builder)
{
	SymbolInfo *info = get_scope()->find_defined(var->get_symbol_name());
	bool global = info->is_global();
	if (global) {
		int local_register = builder.get_local_register();
		builder.add_ins(new Loadop(local_register, convert(info->get_type()), info->get_reg(), global));
		return local_register;
	}
	else
	{
		int local_register = builder.get_local_register();
		builder.add_ins(new Loadop(local_register, convert(info->get_type()), info->get_reg(), global));
		return local_register;	
	}
}

SymbolInfo::llvm_type ExpPrefix::infer_type(TableBuilder& builder)
{
	set_scope(builder.get_current_scope());
	SymbolInfo::llvm_type type = exp->infer_type(builder);
	return type;	
}

int ExpPrefix::gen_exp_code(IRBuilder& builder)
{
	return exp->gen_exp_code(builder);
}

SymbolInfo::llvm_type BinOpExp::infer_type(TableBuilder& builder){
  
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;
  
  ex1_type = ex1->infer_type(builder);
  ex2_type = ex2->infer_type(builder);

  //If either ex1 or ex2 are neither float nor integer, error!
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }

  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }

  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }
  return get_type();
}

/*
SymbolInfo::llvm_type BinOpExp::infer_type(TableBuilder& builder, AstNode* node){
  
  SymbolInfo::llvm_type ex1_type;
  SymbolInfo::llvm_type ex2_type;

  ex1_type = (this->ex1)->infer_type(builder,node);
  ex2_type = (this->ex2)->infer_type(builder,node);

  //If either ex1 or ex2 are neither float nor integer, error!
  if(ex1_type != SymbolInfo::LL_FLOAT && ex1_type != SymbolInfo::LL_INTEGER && ex2_type != SymbolInfo::LL_FLOAT && ex2_type != SymbolInfo::LL_INTEGER){
    serror(get_line(),"Non-numeric expression passed");
  }

  //If either ex1 is float or ex2 is float, the result is float
  if(ex1_type == SymbolInfo::LL_FLOAT || ex2_type == SymbolInfo::LL_FLOAT){
    set_type(SymbolInfo::LL_FLOAT);
  }

  //If both are int, then return int
  else{
    set_type(SymbolInfo::LL_INTEGER);
  }

}

*/
