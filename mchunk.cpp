#include "mchunk.h"
#include "util.h"
#include "parser.h"
#include <iostream>

Mchunk* Mchunk::head = nullptr;
Mchunk* Mchunk::tail = nullptr;

class Parser;

Mchunk::Mchunk(int begin, int size){
    this->begin = begin;
    this->end = begin+size;
    this->next = nullptr;
    this->prev = nullptr;

    //std::cout << "begin: " << begin << " end: " << this->end << std::endl;

    for(Mchunk* i = Mchunk::head; i != nullptr; i = i->next){
        if ((this->end <= i->begin || this->begin >= i->end) && this->begin != i->begin)
            continue;
        else Parser::error("Section intersection.");
    }

    if (Mchunk::head == nullptr){
        Mchunk::head = Mchunk::tail = this;
    }else{
        Mchunk::tail->next = this;
        this->prev = Mchunk::tail;
        Mchunk::tail = this;
    }
}

void Mchunk::clear(){
    Mchunk* ptr = Mchunk::head;
    while (ptr != nullptr){
        Mchunk* tmp = ptr;
        ptr = ptr->next;
        delete tmp;
    }
}