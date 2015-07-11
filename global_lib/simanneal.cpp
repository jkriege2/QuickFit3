#include <iostream>
#include "simanneal.h"
#include "tools.h"

fitSimAnneal::fitSimAnneal()
{
    constrained=true;
    iterationsMax=50000;
    fmax=0.0001;
    NS=40;
    rT=0.5;
    c=2;
    NT=50;
    Nepsilon=4;
    T0=0.001;
    v=NULL;
    x0=NULL;

    param=NULL;
    lowerBounds=NULL;
    upperBounds=NULL;
    rg=new MTRand;
}

fitSimAnneal::~fitSimAnneal()
{
    if (v!=NULL) free(v);
    if (param!=NULL) free(param);
    free(rg);
/*    if (x0!=NULL) free(x0);
    if (lowerBounds!=NULL) free(lowerBounds);
    if (upperBounds!=NULL) free(upperBounds);*/
}

void fitSimAnneal::init(chi2type chi2f, double* initParams, unsigned int pcount, double* lBounds, double* uBounds) {

    lowerBounds=lBounds;
    upperBounds=uBounds;

    // reset function evaluation and iteration counters
    chi2EvalCount=0;
    iterations=0;
    chi2=chi2f;
    fitParamCount=pcount;

    // init random number generator
    randSeed();

    param=(double*)realloc(param, fitParamCount*sizeof(double));
    memcpy(param, initParams, fitParamCount*sizeof(double));
    x0=(double*)realloc(x0, fitParamCount*sizeof(double));
    memcpy(x0, initParams, fitParamCount*sizeof(double));
    currentChi2=chi2(x0, fitParamCount);
    bestChi2=currentChi2;

    // init step vector v
    v=(double*)realloc(v, fitParamCount*sizeof(double));
    for (unsigned int i=0; i<fitParamCount; i++) {
        v[i]=fabs(upperBounds[i]-lowerBounds[i])/10.0;
    }
}





bool fitSimAnneal::fit() {
    int error=0;
    bool finished=false;
    //double newChi2;
    double T=T0*pow(10.0,floor(log10(chi2(param, fitParamCount))));
    int* dirVariations=(int*)calloc(fitParamCount, sizeof(int));
    double* fast=(double*)calloc(Nepsilon, sizeof(double));
    for (int i=0; i<Nepsilon; i++) {
        fast[i]=currentChi2;
    }
    int k=0;

    std::cout<<"  initial temperature: "<<T<<std::endl;
    do {
        for (unsigned int tempSteps=0; tempSteps<NT; tempSteps++) {
            // clear variations counter
            /*for (unsigned int i=0; i<fitParamCount; i++) {
                dirVariations[i]=0;
            }*/

            for (register unsigned int stepVariations=0; stepVariations<NS; stepVariations++) {
                iterations++;
                for (register unsigned int h=0; h<fitParamCount; h++) {
                    //std::cout<<h<<" ";

                    // x' = x + r*v[h] * eh
                    //     where eh is the unit vector in direction h
                    // for this ensure that a_h <= x'_h <= b_h
                    register double new_xh=0;
                    register double old_xh=x0[h];
                    do {
                        // r=random number from flat distribution between -1..1
                        new_xh=x0[h]+(ran(2.0)-1.0)*v[h];
                    } while (new_xh<lowerBounds[h] || new_xh>upperBounds[h]);
                    x0[h]=new_xh;
                    register double testChi2=chi2(x0, fitParamCount);
                    if (testChi2<currentChi2) {
                        // accept trial step
                        currentChi2=testChi2;
                        dirVariations[h]++;
                        //std::cout<<"      downhill trial accepted: "<<doublevectortostr(x0, fitParamCount)<<" <"<<currentChi2<<">";

                        // save best fits
                        if (currentChi2<bestChi2) {
                            //std::cout<<"   as optimal";
                            memcpy(param, x0, fitParamCount*sizeof(double));
                            bestChi2=testChi2;
                        }
                        //std::cout<<std::endl;
                    } else {
                        // Metropolis-Monte-Carlo step
                        if (ran()<exp((currentChi2-testChi2)/T)) {
                            // accept trial step
                            //memcpy(param, x0, fitParamCount*sizeof(double));
                            currentChi2=testChi2;
                            dirVariations[h]++;
                            //std::cout<<"      metropolis trial accepted: "<<doublevectortostr(x0, fitParamCount)<<" <"<<currentChi2<<">\n";
                        } else {
                            // reset to former value (old_xh)
                            x0[h]=old_xh;
                        }
                    }
                }
            }

            // update the step vector
            //std::cout<<"    NS = "<<NS<<"    dirVariations = (";
            for (register unsigned int h=0; h<fitParamCount; h++) {
                //if (h>0) std::cout<<", ";
                //std::cout<<dirVariations[h];
                if (dirVariations[h]>0.6*(double)NS) {
                    v[h]=v[h]*(1.0+c*(dirVariations[h]/(double)NS-0.6)/0.4);
                    if (v[h]>fabs(upperBounds[h]-lowerBounds[h])/2.0) v[h]=fabs(upperBounds[h]-lowerBounds[h])/2.0;
                } else if (dirVariations[h]<0.4*(double)NS) {
                    v[h]=v[h]/(1.0+c*(0.4-dirVariations[h]/(double)NS)/0.4);
                    if (v[h]>fabs(upperBounds[h]-lowerBounds[h])/2.0) v[h]=fabs(upperBounds[h]-lowerBounds[h])/2.0;
                }
                dirVariations[h]=0;

            }
            //std::cout<<")\n";
            //std::cout<<"    new step vector("<<tempSteps<<"/"<<NT<<"): "<<doublevectortostr(v, fitParamCount)<<std::endl;
        }
        // temperature update
        T=T*rT;
        std::cout<<"  new temperature: "<<T;//<<std::endl;

        // stop criterion
        fast[k]=currentChi2;
        finished=true;
        //std::cout<<"  fmax="<<fmax<<"     ";
        for (unsigned int u=0; (u<Nepsilon) && (finished); u++) {
            finished=finished && (fabs(currentChi2-fast[u])<=fmax);
            //std::cout<<"   |"<<currentChi2<<" - "<<fast[u]<<"|="<<fabs(currentChi2-fast[u]);
        }
        //std::cout<<std::endl;
        /*if (finished) {
            finished=(fast[k]-bestChi2)<=fmax;
        }*/
        k++; if (k>=Nepsilon) k=0;

        memcpy(x0, param, fitParamCount*sizeof(double));
        currentChi2=bestChi2;

        //std::cout<<"  current f*="<<doublevectortostr(fast, Nepsilon)<<std::endl;
        std::cout<<"  bestChi2="<<bestChi2<<"   bestParam="<<doublevectortostr(param, fitParamCount)<<"   iterations="<<iterations<<std::endl;

    } while (!finished && error==0 && iterations<iterationsMax);

    free(dirVariations);
    std::cout<<"best fit: ( ";
    for (int i=0; i<fitParamCount; i++) {
        if (i>0) std::cout<<", ";
        std::cout<<param[i];
    }
    std::cout<<" )\n";

    if (iterations>=iterationsMax) std::cout<<" stopped by maximum iterations.\n";
    if (finished) std::cout<<" stopped by abortion criterion f<sub>max</sub>.\n";

    if (error==-1) std::cout<<"ERROR: Not a Number (NaN) occured during chi2() function evaluation\n";
    if (error==-2) std::cout<<"ERROR: Infinity occured (Inf) during chi2() function evaluation\n";


    return (error==0);
}




