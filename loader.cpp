#include "loader.h"
#include "TS_entry.h"
#include "util.h"
#include <iostream>
#include <assert.h>
#include <set>
#include <string>

#define REG0 (16)
#define REG1 (11)
#define REG2 (6)

#define IMMED (4<<21)
#define REGDIR (0<<21)
#define MEMDIR (6<<21)
#define REGIND (2<<21)
#define REGINDDISP (7<<21)

#define DOUBLE_WORD (0<<3)
#define UWORD_ (1<<3)
#define WORD_ (5<<3)
#define UBYTE_ (3<<3)
#define BYTE_ (7<<3)

extern std::set<std::string> check_first_operand_mnemonics;
extern std::set<std::string> check_no_operand_mnemonics;
extern std::set<std::string> check_second_operand_mnemonics;
extern std::set<std::string> regs;

std::string Loader::reloc_type = "";
int Loader::value = 0;
TS_entry* Loader::reloc_symb = nullptr;
std::string Loader::operand = "";
std::string Loader::reg = "";
int Loader::disp = 0;
int Loader::adr_mode = 0;

std::unordered_map<std::string, int> Loader::opcode{
    {"INT",    0 << 24},
    {"JMP",    2 << 24},
    {"CALL",   3 << 24},
    {"RET",    1 << 24},
    {"JZ",     4 << 24},
    {"JNZ",    5 << 24},
    {"JGZ",    6 << 24},
    {"JGEZ",   7 << 24},
    {"JLZ",    8 << 24},
    {"JLEZ",   9 << 24},

    {"LOAD",    16 << 24},
    {"UBLOAD",  16 << 24},
    {"SBLOAD",  16 << 24},
    {"UWLOAD",  16 << 24},
    {"SWLOAD",  16 << 24},
    {"STORE",   17 << 24},
    {"BSTORE",  17 << 24},
    {"WSTORE",  17 << 24},

    {"PUSH",  32 << 24},
    {"POP",   33 << 24},

    {"ADD",   48 << 24},
    {"SUB",   49 << 24},
    {"MUL",   50 << 24},
    {"DIV",   51 << 24},
    {"MOD",   52 << 24},
    {"AND",   53 << 24},
    {"OR",    54 << 24},
    {"XOR",   55 << 24},
    {"NOT",   56 << 24},
    {"ASL",   57 << 24},
    {"ASR",   58 << 24}   
};

