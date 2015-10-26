#include "alexsim.h"
#include<QDebug>
#include <float.h>

AlexSim::AlexSim(double tstart,double tend)
{
    qDebug("\n *** ALEX Simulation *** ");
    this->tStart=tstart;
    tDuration=tend-tstart;
    if(tDuration<=0) {
        qFatal("enter times tstart<tend");
    }
    t=0;
    burstDuration=3e-3; // 2ms
    burstDurationVar=18e-7; // variance of burst duration in seconds^2, estimated from data
    rateDemDex=6e4; // 50kHz
    rateAemAex=6e4;
    rateBackground=1e3; // 1kHz
    rateFirst=1e9;
    rateBlinking;
    rateBleaching=4e3; // per second
    lifetimeBleaching=5e-3;
    lifetimeBlinking=10e-6; // triplet state lifetime ca 3 micro seconds
    crosstalk=0.03;
    directExcitation=0.02;

    tDex=100e-6;
    tAex=80e-6;
    tDexBreak=10e-6;
    tAexBreak=10e-6;

     // photonArrivalTimeChannel1.reserve();

    /* create a generator chosen by the environment variable GSL_RNG_TYPE */
    gsl_rng_env_setup();
    rngType = gsl_rng_default;
    r = gsl_rng_alloc(rngType);
    gsl_rng_set(r, 0); // set the seed to 0, i.e. the default value

    // approximate burst size distribution as lognormal
    sigma=sLogNorm(burstDuration,burstDurationVar);
    mu=mLogNorm(burstDuration, burstDurationVar);
    qDebug()<<"lognormal mu="<<mu;
    qDebug()<<"lognormal sigma="<<sigma;

}



AlexSim::~AlexSim()
{
    gsl_rng_free(r);
}



void AlexSim::simScenario1()
{
    simInit();
    double muTimestep=2*burstDuration;

    background(muTimestep);
    burst();
    background(muTimestep);
    burst();
    background(muTimestep);
    burstBleachingDonor();
    background(muTimestep);
    burstBlinkingAcceptor();
    background(muTimestep);
    burst();
    background(muTimestep);
    burst();
    background(muTimestep);
    burstCoincidenceDonor();
    background(muTimestep);
    burst();
    background(muTimestep);

    writeHist("AlexSimHist.txt");
    writeArrivalTimes("./AlexSimPhotonTimes.txt");
}



void AlexSim::simScenario2(bool writeToFile)
{
    simInit();
    double muTimestep=5*burstDuration;

    background(muTimestep);    burst();
    background(muTimestep);    burst();
    background(muTimestep);    burstFRET(0.5);
    background(muTimestep);    burstFRET(0.5);
    background(muTimestep);    burst();
    background(muTimestep);    burstAcceptorOnly();
    background(muTimestep);    burstAcceptorOnly();
    background(muTimestep);    burstDonorOnly();
    background(muTimestep);    burstDonorOnly();

    background(muTimestep);    burstDelayedDonor();
    background(muTimestep);    burstDelayedDonor();
    background(muTimestep);    burstDelayedAcceptor();
    background(muTimestep);    burstDelayedAcceptor();

    background(muTimestep);    burstBleachingDonor();
    background(muTimestep);    burstBleachingDonor();
    background(muTimestep);    burstBleachingAcceptor();
    background(muTimestep);    burstBleachingAcceptor();

    background(muTimestep);    burstBlinkingDonor();
    background(muTimestep);    burstBlinkingDonor();
    background(muTimestep);    burstBlinkingAcceptor();
    background(muTimestep);    burstBlinkingAcceptor();

    background(muTimestep);    burstDelayedDonor();     burstAcceptorOnly();
    background(muTimestep);    burstDelayedDonor();     burstAcceptorOnly();
    background(muTimestep);    burstDelayedAcceptor();  burstDonorOnly();
    background(muTimestep);    burstDelayedAcceptor();  burstDonorOnly();

    background(muTimestep);    burstDelayedDonor();     burstDonorOnly();
    background(muTimestep);    burstDelayedDonor();     burstDonorOnly();
    background(muTimestep);    burstDelayedAcceptor();  burstAcceptorOnly();
    background(muTimestep);    burstDelayedAcceptor();  burstAcceptorOnly();

    background(muTimestep);    burstCoincidenceDonor();
    background(muTimestep);    burstCoincidenceDonor();
    background(muTimestep);    burstCoincidenceAcceptor();
    background(muTimestep);    burstCoincidenceAcceptor();

    background(muTimestep);

    writeHist("AlexSimHist.txt");
    if (writeToFile) writeArrivalTimes("./AlexSimPhotonTimes.csv");
}



