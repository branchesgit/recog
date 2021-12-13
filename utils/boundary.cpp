#include "boundary.h"


using namespace std;

Boundary::Boundary()
{

}


vector<vector<Point>>  Boundary::handleBoundary(Mat mat)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(mat, contours, hierarchy,
                 RetrievalModes::RETR_LIST, ContourApproximationModes::CHAIN_APPROX_NONE, Point());


    return contours;
}


