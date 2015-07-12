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
#include "cpptools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
#include "image_tools.h"
#include <map>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define MAX_INPUT   16
#define MAX_NORM    16


//a blocking wrapper of the ioctl function
static int xioctl (int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}














struct DeviceData {
    std::string dev_name;
    int input;

    explicit DeviceData(std::string dev_name=std::string(""), int input=-1) {
        this->dev_name=dev_name;
        this->input=input;
    }
};

class VideoCapture_private {
    public:
        uint8_t* videoFrame;
        std::vector<DeviceData> deviceData;
        std::string lastError;
        int fd;
        void * buffer;
        size_t buffer_length;
        struct v4l2_format fmt;
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_requestbuffers reqbuf;
        struct v4l2_queryctrl queryctrl;
        struct v4l2_querymenu querymenu;
        std::string fourcc;
        bool memmapped;
        std::map<int, int> moreUserProperties;
        std::map<int, VideoCapture::UserPropertieType> moreUserPropertiesType;
        std::map<int, std::string> moreUserPropertiesName;
        std::map<int, std::vector<std::string> > moreUserPropertiesChoices;

        struct buffers_type {
            void *start;
            size_t length;
        };
        buffers_type* buffers;


        bool open_device (int * fd, const char * dev_name){
            struct stat st;

            if (-1 == stat (dev_name, &st)) {
                lastError= format("Cannot identify '%s': %d, %s\n", dev_name, errno, strerror (errno));
                return false;
            }

            if (!S_ISCHR (st.st_mode)) {
                lastError= format("%s is no device\n", dev_name);
                return false;
            }

            *fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

            if (-1 == *fd) {
                lastError= format("Cannot open '%s': %d, %s\n", dev_name, errno, strerror (errno));
                return false;
            }
            return true;
        }

        void close_device (int * fd) {
            if (-1 == close (*fd)) {
                lastError= errno_exit("close");
            }
            *fd = -1;
        }

        std::string errno_exit(const char *s) {
            return (lastError= format("%s error %d, %s\n",s, errno, strerror (errno)));
        }


        //configure the video input
        bool set_input(int * fd, int dev_input) {
            struct v4l2_input input;
            //set the input
            int index = dev_input;
            if (-1 == xioctl (*fd, VIDIOC_S_INPUT, &index)) {
                lastError= errno_exit("VIDIOC_S_INPUT");
                return false;
            }
            //check the input
            if (-1 == xioctl (*fd, VIDIOC_G_INPUT, &index)) {
                lastError= errno_exit("VIDIOC_G_INPUT");
                return false;
            }
            memset (&input, 0, sizeof (input));
            input.index = index;
            if (-1 == xioctl (*fd, VIDIOC_ENUMINPUT, &input)) {
                lastError= errno_exit("VIDIOC_ENUMINPUT");
                return false;
            }
            //fprintf (stderr,"input: %s\n", input.name);
            return true;
        }

        void list_controls(int fd) {
            int cnt=0;
            memset (&queryctrl, 0, sizeof (queryctrl));
            for (queryctrl.id = V4L2_CID_PRIVATE_BASE;; queryctrl.id++) {
                if (0 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
                    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                        continue;



                    if (queryctrl.type == V4L2_CTRL_TYPE_MENU) {
                        std::vector<std::string> menu;
                        memset (&querymenu, 0, sizeof (querymenu));
                        querymenu.id = queryctrl.id;

                        for (querymenu.index = queryctrl.minimum; querymenu.index <= queryctrl.maximum;  querymenu.index++) {
                            if (0 == xioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
                                menu.push_back((char*)querymenu.name);
                            } else {
                                break;
                            }
                        }
                        moreUserProperties[cnt+VideoCapture::upFirstAdditional]=queryctrl.id;
                        moreUserPropertiesName[cnt+VideoCapture::upFirstAdditional]=(char*)queryctrl.name;
                        moreUserPropertiesChoices[cnt+VideoCapture::upFirstAdditional]=menu;
                        moreUserPropertiesType[cnt+VideoCapture::upFirstAdditional]=VideoCapture::uptMenu;
                    } else if (queryctrl.type==V4L2_CTRL_TYPE_INTEGER) {
                        moreUserProperties[cnt+VideoCapture::upFirstAdditional]=queryctrl.id;
                        moreUserPropertiesName[cnt+VideoCapture::upFirstAdditional]=(char*)queryctrl.name;
                        moreUserPropertiesType[cnt+VideoCapture::upFirstAdditional]=VideoCapture::uptInteger;
                    } else if (queryctrl.type==V4L2_CTRL_TYPE_BOOLEAN) {
                        moreUserProperties[cnt+VideoCapture::upFirstAdditional]=queryctrl.id;
                        moreUserPropertiesName[cnt+VideoCapture::upFirstAdditional]=(char*)queryctrl.name;
                        moreUserPropertiesType[cnt+VideoCapture::upFirstAdditional]=VideoCapture::uptBoolean;
                    }

                } else {
                    break;
                }
            }
        }


