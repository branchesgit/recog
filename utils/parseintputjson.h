#ifndef PARSEINTPUTJSON_H
#define PARSEINTPUTJSON_H

#include "../items/settingcontent.h"
#include <json/json.h>
#include "../items/cardcontent.h"
#include "../items/supercontent.h"

using namespace std;

class ParseIntputJson
{
public:
    ParseIntputJson();


    bool parse(string inputStr, SettingContent& settingContent,
               vector<SuperContent*>& contents, Json::Value& originalSetting,
               vector<Json::Value>& originalContents, vector<ImgPointPair>& topPairs,
               vector<ImgPointPair>& bottomPairs, int& imgRotation);

    bool parse4Stitching(string inputStr, SettingContent& settingContent);
};

#endif // PARSEINTPUTJSON_H
