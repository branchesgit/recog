#include "File.h"

#include <iostream>
#include <fstream>


#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>


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

// create direcotry.
bool File::createDirectory(string filePath)
{
    string command = "mkdir -p " + filePath;
    int status = std::system(command.c_str());
    return status == -1 ? false : true;
}
