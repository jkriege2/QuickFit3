/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "videocapture.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <vfw.h> // video for windows library
#include "image_tools.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tools.h"
#include <CImg.h>









#ifndef __capSendMessage
#define     __capSendMessage(hWnd, uMsg, wParm, lParam) ((IsWindow(hWnd)) ? SendMessage(hWnd, uMsg, (WPARAM)(wParm), (LPARAM)(lParam)) : 0)
#endif
#define capFileSaveDIBA(hwnd, szName)                ((BOOL) __capSendMessage(hwnd,WM_CAP_FILE_SAVEDIBA,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))


class VideoCapture_private {
    public:

        /** \brief a list of the available devices */
        std::vector<std::string> devices;

        /** \brief the <a href="http://msdn.microsoft.com/en-us/library/ms707173(VS.85).aspx">driver capabilities</a> */
        CAPDRIVERCAPS   CapDrvCaps;

        /** \brief the <a href="http://msdn.microsoft.com/en-us/library/ms707267(VS.85).aspx">capture parameters</a> */
        CAPTUREPARMS    CapParms;

        /** \brief handle to access the VfW API */
        HWND hwndVideo;

        /** \brief enable/disable <a href="http://msdn.microsoft.com/en-us/library/ms713815(VS.85).aspx">Hardware Overlay mode</a> */
        bool hardwareOverlay;

        /** \brief indicates whether we are connected to a data source */
        bool connected;

        std::string lastError;

        static LRESULT PASCAL frame_callback_cimg(HWND window, LPVIDEOHDR video);

        bool setUseHardwareOverlay(bool use) {
            hardwareOverlay=use;
            capOverlay(hwndVideo, hardwareOverlay);
            return true;
        }

        std::string win32_error() {
            /* return the windows error message for an error code:
              taken and modified from http://msdn2.microsoft.com/en-us/library/ms701797.aspx */
            char pMsgBuf[4096] ;
            DWORD dwErrorCode=GetLastError();

            // Retrieve the Win32 error message.
            DWORD dwSize = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                   NULL, dwErrorCode, 0, (LPTSTR) pMsgBuf, 0, NULL);

            std::cout<<"[size="<<dwSize<<"]";
            std::string res;

            // Return the description and size to the caller in the OUT parameters.
            if (dwSize>0)
            {
                  char err[1024];
                  sprintf(err, "errorcode %d: %s", (int)dwErrorCode, pMsgBuf);
                  res=err;
                  LocalFree(pMsgBuf);
            } else {
              char err[1024];
              sprintf(err, "errorcode %d: sorry, no description available", (int)dwErrorCode);
              res=err;
            }
            return res;
        }

        void setFramerate(double rate) {
            capCaptureGetSetup (hwndVideo, &CapParms, sizeof (CapParms));
            CapParms.dwRequestMicroSecPerFrame = (int)round(1000000.0/rate);
            capCaptureSetSetup (hwndVideo, &CapParms, sizeof (CapParms));
        };

        void readDeviceProperties(int & Width, int& Height, double& framerate) {
            CAPSTATUS CapStatus;
            // ---- Get extended information on the capture device
            capCaptureGetSetup (hwndVideo, &CapParms, sizeof (CapParms));
            framerate=1000000.0/(double)CapParms.dwRequestMicroSecPerFrame;

            capGetStatus(hwndVideo, &CapStatus, sizeof(CAPSTATUS));
            Height = CapStatus.uiImageHeight;
            Width  = CapStatus.uiImageWidth;

        }

};






VideoCapture::VideoCapture() {
    d=new VideoCapture_private();
    d->hardwareOverlay=false;
    d->connected=false;

    d->hwndVideo = capCreateCaptureWindowA(
                    (LPSTR) "Capture Preview Window",
                    WS_POPUPWINDOW,
                    0, 0, 100,100,//Rect.right-Rect.left, Rect.bottom-Rect.top,
                    (HWND)HWND_DESKTOP, //(HWND) Handle,
                    (int) 0);
    capOverlay(d->hwndVideo, d->hardwareOverlay);
    d->setFramerate(15);

    enumDevices();
}

