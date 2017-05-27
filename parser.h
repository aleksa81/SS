#ifndef PARSER_
#define PARSER_

#include <fstream>
#include <string>
#include <iostream>

#define LC_START (0)

class Parser{
private:
    static std::string label_name;
    static int line_counter; 
    static int location_counter; 

    static int ORG_VALUE;
    static bool ORG_FLAG;

    static bool was_ended;

    Parser(){}

    static void get_label(std::string &line);
    static bool is_end(std::string &line);
    static bool is_global(std::string &line);
    static bool is_section(std::string &line);
    static bool is_definition(std::string &line);
    static bool is_org(std::string &line);
    static bool is_data_definition(std::string &line);
    static bool is_instruction(std::string &line);

public:
    static void error(std::string body);
    static Parser& getInstance(){

        // initialized only once (on first encounter)
        // deleted when program terminates
        static Parser instance;

        return instance;
    }

    Parser(Parser const&) = delete;
    void operator=(Parser const&) = delete;
    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status.

    void parse(std::string file_name);

    friend class Line;
    friend class Section;
    friend class Relocation;
    friend class Loader;

};

#endif