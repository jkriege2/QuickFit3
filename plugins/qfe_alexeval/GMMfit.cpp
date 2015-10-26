/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "GMMfit.h"


double GMMfit::em_iteration()
{
//    std::cout<<"em_iteration"<<std::endl;
#ifdef TIMING
    tick();
#endif

//    2. Expectation step - setting responsibilities of data points:

    for (int j = 0; j<k; ++j) {
//        std::cout<<"mu "<<std::endl<<gm.at(j).getMu()<<std::endl;
        for (int i = 0; i<N; ++i) {
            responsibilities(i,j) = priors(j)*gm.at(j).p(xVec(i));
        }
    }
//    tockPrint("em:responsibilities");

//    tick();
//    std::cout<<"responsibilities "<<std::endl<<responsibilities<<std::endl;
    double responsibilities_sum=0;
    for (int i = 0; i<N; ++i) {
        responsibilities_sum=responsibilities.rowwise().sum()(i);
//        responsibilities_sum=0;
//        for (int j = 0; j<k; ++j) {
//            responsibilities_sum+=responsibilities(i,j);
//        }
        responsibilities.row(i) /= responsibilities_sum;
//        std::cout<<"responsibilities_sum "<<responsibilities_sum<<std::endl;
//        std::cout<<"responsibilities.col(j) "<<std::endl<<responsibilities.col(j)<<std::endl;
//        std::cout<<"responsibilities.row(i) "<<std::endl<<responsibilities.row(i)<<std::endl;
    }
//    tockPrint("em:responsibilities:normalize");
//    std::cout<<"responsibilities "<<responsibilities<<std::endl;
#ifdef TIMING
    tockPrint("expectation");
    tick();
#endif

//    3. Maximization - calculating new parameter values:

    Nk = responsibilities.colwise().sum();
//    std::cout<<"Nk"<<std::endl<<Nk<<std::endl;

    for (int j = 0; j<k; ++j) {
        tempMu.setZero(dim);
        tempSigma.setZero(dim,dim);
        for (int i = 0; i<N; ++i) {
            tempVecX=xVec(i);
            tempMu += tempVecX*responsibilities(i,j);

            tempSigma += responsibilities(i,j)*
                    (tempVecX-gm.at(j).getMu())*(tempVecX-gm.at(j).getMu()).transpose();
        }
        tempMu/=Nk(j);
        tempSigma/=Nk(j);
        gm[j].setMu(tempMu);
        gm[j].setSigma(tempSigma);
//        std::cout<<"mu"<<std::endl<<mu<<std::endl;
//        std::cout<<"sigma"<<std::endl<<sigma<<std::endl;
    }

    priors = Nk/N;
#ifdef TIMING
    tockPrint("em: maximization");
#endif

//    Calculating new loglikelihood
    double loglikelihood = 0;
    for (int j = 0; j<k; ++j) {
        for (int i = 0; i<N; ++i) {
            loglikelihood += log(priors(j))+gm.at(j).logP(xVec(i)); //FIXME gm.at(j).p(data[i]));

        }
    }
//    tockPrint("em_iteration");
    return loglikelihood;

}
void GMMfit::fit()
{
    if(N==0) return;
    responsibilities.resize(N,k);
#ifdef TIMING
    tick();
#endif

    // normalize priors
    double norm=0;
    for (int j = 0; j<k; ++j) {
        priors(j) = fabs(priors(j));
        norm += priors(j);
    }
    priors /=norm;

    /*******************************************
     * EM Algorithm for Gaussian Mixture Model *
     *******************************************/

//    Calculating initial loglikelihood
    double loglikelihood = 0;

    for (int i = 0; i<N; ++i) {
        for (int j = 0; j<k; ++j) {
            loglikelihood += log(priors(j))+gm.at(j).logP(xVec(i));
        }
    }
    options.logLikelihoodInitial=loglikelihood;

//    Threshold for the iteration (end iteration when difference of
//    new and previous loglikelihood is below)
    double diff = 2*options.threshold;
    int nIter=0; // number of iterations used

//    Iteration Loop for calculating best Gaussians
    double loglikelihood_old;
    while ( (diff > options.threshold) && (nIter < options.nIterMax) ) {
        loglikelihood_old = loglikelihood;
        loglikelihood = em_iteration();
        diff = fabs(loglikelihood_old - loglikelihood);
//        std::cout<<"diff = "<<diff<<std::endl;
        ++nIter;
        if(std::isinf(loglikelihood)|| std::isnan(loglikelihood)) {
            diff = INFINITY; // 2*options.threshold;
            std::cout<<"Warning: loglikelihood = "<<loglikelihood<<std::endl;
            loglikelihood=INFINITY; //FIXME
        }
    }
    options.nIter=nIter;
    options.logLikelihoodFinal=loglikelihood;
    options.logLikelihoodFinalDiff=diff;
    if(nIter==options.nIterMax) {
        std::cerr<<"Warning: GMM did NOT converge in "<<nIter<<" iterations"<<std::endl;
    }
#ifdef TIMING
    tockPrint("fit");
#endif

}

