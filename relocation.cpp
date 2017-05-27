#include "relocation.h"
#include <assert.h>

Relocation::Relocation(std::string type, TS_entry* target){

    assert(Section::current != nullptr);
    
    this->offset = Section::current->getValue() + Parser::location_counter + 4;
    this->type = type;
    this->target = target;

    if (Section::current->reloc_head == nullptr){
        Section::current->reloc_head = Section::current->reloc_tail = this;
    }else{
        this->prev = Section::current->reloc_tail;
        Section::current->reloc_tail->next = this;
        Section::current->reloc_tail = this;
    }

    // add relocation to my_section's relocations list
}