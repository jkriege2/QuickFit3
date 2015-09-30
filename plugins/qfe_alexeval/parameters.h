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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "jkiniparser2.h"
#include<QString>
#include "alexevallog.h"
#include<QDebug>

struct BurstSearchParams_t;
struct FRETparams_t;
struct FCSparams_t ;

#define FILE_ALEXINI "./alex.ini"


enum Channel {
    DonorEm=1<<0,
    AcceptorEm=1<<1,
    unknownChannel3=1<<2,
    unknownChannel4=1<<3,
    Channels=0b00001111,
    nChannels=4
};

//enum ChannelRaw {
//    channel1=1<<0,
//    channel2=1<<1,
//    channel3=1<<2,
//    channel4=1<<3,
//    nChannelsRaw=4
//};

enum Marker {
    DonorEx=1<<4,
    AcceptorEx=1<<5,
    Marker3=1<<6,
    Markers=0b11110000,
    nMarker=3
};

//enum MarkerRaw {
//    Pin5,
//    Pin4,
//    Pin9,
//    unknown,
//    nMarkerRaw
//};

/**
  Read parameter values from an ini file.
  The values are stored in the various data structures given as arguments. If the filename is empty, it will try to read from alex.ini, storing that name in the iniFile variable.
  */
int  readFromIni(BurstSearchParams_t &BurstSearchParams,BurstSearchParams_t &burstSearchParamsCh1,BurstSearchParams_t &burstSearchParamsCh2, FCSparams_t &FCSparams, FRETparams_t &FRETparams, std::string iniFile);

int  readFromIniFCS(FCSparams_t &FCSparams, std::string iniFile);

/**
 * @brief readFromIniBackground read only background information to FRETparams
 */
int readFromIniBackground(FRETparams_t &FRETparams, const std::string iniFile);

/**
  Write parameter values to an ini file.
  The values are stored in the various data structures given as arguments. If the filename is empty, it will fail and return EXIT_FAILURE.
  */
int writeToIni(const BurstSearchParams_t &BurstSearchParams, const BurstSearchParams_t &burstSearchParamsCh1, const BurstSearchParams_t &burstSearchParamsCh2, const FCSparams_t &FCSparams, const FRETparams_t &FRETparams, const std::string iniFile);

/// Parameters for burst search
struct BurstSearchParams_t
{
    enum BurstSearchMethodsSingle { ///< enum for the burst search method on a single photon channel
        Sliding=0, // Sliding=0;
        LeeFilter=1, // LeeFilter=1
        Binned=2 ///< binned burst search
    } burstSearchMethodSingle; ///< burst search method for a single photon channel for \a Burst
    const std::string burstSearchMethodSingleString() const;

    enum BurstSearchMethodsDual { ///< enum for the burst search method on both photon channels
        CombineBurstsOR, ///<  (single channel burst search in both channels, saving overlapping and non-overlapping bursts)
        DualChannel, ///< Dual channel burst search
        AllPhotons, ///< All photons burst search: Combine both channels to one photon stream and use single channel burst search.
        DualChannelExt, ///< DCBS with Donly and Aonly detection, alternative name: CombineBurstsOR_DualAND
        None
    } burstSearchMethodDual; ///< burst search method for dual bursts \a BurstDual
    const std::string burstSearchMethodDualString() const;

    uint channel; ///< the channel selection for the photons if applicable

    double MinBurstrate; ///< minimal burst rate in 1/s
    double MaxBurstrate; ///< maximal burst rate in 1/s
    double MaxDuration; ///< maximal burst duration in s
    int MinBurstSize; ///< minimal burst size
    int MinBurstSizeDex; ///< minimal number of photons during Dex
    int MinBurstSizeAex; ///< minimal number of photons during Dex

    double MaxIPT; ///< upper limit for inter photon time in s
    int LeeWinSize; ///< Lee filter: window size
    double LeeVar; ///< Lee filter: variance in s^2

    inline int L() const {return MinBurstSize;}
    int M; ///< neighbouring photons
    double T; ///< time window for neighbouring photons in seconds

    /// initialize the parameters with default values
    BurstSearchParams_t();
    /// writes all parameters to a QString in a \verbatim prefix variable=value delimiter variable=value delimiter ... \endverbatim format.
    const QString toString(const QString prefix = QString(""), const QString delimiter=QString("\n")) const;

    int readFromIni(jkINIParser2 &ini, std::string group);
    int writeToIni(jkINIParser2 &ini, std::string group="BurstSearchParams") const;
};



/// parameter for ALEX-FRET
struct FRETparams_t
{
    double gamma; ///< correction factor
    double crosstalk; ///< relative crosstalk from donor into acceptor emission channel;  crosstalk=F_Aem/F_Dem for D-only sample
    double directExc; ///< direct excitation of the acceptor during donor excitation
    double scaleAcceptor; ///< scale parameter for the acceptor exc periods. Can be used to correct for different laser intensities or excitation period lenghts
    double directExcThresholdDex; ///< threshold for the number of Dex photons. For lower numbers, P will be set to one
    bool useNegativeIntesities; ///< wether to use negative intensities or not.

