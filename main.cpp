#include "line.h"
#include "TS_entry.h"
#include "util.h"
#include "parser.h"
#include "mchunk.h"
#include "loader.h"
#include "relocation.h"

int main(){

    Parser::getInstance().parse("machine_code_test");

    Line::print_all();
    TS_entry::print_TS();


    Loader::getInstance().load();

    Relocation::print_all();

    TS_entry::print_machine_code();


    // std::string reg;
    // std::string disp;
    // bool a = is_reg_ind_disp("[R1+b]", reg, disp);
    // std::cout << reg << " " << disp << std::endl;

    // int i =1;
    // if (*(char*)&i == 1)
    //     std::cout << "Little endian" << std::endl;
    // else
    //     std::cout << "Big endian" << std::endl;




    //std::cout << str_to_int("0b11111111111111111111111111111111") << std::endl;

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