#include "roipixel.h"

void RoiPixel::addContent(double counts){
    mContent += counts;
}

double RoiPixel::content(){
    return mContent;
}
