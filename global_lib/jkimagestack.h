#ifndef JKIMAGESTACK_H_INCLUDED
#define JKIMAGESTACK_H_INCLUDED

/**
 * \defgroup imagestack_tools Image Stack Template Class
 * \ingroup image_tools
 */

#include <stdint.h>
#include <stdlib.h>
#include <tiffio.h>
#include <vector>
#include "tools.h"


/*! \brief this class provides basic capabilities to manage up to 4D images.
    \ingroup imagestack_tools

    This class stores data internally as a 1D array of a given type (templated class!). The
    interfacerepresents the data in a structured manor to the user. The user may use up
    to four diemnsions for the data, so this class represents either a 1D-, 2D-, 3D- or
    4D-array. The dimensions are named:
      * width
      * height
      * channel
      * depth/time
    .
    Internally thedata is represented in this fashion:
    \image html jkimagestack_dataorganisation.png

    So the whole array of data is
    \verbatim width * height * channels * depth * sizeof(T) \verbatim
    bytes in size, where T is the template parameter.

    The data may either be managed internally (then all functions are allowed on this
    image, or the data is shared. In the lather case the class may be used to access and
    alter the contained data, but all resizing methods are disabled. You may still call them,
    but they will have no effect. The shared images are used sso you

 */
template <class T>
class JKImageStack {
    protected:
        /** \brief array of the data */
        T* d;

        /** \brief image width */
        uint32_t iwidth;

        /** \brief image height */
        uint32_t iheight;

        /** \brief image channels */
        uint32_t ichannels;

        /** \brief image depth */
        uint32_t idepth;

        /** \brief indicates that this is a shred data object */
        bool shared;


        /** \brief just some dummy data */
        T dummy;


    public:
        /** \brief class constructor, constructs an empty (NULL) image */
        JKImageStack() {
            iwidth=0;
            iheight=0;
            ichannels=0;
            idepth=0;
            d=NULL;
            dummy=0;
            shared=false;
        };

        /** \brief class constructor, constructs a shared image from \a other */
        JKImageStack(JKImageStack<T>& other) {
            iwidth=shared.iwidth;
            iheight=shared.iheight;
            ichannels=shared.ichannels;
            idepth=shared.idepth;
            d=shared.d;
            dummy=0;
            shared=true;
        };

        /** \brief class constructor, constructs a shared image from the given data
         *  \param otherData points to the data of the other imagedata
         *  \param otherWidth width of \a otherData
         *  \param otherHeight height of \a otherData
         *  \param otherChannels channels of \a otherData
         *  \param otherDepth depths of \a otherData
         */
        JKImageStack(T* otherData, uint32_t otherWidth, uint32_t otherHeight=1, uint32_t otherChannels=1, uint32_t otherDepth=1) {
            iwidth=otherWidth;
            iheight=otherHeight;
            ichannels=otherChannels;
            idepth=otherDepth;
            d=otherData;
            dummy=0;
            shared=true;
        };

        /** \brief class destructor */
        ~JKImageStack() {
            if (d && (!shared)) {
                free(d);
                d=NULL;
                iwidth=0;
                iheight=0;
                ichannels=0;
                idepth=0;
                shared=false;
            }
        }

        /** \brief returns \c true, if the image is a NULL (i.e. not-existent) image */
        inline isNull() { return (d==NULL); }

        /** \brief returns \c true if this is a shared image  */
        inline isShared() { return ishared; }

        /** \brief returns pointer to the data */
        inline T* data() const { return d; };

        /*! \brief returns a pointer to the specified location in the image
         */
        inline T* data(uint32_t x, uint32_t y=0, uint32_t c=0, uint32_t z=0) const {
            return &(d[iwidth*iheight*ichannels*z+iwidth*iheight*c+iwidth*y+x]);
        };

        /*! \brief returns a pointer to the specified line in the image stack
         */
        inline T* lineData(uint32_t y=0, uint32_t c=0, uint32_t z=0) const {
            return data(0,y,c,z);
        };

