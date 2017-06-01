#include "objfile.h"
#include "TS_entry.h"
#include "relocation.h"
#include "line.h"
#include "util.h"
#include <iomanip>
#include <bitset>

void ObjFile::make(std::string file_name){

	output.open(file_name);

	output << "#TabelaSimbola" << std::endl;

    for (TS_entry* i = Section::head; i != nullptr; i = i->next){
    	std::string flags;
	    if (i->type == SECTION_DATA) flags = "WPA";
	    else if (i->type == SECTION_RODATA) flags = "RPA";
	    else if (i->type == SECTION_TEXT) flags = "XPA";
	    else flags = "W-A";

	    if (((Section*)i)->isStatic()) flags.append("S");
	    else flags.append("-");


        output << "SEG " << i->ID << "" << i->getName() << " " << i->ID << 
               " 0x" << std::setfill('0') << std::setw(8) << std::hex << i->getValue() <<
        	   " 0x" << std::setfill('0') << std::setw(8) << std::hex << ((Section*)i)->getSize() << " " <<
        	   flags << std::endl;
    }

    for (TS_entry* i = Symbol::head; i != nullptr; i = i->next){
    	std::string my_section;
	    if (i->type == SYMBOL_EXTERN) 
	        my_section = "0";
	    else if (i->type == SYMBOL_CONSTANT || i->section->is_static == true) 
	        my_section = "-1";
	    else 
	        my_section = std::to_string(i->section->ID);

	    std::string scope;
	    if (i->scope_flag == SCOPE_GLOBAL) scope = "G";
	    else scope = "L";

        output << "SYM " << std::dec <<i->getID() << " " << i->getName() << " " << my_section <<
        	      " 0x" << std::setfill('0') << std::setw(8) << std::hex << (unsigned)i->getValue() <<
        	      " " << scope << std::endl;
    }

	/* -------------------------------------------------------------------------------------- */
    /* ------------------------------------- RELOCATION ------------------------------------- */
    /* -------------------------------------------------------------------------------------- */

    int is_little_endian = 0;

    int i =1;
    if (*(char*)&i == 1)
        is_little_endian = 0; // is little endian: =0 for little endian print, =1 for big endian
    else
        is_little_endian = 1; // is big endian: =1 for little endian print, =0 for big endian

    int byte_cntr = 0;

    for (TS_entry* s = Section::head; s != nullptr; s = s->next){

    	// bss section doesn't have data in obj file
    	if (s->getType()==SECTION_BSS)
    		continue;

    	if(((Section*)s)->reloc_head != nullptr)
            output << "#" << s->getName() << std::endl;

        // Print all relocation for current section
        for (Relocation* i = ((Section*)s)->reloc_head; i != nullptr; i=i->next){

	        unsigned target;
		    if (i->target == nullptr)
		        target = 0;
		    else 
		        target = i->target->getID();

            output << "0x" << std::setfill('0') << std::setw(8) << std::hex << i->offset <<
            	      " " << i->type << " " << std::dec <<target << std::endl;
        }

        output << s->getName() << std::endl;

        Line* line = ((Section*)s)->line_head;
        size_t dsize = 0;
        size_t drept = 0;
        size_t dbase = 0;
        byte_cntr = 0;
      
        while(line != nullptr){
            if (line->mnemonic == "DD"){
                dsize = 4;
                drept = str_to_int(line->ops[0]);
            }
            else if (line->mnemonic == "DW"){
                dsize = 2;
                drept = str_to_int(line->ops[0]);
            }
            else if (line->mnemonic == "DB"){
                dsize =1;
                drept = str_to_int(line->ops[0]);
            }
            else{
                dsize = 4;
                drept = line->getSize() / 4;
            }

            //std::cout << "data size: " << dsize << " rept: " <<drept <<std::endl;

            for (size_t i = 0; i < drept; i++){
                for (size_t j = 0; j < dsize; j++){

                    std::bitset<8> bs(((Section*)s)->machine_code
                        [dbase + (i+is_little_endian*1)*dsize-(1+j)*is_little_endian + j*(1-is_little_endian)]);

                    output << str_to_hex(bs.to_string()) << " ";

                    byte_cntr++;

                    if (byte_cntr == 16){
	                	output << std::endl;
	                	byte_cntr = 0;
                	}
                }
            }

            dbase += dsize*drept;

            line = line->next;
        }
        if (byte_cntr != 0)
        	output << std::endl;
    }

    output << "#end" << std::endl;

	output.close(); 
}