std::unordered_map<std::string, int> Loader::regcode{
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

    for (Section::current = (Section*)Section::head; 
         Section::current != nullptr; 
         Section::current = (Section*)Section::current->next){

        // set LC
        Parser::location_counter = LC_START;

        for (Line* line = Section::current->line_head; line != nullptr; line = line->next){

            // Parser::line_counter changes in every iteration
            Parser::line_counter = line->line_number;

            std::string mne = line->mnemonic;

            // Absolute or PC relative
            Loader::reloc_type = "A";
            Loader::reloc_symb = nullptr;
            // Operand with potential relocation
            Loader::operand = "";
            Loader::reg = "";
            Loader::disp = 0;
            Loader::adr_mode = -1;
            Loader::value = 0;

            if (check_first_operand_mnemonics.find(mne) != check_first_operand_mnemonics.end()){

                operand = line->ops[0];
                *((int*)(Section::current->machine_code + Parser::location_counter)) |=  opcode[mne];

                if (is_reg_dir(operand)){
                    Loader::process_reg_dir(REG0);
                    Parser::location_counter += line->getSize();
                    continue;
                }
                else if (is_reg_ind_disp(operand, reg, disp) && !Loader::is_INT(mne)){
                    process_reg_ind_disp(REG0);
                    Parser::location_counter += line->getSize();
                    continue;
                }
                else if (is_pc_rel(operand) && !Loader::is_INT(mne))
                    Loader::process_pc_rel(REG0);
                else if (is_mem_dir(operand) && !Loader::is_INT(mne))
                    Loader::process_mem_dir();
                else
                    Parser::error("Address mode mismatch.");
                
            }
            else if (check_second_operand_mnemonics.find(mne) != check_second_operand_mnemonics.end()){

                operand = line->ops[1];
                if (!is_reg_dir(line->ops[0]))
                    Parser::error("Address type mismatch.");

                *((int*)(Section::current->machine_code + Parser::location_counter)) |=  opcode[mne];
                *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[line->ops[0]]<<REG0);

                if (is_reg_dir(operand)){
                    Loader::process_reg_dir(REG1);
                    Parser::location_counter += line->getSize();
                    continue;
                }
                else if (is_reg_ind_disp(operand, reg, disp)){
                    Loader::process_reg_ind_disp(REG1);
                    Parser::location_counter += line->getSize();
                    continue;
                }
                else if (is_pc_rel(operand))
                    Loader::process_pc_rel(REG1);
                else if (is_immed(operand) && !is_store(mne) && !is_JZ(mne))
                    Loader::process_immed();
                else if (is_mem_dir(operand) && !is_immed(operand))
                    Loader::process_mem_dir();
                else
                    Parser::error("Address mode mismatch.");

                if (is_store(mne))
                    Loader::process_store(mne);
                if (is_load(mne))
                    Loader::process_load(mne);
            }
            else if (check_no_operand_mnemonics.find(mne) != check_no_operand_mnemonics.end()){

                *((int*)(Section::current->machine_code + Parser::location_counter)) |=  opcode[mne];

                if (mne == "RET"){
                    Parser::location_counter += line->getSize(); 
                    continue;
                }
                *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[line->ops[0]]<<REG0);
                if (mne == "PUSH" || mne == "POP"){
                    Parser::location_counter += line->getSize(); 
                    continue;
                }
                *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[line->ops[1]]<<REG1);
                *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[line->ops[2]]<<REG2);

                Parser::location_counter += line->getSize();
                continue;

            }
            else if (mne == "DD" || mne == "DW" || mne == "DB"){

                operand = line->ops[2];

                //std::cout << "OPERAND:" << operand << "* " << str_to_int(line->ops[0]) << " "<< line->getSize() <<std::endl;

                if (operand == "?"){
                    Parser::location_counter += line->getSize();
                    continue;
                }

                value = calc_const_expr(operand, reloc_symb);

                int dup = str_to_int(line->ops[0]);

                if (reloc_symb != nullptr){

                    if (mne == "DB" || mne == "DW")
                        Parser::error("Address needs double word to be stored.");

                    for (int i=0;i<dup;i++){
                        if (reloc_symb->getScope() == SCOPE_GLOBAL){
                            *((int*)(Section::current->machine_code + Parser::location_counter + i*4)) = value - reloc_symb->getValue(); // CHECK THIS
                            new Relocation(Section::current->getValue() + Parser::location_counter + i*4, reloc_type, reloc_symb, value - reloc_symb->getValue());
                        }
                        else{
                            assert(reloc_symb->getSection() != nullptr);
                            *((int*)(Section::current->machine_code + Parser::location_counter + i*4)) = value;
                            new Relocation(Section::current->getValue() + Parser::location_counter+i*4, reloc_type, reloc_symb->getSection(), value);
                        }
                    } 
                }
                else{
                    if (mne == "DB"){
                        for (int i = 0; i< dup;i++)
                            *(Section::current->machine_code + Parser::location_counter + i) = value;
                        
                    }else if (mne == "DW")
                        for (int i = 0; i< dup;i++){
                            *((short*)(Section::current->machine_code + Parser::location_counter + i*2)) = value;
                    }
                }

                Parser::location_counter += line->getSize();
                continue;
            }

            /* -------------------------------------------------------------------------------------- */
            /* ------------------------------------- RELOCATION ------------------------------------- */
            /* -------------------------------------------------------------------------------------- */

            if (reloc_symb == nullptr && adr_mode == REGINDDISP){

                if (Section::current->is_static == true){
                    *((int*)(Section::current->machine_code + Parser::location_counter + 4)) = value - Parser::location_counter - 8;
                }else{
                    *((int*)(Section::current->machine_code + Parser::location_counter + 4)) = value - 4;
                    new Relocation(Section::current->getValue() + Parser::location_counter+4, reloc_type, nullptr, value - 4);
                }

            }
            else if (adr_mode == REGINDDISP){

                if (reloc_symb->getSection()->getID() == Section::current->getID()){
                    *((int*)(Section::current->machine_code + Parser::location_counter) + 4) = value - Parser::location_counter - 8;
                }
                else if (reloc_symb->getScope() == SCOPE_GLOBAL){
                    *((int*)(Section::current->machine_code + Parser::location_counter) + 4) = -4;
                    new Relocation(Section::current->getValue() + Parser::location_counter+4, reloc_type, reloc_symb, -4);
                }
                else{
                    *((int*)(Section::current->machine_code + Parser::location_counter) + 4) = value - 4;
                    new Relocation(Section::current->getValue() + Parser::location_counter+4, reloc_type, reloc_symb->getSection(), value - 4);
                }

            }
            else if (reloc_symb != nullptr && (adr_mode == IMMED || adr_mode == MEMDIR)){

                if (reloc_symb->getScope() == SCOPE_GLOBAL){
                    *((int*)(Section::current->machine_code + Parser::location_counter + 4)) = value - reloc_symb->getValue(); // CHECK THIS
                    new Relocation(Section::current->getValue() + Parser::location_counter+4, reloc_type, reloc_symb, value - reloc_symb->getValue());
                }
                else{
                    *((int*)(Section::current->machine_code + Parser::location_counter + 4)) = value;
                    new Relocation(Section::current->getValue() + Parser::location_counter+4, reloc_type, reloc_symb->getSection(), value);
                }
            }

            Parser::location_counter += line->getSize(); // int* cast...

        }
    }
}

