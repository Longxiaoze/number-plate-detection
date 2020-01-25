//
//  generic.cpp
//  main_system
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

/* ------ definition of FocusRect ----- */
void FocusRect::focus(Rect on, int frameWidth, int frameHeight) {
    if(on.width > 0) {
        vector<int> dist(4);
        dist[0] = on.x - p1.x;
        dist[1] = on.y - p1.y;
        dist[2] = p2.x - (on.x + on.width);
        dist[3] = p2.y - (on.y + on.height);

        p1.x = p1.x + dist[0] * focusSpeed;
        p1.y = p1.y + dist[1] * focusSpeed;
        p2.x = p2.x - dist[2] * focusSpeed;
        p2.y = p2.y - dist[3] * focusSpeed;

        if(p1.x < 0) p1.x = 0;
        if(p1.y < 0) p1.y = 0;
        if(p2.x > frameWidth) p2.x = frameWidth;
        if(p2.y > frameHeight) p2.y = frameHeight;
    }
}

void FocusRect::defocus(int fWidth, int fHeight) {
    if(p1.x - defocusSpeed > 0) p1.x -= defocusSpeed;
    if(p1.y - defocusSpeed > 0) p1.y -= defocusSpeed;
    if(p2.x + defocusSpeed < fWidth) p2.x += defocusSpeed;
    if(p2.y + defocusSpeed < fHeight) p2.y += defocusSpeed;
}

Rect FocusRect::toRect() {
    Rect dst(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
    return dst;
}

bool FocusRect::hasSize() {
    if(p1.x - p2.x == 0 || p1.y - p2.y == 0) return false;
    else return true;
}

/* ----- definition of HistBlock ----- */
int HistBlock::getWidth() {
    return bottom - top;
}

/* ----- 距離計算 ----- */
float calculateDistance(double plateWidth, double carWidth, DetectionMode mode) {
    static size_t frames;
    static vector<float> accumPlateWidth(10);
    static vector<float> accumCarWidth(10);
    static float widthRatio;
    float distance = 100.0;

    accumPlateWidth[frames % accumPlateWidth.size()] = plateWidth;
    accumCarWidth[frames % accumCarWidth.size()] = carWidth;
    frames++;

    float avrPlateWidth = accumulate(accumPlateWidth.begin(), accumPlateWidth.end(), 0) / (float)accumPlateWidth.size();
    float avrCarWidth = accumulate(accumCarWidth.begin(), accumCarWidth.end(), 0) / (float)accumCarWidth.size();

    if(mode == MODE_PLATE) {
        widthRatio = avrPlateWidth / avrCarWidth;
        distance = FOCAL_LENGTH * (PLATE_HEIGHT / (PITCH * plateWidth));
    } else {
        distance = FOCAL_LENGTH * (PLATE_HEIGHT / (PITCH * widthRatio * avrCarWidth));
    }

    return distance;
}
