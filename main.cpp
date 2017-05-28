#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"
#include "loader.h"
#include "relocation.h"

int main(){

    Parser::getInstance().parse("tekst3");

    Line::print_all();
    TS_entry::print();

    //std::cout << calc_const_expr_no_reloc(" (x - y) +3 ") << std::endl;

    Loader::getInstance().load();

    Relocation::print_all();

    return 0;
}