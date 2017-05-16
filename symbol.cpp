#include "symbol.h"

int Symbol::ID = 0;
std::unordered_map<std::string, Symbol*> Symbol::mapping;

Symbol::Symbol(std::string name, int section, int offset){
	this->name = name;
	this->section = section;
	this->offset = offset;
	this->binding = 'L';
	this->serial = ID++;
}

Symbol* Symbol::getSymbol(std::string name){
	return Symbol::mapping[name];
}

void Symbol::addSymbol(Symbol* symbol){
	Symbol::mapping[symbol->name] = symbol;
}


