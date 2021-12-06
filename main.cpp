#include <QCoreApplication>
#include "utils/File.h"
#include <iostream>
#include <json/json.h>
#include <opencv4/opencv2/opencv.hpp>
#include "utils/boundary.h"
#include "utils/cutting.h"
#include "utils/choice.h"
#include "utils/choiceitem.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    File file;
//    std::string rootDir = "/home/branches/branches/answercards/";
//    std::string filePath = rootDir + "tp.json";
//    std::string str = file.readFile(filePath);
//    // tempalte information.
//    Json::Value root = file.parse(str);
//    string templateFilePath = root["setting"]["featureRecognition"]["templateFile"].asString();

//    templateFilePath = rootDir + "files/DN1109000001.TIF";
//    cv::Mat mat = cv::imread(templateFilePath);

//    Boundary boundary;
//    vector<vector<Point>> contours = boundary.handleBoundary(mat);

//    cout << "Size is " << contours.size();
//    Cutting cutting;
//    cutting.saveContoures2Local(mat, templateFilePath, contours);

//    string choiceFilePath = "/home/branches/branches/answercards/files/DN1109000001/rect_770.png";
//    Choice choice;
//    choice.handleRecognition(choiceFilePath);
    string filePath = "/home/branches/mat/rect_19.png";
    ChoiceItem item;
    Mat mat = imread(filePath);

    item.cuttingChoiceItem(mat);
    return 0;
}
