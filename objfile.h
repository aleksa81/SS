#ifndef OBJFILE_
#define OBJFILE_

#include <iostream>
#include <fstream>
#include <string>

class ObjFile{
private:
    std::ofstream output; 

    ObjFile(){}
public:
     static ObjFile& getInstance(){

        // initialized only once (on first encounter)
        // deleted when program terminates
        static ObjFile instance;

        return instance;
    }

    ObjFile(ObjFile const&) = delete;
    void operator=(ObjFile const&) = delete;

    void make(std::string file_name);

};

#endif