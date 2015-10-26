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

#ifndef ALEXDATA_H
#define ALEXDATA_H
#include "parameters.h"
#include "burst.h"
#include <QVector>
#include <QString>
#include "analysis.h"
#include"alexevallog.h"
#include "GMMfit.h"
#include "ticktock.h"
#include "readArrivalTimes.h"

#define DISTRIBUTION_COLOR "darkblue"
#define DISTRIBUTION_COLOR_SELECTED "red"
#define COUNTRATE_COLOR_DONOR "green"
#define COUNTRATE_COLOR_ACCEPTOR "red"
#define COUNTRATE_COLOR_CH1 "blue"
#define COUNTRATE_COLOR_CH2 "orange"
#define FCS_COLOR_CROSS "grey"
#define FCS_COLOR_MARKER "pink"
#define RANGE_COLOR_BURST "grey"
//"navy"

struct AlexData;
struct GMMfitData;
struct Range_t;

/**
 * @brief SaveBursts Saves the entries of burstVector to a csv file filename. delimiter=\\t
 * @param[in] burstVector the vector of Bursts to store
 * @param[in] BurstSearchParams parameters are written to the file header
 * @param[in] filename Name of the file, where the data is stored. If the file already exists, it is overwritten.
 * @return EXIT_SUCCESS or EXIT_FAILURE if no data was written or the file could not be opened.
 */
int SaveBursts(const QVector<Burst> &burstVector, const BurstSearchParams_t BurstSearchParams, const QString filename);

/**
 * @brief SaveBurstsDual Saves the entries of burstVectorDual to a csv file filename. delimiter=\\t
 * @param[in] burstVectorDual the vector of BurstDuals to store
 * @param[in] filename Name of the file, where the data is stored. If the file already exists, it is overwritten.
 */
void SaveBurstsDual(const QVector<BurstDual> &burstVectorDual, const QString filename);


/// Calls the burst search method specified in BurstSearchParams::BurstSearchMethodsDual
int BurstSearch(AlexData *ad);


//int readTTTRfile(AlexData *ad);

struct Range_t { /// defines the range of the ALEX P-S-diagram.
    double minP;
    double maxP;
    double minS;
    double maxS;
    double x() {return minP;}
    double y() {return minS;}
    double width() const {return maxP-minP;}
    double heigth() const {return maxS-minS;}
    QString toString(QString prefix="Range: ",QString delimiter="\n") const;
    void check();
    void setDefault();
    Range_t();
    int readFromIni(const std::string iniFilename);
    int writeToIni(const std::string iniFilename) const;
};


/// the data class for the programm. Here, all data is centralised and can be accessed by any of the widgets
struct AlexData
{
    enum Method {
        swFRET,
        ALEX,
        Simulation,
        nMethods
    } method;

    BurstSearchParams_t burstSearchParamsGlobal; ///< parameter for Burst Search (global)
    BurstSearchParams_t burstSearchParamsCh1; ///< parameter for Burst Search (donor channel)
    BurstSearchParams_t burstSearchParamsCh2; ///< parameter for Burst Search (acceptor channel)
    FRETparams_t FRETparams;///< parameter for FRET evaluation
    FCSparams_t FCSparams; ///< parameter for species-selective FCS
    Range_t rangeALEX; ///< range of the ALEX histogram

    QVector<double> arrivalTimeCh1; // in seconds
    QVector<double> arrivalTimeCh2; // in seconds
    Photons photons;
    QVector<Burst> burstVectorCh1;
    QVector<Burst> burstVectorCh2;
    QVector<BurstDual> burstVectorDual;

    BurstDual background; ///< contains background photon numbers

    QVector<double> markerTimeDexc; ///< Donor excitation period start time in seconds
    QVector<double> markerTimeAexc; ///< Acceptor excitation period start time in seconds

    double start; /// range of photons in seconds
    double stop;
    double max_measurementtime; /// in seconds
    inline double measurementDuration() {return (stop-start);} // in seconds

    QString iniFilename; /// filename of alex.ini file
    QString TTTRfilename; /// the source of the arrival times
    QString name; /// name of the measurement (default: name of t3r file)

    QString currentDir; /// current working directory (load TTTR, save results)

