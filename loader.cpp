#include "loader.h"
#include "TS_entry.h"
#include <iostream>

#define IMMED (4)
#define REGDIR (0)
#define MEMDIR (6)
#define REGIND (2)
#define REGINDDISP (7)

#define DOUBLE_WORD (0)
#define UWORD (1)
#define WORD (5)
#define UBYTE (3)
#define BYTE (7)

std::unordered_map<std::string, char> Loader::opcode{
	{"INT",    0},
	{"JMP",    2},
	{"CALL",   3},
	{"RET",    1},
	{"JZ",     4},
	{"JNZ",    5},
	{"JGZ",    6},
	{"JGEZ",   7},
	{"JLZ",    8},
	{"JLEZ",   9},

	{"LOAD",  16},
	{"STORE", 17},

	{"PUSH",  32},
	{"POP",   33},

	{"ADD",   48},
	{"SUB",   49},
	{"MUL",   50},
	{"DIV",   51},
	{"MOD",   52},
	{"AND",   53},
	{"OR",    54},
	{"XOR",   55},
	{"NOT",   56},
	{"ASL",   57},
	{"ASR",   58}	
};

std::unordered_map<std::string, char> Loader::regcode{
	{"R0",   0},
	{"R1",   1},
	{"R2",   2},
	{"R3",   3},
	{"R4",   4},
	{"R5",   5},
	{"R6",   6},
	{"R7",   7},
	{"R8",   8},
	{"R9",   9},
	{"R10", 10},
	{"R11", 11},
	{"R12", 12},
	{"R13", 13},
	{"R14", 14},
	{"R15", 15},

	{"SP",  16},
	{"PC",  17}
};

void Loader::load(){
	Section::current = (Section*)Section::head;

	//std::cout << std::endl;

	while (Section::current != nullptr){

		// Section::current and LC change in every iteration
		Line* line = Section::current->line_head;
		Parser::location_counter = LC_START;


		while (line != nullptr){

			// Parser::line_counter changes in every iteration
			Parser::line_counter = line->line_number;

			

			line = line->next;
		}


		Section::current = (Section*)Section::current->next;
	}


}