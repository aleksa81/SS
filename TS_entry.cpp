#include "TS_entry.h"
#include "util.h"

#define MAX_SEG_NAME_LEN (12)
#define MAX_SYM_NAME_LEN (15)

TS_entry* Section::head = nullptr;
TS_entry* Section::tail = nullptr;

Section* Section::current = nullptr;
int Section::num_of_sections = 0;

TS_entry* Symbol::head = nullptr;
TS_entry* Symbol::tail = nullptr;

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

void TS_entry::init(){

    // sets IDs to all symbol table entries
    unsigned int ID = 0;

    // assign IDs to sections 
    for (TS_entry* i = Section::head; i != nullptr;i = i->next){
        i->setID(++ID);
        Section::num_of_sections++;
    }

    // assign IDs to symbols 
    for (TS_entry* i = Symbol::head; i != nullptr;i = i->next){
        i->setID(++ID);
    }
}

bool TS_entry::is_key_word(const std::string &str){
    if (TS_entry::system_defined_words.find(str) == TS_entry::system_defined_words.end()) 
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

    if (type == "data") 
        section->type = SECTION_DATA;
    
    else if (type == "text") 
        section->type = SECTION_TEXT;
    
    else if (type == "rodata") 
        section->type = SECTION_RODATA;
    
    else if (type == "bss") 
        section->type = SECTION_BSS;
    
    if (Section::current != nullptr) 
        current->size = location_cntr;

    Section::current = section;
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

void Symbol::add_symbol_as_global(std::string name, int value){
    if (TS_entry::TS_entry_mapping.find(name) == TS_entry::TS_entry_mapping.end()){

        // if global is hit before definition
        Symbol* symbol = new Symbol(name, value, nullptr);
        symbol->setType(SYMBOL_EXTERN);
    }
    TS_entry::TS_entry_mapping[name]->setScope(SCOPE_GLOBAL);
}

bool Symbol::add_symbol_as_defined(std::string name, int value, unsigned short type){
    Symbol* symbol;
    if (TS_entry::TS_entry_mapping.find(name) == TS_entry::TS_entry_mapping.end()){
        symbol = new Symbol(name, value, Section::current);
    }else{

        if (TS_entry::TS_entry_mapping[name]->getType() != SYMBOL_EXTERN){
            // returns false for redefinition
            return false;
        }
        // if symbol was declared global before definition
        symbol = (Symbol*)TS_entry::TS_entry_mapping[name];
        symbol->value = value; 
        symbol->section = Section::current;
    }
    symbol->type = type;

    return true;
}

std::string Section::to_string(){
    std::string flags;
    if (this->type == SECTION_DATA) flags = "WPA";
    else if (this->type == SECTION_RODATA) flags = "RPA";
    else if (this->type == SECTION_TEXT) flags = "XPA";
    else flags = "W-A";

    return right_padding("SEG", 4) +
           left_padding(std::to_string(this->ID), 4) + 
           " " +
           right_padding(this->name, MAX_SEG_NAME_LEN) +
           left_padding(std::to_string(this->ID), 4) +
           left_padding(std::to_string(this->start), 7) +
           left_padding(std::to_string(this->size), 7) +
           " " +
           flags;
}

std::string Symbol::to_string(){
    std::string my_section;
    if (this->type == SYMBOL_EXTERN) my_section = "0";
    else if (this->type == SYMBOL_CONSTANT) my_section = "-1";
    else my_section = std::to_string(this->section->ID);

    std::string scope;
    if (this->scope_flag == SCOPE_GLOBAL) scope = "G";
    else scope = "L";

    return right_padding("SYM", 4) +
           left_padding(std::to_string(this->ID), 4) + 
           " " +
           right_padding(this->name, MAX_SYM_NAME_LEN) +
           left_padding(my_section, 4) +
           left_padding(std::to_string(this->value), 7) +
           " " +
           scope;
}

void TS_entry::print(){
    for (TS_entry* i = Section::head; i != nullptr; i = i->next){
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Number of sections: " << Section::num_of_sections << "."<<std::endl;
    std::cout << std::endl;
    for (TS_entry* i = Symbol::head; i != nullptr; i = i->next){
        std::cout << i->to_string() << std::endl;
    }
}