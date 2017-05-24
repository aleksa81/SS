#ifndef RELOCATION_
#define RELOCATION_

#include <string>
#include "TS_entry.h"

class Section;

class Relocation{
private:
	size_t offset;
	std::string type;
	int symbID;
	Section* my_section;

	Relocation* next;
	Relocation* prev;
public:

	Relocation(size_t offset, std::string type, 
			   int symbID, Section* my_section);

};



#endif