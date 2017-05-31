#ifndef LOADER_
#define LOADER_

#include <unordered_map>
#include <string>
#include "TS_entry.h"

class TS_entry;

class Loader{
private:
    Loader(){}

    static std::string operand;
    static std::string reloc_type;
    static int value;
    static TS_entry* reloc_symb;
    static std::string reg;
    static std::string disp;
    static int adr_mode;

    static std::unordered_map<std::string, int> opcode;
    static std::unordered_map<std::string, int> regcode;

public:

    static Loader& getInstance(){

        // initialized only once (on first encounter)
        // deleted when program terminates
        static Loader instance;

        return instance;
    }

    Loader(Loader const&) = delete;
    void operator=(Loader const&) = delete;
    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status.

    void load();

    static void process_reg_ind_disp(int regpos);
    static void process_reg_dir(int regpos);
    static void process_pc_rel(int regpos);
    static void process_immed();
    static void process_mem_dir();

    static bool is_store(const std::string &str);
    static bool is_load(const std::string &str);
    static bool is_INT(const std::string &str);
    static bool is_JZ(const std::string &str);

    static void process_store(const std::string &str);
    static void process_load(const std::string &str);

};

#endif