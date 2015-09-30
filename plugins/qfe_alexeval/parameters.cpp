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

#include "parameters.h"

BurstSearchParams_t::BurstSearchParams_t()
{
    this->burstSearchMethodDual=AllPhotons;
    this->burstSearchMethodSingle=Sliding;
    this->M=2;
    this->T=500e-6;

    this->LeeWinSize=6;
    this->LeeVar=5e-6;

    this->MaxIPT=1e-4; // 100 us

    this->channel=DonorEx|DonorEm;

    this->MinBurstrate=10e3;
    this->MaxBurstrate=80e3;
    this->MaxDuration=10e-3;
    this->MinBurstSize=30;
    this->MinBurstSizeDex=0;
    this->MinBurstSizeAex=0;
}

const QString BurstSearchParams_t::toString(const QString prefix, const QString delimiter) const
{
    QString str;
    str=prefix;
    str+="BurstSearchParameters:"+delimiter;
//    str+="burstSearchMethodSingle="+QString::number((int)burstSearchMethodSingle) + delimiter;
//    str+="burstSearchMethodDual="+QString::number((int)burstSearchMethodDual) + delimiter;
    str+="burstSearchMethodSingle="+QString::fromStdString(burstSearchMethodSingleString()) + delimiter;
    str+="burstSearchMethodDual="+QString::fromStdString(burstSearchMethodDualString()) + delimiter;
    str+="channel selection="+QString::number(channel,2) + delimiter;
    str+="M="+QString::number(M) + delimiter;
    str+="L="+QString::number(L()) + delimiter;
    str+="T="+QString::number(T) + delimiter;

    str+="LeeWinSize="+QString::number(LeeWinSize) + delimiter;
    str+="LeeVar="+QString::number(LeeVar) + delimiter;
    str+="MaxIPT="+QString::number(MaxIPT) + delimiter;

    str+="MinBurstrate="+QString::number(MinBurstrate) + delimiter;
    str+="MaxBurstrate="+QString::number(MaxBurstrate) + delimiter;
    str+="MinBurstSize="+QString::number(MinBurstSize) + delimiter;
    str+="MinBurstSizeDex="+QString::number(MinBurstSizeDex) + delimiter;
    str+="MinBurstSizeAex="+QString::number(MinBurstSizeAex) + delimiter;
    str+="MaxDuration="+QString::number(MaxDuration) + delimiter;

    return str;
}

int BurstSearchParams_t::readFromIni(jkINIParser2 &ini, std::string group)
{
    BurstSearchParams_t defaultValuesBurst;
    ini.enterGroup(group); // burst search parameters
    burstSearchMethodSingle = (BurstSearchParams_t::BurstSearchMethodsSingle) ini.getAsInt("burstSearchMethodSingle", defaultValuesBurst.burstSearchMethodSingle);
    burstSearchMethodDual = (BurstSearchParams_t::BurstSearchMethodsDual) ini.getAsInt("burstSearchMethodDual", defaultValuesBurst.burstSearchMethodDual);
    channel = (uint)ini.getAsInt("channel", defaultValuesBurst.channel);

    M = ini.getAsInt("M", defaultValuesBurst.M);
    MinBurstSize= ini.getAsInt("MinBurstSize", defaultValuesBurst.MinBurstSize);
    MinBurstSizeDex= ini.getAsInt("MinBurstSizeDex", defaultValuesBurst.MinBurstSizeDex);
    MinBurstSizeAex= ini.getAsInt("MinBurstSizeAex", defaultValuesBurst.MinBurstSizeAex);
    T = ini.getAsDouble("T", defaultValuesBurst.T);

    LeeWinSize = ini.getAsInt("LeeWinSize", defaultValuesBurst.LeeWinSize);
    LeeVar = ini.getAsDouble("LeeVar", defaultValuesBurst.LeeVar);
    MaxIPT = ini.getAsDouble("MinIPT", defaultValuesBurst.MaxIPT);

    MinBurstrate = ini.getAsDouble("MinBurstrate", defaultValuesBurst.MinBurstrate);
    MaxBurstrate = ini.getAsDouble("MaxBurstrate", defaultValuesBurst.MaxBurstrate);
    MaxDuration = ini.getAsDouble("MaxDuration", defaultValuesBurst.MaxDuration);
    ini.leaveGroup();
    return EXIT_SUCCESS;
}

