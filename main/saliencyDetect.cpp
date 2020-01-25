//
//  saliencyDetect.cpp
//  main_system
//
//  BING顕著度を用いたナンバープレート検出
//  selective search より早いけど車体と同色のプレートでは機能しない
//
//  Created by 船津武志 on 2018/12/03.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"
#include <opencv2/saliency.hpp>

using namespace std;
using namespace cv;
using namespace saliency;

Rect saliencyDetect(const Mat& inputImg) {
    /* ----- define variables ----- */
    Rect plate(0, 0, 0, 0);
    Vec4i plateVec;
    vector<Vec4i> objects;
    static Rect prevPlate;

    const int ROIArea = inputImg.cols * inputImg.rows;
    const int ROICenter = inputImg.cols / 2;
    double maxScore = 10;
    /* ----- define variables(end) ----- */

    /* ----- BING saliency calculate ----- */
    Ptr<ObjectnessBING> BINGdetector = ObjectnessBING::create();
    BINGdetector->computeSaliency(inputImg, objects);
    /* ----- BING saliency calculate(end) ----- */

    /* -------- find number plate -------- */
    for(const auto& obj : objects) {
        int objWidth = obj[2] - obj[0], objHeight = obj[3] - obj[1];
        int area = objWidth * objHeight;
        int objX = obj[0];                    /*         ↓ search conditions ↓             */
        if(objWidth > objHeight     /* its width is bigger than height           */
            && area > ROIArea/30                     /* its size is not too small compared to ROI */
            && area < ROIArea/6                      /* its size is not too big compared to ROI   */
            && objX < ROICenter                      /* it contains the x center of ROI           */
            && objX + objWidth > ROICenter) {   // ↑ this condition works good because "ROI correspond to car"
            double aspectRatio = objWidth / (double)objHeight;
            if(abs(2.0 - aspectRatio) < maxScore) {
                plateVec = obj;
                maxScore = abs(2.0 - aspectRatio);
            }
        }
    }
    /* ------- find number plate(end) -------- */

    if(plateVec[2] - plateVec[0] > 0) {
        plate.x = plateVec[0];
        plate.y = plateVec[1];
        plate.width = plateVec[2] - plateVec[0];
        plate.height = plateVec[3] - plateVec[1];
        return plate;
    }
    else return prevPlate;
}