void Loader::process_reg_ind_disp(int regpos){
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= REGINDDISP;
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[Loader::reg]<<regpos);
    if (!is_reg_ind(Loader::operand))
        *((int*)(Section::current->machine_code + Parser::location_counter +4)) = Loader::disp;
    return;
}

void Loader::process_reg_dir(int regpos){
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= REGDIR;
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode[Loader::operand]<<regpos);
    return;
}

void Loader::process_pc_rel(int regpos){
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= REGINDDISP;
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= (regcode["PC"]<<regpos);
    Loader::value = calc_const_expr(Loader::operand.substr(1, std::string::npos), Loader::reloc_symb);
    Loader::reloc_type = "R";

    Loader::adr_mode = REGINDDISP;
}

void Loader::process_immed(){
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= IMMED;
    Loader::value = calc_const_expr(operand.substr(1, std::string::npos), Loader::reloc_symb);

    Loader::adr_mode = IMMED;
}

void Loader::process_mem_dir(){
    *((int*)(Section::current->machine_code + Parser::location_counter)) |= MEMDIR;
    Loader::value = calc_const_expr(Loader::operand, Loader::reloc_symb);

    Loader::adr_mode = MEMDIR;
}

bool Loader::is_store(const std::string &str){
    if (opcode[str] == (17 << 24)) return true;
    return false;
}
bool Loader::is_load(const std::string &str){
    if (opcode[str] == (16 << 24)) return true;
    return false;

}
bool Loader::is_INT(const std::string &str){
    if (opcode[str] == (0)) return true;
    return false;
}

bool Loader::is_JZ(const std::string &str){
    if (opcode[str] >=(4<<24) && opcode[str] <=(9<<24)) return true;
    return false;
}

void Loader::process_store(const std::string &str){
    if (str[0] == 'B')
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (BYTE_);
    else if (str[0] == 'W')
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (WORD_);
    else
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (DOUBLE_WORD);
    return;
}

void Loader::process_load(const std::string &str){
    if (str.substr(0,2) == "UB")
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (UBYTE_);
    else if (str.substr(0,2) == "SB")
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (BYTE_);
    else if (str.substr(0,2) == "UW")
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (UWORD_);
    else if (str.substr(0,2) == "SW")
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (WORD_);
    else
        *((int*)(Section::current->machine_code + Parser::location_counter)) |= (DOUBLE_WORD);
    return;
}