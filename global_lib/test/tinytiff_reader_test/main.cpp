#ifdef TEST_LIBTIFF
#include <tiffio.h>
#include "../../libtiff_tools.h"
#endif
#include <iostream>
#include "../../tinytiffwriter.h"
#include "../../tinytiffreader.h"
#include "../../highrestimer.h"
#include "../../tools.h"

using namespace std;

#define WIDTH 64
#define HEIGHT 64
#define PATTERNSIZE 12
#define SPEEDTEST_SIZE 1000
#define SPEEDTEST_OUTPUT 10
#define TEST_FRAMES 100000

#ifdef TEST_LIBTIFF
#define SAVE_TIFF(filename, data, width,height) { \
    std::cout<<"SAVE_TIFF("<<filename<<")\n"; \
    TIFF* t=TIFFOpen(filename, "w"); \
    uint32_t* d=(uint32_t*)calloc(width*height,sizeof(uint32_t));\
    for (int kkt=0; kkt<width*height; kkt++) {\
        d[kkt]=data[kkt];\
    }\
    TIFFTWriteUint16from32(t, d, width, height);\
    TIFFClose(t); \
    free(d);\
}
#else
#define SAVE_TIFF(filename, data, width,height)
#endif

#define TEST_SIMPLE(filename, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    tiffr=TinyTIFFReader_open(filename); \
    if (!tiffr) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; \
        timer.start(); \
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        do { \
            uint32_t width=TinyTIFFReader_getWidth(tiffr); \
            uint32_t height=TinyTIFFReader_getHeight(tiffr); \
            if (width>0 && height>0) std::cout<<"    size of frame "<<frame<<": "<<width<<"x"<<height<<"\n"; \
            else { std::cout<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height<<"\n"; ok=false; } \
            if (ok) { \
                frame++; \
                imagetype* tmp=(imagetype*)calloc(width*height, sizeof(imagetype));  \
                TinyTIFFReader_getSampleData(tiffr, tmp, 0); \
                if (TinyTIFFReader_wasError(tiffr)) { ok=false; std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; } \
                if (ok) { \
                    char fn[1024];\
                    sprintf(fn, "%s.%u.tif", filename, frame); \
                    SAVE_TIFF(fn, tmp, width, height); \
                }\
                free(tmp); \
            } \
        } while (ok && TinyTIFFReader_readNext(tiffr));\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TinyTIFFReader_close(tiffr); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";



