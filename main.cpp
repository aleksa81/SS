#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"

int main(){

    Parser::getInstance().parse("tekst3");

    Line::print();
    TS_entry::print();

    return 0;
}