VideoCapture::~VideoCapture() {
    if (d->connected) {
        capDriverConnect(d->hwndVideo, selectedDevice);
        capDriverDisconnect(d->hwndVideo); // disconnect from driver
    }
    delete d;
}

std::string VideoCapture::getLastError() const  {
    return d->lastError;
}

void VideoCapture::enumDevices() {
    char szDeviceName[80]; // driver name
    char szDeviceVersion[80]; // driver version
    char str[161]; // concatinated string
    int xcap; // counter

    xcap = 0;
    devices.clear ();
    for (;;)  {
        if (capGetDriverDescriptionA(xcap, szDeviceName, sizeof(szDeviceName),
                                    szDeviceVersion, sizeof(szDeviceVersion))) {

            sprintf (str, "%s, %s", szDeviceName, szDeviceVersion);
            devices.push_back(str);
        } else {
            break;
        }
        xcap++;
    };

}

bool VideoCapture::connect(int device) {
    CAPSTATUS CapStatus;
    int       hsize;

    // connect to the driver
    if (selectedDevice != -1) {
        capDriverConnect(d->hwndVideo, selectedDevice);
    }

    if (!capDriverConnect(d->hwndVideo, device)) {
        // ---- Unable to connect to driver
        std::cout<<"unable to connect to driver\n";
        return false;
    }
    std::cout<<"capDriverConnect:   "<<d->win32_error()<<std::endl;


    // update the driver capabilities
    capDriverGetCaps(d->hwndVideo, sizeof(CAPDRIVERCAPS), &(d->CapDrvCaps));

    //capSetCallbackOnFrame (hwndVideo, cap_CallbackProc);

    //capDlgVideoFormat(ParentHandle);

    // Are there new image dimensions
    capGetStatus(d->hwndVideo, &CapStatus, sizeof(CAPSTATUS));
    //HDC  hdc = GetDC (hwndVideo);

    hsize = GetSystemMetrics(SM_CYMENU);
    hsize += GetSystemMetrics(SM_CYCAPTION);

    // ---- Rescaling the windows
    SetWindowPos(d->hwndVideo, NULL, 0, 0, CapStatus.uiImageWidth,  CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE);
    //std::cout<<"SetWindowPos:   "<<win32_error()<<std::endl;

    height = CapStatus.uiImageHeight;
    width  = CapStatus.uiImageWidth;

    // ---- Get extended information on the capture device
    capCaptureGetSetup (d->hwndVideo, &(d->CapParms), sizeof(d->CapParms));

    d->CapParms.dwRequestMicroSecPerFrame = (int)round(1000000.0/15);
    d->CapParms.fYield = FALSE;
    d->CapParms.fCaptureAudio = FALSE;

    capCaptureSetSetup (d->hwndVideo, &(d->CapParms), sizeof (d->CapParms));
    std::cout<<"capCaptureStop"<<capCaptureStop(d->hwndVideo)<<std::endl;

    d->setFramerate(30);

    // ---- Remember selected device
    selectedDevice = device;
    d->connected=true;

    return d->connected;
}

void VideoCapture::disconnect() {
    if (d->connected) capDriverDisconnect(d->hwndVideo);
    d->connected=false;
}

bool VideoCapture::isConnected() const {
    return d->connected;
}

bool VideoCapture::captureFrame(unsigned char* image) {

    BITMAPINFO bmi;

    double framerate;
    d->readDeviceProperties(width, height, framerate);
    exposureTime=1/framerate;

    bmi.bmiHeader.biSize          = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth         = width;
    bmi.bmiHeader.biHeight        = height;
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = 24;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed       = 0;
    bmi.bmiHeader.biClrImportant  = 0;

    // try to set video format to the current width/height and 24BIt RGB (=3*8Bit)
    capSetVideoFormat(d->hwndVideo, &bmi, sizeof(BITMAPINFO));


    // read actual video fromat settings
    capGetVideoFormat(d->hwndVideo, &bmi, sizeof(BITMAPINFO));

    // we init the image with the right format
    cimg_library::CImg<unsigned char> imagei;
    imagei.assign(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 1, 3, 0);


    // now register callback function which writes the frame into the CImg
    bool res=false;

    res=capGrabFrameNoStop(d->hwndVideo);
    const char fn[]="temp_cap.bmp";
    capFileSaveDIBA(d->hwndVideo, fn);
    imagei.load_bmp(fn);

    for (int c=0; c<3; c++) {
        for (int y=0; y<bmi.bmiHeader.biHeight; y++) {
            for (int x=0; x<bmi.bmiHeader.biWidth; x++) {
                image[y*bmi.bmiHeader.biWidth*3+x*3+c]=imagei(x, y, c);
            }
        }
    }


    return res;
}