        /*! \brief returns a pointer to the specified image channel in the image
         */
        inline T* imageData(uint32_t c, uint32_t z=0) const {
            return data(0,0,c,z);
        };

        /*! \brief returns a pointer to the specified image channel in the image
         */
        inline T* imageData(uint32_t z) const {
            return data(0,0,0,z);
        };


        /** \brief return image width */
        inline uint32_t width() const { return iwidth; };


        /** \brief return image height */
        inline uint32_t height() const { return iheight; };


        /** \brief return image channels */
        inline uint32_t channels() const { return ichannels; };


        /** \brief return image depth */
        inline uint32_t depth() const { return idepth; };

        /** \brief return the number of pixels in the image */
        inline uint32_t pixels() const { return iwidth*iheight*ichannels*idepth; }

        /** \brief return the number of bytes needed for the representation of the image */
        inline uint32_t pixels() const { return pixels()*sizeof(T); }

        /** \brief returns \c true if the size of this image differs from the given \a compareImage */
        template <class t>
        inline bool sizeDiffersFrom(const JKImageStack<t>& compareImage) {
            return (width()!=compareImage.width()) || (height()!=compareImage.height());
        }


        /** \brief access image pixel */
        T& operator()(uint32_t x, uint32_t y=0, uint32_t c=0, uint32_t z=0) {
            if ((x<iwidth) && (y<iheight) && (c<ichannels) && (i<idepth)) {
                return *data(x,y,c,z);
            }
            return dummy;
        };



        /** \brief resize the image to a given size. Return \c true if a resize has been carried out */
        inline bool resize(uint32_t newwidth, uint32_t newheight=1, uint32_t newchannels=1, uint32_t newdepth=1) {
            if (shared) return false;
            if ((newwidth==iwidth) && (newheight==iheight) && (newchannels==ichannels) && (newdepth==idepth)) return false;
            if (d) free(d);
            d=NULL;
            iwidth=newwidth;
            iheight=newheight;
            ichannels=newchannels;
            idepth=newdepth;
            d=(T*)calloc(newwidth*newheight*newchannels*newdepth,sizeof(T));
            return true;
        }


        /** \brief resize the image to a given size and set all pixels to the given value */
        inline void assign(T value, uint32_t newwidth, uint32_t newheight=1, uint32_t newchannels=1, uint32_t newdepth=1) {
            bool r=resize(newwidth, newheight, newchannels, newdepth);
            if ((!ishared)||(shared && (!r))) {
                for (uint32_t i=0; i< iwidth*iheight*ichannels*idepth; i++) {
                    d[i]=value;
                }
            }
        };


        /*! \brief copy data from the given array into the image
            \param newimage points to the new image, this has to be of size \c newwidth*newheight*sizeof(T)
            \param newwidth width of \a newimage
            \param newheight height of \a newimage
            \param newchannels number of channels of \a newimage
            \param newdepth depth of \a newimage

            \note The data is converted to the datatype of this class when copied!!! This uses the implicit C++ type casting.
         */
        template <class t>
        inline void assign(t* newimage, uint32_t newwidth, uint32_t newheight=1, uint32_t newchannels=1, uint32_t newdepth=1) {
            bool r=resize(newwidth, newheight, newchannels, newdepth);
            if ((!ishared)||(shared && (!r))) {
                for (uint32_t i=0; i< iwidth*iheight*ichannels,*idepth; i++) {
                    d[i]=newimage[i];
                }
            }
        }


        /** \brief set all pixels to the given value */
        inline void fill(T value=0) {
            for (uint32_t i=0; i< iwidth*iheight*ichannels*idepth; i++) {
                d[i]=value;
            }
        }

        /** \brief set all pixels to 0 */
        inline void clear() {
            setAll(0);
        }



        // TODO: go on to rewrite from here ...

        /** \brief resize the image to a given size and set all pixels to the given value */
        template <class t>
        inline void assign(const JKImage<t>& img) {
            bool r=resize(img.width(), img.height(), img.channels(), img.depth());
            if ((!ishared)||(shared && (!r))) {
                t* d1=img.data();
                for (uint32_t i=0; i< iwidth*iheight; i++) {
                    d[i]=d1[i];
                }
            }
        }