void GMMfit::setData(double *data, unsigned int N)
{
    if(N%dim) {
        std::cerr<<"ERROR N%dim = "<<N%dim<<std::endl;
    } else {
        this->data[0]=data;
        this->N=N/dim;
        useSingleArray=true;
    }
}

void GMMfit::setData(std::vector<double *> &data, unsigned int N)
{
    if((size_t)dim==data.size()) {
        this->data.clear();
        this->data=data;
        this->N=N;
        useSingleArray=false;
    } else {
        std::cerr<<"ERROR dim!=data.size() "<<std::endl;
    }
}

void GMMfit::setData(double **data, unsigned int N)
{
    this->data.clear();
    for(int d=0;d<dim;++d) {
        this->data.push_back(data[d]);
    }
    useSingleArray=false;
    this->N=N;
}


void GMMfit::init()
{
//    1. Random Initialization

//    Setting mean and covariance
    for (int j = 0; j<k; ++j) {
        gm.at(j).init();
    }

//    Setting the priors to random probabilities (positive, normalized to 1)
    priors=VectorXd::Random(k);
    double norm=0;
    for (int j = 0; j<k; ++j) {
        priors(j) = fabs(priors(j));
        norm += priors(j);
    }
    priors /=norm;

    // resizing temporary variables
    tempVecX.resize(dim);
    Nk.resize(k);
}

VectorXd &GMMfit::xVec(const int &index)
{
    if(index<N) { // FIXME?
        if(useSingleArray) {
            for(int d=0; d<dim;++d) {
                tempVecX(d)=data.front()[index*dim+d];
            }
        } else {
            for(int d=0; d<dim;++d) {
                tempVecX(d)=data.at(d)[index];
            }
        }
    } else {
        std::cerr<<"xVec: index error"<<std::endl;
    }
    return tempVecX;
}


GMMfit::GMMfit()
{
    this->dim=1;
    this->k=1;
    gm.resize(k,Gaussian(dim));
    init();
    data.push_back(NULL);
    N=0;
    useSingleArray=false;
}

GMMfit::GMMfit(int dim, int nComponents)
{
#ifdef TIMING
    tick();
#endif
    if(dim<1) {
        std::cerr<<"GMMfit: dimension must be at least 1"<<std::endl;
        dim=1;
    }
    if(nComponents<1) {
        std::cerr<<"GMMfit: nComponents must be at least 1"<<std::endl;
        nComponents=1;
    }

    this->dim=dim;
    this->k=nComponents;
    gm.resize(k,Gaussian(dim));
    init();
    data.push_back(NULL);
    N=0;
    useSingleArray=false;
#ifdef TIMING
    tockPrint("init");
#endif
}



void GMMfit::print() const
{
    std::cout<<toString()<<std::endl;
}

std::string GMMfit::toString() const
{
    std::stringstream out;
//    out<<"Gaussian Mixture Model \""<<name<<"\" in "<<dim<<" dimensions after "<<options.nIter<<" steps."<<std::endl;
    out<<"Gaussian Mixture Model in "<<dim<<" dimensions after "<<options.nIter<<" steps."<<std::endl;
    out<<"Number of mixture components = "<<k<<std::endl;
    out<<"Number of data points = "<<N<<std::endl;
    for(int i =0; i<k;++i) {
        out<<"component= "<<i+1<<std::endl;
        out<<"weight= "<<priors(i)<<std::endl;
//        out<<"Distribution:"<<std::endl;
        out<<gm.at(i).toString()<<std::endl;
        out<<"-------------------------------------------------------"<<std::endl;
    }
    out<<"final loglikelihood = "<< options.logLikelihoodFinal << " and difference = " << options.logLikelihoodFinalDiff <<std::endl;
    out<<"-------------------------------------------------------"<<std::endl;
    out<<"-------------------------------------------------------";
    return out.str();
}





