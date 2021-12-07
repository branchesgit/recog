#include "parseintputjson.h"
#include "../items/serialnumber.h"
#include "../items/number.h"
#include "../items/radio.h"
#include "../items/checkbox.h"
#include "../items/trueorfalse.h"
#include "../items/subjectiveitem.h"
#include "../items/absentflag.h"
#include "../items/itemsblock.h"
#include "../items/radiosimple.h"
#include "../items/cardscore.h"
#include "../items/pixelspercentscore.h"
#include "../items/handwritingstudentno.h"
#include "../items/handwritingcardscore.h"


ParseIntputJson::ParseIntputJson(){

}

bool ParseIntputJson::parse(
        string inputStr,
        SettingContent& settingContent,
        vector<SuperContent*>& contents,
        Json::Value& originalSetting,
        vector<Json::Value>& originalContents,
        vector<ImgPointPair>& topPairs,
        vector<ImgPointPair>& bottomPairs,
        int& imgRotation)
{
    Json::Reader reader;
        Json::Value value;

        settingContent.hasFeatureDes = false;

        if (reader.parse(inputStr, value)){
            originalSetting = value["setting"];

            Json::Value patObj = originalSetting["paperType"];
            if (!patObj.empty()){
                if (patObj.asString().compare("A3") == 0 || patObj.asString().compare("a3") == 0)
                    settingContent.paperType = Paper_A3;
                else
                    settingContent.paperType = Paper_A4;
            }
            if (!originalSetting["dpi"].empty())
                settingContent.dpi = originalSetting["dpi"].asInt();
            if (!originalSetting["width"].empty())
                settingContent.paperWidth = originalSetting["width"].asInt();
            if (!originalSetting["height"].empty())
                settingContent.paperHeight = originalSetting["height"].asInt();

            Json::Value choiceRect = originalSetting["choiceRect"];
            if (!choiceRect.empty()) {
                if (!choiceRect["width"].empty())
                    settingContent.choiceRectWidth = choiceRect["width"].asFloat();
                if (!choiceRect["height"].empty())
                    settingContent.choiceRectHeight = choiceRect["height"].asFloat();
            }

            settingContent.topMiddleMark = Rect(0, 0, 0, 0);
            settingContent.bottomMiddleMark = Rect(0, 0, 0, 0);

            settingContent.isOnlyStitchingImages = false;
            Json::Value isosiObj = originalSetting["isOnlyStitchingImages"];
            if (!isosiObj.empty())
                settingContent.isOnlyStitchingImages = isosiObj.asBool();

            Json::Value featureObj = originalSetting["featureRecognition"];
            if (!featureObj.empty()){
                settingContent.hasFeatureDes = true;
                settingContent.featureDes.featureType = featureObj["featureType"].asInt();

                settingContent.featureDes.templateFile = featureObj["templateFile"].asString();
                settingContent.featureDes.featureDescribeFile = featureObj["featureDescribeFile"].asString();

                if (!featureObj["matchRatio"].empty()) {
                    settingContent.featureDes.matchRatio = featureObj["matchRatio"].asFloat();
                }
                else {
                    settingContent.featureDes.matchRatio = 0.45;
                }

                settingContent.featureDes.seachAreaPadding = 0;
                if (!featureObj["seachAreaPadding"].empty()){
                    settingContent.featureDes.seachAreaPadding = featureObj["seachAreaPadding"].asFloat();
                }

                Json::Value rectObj = featureObj["rect"];
                settingContent.featureDes.rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
            }
            else{
                settingContent.hasFeatureDes = false;
            }

            Json::Value frontParamObj = originalSetting["frontCorrectionParam"];
            if (!frontParamObj.empty()){
                settingContent.hasFrontCorrectionParam = true;
                settingContent.frontRotation = frontParamObj["rotation"].asInt();
                settingContent.frontWarpMat = Mat(2, 3, CV_64FC1);

                if (!frontParamObj["isBackRotated180"].empty())
                    settingContent.isBackRotated180 = frontParamObj["isBackRotated180"].asBool();
                else
                    settingContent.isBackRotated180 = false;

                if (!frontParamObj["backAngleDeviation"].empty())
                    settingContent.backAngleDeviation = frontParamObj["backAngleDeviation"].asFloat();
                else
                    settingContent.backAngleDeviation = 0;

                Json::Value warpMatData = frontParamObj["warpMat"];
                for (int i = 0; i < 6; i++){
                    ((double*)settingContent.frontWarpMat.data)[i] = warpMatData[i].asDouble();
                }

                Json::Value isossObj = frontParamObj["isFrontParamForSaveImage"];
                if (!isossObj.empty()){
                    settingContent.isFrontParamForSaveImage = isossObj.asBool();
                }
                else
                    settingContent.isFrontParamForSaveImage = false;
            }
            else{
                settingContent.hasFrontCorrectionParam = false;
            }

            Json::Value issiObj = originalSetting["isSaveStandardImage"];
            if (!issiObj.empty()){
                settingContent.isSaveStandardImage = issiObj.asBool();
            }
            else
                settingContent.isSaveStandardImage = false;

            Json::Value isObjectiveDraw = originalSetting["isObjectiveDraw"];
            if (!isObjectiveDraw.empty()){
                settingContent.isObjectiveDraw = isObjectiveDraw.asBool();
            }
            else
                settingContent.isObjectiveDraw = false;

            if (settingContent.isObjectiveDraw){
                if (!originalSetting["roiX"].empty())
                    settingContent.roiX = originalSetting["roiX"].asInt();
                if (!originalSetting["roiY"].empty())
                    settingContent.roiY = originalSetting["roiY"].asInt();
                if (!originalSetting["roiWidth"].empty())
                    settingContent.roiWidth = originalSetting["roiWidth"].asInt();
                if (!originalSetting["roiHeight"].empty())
                    settingContent.roiHeight = originalSetting["roiHeight"].asInt();
                if (!originalSetting["roiDirection"].empty())
                    settingContent.roiDirection = originalSetting["roiDirection"].asString();
                if (!originalSetting["roiRotation"].empty())
                    settingContent.roiRotation = originalSetting["roiRotation"].asInt();
                if (!originalSetting["roiMicroRotate"].empty())
                    settingContent.roiMicroRotate = originalSetting["roiMicroRotate"].asFloat();

                if (!originalSetting["itemNumber"].empty())
                    settingContent.itemNumber = originalSetting["itemNumber"].asInt();
                else
                    settingContent.itemNumber = 0;
                if (!originalSetting["questionCount"].empty())
                    settingContent.questionCount = originalSetting["questionCount"].asInt();
                else
                    settingContent.questionCount = 0;
            }

            Json::Value sifObj = originalSetting["standardImageFilename"];
            if (!sifObj.empty()){
                settingContent.standardImageFilename = sifObj.asString();
            }

            Json::Value locif = originalSetting["locationIdentifier"];
            if (!locif.empty()){
                Json::Value markObj = locif["topLeft"];
                settingContent.topleftMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());
                markObj = locif["topRight"];
                settingContent.topRightMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());
                markObj = locif["bottomLeft"];
                settingContent.bottomLeftMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());
                markObj = locif["bottomRight"];
                settingContent.bottomRightMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());
                markObj = locif["topMiddle"];
                settingContent.topMiddleMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());
                markObj = locif["bottomMiddle"];
                settingContent.bottomMiddleMark = Rect(markObj["x"].asInt(), markObj["y"].asInt(), markObj["width"].asInt(), markObj["height"].asInt());

                Json::Value tops = locif["top"];
                Json::Value identifiers = tops["identifiers"];
                for (unsigned int i = 0; i < identifiers.size(); i++)
                {
                    Json::Value item = identifiers[i];
                    settingContent.topRects.push_back(Rect(item["x"].asInt(), item["y"].asInt(), item["width"].asInt(), item["height"].asInt()));
                }

                Json::Value lefts = locif["left"];
                identifiers = lefts["identifiers"];
                for (unsigned int i = 0; i < identifiers.size(); i++)
                {
                    Json::Value item = identifiers[i];
                    settingContent.leftRects.push_back(Rect(item["x"].asInt(), item["y"].asInt(), item["width"].asInt(), item["height"].asInt()));
                }

                Json::Value bottoms = locif["bottom"];
                identifiers = bottoms["identifiers"];
                for (unsigned int i = 0; i < identifiers.size(); i++)
                {
                    Json::Value item = identifiers[i];
                    settingContent.bottomRects.push_back(Rect(item["x"].asInt(), item["y"].asInt(), item["width"].asInt(), item["height"].asInt()));
                }

                settingContent.expandSearchMarkLegth = 0;
                Json::Value esmObj = locif["seachAreaPadding"];
                if (!esmObj.empty())
                {
                    settingContent.expandSearchMarkLegth = (int)(esmObj.asFloat());
                }
            }

            Json::Value crObj = originalSetting["choiceRect"];
            if (!crObj.empty())
            {
                settingContent.choiceSize = Size(crObj["width"].asInt(), crObj["height"].asInt());
            }

            Json::Value stObj = originalSetting["selectedThreshold"];
            if (!stObj.empty())
            {
                settingContent.selectedThreshold = stObj.asFloat();
            }
            else
                settingContent.selectedThreshold = 0.4f;

            Json::Value isMorObj = originalSetting["isMorphology"];
            if (!isMorObj.empty())
            {
                settingContent.isMorphology = isMorObj.asBool();
            }
            else
                settingContent.isMorphology = false;


            settingContent.barcodeType = originalSetting["barcodeType"].asString();

            Json::Value imgSaveObj = originalSetting["imageSave"];
            if (!imgSaveObj.empty())
            {
                if (imgSaveObj["format"].asString().compare("JPEG") == 0)
                    settingContent.saveImageFormat = Format_JPEG;
                else
                    settingContent.saveImageFormat = Format_PNG;
                Json::Value scaleObj = imgSaveObj["scale"];
                if (!scaleObj.empty())
                    settingContent.saveImageScale = scaleObj.asFloat();
                else
                    settingContent.saveImageScale = -1;

                Json::Value cotObj = imgSaveObj["compressionType"];
                if (!cotObj.empty())
                {
                    int tc = cotObj.asInt();
                    switch (tc)
                    {
                        case 1:
                            settingContent.saveParams.push_back(16);
                            settingContent.saveParams.push_back(3);
                            break;
                        case 2:
                            settingContent.saveParams.push_back(16);
                            settingContent.saveParams.push_back(1);
                            break;
                        case 3:
                            settingContent.saveParams.push_back(16);
                            settingContent.saveParams.push_back(3);
                            settingContent.saveParams.push_back(16);
                            settingContent.saveParams.push_back(1);
                            break;
                        default:
                            break;
                    }
                }

                settingContent.isThreshBinary = false;
                Json::Value itotsu = imgSaveObj["isThreshBinary"];
                if (!itotsu.empty())
                {
                    settingContent.isThreshBinary = itotsu.asBool();
                }

                settingContent.threshBinaryValue = 130;
                Json::Value tbvObj = imgSaveObj["threshBinaryValue"];
                if (!tbvObj.empty())
                {
                    settingContent.threshBinaryValue = tbvObj.asFloat();
                }
            }

            Json::Value quaObj = originalSetting["supportLowQuality"];
            if (!quaObj.empty())
            {
                settingContent.supportLowQuality = quaObj.asBool();
            }
            else
                settingContent.supportLowQuality = true;

            Json::Value enablePredictMark = originalSetting["enablePredictMark"];
            if (!enablePredictMark.empty())
            {
                settingContent.enablePredictMark = enablePredictMark.asBool();
            }
            else
                settingContent.enablePredictMark = true;

            Json::Value deepfindlocation = originalSetting["deepFindLocation"];
            if (!deepfindlocation.empty())
            {
                settingContent.deepfindlocation = deepfindlocation.asBool();
            }
            else
                settingContent.deepfindlocation = false;

            Json::Value topAndBottomMaxSlope = originalSetting["topAndBottomMaxSlope"];
            if (!topAndBottomMaxSlope.empty())
            {
                settingContent.topAndBottomMaxSlope = topAndBottomMaxSlope.asBool();
            }
            else
                settingContent.topAndBottomMaxSlope = 0.03;

            Json::Value pageMark = originalSetting["pageMark"];
            if (!pageMark.empty())
            {
                settingContent.hasPageMark = true;
                Json::Value pmRect = pageMark["rect"];
                int pageMarkItemNumber = pageMark["itemNumber"].asInt();

                settingContent.pageMarkItemNumber = pageMarkItemNumber;
                settingContent.pageMarkRect =
                    Rect(pmRect["x"].asInt(), pmRect["y"].asInt(), pmRect["width"].asInt(), pmRect["height"].asInt());
            }
            else
                settingContent.hasPageMark = false;

            Json::Value pageNumber = originalSetting["pageNumber"];
            if (!pageNumber.empty())
            {
                settingContent.pageNumber = pageNumber.asInt();
            }
            else
                settingContent.pageNumber = -1;

            Json::Value imagesStitching = value["imagesStitching"];
            if (!imagesStitching.empty())
            {
                for (unsigned int i = 0; i < imagesStitching.size(); i++)
                {
                    StitchingObject so;

                    Json::Value item = imagesStitching[i];

                    so.output = item["output"].asString();

                    if (item["orientation"].asString().compare("horizontal") == 0)
                        so.orientation = Horizontal;
                    else
                        so.orientation = Vertical;

                    Json::Value ins = item["inputs"];
                    for (unsigned int j = 0; j < ins.size(); j++)
                    {
                        so.inputs.push_back(ins[j].asString());
                    }

                    settingContent.imagesStitchings.push_back(so);
                }
            }

            Json::Value cardContents = value["recognitionContent"];
            if (!cardContents.empty())
            {
                for (unsigned int i = 0; i < cardContents.size(); i++)
                {
                    Json::Value item = cardContents[i];
                    originalContents.push_back(item);
                    int type = item["recognitionType"].asInt();

                    if (type == 1) //ӡˢ������
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        SerialNumber* object = new SerialNumber();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();

                        object->rotation = item["rotation"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 2) //��Ϳ����
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        Number* object = new Number();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();

                        if (item["rotation"].isNull())
                            object->rotation = 0;
                        else
                            object->rotation = item["rotation"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 3) //��ѡ��
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        Radio* object = new Radio();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 4) //��ѡ��
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        CheckBox* object = new CheckBox();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 5) //�ж���
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        TrueOrFalse* object = new TrueOrFalse();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 6) //������
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        SubjectiveItem* object = new SubjectiveItem();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 7) //ȱ��
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        AbsentFlag* object = new AbsentFlag();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        Json::Value isCircleDiameter = item["isCircleDiameter"];
                        if (!isCircleDiameter.empty())
                        {
                            object->isCircleDiameter = isCircleDiameter.asBool();
                        }
                        else
                        {
                            object->isCircleDiameter = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 8) //�ֶ�����ѡ����/ѡ����Ϳ��
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        ItemsBlock* object = new ItemsBlock();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        object->itemNumber = item["itemNumber"].asInt();
                        object->questionAmount = item["questionAmount"].asInt();
                        object->itemsPerRow = item["itemsPerRow"].asInt();
                        object->containNumber = item["containNumber"].asBool();

                        if (item["questionOrientation"].asString().compare("horizontal") == 0)
                            object->questionOrientation = Horizontal;
                        else
                            object->questionOrientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        Json::Value crObj = item["itemSize"];
                        if (!crObj.empty())
                        {
                            object->itemSize = Size(crObj["width"].asInt(), crObj["height"].asInt());
                        }
                        else
                        {
                            object->itemSize = Size(0, 0);
                        }

                        Json::Value firstRectObj = item["firstItemRect"];
                        if (!firstRectObj.empty())
                        {
                            object->firstItemRect = Rect(firstRectObj["x"].asInt(), firstRectObj["y"].asInt(), firstRectObj["width"].asInt(), firstRectObj["height"].asInt());
                        }

                        Json::Value lastItemRect = item["lastItemRect"];
                        if (!lastItemRect.empty())
                        {
                            object->lastItemRect = Rect(lastItemRect["x"].asInt(), lastItemRect["y"].asInt(), lastItemRect["width"].asInt(), lastItemRect["height"].asInt());
                        }

                        contents.push_back(object);
                    }
                    else if (type == 9) //ѡ����
                    {
                        //rect
                        Json::Value rectObj = item["rect"];

                        RadioSimple* object = new RadioSimple();
                        object->id = item["id"].asString();
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }
                        contents.push_back(object);
                    }
                    else if (type == 11) //Ŀǰ11�����ķ���ʶ��
                    {
                        Json::Value rectObj = item["rect"];

                        CardScore* object = new CardScore();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        object->questionNumber = item["questionNumber"].asInt();
                        object->itemNumber = item["itemNumber"].asInt();
                        object->scoreValue = item["scoreValue"].asFloat();
                        object->row = item["row"].asInt();

                        contents.push_back(object);
                    }
                    else if (type == 12) //Ŀǰ12�Ǵ򹳵�����ռ�ٷֱ�
                    {
                        Json::Value rectObj = item["rect"];

                        PixelsPercentScore* object = new PixelsPercentScore();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        object->questionNumber = item["questionNumber"].asInt();
                        object->itemNumber = item["itemNumber"].asInt();
                        object->row = item["row"].asInt();

                        Json::Value scoreValue = item["scoreValue"];
                        if (!scoreValue.empty())
                        {
                            object->scoreValue = item["scoreValue"].asFloat();
                        }
                        else
                        {
                            object->scoreValue = -1;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 13)
                    {
                        Json::Value rectObj = item["rect"];

                        HandwritingStudentNo* object = new HandwritingStudentNo();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();
                        object->row = item["row"].asInt();

                        if (item["rotation"].isNull())
                            object->rotation = 0;
                        else
                            object->rotation = item["rotation"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 15)
                    {
                        Json::Value rectObj = item["rect"];

                        HandwritingCardScore* object = new HandwritingCardScore();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();
                        object->row = item["row"].asInt();
                        object->questionNumber = item["questionNumber"].asInt();
                        object->scoreValue = item["scoreValue"].asFloat();

                        Json::Value childrenRectObj = rectObj["children"];
                        if (!childrenRectObj.empty())
                        {
                            vector<Rect> childrenRects;
                            for (unsigned int inx = 0; inx < childrenRectObj.size(); inx++)
                            {
                                Json::Value cRect = childrenRectObj[inx];
                                childrenRects.push_back(Rect(cRect["x"].asInt(), cRect["y"].asInt(), cRect["width"].asInt(), cRect["height"].asInt()));
                            }
                            object->childrenRect = childrenRects;
                        }

                        Json::Value denoisingScale = item["denoisingScale"];
                        if (!denoisingScale.empty())
                        {
                            object->denoisingScale = item["denoisingScale"].asFloat();
                        }
                        else
                        {
                            object->denoisingScale = 0.004;
                        }

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                    else if (type == 21) //ҳ��
                    {
                        Json::Value rectObj = item["rect"];

                        Radio* object = new Radio();
                        object->id = item["id"].asString();
                        object->structureNumber = item["structureNumber"];
                        object->type = type;
                        object->rect = Rect(rectObj["x"].asInt(), rectObj["y"].asInt(), rectObj["width"].asInt(), rectObj["height"].asInt());
                        if (item["orientation"].asString().compare("horizontal") == 0)
                            object->orientation = Horizontal;
                        else
                            object->orientation = Vertical;
                        object->itemNumber = item["itemNumber"].asInt();

                        Json::Value saveImage = item["saveImage"];
                        if (!saveImage.empty())
                        {
                            object->isSaveImage = saveImage.asBool();
                        }
                        else
                        {
                            object->isSaveImage = false;
                        }

                        contents.push_back(object);
                    }
                }
            }

            Json::Value markPointsObj = value["markPoints"];
            if (!markPointsObj.empty())
            {
                imgRotation = markPointsObj["imgRotation"].asInt();

                Json::Value topPointsObj = markPointsObj["topPoints"];

                for (unsigned int i = 0; i < topPointsObj.size(); i++)
                {
                    Json::Value item = topPointsObj[i];
                    ImgPointPair pt;
                    pt.loc = 1;
                    pt.isPredict = item["isPredict"].asBool() ? 1 : 0;
                    Json::Value standPointObj = item["standPoint"];
                    pt.standPoint = Point2f(standPointObj["x"].asFloat(), standPointObj["y"].asFloat());
                    Json::Value imgPointObj = item["imgPoint"];
                    pt.imgPoint = Point2f(imgPointObj["x"].asFloat(), imgPointObj["y"].asFloat());
                    topPairs.push_back(pt);
                }

                Json::Value bottomPointsObj = markPointsObj["bottomPoints"];
                for (unsigned int i = 0; i < bottomPointsObj.size(); i++)
                {
                    Json::Value item = bottomPointsObj[i];
                    ImgPointPair pt;
                    pt.loc = 2;
                    pt.isPredict = item["isPredict"].asBool() ? 1 : 0;
                    Json::Value standPointObj = item["standPoint"];
                    pt.standPoint = Point2f(standPointObj["x"].asFloat(), standPointObj["y"].asFloat());
                    Json::Value imgPointObj = item["imgPoint"];
                    pt.imgPoint = Point2f(imgPointObj["x"].asFloat(), imgPointObj["y"].asFloat());
                    bottomPairs.push_back(pt);
                }
            }
        }
        else
        {
            return false;
        }

        return true;
}

bool ParseIntputJson::parse4Stitching(string inputStr, SettingContent& settingContent) {

}
