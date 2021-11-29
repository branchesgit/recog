#include "File.h"

#include <iostream>
#include <fstream>


File::File()

{

}

std::string File::readFile(string filePath)
{
    std::string str;
    std::ifstream myfile;
    myfile.open(filePath);
    std::string myline;

    if (myfile.is_open()) {
        while ( myfile ) {
            std::getline (myfile, myline);
            str += myline;
        }
        myfile.close();
    }
    return str;
}

Json::Value File::parse(string str) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(str, root);
    return root;
}
