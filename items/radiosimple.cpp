#include "radiosimple.h"

RadioSimple::RadioSimple()
{

}


void RadioSimple::Do(Mat& img)
{
    ItemSingle(img, *this, isSaveImage);
}
