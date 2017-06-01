#ifndef LINE_
#define LINE_

#include <map>
#include <string>
#include <iostream>
#include "TS_entry.h"
#include "parser.h"

class Section;

class Line{
private:
    std::string label; //
    std::string mnemonic; //
    std::string ops[3];
    Section* section; //
    size_t size;
    bool is_section;

    int line_number;

    Line* next;
    Line* prev;

public:
    Line(std::string mnemonic, 
         std::string op1,
         std::string op2,
         std::string op3, 
         size_t size,
         bool is_section);

    size_t getSize();

    std::string to_string();

    static void print_all();

    friend class Loader;
    friend class TS_entry;
    friend class ObjFile;
};

#endif