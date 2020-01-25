//
//  selectiveHeader.hpp
//  combined_system
//
//  Created by 船津武志 on 2018/11/07.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#ifndef selectiveHeader_hpp
#define selectiveHeader_hpp

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#define PLATE_WIDTH 0.33        /* ナンバープレートの横幅[m] */
#define PLATE_HEIGHT 0.145      /* ナンバープレートの縦幅[m](文字端) */
//#define FOCAL_LENGTH 3.99/1000  /* 焦点距離[m](iphone7) */
//#define FOCAL_LENGTH 4.15/1000  /* 焦点距離[m](iphone6s) */
#define FOCAL_LENGTH 4.12/1000  /* 焦点距離[m](iphone5) */
//#define PITCH 3.3/1000000          /* 画素ピッチ7・6s(FullHD換算)[m] */
#define PITCH 3.0556/1000000          /* 画素ピッチ５5(FullHD換算)[m] */

#define ASCII_F 102

enum DetectionMode{MODE_CAR, MODE_PLATE};    // 距離測定モード
enum DetectionScale{SCALE_FAR, SCALE_NEAR};  // 車体検出スケール

cv::Rect carDetection(const cv::Mat& inputImg, cv::CascadeClassifier& cascade, DetectionScale scale);
cv::Rect strEdgeDetect(const cv::Mat& inputImg);
cv::Rect rescaleRect(const cv::Rect& inputRect, const int scale);
float calculateDistance(double plateWidth, double carWidth, DetectionMode mode);

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

#endif /* selectiveHeader_hpp */