        /** \brief subtract the given image from the current */
        template <class t>
        inline void subtract(const JKImage<t>& img) {
            if ((! sizeDiffersFrom(img)) && d) {
                t* d1=img.data();
                for (uint32_t i=0; i< pixels(); i++) {
                    d[i] = d[i]-d1[i];
                }
            }
        }


        /** \brief subtract the given image from the current */
        template <class t>
        inline void add(const JKImage<t>& img) {
            if ((! sizeDiffersFrom(img)) && d) {
                t* d1=img.data();
                for (uint32_t i=0; i< pixels(); i++) {
                    d[i] = d[i] + d1[i];
                }
            }
        }

        /** \brief return min and max of image values
         *
         * Call it with \code T min, max; min=img.min_max(max); \endcode to obtain
         * the minimum and maximum pixel value in the variables min and max.
         */
        inline T min_max(T& maxx) {
            T minn=0;
            maxx=0;
            if (d) {
                minn=maxx=d[0];
                for (uint32_t i=1; i< iwidth*iheight; i++) {
                    register T t=d[i];
                    if (t>maxx) maxx=t;
                    if (t<minn) minn=t;
                }
            }
            return minn;
        }

        /** \brief return min and max of image values and ignore masked pixels
         *
         * Call it with \code T min, max; min=img.min_max(max); \endcode to obtain
         * the minimum and maximum pixel value in the variables min and max.
         */
        inline T min_max(T& maxx, bool* mask) {
            T minn=0;
            maxx=0;
            if (d) {
                bool first=true;
                for (uint32_t i=1; i< iwidth*iheight; i++) {
                    if (!mask[i]) {
                        if (first) {
                            minn=maxx=d[i];
                            first=false;
                        } else {
                            register T t=d[i];
                            if (t>maxx) maxx=t;
                            if (t<minn) minn=t;
                        }
                    }
                }
            }
            return minn;
        }