    double bg_Dem_Dex; ///< background rate in the donor emission channel during donor exciatation period in Hz
    double bg_Aem_Dex; ///< background rate in the acceptor emission channel during Donor exciatation period in Hz
    double bg_Dem_Aex; ///< background rate in the donor emission channel during donor exciatation period in Hz
    double bg_Aem_Aex; ///< background rate in the acceptor emission channel during acceptor exciatation period in Hz
    double dutyCycleDex; ///< duty cycle of the donor excitation laser. It can be calculated as T_D/(T_D+T_A).
    double dutyCycleAex; ///< duty cycle of the acceptor excitation laser. It can be calculated as T_A/(T_D+T_A).

    /// filtering thresholds for subpopulation selection
    double MaxS;  ///< upper boundary for S-value
    double MinS;  ///< lower boundary for S-value
    double MaxP; ///< upper boundary for PR-value
    double MinP; ///< lower boundary for PR-value
    double MinTgTr; ///< lower boundary for t_g-t_r in ms
    double MaxTgTr; ///< upper boundary for t_g-t_r in ms

    /// initialize the parameters with default values
    FRETparams_t();
    /// writes all parameters to a QString in a \verbatim variable=value\\n \endverbatim format.
    const QString toString(const QString prefix = QString(""), const QString delimiter=QString("\n")) const;
    const QString filterToString(const QString prefix = QString("")) const;

    int readFromIni(jkINIParser2 &ini, std::string group="FRETparams");
    int writeToIni(jkINIParser2 &ini, std::string group="FRETparams") const;
};

struct FRETstatistics_t
{
    double Pmean;
    double Pstd;
    double Smean;
    double Sstd;
    double meanBurstDuration;
    double meanBurstSize; ///< mean number of photon per burst
    double meanBurstRate; ///< mean of burst rates in Hz
    double meanBurstSizeD; ///< mean burst size of bursts declared as Donly
    double meanBurstSizeA; ///< mean burst size of bursts declared as Aonly
    double quasiBulkP;
    double quasiBulkS;
    int nDonly; ///< number of samples classified as Donly
    int nAonly;///< number of samples classified as Aonly
    int nDual;///< number of samples classified as dual bursts
    int n; ///< number of samples

    FRETstatistics_t();

    QString toString(const QString prefix = QString(""), const QString delimiter=QString("\n")) const;
};


/// parameter for species-selective FCS
struct FCSparams_t
{
    enum FCSMethods {
        FCScomplete, ///< divide the whole observation time into bins and correlate them
        FCSstep, ///< each burst (+ \c offset photons before and after the burst) is divided into bins and successively correlated burst by burst
        FCS_TTTR ///< calculate correlation function from single photon arrival times (TTTR data)
    } FCSMethod;
    /// options for FCS
    bool doBoxFiltering;
    bool doDivideByMarkerCorr;
    bool doDivideByLaterCorr;
    bool doIgnoreZeros;
    bool doCenterTau;

    /// lag time parameters
    int S; ///< number of blocks
    int m; ///< multiply delta tau
    int P; ///< tau values per block
    double MinLagTime; ///< the minimal lag time

    /// parameters for MultiTau correlator (replaced by TTTRcorrelator)
    double binwidth; ///< the binwidth used for the intensity histograms in seconds
    //    double MaxLagTime; ///< the maximal lag time in seconds. This is used for the burstwise binning for the stepwise correlator.
    int offset; ///< number of background photons before and after a burst included in the correlation

    /// parameters for TTTRcorrelator
    double CorrelationWindow; ///< the length of the correlation window. The window is centered around each burst and photons therein are correlated.

    /// parameters for calculating the correlation of excitation
    double dummyPhotonIPT; ///< inter-photon time of dummy photons for simulating the excitation
    double nPeriods; ///< the number of alternation periods used.
    double tDex; ///< length of donor excitation period
    double tGapDexAex; ///< length of break between donor and acceptor excitation period
    double tAex; ///< length of acceptor excitation period
    double tGapAexDex; ///< length of break between acceptor and donor excitation period

    double backgroundDonor; ///< estimated background rate in the donor channel
    double backgroundAcceptor; ///< estimated background rate in the acceptor channel

    /// parameters for smoothing
    double BoxFilterTimeWindow; ///< time window for averaging box filter
    int BoxFilterIndexWindow; ///< index window for box filtering

    /// initialize the parameters with default values
    FCSparams_t();
    /// writes all parameters to a human readable QString in a \verbatim variable=value\\n \endverbatim format.
    const QString toString(const QString prefix = QString(""), const QString delimiter=QString("\n")) const;

    /// read FCS parameters from an ini instance
    int readFromIni(jkINIParser2 &ini, std::string group="FCSparams");
    /// write FCS parameters to an ini instance
    int writeToIni(jkINIParser2 &ini, std::string group="FCSparams") const;
};


;
#endif // PARAMETERS_H
