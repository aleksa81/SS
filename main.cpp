#include <iostream>
#include <string>
#include <fstream>
#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"

int main(){

    Parser::getInstance().parse("tekst");

    Line::print();
    TS_entry::print();

    return 0;
}