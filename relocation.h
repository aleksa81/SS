#ifndef RELOCATION_
#define RELOCATION_

#include <string>
#include "TS_entry.h"

class Section;

class Relocation{
private:
    size_t offset;
    std::string type;
    Symbol* my_symbol;
    Section* my_section;

    Relocation* next;
    Relocation* prev;
public:

    Relocation(size_t offset, std::string type, 
               Symbol* symbol, Section* my_section);

};

#endif