int VideoCapture::getConfigDialogCount() const {
    return 2;
}

std::string VideoCapture::getConfigDialogName(int dlg) const {
    if (dlg==0) return std::string("video format dialog");
    if (dlg==1) return std::string("video source dialog");
    return std::string("");
}

void VideoCapture::showConfigDialog(int dlg) {
    if (dlg==0) {
        int       hsize;

         CAPSTATUS CapStatus;

         capDlgVideoFormat(d->hwndVideo);

         // Are there new image dimensions
         capGetStatus(d->hwndVideo, &CapStatus, sizeof(CAPSTATUS));

         hsize = GetSystemMetrics(SM_CYMENU);
         hsize += GetSystemMetrics(SM_CYCAPTION);

         //SetWindowPos(ParentHandle, NULL, 0, hsize, CapStatus.uiImageWidth, CapStatus.uiImageHeight+hsize, SWP_NOZORDER | SWP_NOMOVE);
         SetWindowPos(d->hwndVideo, NULL, 0, 0, CapStatus.uiImageWidth, CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE);
         double framerate;
         d->readDeviceProperties(width, height, framerate);
         exposureTime=1/framerate;
    } else if (dlg==1) {
         capDlgVideoSource(d->hwndVideo);
         double framerate;
         d->readDeviceProperties(width, height, framerate);
         exposureTime=1/framerate;
    }
}


bool VideoCapture::supportsUserProperty(int prop) const {
    return false;
}

void VideoCapture::getUserPropertyRange(int prop, int& min, int& max) const {
    min=max=0;
}

int VideoCapture::getUserProperty(int prop) {
    return 0;
}


void VideoCapture::setUserProperty(int prop, int value) {
}

int VideoCapture::getAdditionalUserPropertyCount() const {
    return 0;
}

bool VideoCapture::isUserPropertyEnabled(int prop) {
    return false;
}

VideoCapture::UserPropertieType VideoCapture::getUserPropertyType(int prop) const {
    switch(prop) {
        case upBrightness:
        case upContrast:
        case upSaturation:
        case upHue:
        case upRedBalance:
        case upBlueBalance:
        case upGamma:
        case upExposure:
        case upGain:
        case upSharpness:
        case upBacklightCompensation:
            return VideoCapture::uptInteger;

        case upPowerlineFilter:
            return VideoCapture::uptMenu;

        case upAutoWhitebalance:
        case upAutogain:
            return VideoCapture::uptBoolean;
    }
    return VideoCapture::uptNone;
}

std::string VideoCapture::getUserPropertyName(int prop) const {
    switch(prop) {
        case upBrightness: return std::string("brigthness:");
        case upContrast: return std::string("contrast:");
        case upSaturation: return std::string("saturation:");
        case upHue: return std::string("hue:");
        case upAutoWhitebalance: return std::string("auto whitebalance:");
        case upRedBalance: return std::string("red balance:");
        case upBlueBalance: return std::string("blue balance:");
        case upGamma: return std::string("gamma:");
        case upExposure: return std::string("exposure:");
        case upAutogain: return std::string("autogain:");
        case upGain: return std::string("gain:");
        case upPowerlineFilter: return std::string("powerline filter:");
        case upSharpness: return std::string("sharpness:");
        case upBacklightCompensation: return std::string("backlight compensation:");
    }
    return std::string("");
}

std::vector<std::string> VideoCapture::getUserPropertyMenuItems(int prop) const {
    std::vector<std::string> l;
    if (prop==upPowerlineFilter) {
        l.push_back("disabled");
        l.push_back("50Hz");
        l.push_back("60Hz");
    }
    return l;
}
