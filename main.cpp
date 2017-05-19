#include <iostream>
#include <string>
#include <fstream>
#include "line.h"
#include "TS_entry.h"
#include "util.h"

#define LC_START (0)

using namespace std;

int main(){

    ifstream infile("tekst");

    string line;
    string label_name;
    int line_counter = 0;
    string section_type;
    bool ORG_FLAG = false;
    int ORG_VALUE = 0;

    int location_counter = LC_START; 

    // reset stream 
    infile.clear();
    infile.seekg(0);

    while (getline(infile, line)){
        std::size_t found;
        std::size_t found_comma;
        std::size_t found_space;

        line_counter++;
        label_name = "";

        strip_off_comment(line);
        strip_off_spaces(line);
        if (line.empty()) continue;
        
        // LABEL
        found = line.find(":");
        if (found != string::npos){
            label_name = line.substr(0, found);

            // TODO: LABEL_NAME MUST NOT BE ANY OF THE RESERVED WORDS

            if (!is_alphanum(label_name) || TS_entry::is_key_word(label_name)){
                cout << "Line: " << line_counter << ". Illegal label definition." << endl;
                exit(1);
            }

            bool ok = Symbol::add_symbol_as_defined(label_name, location_counter, Section::current, SYMBOL_LABEL);
            if (!ok){
                cout << "Line: " << line_counter << ". Symbol redefinition." << endl;
                exit(1);
            }

            // strip off label and spaces
            line.erase(0,found+1);
            strip_off_spaces(line);
        }

        // END / GLOBAL / SECTION
        if (*line.begin()=='.'){

            // if it's end of file, break the loop
            if (line.substr(1,3) == "end") {
                Section::current->setSize(location_counter);
                break;
            }

            if (line.substr(1,6) == "global"){
                found_space = line.find(" ");
                if (found_space == string::npos){
                    cout << "Line: " << line_counter << ". .global arguments missing." << endl;
                    exit(1);
                }
                string globals = line.substr(found_space+1, string::npos);

                strip_off_spaces(globals);

                //cout << globals << endl;

                while (true){
                    found_comma = globals.find(",");
                    string g_symbol = globals.substr(0, found_comma);
                    strip_off_spaces(g_symbol);
                    globals.erase(0, found_comma+1);
                    Symbol::add_symbol_as_global(g_symbol, 0, nullptr);
                    //cout << g_symbol << " ";
                    if (found_comma == string::npos) break;
                }
                //cout << endl;

                new CodeLine(label_name, line, "","","", 0, false);

                continue;
            }

            found = line.find('.',1);
            if (found != string::npos) section_type = line.substr(1, found -1);
            else{
                section_type = line;
                section_type.erase(section_type.begin());
            }

            // error?
            if (
                (section_type != "data" && section_type != "text" && section_type != "rodata" && section_type != "bss") || 
                (found != string::npos && !is_digits(line.substr(found+1, string::npos))) || (TS_entry::is_key_word(line))
                )
            {
                cout << "Line: " << line_counter << ". Illegal section definition." << endl;
                exit(1);
            }

            Section::add_section(line, location_counter, section_type);
            location_counter = LC_START;
            if (ORG_FLAG == true){
                Section::current->setStart(ORG_VALUE);
                ORG_FLAG = false;
            }
            
            new CodeLine(label_name, line, "","","", 0, true);

            continue;
        }

        if (ORG_FLAG == true){
            cout << "Line: " << line_counter << ". No section definition after ORG." << endl;
            exit(1);
        }

        // DEF
        found = line.find(" DEF ");
        if (found != string::npos){
            found_space = line.find(" ");
            string symbol_name = line.substr(0, found_space);

            if (!is_alphanum(symbol_name) || TS_entry::is_key_word(symbol_name)){
                cout << "Line: " << line_counter << ". Illegal constant definition." << endl;
                exit(1);
            }
            string symbol_value = line.substr(found+5, string::npos);

            strip_off_spaces(symbol_value);

            // TODO: constant expression
            if (!is_absolute(symbol_value)){
                cout << "Line: " << line_counter << ". DEF must have an absolute argument." << endl;
                exit(1);
            }

            bool ok = Symbol::add_symbol_as_defined(symbol_name, str_to_int(symbol_value), Section::current, SYMBOL_CONSTANT);
            if (!ok){
                cout << "Line: " << line_counter << ". Symbol redefinition." << endl;
                exit(1);
            }

            new CodeLine(label_name, "DEF", symbol_name, symbol_value, "", 0, false);

            continue;
        }

        // ORG
        if (line.substr(0,4) == "ORG "){
            string value = line.substr(4, string::npos);
            strip_off_spaces(value);
            if (!is_absolute(value)){
                cout << "Line: " << line_counter << ". ORG must have an absolute argument." << endl;
                exit(1);
            }
            ORG_VALUE = str_to_int(value);
            ORG_FLAG = true;

            new CodeLine(label_name, "ORG", value, "", "", 0, false);
            continue;
        }

        // DD
        if ((line[0] == 'D') && 
            (line[1]=='B' || line[1]=='W' || line[1]=='D') &&
            (line[2] == ' ') 
            ){

            string data_value = "";
            string data_rept = "1";

            found = line.find(" DUP ");
            if (found != string::npos){
                data_rept = line.substr(2, found-2);
                data_value = line.substr(found+4, string::npos);
            }else{
                data_value = line.substr(3, string::npos);
            }
            strip_off_spaces(data_value);
            strip_off_spaces(data_rept);

            // TODO: constant expression
            if (!is_absolute(data_rept) || !is_absolute(data_value)){
                cout << "Line: " << line_counter << ". Ds must have absolute arguments." << endl;
                exit(1);
            }

            int data_size;
            int data_irept = str_to_int(data_rept);
            if (line[1] == 'B') data_size = 1;
            else if (line[1] == 'W') data_size = 2;
            else data_size = 4;

            location_counter += data_irept * data_size;

            new CodeLine(label_name, line.substr(0,2), data_rept, "DUP", data_value, data_irept * data_size, false);

            continue;
        }

        found = line.find(" ");
        if (found != string::npos){
            string mnemonic = line.substr(0, found);
            if (!is_mnemonic(mnemonic)){
                cout << "Line: " << line_counter << ". Mnemonic syntax error.";
                cout << "\"" << mnemonic << "\"" << " is not a mnemonic." << endl;
                exit(1);
            }

            // strip off mnemonic and make ops_string which contains all the operands
            string ops_string = line.substr(found+1, string::npos);

            // strip off spaces from ops_string 
            strip_off_spaces(ops_string);

            // max of 3 operands are allowed, ops[i] will be empty if less
            string ops[3];

            // parsing operands:
            // split string with comma, strip off spaces, if empty -> error
            // if more then 3 commas -> error
            // cut string in each iteration
            for (int i=0;i<3;i++){

                found_comma = ops_string.find(",");
                ops[i] = ops_string.substr(0,found_comma);

                strip_off_spaces(ops[i]);

                if (ops[i].empty()){
                    cout << "Line: " << line_counter << ". Comma syntax error." << endl;
                    exit(1);
                }

                ops_string.erase(0,found_comma+1);
                if (found_comma == string::npos) break;
            }

            // if there were 3 or more commas
            if (found_comma != string::npos){
                cout << "Line: " << line_counter << ". Too many arguments." << endl;
                exit(1);
            }

            int instr_size = get_instruction_size(mnemonic, ops[0], ops[1], ops[2]);
            if (instr_size == 0){
                cout << "Line: " << line_counter << ". Arguments number mismatch." << endl;
                exit(1);
            }

            location_counter += instr_size;
        
            new CodeLine(label_name, mnemonic, ops[0], ops[1], ops[2], instr_size, false);
            continue;
        }

        cout << "Line: " << line_counter << ". Syntax error." << endl;
        exit(1);
    }

    cout << "#LINES:" << line_counter << "." << endl << endl;

    TS_entry::setIDs();

    CodeLine::print();

    Section::print();
    Symbol::print();

    return 0;
}