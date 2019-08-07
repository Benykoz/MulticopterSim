/*
   DVS simulator demo program

   Copyright(C) 2019 Simon D.Levy

   MIT License
*/

#include <opencv2/highgui/highgui.hpp>

#include "Davis346Sim.hpp"

static const uint16_t CAMERA_COLS = 346;
static const uint16_t CAMERA_ROWS = 260;

int main(int argc, char ** argv)
{
    while (true) {

        cv::Mat cameraImage = cv::Mat::zeros(CAMERA_ROWS, CAMERA_COLS, CV_8UC3);

        cv::imshow("EventCamera", cameraImage);

        if (cv::waitKey(1) == 27) break; // quit on ESC
    }

    return 0;
}


