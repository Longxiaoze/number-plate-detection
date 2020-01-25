//
//  main.cpp
//  Vertical_Edge_Histogram
//
//  垂直エッジヒストグラムを用いたナンバープレート検出精度評価用プログラム
//
//  Created by 船津武志 on 2019/01/10.
//  Copyright © 2019 Sasaki-lab. All rights reserved.
//

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <vector>
#include <iostream>

#define PLATE_WIDTH 0.33        /* ナンバープレートの横幅[m] */
#define PLATE_HEIGHT 0.145      /* ナンバープレートの縦幅[m](文字端): 外側を検出してしまう傾向への対策として実際(135mm)より+10mmの補正 */
//#define FOCAL_LENGTH 3.99/1000  /* 焦点距離[m](iphone7) */
//#define FOCAL_LENGTH 4.15/1000  /* 焦点距離[m](iphone6s) */
#define FOCAL_LENGTH 4.12/1000  /* 焦点距離[m](iphone5) */
//#define PITCH 3.3/1000000          /* 画素ピッチ7・6s(FullHD換算)[m] */
#define PITCH 3.0556/1000000          /* 画素ピッチ5(FullHD換算)[m] */

#define ASCII_F 102  // f(=false) on ASCII

using namespace std;
using namespace cv;

// ヒストグラム中の候補ブロックのクラス
class HistBlock {
public:
    double max; // ピークの値
    int top;  // ピーク位置
    int bottom; // 下端位置
    int peak;   // 上端位置

    HistBlock(double max_, int top_, int bottom_, int peak_) : max(max_), top(top_), bottom(bottom_), peak(peak_) {}
    int getWidth();
};

// 候補ブロック探索の関数
void findPeak(std::vector<HistBlock>& dst, std::vector<double>::iterator begin, std::vector<double>::iterator end, std::vector<double>::iterator base);

int main(int argc, const char * argv[]) {
    string path = "/Users/funatsutakeshi/Desktop/tokken/data/distance/iphone5/2015_4_3_2/";  // 評価用画像へのパス
    for(int i = 2; i < 100; i++) {
        Mat inputImg = imread(path + "trim/" + to_string(i) + "m.JPG");
        if(inputImg.empty()) continue;
        int width = inputImg.cols;
        int height = inputImg.rows;
        int searchWidth = width * 0.3;  // ナンバープレート探索幅
        Rect roi((width - searchWidth)/2, 0, searchWidth, height);  // ナンバープレート探索領域
        vector<double> histogram(height, 0);  // ヒストグラム実態

        /* ----- pre-processing(グレースケール変換 & 2次微分) ----- */
        Mat grayImg;
        cvtColor(inputImg, grayImg, CV_BGR2GRAY);
        Sobel(grayImg, grayImg, CV_8UC1, 2, 0);
        Mat roiImg = grayImg(roi);

        /* ----- calculate histogram ----- */
        for(size_t i = 0; i < height; i++) {
            size_t step = i * roiImg.step;
            for(size_t j = 0; j < roiImg.cols; j++) {
                histogram[i] += roiImg.data[step + j*roiImg.elemSize()];
            }
        }

        /* ----- normalize ----- */
        double histMax = *max_element(histogram.begin(), histogram.end());
        for(auto& hist : histogram) {
            hist /= histMax;
        }

        /* ----- find peaks ----- */
        vector<HistBlock> h_blocks;
        findPeak(h_blocks, histogram.begin(), histogram.end(), histogram.begin());

        /* ----- sort blocks with max-value ----- */
        for(int i = 1; i < h_blocks.size(); i++) {
            int j = i;
            while(j >= 1 && h_blocks[j-1].max < h_blocks[j].max) {
                swap(h_blocks[j], h_blocks[j-1]);
                j--;
            }
        }

        /* ----- find NP ----- */
        int top = 0, bottom = 0;
        bool detected = false;
        char count = 1;
        for(auto& base_block : h_blocks) {
            for(int i = count; i < h_blocks.size(); i++) {
                if(base_block.peak < h_blocks[i].peak && h_blocks[i].top - base_block.bottom < height * 0.03) {
                    if(base_block.getWidth() < h_blocks[i].getWidth()) {
                        top = base_block.top;
                        bottom = h_blocks[i].bottom;
                        detected = true;
                        break;
                    }
                }
                else if(base_block.peak > h_blocks[i].peak && base_block.top - h_blocks[i].bottom < height * 0.03) {
                    if(base_block.getWidth() > h_blocks[i].getWidth()) {
                        top = h_blocks[i].top;
                        bottom = base_block.bottom;
                        detected = true;
                        break;
                    }
                }
            }
            if(detected) break;
            count++;
        }

        /* ----- draw histogram ----- */
        Mat showHist = Mat::zeros(height, 100, CV_8UC1);
        for(int i = 0; i < height; i++) {
            line(showHist, Point(0, i), Point(int(histogram[i]*100), i), 255);
        }

        /* ----- distance calcuration and show image ----- */
        double plateHeight = bottom - top;
        double distance = FOCAL_LENGTH * (PLATE_HEIGHT / (PITCH * plateHeight));
        rectangle(inputImg, Rect(0, top, width, plateHeight), Scalar(0, 0, 255), 2);
        imshow("dst", inputImg);
        imshow("hist", showHist);
        char key = waitKey();
        string write_dst = path + "dst/" + to_string(i) + "m.jpg";
        imwrite(write_dst, inputImg);
        if(key == ASCII_F) distance = 0;
        cout << "distance " << i << ":" << distance << "[m]" << endl;
    }
}

// ブロック幅取得
int HistBlock::getWidth() {
    return bottom - top;
}

void findPeak(vector<HistBlock>& dst, vector<double>::iterator begin, vector<double>::iterator end, vector<double>::iterator base) {
    decltype(begin) it_peak = max_element(begin, end);

    /* ----- 終了条件: 領域幅が0 or 0.4より大きいピークが存在しない ----- */
    if(end - begin > 0 && *it_peak > 0.4) {
        decltype(it_peak) it_back = it_peak;   // 上を捜査するイテレータ
        decltype(it_peak) it_front = it_peak;  // 下を捜査するイテレータ
        HistBlock h_block(*it_peak, int(begin - base), int(end - base), int(it_peak - base));

        /* ----- ヒストグラムが下がりきる(<0.2) or 極小値になる 点を探索 ----- */
        while(it_back != begin) {
            if(*it_back < 0.2 || (*(it_back - 1) > *it_back && *it_peak - *it_back > 0.5)) {
                h_block.top = int(it_back - base);
                break;
            }
            else it_back--;
        }
        while(it_front != end) {
            if(*it_front < 0.2 || (*(it_front + 1) > *it_front && *it_peak - *it_front > 0.5)) {
                h_block.bottom = int(it_front - base);
                break;
            }
            else it_front++;
        }

        /* ----- ブロック位置が極端に上でなければブロックを格納(上部は木や建物など背景の垂直エッジが入りやすい為) ----- */
        if(h_block.top - *base > 5) dst.push_back(h_block);

        /* ----- 残りの領域を再帰的に探索 ----- */
        findPeak(dst, begin, it_back, base);
        findPeak(dst, it_front, end, base);
    }
}