int BurstSearchParams_t::writeToIni(jkINIParser2 &ini, std::string group) const
{
    ini.enterGroup(group);
    ini.set("burstSearchMethodSingle",(int64_t)burstSearchMethodSingle);
    ini.set("burstSearchMethodDual",(int64_t) burstSearchMethodDual);
    ini.set("channel",(int64_t) channel);

    ini.set("M",(int64_t)M);
    ini.set("MinBurstSize", (int64_t)MinBurstSize);
    ini.set("MinBurstSizeDex", (int64_t)MinBurstSizeDex);
    ini.set("MinBurstSizeAex", (int64_t)MinBurstSizeAex);
    ini.set( "T", T);

    ini.set("LeeWinSize", (int64_t)LeeWinSize);
    ini.set("LeeVar", LeeVar);

    ini.set("MaxIPT", MaxIPT);
    ini.set("MinBurstrate", MinBurstrate);
    ini.set("MaxBurstrate", MaxBurstrate);
    ini.set("MaxDuration", MaxDuration);
    ini.leaveGroup();

    return EXIT_SUCCESS;

}



FRETparams_t::FRETparams_t()
{
    gamma = 1;
    crosstalk = 0.05;
    directExc=0.05;
    directExcThresholdDex=2;
    scaleAcceptor=1.0;
    useNegativeIntesities=false;

    bg_Dem_Dex=1000;
    bg_Aem_Dex=1000;
    bg_Dem_Aex=1000;
    bg_Aem_Aex=1000;

    dutyCycleAex=0.5;
    dutyCycleDex=0.5;

    MaxS  =+1.1;
    MinS  =-0.1;
    MaxP =+1.1;
    MinP =-0.1;

    MaxTgTr=5;
    MinTgTr=-5;
}

const QString FRETparams_t::toString(const QString prefix, const QString delimiter) const
{
    return  prefix +
            "FRETparameters:" + delimiter +
            "MaxS="+QString::number(MaxS) + delimiter +
            "MinS="+QString::number(MinS) + delimiter +
            "MaxPR="+QString::number(MaxP) + delimiter +
            "MinPR="+QString::number(MinP) + delimiter +
            "MaxTgTr="+QString::number(MaxTgTr) + delimiter +
            "MinTgTr="+QString::number(MinTgTr) + delimiter +
            "useNegativeIntesities="+QString::number(useNegativeIntesities) + delimiter +
            "gamma="+QString::number(gamma) + delimiter +
            "crosstalk="+QString::number(crosstalk) + delimiter +
            "directExc="+QString::number(directExc) + delimiter +
            "directExcThresholdDex="+QString::number(directExcThresholdDex) + delimiter +
            "scaleDonor="+QString::number(scaleAcceptor) + delimiter +
            "bg_Dem_Dex="+QString::number(bg_Dem_Dex) + delimiter +
            "bg_Aem_Dex="+QString::number(bg_Aem_Dex) + delimiter +
            "bg_Dem_Aex="+QString::number(bg_Dem_Aex) + delimiter +
            "bg_Aem_Aex="+QString::number(bg_Aem_Aex) + delimiter +
            "dutyCycleDex="+QString::number(dutyCycleDex) + delimiter +
            "dutyCycleAex="+QString::number(dutyCycleAex) + delimiter +
            "";
}

const QString FRETparams_t::filterToString(const QString prefix) const
{
    return  "Range:\n" +
            QString::number(MinP) + " < PR < " +
            QString::number(MaxP) + "\n" +
            QString::number(MinS) + " < S  < " +
            QString::number(MaxS) + "\n";
}

