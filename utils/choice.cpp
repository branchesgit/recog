#include "choice.h"
#include "boundary.h"

using namespace std;


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

    cout << "degree = " << degree << endl;

    Point center = Point(mat.cols / 2, mat.rows / 2);
    double angle = degree;
    double scale = 1;
    Mat rot(2, 3, CV_32FC1);
    rot = getRotationMatrix2D(center, angle, scale);//getRotationMatrix2D():以X轴正方形为起点，顺时针为负，逆时针为正
    Mat rotimage;
    warpAffine(mat, rotimage, rot, mat.size());
    imshow("rotimage", rotimage);
    waitKey(0);


}
