#include "line.h"
#include "TS_entry.h"

Line::Line(std::string mnemonic, 
         std::string op1,
         std::string op2,
         std::string op3, 
         size_t size,
         bool is_section){

    //this->label = Parser::label_name;
    this->label = "";
    this->mnemonic = mnemonic;
    this->section = Section::current;
    ops[0] = op1;
    ops[1] = op2;
    ops[2] = op3; 
    this->size = size;

    this->is_section = is_section;
    this->line_number = Parser::line_counter;

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

std::string Line::to_string(){
    std::string ops = "";

    for (int j=0;j<3;j++)
        if (!this->ops[j].empty())
            ops.append(this->ops[j]+ std::string(" ")); 
    
    return "#" + 
           std::to_string(this->line_number) + 
           " " +
           this->mnemonic + 
           " " + 
           ops +
           "[ size = " + 
           std::to_string(this->getSize()) + 
           "]"; 
}

void Line::print_all(){
    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

        std::cout << s->getName() << std::endl;

        for (Line* i = ((Section*)s)->line_head; i != nullptr; i=i->next){
            std::cout << i->to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}

size_t Line::getSize(){
    return this->size;
}