void AlexSim::simScenario3(int nBursts, bool writeToFile)
{
    simInit();
    double muTimestep=2*burstDuration;
    qDebug("simScenario3 time t=%.2f",t);
    for(int j=0; j<nBursts; j++) {
        background(muTimestep);
        burst(false);
    }

    qDebug("time t=%.2f",t);
    for(int j=0; j<nBursts; j++) {
        background(muTimestep);
        burstCoincidenceAcceptor(3.0);
    }

    qDebug("time t=%.2f",t);
    for(int j=0; j<nBursts; j++) {
        background(muTimestep);
        burst(false);
    }

    qDebug("time t=%.2f",t);
    for(int j=0; j<nBursts; j++) {
        background(muTimestep);
        burstCoincidenceDonor(3.0);
    }
    qDebug("time t=%.2f",t);
    for(int j=0; j<nBursts; j++) {
        background(muTimestep);
        burst(false);
    }

}



void AlexSim::simBurstDuration(int nBursts,bool writeToFile)
{
    // simulate many photon bursts and write their sampled duration and burst size to file --> linear relationship?

    simInit();
//    photonArrivalTimeDonor.reserve((int)(nBursts*burstDuration*rateDemDex));
//    photonArrivalTimeAcceptor.reserve((int)(nBursts*burstDuration*rateAemAex));

    if(writeToFile) {
        QFile file("./AlexSimBurstDuration.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out <<"# Simulation data. All times in ms.Parameters are:\n";
        out <<"# rateDonor="<<rateDemDex<<"\trateAcceptor="<<rateAemAex<<"/trateBackground="<<rateBackground<<"\tburstDuration="<<burstDuration<<"\tvariance="<<burstDurationVar<<"\ttStart="<<tStart<<"\ttEnd="<<tEnd()<<"\n";
        out.setRealNumberPrecision(11);
        out <<"#  time \tburst duration\t#photons\n";

        for(int i=0;i<nBursts;i++) {
            burstCount(out);
            background(burstDuration);
        }

        file.close();
        writeArrivalTimes("./AlexSimPhotonTimes.txt");
    } else {
        for(int i=0;i<nBursts;i++) {
            burst(false);
            background(burstDuration);
        }

    }

}



// a burst from tstart with length of burstDuration
// interphoton times are assumed to be exponentially distributed
void AlexSim::burst(bool debug)
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    if(debug) qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}

void AlexSim::burstFRET(const double FRET)
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    Photons photonsTemp;
    photonsTemp<<burst(tstart, tend,FRET);
    photonsTemp.sort();
    photons<<photonsTemp;
}

void AlexSim::burstDonorOnly()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Donor only ";

    Photons photonsTemp;
    photonsTemp<<donorOnly(tstart, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}

void AlexSim::burstAcceptorOnly()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Acceptor only";

    Photons photonsTemp;
    photonsTemp<<acceptorOnly(tstart, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}


double AlexSim::burstCount(QTextStream &out)
{
    double tstart=t;
    // double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double burstDurationSample=gsl_ran_exponential(r, burstDuration);
    double tend = tstart + burstDurationSample;

    int nPhotons=burstCount(tstart, tend);

    out<<t*1e3<<"\t"<<burstDurationSample*1e3<<"\t"<<nPhotons<<"\n";

    return burstDurationSample;
}



