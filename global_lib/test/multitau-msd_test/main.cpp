#include <iostream>
#include <cstdio>
#include <stdint.h>
#include "../../multitau-msd.h"
#include "../../statistics_tools.h"
#include "../../highrestimer.h"
#include "../../MersenneTwister.h"
#include "../../tools.h"

using namespace std;

#define DATATYPE double
#define STEPS 10000000

double step_mean=0;
double step_var=1;
double msd_P=8;
double msd_m=2;
double msd_S=15;
double jump=1;


int main() {
    MTRand rand(12345);
    FILE* pFile=NULL;

    printf("\n\nCREATING RANDOM WALK TIMESERIES: =========================================\n");

    uint64_t* tau=(uint64_t*)calloc(msd_P*msd_S,sizeof(uint64_t));
    double* msd=(double*)calloc(msd_P*msd_S,sizeof(double));
    statisticsAutocorrelateCreateMultiTau64(tau, msd_S, msd_m, msd_P);
    DATATYPE* x=(DATATYPE*)calloc(STEPS,sizeof(DATATYPE));
    DATATYPE* y=(DATATYPE*)calloc(STEPS,sizeof(DATATYPE));
    x[0]=0;
    y[0]=0;
    for (uint64_t i=1; i<STEPS; i++) {
        //x[i]=x[i-1]+rand.randNorm(step_mean, step_var);
        if (rand.rand()<0.5) x[i]=x[i-1]+jump;
        else  x[i]=x[i-1]-jump;
        if (rand.rand()<0.5) y[i]=y[i-1]+jump;
        else  y[i]=y[i-1]-jump;
    }

        statisticsMSDMultiTau1D<DATATYPE, DATATYPE>(msd, x, STEPS, tau, msd_P*msd_S, 1);
        pFile = fopen("msd_exact.txt","w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<msd_P*msd_S; i++) {
                fprintf(pFile, "%lg, %lg\n", (double)tau[i], (double)msd[i]);
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        statisticsMSDMultiTau2D<DATATYPE, DATATYPE>(msd, x, y, STEPS, tau, msd_P*msd_S, 1);
        pFile = fopen("msd_exact2D.txt","w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<msd_P*msd_S; i++) {
                fprintf(pFile, "%lg, %lg\n", (double)tau[i], (double)msd[i]);
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }


        MultiTauMSD<double> msdo(msd_S, msd_m, msd_P, 1);
        for (uint64_t i=1; i<STEPS; i++) {
            msdo.contribute_step(x[i]);
        }
        msdo.normalize();
        double* taus=msdo.getMSDTau();
        double* msd2=msdo.getMSD();
        pFile = fopen("msd_mtau.txt","w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<msdo.getSlots(); i++) {
                fprintf(pFile, "%lg, %lg\n", taus[i], (double)msd2[i]);
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        MultiTauMSD<double> msdo2(msd_S, msd_m, msd_P, 1);
        for (uint64_t i=1; i<STEPS; i++) {
            msdo2.contribute_step(x[i], y[i]);
        }
        msdo2.normalize();
        taus=msdo2.getMSDTau();
        msd2=msdo2.getMSD();
        pFile = fopen("msd_mtau2D.txt","w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<msdo2.getSlots(); i++) {
                fprintf(pFile, "%lg, %lg\n", taus[i], (double)msd2[i]);
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

    free(x);
    free(tau);
    free(msd);

        printf("\n\nWRITING GNUPLOT FILE: ====================================================\n");
        pFile = fopen("msd.plt","w");
        if (pFile!=NULL) {
            fprintf(pFile, "fn(x)=2*Dn*x\n");
            fprintf(pFile, "Dn=1\n");
            fprintf(pFile, "fn2(x)=2*Dn2*x\n");
            fprintf(pFile, "Dn2=1\n");
            fprintf(pFile, "fit fn(x) 'msd_exact.txt' via Dn\n");
            fprintf(pFile, "fit fn2(x) 'msd_mtau.txt' via Dn2\n");
            fprintf(pFile, "fn_2(x)=4*Dn_2*x\n");
            fprintf(pFile, "Dn_2=1\n");
            fprintf(pFile, "fn2_2(x)=4*Dn2_2*x\n");
            fprintf(pFile, "Dn2_2=1\n");
            fprintf(pFile, "fit fn_2(x) 'msd_exact2D.txt' via Dn_2\n");
            fprintf(pFile, "fit fn2_2(x) 'msd_mtau2D.txt' via Dn2_2\n");
            for (int i=0; i<2; i++) {
                if (i==0) {
                    fprintf(pFile, "set terminal pdfcairo color solid font \"Arial,12\" linewidth 2 size 20cm,20cm\n");
                    fprintf(pFile, "set output \"msd.pdf\"\n");
                } else {
                    fprintf(pFile, "set terminal wxt\n");
                    fprintf(pFile, "set output \n");
                }
                fprintf(pFile, "set logscale x\n");
                fprintf(pFile, "set logscale y\n");
                fprintf(pFile, "set samples 10000\n");
                fprintf(pFile, "set title '1D random walk'\n");
                fprintf(pFile, "plot fn(x) title sprintf('2*%%f*tau', Dn) with lines, \\\n");
                fprintf(pFile, "     \"msd_exact.txt\" title 'statisticsMSDMultiTau1D' with points, \\\n");
                fprintf(pFile, "     fn2(x) title sprintf('2*%%f*tau', Dn2) with lines, \\\n");
                fprintf(pFile, "     \"msd_mtau.txt\" title 'multitau-msd' with points  \n");
                if (i==1) fprintf(pFile, "pause -1\n");

                fprintf(pFile, "set title '1D random walk'\n");
                fprintf(pFile, "plot fn_2(x) title sprintf('2*%%f*tau', Dn_2) with lines, \\\n");
                fprintf(pFile, "     \"msd_exact2D.txt\" title 'statisticsMSDMultiTau1D' with points, \\\n");
                fprintf(pFile, "     fn2_2(x) title sprintf('2*%%f*tau', Dn2_2) with lines, \\\n");
                fprintf(pFile, "     \"msd_mtau2D.txt\" title 'multitau-msd' with points  \n");

                if (i==1) fprintf(pFile, "pause -1\n");

            }
            fclose (pFile);

        }
        std::string gnuplot="C:\\Programme\\gnuplot\\binary\\wgnuplot.exe";
        if (gnuplot.size()>0) {
            std::string f="msd.plt";
            std::cout<<"running gnuplot on '"<<f<<"' ... ";
            system((gnuplot+" "+f).c_str());
            std::cout<<"OK\n";
        }

    return 0;
}
