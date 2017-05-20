#include "parser.h"
#include "util.h"
#include "TS_entry.h"
#include "line.h"

#define LC_START (0)

int Parser::line_counter = 0;
int Parser::location_counter = 0;
std::string Parser::label_name = ""; // zbog Line()

int Parser::ORG_VALUE = 0;
bool Parser::ORG_FLAG = false;

void Parser::parse(std::string file_name){

    std::ifstream my_file(file_name.c_str());

    std::string line;

    while (getline(my_file, line)){

        Parser::line_counter++;
        Parser::label_name = "";

        strip_off_comment(line);
        strip_off_spaces(line);

        if (line.empty()) continue;

        Parser::get_label(line);

        if (Parser::is_end(line)) break;
        else if (Parser::is_global(line)) continue;
        else if (Parser::is_section(line)) continue;
        else if(Parser::is_definition(line)) continue;
        else if (Parser::is_org(line)) continue;
        else if (Parser::is_data_definition(line)) continue;
        else if (Parser::is_instruction(line)) continue;
        else Parser::error("Syntax error.");
    }

    TS_entry::init();
    Line::print();
    TS_entry::print();
}

void Parser::get_label(std::string &line){
    size_t found = line.find(":");
    if (found != std::string::npos){
        Parser::label_name = line.substr(0, found);

        if (!is_alphanum(Parser::label_name) || TS_entry::is_key_word(Parser::label_name)){
            Parser::error("Illegal label definition.");
        }

        bool ok = Symbol::add_symbol_as_defined(label_name, Parser::location_counter, SYMBOL_LABEL);
        if (!ok){
            Parser::error("Symbol redefinition.");
        }

        // strip off label and spaces
        line.erase(0,found+1);
        strip_off_spaces(line);
    }
}

bool Parser::is_end(std::string &line){
    if (line.substr(0,4) == ".end"){
        Section::current->setSize(Parser::location_counter);
        return true;
    }
    return false;
}

bool Parser::is_global(std::string &line){
    if (line.substr(0,7) == ".global"){
        size_t found_space = line.find(" ");
        if (found_space == std::string::npos){
            Parser::error(".global arguments missing.");
        }

        std::string globals = line.substr(found_space+1, std::string::npos);
        strip_off_spaces(globals);

        while (true){
            size_t found_comma = globals.find(",");
            std::string g_symbol = globals.substr(0, found_comma);
            strip_off_spaces(g_symbol);
            globals.erase(0, found_comma+1);
            Symbol::add_symbol_as_global(g_symbol, 0);
            if (found_comma == std::string::npos) break;
        }

        new Line(line, "", "", "", 0, false);
        return true;
    }
    return false;
}

bool Parser::is_section(std::string &line){
    if (line[0] == '.'){
        size_t found = line.find('.',1);
        std::string section_type;
        if (found != std::string::npos) section_type = line.substr(1, found -1);
        else section_type = line.substr(1, std::string::npos);

        // error?
        if (
            (section_type != "data" && section_type != "text" && section_type != "rodata" && section_type != "bss") || 
            (found != std::string::npos && !is_digits(line.substr(found+1, std::string::npos))) || (TS_entry::is_key_word(line))
            )
        {
            Parser::error("Illegal section definition.");
        }

        Section::add_section(line, Parser::location_counter, section_type);
        Parser::location_counter = LC_START;
        if (Parser::ORG_FLAG == true){
            Section::current->setStart(Parser::ORG_VALUE);
            Parser::ORG_FLAG = false;
        }
        
        new Line(line, "", "", "", 0, true);

        return true;
    }
    if (Parser::ORG_FLAG == true) Parser::error("No section definition after ORG.");
    return false;
}

