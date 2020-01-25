//
//  main.cpp
//  combined_system
//
//  垂直エッジヒストグラムと車体幅の両方を用いて車間距離を測定するシステムの評価
//
//  Created by 船津武志 on 2019/02/28.
//  Copyright © 2019 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"

using namespace std;
using namespace cv;

int main() {
    /* ------- initialize -------*/
    const int maxImg = 100;
    const string path = "/Users/funatsutakeshi/Desktop/tokken/data/distance/iphone5/2015_4_3_2/fullHD/";
    CascadeClassifier cascade("/Users/funatsutakeshi/Desktop/tokken/data/ML/Cascades/Haar/size20/stages20/false05.xml");
    if(cascade.empty()) {
        cerr << "cascade file was not found." << endl;
        return -1;
    }
    DetectionMode currentMode = MODE_PLATE;
    Mat imgGray, imgCarROI;
    Rect plate, car;
    DetectionScale scale = SCALE_NEAR;
    /* -------- initialize (end) -------- */

    for (int i = 2; i <= maxImg; i++) {
        Mat inputImg = imread(path + to_string(i) + "m.JPG");
        if(inputImg.empty()) continue;
        if(i > 8) currentMode = MODE_CAR;  // 遠距離(9m以上)では車体幅から距離を算出

        cvtColor(inputImg, imgGray, CV_BGR2GRAY);

        if(i > 11) scale = SCALE_FAR;  // 遠距離(12m以上)では車体検出時の画像スケールを大きく
        car = carDetection(imgGray, cascade, scale);

        /* ----- plate detection ----- */
        if(car.width > 0 && currentMode == MODE_PLATE) {
            imgCarROI = inputImg(car);
            plate = strEdgeDetect(imgCarROI);
            plate.x += car.x;
            plate.y += car.y;
        }

        /* ----- draw ----- */
        rectangle(inputImg, car, Scalar(0, 0, 255), 3);
        if(currentMode == MODE_PLATE) rectangle(inputImg, plate, Scalar(0, 255, 0), 3);

        /* ----- distance calcuration ----- */
        float distance = calculateDistance(plate.height, car.width, currentMode);

        /* ----- show ----- */
        imshow("dst", inputImg);
        unsigned char key = waitKey();
        if(key == ASCII_F) distance = 0;

        cout << "distance " << i << ":" << distance << "[m]" << endl;
    }

    return 0;

}
