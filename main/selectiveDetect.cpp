//
//  selectiveDetect.cpp
//  main_system
//
//  selective search によるナンバープレート検出
//  Huモーメント同様車体と同色の場合に機能しない & 計算量多い
//
//  Created by 船津武志 on 2018/11/07.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"
#include <opencv2/ximgproc/segmentation.hpp>

using namespace cv;
using namespace std;
using namespace cv::ximgproc::segmentation;

Rect selectiveDetect(const Mat& inputImg) {
    /* -------- define variables -------- */
    // process image
    Mat procImg;
    resize(inputImg, procImg, Size(), 0.25, 0.25);
    // ROI info
    const int ROIArea = procImg.rows * procImg.cols;
    const int ROICenter = procImg.cols/2;
    // previous plate
    static Rect prevPlate;
    // bounding-box of objects and Number-Plate
    vector<Rect> objects;
    Rect plate(0, 0, 0, 0);
    // detection parameters
    double maxScore = 10;
    int numSearchRect = 100;
    /* -------- define variables(end) -------- */

    /* -------- pre-processing -------- */
    double sigmaX = 1.0, sigmaY = 1.0;
    GaussianBlur(procImg, procImg, Size(5, 5), sigmaX, sigmaY);
    /* -------- pre-priocessing(end) -------- */

    /* ------- detect object-like bounding boxes -------- */
    Ptr<SelectiveSearchSegmentation> p_ss = createSelectiveSearchSegmentation();
    p_ss -> setBaseImage(procImg);
    p_ss -> switchToSelectiveSearchFast();
    p_ss -> process(objects);
    /* ------- detect object-like bounding boxes(end) -------- */

    /* -------- find number plate -------- */
    for(size_t i = 0; i < objects.size(); i++) {
        if(i < numSearchRect) {
            int area = objects[i].area();
            int objX = objects[i].x;                    /*         ↓ search conditions ↓             */
            if(objects[i].width > objects[i].height     /* its width is bigger than height           */
               && area > ROIArea/30                     /* its size is not too small compared to ROI */
               && area < ROIArea/6                      /* its size is not too big compared to ROI   */
               && objX < ROICenter                      /* it contains the x center of ROI           */
               && objX + objects[i].width > ROICenter) {   // ↑ this condition works good because "ROI correspond to car"
                double aspectRatio = objects[i].width / (double)objects[i].height;
                if(abs(2.0 - aspectRatio) < maxScore) {
                    plate = objects[i];
                    maxScore = abs(2.0 - aspectRatio);
                }
            }
        } else break;
    }
    /* ------- find number plate(end) -------- */

    // if new plate detected, return it
    plate = rescaleRect(plate, 4);
    if(plate.width > 0) {
        prevPlate = plate;
        return plate;
    }
    else return prevPlate;
}
