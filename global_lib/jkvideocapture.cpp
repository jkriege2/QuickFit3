#include "jkvideocapture.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "tools.h"



#define capFileSaveDIBA(hwnd, szName)                ((BOOL) __capSendMessage(hwnd,WM_CAP_FILE_SAVEDIBA,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
/*
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

// Handle to a DIB
DECLARE_HANDLE(HDIB);

HDIB ReadDIBFile(char* filename)
{

	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HDIB hDIB;
	LPSTR pDIB;



	// get length of DIB in bytes for use when reading
	dwBitsSize = get_filesize(filename);
	std::cout<<"filesize="<<dwBitsSize<<std::endl;

    // open file
    FILE* f=fopen(filename, "r");


	// Go read the DIB file header and check if it's valid.
	if (fread(&bmfHeader, sizeof(bmfHeader), 1, f) != sizeof(bmfHeader)) {
	    fclose(f);
	    return NULL;
	}
	std::cout<<"read header"<<std::endl;

	if (bmfHeader.bfType != DIB_HEADER_MARKER) {
	    fclose(f);
		return NULL;
	}

	// Allocate memory for DIB
	hDIB = (HDIB) GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
        fclose(f);
		return NULL;
	}
	pDIB = (LPSTR) GlobalLock((HGLOBAL) hDIB);
	std::cout<<"got memory for DIB size="<<dwBitsSize<<std::endl;

	// Go read the bits.
	if (fread(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER), 1, f) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		GlobalUnlock((HGLOBAL) hDIB);
		GlobalFree((HGLOBAL) hDIB);
		fclose(f);
		return NULL;
	}
	std::cout<<"read DIB"<<std::endl;
	GlobalUnlock((HGLOBAL) hDIB);
	fclose(f);
	return hDIB;
}
*/

jkVideoCapture::jkVideoCapture(HWND winHandle) {
    hardwareOverlay=true;
    connected=false;
    useCallback=false;
    intimage.assign(320,200,1,3,1);


    hwndVideo = capCreateCaptureWindowA(
                    (LPSTR) "Capture Preview Window",
                    WS_POPUPWINDOW,
                    0, 0, 100,100,//Rect.right-Rect.left, Rect.bottom-Rect.top,
                    winHandle, //(HWND) Handle,
                    (int) 0);
    //std::cout<<"capCreateCaptureWindow:   "<<win32_error()<<std::endl;

    //std::cout<<"capSetCallbackOnFrame: "<<capSetCallbackOnFrame(hwndVideo, frame_callback_cimg)<<"    "<<win32_error()<<std::endl;
    //SendMessage(hwndVideo,WM_CAP_SET_CALLBACK_FRAME,0,(LPARAM)(LPVOID)(frame_callback_cimg));
    capOverlay(hwndVideo, hardwareOverlay);
    //std::cout<<"capOverlay:   "<<win32_error()<<std::endl;
    enumDevices();
    framerate=15;
    SelectedDevice = -1;
}

jkVideoCapture::~jkVideoCapture() {

    if (connected) {
        capDriverConnect(hwndVideo, SelectedDevice);
        capDriverDisconnect(hwndVideo); // disconnect from driver
    }

}

bool jkVideoCapture::setUseHardwareOverlay(bool use) {
    hardwareOverlay=use;
    capOverlay(hwndVideo, hardwareOverlay);
    return true;
};


