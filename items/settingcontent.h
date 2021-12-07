#ifndef SETTINGCONTENT_H
#define SETTINGCONTENT_H

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

enum PaperType
{
    Paper_A3,
    Paper_A4
};


enum SaveImageFormat
{
    Format_JPEG,
    Format_PNG
};

enum ContentOrientation
{
    Horizontal,
    Vertical
};

typedef struct _FeatureDescribe
{
    int featureType;
    string featureDescribeFile;
    string templateFile;
    float seachAreaPadding;
    Rect rect;
    float matchRatio;
}FeatureDescribe;


typedef struct _StitchingObject
{
    vector<string> inputs;
    string output;
    ContentOrientation orientation;
}StitchingObject;



class SettingContent
{
public:
    SettingContent();

    PaperType paperType;
    int dpi;
    int paperWidth;
    int paperHeight;
    float choiceRectWidth;
    float choiceRectHeight;

    Rect topleftMark;
    Rect topRightMark;
    Rect bottomLeftMark;
    Rect bottomRightMark;
    Rect topMiddleMark;
    Rect bottomMiddleMark;
    vector<Rect> topRects;
    vector<Rect> leftRects;
    vector<Rect> bottomRects;


    float selectedThreshold;
    Size choiceSize;
    bool isMorphology;
    string barcodeType;
    string imageSavePath;


    bool hasFeatureDes;
    FeatureDescribe featureDes;


    bool hasFrontCorrectionParam;
    int frontRotation;
    Mat frontWarpMat;
    bool isBackRotated180;
    float backAngleDeviation;
    bool isFrontParamForSaveImage;


    bool isSaveStandardImage;
    string standardImageFilename;
    SaveImageFormat saveImageFormat;
    float saveImageScale;
    bool isThreshBinary;
    float threshBinaryValue;
    vector<int> saveParams;


    bool supportLowQuality;

    int expandSearchMarkLegth;

    vector<StitchingObject> imagesStitchings;
    bool isOnlyStitchingImages;
    bool enablePredictMark;
    string dbrLicense; //DBR
    bool deepfindlocation;
    float topAndBottomMaxSlope;
    Rect pageMarkRect;
    int pageMarkItemNumber;
    bool hasPageMark;
    int pageNumber;

    bool isObjectiveDraw;
    int roiX;
    int roiY;
    int roiWidth;
    int roiHeight;
    int roiRotation;
    float roiMicroRotate;
    string roiDirection;
    int itemNumber;
    int questionCount;
};

#endif // SETTINGCONTENT_H
