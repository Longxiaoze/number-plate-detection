//
//  carDetection.cpp
//  main_system
//
//  車体検出(要改善)
//  評価版のように、中心軸の位置による絞り込み・画像スケールの変化を実装した方がいいかも
//
//  Created by 船津武志 on 2018/11/28.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"

using namespace std;
using namespace cv;

Rect carDetection(const Mat& inputImg, CascadeClassifier& cascade) {
    /* ----- define variables ----- */
    Mat procImg;
    Rect car(0, 0, 0, 0);
    vector<Rect> objects;
    /* ----- define variables(end) ----- */

    resize(inputImg, procImg, Size(), 0.25, 0.25);
    //equalizeHist(procImg, procImg);
    cascade.detectMultiScale(procImg, objects, 1.12, 2);
    if (objects.size() > 0)
        car = objects[0];   // 評価値最大の矩形を取得

    car = rescaleRect(car, 4);
    return car;
}
