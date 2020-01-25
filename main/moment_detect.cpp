//
//  moment_detect.cpp
//  main_system
//
//  Huモーメント不変量を用いたナンバープレート検出
//  早くてそこそこ強いが、車体と同色のプレートでは輪郭線が取れないので機能しない。
//
//  Created by 船津武志 on 2018/04/01.
//  Copyright © 2018年 Sasaki_lab. All rights reserved.
//

#include "detectionHeader.hpp"

using namespace cv;
using namespace std;

Rect momentDetect(const Mat& inputImg, const vector<vector<Point>>& plateCont){
    // ROI info
    int height = inputImg.rows;
    int width = inputImg.cols;
    int roiSize = height * 2 + width * 2;


    /* BGR→GRAY */
    Mat grayImg(height, width, CV_8UC1);
    cvtColor(inputImg, grayImg, CV_BGR2GRAY);

    /* thresholding */
    adaptiveThreshold(grayImg, grayImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 10);

    /* detect contours */
    vector<Vec4i> hierarchy;
    vector<vector<Point>> contours;
    findContours(grayImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    /* 図形の絞り込み(輪郭線候補を3つ選択) */
    int plateIdx[3] = { 0 };
    size_t contSize = 0;
    double matchTop[3] = { 100 };
    double match;
    for(int i = 0; i < contours.size(); i++){
        contSize = contours[i].size();
        //int contArea = contourArea(contours[i]);
        if( contSize > (int)(roiSize / 10) && contSize < (int)(roiSize / 2)){
            match = matchShapes(contours[i], plateCont[0], CV_CONTOURS_MATCH_I2, 0);
            if(match < matchTop[0]){
                plateIdx[1] = plateIdx[0];
                plateIdx[0] = i;
                matchTop[1] = matchTop[0];
                matchTop[0] = match;
            }else if(match < matchTop[1]){
                plateIdx[2] = plateIdx[1];
                plateIdx[1] = i;
                matchTop[2] = matchTop[1];
                matchTop[1] = match;
            }else if(match < matchTop[2]){
                plateIdx[2] = i;
                matchTop[2] = match;
            }
        }
    }

    // convert to Rect and calculate aspect racio
    Rect dstRect, tmpRect;
    double aspect, aspectDst = 10.0;
    for(int i = 0; i < 3; i++) {
        tmpRect = boundingRect(contours[plateIdx[i]]);
        aspect = double(tmpRect.width / tmpRect.height);
        if(aspect > 1.0 && abs(2.0 - aspect) < abs(2.0 - aspectDst)) {
            aspectDst = aspect;
            dstRect = tmpRect;
        }
    }

    return dstRect;
}
