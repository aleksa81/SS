#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"

int main(){

    Parser::getInstance().parse("tekst");

    Line::print();
    TS_entry::print();

    std::cout << calc_const_expr(" y-x + e ") << std::endl;


    return 0;
}