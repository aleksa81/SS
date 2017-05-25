#include "mchunk.h"
#include "util.h"
#include <iostream>

Mchunk* Mchunk::head = nullptr;
Mchunk* Mchunk::tail = nullptr;

Mchunk::Mchunk(int begin, int size){
    this->begin = begin;
    this->end = begin+size;
    this->next = nullptr;
    this->prev = nullptr;

    std::cout << "Begin: " << begin << "Size: " << size << std::endl;

    for(Mchunk* i = Mchunk::head; i != nullptr; i = i->next){
        if ((this->end <= i->begin || this->begin >= i->end) && this->begin != i->begin)
            continue;
        else error("Section intersection.", 4);
    }

    if (Mchunk::head == nullptr){
        Mchunk::head = Mchunk::tail = this;
    }else{
        Mchunk::tail->next = this;
        this->prev = Mchunk::tail;
        Mchunk::tail = this;
    }
}