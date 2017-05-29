#include "relocation.h"
#include "util.h"
#include <assert.h>

Relocation::Relocation(int offset, std::string type, TS_entry* target, int adr_field_value){

    assert(Section::current != nullptr);

    this->offset = offset;
    this->type = type;
    this->target = target;
    this->adr_field_value = adr_field_value;

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

    std::string target;
    if (this->target == nullptr)
        target = "0";
    else 
        target = this->target->getName();

    return left_padding(std::to_string(this->offset), 4) + 
           " " +
           right_padding(this->type,3) + 
           " " +
           right_padding(target, 15) +
           " " +
           left_padding(std::to_string(this->adr_field_value), 4);
}

void Relocation::print_all(){

    std::cout << std::endl <<"RELOCATIONS" << std::endl;

    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

        if(((Section*)s)->reloc_head != nullptr)
            std::cout << s->getName() << std::endl;
        else
            continue;

        for (Relocation* i = ((Section*)s)->reloc_head; i != nullptr; i=i->next){
            std::cout << i->to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}