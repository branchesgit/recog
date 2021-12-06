#include "choice.h"
#include "boundary.h"
#include <opencv2/core.hpp>
#include "cutting.h"

using namespace std;
using namespace cv;

Choice::Choice()
{

}


bool sortContour(vector<Point> a, vector<Point> b)
{
    Rect arect = boundingRect(a);
    Rect brect = boundingRect(b);
    return arect.area() > brect.area();
}

/**
 * @brief Choice::handleRecognition
 * @param choiceFilePath
 * find each choice boundary..
 */
void Choice::handleRecognition(string choiceFilePath)
{
//    choiceFilePath = "/home/branches/branches/answercards/files/DN1109000001/rect_770.png";
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
        );

    Mat mat = imread(choiceFilePath);
    Mat grayImg, thresholdImg;
    cvtColor(mat, grayImg, cv::COLOR_BGR2GRAY);
    threshold(grayImg, thresholdImg, 80, 255, THRESH_BINARY);
    thresholdImg = ~thresholdImg;
    mat = thresholdImg.clone();

    Boundary boundary;
    vector<vector<Point>> contours = boundary.handleBoundary(mat);
    sort(contours.begin(), contours.end(), sortContour);

    double degree = 0;
    int i = 0;

    while(i < 2) {
        RotatedRect minrect = minAreaRect(contours[1]);//minAreaRect():以X轴正方形为起点，顺时针为正，逆时针为负
        degree = minrect.angle;

        //此处目的是为了让目标图像旋转到水平位置
        if (0< abs(degree) && abs(degree) <= 45)
            degree = degree;
        else if (45< abs(degree) && abs(degree)<90)
            degree = 90 - abs(degree);
        i++;
    }

    Mat dstImg = mat.clone();
    this->fillBoundary(dstImg, contours, 1200, 80);
    cv::Mat element = cv::getStructuringElement(0, cv::Size(4, 4));
    erode(dstImg, dstImg, element);


    contours = boundary.handleBoundary(dstImg);
    this->handleChoiceItems(dstImg, contours, 20, 25);
    this->handleChoiceItemBoundary(dstImg, 20, 36);

    string path = "/home/branches/mat.png";
    imwrite(path, dstImg);
    contours = boundary.handleBoundary(dstImg);
    Cutting cutting;
    cutting.saveContoures2Local(mat, path, contours, 1200);
    std::chrono::milliseconds ms1 = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
        );

    cout << "time spend is " << ms1.count() - ms.count() << endl;

}

vector<vector<Point>> Choice::findNearContours(vector<Point> contour, vector<vector<Point>> contours,int offsetY, int offsetX)
{
    Rect rect = boundingRect(contour);
    int x = rect.tl().x;
    int y = rect.tl().y;


    // direx-x-left-slide
    vector<Point> leftContour;
    vector<Point> rightContour;

    for(size_t i = 0; i < contours.size(); i++)
    {
        Rect rec = boundingRect(contours[i]);
        int recx = rec.x;
        int recy = rec.y;

        // skip self.
        if (x == recx && y == recy){
            continue;
        } else {
            if(abs(y - recy) < offsetY){
                if(leftContour.size() == 0){
                    if (recx < x && abs(x - recx) < offsetX){
                       leftContour = contours[i];
                    }
                } else {
                    Rect leftRec = boundingRect(leftContour);
                    if (recx < x) {
                        if (leftRec.tl().x < recx && abs(leftRec.tl().x - recx) < offsetX){
                            leftContour = contours[i];
                        }
                    }

                }


                if (rightContour.size() == 0){
                    if (recx > x && abs(x - recx) < offsetX){
                        rightContour = contours[i];
                    }
                } else {
                    Rect rightRec = boundingRect(rightContour);
                    if(recx > x) {
                        if (recx < rightRec.tl().x && abs(rightRec.tl().x - recx) < offsetX){
                            rightContour = contours[i];
                        }
                    }

                }
            }
        }
    }

    vector<vector<Point>> nearContours;
    nearContours.push_back(leftContour);
    nearContours.push_back(rightContour);
    return nearContours;
}

//  direct_x  找到对应的选项的区域 handle group...
void Choice::handleChoiceItems(Mat mat, vector<vector<Point>> contours, int offsetY, int offsetX)
{

      // find A, B, C, D... center, found near contour.
    int count = 0;
    for(size_t i = 0; i < contours.size(); i++){
      Rect rec = boundingRect(contours[i]);
      // most probile is A, B, C, D block.
      if (rec.height / rec.width < 2){
          vector<vector<Point>> nearContours = this->findNearContours(contours[i], contours, offsetY, offsetX);
          vector<Point> leftContour = nearContours[0];
          vector<Point> rightContour = nearContours[1];

          if(leftContour.size() == 0 || rightContour.size() == 0){
              continue; // do nothing not A, B, C, D...
          }

          Rect leftRec = boundingRect(leftContour);
          Rect rightRec = boundingRect(rightContour);
          count++;
          //
          vector<Point>  contour;
          contour.push_back(leftRec.tl());
          contour.push_back(Point(rightRec.tl().x + rightRec.width , rightRec.tl().y ) );
          contour.push_back(Point(rightRec.tl().x + rightRec.width, rightRec.tl().y + rightRec.height));
          contour.push_back(Point(leftRec.tl().x , leftRec.tl().y + leftRec.height ));
          fillConvexPoly(mat, contour, cv::Scalar(255,255,255));

      }
    }


}



