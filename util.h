#ifndef UTIL_
#define UTIL_
#include <string>
#include <vector>

#define MEM_DIR (0)
#define MEM_IND (1)
#define REG_DIR (2)
#define REG_IND (3)
#define REG_IND_DISP (4)
#define PC_REL (5)
#define IMMED (6)

bool is_digits(const std::string &str);

bool is_hexadecimals(const std::string &str);

bool is_binaries(const std::string &str);

bool is_alphanum(const std::string &str);

void strip_off_spaces(std::string &str);

void strip_off_comment(std::string &str);

bool is_mnemonic(const std::string &str);

bool is_key_word(const std::string &str);

bool is_reg_dir(const std::string &op);

bool is_reg_ind(const std::string &op);

bool is_mem_dir(const std::string &op);

bool is_mem_ind(const std::string &op);

bool is_pc_rel(const std::string &op);

bool is_reg_ind_disp(const std::string &op);

bool is_immed(const std::string &op);

bool is_binary(const std::string &str);

bool is_hexadecimal(const std::string &str);

bool is_absolute(const std::string &str);

bool is_const_expr(const std::string &str);

int str_to_int(const std::string &str);

size_t get_instruction_size(const std::string &mne,
						 const std::string &op1,
						 const std::string &op2,
						 const std::string &op3);

#endif