#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include "../../gnuplot_tools.h"

using namespace std;

#define WIDTH 128
#define HEIGHT 128
#define XPOS 64
#define YPOS 64
#define PAT_WIDTH 10
#define PAT_HEIGHT M_PI*10.0
#define PATTERNSIZE M_PI*25.0
#define SPEEDTEST_SIZE 100000
#define SPEEDTEST_OUTPUT 10

#define sqr(x) ((x)*(x))

int main(int argc, char *argv[]) {
	//printf("1\n");
    float* imagef=(float*)calloc(WIDTH*HEIGHT, sizeof(float));
	FILE* f;
	//printf("2\n");
	printf("GENERATE MATRIX\n");
    int i=0;
	for (int y=0; y<HEIGHT; y++) {
		for (int x=0; x<WIDTH; x++) {
            imagef[i]=0.5+0.5*cos(sqrt(sqr(double(x-XPOS))/double(PAT_WIDTH)+sqr(double(y-YPOS))/double(PAT_HEIGHT)));
            i++;
			//printf("3: %d\n",i);
        }
    }
	//printf("4\n");
	gnuplotWriteBinaryArray("gp_mat_float.bin", imagef, WIDTH, HEIGHT);
	
	printf("WRITE TEST 1\n");
    f=fopen("gp_mat_float.plt", "w");
	if (f) {
	    //printf("6\n");
		for (int plt=0; plt<2; plt++) {
			if (plt==0) {
				fprintf(f, "set terminal pdfcairo color solid font \",5\" linewidth 2 size 20cm,15cm\n" );
				fprintf(f, "set output \"gp_mat_float.pdf\"\n");
			} else if (plt==1) {
				fprintf(f, "set terminal wxt font \",5\"\n");
				fprintf(f, "set output\n");
			}

			fprintf(f, "set size noratio\n");
			fprintf(f, "set pm3d\n");
			for (int mp=0; mp<2; mp++) {
				
				fprintf(f, "splot \"gp_mat_float.bin\" binary matrix using 1:2:3 with pm3d\n");
				if (plt==1) fprintf(f, "pause -1\n");
			}
		}
		printf("7\n");
		fclose(f);
	}
    printf("CLEAN\n");
    free(imagef);
	printf("DONE\n");
    return 0;
}