        /*! \brief calculate the statistics (& optionally a histogram) of the given image

            This function takes the given image and a given mask which both have to be of size \c image_width*image_height
            and calculates the image statistics (sum of pixel values, average pixel value, stadard deviation, minimum-
            and maximum pixel value). If the histogram_... pointers are provided the function also calculates a histogram
            of the pixel values.

            \param[in] image points to the input image
            \param[in] mask points to the input image mask (for broken pixels)
            \param[in] image_width width of the image in pixels
            \param[in] image_height height of the image in pixels
            \param[out] brokenPixels returns the number of broken pixels, i.e. the number of \c true pixels in mask)
            \param[out] sum sum of all pixel values \f$ \sum\limits_{<x,y>}g(x,y) \f$
            \param[out] mean sum of all pixel values \f$ \frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}}\cdot\sum\limits_{<x,y>}g(x,y) \f$
            \param[out] stddev standard deviation of all pixel values \f$ sqrt{\frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}-1}\cdot\left[\left(\sum\limits_{<x,y>}g(x,y)^2\right)-\mbox{imagewidth}\cdot\mbox{imageheight}\cdot\mbox{mean}\right]} \f$
            \param[out] imin returns the smallest pixel value
            \param[out] imax returns the biggest pixel value
            \param[in,out] histogram_x points to an array of the histogram x-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_y points to an array of the histogram y-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_n points to the number of histogram bins. If histogram_autoscale is \c true this could be changed to a new values and histogram_x/histogram_y will be realloced
            \param[out] histogram_min returns the smallest histogram x-value
            \param[out] histogram_max returns the biggest histogram x-value
            \param[out] histogram_fmax returns the biggest histogram y-value
            \param[in] histogram_autoscale if \c true the number of histogram bins is given by the range of pixel values

        */
        void calcImageStatistics(bool* mask, uint32_t* brokenPixels, double* sum, double* mean, double* stddev, T* imin, T* imax, double** histogram_x=NULL, double** histogram_y=NULL, uint32_t* histogram_n=NULL, double* histogram_min=NULL, double* histogram_max=NULL, double* histogram_fmax=NULL, bool histogram_autoscale=false) {
            T* image=d;
            uint32_t image_width=iwidth;
            uint32_t image_height=iheight;
            if (!image || !mask || !brokenPixels || !sum || !mean || !stddev || !imin || !imax || image_width<=0 || image_height<=0) return;
            T iimin=image[0];
            T iimax=image[0];
            //*imin=image[0];
            //*imax=image[0];
            bool first=true;
            double sum2=0;
            double ssum=0;
            //*brokenPixels=0;
            uint32_t bp=0;
            uint32_t loopsize=image_width*image_height;
            // calculate sum of grayvalues, sum of squared grayvalues (for std dev.), as well as min and max of image
            // care for the masked pisels!
            for (register uint32_t i=0; i<loopsize; i++) {
                if (!mask[i]) {
                    T im=image[i];
                    if (first) {
                        iimin=im;
                        iimax=im;
                        first=false;
                    } else {
                        if (im<iimin) iimin=im;
                        if (im>iimax) iimax=im;
                    }
                    ssum+=im;
                    sum2+=im*im;
                } else bp++;
            }
            *sum=ssum;
            *mean=ssum/(double)(image_width*image_height);
            *stddev=sqrt((sum2-ssum*ssum/(double)loopsize)/(double)(loopsize-1));
            *brokenPixels=bp;
            T hrange=iimax-iimin;

            if (histogram_n && histogram_min && histogram_max && histogram_fmax) {
                uint32_t hn=*histogram_n;
                if (histogram_autoscale && hrange>0) {
                    hn=(uint32_t)std::abs((int32_t)hrange);
                    if (hn<1) { hn=1; }
                    if (hn>100000) { hn=100000; }
                }
                *histogram_n=hn;

                *histogram_x=(double*)realloc(*histogram_x, hn*sizeof(double));
                *histogram_y=(double*)realloc(*histogram_y, hn*sizeof(double));

                double* hx = (*histogram_x);
                double* hy = (*histogram_y);
                /*for (int i=0; i<hn; i++) {
                    hx[i]=i;
                    hy[i]=0;
                }*/
                // set histogram to all 0



                if (hrange==0) {
                    /*for (int i=0; i<hn; i++) {
                        hy[i]=hx[i]=0;
                    }*/
                    *histogram_min=0;
                    *histogram_max=1;
                    *histogram_fmax=1;
                    memset(hx, hn*sizeof(double),0);
                    memset(hy, hn*sizeof(double),0);
                } else {
                    double dh=(double)hrange/(double)hn;
                    double hfmax=0;
                    //memset(hy, hn*sizeof(double),0);
                    for (uint32_t i=0; i<hn; i++) {
                        hy[i]=0;
                        hx[i]=(double)(iimin)+((double)i)*dh;
                    }
                    *histogram_min=(double)(iimin);
                    *histogram_max=(double)(iimin)+(double)(hn-1)*dh;
                    for (register uint32_t i=0; i<loopsize; i++) {
                        if (!mask[i]) {
                            int bin=floor((double)(image[i]-(iimin))/dh);
                            if (bin>=(int64_t)hn) bin=hn-1;
                            if (bin<0) bin=0;
                            double h=hy[bin]+1;
                            if (h>hfmax) hfmax=h;
                            hy[bin]=h;
                        }
                    }
                    *histogram_fmax=hfmax;

                }
            }
            *imin=iimin;
            *imax=iimax;
        }


