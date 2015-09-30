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

#ifndef FCSDATA_H
#define FCSDATA_H

#include "FCScurve.h"
#include "alexdata.h"
#include "qftools.h"
/// contains the results of a correlation analysis.
class FCSdata
{
public:
    FCSdata();

    QVector<BurstDual> subpopulation; // the subpopulation
    FCScurve tau; ///< the correlation time lag
    FCScurve ACF_Dem_Dex; ///< the correlation function of the donor emission channel during donor excitation
    FCScurve ACF_Dem_Dex_later; ///< the correlation function of the donor emission channel during donor excitation
    FCScurve ACF_Aem_Dex; ///< the correlation function of the acceptor emission channel during donor excitation
    FCScurve ACF_Aem_Aex; ///< the correlation function of the acceptor emission channel during acceptor excitation
    FCScurve CCF_Dex; ///< the crosscorrelation function during donor excitation
    FCScurve ACF_Dem; ///< the correlation function of the donor emission channel
    FCScurve ACF_Aem; ///< the correlation function of the acceptor emission channel
    FCScurve ACF_sumAD_Dex; ///< the correlation function of the sum of donor and acceptor emission channels during donor excitation
    FCScurve ACF_marker;
    FCScurve ACF_markerD;
    FCScurve ACF_markerA;

    QVector<double> meanTau() const; ///< the correlation functions are binned between 0,tau_1;tau_1,tau_2;tau_2,tau_3;... The tau values are therefore approximated by the bin centers.

    /// returns true if any of the vectors are empty
    bool isEmpty() const;
    /// saves the correlation functions to file
    void saveCorr(const QString &filename, const FCSparams_t FCSparams ) const;

    QString getCorrCSV(int which, const FCSparams_t FCSparams) const;
    /// prints information on the current FCS analysis such as the number of bursts in the selected subpopulation
    QString resultFCS(const QString prefix = QString("")) const;

    void clear();
    void clearMarker();

    void remove(int index);

    gsl_histogram *burstDurationDistribution(size_t nbins, BurstSearchParams_t burstSearchParams);
    gsl_histogram *burstSizeDistribution(size_t nbins, BurstSearchParams_t burstSearchParams);
    gsl_histogram *burstRateDistribution(size_t nbins, BurstSearchParams_t burstSearchParams);

};

int analysisFCS(FCSdata *fd, const AlexData *ad);
int analysisFCStotal(FCSdata *fd, const AlexData *ad);
int analysisFCSburstwise(FCSdata *fd, const AlexData *ad);
int burstPhotonsToChannel(QVector<double> &tDexDem,QVector<double> &tDexAem,QVector<double> &tAexAem,
                          const Photons &photons, const QVector<BurstDual> &dualBurstVector, const FCSparams_t FCSparams );

#endif // FCSDATA_H
