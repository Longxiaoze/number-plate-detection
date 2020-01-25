//
//  detectionHeader.hpp
//  main_system
//
//  Created by 船津武志 on 2018/11/07.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#ifndef detectionHeader_hpp
#define detectionHeader_hpp

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#define PLATE_WIDTH 0.33        /* ナンバープレートの横幅[m] */
#define PLATE_HEIGHT 0.145      /* ナンバープレートの縦幅[m](文字端): +10[mm]の補正あり */
#define FOCAL_LENGTH 4.15/1000  /* 焦点距離[m](iphone7) */
// #define PITCH 2.5/1000000          /* 画素ピッチ横(動画)[m] */
#define PITCH 3.3/1000000         /* 画素ピッチ縦(動画)[m] */
#define ASCII_ESC 27

enum DetectionMode{MODE_CAR, MODE_PLATE};

cv::Rect carDetection(const cv::Mat& inputImg, cv::CascadeClassifier& cascade);
cv::Rect selectiveDetect(const cv::Mat& inputImg);
cv::Rect saliencyDetect(const cv::Mat& inputImg);
cv::Rect strEdgeDetect(const cv::Mat& inputImg);
cv::Rect rescaleRect(const cv::Rect& inputRect, const int scale);
float calculateDistance(double plateWidth, double carWidth, DetectionMode mode);

class FocusRect {
    float focusSpeed;
    float defocusSpeed;
public:
    cv::Point p1, p2;
    FocusRect(cv::Point p1_, cv::Point p2_, float Fs, float deFs) : p1(p1_), p2(p2_), focusSpeed(Fs), defocusSpeed(deFs) {}
    void focus(cv::Rect on, int frameWidth, int frameHeight);
    void defocus(int frameWidth, int frameHeight);
    cv::Rect toRect();
    bool hasSize();
};

class HistBlock {
public:
    double max;
    int top;
    int bottom;
    int peak;
    HistBlock(double max_, int top_, int bottom_, int peak_) : max(max_), top(top_), bottom(bottom_), peak(peak_) {}
    int getWidth();
};

void findPeak(std::vector<HistBlock>& dst, std::vector<double>::iterator begin, std::vector<double>::iterator end, std::vector<double>::iterator base);

#endif /* detectionHeader_hpp */