        /*! \brief calculate the statistics (& optionally a histogram) of the given image

            This function takes the given image and a given mask which both have to be of size \c image_width*image_height
            and calculates the image statistics (sum of pixel values, average pixel value, stadard deviation, minimum-
            and maximum pixel value). If the histogram_... pointers are provided the function also calculates a histogram
            of the pixel values.

            \param[in] image points to the input image
            \param[in] mask points to the input image mask (for broken pixels)
            \param[in] image_width width of the image in pixels
            \param[in] image_height height of the image in pixels
            \param[out] brokenPixels returns the number of broken pixels, i.e. the number of \c true pixels in mask)
            \param[out] sum sum of all pixel values \f$ \sum\limits_{<x,y>}g(x,y) \f$
            \param[out] mean sum of all pixel values \f$ \frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}}\cdot\sum\limits_{<x,y>}g(x,y) \f$
            \param[out] stddev standard deviation of all pixel values \f$ sqrt{\frac{1}{\mbox{imagewidth}\cdot\mbox{imageheight}-1}\cdot\left[\left(\sum\limits_{<x,y>}g(x,y)^2\right)-\mbox{imagewidth}\cdot\mbox{imageheight}\cdot\mbox{mean}\right]} \f$
            \param[out] imin returns the smallest pixel value
            \param[out] imax returns the biggest pixel value
            \param[in,out] histogram_x points to an array of the histogram x-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_y points to an array of the histogram y-values. This could point to a new array afterwards, as the function internally calls realloc() !)
            \param[in,out] histogram_n points to the number of histogram bins. If histogram_autoscale is \c true this could be changed to a new values and histogram_x/histogram_y will be realloced
            \param[out] histogram_min returns the smallest histogram x-value
            \param[out] histogram_max returns the biggest histogram x-value
            \param[out] histogram_fmax returns the biggest histogram y-value
            \param[in] histogram_autoscale if \c true the number of histogram bins is given by the range of pixel values

        */
        inline void calcImageStatistics(JKImage<bool> mask, uint32_t* brokenPixels, double* sum, double* mean, double* stddev, T* imin, T* imax, double** histogram_x=NULL, double** histogram_y=NULL, uint32_t* histogram_n=NULL, double* histogram_min=NULL, double* histogram_max=NULL, double* histogram_fmax=NULL, bool histogram_autoscale=false) {
            calcImageStatistics(mask.data(), brokenPixels, sum, mean, stddev, imin, imax, histogram_x, histogram_y, histogram_n, histogram_min, histogram_max, histogram_fmax, histogram_autoscale);
        }

