#ifndef TS_ENTRY_
#define TS_ENTRY_

#include "line.h"
#include "relocation.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <set>

#define SCOPE_GLOBAL (0)
#define SCOPE_LOCAL (1)

// section types
#define SECTION_DATA (0)
#define SECTION_TEXT (1)
#define SECTION_RODATA (2)
#define SECTION_BSS (3)

// symbol types
#define SYMBOL_LABEL (4)
#define SYMBOL_EXTERN (5)
#define SYMBOL_CONSTANT (6)

class Line;
class Section;
class Symbol;
class Relocation;

class TS_entry{
private:
    std::string name;
    Section* section;
    int value;
    unsigned short type;
    unsigned int ID;
    unsigned short scope_flag;
    static std::unordered_map<std::string, TS_entry*> TS_entry_mapping;
    static const std::set<std::string> system_defined_words;

public:
    TS_entry(std::string name);
    virtual ~TS_entry(){}

    void setID(unsigned int ID);
    unsigned int getID();

    void setScope(unsigned short scope);
    unsigned short getScope();

    void setType(unsigned short type);
    unsigned short getType();

    std::string getName();

    Section* getSection();

    void setValue(int value);
    int getValue();

    bool is_in_static_section();
    bool is_label_or_extern();
    bool is_constant();

    static bool is_key_word(const std::string &str);
    static void print_TS();
    static void print_machine_code();
    static void init();
    static void clear();

    virtual std::string to_string(){
        return "TS_entry";
    }

protected:
    TS_entry* next;
    TS_entry* prev;

    friend class Symbol;
    friend class Section;
    friend class Line;
    friend class ObjFile;
    friend class Loader;
    friend class Relocation;
    friend class Parser;
    friend int calc_postfix(std::string input, TS_entry*& rsymbol);
    friend bool is_constant(const std::string&);
    friend bool is_label_or_extern(const std::string&);
    friend bool exists_symbol(const std::string&);
    friend bool are_from_same_section_labels(const std::string&, 
                                             const std::string&);
};

class Section: public TS_entry{
private:
    size_t size;
    static TS_entry* head;
    static TS_entry* tail;
    bool is_static; // is section defined with ORG?

    char* machine_code;

    Line* line_head;
    Line* line_tail;

    Relocation* reloc_head;
    Relocation* reloc_tail;

public:
    static Section* current;
    unsigned short flags;

    Section(std::string name);
    virtual ~Section() override;

    void setSize(size_t size);
    size_t getSize();

    bool isStatic();

    static void add_section(std::string &str, std::string type);

    virtual std::string to_string() override;

    static int num_of_sections;

    friend class TS_entry;
    friend class Symbol;
    friend class Line;
    friend class Parser;
    friend class Relocation;
    friend class Loader;
    friend class ObjFile;
};

class Symbol: public TS_entry{
private:
    static TS_entry* head;
    static TS_entry* tail;

public:
    Symbol(std::string name, int value, Section* section);
    virtual ~Symbol() override= default;

    static void add_symbol_as_global(std::string name, int value);
    static bool add_symbol_as_defined(std::string name, int value, unsigned short type);

    virtual std::string to_string() override;

    friend class TS_entry;
    friend class ObjFile;
};

#endif