std::string jkVideoCapture::win32_error() {
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


void jkVideoCapture::enumDevices() {
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

void jkVideoCapture::disconnect() {
    if (connected) capDriverDisconnect(hwndVideo);
    connected=false;
}

void jkVideoCapture::connect(int device) {
    CAPSTATUS CapStatus;
    int       hsize;

    // connect to the driver
    if (SelectedDevice != -1) {
        capDriverConnect(hwndVideo, SelectedDevice);
    }

    if (!capDriverConnect(hwndVideo, device)) {
        // ---- Unable to connect to driver
        std::cout<<"unable to connect to driver\n";
        return;
    }
    std::cout<<"capDriverConnect:   "<<win32_error()<<std::endl;
    if (useCallback) std::cout<<"capSetCallbackOnFrame: "<<capSetCallbackOnFrame(hwndVideo, frame_callback_cimg)<<"    "<<win32_error()<<std::endl;


    // update the driver capabilities
    capDriverGetCaps (hwndVideo, sizeof(CAPDRIVERCAPS), &CapDrvCaps);

    //capSetCallbackOnFrame (hwndVideo, cap_CallbackProc);

    //capDlgVideoFormat(ParentHandle);

    // Are there new image dimensions
    capGetStatus(hwndVideo, &CapStatus, sizeof(CAPSTATUS));
    //HDC  hdc = GetDC (hwndVideo);

    hsize = GetSystemMetrics(SM_CYMENU);
    hsize += GetSystemMetrics(SM_CYCAPTION);

    // ---- Rescaling the windows
    SetWindowPos(hwndVideo, NULL, 0, 0, CapStatus.uiImageWidth,  CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE);
    //std::cout<<"SetWindowPos:   "<<win32_error()<<std::endl;

    Height = CapStatus.uiImageHeight;
    Width  = CapStatus.uiImageWidth;

    // ---- Get extended information on the capture device
    capCaptureGetSetup (hwndVideo, &CapParms, sizeof (CapParms));

    CapParms.dwRequestMicroSecPerFrame = (int)round(1000000.0/framerate);
    CapParms.fYield = FALSE;
    CapParms.fCaptureAudio = FALSE;

    capCaptureSetSetup (hwndVideo, &CapParms, sizeof (CapParms));
    std::cout<<"capCaptureStop"<<capCaptureStop(hwndVideo)<<std::endl;
    if (useCallback) std::cout<<"capSetCallbackOnFrame: "<<capSetCallbackOnFrame(hwndVideo, frame_callback_cimg)<<"    "<<win32_error()<<std::endl;

    // ---- Remember selected device
    SelectedDevice = device;
    connected=true;
}

void jkVideoCapture::setFramerate(double rate) {
    framerate=rate;

    capCaptureGetSetup (hwndVideo, &CapParms, sizeof (CapParms));
    CapParms.dwRequestMicroSecPerFrame = (int)round(1000000.0/framerate);
    capCaptureSetSetup (hwndVideo, &CapParms, sizeof (CapParms));
};

void jkVideoCapture::readDeviceProperties() {
    CAPSTATUS CapStatus;
    // ---- Get extended information on the capture device
    capCaptureGetSetup (hwndVideo, &CapParms, sizeof (CapParms));
    framerate=1000000.0/(double)CapParms.dwRequestMicroSecPerFrame;

    capGetStatus(hwndVideo, &CapStatus, sizeof(CAPSTATUS));
    Height = CapStatus.uiImageHeight;
    Width  = CapStatus.uiImageWidth;

}

void jkVideoCapture::displayFormatCfg() {
    int       hsize;

    CAPSTATUS CapStatus;

    capDlgVideoFormat(hwndVideo);

    // Are there new image dimensions
    capGetStatus(hwndVideo, &CapStatus, sizeof(CAPSTATUS));

    hsize = GetSystemMetrics(SM_CYMENU);
    hsize += GetSystemMetrics(SM_CYCAPTION);

    //SetWindowPos(ParentHandle, NULL, 0, hsize, CapStatus.uiImageWidth, CapStatus.uiImageHeight+hsize, SWP_NOZORDER | SWP_NOMOVE);
    SetWindowPos(hwndVideo, NULL, 0, 0, CapStatus.uiImageWidth, CapStatus.uiImageHeight, SWP_NOZORDER | SWP_NOMOVE);
    readDeviceProperties();
}

void jkVideoCapture::displayDeviceCfg() {
    capDlgVideoSource(hwndVideo);
    readDeviceProperties();
}

// this LOCAL function is used as a callback for the winAPI which is called every
// time a frame has been grabbed and wich copies the frame data into an CImg
// object pointed to by the Cap User Data
LRESULT PASCAL jkVideoCapture::frame_callback_cimg(HWND window, LPVIDEOHDR video)
{
    std::cout<<"in cimg callback\n";
    //MessageBoxA(window, (LPSTR) "in cimg callback", (LPSTR) "in cimg callback",MB_OK);

    FILE* f=fopen("c:\test.txt", "w");
    fprintf(f, "in cimg callback");
    fclose(f);

    cimg_library::CImg<unsigned char>* img;

    if(!window)
        return FALSE;

    img = (cimg_library::CImg<unsigned char>*) capGetUserData(window);
    unsigned char* frame = video->lpData;

    int linesize=img->width()*img->spectrum();

    for(register int y = 0; y < img->height(); y++) {
        for (register int x=0; x<img->width(); x++) {
            img->operator()(x, img->height()-y, 0, 2)=frame[y*linesize+x*3];
            img->operator()(x, img->height()-y, 0, 1)=frame[y*linesize+x*3+1];
            img->operator()(x, img->height()-y, 0, 0)=frame[y*linesize+x*3+2];
        }
    }
    return TRUE;
}



bool jkVideoCapture::captureSingleFrame(cimg_library::CImg<unsigned char> &image) {

    BITMAPINFO bmi;

    readDeviceProperties();

    bmi.bmiHeader.biSize          = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth         = Width;
    bmi.bmiHeader.biHeight        = Height;
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = 24;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed       = 0;
    bmi.bmiHeader.biClrImportant  = 0;

    // try to set video format to the current width/height and 24BIt RGB (=3*8Bit)
    capSetVideoFormat(hwndVideo, &bmi, sizeof(BITMAPINFO));


    // read actual video fromat settings
    capGetVideoFormat(hwndVideo, &bmi, sizeof(BITMAPINFO));

    // we init the image with the right format
    image.assign(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 1, 3, 0);

    // use the video stream user data to provide a pointer to the CImg object
    // which will afterwards hold the frame
    capSetUserData(hwndVideo, &intimage);

    // now register callback function which writes the frame into the CImg
    bool res=false;
    if (useCallback) {
        if (gotimage) {
            std::cout<<"set callback: "<<capSetCallbackOnFrame(hwndVideo, frame_callback_cimg)<<std::endl;
            intimage.assign(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 1, 3, 0);
            res=capGrabFrameNoStop(hwndVideo);
            //std::cout<<"callback called:   "<<res<<"  "<<win32_error()<<std::endl;
        } else {
            image=intimage;
            //std::cout<<"copy image:   "<<win32_error()<<std::endl;
            res=true;
        }
        gotimage=!gotimage;
    } else {
        res=capGrabFrameNoStop(hwndVideo);
        char* fn="temp_cap.bmp";
        capFileSaveDIBA(hwndVideo, fn);
        image.load_bmp(fn);
    }


    return res;
}




/*
// this LOCAL function is used as a callback for the winAPI which is called every
// time a frame has been grabbed and wich copies the frame data into an CImg
// object pointed to by the Cap User Data
static LRESULT PASCAL frame_callback_carray(HWND window, LPVIDEOHDR video)
{
    std::cout<<"in c array callback\n";

    unsigned char* img;

    if(!window)
        return FALSE;

    img = (unsigned char*) capGetUserData(window);
    unsigned char* frame = video->lpData;

    BITMAPINFO bmi;
    capGetVideoFormat(window, &bmi, sizeof(BITMAPINFO));


    int linesize=bmi.bmiHeader.biWidth*bmi.bmiHeader.biBitCount/8;
    int width=bmi.bmiHeader.biWidth;
    int height=bmi.bmiHeader.biHeight;
    size_t channelsize=bmi.bmiHeader.biWidth*bmi.bmiHeader.biHeight;
    //std::cout<<"width="<<bmi.bmiHeader.biWidth<<"  height="<<bmi.bmiHeader.biHeight<<"  bitCount="<<bmi.bmiHeader.biBitCount<<"  linesize="<<linesize<<"  channelsize="<<channelsize<<std::endl;

    if (bmi.bmiHeader.biBitCount==24) {
        for(register int y = 0; y<height; y++) {
            for (register int x=0; x<width; x++) {
                img[y*width+2*channelsize+x]=frame[y*linesize+x*3];
                img[y*width+1*channelsize+x]=frame[y*linesize+x*3+1];
                img[y*width+0*channelsize+x]=frame[y*linesize+x*3+2];
            }
        }
    } else if (bmi.bmiHeader.biBitCount==8) {
        for(register int y = 0; y<height; y++) {
            for (register int x=0; x<width; x++) {
                img[y*width+2*channelsize+x]=frame[y*linesize+x*3];
                img[y*width+1*channelsize+x]=0;
                img[y*width+0*channelsize+x]=0;
            }
        }
    }
    return TRUE;
}



bool jkVideoCapture::captureSingleFrame(unsigned char* image) {

    BITMAPINFO bmi;

    readDeviceProperties();

    bmi.bmiHeader.biSize          = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth         = Width;
    bmi.bmiHeader.biHeight        = Height;
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = 24;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed       = 0;
    bmi.bmiHeader.biClrImportant  = 0;

    // try to set video format to the current width/height and 24BIt RGB (=3*8Bit)
    capSetVideoFormat(hwndVideo, &bmi, sizeof(BITMAPINFO));

    // read actual video fromat settings
    capGetVideoFormat(hwndVideo, &bmi, sizeof(BITMAPINFO));

    // use the video stream user data to provide a pointer to the CImg object
    // which will afterwards hold the frame
    capSetUserData(hwndVideo, image);

    // now register callback function which writes the frame into the CImg
    if (useCallback) capSetCallbackOnFrame(hwndVideo, frame_callback_carray);



    bool res=capGrabFrameNoStop(hwndVideo);

    if (!useCallback) {
        char* fn="temp_cap.bmp";
        capFileSaveDIBA(hwndVideo, fn);


    }

    return res;
}*/