int FRETparams_t::readFromIni(jkINIParser2 &ini, std::string group)
{
    FRETparams_t defaultValuesFRET;

    ini.enterGroup(group);
    MaxP = ini.getAsDouble("MaxPR", defaultValuesFRET.MaxP);
    MinP = ini.getAsDouble("MinPR", defaultValuesFRET.MinP);
    MaxS = ini.getAsDouble("MaxS", defaultValuesFRET.MaxS);
    MinS = ini.getAsDouble("MinS", defaultValuesFRET.MinS);
    MaxTgTr = ini.getAsDouble("MaxTgTr", defaultValuesFRET.MaxTgTr);
    MinTgTr = ini.getAsDouble("MinTgTr", defaultValuesFRET.MinTgTr);
    useNegativeIntesities = ini.getAsDouble("useNegativeIntesities", defaultValuesFRET.useNegativeIntesities);
    gamma = ini.getAsDouble("gamma", defaultValuesFRET.gamma);
    crosstalk  = ini.getAsDouble("crosstalk",  defaultValuesFRET.crosstalk);
    directExc  = ini.getAsDouble("directExc",  defaultValuesFRET.directExc);
    directExcThresholdDex  = ini.getAsDouble("directExcThresholdDex",  defaultValuesFRET.directExcThresholdDex);
    scaleAcceptor = ini.getAsDouble("scaleDonor", defaultValuesFRET.scaleAcceptor);
    bg_Dem_Dex = ini.getAsDouble("bg_Dem_Dex", defaultValuesFRET.bg_Dem_Dex);
    bg_Aem_Dex = ini.getAsDouble("bg_Aem_Dex", defaultValuesFRET.bg_Aem_Dex);
    bg_Dem_Aex = ini.getAsDouble("bg_Dem_Aex", defaultValuesFRET.bg_Dem_Aex);
    bg_Aem_Aex = ini.getAsDouble("bg_Aem_Aex", defaultValuesFRET.bg_Aem_Aex);
    dutyCycleAex = ini.getAsDouble("dutyCycleAex", defaultValuesFRET.dutyCycleAex);
    dutyCycleDex = ini.getAsDouble("dutyCycleDex", defaultValuesFRET.dutyCycleDex);
    ini.leaveGroup();
    return EXIT_SUCCESS;
}

int FRETparams_t::writeToIni(jkINIParser2 &ini, std::string group) const
{
    ini.enterGroup(group);
    ini.set("MaxPR",MaxP);
    ini.set("MinPR", MinP);
    ini.set("MaxS", MaxS);
    ini.set("MinS", MinS);
    ini.set("MinTgTr", MinTgTr);
    ini.set("MaxTgTr", MaxTgTr);
    ini.set("useNegativeIntesities", useNegativeIntesities);
    ini.set("gamma", gamma);
    ini.set("crosstalk ", crosstalk);
    ini.set("directExc ", directExc);
    ini.set("directExcThresholdDex ", directExcThresholdDex);
    ini.set("scaleDonor", scaleAcceptor);
    ini.set("bg_Dem_Dex", bg_Dem_Dex);
    ini.set("bg_Aem_Dex", bg_Aem_Dex);
    ini.set("bg_Dem_Aex", bg_Dem_Aex);
    ini.set("bg_Aem_Aex", bg_Aem_Aex);
    ini.leaveGroup();

    return EXIT_SUCCESS;

}



FCSparams_t::FCSparams_t()
{
    this->FCSMethod=FCSstep;
    this->S=10;
    this->m=2;
    this->P=15;
    this->MinLagTime=1e-5;
    this->CorrelationWindow=10e-3;
    this->offset=0;
    this->backgroundAcceptor=1000;
    this->backgroundDonor=1000;
    this->tDex=110;
    this->tGapDexAex=10;
    this->tAex=90;
    this->tGapAexDex=10;
    this->BoxFilterIndexWindow=5;
    this->BoxFilterTimeWindow=100e-6;
    this->doBoxFiltering=false;
    this->doDivideByMarkerCorr=false;
    this->doDivideByLaterCorr=false;
    this->doCenterTau=false;
    this->dummyPhotonIPT=1e-6;
    nPeriods=1e4;
    binwidth=1e-5;
}

