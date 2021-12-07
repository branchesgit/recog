#ifndef ITEMSBLOCK_H
#define ITEMSBLOCK_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class ItemsBlock :public SuperContent
{
public:
    int itemNumber;//ѡ�����
    int questionAmount;//����
    int itemsPerRow; //ÿ���ж��ٸ���Ŀ
    bool containNumber;//�Ƿ����������Ŀ���
    Size itemSize; //ѡ����
    Rect firstItemRect;//��һ����Ϳѡ���Rect
    Rect lastItemRect;//���һ����Ϳѡ���Rect
    ContentOrientation questionOrientation;//����
    virtual void Do(Mat& img);
};

#endif // ITEMSBLOCK_H
