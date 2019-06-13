#if !defined(SYMTABLE_H)
#define SYMTABLE_H
#include <map>
#include <vector>
#include <string>

// Two steps in backend
// 1. Type inference and symbol table construction
// 2. llvm IR generation

// At a later time should use
// #include <ext/hash_map>
// namespace std { using namespace __gnu_cxx; }
// ... on second thought this is deprecated and we shouldn't use c++0x <unordered_map>


class AstNode;
class FuncDeclSt;

struct SymbolInfo
{
	// Used to establish type of statement
	enum llvm_type { LL_BOOL, LL_STRING, LL_FLOAT, LL_INTEGER, LL_TABLE, LL_NIL, NO_TYPE, LL_UNKNOWN, LL_VOID} type;

	// Used to store part of tree used for function declaration
	FuncDeclSt* func;

	// Used to hold argument types of function
	typedef std::vector<llvm_type> ArgTypeList;
	ArgTypeList func_args;

	bool defined;
	bool global;
	int latest_register;

	// reference back to ast node of statement
	SymbolInfo() : type(LL_UNKNOWN), func(NULL), defined(false), global(false) {}
	SymbolInfo(llvm_type t, FuncDeclSt* p = NULL) : type(t), func(p), defined(false), global(false) {}
	void add_func_arg(llvm_type t) { func_args.push_back(t); }
	ArgTypeList& get_args() { return func_args; }
	bool equal_args(ArgTypeList& ref) {
		if (ref.size() != func_args.size()) return false;
		for (unsigned int i = 0; i < func_args.size(); i++) {
			if (ref[i] != func_args[i]) return false;
		}
		return true;
	}
	void set_type(llvm_type t) { type = t; }
	llvm_type get_type() { return type; }
	FuncDeclSt* get_func_decl() { return func; }
	void set_func_decl(FuncDeclSt* node) { func = node; }
	void set_reg(int i) { latest_register = i; }
	int get_reg() { return latest_register; }
	bool is_defined() { return defined; }
	void define() { defined = true; }
	bool is_global() { return global; }
	void make_global() { global = true; }
	// std::string func_name;
	// std::string llvm_register
};

class SymbolTable {
public:
	// table and children objects
	typedef std::map<std::string, SymbolInfo*> NameMap;
	typedef std::vector<SymbolTable*> ScopeList;

	// iterators
	typedef NameMap::iterator MapItr;
	typedef ScopeList::iterator ScopeItr;
	typedef std::pair<MapItr,MapItr> ItrPair;

	// table entry
	typedef std::pair<std::string,SymbolInfo*> TableEntry;

	// parent
	SymbolTable *parent;

private:
	NameMap table;
	ScopeList nested_scopes;
public:
	SymbolTable* add_scope();
	SymbolTable* insert_scope(int offset);
	SymbolInfo* find_local(const std::string& key);

	MapItr end() { return table.end(); }
	void insert_symbol(const TableEntry& entry) { table.insert(entry); }
	ItrPair itr_table() { return ItrPair(table.begin(), table.end()); }
	int get_table_size() { return table.size(); }
	SymbolTable* get_child(int offset) { return nested_scopes[offset]; }

	void remove_last_scope() { delete nested_scopes.back(); nested_scopes.pop_back(); }
	SymbolInfo* find_in_table(const std::string& key);
	SymbolInfo* find_defined(const std::string& key);
	void remove(const std::string& key);
	void set_parent(SymbolTable* padre);
	SymbolTable* get_parent();
	int get_num_child();

	SymbolTable();
	SymbolTable(SymbolTable* padre);
	~SymbolTable();
};

class TableBuilder
{
public:
	// will contain: add_scope, ascend hierachy, descend, etc
	// Contains stack of references to higher scopes

	typedef SymbolTable::ItrPair ItrPair;
	typedef SymbolTable::MapItr MapItr;
	typedef SymbolTable::TableEntry TableEntry;
	
	std::vector<std::string> global_writes;	
	std::vector<int> num_writes;
	bool recording;
	std::vector<bool> prev_state;

private:
	SymbolTable *global;
	SymbolTable *cur_scope;
public:
	TableBuilder(SymbolTable* root) {
		global = root;
		cur_scope = global;
		recording = false;
	}
	~TableBuilder() {
	}
	// new scope creates a new symbol table layer and makes all table operations use that new table
	void new_scope() {
		cur_scope = cur_scope->add_scope();
	}
	// goes up a scope
	void ascend() {
		cur_scope = cur_scope->get_parent();
	}
	//void set_node_scope(AstNode* node) { node->set_scope(cur); }
	// looks through all stack functions (returns null if nothing found)
	SymbolInfo* find(const std::string& key) {
		return cur_scope->find_in_table(key);
	}
	SymbolInfo* find_global(const std::string& key) {
		return global->find_in_table(key);
	}
	SymbolInfo* find_in_current_scope(const std::string& key) {
		SymbolInfo *match = NULL;
		match = cur_scope->find_local(key);
		return match;
	}
	void record_global_writes() { 
		if (recording == true) {
			num_writes.push_back(global_writes.size());
		}
		recording = true; 
	}
	void stop_record() { prev_state.push_back(recording); recording = false; }
	void resume_record() { recording  = prev_state.back(); prev_state.pop_back(); }
	void destroy_scope() { ascend(); cur_scope->remove_last_scope(); }
	void revert_global_writes();
	// returns array of ptrs to different values that match key
	// vector is empty if
	/*
	SymVec find_multiple(std::string& key) {
		SymVec sym;
		for (int i = scopes.size()-1; i >= 0; i--)
		{
			ItrPair match(scopes[i]->find_multiple(key));
			if (match.first == scopes[i]->end())
				continue;
			else {
				MapItr it = match.first;
				for (; it != match.second; it++)
				{
					sym.push_back(it->second);
				}
		turn sym;
			}
		}
		return sym;
	}
	*/
	// Add local symbol to table
	// symbol table takes ownership of pointer
	void add_local(const std::string& key, SymbolInfo *val);
	void add_global(const std::string& key, SymbolInfo *val);
	SymbolTable* get_current_scope() { return cur_scope; }
	void set_current_scope(SymbolTable* scope) { cur_scope = scope; }
};


#endif
