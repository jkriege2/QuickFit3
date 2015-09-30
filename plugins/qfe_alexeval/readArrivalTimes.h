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

#ifndef READARRIVALTIMES_H
#define READARRIVALTIMES_H

#include<QVector>
#include<limits>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include<QDebug>
#include<QFile>
#include<QString>
#include<QTextStream>
#include<qstringlist.h>
#include "parameters.h"
#include "burst.h"

using namespace std;

/**
 * @brief checkMarker Checks for alternating markers and corrects for missing alternation periods. The alternation period of each channel is approximated by the average alternation period. For each period length that is 20% longer than the approximated period a new marker is inserted. It is also assured that channel 1 has the first marker event.
 * @param[in,out] markerTimeChannel1
 * @param[in,out] markerTimeChannel2
 * @return Returns the number of inserted markers.
 */
int checkMarker(QVector<double> &markerTimeChannel1, QVector<double> &markerTimeChannel2);

/// Helper function used by checkMarker for a single marker channel. Returns the number of inserted markers.
int checkMarker(QVector<double> &markerTime, double alternationPeriod=0);



/**
 * @brief readTTTRfile Read a .t3r file and stores photon arrival times for two channels and marker events for two alternating channels with their respective macrotime. Marker alternation is forced: non-alternating marker events will be discarded.
 * @param[out] photonArrivalTimeRoute0 vector for photon arrival times in channel route 0
 * @param[out] photonArrivalTimeRoute1 vector for photon arrival times in channel route 1
 * @param[out] markerTimePin4 vector for marker arrival times at pin4
 * @param[out] markerTimePin5 vector for marker arrival times at pin5
 * @param[in] filename file name of the .t3r file
 * @param[in] forceAlternation if true, only alternating marker entries are read. If there is an event in one marker channel, no more markers are stored in this channel until an event in the other marker channel occurs
 * @return EXIT_SUCCESS or EXIT_FAILURE if no photons were read
 */

int readTTTRfile(Photons& photons, QVector<double> &markerTimePin4, QVector<double> &markerTimePin5, const QString filename, const Channel route0, const Marker Pin4, bool forceAlternation=false, bool isV6=false);
//int readTTTRfile(QVector<double> &photonArrivalTimeRoute0, QVector<double> &photonArrivalTimeRoute1, QVector<double> &markerTimePin4, QVector<double> &markerTimePin5, const QString filename, bool forceAlternation=false);

/// read character-separated value file and store columns in vectors in data. Return header string
int readCSVtoList(QList<QVector<double> >& data, const QString filename, const QString delimiter="\t", const QString comment="#");

int writeListtoCSV(const QList<QVector<double> >& data, const QString &prefix, const QString filename, const QString delimiter="\t", const QString comment="#");



/**
  read a text file with delimiter "\t" and 4 columns corresponding to start index, start time in s, number of photons, burst duration in ms
  **/
int readBurstFile(QVector<Burst> &burstVector, const QString filename);


///Check the markers.
/**    The alternating excitation is supposed to have the following repeated scheme:
       |                      |      |                      |      |
  ||:  |<---------T1--------->|<-T2->|<---------T3--------->|<-T4->|  :||
       |   Donor excitation   | break| Acceptor excitation  | break|
       |                      |      |                      |      |
       ^                             ^
       markerDexc                    markerAexc

       The T1 and T3 denote the duration of an excitation, T2 and T3 the duration of a break (no excitation). At the start of the respective excitation period a marker should have been placed (denoted by markerDexc and markerAexc). It is not possible to reconstruct the breaks from the marker events, but they are assumed to be neglegible.
       The following checks are performed:
        1) If one marker vector is empty, the markers are constructed from the other marker channel. It is assumed that T1+T2=T3+T4. The period is estimated from all marker events (does not work if many markers are missing)
        2) For each period that deviates more than 20% from the estimated mean period,  a new marker is inserted.
        3) It is assured that the first event is in the donor marker channel and the last event in the acceptor marker channel. All other events are removed.
  \param[in,out] arrivalTimeDonor Single photon arrival times in the donor emission channel
  \param[in,out] arrivalTimeAcceptor Single photon arrival times in the acceptor emission channel
  \param[in,out] markerTimeDexc Marker events of the donor excitation period
  \param[in,out] markerTimeAexc Marker events of the acceptor excitation period
  */
int checkAlternationPeriods(Photons& photons, QVector<double> &markerTimeDexc,  QVector<double> &markerTimeAexc);
//int setAlternationPeriods(Photons& photons, QVector<double> &markerTimeDexc,  QVector<double> &markerTimeAexc);

/**
 * @brief constructMarkerEvents simulates marker events with period \a period and relative offset offsetTime in the delayed marker channel
 * @param markerTime
 * @param markerTimeDelayed
 * @param period
 * @param offsetTime
 * @param tMax
 * @return
 */
int constructMarkerEvents(QVector<double> &markerTime, QVector<double> &markerTimeDelayed, const double &period, const double &offsetTime, const double &tMax);


/**
 * @brief estimateMarkerEvents duplicates the events in vector markerTime with an offset of offsetTime to the (empty) vector markerTimeNew
 * @param markerTimeDelayed
 * @param markerTime
 * @param offsetTime
 * @return 0 on success
 */
int estimateMarkerEvents(QVector<double> &markerTimeDelayed,  const QVector<double> &markerTime, const double offsetTime);

/**
 * @brief background calculate background in all channels
 * assigns the photons to donor and acceptor excitation periods. To obtain burstrates you can use
 * \verbatim
 * background rate Dem_Dex = background.n_Dem_Dex/excitationPeriodDonor/markerTimeDexc.size()\n
 * background rate n_Aem_Dex = background.n_Aem_Dex/excitationPeriodDonor/ad->markerTimeDexc.size()\n
 * etc.
 * \endverbatim
 * @param[out] background
 * @param[in] arrivalTimeDonor
 * @param[in] arrivalTimeAcceptor
 * @param[in] markerTimeDexc
 * @param[in] markerTimeAexc
 * @return EXIT_SUCCESS
 */
int background(BurstDual &background, const Photons photons);

/**
 * @brief setRange remove elements in the ordered vector \a orderedVector until $start \leq vector[i] \leq end$ for all $i$
 * @param[in,out] orderedVector an ordered QVector with vector[i]<vector[i+1] for all i
 * @param[in] start start value
 * @param[in] end end value
 * @return number of elements removed or -1 if out of range
 */
int setRange(QVector<double> &orderedVector, const double &start, const double &end);

int setRange(Photons& photons, const double &Tstart, const double &Tend);
#endif // READARRIVALTIMES_H
