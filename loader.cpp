#include "loader.h"
#include "TS_entry.h"
#include "util.h"
#include <iostream>
#include <assert.h>
#include <set>
#include <string>

#define IMMED (4)
#define REGDIR (0)
#define MEMDIR (6)
#define REGIND (2)
#define REGINDDISP (7)

#define DOUBLE_WORD (0)
#define UWORD (1)
#define WORD (5)
#define UBYTE (3)
#define BYTE (7)

extern std::set<std::string> check_first_operand_mnemonics;
extern std::set<std::string> check_no_operand_mnemonics;
extern std::set<std::string> check_second_operand_mnemonics;
extern std::set<std::string> regs;

std::unordered_map<std::string, char> Loader::opcode{
    {"INT",    0},
    {"JMP",    2},
    {"CALL",   3},
    {"RET",    1},
    {"JZ",     4},
    {"JNZ",    5},
    {"JGZ",    6},
    {"JGEZ",   7},
    {"JLZ",    8},
    {"JLEZ",   9},

    {"LOAD",  16},
    {"STORE", 17},

    {"PUSH",  32},
    {"POP",   33},

    {"ADD",   48},
    {"SUB",   49},
    {"MUL",   50},
    {"DIV",   51},
    {"MOD",   52},
    {"AND",   53},
    {"OR",    54},
    {"XOR",   55},
    {"NOT",   56},
    {"ASL",   57},
    {"ASR",   58}   
};

std::unordered_map<std::string, char> Loader::regcode{
    {"R0",   0},
    {"R1",   1},
    {"R2",   2},
    {"R3",   3},
    {"R4",   4},
    {"R5",   5},
    {"R6",   6},
    {"R7",   7},
    {"R8",   8},
    {"R9",   9},
    {"R10", 10},
    {"R11", 11},
    {"R12", 12},
    {"R13", 13},
    {"R14", 14},
    {"R15", 15},

    {"SP",  16},
    {"PC",  17}
};

void Loader::load(){
    Section::current = (Section*)Section::head;

    std::cout << std::endl;

    for (Section::current = (Section*)Section::head; 
         Section::current != nullptr; 
         Section::current = (Section*)Section::current->next){

        // set LC
        Parser::location_counter = LC_START;

        for (Line* line = Section::current->line_head; line != nullptr; line = line->next){

            // Parser::line_counter changes in every iteration
            Parser::line_counter = line->line_number;

            // Absolute or PC relative
            std::string reloc_type;

            // Operand with potential relocation
            std::string operand = "";

            if (check_first_operand_mnemonics.find(line->mnemonic) != check_first_operand_mnemonics.end()){

                operand = line->ops[0];
                //std::cout << operand << std::endl;

            }else if (check_second_operand_mnemonics.find(line->mnemonic) != check_second_operand_mnemonics.end()){

                operand = line->ops[1];
                //std::cout << operand << std::endl;

            }else if (check_no_operand_mnemonics.find(line->mnemonic) != check_no_operand_mnemonics.end()){
                // operand stays empty

            }else if (line->mnemonic == "DD" || line->mnemonic == "DW" || line->mnemonic == "DB"){

                operand = line->ops[2];
                //std::cout << operand << std::endl;

            }

            Parser::location_counter += line->getSize();

            //std::cout << operand;

            if (operand.empty()){
                //std::cout << std::endl;
                continue;
            }

            // Initialize relocation type string 
            if (operand[0] == '$')
                reloc_type = "R";
            else 
                reloc_type = "A";

            // Cut those symbols before calculation
            if (operand[0] == '$' || operand[0] == '#')
                operand.erase(operand.begin());

            // Calculate operand value
            int value;
            TS_entry* reloc_symb = nullptr;

            if (operand != "?" && regs.find(operand) == regs.end())
                value = calc_const_expr(operand, reloc_symb);

            // Is relocation needed? 
            // No, when reloc_symb is from current section and type is R or when reloc_symb == nullptr.
            // (reloc_symb will be equal to nullptr when operand does not leave relocation symbol.)
            // Yes, in all other cases.

            if (reloc_symb == nullptr || 
                (reloc_symb->getSection() != nullptr && 
                 reloc_symb->getSection()->getID() == Section::current->getID() && 
                 reloc_type == "R"
                )
               ){
               //std::cout << std::endl;
               continue;
            }

            // Which symbol from symbol table will relocation entry target?

            Relocation* reloc;

            int offset = Section::current->getValue() + Parser::location_counter - line->getSize();

            if (line->mnemonic != "DD" && line->mnemonic != "DW" && line->mnemonic != "DB")
                offset += 4;

            if (reloc_symb->getScope() == SCOPE_GLOBAL)
                reloc = new Relocation(offset, 
                                       reloc_type, 
                                       reloc_symb);
            else{
                // This assert must be true (all externs are marked with global scope)
                assert(reloc_symb->getSection() != nullptr);
                reloc = new Relocation(offset,
                                       reloc_type, 
                                       reloc_symb->getSection()); 
            }

            //std::cout << " RELOCATION: " << reloc->to_string() << std::endl;

        }
    }

    std::cout << std::endl;
}