        //configure and initialize the hardware device
        bool init_device (int * fd) {
            unsigned int min;
            memmapped=false;

            if (-1 == xioctl(*fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                    lastError=format("device is no V4L2 device\n");
                    return false;
                } else  {
                    lastError=errno_exit("VIDIOC_QUERYCAP");
                    return false;
                }
            }

            if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                lastError=format("device is not a video capture device\n");
                return false;
            }

            //reset cropping parameters to default
            memset (&cropcap, 0, sizeof (cropcap));
            cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl (*fd, VIDIOC_CROPCAP, &cropcap)) {
                /* Errors ignored. */
            }
            crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            crop.c = cropcap.defrect;
            if (-1 == xioctl (*fd, VIDIOC_S_CROP, &crop)) {
                /* Errrors ignored */
            }
            CLEAR (fmt);
            //set image properties
            fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;


            if (-1 == xioctl (*fd, VIDIOC_G_FMT, &fmt)) {
            }
            int w=fmt.fmt.pix.width;
            int h=fmt.fmt.pix.height;
            // READ FOURCC
            fourcc="    ";
            for (int i=0; i<4; i++) {
                char* f=(char*)(&(fmt.fmt.pix.pixelformat));
                fourcc[i]=f[i];
            }
            /*std::cout<<"fmt.fmt.pix.pixelformat = "<<fourcc<<"\n";
            std::cout<<"fmt.fmt.pix.colorspace  = "<<fmt.fmt.pix.colorspace<<"\n";
            std::cout<<"fmt.fmt.pix.field       = "<<fmt.fmt.pix.field<<"\n";
            std::cout<<"fmt.fmt.pix.sizeimage   = "<<fmt.fmt.pix.sizeimage<<"\n";
            std::cout<<"fmt.fmt.pix.width       = "<<fmt.fmt.pix.width<<"\n";
            std::cout<<"fmt.fmt.pix.height      = "<<fmt.fmt.pix.height<<"\n\n\n";
            std::cout.flush();*/

            //set image properties, try to set one of these formats (FOURCC): RGB24, BGR24, YUYV
            const int32_t pixelformats[]={V4L2_PIX_FMT_RGB24, V4L2_PIX_FMT_BGR24, V4L2_PIX_FMT_YUYV};
            const int pixelformatsCount=3;
            bool found=false;
            int pfi=0;
            while (!found && pfi<pixelformatsCount) {
                CLEAR (fmt);
                fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                fmt.fmt.pix.width=w;
                fmt.fmt.pix.height=h;
                fmt.fmt.pix.pixelformat = pixelformats[pfi];
                fmt.fmt.pix.colorspace  = V4L2_COLORSPACE_SRGB;
                fmt.fmt.pix.field       = V4L2_FIELD_ANY;
                if (-1 != xioctl (*fd, VIDIOC_S_FMT, &fmt)) {
                    found=true;
                }

                pfi++;
            }
            if (!found) {
                lastError=std::string("no usable pixelformat is supported by the device");
                return false;
            }
            //check the configuration data
            min = fmt.fmt.pix.width * 4;
            if (fmt.fmt.pix.bytesperline < min)
                            fmt.fmt.pix.bytesperline = min;
            min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
            if (fmt.fmt.pix.sizeimage < min)
                            fmt.fmt.pix.sizeimage = min;

            CLEAR (fmt);
            //set image properties
            fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;


            if (-1 == xioctl (*fd, VIDIOC_G_FMT, &fmt)) {
                //std::cout<<errno_exit("VIDIOC_G_FMT");
                //std::cout.flush();
            }


            // READ FOURCC
            fourcc="    ";
            for (int i=0; i<4; i++) {
                char* f=(char*)(&(fmt.fmt.pix.pixelformat));
                fourcc[i]=f[i];
            }

            std::cout<<"fmt.fmt.pix.pixelformat = "<<fourcc<<"\n";
            std::cout<<"fmt.fmt.pix.colorspace  = "<<fmt.fmt.pix.colorspace<<"\n";
            std::cout<<"fmt.fmt.pix.field       = "<<fmt.fmt.pix.field<<"\n";
            std::cout<<"fmt.fmt.pix.sizeimage   = "<<fmt.fmt.pix.sizeimage<<"\n";
            std::cout<<"fmt.fmt.pix.width       = "<<fmt.fmt.pix.width<<"\n";
            std::cout<<"fmt.fmt.pix.height      = "<<fmt.fmt.pix.height<<"\n\n\n";
            std::cout.flush();

            if (fmt.fmt.pix.colorspace  != V4L2_COLORSPACE_SRGB) {
                //lastError=std::string("SRGB color space is not supported by device");
                std::cout<<"SRGB color space is not supported by device\n";
                //return false;
            }

            list_controls(*fd);


            //std::cout.flush();

            /* Note VIDIOC_S_FMT may change width and height. */



            // START STREAMING; IF SELECTED
            if ((!(cap.capabilities & V4L2_CAP_READWRITE)) && (cap.capabilities & V4L2_CAP_STREAMING)) {
                enum v4l2_buf_type type;


                // WE HAVE TO USE THE STREAMING API; SO WE HAVE TO INITIALIZE IT!!!
                memset (&reqbuf, 0, sizeof (reqbuf));
                reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                reqbuf.memory = V4L2_MEMORY_MMAP;//V4L2_MEMORY_USERPTR;
                reqbuf.count = 4;
                memmapped=true;


                if (xioctl (*fd, VIDIOC_REQBUFS, &reqbuf) == -1) {
                        if (errno == EINVAL)
                                lastError=std::string("Memory mapped streaming is not supported\n");
                        else
                                errno_exit("VIDIOC_REQBUFS");

                        return false;
                }




                /* We want at least two buffers. */
                if (reqbuf.count < 2) {
                        /* You may need to free the buffers here. */
                        lastError=std::string ("Not enough buffer memory\n");
                        return false;
                }
                buffers = (buffers_type*)calloc (reqbuf.count, sizeof(buffers_type));

                // allocate, map and enqueue all buffers
                for (int i = 0; i < reqbuf.count; i++) {
                        struct v4l2_buffer v4fbuffer;

                        memset (&v4fbuffer, 0, sizeof (buffer));
                        v4fbuffer.type = reqbuf.type;
                        v4fbuffer.memory = V4L2_MEMORY_MMAP;
                        v4fbuffer.index = i;

                        if (-1 == xioctl (*fd, VIDIOC_QUERYBUF, &v4fbuffer)) {
                                errno_exit ("VIDIOC_QUERYBUF");
                                return false;
                        }

                        buffers[i].length = v4fbuffer.length; /* remember for munmap() */

                        buffers[i].start = mmap (NULL, v4fbuffer.length,
                                                 PROT_READ | PROT_WRITE, /* recommended */
                                                 MAP_SHARED,             /* recommended */
                                                 *fd, v4fbuffer.m.offset);

                        if (MAP_FAILED == buffers[i].start) {
                            errno_exit ("mmap");
                            /* If you do not exit here you should unmap() and free()
                               the buffers mapped so far. */
                            for (int j = 0; i < i; j++) {
                                munmap (buffers[j].start, buffers[j].length);
                            }
                            return false;
                        }

                        memset (&v4fbuffer, 0, sizeof (buffer));
                        v4fbuffer.type = reqbuf.type;
                        v4fbuffer.memory = V4L2_MEMORY_MMAP;
                        v4fbuffer.index = i;
                        if (-1 == xioctl (*fd, VIDIOC_QBUF, &v4fbuffer)) {
                            for (int j = 0; i < i; j++) {
                                munmap (buffers[j].start, buffers[j].length);
                            }
                            errno_exit ("VIDIOC_QBUF");
                            return false;
                        }
                }



                // start streaming
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                //start the capture from the device
                if (-1 == xioctl (*fd, VIDIOC_STREAMON, &type)) {
                    errno_exit ("VIDIOC_STREAMON");
                    return false;
                }

            } else {
                // allocate buffer for image rreading with the read() method
                if (buffer) free(buffer);
                buffer=malloc(fmt.fmt.pix.sizeimage);
                buffer_length=fmt.fmt.pix.sizeimage;
            }

            return true;
        }

        void deinit(int* fd) {
            // in streaming mode we have to dequeue all buffers, unmap and ree them
            if ((!(cap.capabilities & V4L2_CAP_READWRITE)) && (cap.capabilities & V4L2_CAP_STREAMING)) {
                enum v4l2_buf_type type;

                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                //this call to xioctl allows to stop the stream from the capture device
                if (-1 == xioctl (*fd, VIDIOC_STREAMOFF, &type))
                        errno_exit("VIDIOC_STREAMOFF");

                for (int i = 0; i < reqbuf.count; i++) {
                    munmap (buffers[i].start, buffers[i].length);
                }
                free(buffers);
                buffer=NULL;
            } else {
                if (buffer) free(buffer);
                buffer=NULL;
            }

        }

        void process_image (const uint8_t * videoFrame,  uint8_t* output, int width, int height) {
            switch(fmt.fmt.pix.pixelformat)  {
                case V4L2_PIX_FMT_RGB24:
                    process_image_rgb24(videoFrame, output, width, height,0,1,2);
                    break;
                case V4L2_PIX_FMT_BGR24:
                    process_image_rgb24(videoFrame, output, width, height, 2,1,0);
                    break;
                case V4L2_PIX_FMT_YUYV:
                    process_image_yuyv(videoFrame, output, width, height);
                    break;
                case V4L2_PIX_FMT_YVYU:
                    process_image_yuyv(videoFrame, output, width, height, 0,3,2,1);
                    break;
                case V4L2_PIX_FMT_UYVY:
                    process_image_yuyv(videoFrame, output, width, height, 1, 0, 3, 2);
                    break;
                case V4L2_PIX_FMT_VYUY:
                    process_image_yuyv(videoFrame, output, width, height, 1, 2, 3, 0);
                    break;
                case V4L2_PIX_FMT_GREY:
                    process_image_grey(videoFrame, output, width, height);
                    break;
            }
        }

        int isControl(int control, struct v4l2_queryctrl *queryctrl) {
            int err =0;
            queryctrl->id = control;
            if ((err= xioctl(fd, VIDIOC_QUERYCTRL, queryctrl)) < 0) {
                return -1;
            } else if (queryctrl->flags & V4L2_CTRL_TYPE_BOOLEAN) {
              return 1;
            } else if (queryctrl->flags & V4L2_CTRL_TYPE_MENU) {
              return 2;
            } else if (queryctrl->type & V4L2_CTRL_TYPE_INTEGER) {
              return 0;
            } else {
                return -1;
            }
            return -1;
        }


        bool isControlEnabled(int control, struct v4l2_queryctrl *queryctrl) {
            int err =0;
            queryctrl->id = control;
            if ((err= xioctl(fd, VIDIOC_QUERYCTRL, queryctrl)) < 0) {
                return false;
            } else if (queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {
              return false;
            }
            return true;
        }

        bool getControl(int control, int& value) {
            struct v4l2_queryctrl queryctrl;
            struct v4l2_control control_s;
            int err;
            if (isControl(control, &queryctrl) < 0)
              return false;
            control_s.id = control;
            if ((err = xioctl(fd, VIDIOC_G_CTRL, &control_s)) < 0) {
              return false;
            }
            value= control_s.value;
            return true;
        }

        void setControl(int control, int value) {
            struct v4l2_control control_s;
            struct v4l2_queryctrl queryctrl;
            int min, max, step, val_def;
            int err;
            if (isControl(control, &queryctrl) < 0)
              return ;
            min = queryctrl.minimum;
            max = queryctrl.maximum;
            step = queryctrl.step;
            val_def = queryctrl.default_value;
            if ((value >= min) && (value <= max)) {
              control_s.id = control;
              control_s.value = value;
              if ((err = xioctl(fd, VIDIOC_S_CTRL, &control_s)) < 0) {
                  return ;
              }
            }
        }

        int userProperty2Control(int prop) {
            switch(prop) {
                case VideoCapture::upBrightness: return V4L2_CID_BRIGHTNESS;
                case VideoCapture::upContrast: return V4L2_CID_CONTRAST;
                case VideoCapture::upSaturation: return V4L2_CID_SATURATION;
                case VideoCapture::upHue: return V4L2_CID_HUE;
                case VideoCapture::upAutoWhitebalance: return V4L2_CID_AUTO_WHITE_BALANCE;
                case VideoCapture::upRedBalance: return V4L2_CID_RED_BALANCE;
                case VideoCapture::upBlueBalance: return V4L2_CID_BLUE_BALANCE;
                case VideoCapture::upGamma: return V4L2_CID_GAMMA;
                case VideoCapture::upExposure: return V4L2_CID_EXPOSURE;
                case VideoCapture::upAutogain: return V4L2_CID_AUTOGAIN;
                case VideoCapture::upGain: return V4L2_CID_GAIN;
                case VideoCapture::upPowerlineFilter: return V4L2_CID_POWER_LINE_FREQUENCY;
                case VideoCapture::upSharpness: return V4L2_CID_SHARPNESS;
                case VideoCapture::upBacklightCompensation: return V4L2_CID_BACKLIGHT_COMPENSATION;
                default:
                    if (prop>=VideoCapture::upFirstAdditional) {
                        std::map<int,int>::iterator it=moreUserProperties.find(prop);
                        if (it!=moreUserProperties.end()) {
                            return moreUserProperties[it->second];
                        }
                    }
            }
            return -1;
        }

};






