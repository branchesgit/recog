#include "choiceitem.h"
#include "boundary.h"
#include "cutting.h"

ChoiceItem::ChoiceItem()
{

}

bool sortXX(vector<Point> a, vector<Point> b) {
    return boundingRect(a).x < boundingRect(b).x;
}


// cutting each item and number.
vector<Mat> ChoiceItem::cuttingChoiceItem(Mat mat) {
    Mat copyMat = mat.clone();
    cvtColor(copyMat, copyMat, cv::COLOR_BGR2GRAY);

    // er zhi hua picture.
    threshold(copyMat, copyMat, 128, 255, ThresholdTypes::THRESH_BINARY);

    Boundary boundary;
    vector<vector<Point>> contours = boundary.handleBoundary(copyMat);

    for(size_t i = 0; i < contours.size(); i++) {
        Rect rec = boundingRect(contours[i]);

        if (rec.area() > 20) {
//            cout << "are is " << rect.area() << endl;
            vector<Point>  contour;
            contour.push_back(rec.tl());
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y ) );
            contour.push_back(Point(rec.tl().x + rec.width , rec.tl().y + rec.height));
            contour.push_back(Point(rec.tl().x , rec.tl().y + rec.height ));

            fillConvexPoly(copyMat, contour, cv::Scalar(255,255,255));
        }
    }

    imwrite("/home/branches/mat/s.png", copyMat);

    Mat dstImage;
    Mat element = getStructuringElement(MORPH_RECT, Size(1, 1));
    dilate(copyMat, dstImage, element);

    contours = boundary.handleBoundary(dstImage);
    sort(contours.begin(), contours.end(), sortXX);
    // record last index value.
    vector<int> lastIndexes;

    for(size_t i = 0; i < contours.size(); i++) {
        int lastIndex = -1;
        if (lastIndexes.size() > 0) {
            lastIndex = lastIndexes[lastIndexes.size() - 1];
        }

        Rect rect = boundingRect(contours[i]);
        if (i == 0) {
            lastIndexes.push_back(i);
        } else {
            Rect lastRec = boundingRect(contours[lastIndex]);
            int dis = rect.tl().x  - lastRec.tl().x - lastRec.width;
            if (dis < 12) {
                lastIndexes[lastIndexes.size() - 1] = i;
            }else {
                lastIndexes.push_back(i);
            }
        }
    }

    int start = -1;
    for(int i = 0; i < lastIndexes.size(); i++) {

        int end = lastIndexes[i];

        //
        if (end - start > 1) {
            start = start + 1;
            Rect leftRec = boundingRect(contours[start]);
            Rect rightRec = boundingRect(contours[end]);

            vector<Point>  contour;
            contour.push_back(leftRec.tl());
            contour.push_back(Point(rightRec.tl().x + rightRec.width , rightRec.tl().y ) );
            contour.push_back(Point(rightRec.tl().x + rightRec.width, rightRec.tl().y + rightRec.height));
            contour.push_back(Point(leftRec.tl().x , leftRec.tl().y + leftRec.height ));
            fillConvexPoly(dstImage, contour, cv::Scalar(255,255,255));
        }

        // s
        start = end;
    }

    erode(dstImage, dstImage, element);
    contours = boundary.handleBoundary(dstImage);

    string path = "/home/branches/mat/ss.png";
    imwrite(path, dstImage);
    Cutting cutting;
    cutting.saveContoures2Local(mat, path, contours, 60);

}
