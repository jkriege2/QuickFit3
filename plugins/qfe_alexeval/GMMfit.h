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

#ifndef GMMFIT_H
#define GMMFIT_H

#include <Eigen/Eigen>
#include "vector"
#include <math.h>
#include <iostream>


//#define TIMING
#ifdef TIMING
#include "../ticktock.h"
#endif

using namespace Eigen;

struct GMMfitOptions;
class Gaussian;
class GMMfit;


class Gaussian
        #ifdef TIMING
        : public TickTock
        #endif
{
    int dim; ///< dimension
    MatrixXd sigma; ///< covariance matrix
    MatrixXd precision; ///< precision matrix = sigma inverse
//    LDLT<MatrixXd> sigmaLDLT; ///< Cholesky decomposition of sigma
    double norm; ///< mormalization of the distribution
    double logNorm;
    VectorXd mu; ///< vector of mean values

    VectorXd tempVecX;
    void precalculate();

public:
    Gaussian(unsigned int dim);
    bool checkDim() const;
    /**
     * @brief p calculates the probability density function (pdf) of a Gaussian.
     * @param x a vector of dimension \a dim
     * @return probability at point x
     */
    double p(const VectorXd& x);
    //double p(const double* x);

    /** Use this function to calculate loglikelihoods with higher accuracy
     * @brief p calculates the logarithm of the probability density function (pdf) of a Gaussian.
     * @param x a vector of dimension \a dim
     * @return logarithm of the probability at point x
     */
    double logP(const VectorXd& x);
    //double logP(const double* x);

    /// set covariance matrix
    void setSigma(const MatrixXd &sigma);
    inline const MatrixXd getSigma() const { return sigma;}
    /// set the vector of mean values
    void setMu(const VectorXd &mu);
    void setMu(const double* mu);
    inline const VectorXd getMu() const {return mu;}
    /// random initialization
    void init();
    /// print an overview of the Gaussian to \a std::cout
    void print() const;
    /// print an overview of the Gaussian to a \a std::string
    std::string toString() const;
};


struct GMMfitOptions
{
    double threshold; ///< threshold for loglikelihood difference
    int nIter; ///< number of iterations used
    int nIterMax; ///< maximal number of iterations
    double logLikelihoodFinal;
    double logLikelihoodInitial;
    double logLikelihoodFinalDiff;
    GMMfitOptions();
    std::string toString() const;
};

class GMMfit
        #ifdef TIMING
        : public TickTock
        #endif
{
    int dim; ///< dimension
    int k; ///< mixture components
    VectorXd priors; ///< the mixture weights
    std::vector<Gaussian> gm; ///< mixture of Gaussians

    double em_iteration();
    void init(); /// random initialization

    bool useSingleArray; ///< set to true if you use a single array for the data points.
    std::vector<double *> data; ///< contains \a dim pointers to \a N data points
    int N; ///< the number of data points. Note: The number of values in a single array \a data must be \a N*dim
    VectorXd& xVec(const int &index);

    /// temporary variables
    // FIXME: destroy after they have been used
    VectorXd tempVecX;
    Eigen::MatrixXd responsibilities; // column major in Eigen by default
    VectorXd tempMu;
    MatrixXd tempSigma;
    VectorXd Nk;

    /// contains the fitting options like loglikelihood threshold, maximal number of iterations, ...
    GMMfitOptions options;

public:
    GMMfit();

    /**
     * @brief GMMfit initializes a Gaussian Mixture Model (GMM) with k components in dim dimensions
     * @param dim dimension of the data points
     * @param nComponents number of mixing components
     */
    GMMfit(int dim, int nComponents);

    /**
     * @brief fit uses the EM-Algorithm to fit a Gaussian Mixture Model (GMM) of \a k components to \a N data points
     * compare for example \link http://en.wikipedia.org/wiki/Mixture_model#Expectation_maximization_.28EM.29
     */
    void fit();

    /** use this for data in a single array only
     * @brief setData set data points
     * @param data pointer to the data array
     * @param N the size of the array. Note that the number of data points equals \a N/dim
     */
    void setData(double* data, unsigned int N);
    void setData(std::vector<double *> &data, unsigned int N);
    /** use this if you have a seperate array for each dimension
     * @brief setData set data points
     * @param data pointer to an array with \a dim dimensions
     * @param N the number of data points in each array
     */
    void setData(double** data, unsigned int N);

    /// returns a reference to the Gaussian distribution of the \a k's component.
    /// Use this to set starting parameters of mu and sigma.
    inline Gaussian& getGauss(int k) {return gm[k];}
    inline const Gaussian& getGauss(int k) const {return gm[k];}
    inline VectorXd& getPriors() {return priors;}

    inline double getPrior(int k) const {return priors(k);}
    inline double getMu( int k, int dim) const {return gm.at(k).getMu()(dim);}

    void setUseSingleArray(bool useSingleArray) {this->useSingleArray=useSingleArray;}
    int getN() const {return N;}
    int getDim() const {return dim;}
    int getNumberOfComponents() const {return k;}
    void setOptions(GMMfitOptions &options) { this->options=options;}
    GMMfitOptions getOptions() const { return this->options;}
//    std::string getName() const {return name;}
//    void setName(const std::string _name) {name=_name;}

    /// print the current GMM to \a std::cout
    void print() const;
    /// print the current GMM to a \a std::string
    std::string toString() const;
};






#endif // GMMFIT_H