VideoCapture::VideoCapture() {
    d=new VideoCapture_private();
    d->fd=-1;
    d->buffer=NULL;
    d->buffer_length=0;
    d->buffers=NULL;
    selectedDevice=-1;
    height=0;
    width=0;
    exposureTime=0;
    d->lastError="";
    enumDevices();
}

VideoCapture::~VideoCapture() {
    disconnect();
    if (d->buffer) free(d->buffer);
    delete d;
}

std::string VideoCapture::getLastError() const  {
    return d->lastError;
}

void VideoCapture::enumDevices() {
    devices.clear();
    d->deviceData.clear();
    std::vector<std::string> list=listfiles_wildcard("/dev/video*");
    for (size_t i=0; i<list.size(); i++) {
        int fd=-1;
        if (d->open_device(&fd, list[i].c_str())) {

            struct v4l2_input  inp[MAX_INPUT];

            for (int ninputs = 0; ninputs < MAX_INPUT; ninputs++) {
                inp[ninputs].index = ninputs;
                if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &inp[ninputs])) break;

                struct v4l2_capability cap;
                if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) break;

                std::string n=format("%s (%s) input %s (%d)", cap.card, cap.driver/*list[i].c_str()*/, inp[ninputs].name, ninputs);
                devices.push_back(n);
                d->deviceData.push_back(DeviceData(list[i], ninputs));
            }
            d->close_device(&fd);
        }
    }
}