void AlexSim::burstBleachingDonor() // bleaching at a random time during a burst (for the rest of the burst)
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tBleach=gsl_ran_flat(r, tstart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Donor bleaching after "<<(tBleach-tstart)*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tBleach);
    photonsTemp<<acceptorOnly(tBleach, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstBleachingAcceptor()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tBleach=gsl_ran_flat(r, tstart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Acceptor bleaching after "<<(tBleach-tstart)*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tBleach);
    photonsTemp<<donorOnly(tBleach, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstBlinkingDonor() // The time a flourophore blinks during a burst is chosen at random (from a uniform distribtion) and its length is drawn from an exponential distribution with mean value of the triplet lifetime.
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tBlinkStart=gsl_ran_flat(r, tstart, tend);
    double tBlinkEnd=tBlinkStart + gsl_ran_exponential(r, lifetimeBlinking);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Donor blinks after"<<(tBlinkStart-tstart)*1e3<<"ms for"<<(tBlinkEnd-tBlinkStart)*1e3<<"ms";

    burst(tstart, tBlinkStart);
    acceptorOnly(tBlinkStart, tBlinkEnd);
    burst(tBlinkEnd, tend);

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tBlinkStart);
    photonsTemp<<acceptorOnly(tBlinkStart, tBlinkEnd);
    photonsTemp<<burst(tBlinkEnd, tend);
    photonsTemp.sort();
    photons<<photonsTemp;

}



void AlexSim::burstBlinkingAcceptor()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tBlinkStart=gsl_ran_flat(r, tstart, tend);
    double tBlinkEnd=tBlinkStart + gsl_ran_exponential(r, lifetimeBlinking);
    if (tBlinkEnd>tend) tBlinkEnd=tend;
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Acceptor blinks after"<<(tBlinkStart-tstart)*1e3<<"ms for"<<(tBlinkEnd-tBlinkStart)*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tBlinkStart);
    photonsTemp<<donorOnly(tBlinkStart, tBlinkEnd);
    photonsTemp<<burst(tBlinkEnd, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstCoincidenceAcceptor(double scale)
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tCoincidenceStart=gsl_ran_flat(r, tstart, tend);
    double tCoincidenceEnd=gsl_ran_flat(r, tCoincidenceStart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Acceptor coincidence after"<<(tCoincidenceStart-tstart)*1e3<<"ms for"<<(tCoincidenceEnd-tCoincidenceStart)*1e3<<"ms for"<<(tCoincidenceEnd-tCoincidenceStart)*1e3<<"ms with scale parameter"<<scale;

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tend);
    photonsTemp<<acceptorOnly(tCoincidenceStart,tCoincidenceEnd);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstCoincidenceDonor(double scale)
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tCoincidenceStart=gsl_ran_flat(r, tstart, tend);
    double tCoincidenceEnd=gsl_ran_flat(r, tCoincidenceStart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Donor coincidence after"<<(tCoincidenceStart-tstart)*1e3<<"ms for"<<(tCoincidenceEnd-tCoincidenceStart)*1e3<<"ms with scale parameter"<<scale;

    Photons photonsTemp;
    photonsTemp<<burst(tstart, tend);
    photonsTemp<<donorOnly(tCoincidenceStart,tCoincidenceEnd);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstDelayedDonor()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tDelayed=gsl_ran_flat(r, tstart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Donor delayed for"<<(tDelayed-tstart)*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<donorOnly(tstart, tDelayed);
    photonsTemp<<burst(tDelayed, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}



void AlexSim::burstDelayedAcceptor()
{
    double tstart=t;
    double burstDurationSample=gsl_ran_lognormal(r, mu, sigma);
    double tend = tstart + burstDurationSample;
    double tDelayed=gsl_ran_flat(r, tstart, tend);
    qDebug()<<"burst at"<<t*1e3<<"ms for"<<burstDurationSample*1e3<<"ms. Acceptor delayed for"<<(tDelayed-tstart)*1e3<<"ms";

    Photons photonsTemp;
    photonsTemp<<acceptorOnly(tstart, tDelayed);
    photonsTemp<<burst(tDelayed, tend);
    photonsTemp.sort();
    photons<<photonsTemp;
}






int AlexSim::burstCount(const double tstart, const double tend)
{
    t=tstart;
    int nPhotonsD=0,nPhotonsA=0;
    while(t<tend) {
        photonArrivalTimeDonor.push_back(t);
        nPhotonsD++;
        t+=gsl_ran_exponential(r, 1/rateDemDex);
    }

    t=tstart;
    while(t<tend) {
        photonArrivalTimeAcceptor.push_back(t);
        nPhotonsA++;
        t+=gsl_ran_exponential(r, 1/rateAemAex);
    }
    return nPhotonsD+nPhotonsA;
}



void AlexSim::burstConstRate()
{
    double tstart=t;
    double tend = tstart + burstDuration;
    while(t<tend) {
        photonArrivalTimeDonor.push_back(t);
        t+=1/rateDemDex;
    }

    t=tstart;
    while(t<tend) {
        photonArrivalTimeAcceptor.push_back(t);
        t+=1/rateAemAex;
    }
}


Photons AlexSim::Dex(const double &rate, const double &tEnd, double directExc)
{
    Photons photonsTemp;
    t+=gsl_ran_exponential(r, 1/rateFirst);
    while(t<tEnd) {
        while((t<tEnd)&&(fmod(t,tPeriod())<tDperiod())) {
            if(gsl_ran_flat(r,0,1)>directExc) p.setFlags(DonorEm,DonorEx);
            else p.setFlags(AcceptorEm,DonorEx);
            p.time=t;
            photonsTemp.append(p);
            t+=gsl_ran_exponential(r, 1/rate);
        }
        t+=tDperiod();
    }
    return photonsTemp;
}

Photons AlexSim::DemAex(const double &rate, const double &tEnd)
{
    Photons photonsTemp;
    t+=gsl_ran_exponential(r, 1/rateFirst);
    while(t<tEnd) {
        while((t<tEnd)&&(fmod((t+tDperiod()),tPeriod())<tAperiod())) {
            p.setFlags(DonorEm,AcceptorEx);
            p.time=t;
            photonsTemp.append(p);
            t+=gsl_ran_exponential(r, 1/rate);
        }
        t+=tAperiod();
    }
    return photonsTemp;
}

Photons AlexSim::AemAex(const double &rate, const double &tEnd)
{
    Photons photonsTemp;
    t+=gsl_ran_exponential(r, 1/rateFirst);
    while(t<tEnd) {
        while((t<tEnd)&&(fmod((t+tDperiod()),tPeriod())<tAperiod())) {
            p.setFlags(AcceptorEm,AcceptorEx);
            p.time=t;
            photonsTemp.append(p);
            t+=gsl_ran_exponential(r, 1/rate);
        }
        t+=tAperiod();
    }
    return photonsTemp;
}

Photons AlexSim::burst(const double tstart, const double tend,const double eFRET)
{
    t=tstart;
    Photons unsortedPhotons;
    unsortedPhotons<<Dex(rateDemDex,tend,eFRET+directExcitation);
    t=tstart;
    unsortedPhotons<<AemAex(rateAemAex,tend);
    // background:
    t=tstart;
    unsortedPhotons<<Dex(2*rateBackground,tend,0.5);
    t=tstart;
    unsortedPhotons<<AemAex(rateBackground,tend);
    t=tstart;
    unsortedPhotons<<DemAex(rateBackground,tend);

    return unsortedPhotons;
}


void AlexSim::background(const double meanDurationBackground, const bool minDuration)
{
    double tstart=t;
    double duration=meanDurationBackground;
//    double duration=gsl_ran_exponential(r, meanDurationBackground);
//    if(minDuration) {
//        while(duration<meanDurationBackground) duration=gsl_ran_exponential(r, meanDurationBackground);
//    }

    double tend=tstart+duration;
    Photons unsortedPhotons;
    unsortedPhotons<<Dex(2*rateBackground,tend,0.5);
    t=tstart;
    unsortedPhotons<<AemAex(rateBackground,tend);
    t=tstart;
    unsortedPhotons<<DemAex(rateBackground,tend);

    unsortedPhotons.sort();
    this->photons<<unsortedPhotons; // now sorted :-)
}



Photons AlexSim::acceptorOnly(const double tstart, const double tend)
{
    t=tstart;
    Photons unsortedPhotons;
    unsortedPhotons<<Dex(rateAemAex*directExcitation,tend,1-directExcitation);
    t=tstart;
    unsortedPhotons<<AemAex(rateAemAex,tend);
    t=tstart;
    unsortedPhotons<<DemAex(rateBackground,tend);
    return unsortedPhotons;
}



Photons AlexSim::donorOnly(const double tstart, const double tend)
{
    t=tstart;
    Photons unsortedPhotons;
    unsortedPhotons<<Dex(rateDemDex,tend,directExcitation);
    t=tstart;
    unsortedPhotons<<AemAex(rateBackground,tend);
    t=tstart;
    unsortedPhotons<<DemAex(rateBackground,tend);
    return unsortedPhotons;
}


void AlexSim::simInit()
{
    qDebug()<<"Initializing. tau background"<<1.0/rateBackground*1e3<<"ms. tStart="<<tStart;
    if (photonArrivalTimeDonor.size()>0) {
        qDebug()<<"overwriting data";
        photonArrivalTimeDonor.clear();
        photonArrivalTimeAcceptor.clear();
    }
//    photonArrivalTimeDonor.reserve((int)((tEnd-tStart)*rateDonor));
//    photonArrivalTimeAcceptor.reserve((int)((tEnd-tStart)*rateAcceptor));
    t=tStart;
}



void AlexSim::writeHist(const QString filename) const
{
    long ii;
    long nbins=(long)((t-tStart)/burstDuration*10);
    if(nbins==0||photons.size()==0) qWarning()<<"writeHist: no photon records ";
    gsl_histogram * histDonor = gsl_histogram_alloc (nbins);
    gsl_histogram_set_ranges_uniform (histDonor, tStart, t); // change t to tEnd if the latter is implemented
    gsl_histogram * histAcceptor = gsl_histogram_alloc (nbins);
    gsl_histogram_set_ranges_uniform (histAcceptor, tStart, t); // change t to tEnd if the latter is implemented
    for (ii=0;ii<photons.size();ii++) {
#ifdef PHOTONMACRO
        if(isChannel(photons.at(ii),DonorEm)) gsl_histogram_increment (histDonor, photons.at(ii).time);
#else
        if(photons.at(ii).isChannel(DonorEm)) gsl_histogram_increment (histDonor, photons.at(ii).time);
#endif
        else gsl_histogram_increment (histAcceptor, photons.at(ii).time);
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) QMessageBox::warning(0, "error", file.errorString());
    QTextStream out(&file);
    out <<"# Simulation data. photon count over time. parameters are:\n";
    out <<"# rateDonor="<<rateDemDex<<"\trateAcceptor="<<rateAemAex<<"\trateBackground="<<rateBackground<<"\tburstDuration="<<burstDuration<<"\tsigma="<<sigma<<"\ttStart="<<tStart<<"\ttEnd="<<tEnd()<<"\n";
    out.setRealNumberPrecision(11);
    out <<"#  time in ms \tdonor channel \tacceptor channel\n";

    for(ii=0;ii<histDonor->n;ii++) {
        out << histDonor->range[ii]*1e3 << "\t" << gsl_histogram_get(histDonor,ii) << "\t" << gsl_histogram_get(histAcceptor,ii) << "\n";
    }
    file.close();
    gsl_histogram_free (histDonor);
    gsl_histogram_free (histAcceptor);

    qDebug()<<nbins<<"histogram entries written to file "<<filename;
}



void AlexSim::readArrivalTimes(const QString filename) //FIXME
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) QMessageBox::warning(0, "Warning", file.errorString());
    qDebug()<<"read arrival times of file"<<filename;
    QTextStream in(&file);
    if (photonArrivalTimeDonor.size()||photonArrivalTimeAcceptor.size()) {
        qWarning("overwriting data");
        photonArrivalTimeDonor.clear();
        photonArrivalTimeAcceptor.clear();
    }
    QStringList fields;
    bool conv_ok=true;
    while(in.readLine().contains("#")) {}// skip header lines
    while(!in.atEnd()&&conv_ok) {
        fields = in.readLine().split("\t");
        photonArrivalTimeDonor.push_back(fields.at(0).toDouble(&conv_ok));
        photonArrivalTimeAcceptor.push_back(fields.at(1).toDouble(&conv_ok));
    }
    if(!conv_ok) QMessageBox::information(0, "error", "conversion to double failed");

    file.close();
}



void AlexSim::readArrivalTimes(const QString filename, QVector<double> &arrivalTime) const //FIXME
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) QMessageBox::warning(0, "Warning", file.errorString());
    qDebug()<<"read arrival times of file"<<filename;
    QTextStream in(&file);
    if (arrivalTime.size()) {
        qWarning("overwriting data");
        arrivalTime.clear();
    }
    bool conv_ok=true;
    QString line;
    int lc=-1;
    while(!in.atEnd()) {// skip header lines
        line=in.readLine();
        lc++;
        if(line.contains("#")||line.contains("\\")||line.contains("/")) continue;
        else break;
    }
    qDebug()<<"cut "<<lc<<"lines of header";
    while(!in.atEnd()&&conv_ok) {
        conv_ok=true;
        double d=line.toDouble(&conv_ok);
        if (conv_ok) arrivalTime.push_back(d);
        line=in.readLine();
        lc++;
    }
    qDebug()<<"lines read: "<<lc<<"   photons: "<<arrivalTime.size();
    if(!conv_ok) QMessageBox::information(0, "error", "conversion to double failed");
    file.close();
}




void AlexSim::simTestLognormal()
{
    int n=10000; // number of samples
    int nbins=100; // number of bins for the histogram

    QFile file("./AlexSimTestLognormal.txt");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) QMessageBox::information(0, "error", file.errorString());
    QTextStream out(&file);
    out <<"# Test lognormal distribution. Parameters are: burstDuration="<<burstDuration<<"\tvariance="<<burstDurationVar<<"\n";
    out <<"# burst duration in ms\tfrequency\n";
    out.setRealNumberPrecision(11);

    gsl_histogram * hist = gsl_histogram_alloc (nbins);
    gsl_histogram_set_ranges_uniform (hist, 0, 10*burstDuration);
    for (int ii=0;ii<n;ii++) {
        gsl_histogram_increment (hist,gsl_ran_lognormal(r,mu,sigma));
    }

    for(unsigned int ii=0;ii<hist->n;ii++) {
        out << hist->range[ii]*1e3 << "\t" << gsl_histogram_get(hist,ii) << "\n";
    }
    file.close();
    gsl_histogram_free (hist);
}



