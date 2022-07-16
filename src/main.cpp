#include <opencv2/opencv.hpp>
#include "yukihikccd.hpp"

int main() {
    // 1. First instantiate YukiHikCCD class.
    ykhik::YukiHikCCD yukiHikCCD;
    // 2. Select CCD Devices index. (default 0)
    yukiHikCCD.SELECTED_CAM_NUM = 0;
    // 3. Open selected ccd device.
    yukiHikCCD.OPEN_CCD_DEVICE();

    cv::Mat reSizeFrame;
    cv::Size dsize = cv::Size(640, 640);
    while (true) {
        // 4. Format ccd images to opencv mat data.
        yukiHikCCD.HKCCD_TO_CV();
        // 5. Get formated opencv mat data.
        cv::resize(yukiHikCCD.CV_MAT, reSizeFrame, dsize);
        cv::imshow("hikccd", reSizeFrame);
        cv::waitKey(30);
    }

    return 0;
}