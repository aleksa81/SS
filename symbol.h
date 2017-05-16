#ifndef SYMBOL_
#define SYMBOL_

#include <unordered_map>
#include <string>
#include <iostream>

class Symbol{
private:
	static std::unordered_map<std::string, Symbol*> mapping;
	static int ID;
public:
	std::string name;
	std::string section;
	int offset;
	char binding;
	int serial;
	int line;

	Symbol(std::string name, int line, std::string section = "", int offset = 0);

	static Symbol* getSymbol(std::string name);

	static void printAll(){
		std::cout << "SYMBOLS:" << std::endl;
  		for ( auto it = mapping.begin(); it != mapping.end(); ++it ){
    		std::cout << it->first << " line: " << it->second->line << std::endl;
  			//std::cout << std::endl;
		}
	}
};

#endif




