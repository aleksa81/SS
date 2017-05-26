#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"

int main(){

    Parser::getInstance().parse("tekst3");

    Line::print();
    TS_entry::print();

    std::cout << calc_const_expr_no_reloc(" (x - y) +3 ") << std::endl;

    return 0;
}