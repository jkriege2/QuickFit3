/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-06-22 12:12:54 +0200 (So, 22 Jun 2014) $  (revision $Rev: 3280 $)

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



#include "gnuplot_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>




bool gnuplotWriteBinaryArray(const char* filename, float* image, int width, int height, float* xvals, float* yvals) {
	FILE* fout=fopen(filename, "wb");
	//printf("gnuplotWriteBinaryArray: fout=%x\n", fout);
	if (fout) {
	    
		float length = height;
		fwrite((char *) &length, sizeof(float), 1, fout);
		//printf("gnuplotWriteBinaryArray: 2\n");
		if (yvals) {
			fwrite((char *) yvals, sizeof(float), height, fout);
		} else {
		    for (float j=0; j<height; j++) {
			    fwrite((char *) &j, sizeof(float), 1, fout);
			}
		}
		//printf("gnuplotWriteBinaryArray: 3\n");
		for (int j = 0; j < width; j++) {
			float val=j;
			if (xvals) val=xvals[j];
			fwrite((char *) &val, sizeof(float), 1, fout);
			for (int y=0; y<height; y++) {
			    const float val=image[y*width+j];
			    fwrite((char *) &val, sizeof(float), 1, fout);
			}
		}
		//printf("gnuplotWriteBinaryArray: 4\n");
		
	    fclose(fout);
		//printf("gnuplotWriteBinaryArray: 5\n");
		return true;
	}
	return false;
}



bool gnuplotWriteBinaryArray(const char* filename, double* image, int width, int height, double* xvals, double* yvals) {
	FILE* fout=fopen(filename, "wb");
	if (fout) {
	    
		float length = height;
		fwrite((char *) &length, sizeof(float), 1, fout);
		if (yvals) {
			for (int y=0; y<height; y++) {
			    const float val=yvals[y];
			    fwrite((char *) &val, sizeof(float), 1, fout);
			}
		} else {
		    for (float j=0; j<height; j++) {
			    fwrite((char *) &j, sizeof(float), 1, fout);
			}
		}
		for (int j = 0; j < width; j++) {
			float val=j;
			if (xvals) val=xvals[j];
			fwrite((char *) &val, sizeof(float), 1, fout);
			for (int y=0; y<height; y++) {
			    const float val=image[y*width+j];
			    fwrite((char *) &val, sizeof(float), 1, fout);
			}
		}
		
	    fclose(fout);
		return true;
	}
	return false;
}
