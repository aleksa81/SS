#ifndef SYMBOL_
#define SYMBOL_

#include <unordered_map>
#include <string>

class Symbol{
private:
	static std::unordered_map<std::string, Symbol*> mapping;
	static int ID;
public:
	std::string name;
	int section;
	int offset;
	char binding;
	int serial;

	Symbol(std::string name, int section = 0, int offset = 0);

	static Symbol* getSymbol(std::string name);

	static void addSymbol(Symbol* symbol);
};

#endif




