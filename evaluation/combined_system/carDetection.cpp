//
//  carDetection.cpp
//  combined_system
//
//  車体検出実装(評価用)
//
//  Created by 船津武志 on 2018/11/28.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"

using namespace std;
using namespace cv;

Rect carDetection(const Mat& inputImg, CascadeClassifier& cascade, DetectionScale scale) {
    /* ----- define variables ----- */
    Mat procImg;
    Rect car(0, 0, 0, 0);
    vector<Rect> objects;
    int frameCent = inputImg.cols / 2;
    int topCent = 2000;
    float imgScale = 0.25;
    int imgReScale = 4;
    if(scale == SCALE_FAR) {
        imgScale = 0.5;
        imgReScale = 2;
    }
    /* ----- define variables(end) ----- */

    resize(inputImg, procImg, Size(), imgScale, imgScale);
    //equalizeHist(procImg, procImg);
    cascade.detectMultiScale(procImg, objects, 1.1, 2);

    /* ----- 最も画像中心軸に近いものを選択 ----- */
    if (objects.size() > 0) {
        for(auto& object : objects) {
            int i = 0;
            int objCent = object.x + (object.width / 2);
            if(abs(frameCent - objCent) < topCent) {
                car = object;
                topCent = abs(frameCent - objCent);
            }
            if(i > 3) break;  // 評価値top4つのみ候補とする
            i++;
        }
    }

    car = rescaleRect(car, imgReScale);  // スケールを戻す
    return car;
}
