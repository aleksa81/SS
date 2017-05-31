#ifndef UTIL_
#define UTIL_
#include <string>
#include <vector>

#define PADDING
//#define BINARY_MACHINE_CODE

class TS_entry;

bool is_digits(const std::string &str);

bool is_hexadecimals(const std::string &str);

bool is_binaries(const std::string &str);

bool is_alphanum(const std::string &str);

void strip_off_spaces(std::string &str);

void strip_off_comment(std::string &str);

bool is_mnemonic(const std::string &str);

bool is_key_word(const std::string &str);

bool is_reg_dir(const std::string &op); // addressing

bool is_reg_ind(const std::string &op); // addressing

bool is_mem_dir(const std::string &op); // addressing

bool is_pc_rel(const std::string &op); // addressing

bool is_immed(const std::string &op); // addressing

bool is_reg_ind_disp(const std::string &op, std::string &reg, std::string &disp); // addressing

bool is_binary(const std::string &str);

bool is_hexadecimal(const std::string &str);

bool is_decimal(const std::string &str);

bool is_absolute(const std::string &str);

bool is_const_expr(const std::string &str);

long str_to_int(const std::string &str);

size_t get_instruction_size(const std::string &mne,
                            const std::string &op1,
                            const std::string &op2,
                            const std::string &op3);

std::string right_padding(const std::string &str, const size_t size);

std::string left_padding(const std::string &str, const size_t size);

int getStackPrio(std::string op);

int getCurrentPrio(std::string op);

bool is_lequ_prio(char op1, char op2);

std::string infix_to_postfix(std::string input);

int calc_postfix(std::string input, TS_entry*& reloc_symb);

int calc_const_expr(std::string input, TS_entry*& reloc_symb);

bool is_operation(std::string op);

int calc_const_expr_no_reloc(std::string input);

bool is_constant(const std::string &str);

bool are_constants(const std::string&, const std::string&);

bool is_label_or_extern(const std::string &str);

bool are_from_same_section_labels(const std::string &l1, 
                                  const std::string &l2);

bool exists_symbol(const std::string& str);

std::string get_reg(const std::string& str);

std::string str_to_hex(const std::string &str);

#endif