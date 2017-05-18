#include "TS_entry.h"

Section* Section::head = nullptr;
Section* Section::tail = nullptr;

Section* Section::current = nullptr;

Symbol* Symbol::head = nullptr;
Symbol* Symbol::tail = nullptr;

std::unordered_map<std::string, TS_entry*> TS_entry::TS_entry_mapping;

const std::set<std::string> TS_entry::system_defined_words = 
{ "R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14", "R15",
  "SP","PC", "DUP", "DEF", "ORG"
};

TS_entry::TS_entry(std::string name){
	this->name = name;
	this->scope_flag = SCOPE_LOCAL;

	TS_entry::TS_entry_mapping[name] = this;
}

void TS_entry::setID(unsigned int ID){
	this->ID = ID;
}

unsigned int TS_entry::getID(){
 	return this->ID;
}

void TS_entry::setScope(unsigned short scope){
 	this->scope_flag = scope;
}

unsigned short TS_entry::getScope(){
  	return this->scope_flag;
}

std::string TS_entry::getName(){
  	return this->name;
}

void TS_entry::setType(unsigned short type){
	this->type = type;
}

unsigned short TS_entry::getType(){
  	return this->type;
}

void TS_entry::setIDs(){
	unsigned int ID = 0;

	// assign IDs to sections 
	for (TS_entry* i = Section::head; i != nullptr;i = i->next){
		i->setID(++ID);
	}

	// assign IDs to symbols 
	for (TS_entry* i = Symbol::head; i != nullptr;i = i->next){
		i->setID(++ID);
	}
}

bool TS_entry::is_key_word(const std::string &str){
    if (TS_entry::system_defined_words.find(str) == TS_entry::system_defined_words.end() &&
    	TS_entry::TS_entry_mapping.find(str) == TS_entry::TS_entry_mapping.end())
    return false;
	return true;
}

Section::Section(std::string name):TS_entry(name){
	this->start = 0;
	this->next = nullptr;
	this->prev = nullptr;
	if (Section::head == nullptr){
		Section::head = Section::tail = this;
	}else{
		Section::tail->next = this;
		this->prev = Section::tail;
		Section::tail = this;
	}
} 

void Section::setSize(size_t size){
	this->size = size;
}

size_t Section::getSize(){
	return this->size;
}

void Section::add_section(std::string name, int location_cntr, std::string type){
	Section* section = new Section(name);
	if (type == "data") this->type = SECTION_DATA;
	else if (type == "text") this->type = SECTION_TEXT;
	else if (type == "rodata") this->type = SECTION_RODATA;
	else if (type == "bss") this->type = SECTION_BSS;
	if (Section::current != nullptr) current->size = location_cntr;
	Section::current = this;
}

void Section::setStart(size_t start){
	this->start = start;
}

size_t Section::getStart(){
	return this->start;
}

Symbol::Symbol(std::string name, int value, Section* section)
:TS_entry(name){
	this->section = section;
	this->value = value;

	this->next = nullptr;
	this->prev = nullptr;
	if (Symbol::head == nullptr){
		Symbol::head = Symbol::tail = this;
	}else{
		Symbol::tail->next = this;
		this->prev = Symbol::tail;
		Symbol::tail = this;
	}
}

Section* Symbol::getSection(){
	return this->section;
}

int Symbol::getValue(){
	return this->value;
}

void Symbol::add_symbol_as_global(std::string name, int value, Section* section){
	if (TS_entry::TS_entry_mapping.find(name) == TS_entry::TS_entry_mapping.end()){

		// if global is hit before definition
		Symbol* symbol = new Symbol(name, value, section);
		symbol->setType(SYMBOL_EXTERN);
	}
	TS_entry::TS_entry_mapping[name]->setScope(SCOPE_GLOBAL);
}

void Symbol::add_symbol_as_defined(std::string name, int value, Section* section, unsigned short type){
	Symbol* symbol;
	if (TS_entry::TS_entry_mapping.find(name) == TS_entry::TS_entry_mapping.end()){
		new Symbol(name, value, section);
	}else{

		// if symbol was declared global before definition
		symbol = (Symbol*)TS_entry::TS_entry_mapping[name];
		symbol->value = value; //
		symbol->section = section;
	}
	symbol->type = type;
}