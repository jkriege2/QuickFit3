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

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include<QVector>
#include "parameters.h"
#include<statistics_tools.h>
#include<multitau-correlator.h>
#include <gsl/gsl_histogram.h>
#include "burst.h"


// ADD probability distribution analysis: Nir et al. 2006



/**
  \brief Calculate FRET-specific values.
  For each burstStructDualChannel of the vector the proximity ratio, stoichiometry ratio and energy transfer is calculated and stored in the data structure.
  \param burstVectorDual contains the bursts. The PR- and S-values are stored in it as well.
  \param[in] FRETparams contains correction factor such as background, crosstalk and direct excitation.
  **/
int analysisFRET(QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams);

/**
  \brief Calculate FRET-specific statistics.
  Mean and standard deviation of S and PR are calculated.
  returns the number of bursts used for the averaging
  **/
int statisticsFRET(FRETstatistics_t &FRETstatistics, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams );

/**
  \brief Performs auto- and crosscorrelation.
  * The arrival times corresponding to bursts in the vector burstVectorDual are binned and correlated according to the method specified in
  * FCSparams_t::FCSMethods. Burst selection based on S and PR values is supported for stepwise correlation only.
  * A similar method for AlexData and FCSData is implemented in alexdata.h
  * FIXME: add selection for FCS()
  * FIXME: method works only for FCScomplete
  **/
int analysisFCStotal(const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams, const FCSparams_t &FCSparams );



/**
  \brief The whole photon stream is binned and then correlated.
  Since all photon times are inserted into a histogram over the whole measurement length, for a small binsize the array will be very large. Therefore, this method is very memory intensive and only feasible for small sets of data. For larger sets of data, use FCSstep.
  **/
int FCSmultiTauBinned(const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const FCSparams_t &FCSparams); /// bin and correlate

int FCSmultiTauStepwiseSimple(QVector<double> &tau, QVector<double> &corr, const QVector<double> &arrivalTimes, const FCSparams_t &FCSparams );


/**
  \brief Each burst is binned and a stepwise correlator is called.
  The auto- and crosscorrelation curves are calculated from all photons from both detection channels during donor excitation (Koopmann et.al. 2009).
  Execution time is larger, but only single bursts are binned as opposed to the whole photon stream in FCS(). Thus, smaller binsizes are feasible as well.
 \param[in] FCSparams Contains the binsize and number of offset photons that additionally are accounted for. Additionally, the spacing of lag times tau can be specified by S,P,m.
 \param[out] tau vector containing the lag time of the correlation functions (one for all).
 \param[out] corrDonor vector of correlation of donor channel.
 \param[out] corrAcceptor vector of correlation of acceptor channel.
 \param[out] crossCorr vector of cross correlation of both channel.
 \param[in] arrivalTimeDonor photon arrival times in the donor channel.
 \param[in] arrivalTimeAcceptor photon arrival times in the acceptor channel.
 \param[in] burstVectorDual vector containing all bursts.
  **/
int FCSmultiTauStepwise(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, QVector<double> &corrSum, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> markerTimeDexc,  const QVector<double> markerTimeAexc, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams, bool doBoxFiltering=false );

int FCS_TTTR(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, const QVector<double> &arrivalTimeDemDex, const QVector<double> &arrivalTimeAemDex, const FCSparams_t &FCSparams);

int testCorrelator(QVector<double> &tau, QVector<double> &corr,const QVector<double> &arrivalTime, const FCSparams_t &FCSparams);

int correlateTOA(QVector<double> &tau, QVector<double> &corr,const double *arrivalTime, const uint N, const FCSparams_t &FCSparams);

int crossCorrelateTOA(QVector<double> &tau, QVector<double> &corr,const double *arrivalTime1, const uint N1, const double *arrivalTime2, const uint N2, const FCSparams_t &FCSparams);

int oscillationACF(QVector<double> &corrExc, QVector<double> &corrDonorExc, QVector<double> &corrAcceptorExc, const FCSparams_t &FCSparams);

int oscillationACF_TTTR(QVector<double> &corrExc, QVector<double> &corrDonorExc, QVector<double> &corrAcceptorExc,const QVector<double> &tau, const FCSparams_t &FCSparams);

/// creates \a n logarithmically spaced lag times between \a 10^a to \a 10^b
void createLogTauTTTR(double* tau, double a, double b, unsigned int n);
void createLinspaceTau(double *tau, double a, double b, unsigned int n);

void createLinspace(int a, int b, int n);

/// creates lag times like the correlator in multitau-correlator.h
/// Note: at least P+(S-1)*(P-P/m) entries are needed for tau
void MultiTauCorrelatorCreateTau(double* tau, int S, int m, int P,double tauMin=1);
/**
 * @brief correlateTTTR correlates single photon arrival times in one channel
 * @param t arrival times with N points
 * @param N number of data points in channel t
 * @param g pointer to the correlation function array (call by reference) containing at least ntau allocated entries
 * @param tau pointer to an array of lag times that should be used to calculate the correlation function with ntau elemts
 * @param ntau number of discrete lag times
 * @return EXIT_SUCCESS
 */
int correlateTTTR(const double *t, const uint N, double *g, const double *tau, const uint ntau);
/**
 * @brief crosscorrelateTTTR crosscorrelates single arrival times of two channels
 * @param t arrival times in channel 1 with Nt points
 * @param Nt number of data points in channel t
 * @param u arrival times in channel 2 with Nu points
 * @param Nu number of data points in channel u
 * @param g pointer to the correlation function array (call by reference) containing at least ntau entries
 * @param tau pointer to the lag times that should be used to calculate the correlation function
 * @param ntau number of discrete lag times
 * @return EXIT_SUCCESS
 */
int crosscorrelateTTTR(const double *t, uint Nt, const double *u, uint Nu, double *g, const double *tau, const uint ntau);




/**
  \brief select a subpopulation of bursts.
  Selection is based on boundaries specified in FRETparams. (to add selection parameters, you only have to add the parameter to FRETparams and adapt the isMasked method for bursts in Burst.
 \param[out] burstVectorDualSelected vector containing the bursts of the subpopuation
 \param[in] burstVectorDual vector containing all bursts.
 \param[in] FRETparams Contains the subpopulation boundaries.
  **/
int selectSubpopulation(QVector<BurstDual> &burstVectorDualSelected, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams);

/// box filter
///
/// \param[out] filteredData
/// \param[in] xdata the x values to the corresponding y values in ydata
/// \param[in] ydata the y values to the corresponding x values in xdata
/// \param[in] rangeWindow the width of the filter in units of xdata
int boxFilter(QVector<double> &filteredData, const QVector<double> &xdata, const QVector<double> &ydata, const double &rangeWindow);

/// \overload boxFilter(QVector<double> &filteredData, const QVector<double> &range, const QVector<double> &data, const double &rangeWindow);
/// filters in a window with width 2*indexWindow+1 centered around a data point. In contrast to rangeWindow this window is given in index position, hence no x values are required.
int boxFilter(QVector<double> &filteredData, const QVector<double> &data, const int &indexWindow);

int mergeSorted(QVector<double> &mergedVector,const QVector<double> &orderedVector1,const QVector<double> &orderedVector2);
#endif // ANALYSIS_H
