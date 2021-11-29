#ifndef FILE_H
#define FILE_H

#include <string>
#include <json/json.h>

using namespace std;

class File
{
public:
    File();

    std::string readFile(std::string filePath);

    Json::Value parse(string str);
};

#endif // FILE_H
