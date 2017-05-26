#include "util.h"
#include "TS_entry.h"
#include <cstdlib> // strtol
#include <set>
#include <iostream>
#include <stack>

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
    return (str[0] == '0' && (str[1] == 'b' || str[1] == 'B') && 
           is_binaries(str.substr(2, std::string::npos)));
}

bool is_hexadecimal(const std::string &str){
    return str[0] == '0' && (str[1] == 'x' || str[1] == 'X') && 
           is_hexadecimals(str.substr(2, std::string::npos));
}

bool is_decimal(const std::string &str){
    return (str[0] == '-' && is_digits(str.substr(1, std::string::npos))) || is_digits(str);
}

bool is_absolute(const std::string &str){
    return is_decimal(str) || is_binary(str) || 
       is_hexadecimal(str) || (str[0]=='\'' && str[2] == '\'' && isalpha(str[1]));
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
    if (is_decimal(str)) 
        return strtol(str.c_str(), nullptr, 10);
    if (str[0]=='\'' && str[2] == '\'' && isalpha(str[1]))
        return str[1];
    Parser::error("String to int conversion failed.");
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
        if (is_reg_dir(op1) || is_reg_ind(op1)) return 4;
    }
    else if (check_second_operand_mnemonics.find(mne) != check_second_operand_mnemonics.end()){
        if (!op3.empty()) return 0;
        if (is_reg_dir(op2) || is_reg_ind(op2)) return 4;
    }
    else if(check_no_operand_mnemonics.find(mne) != check_no_operand_mnemonics.end()){
        if (mne == "RET"){

            // RET has no operands
            if (!op1.empty() || !op2.empty() || !op3.empty()) return 0;
            return 4;

        } 
        else if ((mne == "PUSH" || mne == "POP")){

            // PUSH and POP have one operand
            if (!op2.empty() || !op3.empty()) return 0;
            return 4;
        }
        else{

            // arithmetic and logic instructions have three operands
            if (op1.empty() || op2.empty() || op3.empty()) return 0;
            return 4;
        }
    }
    return 8;
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

int getCurrentPrio(std::string op){
    if (op == "(") return 4;
    else if (op == "/" || op == "*") return 3;
    else if (op == "+" || op == "-") return 2;
    else if (op == ")") return 1;
    else{
        Parser::error("Operator unknown in constant expression.");
        return 0;
    }
}

int getStackPrio(std::string op){
    if (op == "(") return 0;
    else if (op == "/" || op == "*") return 3;
    else if (op == "+" || op == "-") return 2;
    else{
        Parser::error("Operator unknown in constant expression.");
        return 0;
    }
}

bool is_lequ_prio(std::string op1, std::string op2){
    return getCurrentPrio(op1) <= getStackPrio(op2);
}

std::string infix_to_postfix(std::string input){
    // will check operators and operands number

    std::string output= "";
    std::stack<std::string> op_stack;
    std::string token;

    // must be equal to 1 at the end of conversion
    int equilibrium = 0;

    while (!input.empty()){
        strip_off_spaces(input);
        size_t found_op = input.find_first_of("+-/*()");

        if (found_op != 0){
            // if operand is found put it in output string

            token = input.substr(0, found_op);
            strip_off_spaces(token);
            token.insert(0, " ");
            output.insert(output.length(), token);

            // increment for every operand found
            equilibrium += 1;
        }

        if (found_op == std::string::npos) break;

        // decrement for every binary operation found
        if (is_operation(std::string(1,input[found_op]))) equilibrium -= 1;

        while (!op_stack.empty() && is_lequ_prio(std::string(1,input[found_op]), op_stack.top())){
            std::string opstr = " " + op_stack.top();
            op_stack.pop();
            output.insert(output.length(), opstr);
        }
        if (std::string(1,input[found_op]) == ")") op_stack.pop(); // stack will have "(" on top
        else op_stack.push(std::string(1,input[found_op]));

        input.erase(0, found_op+1);

    }

    while (!op_stack.empty()){
        std::string opstr = " " + op_stack.top();
        op_stack.pop();
        output.insert(output.length(), opstr);
    }

    if (equilibrium != 1){
        Parser::error("Constant expression error.");
    }

    strip_off_spaces(output);
    return output;
}

