#include "TS_entry.h"
#include "util.h"
#include "mchunk.h"
#include "parser.h"
#include <bitset>
#include <iomanip>

#define MAX_SEG_NAME_LEN (15)
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

void TS_entry::setValue(int value){
    this->value = value;
}

int TS_entry::getValue(){
    return this->value;
}

void TS_entry::setType(unsigned short type){
    this->type = type;
}

unsigned short TS_entry::getType(){
    return this->type;
}

bool TS_entry::is_label_or_extern(){
    return (this->getType() >= SECTION_DATA && this->getType() <= SYMBOL_LABEL && 
           !this->is_in_static_section()) || 
           (this->getType() == SYMBOL_EXTERN);
}

bool TS_entry::is_constant(){
    return this->getType() == SYMBOL_CONSTANT || 
           this->is_in_static_section();
}

void TS_entry::init(){

    // sets IDs to all symbol table entries
    unsigned int ID = 0;

    // assign IDs to sections 
    for (TS_entry* i = Section::head; i != nullptr;i = i->next){
        i->setID(++ID);
        Section::num_of_sections++;

        int size = ((Section*)i)->getSize();

        if (size > 0 ){
            ((Section*)i)->machine_code = new char[size*sizeof(char)];
            for (int k = 0 ; k < size ; k++)
                (((Section*)i)->machine_code)[k] = 0;
        }
    }

    // assign IDs to symbols 
    for (TS_entry* i = Symbol::head; i != nullptr;i = i->next){
        i->setID(++ID);
    }
}

void TS_entry::clear(){

    TS_entry* septr = Section::head;
    while (septr != nullptr){
        TS_entry* tmp = septr;
        septr = septr->next;
        delete tmp;
    }

    TS_entry* syptr = Symbol::head;
    while (syptr != nullptr){
        TS_entry* tmp = syptr;
        syptr = syptr->next;
        delete tmp;
    }

    TS_entry::TS_entry_mapping.clear();
}

bool TS_entry::is_key_word(const std::string &str){
    if (TS_entry::system_defined_words.find(str) == TS_entry::system_defined_words.end()) 
        return false;
    return true;
}

bool TS_entry::is_in_static_section(){
    if (this->section != nullptr) 
        return this->section->is_static;
    return false;
}

Section::Section(std::string name):TS_entry(name){
    this->value = 0;
    this->next = nullptr;
    this->prev = nullptr;

    this->line_head = nullptr;
    this->line_tail = nullptr;
    this->reloc_head = nullptr;
    this->reloc_tail = nullptr;

    this->machine_code = nullptr;
    this->section = this;
    this->is_static = false;
    if (Section::head == nullptr){
        Section::head = Section::tail = this;
    }else{
        Section::tail->next = this;
        this->prev = Section::tail;
        Section::tail = this;
    }
}

Section::~Section(){
    Line* line = this->line_head;
    while ( line != nullptr ){
        Line* tmp = line;
        line = line->next;
        delete tmp;
    }

    Relocation* reloc = this->reloc_head;
    while ( reloc != nullptr ){
        Relocation* rtmp = reloc;
        reloc = reloc->next;
        delete rtmp;
    }
} 

void Section::setSize(size_t size){
    this->size = size;
}

size_t Section::getSize(){
    return this->size;
}

bool Section::isStatic(){
    return this->is_static;
}

void Section::add_section(std::string &str, std::string type){
    Section* section = new Section(str);

    if (type == "data") 
        section->type = SECTION_DATA;
    
    else if (type == "text") 
        section->type = SECTION_TEXT;
    
    else if (type == "rodata") 
        section->type = SECTION_RODATA;
    
    else if (type == "bss") 
        section->type = SECTION_BSS;
    
    if (Section::current != nullptr){ 

        Section::current->size = Parser::location_counter;

        if (Section::current->is_static == true && Section::current->size == 0){
                std::cout << "Section: " << Section::current->getName() 
                          << " is static but has size 0." << std::endl;
                exit(1);
            }
                
        if (Section::current->is_static == true){
            new Mchunk(Section::current->getValue(), 
                       Section::current->getSize());
        }
    }

    Section::current = section;
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

Section* TS_entry::getSection(){
    return this->section;
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

    if (this->isStatic()) flags.append("S");
    else flags.append("-");

    std::string ret;


    ret = right_padding("SEG", 4) +
          left_padding(std::to_string(this->ID), 4) + 
          " " +
          right_padding(this->name, MAX_SEG_NAME_LEN) +
          left_padding(std::to_string(this->ID), 4) +
          left_padding(std::to_string(this->value), 7) +
          left_padding(std::to_string(this->size), 7) +
          " " +
          flags;

    return ret;
}

std::string Symbol::to_string(){
    std::string my_section;
    if (this->type == SYMBOL_EXTERN) 
        my_section = "0";
    else if (this->type == SYMBOL_CONSTANT || this->section->is_static == true) 
        my_section = "-1";
    else 
        my_section = this->section->getName();

    std::string scope;
    if (this->scope_flag == SCOPE_GLOBAL) scope = "G";
    else scope = "L";

    std::string ret;


    ret =  right_padding("SYM", 4) +
           left_padding(std::to_string(this->ID), 4) + 
           " " +
           right_padding(this->name, MAX_SYM_NAME_LEN) +
           left_padding(my_section, MAX_SEG_NAME_LEN) +
           left_padding(std::to_string(this->value), 7) +
           " " +
           scope;

    return ret;
}

void TS_entry::print_TS(){

    std::cout << "SYMBOL TABLE" << std::endl;

    for (TS_entry* i = Section::head; i != nullptr; i = i->next){
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;
    for (TS_entry* i = Symbol::head; i != nullptr; i = i->next){
        std::cout << i->to_string() << std::endl;
    }
}

void TS_entry::print_machine_code(){

    int is_little_endian = 0;

    int i =1;
    if (*(char*)&i == 1)
        is_little_endian = 0; // is little endian: =0 for little endian print, =1 for big endian
    else
        is_little_endian = 1; // is big endian: =1 for little endian print, =0 for big endian

    std::cout << "MACHINE CODE" << std::endl;
    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

        std::cout << s->getName() << std::endl;

        Line* line = ((Section*)s)->line_head;
        size_t dsize = 0;
        size_t drept = 0;
        size_t dbase = 0;
      
        while(line != nullptr){
            if (line->mnemonic == "DD"){
                dsize = 4;
                drept = str_to_int(line->ops[0]);
            }
            else if (line->mnemonic == "DW"){
                dsize = 2;
                drept = str_to_int(line->ops[0]);
            }
            else if (line->mnemonic == "DB"){
                dsize =1;
                drept = str_to_int(line->ops[0]);
            }
            else{
                dsize = 4;
                drept = line->getSize() / 4;
            }

            //std::cout << "data size: " << dsize << " rept: " <<drept <<std::endl;

            for (size_t i = 0; i < drept; i++){
                for (size_t j = 0; j < dsize; j++){
                    std::bitset<8> bs(((Section*)s)->machine_code
                        [dbase + (i+is_little_endian*1)*dsize-(1+j)*is_little_endian + j*(1-is_little_endian)]);
                    std::cout << bs.to_string()<< " ";
                }
                std::cout << std::endl;
            }

            dbase += dsize*drept;

            line = line->next;
        }
        std::cout << std::endl;
    }
}