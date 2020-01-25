//
//  createMLDataHead.h
//  createMLData
//
//  Created by 船津武志 on 2018/10/02.
//  Copyright © 2018 myStudy. All rights reserved.
//

#ifndef createMLDataHead_h
#define createMLDataHead_h

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define ASCII_CR 13
#define ASCII_ESC 27
#define ASCII_DEL 127
#define ASCII_a 97

Mat imreadMultiFormat(string imgPassBeforeExt, string& code);

#endif /* createMLDataHead_h */