int calc_postfix(std::string input, TS_entry*& reloc_symb){
    std::string token;
    std::stack<std::string> ops_stack;

    // Relocation symbol
    reloc_symb = nullptr;
    
    while (true){

        size_t found_space = input.find(" ");
        token = input.substr(0, found_space);
        input.erase(0, found_space+1);
        if (!is_operation(token))
            ops_stack.push(token);
        else if (is_operation(token)){

            std::string op2 = ops_stack.top();
            ops_stack.pop();

            std::string op1 = ops_stack.top();
            ops_stack.pop();

            int iop1, iop2, iresult;
            TS_entry* symb1 = nullptr; 
            TS_entry* symb2 = nullptr;

            if (is_absolute(op1)) 
                iop1 = str_to_int(op1);
            else if(exists_symbol(op1)){
                symb1 = TS_entry::TS_entry_mapping[op1];
                iop1 = symb1->getValue();
            }else 
                Parser::error("Operand unknown in constant expression.");

            if (is_absolute(op2)) 
                iop2 = str_to_int(op2);
            else if(exists_symbol(op2)){
                symb2 = TS_entry::TS_entry_mapping[op2];
                iop2 = symb2->getValue();
            }else 
                Parser::error("Operand unknown in constant expression.");
            
            if (token == "+") {
                if (is_label_or_extern(op1) && is_constant(op2) && reloc_symb == nullptr){
                    // Assign relocation symbol
                    reloc_symb = symb1;
                    //std::cout << "Relocation on symbol: " << symb1->getName() << std::endl;
                }
                else if (is_label_or_extern(op2) && is_constant(op1) && reloc_symb == nullptr){
                    // Assign relocation symbol
                    reloc_symb = symb2;
                    //std::cout << "Relocation on symbol: " << symb2->getName() << std::endl;
                }
                else if (are_constants(op1,op2)){
                    // Idle ...
                }
                else 
                    Parser::error("Illegal constant expression.");
                iresult = iop1 + iop2;
            }
            else if (token == "-") {
                if (is_label_or_extern(op1) && is_constant(op2) && reloc_symb == nullptr){
                    // Assign relocation symbol
                    reloc_symb = symb1;
                    //std::cout << "Relocation on symbol: " << symb1->getName() << std::endl;
                } 
                else if (are_from_same_section_labels(op1,op2) || are_constants(op1, op2)){
                    // Idle ...
                }
                else 
                    Parser::error("Illegal constant expression.");
                iresult = iop1 - iop2;
            }
            else if (token == "/") {
                if (!are_constants(op1,op2)) 
                    Parser::error("Illegal constant expression.");
                iresult = iop1 / iop2;
            }
            else if (token == "*") {
                if (!are_constants(op1,op2)) 
                    Parser::error("Illegal constant expression.");
                iresult = iop1 * iop2;
            }

            std::string result = std::to_string(iresult);
            ops_stack.push(result);
        }

        if (found_space == std::string::npos) 
            break;
    }

    std::string value = ops_stack.top();
    ops_stack.pop();

    int ivalue;

    // Will not be absolute when there is only one symbol to calculate
    if (is_absolute(value)) 
        ivalue = str_to_int(value);
    else if (exists_symbol(value)){ 
        ivalue = TS_entry::TS_entry_mapping[value]->getValue();
        if (is_label_or_extern(value))
            reloc_symb = TS_entry::TS_entry_mapping[value];
    }
    else 
        Parser::error("Operand unknown in constant expression.");

    return ivalue;
}

bool is_operation(std::string op){
    return op == "+" || op == "-" || op == "/" || op == "*";
}

int calc_const_expr(std::string input, TS_entry*& reloc_symb){
    return calc_postfix(infix_to_postfix(input), reloc_symb);
}

int calc_const_expr_no_reloc(std::string input){
    TS_entry* reloc_symb = nullptr;
    int result = calc_postfix(infix_to_postfix(input), reloc_symb);
    if (reloc_symb != nullptr) 
        Parser::error("Symbol relocation in absolute constant expression.");
    return result;
}

bool is_constant(const std::string &str){
    return is_absolute(str) || 
           (exists_symbol(str) && TS_entry::TS_entry_mapping[str]->is_constant());
}

bool is_label_or_extern(const std::string &str){
    return exists_symbol(str) &&
           TS_entry::TS_entry_mapping[str]->is_label_or_extern();
}

bool are_from_same_section_labels(const std::string &l1, const std::string &l2){
    return exists_symbol(l1) &&
           exists_symbol(l2) &&
           TS_entry::TS_entry_mapping[l1]->getSection()->getID() == 
           TS_entry::TS_entry_mapping[l2]->getSection()->getID();
}

bool are_constants(const std::string& str1, const std::string& str2){
    return is_constant(str1) && is_constant(str2);
}

bool exists_symbol(const std::string& str){
    return TS_entry::TS_entry_mapping.find(str) != TS_entry::TS_entry_mapping.end();
}
