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

#ifndef BURSTSEARCH_H
#define BURSTSEARCH_H

#include<QVector>
#include "parameters.h"
#include "burst.h"
#include<gsl/gsl_histogram.h>
#include"alexevallog.h"

//#define MYDEBUG

/**
  \brief Performs a dual channel burst search and stores the result in a vector of \link BurstDualChannel bursts \endlink
  It searches for overlapping regions of burst in \a burstVectorDonor and \a burstVectorAcceptor and extracts the duration where both channels are active. The individual photons are assigned to donor and acceptor excitation periods as  given by the \a markerTime vectors. At last, the burst is assured to fullfill the criteria defined in the \a BurstSearchParams parameters such as minimal and maximal duration, minimal burst size etc.
  You can use the function BurstSearch to obtain burst vectors from photon arrival times data.
  \param[out] burstVectorDual Vector that contains the final dual bursts matching all selection criteria.
  \param[in] burstVectorDonor Vector that contains the bursts of the donor emission channel.
  \param[in] burstVectorAcceptor Vector that contains the bursts of the acceptor emission channel.
  \param[in] arrivalTimeDonor Vector that contains the photon arrival times of the donor emission channel.
  \param[in] arrivalTimeAcceptor Vector that contains the photon arrival times of the acceptor emission channel.
  \param[in] markerTimeDexc Vector that contains the marker times of the donor excitation period.
  \param[in] markerTimeAexc Vector that contains the marker times of the acceptor excitation period.
  \param[in] BurstSearchParams All selection criteria such as thresholds for burt rate, duration, etc. are defined here.
  */
//int DualChannelBurstSearch(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorDonor, const QVector<Burst> &burstVectorAcceptor, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor,  const QVector<double> &markerTimeDexc,  const QVector<double> &markerTimeAexc, const BurstSearchParams_t BurstSearchParams );
int DualChannelBurstSearch(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorDonor, const QVector<Burst> &burstVectorAcceptor, const Photons photons,  const BurstSearchParams_t BurstSearchParams );

///extended DCBS, detects also Donly and Aonly
int DualChannelBurstSearchExt(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorCh1, const QVector<Burst> &burstVectorCh2, const Photons &photons, const BurstSearchParams_t BurstSearchParams );

///**
// \overload int DualChannelBurstSearch(AlexData *ad);
// */
//int DualChannelBurstSearch(AlexData *ad);

/**
  \brief Burst search on all photons
  Burst search is performed on all photons (irregardless of emission channel) and stored in a dual burst vector.
  */

int AllPhotonsBurstSearch(QVector<BurstDual>& burstVectorDual, const Photons& photons, const BurstSearchParams_t& BurstSearchParamsSingle,const BurstSearchParams_t& BurstSearchParamsGlobal );


/**
  Combined Burst Search
  */
int CombineBurstsOR(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorCh1, const QVector<Burst> &burstVectorCh2, const Photons &photons, const BurstSearchParams_t BurstSearchParams );

int inline countPhotons(BurstDual &burst, int &lastD, int &lastA, int &markeridx, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> &markerTimeDexc,  const QVector<double> &markerTimeAexc);


//double guessMarkerTime(const QVector<double> &markerTime, const int markeridx) {return markerTime.first()+markeridx*(markerTime.last()-markerTime.first())/(markerTime.size()-1);}

//int guessMarkerIdx(const QVector<double> &markerTime, const double time) {return (int) ((time)/(markerTime.last()-markerTime.first())*(markerTime.size()-1));}

// /// helper function
//inline int validBurst(const Burst &burst, const BurstSearchParams_t &BurstSearchParams );

/// Calls the burst search method specified in BurstSearchParams_t::BurstSearchMethodsSingle
int BurstSearchSingle(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams);


/**
  SlidingBurstSearch //Eggeling et al. 2001 (single channel burst search method)
  Performs a sliding photon burst search on the vector photonArrivalTimes containing the arrival times of single photons in the detector(s). A burst consists of at least L photons, which have at least M neighbours in a time window T centered around itself. A burst is stored in the burstVector of burstStructs.
  Note that the accepted photonArrivalTimes vector times must be in ascending order.
  */
int SlidingBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams);

/**
  Burst search based on inter photon times criteria. (single channel burst search method)
  Inter photon times are calculated from the arrival times vector and then a Lee filter is applied with the parameters specified in BurstSearchParams. A burst is selected for photons with a maximal inter photon time and other burst selection criteria as specified in BurstSearchParams.
  */
int LeeFilterBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams );


/**
 * BinnedBurstSearch (single channel burst search method)
 * Intensity above threshold counts as a burst, if it has at least MinBurstSize photons.
  */
int BinnedBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const double binwidth=100e-6, const int threshold=5, const int MinBurstSize=40);


/**
  outputs the intensity over time of a single burst. The arrival times are inserted into a histogram and output to qDebug()
  **/
void burstIntensity(const BurstDual &burst, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor);

/**
  outputs the intensity over time of a single burst and additionally the marker events. The arrival times and markers are inserted into a histogram and output to qDebug()
  **/
void burstIntensity(const BurstDual &burst, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<double> &markerTimeDexc,const QVector<double> &markerTimeAexc );


#endif // BURSTSEARCH_H
