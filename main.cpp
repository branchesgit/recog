#include <QCoreApplication>
#include "utils/File.h"
#include <iostream>
#include <json/json.h>
#include <opencv4/opencv2/opencv.hpp>
#include "utils/boundary.h"
#include "utils/cutting.h"
#include "utils/choice.h"
#include "utils/choiceitem.h"
#include "utils/linefinder.h"
#include "utils/common.h"

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
//    string filePath = "/home/branches/mat/rect_19.png";
//    ChoiceItem item;
//    Mat mat = imread(filePath);

//    item.cuttingChoiceItem(mat);

    string filePath = "/home/branches/qt/recog/2.TIF";
    Mat mat = imread(filePath);
    cvtColor(mat, mat,ColorConversionCodes::COLOR_BGR2GRAY);
    Mat element = cv::getStructuringElement(MorphShapes::MORPH_RECT, Size(4, 4));
    threshold(mat, mat, 120, 255, 0);

     //先腐蚀再膨胀，用来消除小物体
//    morphologyEx(mat, mat, MORPH_OPEN, element);
//    morphologyEx(mat, mat, MORPH_OPEN, element);
//    morphologyEx(mat, mat, MORPH_OPEN, element);

//    morphologyEx(mat, mat, MORPH_TOPHAT, element);
//    morphologyEx(mat, mat, MORPH_CLOSE, element);
//    morphologyEx(mat, mat, MORPH_CLOSE, element);
//    morphologyEx(mat, mat, MORPH_CLOSE, element);
//    morphologyEx(mat, mat, MORPH_CLOSE, element);
    threshold(mat, mat, 120, 255, 0);
    mat = ~mat;

    Mat cimg;
    mat(Rect(844, 263, 322, 292)).copyTo(cimg);

    Boundary boundary;
    int count = 0;
    vector<vector<Point>> rects;
    vector<vector<Point>> contours = boundary.handleBoundary(cimg);
    for(size_t i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours[i]);

        if (rect.height > 292 * 0.4 || (rect.width > 25  && rect.width < 35)) {
            rectangle(cimg, rect, (0, 100, 100), 2);
            rects.push_back(contours[i]);
        }
//        if(rect.area() > 500 && rect.area() < 800) {
//            cv::rectangle(cimg, rect, cv::Scalar(0, 255, 255), 1);
//            cout << std::to_string(count)  << "=" << rect.area() << endl;
//            count++;
//            rects.push_back(contours[i]);
//        }
    }

    Cutting cutting;
    cutting.saveContoures2Local(cimg, "/home/branches/qt/recog/2.TIF", rects, 0);

    // need to judge rect is around.
    // 10 rects to group.

//    sort(rects.begin(), rects.end(), Common::sortX);
//    cout << "rects size is " << rects.size() << endl;

//    for(int i = 0; i < rects.size(); i++) {
//        cout << "X is " << boundingRect(rects[i]).x << endl;
//    }

//    vector<vector<vector<Point>>> groups{};
//    Rect rect;
//    Rect rec;
//    vector<vector<Point>> group{};
//    for(int i = 0; i < rects.size(); i++) {
//        rect = boundingRect(rects[i]);
//        int idx = -1;

//        if (groups.size() > 0) {
//            group = groups.at(groups.size() - 1);
//        }

//        for(int j = 0; j < group.size(); j++) {
//            rec = boundingRect(group.at(j));
//            int dis = abs(rect.x - rec.x);

//            if (dis < 30) {
//                idx = j;
//                break;
//            }
//        }

//        cout << "group size is " << group.size() << ", idx = " << idx << endl;
//        if (idx == -1) {
//            vector<vector<Point>> grp{};
//            grp.emplace_back(rects[i]);
//            groups.push_back(grp);
//        } else {
//            group.emplace_back(rects[i]);
//            groups[groups.size() - 1] = group;
//        }
//    }

//    for(int i = 0; i < groups.size(); i++) {
//        vector<vector<Point>> group = groups[i];

//        if (group.size() > 7) {
//            sort(group.begin(), group.end(), Common::sortY);

//            for(int j = 0; j < group.size() - 1; j++) {
//                Rect top = boundingRect(group[j]);
//                Rect bottom = boundingRect(group[j + 1]);

//                vector<Point>  contour;
//                contour.push_back(top.tl());
//                contour.push_back(Point(top.tl().x + top.width , top.tl().y ) );
//                contour.push_back(Point(bottom.tl().x + bottom.width , bottom.tl().y + rec.height));
//                contour.push_back(Point(bottom.tl().x , bottom.tl().y + bottom.height ));

//                fillConvexPoly(cimg, contour, cv::Scalar(255,255,255));
//            }
//        }
//    }


    cv::namedWindow("camera", 0);
    cv::imshow("camera", cimg);
    waitKey(0);

    return 0;

//    cv::Mat image = cv::imread(filePath);
//    cv::Mat imageGray;
//    cv::Mat contours;
//    cv::cvtColor(image, imageGray, cv::COLOR_RGB2GRAY);
//    cv::Canny(imageGray, contours, 50, 150);
//    // 在原图的拷贝上画直线
//    cv::Mat result(contours.rows, contours.cols, CV_8U, cv::Scalar(255));
//    image.copyTo(result);
//    // Hough变换检测
//    LineFinder finder;
//    finder.setMinVote(156);
//    finder.findLines(contours);
//    finder.drawDetectedLines(result);

//    // 显示
//    cv::namedWindow("Detected Lines with Hough");
//    cv::imshow("Detected Lines with Hough", result);
//    cv::waitKey(0);
//    return 0;
}



