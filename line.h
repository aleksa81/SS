#ifndef CODELINE_
#define CODELINE_

#include <map>
#include <string>
#include <iostream>
#include "TS_entry.h"

class Line{
private:
    std::string label; //
    std::string mnemonic; //
    std::string ops[3];
    Section* section; //
    size_t size;
    bool is_section;

    Line* next;
    Line* prev;

    static Line* head;
    static Line* tail;
public:
    Line(std::string label, 
         std::string mnemonic, 
         std::string op1,
         std::string op2,
         std::string op3, 
         size_t size,
         bool is_section){

        this->label = label;
        this->mnemonic = mnemonic;
        this->section = Section::current;
        ops[0] = op1;
        ops[1] = op2;
        ops[2] = op3; 
        this->size = size;

        this->is_section = is_section;

        this->next = nullptr;
        this->prev = nullptr;

        if (head == nullptr) {
            head = tail = this;
        }
        else{
            tail->next = this;
            this->prev = tail;
            tail = this;
        }
    }

    size_t getSize(){
        return this->size;
    }

    static void print(){
        for (Line* i = head; i != nullptr; i=i->next){
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
};

#endif