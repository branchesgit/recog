#include "boundary.h"


using namespace std;

Boundary::Boundary()
{

}


vector<vector<Point>>  Boundary::handleBoundary(Mat mat)
{
    Mat grayImg, dstImg, thresholdImg;
    cvtColor(mat, grayImg, cv::COLOR_BGR2GRAY);

    threshold(grayImg, thresholdImg, 80, 255, THRESH_BINARY);
    thresholdImg = ~thresholdImg;


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(thresholdImg, contours, hierarchy,
                 RetrievalModes::RETR_LIST, ContourApproximationModes::CHAIN_APPROX_NONE, Point());

    return contours;
//    cout << "size is " << contours.size();
//    dstImg = thresholdImg.clone();

//    drawContours(dstImg, contours, -1, (0,100, 100), 2);
//    Scalar odd = Scalar(128, 128, 100);
//    Scalar even = Scalar(255, 100, 100);
//    int count  = 0;

//    for(int i = 0; i < contours.size(); i++)
//    {
//        if(contourArea(contours[i]) > 10 * 10000) {
//            cout << "area is :" << contourArea(contours[i]) << std::endl;
//            Rect rect = boundingRect(contours.at(i));
//            rectangle(dstImg, rect, count % 2 == 0 ? odd : even, 3,
//                      count % 2 == 0 ? LINE_AA :  LINE_8);
//            count++;
//        }
//    }

//    imshow("dstImg", dstImg);
//    waitKey(0);
}
