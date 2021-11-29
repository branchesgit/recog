#include "boundary.h"


using namespace std;

Boundary::Boundary()
{

}


void Boundary::handleBoundary(Mat mat)
{
    Mat grayImg, dstImg, thresholdImg;
    cvtColor(mat, grayImg, cv::COLOR_BGR2GRAY);

    threshold(grayImg, thresholdImg, 127, 255, THRESH_BINARY);
    thresholdImg = ~thresholdImg;


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(thresholdImg, contours, hierarchy,
                 RetrievalModes::RETR_LIST, ContourApproximationModes::CHAIN_APPROX_NONE, Point());
    cout << "size is " << contours.size();
    dstImg = thresholdImg.clone();

//    drawContours(dstImg, contours, -1, (0,100, 100), 2);

    for(int i = 0; i < contours.size(); i++)
    {

        cout << "area is :" << contourArea(contours[i]) << std::endl;
        if(contourArea(contours[i]) > 600) {
            Rect rect = boundingRect(contours.at(i));
            rectangle(dstImg, rect, (0, 100, 100), 2);
        }
    }

    imshow("dstImg", dstImg);
    waitKey(0);

}
