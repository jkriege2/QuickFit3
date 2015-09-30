#ifndef ALEXSIM_H
#define ALEXSIM_H
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <gsl/gsl_histogram.h>
#include<QMessageBox>
#include "burst.h"

/**
Class to generate photon data.

Interphoton times are drawn from an exponential distribution for both channels with either a flourescent rate or a background rate.
Possible scenarios include background, acceptor only, donor only, regular bursts (burst simultaneously in both channels), blinking, bleaching and coincidence (doubled rate).
  **/
class AlexSim
{
private:
    double rateDemDex;
    double rateDemAex;
    double rateAemDex;
    double rateAemAex;
    double rateBackground;
    double rateFirst;
    double burstDuration; // in seconds
    double burstDurationVar;
    double lifetimeBlinking;
    double lifetimeBleaching;
    double rateBlinking;
    double rateBleaching;
    double crosstalk;
    double directExcitation;

    double tDex;
    double tDexBreak;
    double tAex;
    double tAexBreak;
    inline double tDperiod() const {return tDex+tDexBreak;}
    inline double tAperiod() const {return tAex+tAexBreak;}
    inline double tPeriod() const {return tDex+tDexBreak+tAex+tAexBreak;}

    double tStart;
    inline double tEnd() const {return tStart+tDuration;}
    double tDuration;

    double t; /// the point in time of the simulation
    Photon p; /// the current photon

    Photons Dex(const double& rate, const double& tEnd, double directExc=0);
    Photons DemAex(const double& rate, const double& tEnd);
    Photons AemAex(const double& rate, const double &tEnd);

    /** basic functions
        burst is overloaded (see below)
      **/
    void background(const double meanDurationBackground, const bool minDuration = true);  /// adds photons with rateBackground to the photon stream. The length of the background burst is drawn from an exponential distribution. If minDuration is true, the burst is at least meanDurationBackground long.
    Photons burst(const double tstart, const double tend, const double eFRET=0);
    Photons donorOnly(const double tstart,const double tend);
    Photons acceptorOnly(const double tstart,const double tend);

    void burst(bool debug=true);   /// overload function, where the duration is drawn according to an exponential distribution.
    void burstFRET(const double FRET);
    void burstDonorOnly();
    void burstAcceptorOnly();
    void burstConstRate();
    void burstDelayedDonor();
    void burstBleachingDonor();
    void burstBlinkingDonor();
    void burstCoincidenceDonor(double scale=2);
    void burstDelayedAcceptor();
    void burstBleachingAcceptor();
    void burstBlinkingAcceptor();
    void burstCoincidenceAcceptor(double scale=2);

    double burstCount(QTextStream &out);
    int burstCount(const double tstart, const double tend);

    void simInit();


public:
    AlexSim(double tstart=0,double tend=1);
    ~AlexSim();
    void simScenario1(); /// a specific scenario can be tested here
    void simScenario2(bool writeToFile=true); /// test (nearly) all possible burst scenarios
    void simScenario3(int nBursts=50, bool writeToFile=true);
    void simBurstDuration(int nBursts=1000, bool writeToFile=true); ///
    void simTestLognormal(); /// sample from lognormal distribution and write histogram to file.

    QVector<double> photonArrivalTimeDonor;
    QVector<double> photonArrivalTimeAcceptor;
    Photons photons;

    inline int getNumberOfPhotons() {return photonArrivalTimeDonor.size()+photonArrivalTimeAcceptor.size();}
    inline double getBurstDuration() {return this->burstDuration;}

    inline void setRateDonor(const double k)  {this->rateDemDex=k;}
    inline void setRateAcceptor(const double k) {this->rateAemAex=k;}

    void writeArrivalTimes(const QString filename) const; /// write the vectors photonArrivalTimeDonor and photonArrivalTimeAcceptor to file filename
    void writeHist(const QString filename) const; /// histogram arrival times and write to file.
    void readArrivalTimes(const QString filename); /// read arrival times from file filename. The header lines must be marked with '#' and the donor and acceptor columns spearated by '\t'
    void readArrivalTimes(const QString filename, QVector<double> &arrivalTime ) const;/// read arrival times from file filename into the vector arrivalTime. The header lines must be marked with '#'.

protected:
    /** parameters and conversion functions for the lognormal distribution **/
    double sigma;
    double mu;
    double mLogNorm(const double mean, const double var) const;
    double sLogNorm(const double mean, const double var) const;
    /** random number generator from gsl library **/
    const gsl_rng_type * rngType;
    gsl_rng * r;

};

#endif // ALEXSIM_H
