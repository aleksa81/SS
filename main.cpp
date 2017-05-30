#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"
#include "loader.h"
#include "relocation.h"

int main(){

    Parser::getInstance().parse("machine_code_test");

    //Line::print_all();
    TS_entry::print_TS();

    //std::cout << calc_const_expr_no_reloc(" (x - y) +3 ") << std::endl;

    Loader::getInstance().load();

    Relocation::print_all();

    TS_entry::print_machine_code();

    // std::string reg = "";
    // int value = -1;
    // std::cout << is_reg_ind_disp("[ R1 + 3 + 4- 4 ]", reg, value)<<std::endl;
    // std::cout << reg << " " << value << std::endl;


    // TS_entry* tse = nullptr;
    // std::string s = " 5  ";
    // int a = calc_const_expr("4 + 5 + 2 +1", tse);

    // std::cout << a << std::endl;

    // char * a = new char[10];
    // for (int i=0;i<10;i++)
    //     *(a+i) = 0;

    // *((int*)(a+0)) = 1<<24;
    // // *((int*)a) |= (1<<24);

    // for (int i=0;i<10;i++)
    //     std::cout << (int)(*(a+i)) << " ";

    // std::cout << std::endl;

    return 0;
}