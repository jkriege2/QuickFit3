

/** \file jkvideocapture.h
 *  \ingroup tools_video
 *
 * This may be used to capture a video on WIndows systems, using the Video for Windows API.
 */

#ifndef JKVIDEOCAPTURE_H
#define JKVIDEOCAPTURE_H

#include <vfw.h> // video for windows library
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <CImg.h>

/* link against:
gdi32
user32
kernel32
vfw32
*/


/*! \brief Class that allows to capture a video from a connected device, using Video for Windows.
    \ingroup tools_video

    This class uses the <a href="http://cimg.sourceforge.net/">CImg</a> library for image manipulation and the
    <a href="http://msdn.microsoft.com/en-us/library/ms713492(VS.85).aspx">Video for Windows</a> Interface to
    capture the video!

    If you use this you will have to link against these libraries:
      - vfw32
      - gdi32
    .


    This class is partially based on code by <a href="http://www.opengl.allanpetersen.com/?q=node/13">Allan Peterson</a>.
*/
class jkVideoCapture
{
    protected:

        /** \brief a list of the available devices */
        std::vector<std::string> devices;

        /** \brief the <a href="http://msdn.microsoft.com/en-us/library/ms707173(VS.85).aspx">driver capabilities</a> */
        CAPDRIVERCAPS   CapDrvCaps;

        /** \brief the <a href="http://msdn.microsoft.com/en-us/library/ms707267(VS.85).aspx">capture parameters</a> */
        CAPTUREPARMS    CapParms;

        /** \brief handle to access the VfW API */
        HWND hwndVideo;

        /** \brief which device is currently selected? */
        int     SelectedDevice;

        /** \brief width of the video input */
        int     Height;

        /** \brief height of the video input */
        int     Width;

        /** \brief the frame rate in [fps] */
        double framerate;

        /** \brief enable/disable <a href="http://msdn.microsoft.com/en-us/library/ms713815(VS.85).aspx">Hardware Overlay mode</a> */
        bool hardwareOverlay;

        /** \brief indicates whether we are connected to a data source */
        bool connected;

        /** \brief indicates whether to use the WinAPI callback mechanism (\c true) to extract the current
         *         frame data, or whether to store the frame in a DIB file (\c false, default) which is subsequently
         *         read into a memory array */
        bool useCallback;

        static LRESULT PASCAL frame_callback_cimg(HWND window, LPVIDEOHDR video);
        cimg_library::CImg<unsigned char> intimage;
    private:
    public:
        /** Default constructor */
        jkVideoCapture(HWND winHandle=(HWND)HWND_DESKTOP);

        /** Default destructor */
        virtual ~jkVideoCapture();

        /** \brief return the window handle of the capture window */
        inline HWND getHWND() { return hwndVideo; };

        /** \brief read the device properties */
        void readDeviceProperties();

        /** \brief get a list of the available devices */
        inline std::vector<std::string> getDeviceList() { return devices; };

        /** \brief get the number of available devices */
        inline size_t getDeviceCount() { return devices.size(); };

        /** \brief get the currently selected device */
        inline int getCurrentDevice() { return SelectedDevice; };

        /** \brief get the current framerate in frmaes per second [fps] */
        inline double getFramerate() { return framerate; };

        /** \brief set the current framerate in frmaes per second [fps] */
        void setFramerate(double rate);

        /** \brief get the name of the currently selected device */
        inline std::string getCurrentDeviceName() {
            if (SelectedDevice>=0)
                return devices[SelectedDevice];
            else
                return "";
        };

        /** \brief enumerate the available devices (fill devices vector) */
        void enumDevices();

        /** \brief connect to the specified device driver */
        void connect(int device);

        /** \brief disconnect from device */
        void disconnect();

        /** \brief display the capture format toolbox */
        void displayFormatCfg();

        /** \brief  display device config toolbox */
        void displayDeviceCfg();

        /** \brief get last windows error message */
        std::string win32_error();

        /** \brief get the hardware overlay status */
        inline bool getUseHardwareOverlay() {
            return hardwareOverlay;
        };

        /** \brief get the hardware overlay status */
        bool setUseHardwareOverlay(bool use=true);

        /** \brief get width of video input [pixels] */
        inline int getVideoWidth() { return Width; };

        /** \brief get height of video input [pixels] */
        inline int getVideoHeight() { return Height; };

        /** \brief capture a single frame and store it into the given CImg object */
        bool captureSingleFrame(cimg_library::CImg<unsigned char> &image);


        //bool captureSingleFrame(unsigned char* image);

        /** \brief indicates whether to use the WinAPI callback mechanism (\c true) to extract the current
         *         frame data, or whether to store the frame in a DIB file (\c false, default) which is subsequently
         *         read into a memory array */
        inline void setUseCallback(bool use) { useCallback=use; };



        bool gotimage;
};

#endif // JKVIDEOCAPTURE_H