const QString FCSparams_t::toString(const QString prefix, const QString delimiter ) const
{
    return  prefix +
            "FCSparameters:"+ delimiter+
            "FCSMethod="+QString::number((int)FCSMethod) + delimiter+
            "doBoxFiltering="+QString::number(doBoxFiltering) + delimiter+
            "doDivideByMarkerCorr="+QString::number(doDivideByMarkerCorr) + delimiter+
            "doDivideByLaterCorr="+QString::number(doDivideByLaterCorr) + delimiter+
            "doIgnoreZeros="+QString::number(doIgnoreZeros) + delimiter+
            "doCenterTau="+QString::number(doCenterTau) + delimiter+
            "S="+QString::number(S) + delimiter+
            "m="+QString::number(m) + delimiter+
            "P="+QString::number(P) + delimiter+
            "binwidth="+QString::number(binwidth) + delimiter+
            "MinLagTime="+QString::number(MinLagTime) + delimiter+
            "CorrelationWindow="+QString::number(CorrelationWindow) + delimiter+
            "dummyPhotonIPT="+QString::number(dummyPhotonIPT) + delimiter+
            "nPeriods="+QString::number(nPeriods) + delimiter+
            "tDex="+QString::number(tDex) + delimiter+
            "tGapDexAex="+QString::number(tGapDexAex) + delimiter+
            "tAex="+QString::number(tAex) + delimiter+
            "tGapAexDex="+QString::number(tGapAexDex) + delimiter+
            "offset="+QString::number(offset) + delimiter+
            "backgroundDonor="+QString::number(backgroundDonor) + delimiter+
            "backgroundAcceptor="+QString::number(backgroundAcceptor) + delimiter+
            "BoxFilterIndexWindow="+QString::number(BoxFilterIndexWindow) + delimiter+
            "BoxFilterTimeWindow="+QString::number(BoxFilterTimeWindow);
}

int FCSparams_t::readFromIni(jkINIParser2 &ini, std::string group)
{
    FCSparams_t defaultValuesFCS;
    ini.enterGroup(group);
    FCSMethod = (FCSparams_t::FCSMethods) ini.getAsInt("FCSMethod", defaultValuesFCS.FCSMethod);
    doDivideByMarkerCorr = ini.getAsBool("doDivideByMarkerCorr", defaultValuesFCS.doDivideByMarkerCorr);
    doDivideByLaterCorr = ini.getAsBool("doDivideByLaterCorr", defaultValuesFCS.doDivideByLaterCorr);
    doBoxFiltering = ini.getAsBool("doBoxFiltering", defaultValuesFCS.doBoxFiltering);
    doIgnoreZeros = ini.getAsBool("doIgnoreZeros", defaultValuesFCS.doIgnoreZeros);
    doCenterTau = ini.getAsBool("doCenterTau", defaultValuesFCS.doCenterTau);

    S = ini.getAsInt("S", defaultValuesFCS.S);
    m = ini.getAsInt("m", defaultValuesFCS.m);
    P = ini.getAsInt("P", defaultValuesFCS.P);
    MinLagTime = ini.getAsDouble("MinLagTime", defaultValuesFCS.MinLagTime);

    CorrelationWindow = ini.getAsDouble("MaxLagTime", defaultValuesFCS.CorrelationWindow); // FIXME
    CorrelationWindow = ini.getAsDouble("CorrelationWindow", defaultValuesFCS.CorrelationWindow);
    dummyPhotonIPT = ini.getAsDouble("dummyPhotonIPT", defaultValuesFCS.dummyPhotonIPT);
    nPeriods = ini.getAsDouble("nPeriods", defaultValuesFCS.nPeriods);
    tDex = ini.getAsDouble("tDex", defaultValuesFCS.tDex);
    tAex = ini.getAsDouble("tAex", defaultValuesFCS.tAex);
    tGapAexDex = ini.getAsDouble("tGapAexDex", defaultValuesFCS.tGapAexDex);
    tGapDexAex = ini.getAsDouble("tGapDexAex", defaultValuesFCS.tGapDexAex);

    binwidth = ini.getAsDouble("binwidth", defaultValuesFCS.binwidth); // FIXME
    offset = ini.getAsInt("offset", defaultValuesFCS.offset);
    backgroundAcceptor = ini.getAsInt("backgroundAcceptor", defaultValuesFCS.backgroundAcceptor);
    backgroundDonor = ini.getAsInt("backgroundDonor", defaultValuesFCS.backgroundDonor);

    BoxFilterTimeWindow = ini.getAsDouble("timeWin", defaultValuesFCS.BoxFilterTimeWindow);
    BoxFilterIndexWindow = ini.getAsInt("indexWin", defaultValuesFCS.BoxFilterIndexWindow);
    BoxFilterTimeWindow = ini.getAsDouble("BoxFilterTimeWindow", defaultValuesFCS.BoxFilterTimeWindow);
    BoxFilterIndexWindow = ini.getAsInt("BoxFilterIndexWindow", defaultValuesFCS.BoxFilterIndexWindow);
    ini.leaveGroup();
    return EXIT_SUCCESS;
}