double AlexSim::mLogNorm(const double mean, const double var) const // gives the lognormal parameter m of an underlying normal distribution with expectation value mean and variance var. See wikipedia on lognormal distribution
{
    return log(mean*mean/(sqrt(var+mean*mean)));
}

double AlexSim::sLogNorm(const double mean, const double var) const // gives the lognormal parameter s of an underlying normal distribution with expectation value mean and variance var. See wikipedia on lognormal distribution
{
    return sqrt(log(var/(mean*mean)+1));
}



void AlexSim::writeArrivalTimes(const QString filename) const
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) QMessageBox::information(0, "error", file.errorString());
    QTextStream out(&file); out.setRealNumberPrecision(11);
    out <<"# Simulation data. All times in s. Parameters are:\n";
    out <<"# rateDonor="<<rateDemDex<<"\trateAcceptor="<<rateAemAex<<"\trateBackground="<<rateBackground<<"\tburstDuration="<<burstDuration<<"\tsigma="<<sigma<<"\ttStart="<<tStart<<"\ttEnd="<<tEnd()<<"\n";
    out <<"#  time\tchannel\n";

    long n=photons.size();
    for(long ii=0;ii<n;ii++) {
        out <<photons.value(ii).time<<"\t"<<QString::number(photons.value(ii).flags,2)<<"\n";
    }
    file.close();

    qDebug()<<n<<"photon times written to file "<<filename;

}


