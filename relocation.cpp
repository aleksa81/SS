#include "relocation.h"

Relocation::Relocation(size_t offset, std::string type, 
                       Symbol* symbol, Section* my_section){
    this->offset = offset;
    this->type = type;
    this->my_symbol = symbol;
    this->my_section = my_section;

    // add relocation to my_section's relocations list
}