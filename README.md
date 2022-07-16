# HikvisionCCD-opencv
![Language](https://img.shields.io/badge/Language-c%2B%2B17-blue) ![OpenCV](https://img.shields.io/badge/OpenCV-4.5.5-blue) ![HikvisionSDK](https://img.shields.io/badge/HikvisionSDK-50660099-blue)

All you need to do is copy include folder to your program, then include yukihikccd.hpp. Finally target link MvCameraControl.lib when compile. Now you can use opencv to reveive Hikvision CCD streaming.

# Device
 - MV-CA050-10GM
![CCD](https://www.hikrobotics.com/cn2/source/vision/image/2022/7/13/20220713201025293.png)![CCD Back](https://www.hikrobotics.com/cn2/source/vision/image/2022/7/13/20220713201025419.png)

# Dependence
```
├─include
│  └─hikvision
│  └─yukihikccd.hpp
└─lib
   └─MvCameraControl.lib
```

# Supported Platforms
- Windows X64

# Usage
```c++
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
```

# Compile
- MSVC amd64
- Cmake