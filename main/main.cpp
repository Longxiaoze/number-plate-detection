//
//  main.cpp
//  main_system
//
//  メインシステム(フォーカシング → 車体検出 → ナンバープレート検出 → 距離計算)
//  高速化のため描画はOpenGLで記述
//  速度評価用の記述あり
//
//  Created by 船津武志 on 2018/11/07.
//  Copyright © 2018 Sasaki-lab. All rights reserved.
//

/* ----- OpenGLの関数でdeprecatedのWarningが出るので表示しない ----- */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include "detectionHeader.hpp"
#include <GLFW/glfw3.h>
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;

int main(int argc, const char * argv[]) {
    /* ------- read files -------*/
    // Movie
    VideoCapture capture("/Users/funatsutakeshi/Desktop/tokken/data/mov/for_track/near3.mov");
    if(!capture.isOpened()) return -1;
    const int frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    const int frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    // Car detector
    CascadeClassifier cascade("/Users/funatsutakeshi/Desktop/tokken/data/ML/Cascades/Haar/size20/stages20/false05.xml");
    if(cascade.empty()) {
        cerr << "cascade file was not found." << endl;
        return -1;
    }
    /* -------- read files(end) --------- */


    /* ------- OpenGL initialize -------- */
    // initialize GLFW
    if(!glfwInit()) {
        cerr << "Can't initialize GLFW" << endl;
        return 1;
    }

    // "glfwTerminate" will always be called at the end of this program
    atexit(glfwTerminate);

    // create window(!division by 2 is only for Retina Display!)
    GLFWwindow* const window(glfwCreateWindow(frameWidth/2, frameHeight/2, "output", NULL, NULL));
    if(window == NULL) {
        cerr << "Can't create window" << endl;
        return 1;
    }

    // set window's rendering context to "current"
    glfwMakeContextCurrent(window);

    // clear window
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // axis convertion
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-frameWidth / 2, frameWidth / 2, -frameHeight / 2, frameHeight / 2, -1, 1);
    glViewport(0, 0, frameWidth, frameHeight);
    /* -------- OpenGL initialize(end) ------- */


    /* -------- define variables ------- */
    Mat imgShow, imgProcCol, imgProcGray, imgCarROI;

    Rect car(0, 0, frameWidth, frameHeight);
    Rect plate, newCar, showCar, showPlate;

    FocusRect focusCar(Point(0, 0), Point(frameWidth, frameHeight), 0.1, 5.0);

    DetectionMode currentMode = MODE_PLATE;

    Size movSize(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    VideoWriter movOut = VideoWriter("/Users/funatsutakeshi/Desktop/tokken/summary/car_detection/sec/near3_focus.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, movSize);
    /* -------- define variables(end) -------- */


    /* ------- !main loop --------- */
    while(!glfwWindowShouldClose(window)) {
        /* ----- timer start ----- */
        auto start = system_clock::now();
        /* ----- timer start(end) ----- */

        /* ----- frame preparation ----- */
        glClear(GL_COLOR_BUFFER_BIT);
        capture >> imgShow;
        if(imgShow.empty()) break;
        flip(imgShow, imgShow, -1);
        imgProcCol = imgShow(focusCar.toRect());
        cvtColor(imgProcCol, imgProcGray, CV_BGR2GRAY);
        /* ----- frame preparation(end) ----- */

        /* ----- car detection ----- */
        newCar = carDetection(imgProcGray, cascade);
        if(newCar.width > 0) car = newCar;
        /* ----- car detection(end) -----*/

        /* ----- number-plate detection ----- */
        if(newCar.width > 0 && currentMode == MODE_PLATE) {
           imgCarROI = imgProcCol(car);
           plate = strEdgeDetect(imgCarROI);
        }
        /* ----- number-plate detection(end) ----- */

        /* ----- caribrate position ----- */
        if(newCar.width > 0) {
            showCar = car;
            showPlate = plate;
            showCar.x += focusCar.p1.x;
            showCar.y += focusCar.p1.y;
            showPlate.x += showCar.x;
            showPlate.y += showCar.y;
        }
        /* ----- caribrate position(end) ----- */

        /* ----- focus ----- */
        if(newCar.width > 0) focusCar.focus(showCar, frameWidth, frameHeight);
        focusCar.defocus(frameWidth, frameHeight);
        /* ----- focus(end) ----- */

        /* ----- draw rect ----- */
        if(newCar.width > 0) rectangle(imgShow, showCar, Scalar(0, 0, 255), 4);
        if(currentMode == MODE_PLATE) rectangle(imgShow, showPlate, Scalar(0, 255, 0), 4);
        rectangle(imgShow, focusCar.toRect(), Scalar(255, 0, 0), 4);
        /* ----- draw rect(end) ----- */

        /* ----- distance calculation ----- */
        size_t distance = calculateDistance(plate.height, car.width, currentMode);
        if(currentMode == MODE_PLATE && distance > 8) currentMode = MODE_CAR;
        if(currentMode == MODE_CAR && distance < 8) currentMode = MODE_PLATE;
        putText(imgShow, to_string(distance) + "[m]", Point(20, 70), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 3);
        /* ----- distance calculation(end) ----- */

        /* ----- show ----- */
        flip(imgShow, imgShow, 0);              // convert image
        cvtColor(imgShow, imgShow, CV_BGR2RGB); // into OpenGL format
        glDrawPixels(imgShow.cols, imgShow.rows, GL_RGB, GL_UNSIGNED_BYTE, imgShow.data);
        glFlush();
        glfwSwapBuffers(window);  // show buffer ←→ draw buffer
        glfwPollEvents();         // find event, and then go next
        /* ----- show(end) ----- */

        /* ----- timer end ----- */
        auto end = system_clock::now();
        auto dur = end - start;
        auto msec = duration_cast<milliseconds>(dur).count();
        cout << msec << "[ms]" << endl;
        /* ----- timer end(end) ----- */

        /* ----- write -----*/
        flip(imgShow, imgShow, 0);
        cvtColor(imgShow, imgShow, CV_BGR2RGB);
        movOut << imgShow;
        /*----- write(end) ----- */
    }/* ------- main loop(end) --------- */

    glfwDestroyWindow(window);
    return 0;
}



#pragma clang diagnostic pop

/* ---- template for speed evaluation
 auto start = system_clock::now();
 auto end = system_clock::now();
 auto dur = end - start;
 auto msec = duration_cast<chrono::milliseconds>(dur).count();
 cout << msec << "[ms]" << endl;
*/
