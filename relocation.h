#ifndef RELOCATION_
#define RELOCATION_

#include <string>
#include "TS_entry.h"
#include "parser.h"

class TS_entry;

class Relocation{
private:
    size_t offset;
    std::string type;
    TS_entry* target;
    int adr_field_value;

    Relocation* next;
    Relocation* prev;
public:

    Relocation(int offset, std::string type, TS_entry* target, int adr_field_value );

    std::string to_string();

    static void print_all();

    friend class ObjFile;

};

#endif