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



#include "statistics_tools.h"



void statisticsAutocorrelate(double* dataout, const double* data, uint32_t N) {
    if (N<=0) return;
    register double avg=statisticsAverage<double>(data, N);
    register uint32_t kmax=ceil((double)N/2.0);
    for (register uint32_t k=0; k<kmax; k++) {
        register double sum=0;
        register uint32_t i;
        //#pragma omp simd reduction(+:sum)
        for ( i=0; i<(N-k); i++) {
            sum=sum+(data[i]-avg)*(data[i+k]-avg);
        }
        dataout[k]=sum/(double)N;
    }
}

double* statisticsAllocAutocorrelate(const double* data, long long N) {
    double* d=(double*)statisticsMalloc(ceil((double)N/2.0)*sizeof(double));
    statisticsAutocorrelate(d, data, N);
    return d;
}


void statisticsAutocorrelateCreateMultiTau(long* tau, int S, int m, int P) {
    int deltatau=1;
    long i=0;
    tau[0]=deltatau;
    for (long s=0; s<S; s++) {
        for (long p=0; p<P; p++) {
            if (i>0) {
                tau[i]=tau[i-1]+deltatau;
            };
            i++;
        }
        deltatau=deltatau*m;
    }
}

void statisticsAutocorrelateCreateMultiTau(double* tau, int S, int m, int P, double deltaT) {
    double deltatau=deltaT;
    long i=0;
    tau[0]=deltatau;
    for (long s=0; s<S; s++) {
        for (long p=0; p<P; p++) {
            if (i>0) {
                tau[i]=tau[i-1]+deltatau;
            }
            i++;
        }
        deltatau=deltatau*double(m);
    }
}
void statisticsAutocorrelateCreateMultiTau64(uint64_t* tau, int S, int m, int P) {
    uint64_t deltatau=1;
    uint64_t i=0;
    tau[0]=deltatau;
    for (int64_t s=0; s<S; s++) {
        for (int64_t p=0; p<P; p++) {
            if (i>0) {
                tau[i]=tau[i-1]+deltatau;
            }
            i++;
        }
        deltatau=deltatau*m;
    }
}

double statisticsKernel1DGaussian(double t) {
    return exp(-0.5*t*t)/STATISTICS_SQRT_2PI;
}


double statisticsKernel1DCauchy(double t) {
    return 1.0/(M_PI*(1.0+t*t));
}



double statisticsKernel1DPicard(double t) {
    return exp(-0.5*fabs(t))/2.0;
}


double statisticsKernel1DEpanechnikov(double t) {
    return (fabs(t)<1.0)?(0.75*(1.0-t*t)):0.0;
}


double statisticsKernel1DUniform(double t) {
    return (fabs(t)<1.0)?0.5:0.0;
}


double statisticsKernel1DTriangle(double t) {
    return (fabs(t)<1.0)?(0.75*(1.0-t*t)):0.0;
}



double statisticsKernel1DQuartic(double t) {
    return (fabs(t)<1.0)?(15.0/16.0*statisticsSqr(1.0-t*t)):0.0;
}


double statisticsKernel1DTriweight(double t) {
    return (fabs(t)<1.0)?(35.0/32.0*statisticsCube(1.0-t*t)):0.0;
}



double statisticsKernel1DTricube(double t) {
    return (fabs(t)<1.0)?(70.0/81.0*statisticsCube(1.0-statisticsCube(fabs(t)))):0.0;
}


double statisticsKernel1DCosine(double t) {
    return (fabs(t)<1.0)?(M_PI/4.0*cos(t*M_PI/2.0)):0.0;
}


double statisticsKernel2DGaussian(double tx, double ty)
{
    return exp(-0.5*(tx*tx+ty*ty))/(2.0*M_PI);
}

double statisticsKernel2DUniform(double tx, double ty) {
    return (fabs(tx)<1.0 && fabs(ty)<=1.0)?0.25:0.0;
}
