#include "util.h"
#include <cstdlib> // strtol
#include <set>

// check first operand to get instruction size (32/64 bit)
const std::set<std::string> check_first_operand_mnemonics = 
{"INT", "JMP", "CALL" };

// check second operand to get instruction size (32/64 bit)
const std::set<std::string> check_second_operand_mnemonics = 
{"JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ", "LOAD", "STORE"};

// constant instruction size (32 bit)
const std::set<std::string> check_no_operand_mnemonics =
{"RET", "PUSH", "POP", "ADD", "SUB", "MUL", "DIV", "MOD", "AND", "OR", "XOR", "NOT", "ASL", "ASR"};

// all registers that can be used by user for addressing
const std::set<std::string> regs = 
{"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14","R15"};

bool is_digits(const std::string &str)
{
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

bool is_mnemonic(const std::string &str){
    if (check_first_operand_mnemonics.find(str) == check_first_operand_mnemonics.end() &&
    	check_second_operand_mnemonics.find(str) == check_second_operand_mnemonics.end() &&
    	check_no_operand_mnemonics.find(str) == check_no_operand_mnemonics.end()) return false;
    return true;
}

int instruction_size(std::string op){ 
	if (regs.find(op) != regs.end()) return 32;
	size_t found1 = op.find("[");
	size_t found2 = op.find("]");
	std::string ind = op.substr(found1+1, found2-found1-1);
	strip_off_spaces(ind);
	if (regs.find(ind) != regs.end()) return 32;
	return 64;
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