bool VideoCapture::connect(int device) {
    if (device>=0 && device<devices.size()) {
        if (d->open_device(&(d->fd), d->deviceData[device].dev_name.c_str())) {
            if (d->set_input(&(d->fd), d->deviceData[device].input)) {
                if (d->init_device(&(d->fd))) {
                    width=d->fmt.fmt.pix.width;
                    height=d->fmt.fmt.pix.height;
                    return true;
                } else {
                    d->close_device(&(d->fd));
                    d->lastError=format("could not properly initialize video device %s, error was: '%s'", d->deviceData[device].dev_name.c_str(), d->lastError.c_str());
                    return false;
                }
            } else {
                d->close_device(&(d->fd));
                d->lastError=format("input %d not available on device '%s'", d->deviceData[device].input, d->deviceData[device].dev_name.c_str());
                return false;
            }
        }
    } else {
        if (devices.size()<=0) d->lastError=std::string("no video devices available");
        else d->lastError=std::string("device not available");
    }
    return false;
}

void VideoCapture::disconnect() {
    if (isConnected()) {
        d->deinit(&(d->fd));
        d->close_device(&(d->fd));
    }
}

bool VideoCapture::isConnected() const {
    return d->fd>=0;
}

bool VideoCapture::captureFrame(unsigned char* image) {
    d->lastError="";
    if ((d->cap.capabilities & V4L2_CAP_READWRITE) && (d->buffer)) {
        if (read(d->fd, d->buffer, d->buffer_length)==d->buffer_length) {
            d->process_image((uint8_t*)d->buffer, image, width, height);
            return true;
        } else {
            d->lastError=d->errno_exit("captureFrame() [read()]");
            return false;
        }
    } else if ((d->cap.capabilities & V4L2_CAP_STREAMING)&&(d->buffers)) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl (d->fd, VIDIOC_DQBUF, &buf)) {
            if (errno!=EIO) {
                d->errno_exit ("VIDIOC_DQBUF");
                return false;
            }
        }
        if (buf.index<d->reqbuf.count) {
            //std::cout<<d->buffers[buf.index].length<<"   "<<width*height*3<<"\n";
            //std::cout.flush();
            d->process_image((uint8_t*)d->buffers[buf.index].start, image, width, height);
        } else {
            d->errno_exit ("BUFFER ERROR");
            return false;
        }

        if (-1 == xioctl (d->fd, VIDIOC_QBUF, &buf)) {
            d->errno_exit ("VIDIOC_QBUF");
            return false;
        }
        return  true;




    }
    d->lastError=std::string("device does not support any implemented read method [read() or streaming]");
    return false;
}



