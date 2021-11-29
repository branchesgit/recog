#include "cutting.h"
#include "File.h"

Cutting::Cutting()
{

}


void Cutting::saveContoures2Local(Mat mat, string filePath, vector<vector<Point>> contoures)
{
    int lastSlash = filePath.find_last_of("/");
    string parentPath = filePath.substr(0, lastSlash + 1);
    string fileName = filePath.substr( lastSlash + 1, filePath.length());
    string name = fileName.substr(0, fileName.find_last_of("."));

    // create file folder...
    File file;
    string folderPath = parentPath + name;
    file.createDirectory(folderPath);

    string rectName = "rect_";

    Mat tmpImg;
    for(int i = 0; i < contoures.size(); i++)
    {
        Rect rect = boundingRect(contoures[i]);
        if(rect.area() > 1000)
        {
            mat(rect).copyTo(tmpImg);
            imwrite(folderPath + "/" + rectName + std::to_string(i) + ".png",  tmpImg);
        }

    }


}
