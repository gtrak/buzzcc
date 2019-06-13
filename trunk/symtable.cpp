#include "symtable.h"
#include <cassert>

SymbolTable* SymbolTable::add_scope() 
{
	SymbolTable* ptr = new SymbolTable(this);
	nested_scopes.push_back(ptr);
	return ptr;
}

SymbolTable* SymbolTable::insert_scope(int offset) 
{
	SymbolTable* ptr = new SymbolTable(this);
	nested_scopes.insert(nested_scopes.begin()+offset, ptr);
	return ptr;
}

SymbolInfo* SymbolTable::find_local(const std::string& key) 
{
	NameMap::iterator itr = table.find(key);
	if (itr != table.end())
		return itr->second;
	else return NULL;
}

SymbolInfo* SymbolTable::find_in_table(const std::string& key) 
{
	SymbolInfo *match = 0;
	SymbolTable *s_look = this;
	do {
		match = s_look->find_local(key);
		if (match == NULL) continue;
		else return match;
	} while ((s_look = s_look->get_parent()) != NULL);
    return NULL;
}

SymbolInfo* SymbolTable::find_defined(const std::string& key) 
{
	SymbolInfo *match = 0;
	SymbolTable *s_look = this;
	do {
		match = s_look->find_local(key);
		if (match == NULL) continue;
		else if (!(match->is_defined())) continue;
		return match;
	} while ((s_look = s_look->get_parent()) != NULL);
	return NULL;
}

void SymbolTable::remove(const std::string& key) 
{
	MapItr it = table.find(key);
	delete it->second;
	assert(it != table.end());
	table.erase(it);
}

void SymbolTable::set_parent(SymbolTable* padre) { parent = padre; }

SymbolTable* SymbolTable::get_parent() { return parent; }

int SymbolTable::get_num_child() { return nested_scopes.size(); }

SymbolTable::SymbolTable() : parent(NULL) {}

SymbolTable::SymbolTable(SymbolTable* padre) : parent(padre) {}

SymbolTable::~SymbolTable() 
{
	ScopeItr it;
	for (ScopeItr it = nested_scopes.begin(); it != nested_scopes.end(); it++) {
		delete *it;
	}
	for (MapItr it = table.begin(); it != table.end(); it++) {
		delete it->second;
	}
}


void TableBuilder::revert_global_writes()
{
	unsigned int remove_count;
	if (num_writes.size() > 0) { 
		remove_count = global_writes.size() - num_writes.back();
		num_writes.pop_back();
	}
	else remove_count = global_writes.size();
	//printf("Revert global writes: %d\n", remove_count);
	for (unsigned int i = 0; i < remove_count; i++)
	{
		//printf("Reverting write: %s\n", global_writes.back().c_str());
		global->remove(global_writes.back());
		global_writes.pop_back();
	}
}

void TableBuilder::add_local(const std::string& key, SymbolInfo *val)
{
	TableEntry te(key,val);
	cur_scope->insert_symbol(te);
}
void TableBuilder::add_global(const std::string& key, SymbolInfo *val)
{
	if (recording) {
		global_writes.push_back(key);
	}
	//printf("Add global %s %d\n", key.c_str(), recording);
	global->insert_symbol(TableEntry(key,val));
	val->make_global();
}