int VideoCapture::getConfigDialogCount() const {
    return 0;
}

std::string VideoCapture::getConfigDialogName(int dlg) const {
    return std::string("");
}

void VideoCapture::showConfigDialog(int dlg) {
}


bool VideoCapture::supportsUserProperty(int prop) const {
    int c=d->userProperty2Control(prop);
    if (c<0) return false;
    struct v4l2_queryctrl queryctrl;
    int isc=d->isControl(c, &queryctrl);

    return (c>-1) && (isc!=-1);
}

void VideoCapture::getUserPropertyRange(int prop, int& min, int& max) const {
    int c=d->userProperty2Control(prop);
    min=max=0;
    if (c>=0) {
        struct v4l2_queryctrl queryctrl;
        int isc=d->isControl(c, &queryctrl);

        //std::cout<<prop<<" (isc="<<isc<<"):  min="<<queryctrl.minimum<<"   max="<<queryctrl.maximum<<"\n";
        //std::cout.flush();


        switch(prop) {
            case VideoCapture::upAutoWhitebalance:
            case VideoCapture::upAutogain:
                min=0;
                max=1;
                return;
            case VideoCapture::upPowerlineFilter:
                min=0;
                max=2;
                return;
            default:
                if (isc < 0)
                  return ;
                if (isc==1){
                    min=0; max=1;
                } else {
                    min = queryctrl.minimum;
                    max = queryctrl.maximum;
                }
                return;
        }
    }
}

