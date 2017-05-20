#ifndef TS_ENTRY_
#define TS_ENTRY_

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
#define SYMBOL_CONSTANT (5)
#define SYMBOL_EXTERN (6)

class TS_entry{
private:
    std::string name;
    unsigned short type; // for sections : data, text... for symbols: constant, label, extern
    unsigned int ID;
    unsigned short scope_flag;
    static std::unordered_map<std::string, TS_entry*> TS_entry_mapping;
    static const std::set<std::string> system_defined_words;

public:
    TS_entry(std::string name);

    void setID(unsigned int ID);
    unsigned int getID();

    void setScope(unsigned short scope);
    unsigned short getScope();

    void setType(unsigned short type);
    unsigned short getType();

    std::string getName();

    static bool is_key_word(const std::string &str);
    static void print();
    static void init();

    virtual std::string to_string(){
        return "TS_entry";
    }

protected:
    TS_entry* next;
    TS_entry* prev;

    friend class Symbol;
    friend class Section;
    friend class Line;
};

class Section: public TS_entry{
private:
    size_t size;
    size_t start;
    static TS_entry* head;
    static TS_entry* tail;
    static int num_of_sections;

public:
    static Section* current;
    unsigned short flags;

    Section(std::string name);

    void setSize(size_t size);
    size_t getSize();

    void setStart(size_t start);
    size_t getStart();

    static void add_section(std::string name, int location_cntr, std::string type);

    virtual std::string to_string() override;

    friend class TS_entry;
};

class Symbol: public TS_entry{
private:
    int value;
    Section* section;
    static TS_entry* head;
    static TS_entry* tail;

public:
    Symbol(std::string name, int value, Section* section);

    Section* getSection();
    int getValue();

    static void add_symbol_as_global(std::string name, int value);
    static bool add_symbol_as_defined(std::string name, int value, unsigned short type);

    virtual std::string to_string() override;

    friend class TS_entry;
};

#endif