bool Parser::is_definition(std::string &line){
    size_t found = line.find(" DEF ");
    size_t found_space;
    if (found != std::string::npos){
        found_space = line.find(" ");
        std::string symbol_name = line.substr(0, found_space);

        if (!is_alphanum(symbol_name) || TS_entry::is_key_word(symbol_name)){
            Parser::error("Illegal constant definition.");
        }
        std::string symbol_value = line.substr(found+5, std::string::npos);

        strip_off_spaces(symbol_value);

        // TODO: constant expression
        if (!is_absolute(symbol_value)){
            Parser::error("DEF must have an absolute argument.");
        }

        bool ok = Symbol::add_symbol_as_defined(symbol_name, str_to_int(symbol_value), SYMBOL_CONSTANT);
        if (!ok){
            Parser::error("Symbol redefinition.");
        }

        new Line("DEF", symbol_name, symbol_value, "", 0, false);

        return true;
    }
    return false;
}

bool Parser::is_org(std::string &line){
    if (line.substr(0,4) == "ORG "){
        std::string value = line.substr(4, std::string::npos);
        strip_off_spaces(value);
        if (!is_absolute(value)){
            Parser::error("ORG must have an absolute argument.");
        }
        Parser::ORG_VALUE = str_to_int(value);
        Parser::ORG_FLAG = true;

        new Line("ORG", value, "", "", 0, false);
        return true;
    }
    return false;
}

bool Parser::is_data_definition(std::string &line){
    if ((line[0] == 'D') && 
        (line[1]=='B' || line[1]=='W' || line[1]=='D') &&
        (line[2] == ' ') 
        ){

        std::string data_value = "";
        std::string data_rept = "1";

        size_t found = line.find(" DUP ");
        if (found != std::string::npos){
            data_rept = line.substr(2, found-2);
            data_value = line.substr(found+4, std::string::npos);
        }else{
            data_value = line.substr(3, std::string::npos);
        }
        strip_off_spaces(data_value);
        strip_off_spaces(data_rept);

        // TODO: constant expression
        if (!is_absolute(data_rept) || !is_absolute(data_value)){
            Parser::error("Ds must have absolute arguments.");
        }

        int data_size;
        int data_irept = str_to_int(data_rept);
        if (line[1] == 'B') data_size = 1;
        else if (line[1] == 'W') data_size = 2;
        else data_size = 4;

        Parser::location_counter += data_irept * data_size;

        new Line(line.substr(0,2), data_rept, "DUP", data_value, data_irept * data_size, false);

        return true;
    }
    return false;
}

bool Parser::is_instruction(std::string &line){
    size_t found = line.find(" ");
    if (found != std::string::npos){
        std::string mnemonic = line.substr(0, found);
        if (!is_mnemonic(mnemonic)){
            Parser::error("Unknown mnemonic error.");
        }

        // Strip off mnemonic and make ops_string which contains all the operands.
        std::string ops_string = line.substr(found+1, std::string::npos);

        // Strip off spaces from ops_string.
        strip_off_spaces(ops_string);

        // Max of 3 operands are allowed, ops[i] will be empty if less.
        std::string ops[3];

        // Parsing operands:
        //   - Split string with comma separator, 
        //   - Strip off spaces, if empty -> error,
        //   - If more then 3 commas -> error,
        //   - Cut string in each iteration.

        size_t found_comma;

        for (int i=0;i<3;i++){

            found_comma = ops_string.find(",");
            ops[i] = ops_string.substr(0, found_comma);

            strip_off_spaces(ops[i]);

            if (ops[i].empty()){
                Parser::error("Comma syntax error.");
            }

            ops_string.erase(0,found_comma+1);
            if (found_comma == std::string::npos) break;
        }

        // if there were 3 or more commas
        if (found_comma != std::string::npos){
            Parser::error("Too many arguments.");
        }

        int instr_size = get_instruction_size(mnemonic, ops[0], ops[1], ops[2]);
        if (instr_size == 0){
            Parser::error("Arguments number mismatch.");
        }

        Parser::location_counter += instr_size;
    
        new Line(mnemonic, ops[0], ops[1], ops[2], instr_size, false);
        return true;
    }
    return false;
}

void Parser::error(std::string error_body){
    std::cout << "Line: " << Parser::line_counter << ". " << error_body << std::endl;
    exit(1);
}