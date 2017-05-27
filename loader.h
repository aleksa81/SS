#ifndef LOADER_
#define LOADER_

#include <unordered_map>
#include <string>

class Loader{
private:
	Loader(){}

	static std::unordered_map<std::string, char> opcode;
	static std::unordered_map<std::string, char> regcode;

public:

	static Loader& getInstance(){

        // initialized only once (on first encounter)
        // deleted when program terminates
        static Loader instance;

        return instance;
    }

    Loader(Loader const&) = delete;
    void operator=(Loader const&) = delete;
    // Note: Scott Meyers mentions in his Effective Modern
    //       C++ book, that deleted functions should generally
    //       be public as it results in better error messages
    //       due to the compilers behavior to check accessibility
    //       before deleted status.

    void load();


};






#endif