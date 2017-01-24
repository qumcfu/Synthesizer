#ifndef DETECTION_H
#define DETECTION_H

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

class Detection
{
public:
    Detection();

    /** Function Headers */
    void detectAndDisplay(cv::Mat frame);
    void init();
    bool getEyesOpen();
    bool hasDetectionError();

    /** Global variables */
    String face_cascade_name = "/Users/Arne/Desktop/Media Systems/AVPRG/Synthesizer/haarcascade_frontalface_alt.xml";
    String eyes_cascade_name = "/Users/Arne/Desktop/Media Systems/AVPRG/Synthesizer/haarcascade_eye_tree_eyeglasses.xml";
    String eyes_cascade_right_name = "/Users/Arne/Desktop/Media Systems/AVPRG/Synthesizer/haarcascade_righteye_2splits.xml";
    String eyes_cascade_left_name = "/Users/Arne/Desktop/Media Systems/AVPRG/Synthesizer/haarcascade_lefteye_2splits.xml";
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    CascadeClassifier eyes_cascade_right;
    CascadeClassifier eyes_cascade_left;

private:
    String window_name;
    double cap_win_width;
    double cap_win_height;
    bool eyes_open;
    bool timer_started;
    // auto start_time = 0;
    std::chrono::steady_clock::time_point start_time;
    // auto current_time = 0;
    float delay_time;
    bool eye_blink_detection_enabled;
    VideoCapture capture;
    Mat frame;
    bool detectionError;
    unsigned int ary_index;
    unsigned int min_detection_val;
    bool eye_detection_average_ary[5];
};

#endif // DETECTION_H
