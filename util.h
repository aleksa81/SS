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

bool is_mnemonic(const std::string &str);

bool is_key_word(const std::string &str);

int instruction_size(std::string op);

bool is_binary(const std::string &str);

bool is_hexadecimal(const std::string &str);

bool is_absolute(const std::string &str);

int str_to_int(const std::string &str);

#endif