#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"
#include "loader.h"

int main(){

    Parser::getInstance().parse("tekst3");

    Line::print_all();
    TS_entry::print();

    //std::cout << calc_const_expr_no_reloc(" (x - y) +3 ") << std::endl;

    Loader::getInstance().load();

    return 0;
}