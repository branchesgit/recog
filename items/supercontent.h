#ifndef SUPERCONTENT_H
#define SUPERCONTENT_H

#include <opencv2/core.hpp>
#include <json/json.h>
#include "settingcontent.h"

using namespace cv;
using namespace std;

class SuperContent
{
public:
    Rect rect;
    string id;
    Json::Value structureNumber;
    int type;
    ContentOrientation orientation;

    bool isSaveImage;
    string saveFileUrl;
    bool isCorrectInput;
    Json::Value result;

    SettingContent* settingContent;
    virtual void Do(Mat& img);
};

#endif // SUPERCONTENT_H
