#include "symbol.h"

int Symbol::ID = 0;
std::unordered_map<std::string, Symbol*> Symbol::mapping;

Symbol::Symbol(std::string name, int line, std::string section, int offset){
    this->name = name;
    this->line = line;
    this->section = section;
    this->offset = offset;
    this->binding = 'L';
    this->serial = ID++;

    Symbol::mapping[this->name] = this;
}

Symbol* Symbol::getSymbol(std::string name){
    return Symbol::mapping[name];
}


