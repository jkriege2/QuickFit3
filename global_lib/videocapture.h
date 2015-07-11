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

/**
 * \defgroup tools_video Video Capture Classes
 * \ingroup tools
 *
 * This may be used to capture a video on Windows (vfw) or Linux (v4l2) systems, using the Video for Windows API.
 */



#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>

class VideoCapture_private; // forward

/*! \brief Class that allows to capture a video from a connected device
    \ingroup tools_video


    This header file may be linked to one of several backends:
      * videocapture_v4l2.cpp implements the video capture device using video4linux v2
      * videocapture_vfw.cpp implements the video capture on windows using vfw. Note that you will also have to link against these libraraies: \c -lgdi32 \c -luser32 \c -lkernel32 \c -lvfw32
    .


    The vfw portion of this class is partially based on code by <a href="http://www.opengl.allanpetersen.com/?q=node/13">Allan Peterson</a>.
    The v4l2 portion of this class is based on the <a href="http://www.linuxquestions.org/questions/programming-9/how-to-capture-picture-from-v4l2-webcam-546351/">SDL + Video4Linux example</a>
    and on code from <a href="http://alumnos.elo.utfsm.cl/~yanez/video-for-linux-2-sample-programs/">http://alumnos.elo.utfsm.cl/~yanez/video-for-linux-2-sample-programs/</a>.
*/
class VideoCapture {
    public:
        enum UserProperties {
            upBrightness=0,
            upContrast=1,
            upSaturation=2,
            upHue=3,
            upAutoWhitebalance=4,
            upRedBalance=5,
            upBlueBalance=6,
            upGamma=7,
            upExposure=8,
            upAutogain=9,
            upGain=10,
            upPowerlineFilter=11,
            upSharpness=12,
            upBacklightCompensation=13,
            upFirstAdditional=14
        };

        enum UserPropertieType {
            uptInteger=0,
            uptBoolean=1,
            uptMenu=2,
            uptNone=3
        };

        /** \brief default constructor */
        VideoCapture();
        /** \brief default destructor */
        virtual ~VideoCapture();

        /** \brief enumerate the available devices (fill devices vector) */
        void enumDevices();

        /** \brief connect to the specified device driver */
        bool connect(int device);

        /** \brief disconnect from device */
        void disconnect();

        /** \brief return whether the device is connected */
        bool isConnected() const;


        /** \brief capture a single frame */
        bool captureFrame(unsigned char* image);

        /** \brief return the number of configure dialogs for the device */
        int getConfigDialogCount() const;

        /** \brief return the name of a configure dialog for the device */
        std::string getConfigDialogName(int dlg) const;

        /** \brief show a configure dialog for the device */
        void showConfigDialog(int dlg);

        bool supportsUserProperty(int prop) const;
        void getUserPropertyRange(int prop, int& min, int& max) const;
        int getUserProperty(int prop);
        void setUserProperty(int prop, int value);
        int getAdditionalUserPropertyCount() const;
        UserPropertieType getUserPropertyType(int prop) const;
        std::string getUserPropertyName(int prop) const;
        std::vector<std::string> getUserPropertyMenuItems(int prop) const;
        bool isUserPropertyEnabled(int prop);





        /** \brief get a list of the available devices */
        inline std::vector<std::string> getDeviceList() const { return devices; }

        /** \brief get the number of available devices */
        inline size_t getDeviceCount() const { return devices.size(); }

        /** \brief get the currently selected device */
        inline int getCurrentDevice() const { return selectedDevice; }

        /** \brief get the last error description (empty if no error occured) */
        std::string getLastError() const ;

        /** \brief get the name of the currently selected device */
        inline std::string getCurrentDeviceName() const {
            if (selectedDevice>=0)
                return devices[selectedDevice];
            else
                return "";
        };

        /** \brief get width of video input [pixels] */
        inline int getVideoWidth() const { return width; }


        /** \brief get height of video input [pixels] */
        inline int getVideoHeight() const { return height; }

        /** \brief get exposure time of the last frame in seconds */
        inline double getLastExposureTimet() const { return exposureTime; }

        /** \brief returns \c true if an error occured  */
        inline bool errorOccured() const { return getLastError().size()>0; }

    protected:

        /** \brief internal data pointer (implementation dependent) */
        VideoCapture_private* d;

        /** \brief a list of all available video devices */
        std::vector<std::string> devices;

        /** \brief which device is currently selected? */
        int selectedDevice;

        /** \brief width of the video input */
        int height;

        /** \brief height of the video input */
        int width;

        /** \brief the exposure time of the last frame in seconds */
        double exposureTime;


};
#endif // VIDEOCAPTURE_H
