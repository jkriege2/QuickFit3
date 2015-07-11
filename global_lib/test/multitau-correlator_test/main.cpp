#include <iostream>
#include <cstdio>
#include <stdint.h>
#include "../../multitau-correlator.h"
#include "../../correlator_multitau.h"
#include "../../statistics_tools.h"
#include "../../highrestimer.h"
#include "../../MersenneTwister.h"
#include "../../tools.h"

using namespace std;

#define DATATYPE double



int main() {

    HighResTimer timer;
    DATATYPE period=151;
    long Nflush=0;
    long Nsim=100000;
    long N=Nsim+Nflush;
    DATATYPE A=1e-5;

    int S=10;
    int m=2;
    int P=16;

    MTRand rand1;
    MTRand rand_p0(12345);
    MTRand rand_p1(98765);

    int randprocess_period=N/100;

    for (int mode=0; mode<7; mode++) {
        DATATYPE* timetrace=new DATATYPE [N];
        DATATYPE* timetrace2=new DATATYPE [N];

        MultiTauCorrelator<DATATYPE, DATATYPE> corr(S,m,P,1);
        correlatorjb<DATATYPE, DATATYPE> corrjb(S, P);

        MultiTauCorrelator<DATATYPE, DATATYPE> ccorr(S,m,P,1);
        correlatorjb<DATATYPE, DATATYPE> ccorrjb(S, P);

        long* taus=(long*)malloc(S*P*sizeof(long));
        statisticsAutocorrelateCreateMultiTau(taus, S, m, P);
        double* acf=(double*)malloc(S*P*sizeof(double));
        double* ccf=(double*)malloc(S*P*sizeof(double));

        if (mode==0) {
            printf("\n\n======================================================================================\nTEST 1: SINE/COSINE ==================================================================\n======================================================================================\n");
            for (long i=0; i<N; i++) {
                DATATYPE t=(DATATYPE)i*M_PI/period;
                DATATYPE v=(1.0+sin(t))*A;
                DATATYPE v2=(1.0+cos(t))*A;

                if (i>=Nsim) v=v2=0;
                timetrace[i]=v;
                timetrace2[i]=v2;



            }
        } else if (mode>0 && mode<7) {
            int randprocess_period=pow(10,mode-1);
            printf("\n\n======================================================================================\nTEST 2: DECAYING CORRELATION ==================================================================\n======================================================================================\n");
            DATATYPE p0=0;
            for (long i=0; i<N; i++) {
                DATATYPE t=(DATATYPE)i*M_PI/period;

                DATATYPE v=p0+2.0*rand1.rand()-1.0;
                DATATYPE v2=p0+2.0*rand1.rand()-1.0;
                if (i>=Nsim) v=v2=0;


                timetrace[i]=v;
                timetrace2[i]=v2;

                if (i%randprocess_period==0) {
                    p0=rand_p0.randNorm(5, 2);
                }


            }
        }




        printf("\n\nSIMPLE: ==================================================================\n");
        timer.start();
        statisticsAutocorrelateMultiTauSymmetric(acf, timetrace, N, taus, S*P);
        printf("  ACF: %lf ms\n", timer.get_time()/1e3);
        FILE* pFile = fopen(format("%dacorr_simple.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<S*P; i++) {
                if (mode==0) {
                    DATATYPE e=acf[i]-1.0-0.5*cos(taus[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)taus[i], (double)acf[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)taus[i], (double)acf[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        timer.start();
        statisticsCrosscorrelateMultiTauSymmetric(ccf, timetrace2, timetrace, N, taus, S*P);
        printf("  CCF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dccorr_simple.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<S*P; i++) {
                if (mode==0) {
                    DATATYPE e=ccf[i]-1.0+0.5*sin(taus[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)taus[i], (double)ccf[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)taus[i], (double)ccf[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }



        printf("\n\nSIMPLE AVG: ==============================================================\n");
        timer.start();
        statisticsAutocorrelateMultiTauAvgSymmetric<DATATYPE, DATATYPE>(acf, timetrace, N, S, m, P);
        printf("  ACF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dacorr_simpleavg.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<S*P; i++) {
                if (mode==0) {
                    DATATYPE e=acf[i]-1.0-0.5*cos(taus[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)taus[i], (double)acf[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)taus[i], (double)acf[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        timer.start();
        statisticsCrosscorrelateMultiTauAvgSymmetric<DATATYPE, DATATYPE>(ccf, timetrace2, timetrace, N, S, m, P);
        printf("  CCF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dccorr_simpleavg.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<S*P; i++) {
                if (mode==0) {
                    double e=ccf[i]-1.0+0.5*sin(taus[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)taus[i], (double)ccf[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)taus[i], (double)ccf[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        printf("\n\nJK: ======================================================================\n");
        timer.start();
        for (long i=0; i<N; i++) {
            const DATATYPE v=timetrace[i];
            const DATATYPE v2=timetrace2[i];
            corr.correlate_step(v);
        }
        corr.normalize();
        double* tau=corr.getCorTau();
        double* c=corr.getCor();
        unsigned int cN=corr.getSlots();
        printf("  ACF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dacorr_jk.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<cN; i++) {
                if (mode==0) {
                    double e=c[i]-1.0-0.5*cos(tau[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)tau[i], (double)c[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)tau[i], (double)c[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }

        timer.start();
        for (long i=0; i<N; i++) {
            const DATATYPE v=timetrace[i];
            const DATATYPE v2=timetrace2[i];
            ccorr.crosscorrelate_step(v,v2);
        }
        ccorr.crossnormalize();
        tau=ccorr.getCorTau();
        c=ccorr.getCor();
        cN=ccorr.getSlots();
        printf("  CCF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dccorr_jk.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            for (unsigned int i=0; i<cN; i++) {
                if (mode==0) {
                    double e=c[i]-1.0+0.5*sin(tau[i]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)tau[i], (double)c[i], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)tau[i], (double)c[i]);
                }
            }
            fprintf(pFile, "\n\n");
            fclose (pFile);
        }


        printf("\n\nJB: ======================================================================\n");
        timer.start();
        for (long i=0; i<N; i++) {
            const DATATYPE v=timetrace[i];
            const DATATYPE v2=timetrace2[i];
                corrjb.run(v,v);
        }
        printf("  ACF: %lf ms\n", timer.get_time()/1e3);


        pFile = fopen(format("%dacorr_jb.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            DATATYPE** corr1=corrjb.get_array_G();
            for (int j=0; j<S*P; j++) {
                if (mode==0) {
                    DATATYPE e=corr1[1][j]-1.0-0.5*cos(corr1[0][j]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", corr1[0][j], corr1[1][j], e, fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", corr1[0][j], corr1[1][j]);
                }
            }
            fprintf(pFile, "\n\n");
            free(corr1[0]);
            free(corr1[1]);
            fclose (pFile);
        }

        timer.start();
        for (long i=0; i<N; i++) {
            const DATATYPE v=timetrace[i];
            const DATATYPE v2=timetrace2[i];
                ccorrjb.run(v2,v);
        }
        printf("  CCF: %lf ms\n", timer.get_time()/1e3);
        pFile = fopen(format("%dccorr_jb.txt", mode).c_str(),"w");
        if (pFile!=NULL) {
            DATATYPE** corr1=ccorrjb.get_array_G();
            for (int j=0; j<S*P; j++) {
                if (mode==0) {
                    DATATYPE e=corr1[1][j]-1.0+0.5*sin(corr1[0][j]*M_PI/period);
                    fprintf(pFile, "%lg, %lg, %lg, %lg\n", (double)corr1[0][j], (double)corr1[1][j], (double)e, (double)fabs(e));
                } else {
                    fprintf(pFile, "%lg, %lg\n", (double)corr1[0][j], (double)corr1[1][j]);
                }
            }
            fprintf(pFile, "\n\n");
            free(corr1[0]);
            free(corr1[1]);
            fclose (pFile);
        }



        printf("\n\nWRITING GNUPLOT FILE: ====================================================\n");
        pFile = fopen (format("%dcorr.plt", mode).c_str(),"w");
        if (pFile!=NULL) {
            if (mode==0) {
                for (int i=0; i<2; i++) {
                    if (i==0) {
                        fprintf(pFile, "set terminal pdfcairo color solid font \"Arial,12\" linewidth 2 size 20cm,20cm\n");
                        fprintf(pFile, "set output \"%dcorr.pdf\"\n", mode);
                    } else {
                        fprintf(pFile, "set terminal wxt\n");
                        fprintf(pFile, "set output \n");
                    }
                    fprintf(pFile, "set multiplot layout 3,1 title \"Autocorrelation\"\n");
                    fprintf(pFile, "set logscale x\n");
                    fprintf(pFile, "set samples 10000\n");
                    fprintf(pFile, "acf(x)=1.0+0.5*cos(x*pi/%lf)\n", period);
                    fprintf(pFile, "plot acf(x) title 'theoretical' with lines ls 1, \\\n");
                    fprintf(pFile, "     \"%dacorr_simple.txt\" title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_simpleavg.txt\" title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jk.txt\" title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jb.txt\" title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "plot \"%dacorr_simple.txt\" using 1:3 title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_simpleavg.txt\" using 1:3 title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jk.txt\" using 1:3 title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jb.txt\" using 1:3 title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "set logscale y\n");
                    fprintf(pFile, "plot \"%dacorr_simple.txt\" using 1:4 title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_simpleavg.txt\" using 1:4 title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jk.txt\" using 1:4 title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jb.txt\" using 1:4 title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "unset logscale y\n");
                    fprintf(pFile, "unset multiplot\n");
                    if (i==1) fprintf(pFile, "pause -1\n");

                    fprintf(pFile, "set multiplot layout 3,1 title \"Crosscorrelation\"\n");
                    fprintf(pFile, "set logscale x\n");
                    fprintf(pFile, "set samples 10000\n");
                    fprintf(pFile, "ccf(x)=1.0-0.5*sin(x*pi/%lf)\n", period);
                    fprintf(pFile, "plot ccf(x) title 'theoretical' with lines ls 1, \\\n");
                    fprintf(pFile, "     \"%dccorr_simple.txt\" title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_simpleavg.txt\" title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jk.txt\" title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jb.txt\" title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "plot \"%dccorr_simple.txt\" using 1:3 title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_simpleavg.txt\" using 1:3 title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jk.txt\" using 1:3 title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jb.txt\" using 1:3 title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "set logscale y\n");
                    fprintf(pFile, "plot \"%dccorr_simple.txt\" using 1:4 title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_simpleavg.txt\" using 1:4 title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jk.txt\" using 1:4 title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jb.txt\" using 1:4 title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "unset logscale y\n");
                    fprintf(pFile, "unset multiplot\n");
                    if (i==1) fprintf(pFile, "pause -1\n");
                }
                fclose (pFile);
            } else {
                for (int i=0; i<2; i++) {
                    if (i==0) {
                        fprintf(pFile, "set terminal pdfcairo color solid font \"Arial,12\" linewidth 2 size 20cm,20cm\n");
                        fprintf(pFile, "set output \"%dcorr.pdf\"\n", mode);
                    } else {
                        fprintf(pFile, "set terminal wxt\n");
                        fprintf(pFile, "set output \n");
                    }
                    fprintf(pFile, "set multiplot layout 1,1 title \"Autocorrelation\"\n");
                    fprintf(pFile, "set logscale x\n");
                    fprintf(pFile, "set samples 10000\n");
                    fprintf(pFile, "plot \"%dacorr_simple.txt\" title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_simpleavg.txt\" title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jk.txt\" title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dacorr_jb.txt\" title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "unset multiplot\n");
                    if (i==1) fprintf(pFile, "pause -1\n");

                    fprintf(pFile, "set multiplot layout 1,1 title \"Crosscorrelation\"\n");
                    fprintf(pFile, "set logscale x\n");
                    fprintf(pFile, "set samples 10000\n");
                    fprintf(pFile, "plot \"%dccorr_simple.txt\" title 'simple' with points ls 2, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_simpleavg.txt\" title 'simple, avg' with points ls 5, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jk.txt\" title 'jk' with points ls 3, \\\n", mode);
                    fprintf(pFile, "     \"%dccorr_jb.txt\" title 'jb' with points ls 4\n", mode);
                    fprintf(pFile, "unset multiplot\n");
                    if (i==1) fprintf(pFile, "pause -1\n");
                }
                fclose (pFile);
            }
        }
        std::string gnuplot="C:\\Programme\\gnuplot\\binary\\wgnuplot.exe";
        if (gnuplot.size()>0) {
            std::string f=format("%dcorr.plt", mode);
            std::cout<<"running gnuplot on '"<<f<<"' ... ";
            system((gnuplot+" "+f).c_str());
            std::cout<<"OK\n";
        }

        delete[] timetrace;
        delete[] timetrace2;
        free(taus);
        free(acf);
        free(ccf);
    }
    return 0;
}
