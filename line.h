#ifndef CODELINE_
#define CODELINE_

#include <map>
#include <string>

class CodeLine{
private:
    std::string label; //
    std::string mnemonic; //
    std::string op1; //
    std::string op2; //
    std::string op3; //
    size_t size;
    int section; //
    bool isLabeled; // 

    CodeLine* next;
    CodeLine* prev;

    static CodeLine* head;
    static CodeLine* tail;
public:
    CodeLine(std::string mnemonic, int section){
        this->mnemonic = mnemonic;
        this->section = section;

        // TODO: set size

        this->next = nullptr;
        this->prev = nullptr;

        if (head == nullptr) head = tail = this;
        else{
            tail->next = this;
            this->prev = tail;
            tail = this;
        }
    }

    void setLabel(std::string label){
        this-> label = label;
        this->isLabeled = true;
    }

    void setOps(std::string op1, std::string op2 = "", std::string op3 = ""){
        this->op1 = op1;
        this->op2 = op2;
        this->op3 = op3;
    }

    size_t getSize(){
        return this->size;
    }
};

#endif