void Gaussian::precalculate()
{
#ifdef TIMING
    tick();
#endif
//    sigmaLDLT=sigma.ldlt();
    precision=sigma.inverse();
    double det=sigma.determinant();
    norm =  sqrt(pow(2*M_PI,dim)*fabs(det));
    logNorm =  (dim*log(2*M_PI)+log(fabs(det)))/2;
#ifdef TIMING
    tockPrint("precalculate");
#endif
}

Gaussian::Gaussian(unsigned int dim)
{
    this->dim=dim;
    init();
}


bool Gaussian::checkDim() const
{
//    LUDecomposition<MatrixXd> lu(sigma); // lu.isInvertible() &&
    return  (sigma.cols()==dim) && (sigma.rows()==dim) && (mu.rows()==dim);
}

double Gaussian::p(const VectorXd& x)
{
#ifdef TIMING
    tick();
#endif
    double p = NAN;
    if(x.rows()==dim) {
//        tempVecX=sigmaLDLT.solve(x-mu);
        double quadraticForm = (x-mu).dot(precision*(x-mu));
        p=exp(-quadraticForm/2)/norm;
    }
#ifdef TIMING
    tockPrint("p");
#endif
    return p;
}

//double Gaussian::p(const double* x)
//{
//    for(int i=0;i<dim;++i) {
//        tempVecX(i)=x[i];
//    }
//    return p(tempVecX);
//}


double Gaussian::logP(const VectorXd& x)
{
    if(x.rows()==dim) {
//        tempVecX=sigmaLDLT.solve(x-mu); //  x = A.ldlt().solve(b); solves eq A*x=b for x by Cholesky decomposition
        // see http://eigen.tuxfamily.org/dox-devel/group__TutorialLinearAlgebra.html
//        double quadraticForm = (x-mu).dot(tempVecX);
        double quadraticForm = (x-mu).dot(precision*(x-mu));
        return -quadraticForm/2 - logNorm;
    } else return NAN;

}

//double Gaussian::logP(const double *x)
//{
//    for(int i=0;i<dim;++i) {
//        tempVecX(i)=x[i];
//    }
//    return logP(tempVecX);
//}

void Gaussian::setSigma(const MatrixXd &sigma)
{
    if((sigma.cols()==dim) && (sigma.rows()==dim)) {
        this->sigma=sigma;
        } else {
            std::cerr<<"setSigma: check dimensions"<<std::endl;
    }
    precalculate();
}


void Gaussian::setMu(const VectorXd &mu)
{
    if((mu.rows()==dim)) {
    this->mu=mu;
    } else {
        std::cerr<<"setMu: check dimensions"<<std::endl;
    }
}

void Gaussian::setMu(const double *mu)
{
    for(int i=0;i<dim;++i) {
        tempVecX(i)=mu[i];
    }
    this->mu=tempVecX;
}


void Gaussian::init()
{
//    Setting the mean values to random numbers
    mu=VectorXd::Random(dim);
    tempVecX.resize(dim);

//    Setting the covariance matrices to symmetrical square positive semi-definite matrices
    MatrixXd temp;
    temp=MatrixXd::Random(dim,dim);
    sigma=temp.transpose()*temp;
    precalculate();
}

void Gaussian::print() const
{
    std::cout<<toString()<<std::endl;
}

std::string Gaussian::toString() const
{
    std::stringstream out;
    out<<"---Gaussian distribution in "<<dim<<" dimensions" <<std::endl;
    out<<"mean value:"<<std::endl<<mu<<std::endl;
    out<<"covariance matrix sigma:"<<std::endl<<sigma;
    //    out<<std::endl<<"---";
    return out.str();
}


GMMfitOptions::GMMfitOptions()
{
    threshold=1e-2;
    nIter=0;
    nIterMax=100;
    logLikelihoodFinal=NAN;
    logLikelihoodInitial=NAN;
    logLikelihoodFinalDiff=NAN;
}

std::string GMMfitOptions::toString() const
{
    std::stringstream out;
    out<<"GMMfit Options"<<std::endl;
    out<<"threshold = "<<threshold<<std::endl;
    out<<"nIterMax = "<<nIterMax<<std::endl;
    out<<"nIter = "<<nIter<<std::endl;
    out<<"logLikelihoodFinalDiff = "<<logLikelihoodFinalDiff<<std::endl;
    out<<"logLikelihoodInitial = "<<logLikelihoodInitial<<std::endl;
    out<<"logLikelihoodFinal = "<<logLikelihoodFinal<<std::endl;

    return out.str();
}