int FCSparams_t::writeToIni(jkINIParser2 &ini, std::string group) const
{
    ini.enterGroup(group);
    ini.set("FCSMethod",(int64_t)FCSMethod);
    ini.set("doBoxFiltering",doBoxFiltering);
    ini.set("doDivideByMarkerCorr",doDivideByMarkerCorr);
    ini.set("doDivideByLaterCorr",doDivideByLaterCorr);
    ini.set("doCenterTau",doCenterTau);
    ini.set("doIgnoreZeros",doIgnoreZeros);

    ini.set("S", (int64_t)S);
    ini.set("m", (int64_t)m);
    ini.set("P", (int64_t)P);
    ini.set("binwidth", binwidth);
    ini.set("MinLagTime", MinLagTime);

    ini.set("CorrelationWindow", CorrelationWindow);
    ini.set("nPeriods", nPeriods);
    ini.set("tDex", tDex);
    ini.set("tAex", tAex);
    ini.set("tGapAexDex", tGapAexDex);
    ini.set("tGapDexAex", tGapDexAex);

    ini.set("BoxFilterTimeWindow", BoxFilterTimeWindow);
    ini.set("BoxFilterIndexWindow", (int64_t)BoxFilterIndexWindow);
    ini.set("offset", (int64_t)offset);
    ini.leaveGroup();

    return EXIT_SUCCESS;
}





