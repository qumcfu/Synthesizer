#include "detection.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <vector>
#include <iostream>
#include <stdio.h>
#include <chrono>

using namespace std;
using namespace cv;

Detection::Detection(){

}

void Detection::init(){
    window_name = "Capture - Face detection";
    cap_win_width = 240;
    cap_win_height = 160;
    eyes_open = false;
    timer_started = false;
    delay_time = 0.75;
    eye_blink_detection_enabled = false;
    detectionError = false;

    // load data
    if(!face_cascade.load(face_cascade_name)) {
        printf("Error loading face cascade\n");
        detectionError = true;
    }

    if (!detectionError){
        if(!eyes_cascade.load(eyes_cascade_name)) {
            printf("Error loading eyes cascade\n");
            detectionError = true;
        }
        if(!eyes_cascade_right.load(eyes_cascade_right_name)) {
            printf("Error loading eyes cascade right");
            detectionError = true;
        }

        if(!eyes_cascade_left.load(eyes_cascade_left_name)) {
            printf("Error loading eyes cascade left");
            detectionError = true;
        }
    }

    if (!detectionError){
        // grap webcam output and set frame size
        capture.open(0);
        capture.set(CV_CAP_PROP_FRAME_WIDTH, cap_win_width);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, cap_win_height);

        if(!capture.isOpened()) {
            printf("Error opening video capture\n");
            detectionError = true;
        }
    }

    if (!detectionError){
        while(capture.read(frame)) {
            if(frame.empty()) {
                printf("No captured frame -- Break!");
                break;
            }

            detectAndDisplay(frame);


            // escape
            int cancelKey = waitKey(7);
            if((char) cancelKey == 27) {
                break;
            }
        }
    }
}

void Detection::detectAndDisplay(Mat frame) {
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    // Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));



    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    // std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();

    // delay for eye blink detection
    if(!timer_started) {
        // start_time = std::chrono::high_resolution_clock::now();
        start_time = std::chrono::steady_clock::now();
        eye_blink_detection_enabled = false;
        timer_started = true;

    } else {
        std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count() > delay_time) {
            timer_started = false;
            eye_blink_detection_enabled = true;
        }
    }




    for(size_t i = 0; i < faces.size(); i++) {
        cv::rectangle(frame, faces[i], CV_RGB(0, 255,0), 1);

        //Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
        //ellipse(frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);

        Mat faceROI = frame_gray(faces[i]);
        // std::vector<Rect> mouths;



        // Detect eyes
        std::vector<Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30));

        if(eye_blink_detection_enabled) {
            if(eyes.size() > 0) {
                eyes_open = true;
            } else {
                eyes_open = false;
            }
        //    qDebug()<<eyes_open;
        }


//        for(size_t j = 0; j < eyes.size(); j++) {
//            Point eye_center(faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2);
//            int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
//            circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
//            //cv::rectangle(frame, eyes[j], CV_RGB(0, 255,0), 1);
//        }

        // detect right eye
        std::vector<Rect> eyes_right;
        eyes_cascade_right.detectMultiScale(faceROI, eyes_right, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(20, 20));

        for(size_t j = 0; j < eyes_right.size(); j++) {
            Point eye_center(faces[i].x + eyes_right[j].x + eyes_right[j].width/2, faces[i].y + eyes_right[j].y + eyes_right[j].height/2);
            int radius = cvRound((eyes_right[j].width + eyes_right[j].height)*0.25);
            circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
        }

        // detect left eye
        std::vector<Rect> eyes_left;
        eyes_cascade_right.detectMultiScale(faceROI, eyes_left, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(20, 20));

        for(size_t j = 0; j < eyes_left.size(); j++) {
            Point eye_center(faces[i].x + eyes_left[j].x + eyes_left[j].width/2, faces[i].y + eyes_left[j].y + eyes_left[j].height/2);
            int radius = cvRound((eyes_left[j].width + eyes_left[j].height)*0.25);
            circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
        }


    }

    imshow(window_name, frame);
}

bool Detection::getEyesOpen(){
    return eyes_open;
}

