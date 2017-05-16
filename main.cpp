#include <iostream>
#include <string>
#include "symbol.h"
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include "line.h"

#define LC_START (0)
#define MNE_CNT (31)
#define KW_CNT (18)

using namespace std;

const string mnemonics[MNE_CNT] = {
							"INT", "JMP", "CALL", "RET", "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ",
							"LOAD", "STORE", "PUSH", "POP", "ADD", "SUB", "MUL", "DIV", "MOD", "AND",
							"OR", "XOR", "NOT", "ASL", "ASR", "UBLOAD", "SBLOAD", "UWLOAD", "SWLOAD",
							"BSTORE", "WSTORE" 
						 };

const string key_words[KW_CNT] = {"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14",
							"R15","SP","PC"
						};
						// DUP, DEF, ORG?

// TODO: VECTOR OF USER DEFINED KEY WORDS
// TODO: CHECK NAME AVAILABILITY FUNCTION

bool is_digits(const string &str)
{
     return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_alphanum(const string &str){
	 for (size_t i = 0;i<str.length();i++){
	 	if (!isalnum(str[i]) && str[i] != '_') return false;
	 }
	 return true;
}

void strip_off_spaces(string &str){

		while(!str.empty() && isspace(*str.begin()))
    		str.erase(str.begin());

    	while(!str.empty() && isspace(*str.rbegin()))
    		str.erase(str.length()-1);
}

bool is_mnemonic(const string &str){
	for (int i=0;i<MNE_CNT;i++){
		if (mnemonics[i]==str) return true;
	}
	return false;
}

bool is_key_word(const string &str){
	for (int i=0;i<KW_CNT;i++){
		if (key_words[i]==str) return true;
	}
	return false;
}

int main(){

	ifstream infile("tekst");

	string line;
	string label_name;
	bool is_line_labeled;
	int line_counter = 0;
	int section_id_counter = 0;
	// TODO: int location_counter = LC_START; 

	// reset stream 
	infile.clear();
	infile.seekg(0);

	while (getline(infile, line)){
		std::size_t found;
		std::size_t found_comma;
		std::size_t found_space;


		is_line_labeled = false;
		line_counter++;

		// strip off spaces at the beginning of the line
		while(!line.empty() && isspace(*line.begin()))
    		line.erase(line.begin());

    	// strip off comment before spaces 
    	found = line.find(';');
    	if (found != string::npos){
    		line.erase(found, string::npos);
    	}

    	// strip off space at the end of the line
    	while(!line.empty() && isspace(*line.rbegin()))
    		line.erase(line.length()-1);

    	// is it's empty line skip parsing
    	if (line.empty()) continue;
    	
    	// check if line is labeled
		found = line.find(":");
		if (found != string::npos){
			label_name = line.substr(0, found);

			// TODO: LABEL_NAME MUST NOT BE ANY OF THE RESERVED WORDS

			if (!is_alphanum(label_name) || is_key_word(label_name)){
				cout << "Line: " << line_counter << ". Illegal label definition." << endl;
				exit(1);
			}

			is_line_labeled = true;

			// TODO: PUT LABEL IN SYMBOL TABLE

			// strip off label and spaces
			line.erase(0,found+1);
			strip_off_spaces(line);
		}

    	// check if line is a section definition
    	if (*line.begin()=='.'){

    		// if it's end of file, break the loop
    		if (line.substr(1,3) == "end") break;

    		if (line.substr(1,6) == "global"){
    			found_space = line.find(" ");
    			if (found_space == string::npos){
    				cout << "Line: " << line_counter << ". .global arguments missing." << endl;
    				exit(1);
    			}
    			string globals = line.substr(found_space+1, string::npos);

    			// TODO: PUT LINE IN LINE LIST

    			continue;
    		}

    		// parse section name out of line
    		string section_name;
    		found = line.find('.',1);
    		if (found != string::npos) section_name = line.substr(1, found -1);
    		else{
    			section_name = line;
    			section_name.erase(section_name.begin());
    		}

    		// error?
    		if (
    			(section_name != "data" && section_name != "text" && section_name != "rodata" && section_name != "bss") || 
    			(found != string::npos && !is_digits(line.substr(found+1, string::npos)))
    			)
    		{
    			cout << "Line: " << line_counter << ". Illegal section definition." << endl;
    			exit(1);
    		}

    		section_id_counter++;

    		// TODO: PUT SECTION IN SYMBOL TABLE
    		// TODO: PUT LINE IN LINE LIST

    		continue;
    	}

    	// check if line is a DEF directive
    	found = line.find(" DEF ");
    	if (found != string::npos){
    		found_space = line.find(" ");
    		string symbol_name = line.substr(0, found_space);

			// TODO: SYMBOL_NAME MUST NOT BE ANY OF THE RESERVED WORDS

    		if (!is_alphanum(symbol_name) || is_key_word(symbol_name)){
    			cout << "Line: " << line_counter << ". Illegal constant definition." << endl;
				exit(1);
    		}
    		string symbol_value = line.substr(found+5, string::npos);

    		strip_off_spaces(symbol_value);

    		// cout << "DEF" << endl;
    		// cout << "name:" << symbol_name << "*" << endl;
    		// cout << "value:" << symbol_value << "*" << endl;
    		// cout << endl;

    		// TODO: PUT CONSTANT IN SYMBOL TABLE
    		// TODO: PUT LINE IN LINE LIST
    		continue;
    	}

    	// check if line is an ORG directive
    	if (line.substr(0,4) == "ORG "){
    		// TODO: ASSIGN NEW VALUE TO LC
    		// TODO: PUT LINE IN LINE LIST
    		// TODO: NEXT LINE MUST BE SECTION
    		continue;
    	}

    	// check if line is defining data
    	if ((line[0] == 'D') && 
    		(line[1]=='B' || line[1]=='W' || line[1]=='D') &&
    		(line[2] == ' ') 
    		){

    		string data_value = "";
    		string data_rept = "";

    		found = line.find(" DUP ");
    		if (found != string::npos){
    			data_rept = line.substr(2, found-2);
    			data_value = line.substr(found+4, string::npos);
    		}else{
    			data_value = line.substr(3, string::npos);
    		}
    		strip_off_spaces(data_value);
    		strip_off_spaces(data_rept);

    		// cout << "DD" << endl;
    		// cout << "value:" << data_value << "*" << endl;
    		// cout << "rept:" << data_rept << "*" << endl;
    		// cout << endl;

    		// TODO: ASSIGN NEW VALUE TO LC
    		// TODO: PUT LINE IN LINE LIST
    		// TODO: WRITE TO OBJECT FILE?

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

    		// parsing operands
    		for (int i=0;i<3;i++){

    			// find first appearance of comma if any
    			found_comma = ops_string.find(",");
    			ops[i] = ops_string.substr(0,found_comma);

    			// strip spaces, if MNE ,    ,
    			// ops[i] will be just spaces and that's an error
    			// that will be caught in next if 
    			strip_off_spaces(ops[i]);

    			// if ops[i] is empty there is a syntax error:
    			// MNE ,
    			// MNE ,,
    			// MNE a,b, - comma at the end
    			if (ops[i].empty()){
    				cout << "Line: " << line_counter << ". Comma syntax error." << endl;
    				exit(1);
    			}

    			// erase operand from ops_string and go for the next operand
    			ops_string.erase(0,found_comma+1);
    			if (found_comma == string::npos) break;
    		}

    		// if there were 3 or more commas
    		if (found_comma != string::npos){
    			cout << "Line: " << line_counter << ". Too many arguments." << endl;
    			exit(1);
    		}

    		for (int i=0;i<3;i++){
    			cout << " *" << ops[i] << "* ";
    		}
    		cout << endl;
    	
    		// TODO: ASSIGN NEW VALUE TO LC
    		// TODO: PUT LINE IN LINE LIST
    		// TODO: CHECK OPS FOR ERRORS, IN SECOND PASS?
    		
    		continue;
    	}

    	cout << "Line: " << line_counter << ". Syntax error." << endl;
		exit(1);
	}

	cout << "#LINES:" << line_counter << "." << endl;

	return 0;
}