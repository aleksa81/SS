#ifndef LINE_
#define LINE_

#include <map>
#include <string>
#include <iostream>
#include "TS_entry.h"
#include "parser.h"

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
    Line(std::string mnemonic, 
         std::string op1,
         std::string op2,
         std::string op3, 
         size_t size,
         bool is_section);

    size_t getSize();

    static void print();
};

#endif