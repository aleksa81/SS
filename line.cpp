#include "line.h"
#include "TS_entry.h"

Line::Line(std::string mnemonic, 
         std::string op1,
         std::string op2,
         std::string op3, 
         size_t size,
         bool is_section){

    this->label = Parser::label_name;
    this->mnemonic = mnemonic;
    this->section = Section::current;
    ops[0] = op1;
    ops[1] = op2;
    ops[2] = op3; 
    this->size = size;

    this->is_section = is_section;


    this->next = nullptr;
    this->prev = nullptr;

    if (Section::current != nullptr){
        if (Section::current->line_head == nullptr) {
            Section::current->line_head = Section::current->line_tail = this;
        }
        else{
            this->prev = Section::current->line_tail;
            Section::current->line_tail->next = this;
            Section::current->line_tail = this;
        }
    }
}

void Line::print(){
    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

        std::cout << "Section: " << s->getName() << " ID: "<< s->getID() << std::endl;

        for (Line* i = ((Section*)s)->line_head; i != nullptr; i=i->next){
            if (!i->label.empty()) std::cout << i->label << ": ";
            std::cout << i->mnemonic << " ";
            for (int j=0;j<3;j++){
                if (!i->ops[j].empty()) std::cout << i->ops[j] << " ";
            }
            if (i->size != 0) std::cout << "[ size = " << i->size << "]";
            if (i->is_section == true) std::cout << "[size = " << ((Section*)TS_entry::TS_entry_mapping[i->mnemonic])->getSize() << "]";
            std::cout << std::endl;
        }
        std::cout << std::endl;

    }
    std::cout << std::endl;
}

size_t Line::getSize(){
    return this->size;
}