int VideoCapture::getUserProperty(int prop) {
    int c=d->userProperty2Control(prop);
    if (c>=0) {
        int i=0;
        d->getControl(c, i);
        return i;
    }
    return 0;
}


void VideoCapture::setUserProperty(int prop, int value) {
    int c=d->userProperty2Control(prop);
    if (c>=0) {
        d->setControl(c, value);
    }
}

int VideoCapture::getAdditionalUserPropertyCount() const {
    return d->moreUserProperties.size();
}


bool VideoCapture::isUserPropertyEnabled(int prop) {
    int c=d->userProperty2Control(prop);
    struct v4l2_queryctrl queryctrl;
    if (c>=0) {
        return d->isControlEnabled(c, &queryctrl);
    }
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
    int c=d->userProperty2Control(prop);
    if (c>=0) {
        struct v4l2_queryctrl queryctrl;
        int isc=d->isControl(c, &queryctrl);
        switch(isc) {
            case 0: return VideoCapture::uptInteger;
            case 1: return VideoCapture::uptBoolean;
            case 2: return VideoCapture::uptMenu;
        }
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
    int c=d->userProperty2Control(prop);
    if (c>=0) {
        struct v4l2_queryctrl queryctrl;
        int isc=d->isControl(c, &queryctrl);
        if (d->moreUserPropertiesName.find(isc)!=d->moreUserPropertiesName.end()) {
            return d->moreUserPropertiesName[isc];
        }
    }

    return std::string("");
}

std::vector<std::string> VideoCapture::getUserPropertyMenuItems(int prop) const {
    std::vector<std::string> l;
    if (prop==upPowerlineFilter) {
        l.push_back("disabled");
        l.push_back("50Hz");
        l.push_back("60Hz");
    } else {
        int c=d->userProperty2Control(prop);
        if (c>=0) {
            struct v4l2_queryctrl queryctrl;
            int isc=d->isControl(c, &queryctrl);
            if (d->moreUserPropertiesChoices.find(isc)!=d->moreUserPropertiesChoices.end()) {
                return d->moreUserPropertiesChoices[isc];
            }
        }
    }
    return l;
}
