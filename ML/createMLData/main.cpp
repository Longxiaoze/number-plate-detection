//
//  main.cpp
//  createMLData
//
//  opencv_createsamplesで学習サンプルを作成するための
//  テキストデータを用意するためのプログラム
//  左クリック: 車体領域の左上を決定
//  右クリック: 車体領域の右下を決定
//  Enter   : 領域を決定し次のフレームへ
//  Esc     : プログラムを終了
//
//  Created by 船津武志 on 2018/10/01.
//  Copyright © 2018 myStudy. All rights reserved.
//

#include "createMLDataHead.h"
#include <fstream>

using namespace std;
using namespace cv;

static void onMouse(int event, int x, int y, int flags, void* param);
void putMessage(Mat& img);

Mat imgOrigin, imgShow;
string windowName = "createMLData : set region of car"; // 作業用ウィンドウ名
string textFile = "/Users/funatsutakeshi/Desktop/Xcode_projects/CV/createMLData/pos1.dat";  // テキストファイル生成先
vector<Point> rectPt = {Point(0,0), Point(0,0)};  // 矩形初期化

int main(int argc, const char * argv[]) {

    // image directory name and number of images.
    string dirname = argv[1];
    int imgNum = atoi(argv[2]);

    // output file
    ofstream textToWrite;
    string imgExtension;
    textToWrite.open(textFile, ios::out);

    // set mouse callback
    namedWindow(windowName);
    setMouseCallback(windowName, onMouse);

    // select objects for all images
    for(int i = 0; i <= imgNum; i++) {
        // read a image
        imgOrigin = imreadMultiFormat(dirname + to_string(i), imgExtension);
        imgShow = imgOrigin.clone();
        // error : is not image or don't exist.
        if(imgOrigin.empty())
            cout << "image number" << i << "is invalid file or not existing, then skipped" << endl;
        else {
            // show image.
            putMessage(imgShow);
            imshow(windowName, imgShow);
            // write image file name to text.
            textToWrite << "../data/pos/" << i << imgExtension << "  1  ";

            // key action
            char key = waitKey();
            while(key != ASCII_CR && key != ASCII_ESC) {
                key = waitKey();
            }
            // enterが押されたら矩形情報を書き込んで次の画像へ
            if(key == ASCII_CR) {
                int width = abs(rectPt[1].x - rectPt[0].x);
                int height = abs(rectPt[1].y - rectPt[0].y);
                textToWrite << rectPt[0].x << " " << rectPt[0].y << " " << width << " " << height << endl;
            }
            if(key == ASCII_ESC) break; // escが押されたら終了
        }
    }
    return 0;
}

// Mouse Event
static void onMouse(int event, int x, int y, int flags, void* param) {
    if(event == EVENT_LBUTTONDOWN)
        rectPt[0] = Point(x, y);  // 左クリックで左上の点を保存
    if(event == EVENT_RBUTTONUP)
        rectPt[1] = Point(x, y);  // 右クリックで右上の点を保存
    imgShow = imgOrigin.clone();  // 画像をクリア
    putMessage(imgShow);          // メッセージを再描画
    rectangle(imgShow, rectPt[0], rectPt[1], 255);  // 矩形を更新
    imshow(windowName, imgShow);
}

void putMessage(Mat& img) {
    putText(img, "left click : left-top coordinate", Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0));
    putText(img, "right click : right-bottom coordinate", Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0));
    putText(img, "Enter : go to next image", Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0));
}
