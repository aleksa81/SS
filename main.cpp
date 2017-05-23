#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"

int main(){

    //Parser::getInstance().parse("tekst");

    //Line::print();
    //TS_entry::print();

    std::string str = "asb";
    std::string ins = "eee";
    ins.insert(0, " ");
    str.insert(str.length(), ins);

    std::cout << calc_const_expr("11+ ( 0x14bFd-8*(4+0b001)+'a')-5/(10*7)") << std::endl;


    return 0;
}