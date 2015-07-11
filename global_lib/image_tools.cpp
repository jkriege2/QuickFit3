/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "image_tools.h"





int clamp (double x)
{
        int r = x;      /* round to nearest */

        if (r < 0)         return 0;
        else if (r > 255)  return 255;
        else               return r;
}


void yuv420_rgb (unsigned char Y1, unsigned char Cb, unsigned char Cr, int *ER, int *EG, int *EB)
{
        double r, g, b;         /* temporaries */
        double y1, pb, pr;

        y1 = (255 / 219.0) * (Y1 - 16);
        pb = (255 / 224.0) * (Cb - 128);
        pr = (255 / 224.0) * (Cr - 128);

        r = 1.0 * y1 + 0     * pb + 1.402 * pr;
        g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
        b = 1.0 * y1 + 1.772 * pb + 0     * pr;

        *ER = clamp (r); /* [ok? one should prob. limit y1,pb,pr] */
        *EG = clamp (g );
        *EB = clamp (b );
}

void process_image_rgb32 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r, int pos_g, int pos_b) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            output[(width*3*y)+(3*x)]   = videoFrame[(width*4*y)+(4*x)+pos_b];//blue
            output[(width*3*y)+(3*x)+1] = videoFrame[(width*4*y)+((4*x)+pos_g)];//green
            output[(width*3*y)+(3*x)+2] = videoFrame[(width*4*y)+((4*x)+pos_r)];//red
        }
    }
}

void process_image_rgb24 (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_r, int pos_g, int pos_b) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            output[(width*3*y)+(3*x)]   = videoFrame[(width*3*y)+(3*x)+pos_b];//blue
            output[(width*3*y)+(3*x)+1] = videoFrame[(width*3*y)+(3*x)+pos_g];//green
            output[(width*3*y)+(3*x)+2] = videoFrame[(width*3*y)+(3*x)+pos_r];//red
        }
    }
}

void process_image_yuyv (const uint8_t * videoFrame,  uint8_t* output, int width, int height, int pos_Y1, int pos_U, int pos_Y2, int pos_V) {
    int r,g,b;
    unsigned char Y1, Y2,U,V;
    int ncnt=0;


    for (int x = 0; x < height*width; x=x+2) {

            Y1=videoFrame[ncnt+pos_Y1];
            U=videoFrame[ncnt+pos_U];
            Y2=videoFrame[ncnt+pos_Y2];
            V=videoFrame[ncnt+pos_V];
            ncnt=ncnt+4;

            yuv420_rgb(Y1,U,V,&r,&g,&b);
            output[(x*3)+0] = r;
            output[(x*3)+1] = g;
            output[(x*3)+2] = b;

            yuv420_rgb(Y2,U,V,&r,&g,&b);
            output[(x*3)+3] = r;
            output[(x*3)+4] = g;
            output[(x*3)+5] = b;
    }
}


void process_image_grey (const uint8_t * videoFrame,  uint8_t* output, int width, int height) {
    for (int x = 0; x < height*width; x=x+1) {
        output[(x*3)+0] = videoFrame[x];
        output[(x*3)+1] = videoFrame[x];
        output[(x*3)+2] = videoFrame[x];
    }
}

