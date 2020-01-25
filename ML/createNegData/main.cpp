//
//  main.cpp
//  createNegData
//
//  opencv_traincascadeでの学習時に必要な
//  negデータを作成する為のプログラム
//
//  Created by 船津武志 on 2018/10/12.
//  Copyright © 2018 myStudy. All rights reserved.
//

#include "Header.h"
#include <fstream>

using namespace std;
using namespace cv;

Mat img;
string windowName = "createNegData";
string textFile = "/Users/funatsutakeshi/Desktop/Xcode_projects/CV/createNegData/negative.dat"; // テキストファイル生成path

int main(int argc, const char * argv[]) {

    // image directory name and number of images.
    string dirname = argv[1];
    int imgNum = atoi(argv[2]);
    
    // output file
    ofstream textToWrite;
    string imgExtension;
    textToWrite.open(textFile, ios::out);

    // select objects for all images
    for(int i = 1; i <= imgNum; i++) {
        // read a image
        img = imreadMultiFormat(dirname + "neg" + to_string(i), imgExtension);
        // error : is not image or don't exist.
        if(img.empty())
            cout << "image number" << i << "is unvalid fire or not existing, then skipped" << endl;
        else {
            // write image file name.
            textToWrite << "../data/neg/neg" << i << imgExtension << endl;
        }
    }
    return 0;
}
