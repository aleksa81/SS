#include "relocation.h"
#include <assert.h>

Relocation::Relocation(int offset, std::string type, TS_entry* target){

    assert(Section::current != nullptr);

    this->offset = offset;
    this->type = type;
    this->target = target;

    this->prev = nullptr;
    this->next = nullptr;

    if (Section::current->reloc_head == nullptr){
        Section::current->reloc_head = Section::current->reloc_tail = this;
    }else{
        this->prev = Section::current->reloc_tail;
        Section::current->reloc_tail->next = this;
        Section::current->reloc_tail = this;
    }
}

std::string Relocation::to_string(){
    return std::to_string(this->offset) + 
           " " +
           this->type + 
           " " +
           this->target->getName();
}

void Relocation::print_all(){
    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

        std::cout << s->getName() << std::endl;

        for (Relocation* i = ((Section*)s)->reloc_head; i != nullptr; i=i->next){
            std::cout << i->to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}