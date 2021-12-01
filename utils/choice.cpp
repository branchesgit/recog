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
    Mat mat = imread(choiceFilePath);
    Mat grayImg, thresholdImg;
    cvtColor(mat, grayImg, cv::COLOR_BGR2GRAY);
    threshold(grayImg, thresholdImg, 80, 255, THRESH_BINARY);
    thresholdImg = ~thresholdImg;

//     imshow("thresholdImg", thresholdImg);
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
//    vector<Vec4f> lines;
//    cv::Canny(mat, dstImg, 200, 500);
//    cv::HoughLinesP(dstImg, lines, 1, CV_PI / 180.0,  0);
//    Scalar white = Scalar(255, 255, 255);
//    cout << "lines size is = " << lines.size() << endl;
//    for(size_t i = 0; i < lines.size(); i++)
//    {
//        Vec4f l = lines[i];
//        cv::line(dstImg, Point(l[0], l[1]), Point(l[2], l[3]), white, 1, 8);
//    }
//    dstImg = ~dstImg;
    this->fillBoundary(dstImg, contours, 1200, 80);
//    cvtColor(dstImg, dstImg, cv::COLOR_BGR2GRAY);
//    GaussianBlur(dstImg, dstImg, Size(3,3), 0, 0);
//    GaussianBlur(dstImg, dstImg, Size(3,3), 0, 0);
    cv::Mat element = cv::getStructuringElement(0, cv::Size(4, 4));
    erode(dstImg, dstImg, element);
//    erode(dstImg, dstImg, element);

    contours = boundary.handleBoundary(dstImg);
    this->handleChoiceItems(dstImg, contours, 20, 30);
//    morphologyEx(dstImg, dstImg, cv::MORPH_CLOSE, element);

//    Cutting cutting;
//    cutting.saveContoures2Local(dstImg, choiceFilePath, contours, 100);

    imwrite("/home/branches/mat.png", dstImg);
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

//    vector<Rect> results;
//    cout << "contours size is " << contours.size() << endl;
//    // find y value;
//    vector<int> valueYs;
//    for(size_t i = 0; i < contours.size(); i++)
//    {
//        Rect rect = boundingRect(contours[i]);
//        if (i == 0)
//        {
//            valueYs.push_back(rect.tl().y);
//        }
//        else
//        {
//            int y = rect.tl().y;
//            int count = 0;
//            for(int j = 0; j < valueYs.size(); j++)
//            {
//                if (std::abs(y - valueYs[j]) < offset)
//                {
//                    count++;
//                    break;
//                }
//            }
//            if (count == 0)
//            {
//               valueYs.push_back(y);
//            }
//        }
//    }


//    for(int i = 0; i < valueYs.size(); i++ )
//    {
//        int y = valueYs[i];


//    }

//    return results;
}



// fill boundary...
void Choice::fillBoundary(Mat dstImg, vector<vector<Point>> contours, int max, int min)
{

    for(int i = 0; i < contours.size(); i++)
    {
        Rect rec = boundingRect(contours[i]);

        if(rec.area() > min && rec.area() < max)
        {
            vector<Point>  contour;
            contour.push_back(rec.tl());
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y ) );
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y + rec.height));
            contour.push_back(Point(rec.tl().x , rec.tl().y + rec.height ));

          fillConvexPoly(dstImg, contour, cv::Scalar(255,255,255));
//        fillPoly(dstImg, contour, Scalar(255,255,255));
//        rectangle(dstImg, rec, Scalar(255,255,255), -1, 4);
        }

    }

    //    cout << "degree = " << degree << endl;
    //    for(int i = 0; i < contours.size(); i++)
    //    {
    //        RotatedRect minRect = minAreaRect(contours[i]);
    //        Rect rect = boundingRect(contours[i]);
    //        if(rect.area() > 100 && rect.area() < 2000)
    //        {
    //          rectangle(dstImg, rect, (0, 0, 0), 1);
    //        }
    //    }


    //    Mat imageGray = dstImg.clone(), imageGuussian;

    //    morphologyEx(imageGray, imageGray, cv::MORPH_CLOSE, element);
    //    morphologyEx(imageGray, imageGray, cv::MORPH_CLOSE, element);
    //    erode(imageGray, imageGray, element);
    //    erode(imageGray, imageGray, element);
    //    erode(imageGray, imageGray, element);
    //    blur(imageGray, imageGuussian, cv::Size(3, 3));

    //    //5.均值滤波，消除高频噪声

    //    cv::Mat imageSobleOutThreshold;
    //    threshold(imageGuussian, imageSobleOutThreshold, 180, 255, ThresholdTypes::THRESH_BINARY);

    //    imshow("imageSobleOutThreshold", imageSobleOutThreshold);


    //    Point center = Point(mat.cols / 2, mat.rows / 2);
    //    double angle = degree;
    //    double scale = 1;
    //    Mat rot(2, 3, CV_32FC1);
    //    rot = getRotationMatrix2D(center, angle, scale);//getRotationMatrix2D():以X轴正方形为起点，顺时针为负，逆时针为正
    //    Mat rotimage;
    //    warpAffine(mat, rotimage, rot, mat.size());
    //    imshow("rotimage", rotimage);
    //    waitKey(0);
}
