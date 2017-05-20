#include "util.h"
#include <cstdlib> // strtol
#include <set>

// *check first operand to get instruction size (32/64 bit)
// have one operand
const std::set<std::string> check_first_operand_mnemonics = 
{"INT", "JMP", "CALL" };

// *check second operand to get instruction size (32/64 bit)
// have two operands
const std::set<std::string> check_second_operand_mnemonics = 
{"JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ", "LOAD", "STORE"};

// *constant instruction size (32 bit)
// RET - no operands
// PUSH/POP - one operand
// rest - three operands
const std::set<std::string> check_no_operand_mnemonics =
{"RET", "PUSH", "POP", "ADD", "SUB", "MUL", "DIV", "MOD", "AND", "OR", "XOR", "NOT", "ASL", "ASR"};

// all registers that are visible to user
const std::set<std::string> regs = 
{"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14","R15"};

bool is_digits(const std::string &str){
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_hexadecimals(const std::string &str){
    return str.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
}

bool is_binaries(const std::string &str){
    return str.find_first_not_of("01") == std::string::npos;
}

bool is_alphanum(const std::string &str){
     for (size_t i = 0;i<str.length();i++){
        if (!isalnum(str[i]) && str[i] != '_') return false;
     }
     return true;
}

void strip_off_spaces(std::string &str){
        while(!str.empty() && isspace(*str.begin()))
            str.erase(str.begin());

        while(!str.empty() && isspace(*str.rbegin()))
            str.erase(str.length()-1);
}

void strip_off_comment(std::string &str){
    size_t found = str.find(';');
    if (found != std::string::npos){
        str.erase(found, std::string::npos);
    }
}

bool is_mnemonic(const std::string &str){
    if (check_first_operand_mnemonics.find(str) == check_first_operand_mnemonics.end() &&
        check_second_operand_mnemonics.find(str) == check_second_operand_mnemonics.end() &&
        check_no_operand_mnemonics.find(str) == check_no_operand_mnemonics.end()) return false;
    return true;
}

bool is_reg_dir(const std::string &op){
    return regs.find(op) != regs.end(); 
}

bool is_reg_ind(const std::string &op){
    size_t found1 = op.find("[");
    size_t found2 = op.find("]");
    std::string reg = op.substr(found1+1, found2-found1-1);
    strip_off_spaces(reg);
    return regs.find(reg) != regs.end(); 
}

bool is_mem_dir(const std::string &op){ // TODO
    return true;
} 

bool is_mem_ind(const std::string &op){ // TODO
    return true;
}

bool is_pc_rel(const std::string &op){
    return op[0] == '$' && 
           (is_alphanum(op.substr(1, std::string::npos)) || is_absolute(op.substr(1, std::string::npos)));
}

bool is_reg_ind_disp(const std::string &op){ // TODO
    return true;
}

bool is_immed(const std::string &op){ // TODO
    return true;
}

bool is_binary(const std::string &str){
    return str[0] == '0' && (str[1] == 'b' || str[1] == 'B') && 
           is_binaries(str.substr(2, std::string::npos));
}

bool is_hexadecimal(const std::string &str){
    return str[0] == '0' && (str[1] == 'x' || str[1] == 'X') && 
           is_hexadecimals(str.substr(2, std::string::npos));
}

bool is_absolute(const std::string &str){
    return is_digits(str) || is_binary(str) || is_hexadecimal(str);
}

bool is_const_expr(const std::string &str){ // TODO
    return true;
}

int str_to_int(const std::string &str){

    // c_str() converts string to const char* 
    if (is_binary(str)) 
        return strtol(str.substr(2, std::string::npos).c_str(), nullptr, 2);
    if (is_hexadecimal(str)) 
        return strtol(str.substr(2, std::string::npos).c_str(), nullptr, 16);
    if (is_digits(str)) 
        return strtol(str.c_str(), nullptr, 10);
    return 0;
}

size_t get_instruction_size(const std::string &mne,
                            const std::string &op1,
                            const std::string &op2,
                            const std::string &op3){
    // returns instruction size
    // if the number of operands for given mnemonic is not valid, returns 0
    // address type validity will be checked in second assembly pass

    if (check_first_operand_mnemonics.find(mne) != check_first_operand_mnemonics.end()){
        if (!op2.empty() || !op3.empty()) return 0;
        if (is_reg_dir(op1) || is_reg_ind(op1)) return 32;
    }
    else if (check_second_operand_mnemonics.find(mne) != check_second_operand_mnemonics.end()){
        if (!op3.empty()) return 0;
        if (is_reg_dir(op2) || is_reg_ind(op2)) return 32;
    }
    else if(check_no_operand_mnemonics.find(mne) != check_no_operand_mnemonics.end()){
        if (mne == "RET"){

            // RET has no operands
            if (!op1.empty() || !op2.empty() || !op3.empty()) return 0;
            return 32;

        } 
        else if ((mne == "PUSH" || mne == "POP")){

            // PUSH and POP have one operand
            if (!op2.empty() || !op3.empty()) return 0;
            return 32;
        }
        else{

            // arithmetic and logic instructions have three operands
            if (op1.empty() || op2.empty() || op3.empty()) return 0;
            return 32;
        }
    }
    return 64;
}

std::string right_padding(const std::string &str, const size_t size){
    std::string s = str;
    while(s.length() < size) s+=' ';
    return s;
}

std::string left_padding(const std::string &str, const size_t size){
    std::string s = str;
    s.insert(s.begin(), size-s.length(), ' ');
    return s;
}
