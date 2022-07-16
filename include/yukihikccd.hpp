/**************************************************
Copyright(C), 2022, YukiSoftware. All rights reserved.
Author: Yuki(ZhangTuo)
Version: 1.0
**************************************************/
#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <conio.h>
#include "hikvision/MvCameraControl.h"

namespace ykhik {
class YukiHikCCD {
private:
int SDK_VERSION = 0;                                // Hikversion SDK version.
int ret = MV_OK;
unsigned int payload_size;
MV_CC_DEVICE_INFO_LIST hik_ccd_devices;             // Current device's Hikversion's devices list.
MVCC_INTVALUE hik_param;
MV_FRAME_OUT_INFO_EX hik_frame;                     // Hikversion ccd image.
void* CCD_HANDLE = nullptr;                         // Current selected ccd camera handler.
unsigned char* data;
void INIT_YUKIHIKCCD();
void CHECK_HIK_DEVICES_ACCESS();                    // Check Hikversion Devices available.
void RECEIVE_HIK_FRAME();                           // Receive stream from Hikversion ccd.

public:
unsigned int SELECTED_CAM_NUM = 0;                  // Selected ccd camera number from hik_ccd_devices_list.
cv::Mat CV_MAT;                                     // Hikversion ccd to opencv mat var.
YukiHikCCD();
~YukiHikCCD();
void OPEN_CCD_DEVICE();
void CLOSE_CCD_DEVICE();
void HKCCD_TO_CV();                                 // Format CCD image data to opencv mat data.
};


inline YukiHikCCD::YukiHikCCD() {
    INIT_YUKIHIKCCD();
    CHECK_HIK_DEVICES_ACCESS();
}


inline YukiHikCCD::~YukiHikCCD() {
    CLOSE_CCD_DEVICE();
}


inline void YukiHikCCD::INIT_YUKIHIKCCD() {
    // Get Hikvision SDK version.
    SDK_VERSION = MV_CC_GetSDKVersion();
    std::cout << "[SUCCESS]Hikversion_SDK_Version:" << SDK_VERSION << std::endl;

    memset(&hik_ccd_devices, 0, sizeof(MV_CC_DEVICE_INFO));
    ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &hik_ccd_devices);
    if (ret != MV_OK) {
        std::cout << "[WARING]Not Found Hikversion CCD Devices!!!" << std::endl;
    }
    std::cout << "[SUCCESS]Found Hikversion CCD Devices: " << hik_ccd_devices.nDeviceNum << std::endl;
}


inline void YukiHikCCD::CHECK_HIK_DEVICES_ACCESS() {
    if (hik_ccd_devices.nDeviceNum > 0) {
        for (unsigned int i = 0; i < hik_ccd_devices.nDeviceNum; i++) {
            std::cout << "[INFO]Hik Device: " << i << std::endl;
            MV_CC_DEVICE_INFO* pDeviceInfo = hik_ccd_devices.pDeviceInfo[i];

            bool access = MV_CC_IsDeviceAccessible(pDeviceInfo, 1);

            if (!access) {
                std::cout << "[ERROR]Not Access Device: " << i << std::endl;
            } else {
                std::cout << "[SUCCESS]Access Device: " << i << std::endl;
            }
        }
    } else {
        std::cout << "[ERROR]Check Devices Not Find Devices!!!" << std::endl;
    }
}


inline void YukiHikCCD::OPEN_CCD_DEVICE() {
    if (hik_ccd_devices.nDeviceNum == 0) {
        std::cout << "[ERROR]Open CCD Device Not Found Hikversion Devices!!!" << std::endl;
        return;
    } else if (SELECTED_CAM_NUM >= hik_ccd_devices.nDeviceNum) {
        std::cout << "[ERROR]Device index out range!!!" << std::endl;
        return;
    }

    // Create CCD Handle.
    ret = MV_CC_CreateHandle(&CCD_HANDLE, hik_ccd_devices.pDeviceInfo[SELECTED_CAM_NUM]);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Create CCD handle failed!!!" << std::endl;
        return;
    }

    // Open CCD Device.
    ret = MV_CC_OpenDevice(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Open CCD Device failed!!!" << std::endl;
        return;
    } else {
        std::cout << "[SUCCESS]Device Open. Device: " << SELECTED_CAM_NUM << std::endl;
    }

    // Set trigger mod off.
    ret = MV_CC_SetEnumValue(CCD_HANDLE, "TriggerMode", 0);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Set TriggerMod off failed!!!" << std::endl;
        return;
    }

    RECEIVE_HIK_FRAME();
}


inline void YukiHikCCD::RECEIVE_HIK_FRAME() {
    memset(&hik_param, 0, sizeof(MVCC_INTVALUE));
    ret = MV_CC_GetIntValue(CCD_HANDLE, "PayloadSize", &hik_param);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Get CCD Data packets failed!!!" << std::endl;
        return;
    }
    payload_size = hik_param.nCurValue;
    std::cout << "[INFO]CCD PayloadSize: " << payload_size << std::endl;


    ret = MV_CC_StartGrabbing(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Grab image failed!!!" << std::endl;
        return;
    }
    memset(&hik_frame, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    data = (unsigned char*)malloc(sizeof(unsigned char) * (payload_size));
    if (data == NULL) {
        return;
    }
}


inline void YukiHikCCD::HKCCD_TO_CV() {
    ret = MV_CC_GetOneFrameTimeout(CCD_HANDLE, data, payload_size, &hik_frame, 1000);
    if (ret != MV_OK) {
        free(data);
        data = NULL;
        return;
    }

    if (hik_frame.enPixelType == PixelType_Gvsp_Mono8) {
        CV_MAT = cv::Mat(hik_frame.nHeight, hik_frame.nWidth, CV_8UC1, data);
    }
    else if (hik_frame.enPixelType == PixelType_Gvsp_RGB8_Packed) {
        for (unsigned int j = 0; j < hik_frame.nHeight; j++) {
            for (unsigned int i = 0; i < hik_frame.nWidth; i++) {
                unsigned char red = data[j * (hik_frame.nWidth * 3) + i * 3];
                data[j * (hik_frame.nWidth * 3) + i * 3] = data[j * (hik_frame.nWidth * 3) + i * 3 + 2];
                data[j * (hik_frame.nWidth * 3) + i * 3 + 2] = red;
            }
        }
        CV_MAT = cv::Mat(hik_frame.nHeight, hik_frame.nWidth, CV_8UC3, data);
    }
    else {
        return;
    }

    if (CV_MAT.data == NULL) {
        return;
    }
}


inline void YukiHikCCD::CLOSE_CCD_DEVICE() {
    ret = MV_CC_CloseDevice(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Destory CCD handle failed!!!" << std::endl;
        return;
    }

    ret = MV_CC_DestroyHandle(CCD_HANDLE);
    if (ret != MV_OK) {
        std::cout << "[ERROR]Close CCD handle failed!!!" << std::endl;
        return;
    }
};
}