/*
    Copyright (c) 2008-2014 Nikolas Schnellbächer &
                            Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

/** \file libb040mem.h
*/
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include "Eigen/Dense"
#include "Eigen/SVD"
#include <stdint.h>

#include <QDebug>

#ifndef LIBB040MEM_H
#define LIBB040MEM_H


#define FloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))
#define sqr(x) ((x)*(x))


/*! \defgroup libb040mem Maximum Entropy for FCS

    Group description
 */

/*! \brief performs MaxEnt evalualtion
    \ingroup libb040mem

    This functions performs maxent \f$ \chi^2 \f$ for you:
    \f[  \chi^2=\sum\limits_{i=1}^N\frac{(m_i-f(\tau_i, \vec{p}_i)^2}{\sigma_i^2} \f]
    \param data input data
    \param[out] dataout output data
    \return the result
*/







class MaxEntB040{

    public:
    	/** Default constructor */
        MaxEntB040();
        /** Default destructor */
        virtual ~MaxEntB040();

        enum TauMode {
            tmInputTaus=0,
            tmLog=1,
            tmLin=2,
            tmDCoeffLog=3,
            tmDCoeffLin=4
        };



        void setData(const double* taus, const double* correlation,\
        	const double* weights,unsigned long long Nd,int rangeMinDatarange,\
            int rangeMaxDatarange,uint32_t Ndist, double * dist,double * distTaus,\
            int model,int parameterCount, double *param_list, double wxy, TauMode tauMode=tmInputTaus, double tauMin=0, double tauMax=0);

		void writeDistribution(double * dist);
		
		void writeDistTaus(double * distTaus);

        bool run(double alpha,int NumIter,double * param_list,int model,int parameter_count);

        static void evaluateModel(double * taus, double *modelEval, uint32_t N, double* distTaus, double* dist, \
                           uint32_t Ndist, double* param_list, int model, double wxy);





    protected:
        //protected member variables
        TauMode tauMode;
        double tauMin;
        double tauMax;
        bool m_oldDist;
        int m_Nd;
        int m_N;
        int m_cutoff;
		int m_s;
        double m_kappa;
        double m_alpha;
        double m_tripTau;
        double m_tripTheta;
        double m_wxy;
        double m_wz;
        double m_a;
        int NumIter;
        int m_model;
        Eigen::VectorXd m_tau;
        Eigen::VectorXd m_xdata;
        Eigen::VectorXd m_ydata;
		Eigen::VectorXd m_stdev;
		Eigen::VectorXd m_taudiffs;
		Eigen::MatrixXd m_T;
		Eigen::VectorXd m_svals;
		Eigen::MatrixXd m_V;
		Eigen::MatrixXd m_U;
		Eigen::MatrixXd m_S;
		Eigen::MatrixXd m_Sred;
		Eigen::MatrixXd m_Vred;
		Eigen::MatrixXd m_Ured;
		Eigen::VectorXd m_svalsred;
		Eigen::MatrixXd m_M;
		Eigen::MatrixXd m_sigmamatrix;
		Eigen::VectorXd m_m;
		Eigen::VectorXd m_f;
		Eigen::VectorXd m_u;
		Eigen::MatrixXd m_K;
		Eigen::VectorXd m_F;
		Eigen::VectorXd m_g;
        Eigen::VectorXd m_paramStore;
		
        


        //protected member methods

    void setTauGrid();

/*! \brief The function setTmatrix sets the T matrix, a linear transform matrix betwenn the data
    \f$ \vec{d} \f$ and the image \f$ \vec{f} \f$
    such that \f$ \vec{d}=T\vec{f} \f$.


    \ingroup libb040mem
    The T matrix has the size \f$ T \in \mathbb{R}^{Nd \times N} \f$, where \f$ Nd \f$ is the number of
    data points and \f$ N \f$ is the number of discretizations steps of the distribution.

    \f[  T_{ij}=g(\tau_{i},\tau_{diff,j})=\left(1+\frac{\tau_i}{\tau_{diff,j}}\right)^{-1}
    \left(1+\frac{\tau_i}{\tau_{diff,j}\kappa^2}\right)^{-1/2} \f]
    with  \f$ i=1,\dots ,Nd \quad j=1,\dots ,N \f$

    \param data input data
    \param[out] dataout output data
    \return the result
*/
    void setTmatrix();

/*! \brief This function performs the singular value decomposition (SVD) of the matrix T
    using the GSL routine "gsl_linalg_SV_decomp".

    \ingroup libb040mem
    The given matrix T is decomposed in the following way:
    \f[ T=V\Sigma U^{T}    \f]

    The implemented svd function has the following syntax:
    \code
    Function: int gsl_linalg_SV_decomp (gsl_matrix * A, gsl_matrix * V, gsl_vector * S, gsl_vector * work)
    \endcode
    and requires the
    \code
     #include <gsl/gsl_linalg.h>
    \endcode
    library to be included.
    \param data input data
    \param[out] dataout output data
    \return the result
*/
    void performSVD();

/*! \brief This function sets the Matrix M, which remains constant throughout the entire Maximum Entropy Iteration.

    \ingroup libb040mem
    M is composed in the following way:
    \f[ M=\Sigma^{T} V^{T} W V \Sigma   \f]

    Where  \f$ W \f$ is given by:
    \f[ W=\frac{\partial^2 L(\vec{F},\vec{D})}{\partial \vec{F}^2}=diag\{\sigma_1^2,\dots , \sigma_{N_d}^2 \} \f]
    The matrix products are carried out by BLAS Level 3 routines, namely by the command:
    \code
    int gsl_blas_fgemm(CblasNoTrans,CblasNoTrans,1.0,A,B,0.0,C);
    \endcode
    which does \f$ C=AB \f$.



    \param data input data
    \param[out] dataout output data
    \return the result
*/

    void setMmatrix();
    
    void performIter();

    void diagK();

    void Mmatrix(Eigen::MatrixXd m,int Nd, Eigen::MatrixXd Vred,Eigen::MatrixXd Sred,Eigen::VectorXd stdev);


    
};


#endif // LIBB040MEM_H
