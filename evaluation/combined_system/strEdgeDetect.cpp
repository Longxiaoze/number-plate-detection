//
//  strEdgeDetect.cpp
//  combined_system
//
//  垂直エッジヒストグラムによるナンバープレート探索の実装
//
//  Created by 船津武志 on 2018/12/25.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

#include "detectionHeader.hpp"

using namespace std;
using namespace cv;

Rect strEdgeDetect(const Mat& inputImg) {
    /* ----- define variables ----- */
    int width = inputImg.cols;
    int height = inputImg.rows;
    int searchWidth = width * 0.3;
    Rect roi((width-searchWidth)/2, 0, searchWidth, height);
    vector<double> histgram(height, 0);

    Mat grayImg;
    cvtColor(inputImg, grayImg, CV_BGR2GRAY);
    Sobel(grayImg, grayImg, CV_8UC1, 2, 0);
    Mat roiImg = grayImg(roi);
    /* ----- define variables(end) ----- */

    /* ----- calculate histgram ----- */
    for(size_t i = 0; i < height; i++) {
        size_t step = i * roiImg.step;
        for(size_t j = 0; j < roiImg.cols; j++) {
            histgram[i] += roiImg.data[step + j*roiImg.elemSize()];
        }
    }

    /* ----- normalize ----- */
    double histMax = *max_element(histgram.begin(), histgram.end());
    for(auto& hist : histgram) {
        hist /= histMax;
    }

    /* ----- find peaks ----- */
    vector<HistBlock> h_blocks;
    findPeak(h_blocks, histgram.begin(), histgram.end(), histgram.begin());

    /* ----- sort h_blocks with max-value ----- */
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

    double plateHeight = bottom - top;

    return Rect(0, top, width, plateHeight);
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
