//
//  generic.cpp
//  combined_system
//
//  汎用関数の実装
//
//  Created by 船津武志 on 2018/11/22.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"
#include <numeric>

using namespace std;
using namespace cv;

/* ----- 矩形のスケール変換 ----- */
Rect rescaleRect(const Rect& inputRect, const int scale) {
    Rect dstRect = inputRect;
    dstRect.x *= scale;
    dstRect.y *= scale;
    dstRect.width *= scale;
    dstRect.height *= scale;

    return dstRect;
}

/* ----- get width of HistBlock ----- */
int HistBlock::getWidth() {
    return bottom - top;
}

/* ----- calculate distance(evaluation ver.) ----- */
float calculateDistance(double plateWidth, double carWidth, DetectionMode mode) {
    static int frames = 1;
    static float ratioAdd;
    static float widthRatio;
    static float avrRatio;  // 過去の計算結果も加味した平均の比率を用いる
    float distance;


    if(mode == MODE_PLATE) {
        if(carWidth != 0 && plateWidth != 0) {
            widthRatio = plateWidth / carWidth;
            ratioAdd += widthRatio;
            avrRatio = ratioAdd / (float)frames;
            frames++;
        }
        distance = FOCAL_LENGTH * (PLATE_HEIGHT / (PITCH * plateWidth));
    } else {
        distance = FOCAL_LENGTH * (PLATE_HEIGHT / (PITCH * avrRatio * carWidth));
    }
    return distance;
}