        /** \brief save image as a list of all pixels that are != 0 */
        bool save_nonzero_list(std::string filename) {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            FILE* f=fopen(filename.c_str(), "w");
            if (!f) return false;

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (data[y*width+x]!=0) fprintf(f,"%ld, %ld\n", (long)x, (long)y);
                }
            }
            fclose(f);
            return true;
        }

        /** \brief save image as a list of all pixels that are == 0 */
        bool save_zero_list(std::string filename) {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            FILE* f=fopen(filename.c_str(), "w");
            if (!f) return false;

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (data[y*width+x]==0) fprintf(f,"%ld, %ld\n", (long)x, (long)y);
                }
            }
            fclose(f);
            return true;
        }

        /** \brief load image, as stored by save__nonzero_list */
        bool load_nonzero_list(std::string filename) {

            setAll(0);

            FILE* f=fopen(filename.c_str(), "r");
            std::vector<double> r=csv_readline(f);
            while (r.size()>0) {
                if (r.size()>=2) {
                    uint32_t x=r[0];
                    uint32_t y=r[1];
                    if ((x>0) && (x<iwidth) && (y>0) && (y<iwidth)) d[y*iwidth+x]=(T)1;
                }
                r=csv_readline(f);
            }
            fclose(f);

            return true;
        }

        /** \brief load image, as stored by save_zero_list */
        bool load_zero_list(std::string filename) {

            setAll(1);

            FILE* f=fopen(filename.c_str(), "r");
            std::vector<double> r=csv_readline(f);
            while (r.size()>0) {
                if (r.size()>=2) {
                    int x=r[0];
                    int y=r[1];
                    if (x>0 && x<iwidth && y>0 && y<iwidth) d[y*iwidth+x]=(T)0;
                }
                r=csv_readline(f);
            }
            fclose(f);

            return true;
        }


        /** \brief save image as a Comma-Separated Values file */
        bool save_csv(std::string filename, std::string separator=std::string(", ")) {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;
            FILE* f=fopen(filename.c_str(), "w");
            if (!f) return false;
            fprintf(f, "# width = %ld\n", (long)width);
            fprintf(f, "# height = %ld\n", (long)height);
            fprintf(f, "\n\n");

            for (uint32_t y=0; y<height; y++) {
                for (uint32_t x=0; x<width; x++) {
                    if (x>0) fputs(separator.c_str(), f);
                    double d=data[y*width+x];
                    fprintf(f,"%lg", d);
                }
                fputc('\n', f);
            }
            fclose(f);
            return true;
        }

        /** \brief save the image as TIFF with dataformat float (32 bit) */
        bool save_tifffloat(std::string filename) {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TIFF* tif = TIFFOpen(filename.c_str(), "w");
            if (!tif) return false;


            int sampleperpixel = 1;
            int bitspersample = sizeof(float)*8;
            TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
            TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
            TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
            TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,sampleperpixel);
            TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitspersample);
            TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
            TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
            TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
            TIFFSetField(tif,TIFFTAG_SOFTWARE,"JKImage library");
            TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);

            tsize_t linebytes = sampleperpixel * width*bitspersample/8;     // length in memory of one row of pixel in the image.
            unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
            //    Allocating memory to store the pixels of current row
            if (TIFFScanlineSize(tif)<linebytes)
                buf =(unsigned char *)_TIFFmalloc(linebytes);
            else
                buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
            float* dat=(float*)buf;

            // We set the strip size of the file to be size of one row of pixels
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));

            //Now writing image to the file one strip at a time
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t i=0; i<width; i++) {
                    dat[i]=data[y*width+i];
                }
                //memcpy(buf, dat, linebytes);    // check the index here, and figure out why not using h*linebytes
                if (TIFFWriteScanline(tif, buf, y, 0) < 0)
                break;
            }

            TIFFClose(tif);
            if (buf) _TIFFfree(buf);
            return true;
        }


        /** \brief save the image as TIFF with dataformat unsiged int 16-bit */
        bool save_tiffuint16(std::string filename) {
            T* data=d;
            uint32_t width=iwidth;
            uint32_t height=iheight;

            TIFF* tif = TIFFOpen(filename.c_str(), "w");
            if (!tif) return false;


            int sampleperpixel = 1;
            int bitspersample = sizeof(uint8_t)*8;
            TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,width);
            TIFFSetField(tif,TIFFTAG_IMAGELENGTH,height);
            TIFFSetField(tif,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
            TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,sampleperpixel);
            TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,bitspersample);
            TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
            TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
            TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
            TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
            TIFFSetField(tif,TIFFTAG_SOFTWARE,"JKImage library");
            TIFFSetField(tif,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);

            tsize_t linebytes = sampleperpixel * width*bitspersample/8;     // length in memory of one row of pixel in the image.
            unsigned char *buf = NULL;        // buffer used to store the row of pixel information for writing to file
            //    Allocating memory to store the pixels of current row
            if (TIFFScanlineSize(tif)<linebytes)
                buf =(unsigned char *)_TIFFmalloc(linebytes);
            else
                buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif));
            uint16_t* dat=(uint16_t*)buf;

            // We set the strip size of the file to be size of one row of pixels
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, linebytes));

            //Now writing image to the file one strip at a time
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t i=0; i<width; i++) {
                    if (data[y*width+i]<0) dat[i]=0;
                    if (data[y*width+i]>(double)0xFFFF) dat[i]=0xFFFF;
                    else dat[i]=round(data[y*width+i]);
                }
                //memcpy(buf, dat, linebytes);    // check the index here, and figure out why not using h*linebytes
                if (TIFFWriteScanline(tif, buf, y, 0) < 0)
                break;
            }

            TIFFClose(tif);
            if (buf) _TIFFfree(buf);
            return true;
        }
};


#endif // JKIMAGE_H_INCLUDED