#define TEST(filename, image, imagei, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    tiffr=TinyTIFFReader_open(filename); \
    if (!tiffr) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; \
        timer.start(); \
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        double duration_getdata=0, duration_nextframe=0; \
        bool next; \
        do { \
            uint32_t width=TinyTIFFReader_getWidth(tiffr); \
            uint32_t height=TinyTIFFReader_getHeight(tiffr); \
            ok=(width==WIDTH)&&(height==HEIGHT); \
            if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": size does not match, read "<<width<<"x"<<height<<"    expected "<<WIDTH<<"x"<<HEIGHT<<"\n";; \
            if (ok) { \
                imagetype* tmp=(imagetype*)calloc(width*height, TinyTIFFReader_getBitsPerSample(tiffr)/8);  \
                timer1.start(); \
                TinyTIFFReader_getSampleData(tiffr, tmp, 0); \
                duration_getdata+=timer1.get_time(); \
                if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; \
                uint32_t poserror=0xFFFFFFFF; \
                if (frame%2==0) {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==image[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                } else {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==imagei[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                }\
                free(tmp); \
                if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": did not read correct contents @ pos="<<poserror<<"\n";; \
            } \
            if (ok) { \
                frame++; \
            } \
            timer1.start(); \
            next=TinyTIFFReader_readNext(tiffr);\
            duration_nextframe+=timer1.get_time(); \
        } while (ok && next);\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    getSampleData() consumed "<<floattounitstr(duration_getdata/1.0e6, "s")<<"\n"; \
        std::cout<<"    readNext() consumed "<<floattounitstr(duration_nextframe/1.0e6, "s")<<"\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TinyTIFFReader_close(tiffr); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";



#define TEST_LIBTIFF(filename, image, imagei, imagetype) \
    std::cout<<"\n\nreading '"<<filename<<"' with libtiff and checking read contents ... filesize = "<<bytestostr(get_filesize(filename))<<"\n"; \
    ok=false;\
    timer.start(); \
    std::cout<<"    opening file     [duration: "<<floattounitstr(double(timer.get_time())/1.0e6, "s")<<" ]\n"; \
    ltiff=TIFFOpen(filename, "r"); \
    if (!ltiff) { \
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; \
    } else { \
        timer.start(); \
        uint32_t frames=TIFFCountDirectories(ltiff); \
        double duration=timer.get_time(); \
        std::cout<<"    frames: "<<frames<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        timer.start(); \
        ok=true; \
        uint32_t frame=0; \
        do { \
            uint32_t width; \
            uint32_t height; \
            uint32_t bitspersample; \
            TIFFGetField(ltiff,TIFFTAG_BITSPERSAMPLE,&bitspersample);\
            TIFFGetField(ltiff,TIFFTAG_IMAGEWIDTH,&width); \
            TIFFGetField(ltiff,TIFFTAG_IMAGELENGTH,&height); \
			char*val; \
			TIFFGetField(ltiff,TIFFTAG_IMAGEDESCRIPTION,&val); \
			std::cout<<"    ImageDescription:\n"<<val<<"\n"; \
            ok=(width==WIDTH)&&(height==HEIGHT); \
            if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": size does not match, read "<<width<<"x"<<height<<"    expected "<<WIDTH<<"x"<<HEIGHT<<"\n";; \
            if (ok && image && imagei) { \
                imagetype* tmp=(imagetype*)calloc(width*height, sizeof(imagetype));   \
                TIFFReadFrame(ltiff, tmp); \
                uint32_t poserror=0xFFFFFFFF; \
                if (frame%2==0) {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==image[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                } else {\
                    for (uint32_t i=0; i<width*height; i++) {\
                        ok=ok&&(tmp[i]==imagei[i]); \
                        if (!ok) { poserror=i; break; } \
                    }\
                }\
                free(tmp); \
                if (!ok) std::cout<<"    ERROR IN FRAME "<<frame<<": did not read correct contents @ pos="<<poserror<<"\n";; \
            } \
            if (ok) { \
                frame++; \
            } \
        } while (ok && TIFFReadDirectory(ltiff));\
        duration=timer.get_time(); \
        std::cout<<"    read and checked all frames: "<<((ok)?std::string("SUCCESS"):std::string("ERROR"))<<"     [duration: "<<duration<<" us  =  "<<floattounitstr(duration/1.0e6, "s")<<" ]\n"; \
        std::cout<<"    read "<<frame<<" frames\n"; \
    } \
    TIFFClose(ltiff); \
    if (ok) std::cout<<"  => SUCCESS\n"; \
    else std::cout<<"  => NOT CORRECTLY READ\n";


int main() {
    std::cout<<"creating some test TIFF files ...";

    HighResTimer timer, timer1;


    uint8_t* image8=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint8_t* image8i=(uint8_t*)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    uint16_t* image16=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));
    uint16_t* image16i=(uint16_t*)calloc(WIDTH*HEIGHT, sizeof(uint16_t));

    int i=0;
    for (int x=0; x<WIDTH; x++) {
        for (int y=0; y<HEIGHT; y++) {
            if (x==y) image8[i]=254;
            else if ((x%PATTERNSIZE==0) && (y%PATTERNSIZE==0)) image8[i]=200;
            else if (x%PATTERNSIZE==0) image8[i]=75;
            else if (y%PATTERNSIZE==0) image8[i]=150;
            image8i[i]=255-image8[i];
            image16[i]=(255-image8[i])*255;
            image16i[i]=0xFFFF-image16[i];
            i++;
        }
    }

    TinyTIFFFile* tiff = TinyTIFFWriter_open("test8.tif", 8, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image8);
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("test8m.tif", 8, WIDTH,HEIGHT);
    for (int i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image8);
        TinyTIFFWriter_writeImage(tiff, image8i);
    }
    TinyTIFFWriter_close(tiff);

    tiff = TinyTIFFWriter_open("test16.tif", 16, WIDTH,HEIGHT);
    TinyTIFFWriter_writeImage(tiff, image16);
    TinyTIFFWriter_close(tiff);
    tiff = TinyTIFFWriter_open("test16m.tif", 16, WIDTH,HEIGHT);
    for (int i=0; i<TEST_FRAMES/2; i++) {
        TinyTIFFWriter_writeImage(tiff, image16);
        TinyTIFFWriter_writeImage(tiff, image16i);
    }
    TinyTIFFWriter_close(tiff);
    std::cout<<" DONE!\n";

    bool ok=false;
    TinyTIFFReaderFile* tiffr=NULL;

    TEST_SIMPLE("cell.tif", uint8_t)
    TEST_SIMPLE("circuit.tif", uint8_t)
    TEST_SIMPLE("galaxy.tif", uint8_t)
    TEST_SIMPLE("mri.tif", uint8_t)
    TEST_SIMPLE("multi-channel-time-series.ome.tif", uint8_t)


    TEST("test8.tif", image8, image8i, uint8_t)
    TEST("test8m.tif", image8, image8i, uint8_t)
    TEST("test16.tif", image16, image16i, uint16_t)
    TEST("test16m.tif", image16, image16i, uint16_t)

#ifdef TEST_LIBTIFF
    if (TEST_FRAMES<60000) {
        TIFF* ltiff=NULL;
        TEST_LIBTIFF("test8.tif", image8, image8i, uint8_t)
        TEST_LIBTIFF("test8m.tif", image8, image8i, uint8_t)
        TEST_LIBTIFF("test16.tif", image16, image16i, uint16_t)
        TEST_LIBTIFF("test16m.tif", image16, image16i, uint16_t)
        TEST_LIBTIFF("test16m_imagej.tif", image16, image16i, uint16_t)
    }
	{
		TIFF* ltiff=NULL;
		char* nval=NULL;
		TEST_LIBTIFF("multi-channel-time-series.ome.tif", nval, nval, uint16_t)
		TEST_SIMPLE("multi-channel-time-series.ome.tif", uint8_t)
	}
#endif

    free(image8);
    free(image16);
    free(image8i);
    free(image16i);
    return 0;
}