// fill boundary...
void Choice::fillBoundary(Mat dstImg, vector<vector<Point>> contours, int max, int min){

    for(int i = 0; i < contours.size(); i++){
        Rect rec = boundingRect(contours[i]);

        if(rec.area() > min && rec.area() < max){
            vector<Point>  contour;
            contour.push_back(rec.tl());
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y ) );
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y + rec.height));
            contour.push_back(Point(rec.tl().x , rec.tl().y + rec.height ));

          fillConvexPoly(dstImg, contour, cv::Scalar(255,255,255));
        }

    }
}

bool sortX(vector<Point> a, vector<Point> b) {
    return boundingRect(a).x < boundingRect(b).x;
}


void Choice::handleChoiceItemBoundary(Mat mat, int offsetY, int offsetX) {

    Boundary boundary;
    vector<vector<Point>> contours = boundary.handleBoundary(mat);
    vector<vector<Point>> numbers; // may 11 , 12 ...

    for(size_t i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours[i]);
        double scale = rect.height / rect.width;

        if (scale >= 1.0) {
            if (rect.tl().x > 10 && rect.tl().y > 10){
//                rectangle(mat, rect, (0, 100, 100), 3);
                numbers.push_back(contours[i]);
            }
        }
    }


    vector<vector<vector<Point>>> numberGroups;
    for(int i = 0; i < numbers.size(); i++) {


        vector<vector<Point>> groups;
        vector<Point> contour = numbers[i];

        groups.push_back(contour);
        // find near point.
        Rect rect = boundingRect(contour);
        for(int j = i + 1; j < numbers.size(); j++) {
            Rect rec = boundingRect(numbers[j]);
            if(abs(rec.x - rect.x) < 20 && abs(rec.y - rect.y) < 10) {
                groups.push_back(numbers[j]);
                numbers.erase(numbers.begin() + j);
            }
        }

        numberGroups.push_back(groups);

    }

    //
    sort(contours.begin(), contours.end(), sortX);
    for(size_t i = 0; i < numberGroups.size(); i++) {
        vector<vector<Point>> cs = numberGroups[i];
        vector<Point> right;

        if(cs.size() > 1) {
            cout << "size is " << cs.size() << endl;
            Rect leftRec = boundingRect(cs[0]);
            Rect rightRec = boundingRect(cs[1]);
            vector<Point>  contour;
            contour.push_back(leftRec.tl());
            contour.push_back(Point(rightRec.tl().x + rightRec.width , rightRec.tl().y ) );
            contour.push_back(Point(rightRec.tl().x + rightRec.width, rightRec.tl().y + rightRec.height));
            contour.push_back(Point(leftRec.tl().x , leftRec.tl().y + leftRec.height ));
            fillConvexPoly(mat, contour, cv::Scalar(255,255,255));

            if (leftRec.tl().x > rightRec.tl().x) {
                right = cs[0];
            } else {
                right = cs[1];
            }
        } else {
            right = cs[0];
        }

        Rect r = boundingRect(right);


        // dy is less offsetY, dx less offsetX.
        for(size_t j = 0; j < contours.size(); j++) {
            Rect rect = boundingRect(contours[j]);

            if (rect.tl().x > 10 && rect.tl().y > 10) {
                if (abs(r.tl().y - rect.tl().y) < offsetY) {
                    if (abs(r.tl().x + r.width - rect.tl().x) < offsetX) {
                        Rect leftRec = r;
                        Rect rightRec = boundingRect(contours[j]);
                        vector<Point>  contour;
                        contour.push_back(leftRec.tl());
                        contour.push_back(Point(rightRec.tl().x + rightRec.width , rightRec.tl().y ) );
                        contour.push_back(Point(rightRec.tl().x + rightRec.width, rightRec.tl().y + rightRec.height));
                        contour.push_back(Point(leftRec.tl().x , leftRec.tl().y + leftRec.height ));
                        fillConvexPoly(mat, contour, cv::Scalar(255,255,255));

                        cs.push_back(contours[j]);
                        r = boundingRect(contours[j]);
                    }
                }
            }
        }

    }
}









