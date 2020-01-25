//
//  imreadMultiFormat.cpp
//  createMLData
//
//  様々な拡張子の画像を取得するimreadの拡張
//
//  Created by 船津武志 on 2018/10/02.
//  Copyright © 2018 myStudy. All rights reserved.
//

#include "createMLDataHead.h"

using namespace std;
using namespace cv;

Mat imreadMultiFormat(string imgPassBeforeExt, string& code) {
    Mat img = imread(imgPassBeforeExt + ".jpg");
    code = ".jpg";
    if(img.empty()) {
        img = imread(imgPassBeforeExt + ".png");
        code = ".png";
        if(img.empty()) {
            img = imread(imgPassBeforeExt + ".jpeg");
            code = ".jpeg";
            if(img.empty()) {
                img = imread(imgPassBeforeExt + ".PNG");
                code = ".PNG";
                if(img.empty()) {
                    img = imread(imgPassBeforeExt + ".JPG");
                    code = ".JPG";
                    if(img.empty()) {
                        img = imread(imgPassBeforeExt + ".JPEG");
                        code = ".JPEG";
                    }
                }
            }
        }
    }

    return img;
}
