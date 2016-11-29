#include "roipixel.h"

void RoiPixel::addContent(double counts){
    mContent += counts;
}

double RoiPixel::content(){
    return mContent;
}

std::vector<CdbPixel*> RoiPixel::getContent(std::vector<CdbPixel*> pxList,int depth){
    std::vector<CdbPixel*> remaining;
    if (depth < mMaxDepth){
        for(int n = 0;n<pxList.size();n++){
            CdbPixel *px = pxList[n];
            if (px->inside(this)==4){
                mContent += px->counts();
                //pixel is inside
            }
            else if (px->inside(this)==0 && AbstractPixel::inside(px) ==0) {
                remaining.push_back(px);
                //pixel is completely not inside.
            }
            else {
                remaining.push_back(px);
                //pixel is partially inside
                std::vector<CdbPixel*> subList= px->split();
                getContent(subList,depth+1);
            }
        }
    }
    else {
        for(int n=0;n<pxList.size();n++){
            CdbPixel *px = pxList[n];
            if (px->inside(this)==4){
                mContent += px->counts();
                //pixel is inside
            }
        }
    }
    return remaining;
}
