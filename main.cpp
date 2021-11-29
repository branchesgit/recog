#include <QCoreApplication>
#include "utils/File.h"
#include <iostream>
#include <json/json.h>
#include <opencv4/opencv2/opencv.hpp>
#include "utils/boundary.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
    File file;
    std::string rootDir = "/home/branches/branches/answercards/";
    std::string filePath = rootDir + "tp.json";
    std::string str = file.readFile(filePath);
    // tempalte information.
    Json::Value root = file.parse(str);
    string templateFilePath = root["setting"]["featureRecognition"]["templateFile"].asString();

    cv::Mat mat = cv::imread(templateFilePath);
    Boundary boundary;
    boundary.handleBoundary(mat);

    return 0;
}