    /// clears all data in AlexData
    void clear();
    /// initializes all parameter structs from ini file iniFilename
    int init();
    /// initializes the FCS parameter structs from ini file iniFilename
    int initFCS();
    /// checks if there are any photon arrival times in both channels
    bool isEmpty() const;
    /// checks if there are any markers in both channels
    bool isEmptyMarker() const;
    /// checks if burstVectorDual has entries
    bool isEmptyBurst() const;
    /// checks if burstVectorDual has already entries for P and S (either of them being non-zero for the first burst)
    bool isEmptyFRET() const;
    /// restricts the arrival times to the range in given by \a start and \a stop
    int setRange();
    /// sets the TTTRfilename and name variable from a given .t3r file name
    void setName(const QString &t3rFile);
    /// sets the single burst search method for all channels
    void setBSMethodSingle(const BurstSearchParams_t::BurstSearchMethodsSingle methodSingle);
    /// sets the dual burst search method for all channels
    void setBSMethodDual(const BurstSearchParams_t::BurstSearchMethodsDual methodDual);
    /// writes the burstVectorDual to file
    void save(const QString &filename) const;
    /// save the parameter structs to ini file. If the filename is empty, then it is written to iniFilename.
    void saveIni(const QString &filename=QString("")) const;
    /// calls the toString() method of all parameter structs and concatenates the result
    QString printParams() const;
    /// calls the toString() method of all burst search parameter structs and concatenates the result
    QString printParamsBurstSearch() const;
    /// info string about the burst vectors
    QString resultBS() const;
    /// averages the burst rate over all BurstDual in burstVectorDual
    double meanBurstrate() const;
    /// averages the burst duration over all BurstDual in burstVectorDual
    double meanBurstDuration() const;
    /// averages the number of photons in a burst over all BurstDual in burstVectorDual
    double meanNoOfPhotons() const;
    /// estimates the excitation period of the donor channel from the marker events
    double getExcitationPeriodDonor() const;
    /// estimates the excitation period of the donor channel from the marker events
    double getExcitationPeriodAcceptor() const;
    /// estimates the excitation duration of the donor channel from the marker events (including gaps)
    double getExcitationDurationDonor() const;
    /// estimates the excitation duration of the acceptor channel from the marker events (including gaps)
    double getExcitationDurationAcceptor() const;

    /// mean proximity ratio of all bursts in burstVectorDual
    double meanPR() const;
    /// mean stoichiometry ratio of all bursts in burstVectorDual
    double meanSR() const;

    /// counts the number of bursts with limits of PR and S given by FRETparams
    int countBursts(FRETparams_t &FRETparams);

    gsl_histogram *burstDurationDistribution(size_t nbins); /// in ms
    gsl_histogram *burstSizeDistribution(size_t nbins, double maxBurstSize=0);
    gsl_histogram *burstRateDistribution(size_t nbins); /// in kHz

    /// calculate the background intensities from the struct \a background. Note that struct \a background must contain a background measurement.
    int calculateBackground();
    /// calculates the duty cycles of the excitation lasers
    int calculateDutyCycles();

public:
    AlexData();
    /// constructor. reads parameters from ini file iniFile
    AlexData(const QString &iniFile);

private:


};




/// contains the results of a Gaussian Mixture Models fit to the ALEX-Histogram
class GMMfitData
{
public:
    enum Model {
         LowFRET=0,
         HighFRET,
         LowHighFRET,
         Donly,
         Aonly,
         DonlyAonly,
         LowFRETonly,
         HighFRETonly,
         LowHighFRETonly,
         nModels
    };
    GMMfit* gmm;
    GMMfitOptions options;
    GMMfitData();
    void defaultModel();
    QString getModelName(const GMMfitData::Model model) const;
    Model getModel() const {return GMMmodel;}
    void setModel(Model model) {GMMmodel=model;}

    void init(const Model model);
    void init();
    QString toString() const;
    QString resultToString(const QString name=QString("")) const;
    QString resultToStringHeader() const;
private:
    Model GMMmodel;
    void defaultDonly(Gaussian &g);
    void defaultAonly(Gaussian &g);
    void defaultHighFRET(Gaussian &g);
    void defaultLowFRET(Gaussian &g);
};



;
#endif // ALEXDATA_H