int readFromIniFCS(FCSparams_t &FCSparams, std::string iniFile)
{

    jkINIParser2 ini; // initialize to read the ini file
    if (iniFile.empty()) iniFile = FILE_ALEXINI ;

    qDebug()<<"reading FCS parameters from " << iniFile.c_str();
//    AlexEvalLog::text("reading FCS parameters from " + QString::fromStdString(iniFile));
    try
    {
        ini.readFile(iniFile); // read in an INI file
        FCSparams.readFromIni(ini);
    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int readFromIni(BurstSearchParams_t &burstSearchParams,BurstSearchParams_t &burstSearchParamsCh1,BurstSearchParams_t &burstSearchParamsCh2, FCSparams_t &FCSparams, FRETparams_t &FRETparams, std::string iniFile)
{

    jkINIParser2 ini; // initialize to read the ini file

    if (iniFile.empty()) iniFile = FILE_ALEXINI ;

    AlexEvalLog::text("reading parameters from " + QString::fromStdString(iniFile));
    try
    {
        ini.readFile(iniFile); // read in an INI file

        burstSearchParams.readFromIni(ini,"BurstSearchParams");// global burst search parameters
        burstSearchParamsCh1.readFromIni(ini,"burstSearchParamsCh1");// donor burst search parameters
        burstSearchParamsCh2.readFromIni(ini,"BurstSearchParamsCh2");// acceptor burst search parameters

        FRETparams.readFromIni(ini);

        FCSparams.readFromIni(ini);
    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }

    AlexEvalLog::text("loaded parameters from ini file " + QString::fromStdString(iniFile));

    return EXIT_SUCCESS;
}

// FIXME add new parameters, add case sensitivity
int writeToIni(const BurstSearchParams_t &burstSearchParams, const BurstSearchParams_t &burstSearchParamsCh1, const BurstSearchParams_t &burstSearchParamsCh2, const FCSparams_t &FCSparams, const FRETparams_t &FRETparams, const std::string iniFile)
{
    jkINIParser2 ini; // initialize to write the ini file

    if (iniFile.empty()) {
        AlexEvalLog::warning("filename empty");
        return EXIT_FAILURE;
    }

    AlexEvalLog::text("writing parameters to " + QString::fromStdString(iniFile));
    try
    {

        burstSearchParams.writeToIni(ini,"burstSearchParams");
        burstSearchParamsCh1.writeToIni(ini,"burstSearchParamsCh1");
        burstSearchParamsCh2.writeToIni(ini,"burstSearchParamsCh2");

        FRETparams.writeToIni(ini);

        FCSparams.writeToIni(ini);

        ini.writeFile(iniFile); // write the changed INI file to disk
    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }

    AlexEvalLog::text("written parameters to ini file " + QString::fromStdString(iniFile));

    return EXIT_SUCCESS;
}


int readFromIniBackground(FRETparams_t &FRETparams, const std::string iniFile)
{
    jkINIParser2 ini; // initialize to read the ini file

    if (iniFile.empty()) {
        AlexEvalLog::warning("filename empty");
        return EXIT_FAILURE;
    }
    AlexEvalLog::text("reading background from ini file " + QString::fromStdString(iniFile));
    try
    {
        ini.readFile(iniFile);
        FRETparams.bg_Dem_Dex = ini.getAsDouble("FRETparams.bg_Dem_Dex", FRETparams.bg_Dem_Dex);
        FRETparams.bg_Aem_Dex = ini.getAsDouble("FRETparams.bg_Aem_Dex", FRETparams.bg_Aem_Dex);
        FRETparams.bg_Dem_Aex = ini.getAsDouble("FRETparams.bg_Dem_Aex", FRETparams.bg_Dem_Aex);
        FRETparams.bg_Aem_Aex = ini.getAsDouble("FRETparams.bg_Aem_Aex", FRETparams.bg_Aem_Aex);
    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


FRETstatistics_t::FRETstatistics_t()
{
    Pmean=0;
    Pstd=0;
    Smean=0;
    Sstd=0;
    nAonly=0;
    nDonly=0;
    nDual=0;
    n=0;
    meanBurstDuration=0;
    meanBurstSize=0;
    meanBurstSizeA=0;
    meanBurstSizeD=0;
    quasiBulkP=0;
    quasiBulkS=0;

}

QString FRETstatistics_t::toString(const QString prefix,const QString delimiter) const
{
    return  prefix +
            "FRETstatistics:" + delimiter+
            "PRmean="+QString::number(Pmean) + delimiter+
            "PRstd="+QString::number(Pstd) + delimiter+
            "Smean="+QString::number(Smean) + delimiter+
            "Sstd="+QString::number(Sstd) + delimiter+
            "n="+QString::number(n) + delimiter+
            "nDual="+QString::number(nDual) + delimiter+
            "nDonly="+QString::number(nDonly) + delimiter+
            "nAonly="+QString::number(nAonly) + delimiter+
            "meanBurstSize="+QString::number(meanBurstSize) + delimiter+
            "meanBurstSizeD="+QString::number(meanBurstSizeD) + delimiter+
            "meanBurstSizeA="+QString::number(meanBurstSizeA) + delimiter+
            "meanBurstDuration="+QString::number(meanBurstDuration) + delimiter+
            "quasiBulkP="+QString::number(quasiBulkP) + delimiter+
            "quasiBulkS="+QString::number(quasiBulkS) + delimiter+
            + "";
}


const std::string BurstSearchParams_t::burstSearchMethodSingleString() const
{
    switch(burstSearchMethodSingle) {
    case Sliding:
        return "Sliding";
    case LeeFilter:
        return "LeeFilter";
    case Binned:
        return "Binned";
    };
    return "invalid method";
}

const std::string BurstSearchParams_t::burstSearchMethodDualString() const
{
    switch(burstSearchMethodDual) {
    case CombineBurstsOR:
        return "CombineBurstsOR";
    case DualChannel:
        return "DualChannel";
    case AllPhotons:
        return "AllPhotons";
    case DualChannelExt:
        return "DualChannelExt";
    case None:
        return "None";
    };
    return "invalid method";
}


