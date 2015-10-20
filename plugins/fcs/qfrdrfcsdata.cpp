/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

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

#include "qfrdrfcsdata.h"
#include <errno.h>
#include <QtXml>
#include "jkiniparser2.h"
#include "binarydatatools.h"
#include "flex_sin_tools.h"
#include "alv5000tools.h"
#include "alv6000tools.h"
#include "alv7000tools.h"
#include "confocor3tools.h"
#include "qf3correlationdataformattool.h"
#include "dlgcsvparameters.h"

#define loadSeveral(res, filenames, loadFunction) {\
    res=false; \
    if (filenames.size()>0) { \
        res=true; \
        QList<dataArr> otherdata; \
        for (int i=0; i<filenames.size(); i++) { \
            res=res&&loadFunction(filenames[i]);\
            /*qDebug() << "loaded file "<<filenames[i]<<"\n    correlationRuns="<<correlationRuns<<" rateRuns="<<rateRuns<<"  ==> RES="<<res;*/\
            dataArr t_; \
            t_.correlationRuns=correlationRuns; \
            t_.correlationN=correlationN; \
            t_.rateRuns=rateRuns; \
            t_.rateN=rateN; \
            t_.rateChannels=rateChannels; \
            t_.correlationT=duplicateArray(correlationT, t_.correlationN); \
            t_.correlation=duplicateArray(correlation, t_.correlationRuns*t_.correlationN); \
            t_.correlationErrors=duplicateArray(correlationErrors, t_.correlationRuns*t_.correlationN); \
            t_.rateT=duplicateArray(rateT, t_.rateN); \
            t_.rate=duplicateArray(rate, t_.rateRuns*t_.rateN*t_.rateChannels); \
            otherdata<<t_; \
        } \
        if (res && otherdata.size()>1) { \
            int cruns=otherdata[0].correlationRuns; \
            int rruns=otherdata[0].correlationRuns; \
            long long int ritems=otherdata[0].rateN; \
            for (int i=1; i<otherdata.size(); i++) { \
                bool ok=    (abs(otherdata[0].rateN-otherdata[i].rateN)<=qMax((long long int)2,otherdata[0].rateN/50)) \
                         && (otherdata[0].rateChannels==otherdata[i].rateChannels) \
                         && (otherdata[0].correlationN==otherdata[i].correlationN); \
                if (!ok) {\
                    setError(tr("error loading file '%1': number of data points in rates/correlations, or number of channels was different than in first file (FIRST/THIS FILE: rateN=%2/%3, corrN=%4%5, chan=%6/%7)").arg(filenames.value(i)).arg(otherdata[0].rateN).arg(otherdata[i].rateN).arg(otherdata[0].correlationN).arg(otherdata[i].correlationN).arg(otherdata[0].rateChannels).arg(otherdata[i].rateChannels)); \
                }\
                res=res&&ok;\
                ritems=qMin(ritems, otherdata[i].rateN);\
                if (res) { \
                    for (int t=0; t<correlationN; t++) { \
                        if (fabs(otherdata[0].correlationT[t]-otherdata[i].correlationT[t])>1e-3*otherdata[0].correlationT[t]) { \
                            res=false; \
                            setError(tr("error loading file '%1': lag-time axis does not equal the previous files").arg(filenames.value(i))); \
                        } \
                    } \
                    for (int t=0; t<ritems; t++) { \
                        if (fabs(otherdata[0].rateT[t]-otherdata[i].rateT[t])>1e-3*otherdata[0].rateT[t]) { \
                            res=false; \
                            setError(tr("error loading file '%1': rate time-axis does not equal the previous files").arg(filenames.value(i))); \
                        } \
                    } \
                } \
                if (res) { \
                    cruns=cruns+otherdata[i].correlationRuns; \
                    rruns=rruns+otherdata[i].rateRuns; \
                } \
                /*qDebug() << "checked file "<<filenames.value(i)<<"  OK="<<ok<<"  RES="<<res<<"\n    correlationRuns="<<otherdata[i].correlationRuns<<"/"<<otherdata[0].correlationRuns<<" rateRuns="<<otherdata[i].rateRuns<<"/"<<otherdata[0].rateRuns<<" rateChannels="<<otherdata[i].rateChannels<<"/"<<otherdata[0].rateChannels<<" rateN="<<otherdata[i].rateN<<"/"<<otherdata[0].rateN<<" correlationN="<<otherdata[i].correlationN<<"/"<<otherdata[0].correlationN<<"  ==>   cruns="<<cruns<<" rruns="<<rruns;*/\
                if (!res) break;\
            } \
            if (res) { \
                resizeCorrelations(correlationN, cruns); \
                resizeRates(ritems, rruns, otherdata.first().rateChannels); \
                copyArray(correlationT, otherdata.first().correlationT, correlationN); \
                copyArray(rateT, otherdata.first().rateT, rateN); \
                long long rstart=0; \
                long long cstart=0; \
                for (int i=0; res && i<otherdata.size(); i++) { \
                    dataArr t_=otherdata[i]; \
                    copyArray(&(correlation[cstart]), t_.correlation, t_.correlationRuns* t_.correlationN); \
                    copyArray(&(correlationErrors[cstart]), t_.correlationErrors, t_.correlationRuns* t_.correlationN); \
                    for (int c=0; c<t_.rateChannels; c++) { \
                        copyArray(&(rate[c*rateRuns*rateN+ rstart]), &(t_.rate[c*t_.rateRuns*rateN]), t_.rateRuns* rateN); \
                    } \
                    cstart=cstart+t_.correlationRuns* t_.correlationN; \
                    rstart=rstart+rateN*t_.rateRuns; \
                } \
                recalculateCorrelations();\
                calcBinnedRate();\
            } \
            for (int i=0; res && i<otherdata.size(); i++) { \
                dataArr t_=otherdata[i]; \
                if (t_.correlation) qfFree(t_.correlation); \
                if (t_.correlationErrors) qfFree(t_.correlationErrors); \
                if (t_.correlationT) qfFree(t_.correlationT); \
                if (t_.rateT) qfFree(t_.rateT); \
                if (t_.rate) qfFree(t_.rate); \
            } \
            otherdata.clear(); \
        } \
    } \
}

QList<QPair<QString, QString> > QFRDRFCSData::getFileTypesAndFilters()
{
    QList<QPair<QString, QString> > ft;
    ft.append(qMakePair(QString("ALV5000"), tr("ALV-5000 file (*.asc)")));
    ft.append(qMakePair(QString("ALV6000"), tr("ALV-6000 file (*.asc)")));
    ft.append(qMakePair(QString("ALV7000"), tr("ALV-7000 file (*.asc)")));
    ft.append(qMakePair(QString("CORRELATOR.COM_SIN"), tr("correlator.com files (*.sin)")));
    ft.append(qMakePair(QString("CONFOCOR3"), tr("Zeiss Confocor2/3 files [ASCII] (*.fcs *.fcs.txt)")));
    ft.append(qMakePair(QString("ISS_ALBA"), tr("ISS Alba Files (*.csv)")));
    ft.append(qMakePair(QString("PICOQUANT_ASCII_COR"), tr("PicoQuant FCS Files from v6.0 TTTR correlator (*.cor)")));
    ft.append(qMakePair(QString("PICOQUANT_ASCII_FCS_W"), tr("PicoQuant ASCII FCS Curves Files with weights (*.dat)")));
    ft.append(qMakePair(QString("PICOQUANT_ASCII_FCS"), tr("PicoQuant ASCII FCS Curves Files (*.dat)")));
    ft.append(qMakePair(QString("QF3ASCIICORR"), tr("QuickFit 3.0 ASCII Correlation Data (*.qf3acorr)")));
    ft.append(qMakePair(QString("CSV_CORR"), tr("ASCII Data Files (*.txt *.dat *.csv)")));
    ft.append(qMakePair(QString("DIFFUSION4_SIMRESULTS"), tr("diffusion4 correlation (*corr.dat)")));
    ft.append(qMakePair(QString("OLEGKRIECHEVSKYBINARY"), tr("Oleg Kriechevsky's Binary format(*. *.dat)")));
    return ft;
}

QString QFRDRFCSData::getFileTypesFilterForID(const QString &id)
{
    QList<QPair<QString, QString> > fl=getFileTypesAndFilters();
    for (int i=0; i<fl.size(); i++) {
        if (fl[i].first.toUpper()==id.trimmed().toUpper()) return fl[i].second;
    }
    return "";
}

QString QFRDRFCSData::getFileTypesIDForFilter(const QString &filter)
{
    QList<QPair<QString, QString> > fl=getFileTypesAndFilters();
    for (int i=0; i<fl.size(); i++) {
        if (fl[i].second==filter.trimmed()) return fl[i].first;
    }
    return "";
}

void QFRDRFCSData::fileFiltersSetFCSFilterProperties(QMap<QString, QVariant> &p, const QString &currentFCSFileFormatFilter, const QString &filename, ProgramOptions *settings, QWidget *parentWidget)
{
    if (currentFCSFileFormatFilter==getFileTypesFilterForID("ALV5000")) {
        p["FILETYPE"]="ALV5000";
        p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("ALV6000")){
       p["FILETYPE"]="ALV6000";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("ALV7000")){
       p["FILETYPE"]="ALV7000";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("CORRELATOR.COM_SIN")){
       p["FILETYPE"]="CORRELATOR.COM_SIN";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("CONFOCOR3")){
       p["FILETYPE"]="CONFOCOR3";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("OLEGKRIECHEVSKYBINARY")){
       p["FILETYPE"]="OLEGKRIECHEVSKYBINARY";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("QF3ASCIICORR")){
       p["FILETYPE"]="QF3ASCIICORR";
       p["CHANNEL"]=0;
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("CSV_CORR")) {
        p["FILETYPE"]="CSV_CORR";
        p["CSV_SEPARATOR"]=QString(",");
        p["CSV_COMMENT"]=QString("#");
        p["CSV_STARTSWITH"]=QString("");
        p["CSV_ENDSWITH"]=QString("");
        p["CSV_TIMEFACTOR"]=1.0;
        p["CSV_FIRSTLINE"]=1;
        p["CSV_MODE"]=0;

        dlgCSVParameters* csvDlg=new dlgCSVParameters(parentWidget, settings->getQSettings()->value("fcs/csv_mode", 0).toInt(),
                                                      settings->getQSettings()->value("fcs/csv_startswith", "").toString(),
                                                      settings->getQSettings()->value("fcs/csv_endswith", "").toString(),
                                                      settings->getQSettings()->value("fcs/csv_separator", ",").toString(),
                                                      settings->getQSettings()->value("fcs/csv_comment", "#").toString(),
                                                      settings->getQSettings()->value("fcs/csv_timefactor", 1.0).toDouble(),
                                                      settings->getQSettings()->value("fcs/csv_firstline", 1).toInt());
        loadWidgetGeometry(*settings->getQSettings(), csvDlg, QPoint(50,50), csvDlg->size(), QString("fcs/csv_dialog."));
        if (!filename.isEmpty() && QFile::exists(filename)) csvDlg->setFileContents(filename);
        if (csvDlg->exec()==QDialog::Accepted) {
            p["CSV_SEPARATOR"]=QString(csvDlg->get_column_separator());
            p["CSV_COMMENT"]=QString(csvDlg->get_comment_start());
            p["CSV_STARTSWITH"]=csvDlg->get_startswith();
            p["CSV_ENDSWITH"]=csvDlg->get_endswith();
            p["CSV_TIMEFACTOR"]=csvDlg->get_timefactor();
            p["CSV_FIRSTLINE"]=csvDlg->get_firstLine();
            p["CSV_MODE"]=csvDlg->get_mode();
            settings->getQSettings()->setValue("fcs/csv_separator", QString(csvDlg->get_column_separator()));
            settings->getQSettings()->setValue("fcs/csv_comment", QString(csvDlg->get_comment_start()));
            settings->getQSettings()->setValue("fcs/csv_startswith", QString(csvDlg->get_startswith()));
            settings->getQSettings()->setValue("fcs/csv_endswith", QString(csvDlg->get_endswith()));
            settings->getQSettings()->setValue("fcs/csv_timefactor", csvDlg->get_timefactor());
            settings->getQSettings()->setValue("fcs/csv_firstline", csvDlg->get_firstLine());
            settings->getQSettings()->setValue("fcs/csv_mode", csvDlg->get_mode());
            saveWidgetGeometry(*settings->getQSettings(), csvDlg, "fcs/csv_dialog.");
        } else {
            return;
        }
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("ISS_ALBA")) {
        p["FILETYPE"]="ISS_ALBA";
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("PICOQUANT_ASCII_FCS")) {
        p["FILETYPE"]="PICOQUANT_ASCII_FCS";
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("PICOQUANT_ASCII_FCS_W")) {
        p["FILETYPE"]="PICOQUANT_ASCII_FCS_W";
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("PICOQUANT_ASCII_COR")) {
        p["FILETYPE"]="PICOQUANT_ASCII_COR";
    } else if (currentFCSFileFormatFilter==getFileTypesFilterForID("DIFFUSION4_SIMRESULTS")) {
        p["FILETYPE"]="DIFFUSION4_SIMRESULTS";
        p["CSV_SEPARATOR"]=",";
        p["CSV_COMMENT"]="#";
    }
}

QFRDRFCSData::QFRDRFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
    correlationRuns=0;
    correlationN=0;
    correlationT=NULL;
    correlation=NULL;
    correlationErrors=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;

    rateRuns=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;
    rateChannels=0;

    autoCalcRateN=200;
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;

    leaveout.clear();
}

QFRDRFCSData::~QFRDRFCSData()
{
    if (correlationT) qfFree(correlationT);
    if (correlation) qfFree(correlation);
    if (correlationErrors) qfFree(correlationErrors);
    if (correlationMean) qfFree(correlationMean);
    if (correlationStdDev) qfFree(correlationStdDev);
    if (rateT) qfFree(rateT);
    if (rate) qfFree(rate);
    if (binnedRateT) qfFree(binnedRateT);
    if (binnedRate) qfFree(binnedRate);
    correlationRuns=0;
    correlationN=0;
    correlationT=NULL;
    correlation=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    correlationErrors=NULL;

    autoCalcRateN=-1;
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;

    rateRuns=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;
}

void QFRDRFCSData::resizeCorrelations(long long N, int runs) {
    //qDebug()<<"resizeCorrelations( N="<<N<<",  runs="<<runs<<")";
    if (correlationT) qfFree(correlationT);
    if (correlation) qfFree(correlation);
    if (correlationMean) qfFree(correlationMean);
    if (correlationStdDev) qfFree(correlationStdDev);
    if (correlationErrors) qfFree(correlationErrors);
    correlationRuns=0;
    correlationN=0;
    correlationT=NULL;
    correlation=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    correlationErrors=NULL;
    runsVisibleList.clear();
    for (int i=0; i<=N; i++) runsVisibleList.append(true);
    correlationN=N;
    correlationRuns=runs;
    correlationT=(double*)qfCalloc(correlationN, sizeof(double));
    correlationMean=(double*)qfCalloc(correlationN, sizeof(double));
    correlationStdDev=(double*)qfCalloc(correlationN, sizeof(double));
    correlation=(double*)qfCalloc(correlationN*correlationRuns, sizeof(double));
    correlationErrors=(double*)qfCalloc(correlationN*correlationRuns, sizeof(double));
    if (!correlationT || !correlation || !correlationMean || !correlationStdDev || !correlationErrors)
        setError(tr("Error while allocating memory for correlation function data!"));
    emitRawDataChanged();
}

void QFRDRFCSData::resizeRates(long long N, int runs, int channels) {
    //qDebug()<<"resizeRates( N="<<N<<",  runs="<<runs<<",  channels="<<channels<<")";
    if (rateT) qfFree(rateT);
    if (rate) qfFree(rate);
    rateRuns=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;
    rateN=N;
    rateRuns=runs;
    rateChannels=channels;
    if (N>0) {
        rateT=(double*)qfCalloc(rateN, sizeof(double));
        if (runs>0 && channels>0) {
            rate=(double*)qfCalloc(rateChannels*rateN*rateRuns, sizeof(double));
        }
        if (!rateT || !rate)
            setError(tr("Error while allocating memory for count rate data (N=%1, runs=%2, channels=%3)!").arg(N).arg(runs).arg(channels));
    }
    emitRawDataChanged();
    rateMean.clear();
    rateStdDev.clear();
    rateMin.clear();
    rateMax.clear();
}

void QFRDRFCSData::resizeBinnedRates(long long N) {
    //qDebug()<<"resizeBinnedRates( N="<<N<<"),  rateChannels="<<rateChannels<<",  rateRuns="<<rateRuns;
    if (binnedRateT) qfFree(binnedRateT);
    if (binnedRate) qfFree(binnedRate);
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;
    binnedRateN=N;
    if (N>0) {
        binnedRateT=(double*)qfCalloc(binnedRateN, sizeof(double));
        binnedRate=(double*)qfCalloc(rateChannels*binnedRateN*rateRuns, sizeof(double));
        if (!binnedRateT || !binnedRate)
            setError(tr("Error while allocating memory for binned count rate data!"));
        setIntProperty("BINNED_RATE_N", binnedRateN, false, false);
    }
    emitRawDataChanged();
}


void QFRDRFCSData::calcBinnedRate() {
    if (autoCalcRateN<=0) return;
    long long d=0;
    long N=0;
    /*d=ceil((double)rateN/(double)binnedRateN);
    N=ceil((double)rateN/(double)d);
    if (binnedRateN<=0 && autoCalcRateN>0) {*/
        d=(long long)ceil((double)rateN/(double)autoCalcRateN);
        N=(long long)ceil((double)rateN/(double)d);
        if ((double)rateN/(double)autoCalcRateN<1) N=rateN;
        resizeBinnedRates(N);
        //qDebug()<<"rate N="<<N<<"    binnedRateN="<<binnedRateN;
    //}
    if (binnedRateN>0) {
        //if (N!=binnedRateN) resizeBinnedRates(N);
        for (long run=0; run<rateRuns*rateChannels; run++) {
            for (long long i=0; i<binnedRateN; i++) {
                long long jmin=i*d;
                long long jmax=mmin(rateN, (i+1)*d)-1;
                double sum=0;
                for (long long j=jmin; j<=jmax; j++) {
                    sum=sum+rate[run*rateN+j];
                }
                binnedRate[run*binnedRateN+i]=sum/(double)(jmax-jmin+1);
                binnedRateT[i]=rateT[jmin];
            }
        }
    }
    emitRawDataChanged();
}

int QFRDRFCSData::leaveoutGetRunCount() const
{
    return getCorrelationRuns();
}

QFRawDataRecord::FileListEditOptions QFRDRFCSData::isFilesListEditable() const
{
    QString filetype=getProperty("FILETYPE", "unknown").toString().toUpper();
    if (filetype!="INTERNAL") {
        return QFRawDataRecord::FilesEditable | QFRawDataRecord::CustomFilesAddFunction;
    } else {
        return QFRawDataRecord::FilesNotEditable;
    }
}

bool QFRDRFCSData::selectNewFiles(QStringList &files, QStringList &types, QStringList &descriptions) const
{
    QString filter;
    QString filetype=getProperty("FILETYPE", "unknown").toString().toUpper();

    filter=getFileTypesFilterForID(filetype);
    if (filter.isEmpty()) filter=tr("All Files (*.*)");



    if (filetype!="INTERNAL") {
        files = qfGetOpenFileNamesSet("fcs/new_files/", NULL,
                              tr("Select FCS Data File(s) to Add/Import ..."),
                              ProgramOptions::getInstance()->getCurrentRawDataDir(),
                              filter);
        types.clear();
        descriptions.clear();
        return (files.size()>0);
    }
    return false;
}

void QFRDRFCSData::recalculateCorrelations() {
    if (correlationN<=0 || correlationRuns<=0 || !correlation || !correlationMean || !correlationStdDev) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return;
    }
    for (long long i=0; i<correlationN; i++) {
        double m=0.0;
        double s=0.0;
        double cnt=0;
        for (int run=0; run<correlationRuns; run++) {
            if (!leaveoutRun(run)) {
                double c=correlation[run*correlationN+i];
                m=m+c;
                s=s+c*c;
                cnt=cnt+1.0;
            }
        }
        if (cnt==0) correlationMean[i]=0;
        else correlationMean[i]=m/cnt;
        if (cnt<=1) correlationStdDev[i]=0;
        else correlationStdDev[i]=sqrt(s/cnt-m*m/cnt/cnt);
        if (!QFFloatIsOK(correlationStdDev[i])) correlationStdDev[i]=0;
        if (!QFFloatIsOK(correlationMean[i])) correlationMean[i]=0;
    }
    emitRawDataChanged();
}

void QFRDRFCSData::correctCorrelations()
{
    bool ok=false;
    double offset=getProperty("CF_CORRECT_OFFSET", 0.0).toDouble(&ok);
    if (ok && !qFuzzyIsNull(offset) && correlation) {
        for (int r=0; r<correlationN*correlationRuns; r++) {
            correlation[r]=correlation[r]-offset;
        }
        recalculateCorrelations();
    }
}

double QFRDRFCSData::calcRateMean(int run, int channel) const {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return 0.0;
    }
    double m=0.0;
    if (run<rateRuns) {
        for (long long i=0; i<rateN; i++) {
            m=m+rate[channel*rateN*rateRuns + run*rateN+i];
        }
    }
    double me= m/(double)rateN;
    if (!QFFloatIsOK(me)) me=0;
    rateMean[channel*rateRuns+run]=me;
    return me;
}



double QFRDRFCSData::getRateMean(int run, int channel) const {
    if (rateN<=0 || !rate) {
        return 0.0;
    }
    if (run<0) {
        double cnt=0;
        double sum=0;
        for (int i=0; i<rateRuns; i++) {
            double r=getRateMean(i, channel);
            if (r>0) {
                sum=sum+r;
                cnt++;
            }
        }
        double r= sum/cnt;
        if (QFFloatIsOK(r)) return r; else return 0;
    }
    if (!rateMean.contains(channel*rateRuns+run)) {
        rateMean[channel*rateRuns+run]=calcRateMean(run, channel);
    }
    double r=rateMean[channel*rateRuns+run];
    if (QFFloatIsOK(r)) return r; else return 0;
}

double QFRDRFCSData::getRateStdDev(int run, int channel) const {
   if (rateN<0 || !rate) {
        return 0.0;
    }
    if (run<0) {
        double cnt=0;
        double sum=0;
        for (int i=0; i<rateRuns; i++) {
            double r=getRateStdDev(i, channel);
            if (r>0) {
                sum=sum+r;
                cnt++;
            }
        }
        double r=sum/cnt;
        if (QFFloatIsOK(r)) return r; else return 0;
    }
    if (!rateStdDev.contains(channel*rateRuns+run)) {
        rateStdDev[channel*rateRuns+run]=calcRateStdDev(run, channel);
    }
    double r=rateStdDev[channel*rateRuns+run];
    if (QFFloatIsOK(r)) return r; else return 0;
}

double QFRDRFCSData::calcRateStdDev(int run, int channel) const {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return 0.0;
    }
    double m=0.0;
    double s=0.0;
    if (run<rateRuns && run>=0) {
        for (long long i=0; i<rateN; i++) {
            double v=rate[channel*rateN*rateRuns + run*rateN+i];
            m=m+v;
            s=s+v*v         ;
        }
    }
    double sd=sqrt(s/(double)rateN-m*m/(double)rateN/(double)rateN);
    if (!QFFloatIsOK(sd)) sd=0;
    rateStdDev[channel*rateRuns+run]=sd;
    return sd;
}

void QFRDRFCSData::calcRateMinMax(int run, double& min, double& max, int channel) const {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return;
    }
    min=max=0;
    if (run<rateRuns) {
        min=rate[run*rateN];
        max=min;
        for (long long i=0; i<rateN; i++) {
            double r=rate[channel*rateN*rateRuns + run*rateN+i];
            if (r>max) max=r;
            if (r<min) min=r;
        }
    }

    rateMin[channel*rateRuns+run]=min;
    rateMax[channel*rateRuns+run]=max;
}

void QFRDRFCSData::getRateMinMax(int run, double &min, double &max, int channel) const {
    min=0;
    max=0;
    if (!(rateMin.contains(channel*rateRuns+run) && rateMax.contains(channel*rateRuns+run))) {
        calcRateMinMax(run, min, max, channel);
    }
    min=rateMin.value(channel*rateRuns+run, 0);
    max=rateMax.value(channel*rateRuns+run, 0);
}


void QFRDRFCSData::exportData(const QString& format, const QString& filename)const  {
    if (format.toUpper()=="QF3ASCIICORR") {
        QF3CorrelationDataFormatTool tool;
        tool.channels=rateChannels;
        tool.correlationTypes=1;
        tool.filename=filename;
        tool.folder=getFolder();
        tool.group=getGroupName();
        tool.input_file=getFileName(0);
        tool.role=getRole();
        tool.runs=correlationRuns;
        tool.rateRuns=rateRuns;
        for (unsigned int i=0; i<getPropertyCount(); i++) {
            QString id=getPropertyName(i);
            if (isPropertyVisible(id)) {
                tool.properties.insert(id, getQFProperty(id));
            }
        }
        tool.reserveCorrelations();
        tool.reserveRate();
        for (int i=0; i<correlationN; i++) {
            tool.taus.append(correlationT[i]);
        }
        for (int r=0; r<correlationRuns; r++) {
            double* rd=getCorrelationRun(r);
            for (int i=0; i<correlationN; i++) {
                tool.addCorrelationEntry(rd[i], r);
            }
        }
        //qDebug()<<tool.correlations;
        //qDebug()<<tool.correlations[0];
        //qDebug()<<tool.correlations[0].operator [](0);
        for (int i=0; i<rateN; i++) {
            tool.times.append(rateT[i]);
        }
        for (int c=0; c<rateChannels; c++) {
            for (int r=0; r<rateRuns; r++) {
                double* rd=getRateRun(r, c);
                for (int i=0; i<rateN; i++) {
                    tool.addCountrateEntry(rd[i], r, c);
                }
            }
        }

        tool.saveFile(filename);
    } else {
        QFDataExportTool data;
        bool corr=false;
        bool count=false;
        QString formatout="";
        QStringList formats=QFDataExportHandler::getFormats();
        if (format.toUpper().startsWith("CORRCOUNTS_")) {
            corr=true;
            count=true;
            formatout=format.right(format.size()-QString("CORRCOUNTS_").size());
        } else if (format.toUpper().startsWith("CORR_")) {
            corr=true;
            count=false;
            formatout=format.right(format.size()-QString("CORR_").size());
        } else if (format.toUpper().startsWith("COUNTS_")) {
            corr=false;
            count=true;
            formatout=format.right(format.size()-QString("COUNTS_").size());
        }
        int formatIdx=formats.indexOf(formatout);
        if (formatIdx>=0) {
            int col=0;
            if (corr) {
                data.setColTitle(col,tr("lag time tau [s]"));
                for (int i=0; i<correlationN; i++) {
                    data.set(col, i, correlationT[i]);
                }
                col++;
                for (int r=0; r<correlationRuns; r++) {
                    data.setColTitle(col,tr("correlation, run %1").arg(r));
                    double* rd=getCorrelationRun(r);
                    for (int i=0; i<correlationN; i++) {
                        data.set(col, i, rd[i]);
                    }
                    col++;
                }
                if (correlationRuns>1 || correlationRuns<=0) {
                    data.setColTitle(col,tr("correlation, average"));
                    data.setColTitle(col+1,tr("correlation, stddev"));
                    double* rd=getCorrelationMean();
                    double* re=getCorrelationStdDev();
                    for (int i=0; i<correlationN; i++) {
                        data.set(col, i, rd[i]);
                        data.set(col+1, i, re[i]);
                    }
                    col+=2;
                }
            }
            if (count) {
                data.setColTitle(col,tr("time [s]"));
                for (int i=0; i<rateN; i++) {
                    data.set(col, i, rateT[i]);
                }
                col++;
                for (int ch=0; ch<rateChannels; ch++) {
                    for (int r=0; r<rateRuns; r++) {
                        data.setColTitle(col,tr("countrate, channel %2 run %1").arg(r).arg(ch));
                        double* rd=getRateRun(r, ch);
                        for (int i=0; i<rateN; i++) {
                            data.set(col, i, rd[i]);
                        }
                        col++;
                    }
                }
            }

            data.save(filename, formatIdx);
        }
    }
}

QStringList QFRDRFCSData::getExportFiletypes() const
{
    QStringList sl;
    sl<<"QF3ASCIICORR";
    QStringList formats=QFDataExportHandler::getFormats();
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("CORRCOUNTS_%1").arg(formats[i]);
    }
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("CORR_%1").arg(formats[i]);
    }
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("COUNTS_%1").arg(formats[i]);
    }
    return sl;
}

QString QFRDRFCSData::getExportDialogFiletypes() const
{
    QStringList sl;
    sl<<tr("QuickFit 3.0 ASCII Correlation Data (*.qf3acorr)");
    QStringList formats=QFDataExportHandler::getFormats();
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("correlations+countrate: %1").arg(formats[i]);
    }
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("correlations: %1").arg(formats[i]);
    }
    for (int i=0; i<formats.size(); i++) {
        sl<<QString("countrates: COUNTS_%1").arg(formats[i]);
    }
    return sl.join(";;");

}

double *QFRDRFCSData::getCorrelationRun(int run) const
{
    if (run==-1) return getCorrelationMean();
    if (run>=0 && run<correlationRuns) return &(correlation[run*correlationN]);
    return NULL;
}

double *QFRDRFCSData::getCorrelationRunError(int run) const
{
    if (run==-1) return getCorrelationStdDev();
    if (run>=0 && run<correlationRuns) return &(correlationErrors[run*correlationN]);
    return NULL;
}


void QFRDRFCSData::intWriteData(QXmlStreamWriter& w) const {
    if (leaveout.size()>0) {
        QString l="";
        for (int i=0; i<leaveout.size(); i++) {
            if (!l.isEmpty()) l=l+",";
            l=l+QString::number(leaveout[i]);
        }
        w.writeStartElement("leaveout");
        w.writeAttribute("list", l);
        w.writeEndElement();
    }
    QString filetype=getProperty("FILETYPE", "unknown").toString();
    if (filetype.toUpper()=="INTERNAL") {
        saveInternal(w);
    }
}

void QFRDRFCSData::intReadData(QDomElement* e) {
    leaveout.clear();
    if (e) {
        QDomElement te=e->firstChildElement("leaveout");
        QString l=te.attribute("list");
        QStringList li=l.split(",");
        for (int i=0; i<li.size(); i++) {
            bool ok=false;
            int lo=li[i].toUInt(&ok);
            if (ok) leaveout.append(lo);
        }
    }
    if (!propertyExists("SAME_COUNTRATE_FOR_ALL")) {
        setQFProperty("SAME_COUNTRATE_FOR_ALL", false, true, true);
    }
    QString filetype=getProperty("FILETYPE", "unknown").toString().toUpper();
    if (filetype.toUpper()=="INTERNAL") {
        loadInternal(e);
    } else {
        reloadFromFiles();
    }
    correctCorrelations();



    //std::cout<<"intReadData ended\n";
}

struct CSVDATA {
    datatable2* tab;
    QString filename;
};

bool QFRDRFCSData::loadCountRatesFromCSV(QStringList filenames, int rateChannels) {
    //qDebug()<<"loadCountRatesFromCSV"<<filenames<<rateChannels;
    QList<CSVDATA> data;
    char separatorchar=',';
    char commentchar='#';
    std::string d=getProperty("CSV_SEPARATOR", ",").toString().toStdString();
    std::string startswith=getProperty("CSV_STARTSWITH", "").toString().toStdString();
    std::string endswith=getProperty("CSV_ENDSWITH", "").toString().toStdString();
    double timefactor=getProperty("CSV_TIMEFACTOR", 1.0).toDouble();
    double ratefactor=getProperty("CSV_RATEFACTOR", 1.0).toDouble();
    int firstline=getProperty("CSV_FIRSTLINE", 1).toInt();
    if (d.size()>0) separatorchar=d[0];
    d=getProperty("CSV_COMMENT", "#").toString().toStdString();
    if (d.size()>0) commentchar=d[0];

    long long rrateN=0;
    long long rruns=0;
    bool error=false;

    for (int ii=0; ii<filenames.size(); ii++) {
        if (!QFile::exists(filenames[ii])) {
            QString msg;
            setError(msg=tr("data file '%1' does not exist!\n").arg(filenames[ii]));
            QFPluginServices::getInstance()->log_warning(msg);
            error=true;
            break;
        }
        CSVDATA d;
        d.filename=filenames[ii];

        try {
            d.tab=new datatable2();
            d.tab->load_csv(d.filename.toStdString(), separatorchar, commentchar, startswith, endswith, firstline);        // load some csv file
            long long lines=d.tab->get_line_count();
            long long columns=d.tab->get_column_count();
            rruns=rruns+columns-1;
            rrateN=qMax(lines, rrateN);
        } catch(datatable2_exception& e) {   // error handling with exceptions
            setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(d.filename).arg(QString(e.get_message().c_str())));
            error=true;
            break;
        }
        data.append(d);
        if (error) break;
    }
    if (!error) {
        resizeRates(rrateN, rruns/rateChannels, rateChannels);
        int run0=0;
        for (int ii=0; ii<filenames.size(); ii++) {
            try {
                long long lines=data[ii].tab->get_line_count();
                long long columns=data[ii].tab->get_column_count();
                for (long long l=0; l<lines; l++) {
                    if (ii==0) rateT[l]=data[ii].tab->get(0, l)*timefactor;
                    else if (rateT[l]!=data[ii].tab->get(0, l)*timefactor) {
                        setError(tr("Error while reading countrates from CSV file '%1': time does not match time in file '%2'").arg(data[ii].filename).arg(data[0].filename));
                        error=true;
                        break;
                    }
                    for (int c=1; c<columns; c++) {
                        rate[(run0+c-1)*rateN+l]=data[ii].tab->get(c, l)*ratefactor;
                    }
                }
                run0=run0+columns-1;
            } catch(datatable2_exception& e) {   // error handling with exceptions
                setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(data[ii].filename).arg(QString(e.get_message().c_str())));
                error=true;
                break;
            }
            if (error) break;
        }
    }

    for (int ii=0; ii<data.size(); ii++) {
        delete data[ii].tab;
    }
    data.clear();

    if (!error) {
        emitRawDataChanged();
        if (rateN<1000) autoCalcRateN=rateN;
        calcBinnedRate();
        return true;
    } else {
        return false;
    }
}






bool QFRDRFCSData::loadCorrelationCurvesFromCSV(QStringList filenames) {
    QList<CSVDATA> data;
    char separatorchar=',';
    char commentchar='#';
    std::string d=getProperty("CSV_SEPARATOR", ",").toString().toStdString();
    std::string startswith=getProperty("CSV_STARTSWITH", "").toString().toStdString();
    std::string endswith=getProperty("CSV_ENDSWITH", "").toString().toStdString();
    double timefactor=getProperty("CSV_TIMEFACTOR", 1.0).toDouble();
    int firstline=getProperty("CSV_FIRSTLINE", 1).toInt();
    int mode=getProperty("CSV_MODE", 0).toInt();
    int column=getProperty("CSV_COLUMN", 1).toInt();
    if (d.size()>0) separatorchar=d[0];
    d=getProperty("CSV_COMMENT", "#").toString().toStdString();
    if (d.size()>0) commentchar=d[0];

    long long ccorrN=0;
    long long rruns=0;
    bool error=false;

    for (int ii=0; ii<filenames.size(); ii++) {
        CSVDATA d;
        d.filename=filenames[ii];
        if (!QFile::exists(filenames[ii])) {
            QString msg;
            setError(msg=tr("data file '%1' does not exist!\n").arg(filenames[ii]));
            QFPluginServices::getInstance()->log_warning(msg);
            error=true;
            break;
        }
        try {
            d.tab=new datatable2();
            d.tab->load_csv(d.filename.toStdString(), separatorchar, commentchar, startswith, endswith, firstline);        // load some csv file
            long long lines=d.tab->get_line_count();
            long long columns=d.tab->get_column_count();
            //qDebug()<<"lines="<<lines<<"   columns="<<columns<<QString::number(separatorchar);
            QFPluginServices::getInstance()->log_text(tr("  read CSV file '%1' as FCS record.\n  Found %2 lines and %3 columns.\n").arg(d.filename).arg(lines).arg(columns));
            if (mode==0) { // tau, corr, corr, ...
                rruns=rruns+columns-1;
            } else if (mode==1) { // tau, corr, error, corr, error, ...
                rruns=rruns+(columns-1)/2;
            } else if (mode==2) { // tau, corr, corr, ... -> select only one column
                rruns=1;
            }
            ccorrN=qMax(lines, ccorrN);
        } catch(datatable2_exception& e) {   // error handling with exceptions
            setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(d.filename).arg(QString(e.get_message().c_str())));
            error=true;
            break;
        }
        data.append(d);
        if (error) break;
    }
    if (!error) {
        resizeCorrelations(ccorrN, rruns);
        int run0=0;
        for (int ii=0; ii<filenames.size(); ii++) {
            try {
                long long lines=data[ii].tab->get_line_count();
                long long columns=data[ii].tab->get_column_count();
                int runincrement=(columns-1);

                for (long long l=0; l<lines; l++) {
                    if (ii==0) correlationT[l]=data[ii].tab->get(0, l)*timefactor;
                    else if (correlationT[l]!=data[ii].tab->get(0, l)*timefactor) {
                        setError(tr("Error while reading correlation fucntions from CSV file '%1': time does not match time in file '%2'").arg(data[ii].filename).arg(data[0].filename));
                        error=true;
                        break;
                    }
                    if (mode==0) { // tau, corr, corr, ...
                        for (int c=1; c<columns; c++) {
                            correlation[(run0+c-1)*correlationN+l]=data[ii].tab->get(c, l);
                        }
                        runincrement=(columns-1);
                    } else if (mode==1) { // tau, corr, error, corr, error, ...
                        for (int c=1; c<columns; c+=2) {
                            if (c<columns) {
                                correlation[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c, l);
                            }
                            if (c+1<columns) {
                                correlationErrors[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c+1, l);
                            } else {
                                correlationErrors[(run0+(c-1)/2)*correlationN+l]=0;
                            }
                        }
                        runincrement=(columns-1)/2;
                    } else if (mode==2) { // tau, corr, corr, ... -> select only one column
                        if (column<(long long)data[ii].tab->get_column_count()) correlation[(run0)*correlationN+l]=data[ii].tab->get(column, l);
                        runincrement=1;
                    }
                }
                run0=run0+runincrement;

            } catch(datatable2_exception& e) {   // error handling with exceptions
                setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(data[ii].filename).arg(QString(e.get_message().c_str())));
                error=true;
                break;
            }
            if (error) break;
        }
    }

    for (int ii=0; ii<data.size(); ii++) {
        delete data[ii].tab;
    }
    data.clear();

    if (!error) {
        recalculateCorrelations();
        emitRawDataChanged();
        return true;
    } else {
        return false;
    }
}





struct  ALBADATA {
    QString filename;
    QList<double> tdata, cdata;
};

bool QFRDRFCSData::loadCorrelationCurvesFromALBA(QStringList filenames) {
    QList<ALBADATA> data;
    for (int ii=0; ii<filenames.size(); ii++) {
        if (!QFile::exists(filenames[ii])) {
            QString msg;
            setError(msg=tr("data file '%1' does not exist!\n").arg(filenames[ii]));
            QFPluginServices::getInstance()->log_warning(msg);
            return false;
        }
        ALBADATA d;
        d.filename=filenames[ii];
        data.append(d);

        QFile f(data[ii].filename);
        if (f.open(QIODevice::ReadOnly)) {
            QTextStream stream(&f);
            QString line;
            // search for [Data] in file
            do {
                line=stream.readLine().toLower();
            } while (!stream.atEnd() && !line.isNull() && !line.contains("[data]"));
            if (line.contains("[data]")) {
                QList<double> tdata, cdata;
                do {
                    line=stream.readLine();
                    QStringList sl=line.split(",");
                    if (sl.size()>=2) {
                        tdata.append(sl[0].toDouble());
                        cdata.append(sl[1].toDouble());
                        //std::cout<<tdata[tdata.size()-1]<<", "<<cdata[cdata.size()-1]<<std::endl;
                    }
                } while (!line.isNull());
                data[ii].tdata=tdata;
                data[ii].cdata=cdata;
            } else {
                setError(tr("ISS ALBA file '%1' does not contain a [data] section").arg(filenames[ii]));
                return false;
            }
        } else {
            setError(tr("could not open file '%1'").arg(filenames[ii]));
            return false;
        }
    }

    int ccorrN=0;
    int rrateN=0;
    int rruns=0;
    for (int ii=0; ii<data.size(); ii++) {
        ccorrN=ccorrN+qMin(data[ii].tdata.size(), data[ii].cdata.size());
        rruns++;
    }
    resizeCorrelations(ccorrN, rruns);
    int run0=0;
    for (int ii=0; ii<data.size(); ii++) {
        for (int i=0; i<qMin(data[ii].tdata.size(), data[ii].cdata.size()); i++) {
            if (ii==0) {
                correlationT[i]=data[ii].tdata[i];
            } else {
                if (correlationT[i]!=data[ii].tdata[i]) {
                    setError(tr("lag time in file '%2' and file '%1' are unequal (expected %3, but read %4).").arg(data[0].filename).arg(data[ii].filename).arg(correlationT[i]).arg(data[ii].tdata[i]));
                    return false;
                }
            }
            correlation[run0*correlationN+i]=data[ii].cdata[i];

        }
        run0=run0+1;
    }

    recalculateCorrelations();
    emitRawDataChanged();
    return true;
}


struct ALVFILEDATA  {
    FILE* alv_file;
    QString filename;
    int runs;
    bool isDual;

    bool readingHeader;
    bool findIdentifier;
    ALV_TOKEN token;
    QVector<QVector<double> > dat_corr;
    QVector<QVector<double> > dat_rate;
};



bool QFRDRFCSData::loadFromALV5000Files(QStringList filenames) {

    try {
        double timefactor=getProperty("ALV_TIMEFACTOR", 1.0).toDouble();
        ALVFILEDATA init;
        init.runs=0;
        init.alv_file=NULL;
        init.filename="";
        init.isDual=false;
        init.readingHeader=true;
        init.findIdentifier=true;
        QList<ALVFILEDATA> data;
        bool error=false;
        int channel=0;
        for (int i=0; i<filenames.size(); i++) {
            if (!QFile::exists(filenames[i])) {
                QString msg;
                setError(msg=tr("data file '%1' does not exist!\n").arg(filenames[i]));
                QFPluginServices::getInstance()->log_warning(msg);
                error=true;
                break;
            }
            data.append(init);
            data[i].filename=filenames[i];

            data[i].alv_file=fopen(data[i].filename.toLatin1().data(), "r");

            if (!data[i].alv_file) {
                setError(tr("could not open file '%1'").arg(data[i].filename));
                error=true;
                break;
            }
            if (ferror(data[i].alv_file)) {
                setError(tr("%2").arg(data[i].filename).arg(strerror(errno)));
                error=true;
                break;
            }
            data[i].token=ALV_getToken(data[i].alv_file, data[i].readingHeader);
            if (error) break;


            // first we parse the header (until we find the first quoted string)
            // the data from the header is interpeted and the resulting quickfitDataStoreItem are
            // beeing created.
            while (data[i].token.type!=ALV_EOF && data[i].readingHeader) {
                if (data[i].findIdentifier) {
                    if (data[i].token.type==ALV_NAME) {
                        if (data[i].token.value.contains("ALV-5000", Qt::CaseInsensitive)) {
                            setQFProperty("ALV_TYPE", data[i].token.value, false, true);
                            data[i].findIdentifier=false;
                        } else if (data[i].token.type!=ALV_LINEBREAK && data[i].token.type!=ALV_EOF) {
                            setError(tr("did not find file header").arg(data[i].filename));
                            error =true;
                            break;
                        }
                    }
                } else {
                    if (data[i].token.type==ALV_NAME) { // here we parse "<name> : <value>"
                        QString name=data[i].token.value; // store <name>
                        QString value;
                        // get next token which has to be a colon':'
                        data[i].token=ALV_getToken(data[i].alv_file, data[i].readingHeader);
                        if (data[i].token.type!=ALV_COLON) {
                            if (name.toLower().startsWith("alv") && name.toLower().endsWith("data")) {
                                int pnum=1;
                                while (propertyExists(QString("ALV_TYPE%1").arg(pnum))) pnum++;
                                setQFProperty(QString("ALV_TYPE%1").arg(pnum), data[i].token.value, false, true);
                            } else {
                                setError(tr("colon ':' expected, but found '%2'").arg(data[i].filename).arg(data[i].token.value));
                                error=true;
                                break;
                            }
                        } else {
                            // get next token which has to be a value or a quoted string or a linebreak
                            data[i].token=ALV_getToken(data[i].alv_file, data[i].readingHeader);
                            if (data[i].token.type==ALV_QUOTED) {
                                value=data[i].token.value;
                            } else if (data[i].token.type==ALV_VALUE) {
                                value=data[i].token.value;
                            } else if (data[i].token.type!=ALV_LINEBREAK) {
                                setError(tr("linebreak, number or quoted string expected").arg(data[i].filename));
                                error=true;
                                break;
                            }
                            // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                            if (name.compare("Date",  Qt::CaseInsensitive)==0) {
                                QDate date=ALV_toDate(value);
                                if (date.year()<1950) date.setDate(date.year()+100, date.month(), date.day());
                                if (i==0) setQFProperty("DATE", date, false, true);
                                else if (date!=getProperty("DATE").toDate()) setQFProperty(QString("DATE%1").arg(i+1), date, false, true);
                            } else if (name.compare("Time",  Qt::CaseInsensitive)==0) {
                                QTime date=ALV_toTime(value);
                                if (i==0) setQFProperty("TIME", date, false, true);
                                else if (date!=getProperty("TIME").toTime()) setQFProperty(QString("TIME%1").arg(i+1), date, false, true);
                            } else if (name.contains("Duration",  Qt::CaseInsensitive)) {
                                QString propN="DURATION [s]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.compare("Runs",  Qt::CaseInsensitive)==0) {
                                data[i].runs=(int)round(data[i].token.doubleValue);
                            } else if (name.contains("Temperature",  Qt::CaseInsensitive)) {
                                QString propN="TEMPERATURE [K]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Viscosity",  Qt::CaseInsensitive)) {
                                QString propN="VISCOSITY [cp]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Refractive Index",  Qt::CaseInsensitive)) {
                                QString propN="REFRACTIVE_INDEX";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Wavelength",  Qt::CaseInsensitive)) {
                                QString propN="WAVELENGTH [nm]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Angle",  Qt::CaseInsensitive)) {
                                QString propN=QLatin1String("ANGLE [\xB0]");
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("MeanCR",  Qt::CaseInsensitive)) {
                                // ignore this property, as it is calculated by this class
                            } else if (name.contains("SampMemo",  Qt::CaseInsensitive)) {
                                QString text=getProperty("SAMPLE_MEMO", "").toString();
                                if (!value.isEmpty()) text=text+tr("\nfile %1\n").arg(i+1)+value;
                                setQFProperty("SAMPLE_MEMO", text, false, true);
                            } else if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                                QString propN="MODE";
                                QString propVS=value;
                                if (i==0) setQFProperty(propN, propVS, false, true);
                                else if (propVS!=getProperty(propN).toString()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propVS).arg(propN));

                                propN="CROSS_CORRELATION";
                                bool propV=(bool)value.contains("CROSS", Qt::CaseInsensitive);
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));

                                propN="DUAL_CHANNEL";
                                propV=value.contains("DUAL", Qt::CaseInsensitive);
                                data[i].isDual=propV;
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));


                            } else {
                                QString propN=name;
                                QString propVS=value;
                                if (i==0) setQFProperty(propN, propVS, false, true);
                                else if (propVS!=getProperty(propN).toString()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propVS).arg(propN));
                            }
                        }

                    } else if (data[i].token.type==ALV_QUOTED) {
                        // we stop reading the header when we meet the first quoted string token
                        // this is possible as every line of the header begins with an unquoted name
                        // token followed by a colon and number/quoted or alineabreak (identifier line!)
                        data[i].readingHeader=false;
                    }
                }
                if (data[i].readingHeader) data[i].token=ALV_getToken(data[i].alv_file, data[i].readingHeader);
            }
            if (data[i].isDual) {
                if (propertyExists("CHANNEL")) channel=getProperty("CHANNEL", 0).toInt();
            }






            if (!error) {
                while (data[i].token.type!=ALV_EOF) {
                    bool getNew=true;
                    if (data[i].token.type==ALV_QUOTED && data[i].token.value.contains("correlation", Qt::CaseInsensitive)) {
                        getNew=false;
                        QVector<QVector<double> > dat;
                        data[i].token=ALV_readNumberMatrix(data[i].alv_file, &(dat));
                        data[i].dat_corr=dat;
                        //qDebug()<<"  parsing correlation section ... "<<data[i].dat_corr.size()<<data[i].dat_corr[0].size()<<"\n";
                    }
                    if (data[i].token.type==ALV_QUOTED && data[i].token.value.contains("count", Qt::CaseInsensitive)) {
                        getNew=false;
                        QVector<QVector<double> > dat;
                        data[i].token=ALV_readNumberMatrix(data[i].alv_file, &(dat));
                        data[i].dat_rate=dat;
                        //qDebug()<<"  parsing rate section ... "<<data[i].dat_rate.size()<<data[i].dat_rate[0].size()<<"\n";

                    }
                    if (getNew) data[i].token=ALV_getToken(data[i].alv_file, false);
                }
            }
            if (error) break;
        }


        if (!error) {
            int runs=0;
            int ccorrN=0;
            int rrateN=0;
            for (int i=0; i<filenames.size(); i++) {
                runs+=data[i].runs;
                if (data[i].dat_corr.size()>ccorrN) ccorrN=data[i].dat_corr.size();
                if (data[i].dat_rate.size()>rrateN) rrateN=data[i].dat_rate.size();
            }

            resizeCorrelations(ccorrN, runs);
            if (getProperty("CROSS_CORRELATION", false).toBool()||getProperty("DUAL_CHANNEL", false).toBool()) resizeRates(rrateN, runs, 2);
            else resizeRates(rrateN, runs, 1);
            int run0=0;
            for (int ii=0; ii<data.size(); ii++) {
                //qDebug()<<ii<<data[ii].dat_corr.size();
                for (long long  i=0; i<data[ii].dat_corr.size(); i++) {
                    //qDebug()<<i<<"/"<<data[ii].dat_corr.size()<<":   "<<error;
                    QVector<double> d=data[ii].dat_corr[i];
                    if (d.size()<=0) {
                        setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[ii].filename));
                        error=true;
                        //qDebug()<<"ERROR 1 !";
                        break;
                    }
                    if (ii==0) {
                        correlationT[i]=d[0]*timefactor/1000.0;
                    } else {
                        if (correlationT[i]!=d[0]*timefactor/1000.0) {
                            setError(tr("lag time in file '%2' and file '%1' are unequal (expected %3, but read %4).").arg(data[0].filename).arg(data[ii].filename).arg(correlationT[i]).arg(d[0]/1000.0));
                            error=true;
                            //qDebug()<<"ERROR 2 !";
                            break;
                        }
                    }
                    //qDebug()<<correlationT[i];




                    if (data[ii].runs==1) {
                        // one run => there is no average in the file!
                        if (d.size()<=1+channel)  {
                            setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[0].filename));
                            return false;
                        }
                        correlation[run0*correlationN+i]=d[1+channel];
                    } else if (data[ii].runs>1) {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel*(1+data[ii].runs)+data[ii].runs) {
                            setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[0].filename));
                            error=true;
                            //qDebug()<<"ERROR 3 !   channel="<<channel<<"   runs="<<data[ii].runs<<"    s.size="<<d.size()<<"   comape="<<1+channel*(1+data[ii].runs)+data[ii].runs;
                            break;
                        }
                        for (long j=0; j<data[ii].runs; j++) {
                            correlation[(run0+j)*correlationN+i]=d[1+channel*(1+data[ii].runs)+(1+j)];
                        }
                    }
                    //qDebug()<<correlationT[i]<<correlation[run0*correlationN+i];
                }

                if (error) break;

                for (long long i=0; i<data[ii].dat_rate.size(); i++) {
                    QVector<double>& d=data[ii].dat_rate[i];
                    if (d.size()<1) {
                        setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                        error=true;
                        break;
                    }
                    if (ii==0) {
                        rateT[i]=d[0];
                    } else {
                        if (rateT[i]!=d[0]) {
                            setError(tr("countrate timepoint in file '%2' and file '%1' are unequal (expected %3, but read %4).").arg(data[0].filename).arg(data[ii].filename).arg(rateT[i]).arg(d[0]));
                            error=true;
                            break;
                        }
                    }



                    if (rateChannels<=1) {
                        if (data[ii].runs==1) {
                            // one run => there is no average in the file!
                            if (d.size()<=1+channel)  {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            rate[run0*rateN+i]=d[1+channel];
                        } else if (data[ii].runs>1) {
                            // multiple runs -> average is in file -> ignore average column
                            if (d.size()<=1+channel) {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            rate[(data[ii].runs+run0-1)*rateN+i]=d[1+channel];
                            if (getProperty("SAME_COUNTRATE_FOR_ALL", false).toBool()) {
                                for (int crr=0; crr<data[ii].runs; crr++) {
                                    rate[(crr+run0)*rateN+i]=d[1+channel];
                                }
                            }
                        }
                    } else {
                        if (runs==1) {
                            // one run => there is no average in the file!
                            if (d.size()<=1+channel)  {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            /*if (data[ii].isDual && channel==1) {
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+run0*rateN+i]=d[1+rateChannels-1-c];
                                }
                            } else {*/
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+run0*rateN+i]=d.value(1+c,0);
                                }
                            //}
                            if (getRateChannelsSwapped()) {
                                //qDebug()<<"swapping in "<<getName();
                                qSwap(rate[0*rateN*rateRuns+run0*rateN+i], rate[1*rateN*rateRuns+run0*rateN+i]);
                            }

                        } else if (runs>1) {
                            // multiple runs -> average is in file -> ignore average column
                            if (d.size()<=1+channel) {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            /*if (data[ii].isDual&&channel==1) {
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+rateChannels-1-c];
                                }
                            } else {*/
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d.value(1+c,0);
                                }
                            //}
                            if (getProperty("SAME_COUNTRATE_FOR_ALL", false).toBool()) {
                                for (int c=0; c<rateChannels; c++) {
                                    for (int crr=0; crr<data[ii].runs; crr++) {
                                        rate[c*rateN*rateRuns+(run0+crr)*rateN+i]=d.value(1+c,0);
                                    }
                                }
                            }
                            if (getRateChannelsSwapped()) {
                                //qDebug()<<"swapping in "<<getName();
                                for (int crr=0; crr<data[ii].runs; crr++) {
                                    qSwap(rate[0*rateN*rateRuns+(run0+crr)*rateN+i], rate[1*rateN*rateRuns+(run0+crr)*rateN+i]);
                                }
                            }
                        }
                    }



                    /*if (rateChannels<=1) {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel) {
                            setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[ii].filename));
                            error=true;
                            break;
                        }
                        rate[(run0+runs-1)*rateN+i]=d[1+channel];
                    } else {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel) {
                            setError(tr("too few columns in line %1 of rate block.").arg(i));
                            error=true;
                            break;
                        }
                        if (data[ii].isDual) {
                            for (int c=0; c<rateChannels; c++) {
                                rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+rateChannels-1-c];
                            }
                        } else {
                            for (int c=0; c<rateChannels; c++) {
                                rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+c];
                            }
                        }
                    }*/
                }
                if (error) break;
                run0=run0+data[ii].runs;
            }
        }



        //std::cout<<"parsing data section ... runs="<<runs<<"  channel="<<channel<<std::endl;
        for (int i=0; i<data.size(); i++) {
            if (data[i].alv_file) fclose(data[i].alv_file);
            data[i].alv_file=NULL;
        }

       // std::cout<<"recalc correlations ..."<<std::endl;
        if (!error) {
            recalculateCorrelations();

           // std::cout<<"calc binned rates ..."<<std::endl;
            autoCalcRateN=rateN;
            calcBinnedRate();
            //std::cout<<"DONE !!!"<<std::endl;
            emitRawDataChanged();
            return true;
        } else {
            return false;
        }
    } catch (std::exception& e) {
        setError(tr("error while reading file '1':\n   %2").arg(filenames.value(0, "???")).arg(e.what()));
        return false;
    }


}









struct ALV6FILEDATA  {
    FILE* alv_file;
    QString filename;
    int runs;
    bool isDual;

    bool readingHeader;
    bool findIdentifier;
    ALV6_TOKEN token;
    QVector<QVector<double> > dat_corr;
    QVector<QVector<double> > dat_rate;
};

bool QFRDRFCSData::loadFromALV6000Files(QStringList filenames) {
    try {
        double timefactor=getProperty("ALV_TIMEFACTOR", 1.0).toDouble();
        ALV6FILEDATA init;
        init.runs=0;
        init.alv_file=NULL;
        init.filename="";
        init.isDual=false;
        init.readingHeader=true;
        init.findIdentifier=true;
        QList<ALV6FILEDATA> data;
        bool error=false;
        int channel=0;
        for (int i=0; i<filenames.size(); i++) {
            if (!QFile::exists(filenames[i])) {
                QString msg;
                setError(msg=tr("data file '%1' does not exist!\n").arg(filenames[i]));
                QFPluginServices::getInstance()->log_warning(msg);
                error=true;
                break;
            }
            data.append(init);
            data[i].filename=filenames[i];
            data[i].alv_file=fopen(data[i].filename.toLatin1().data(), "r");

            if (!data[i].alv_file) {
                setError(tr("could not open file '%1'").arg(data[i].filename));
                error=true;
                break;
            }
            if (ferror(data[i].alv_file)) {
                setError(tr("%2").arg(data[i].filename).arg(strerror(errno)));
                error=true;
                break;
            }
            data[i].token=ALV6_getToken(data[i].alv_file, data[i].readingHeader);
            if (error) break;


            // first we parse the header (until we find the first quoted string)
            // the data from the header is interpeted and the resulting quickfitDataStoreItem are
            // beeing created.
            while (data[i].token.type!=ALV6_EOF && data[i].readingHeader) {
                if (data[i].findIdentifier) {
                    if (data[i].token.type==ALV6_NAME) {
                        if (data[i].token.value.contains("ALV-6000", Qt::CaseInsensitive)) {
                            setQFProperty("ALV6_TYPE", data[i].token.value, false, true);
                            data[i].findIdentifier=false;
                        } else if (data[i].token.type!=ALV6_LINEBREAK && data[i].token.type!=ALV6_EOF) {
                            setError(tr("did not find file header").arg(data[i].filename));
                            error =true;
                            break;
                        }
                    }
                } else {
                    if (data[i].token.type==ALV6_NAME) { // here we parse "<name> : <value>"
                        QString name=data[i].token.value; // store <name>
                        QString value;
                        // get next token which has to be a colon':'
                        data[i].token=ALV6_getToken(data[i].alv_file, data[i].readingHeader);
                        if (data[i].token.type!=ALV6_COLON) {
                            if (name.toLower().startsWith("alv") && name.toLower().endsWith("data")) {
                                int pnum=1;
                                while (propertyExists(QString("ALV6_TYPE%1").arg(pnum))) pnum++;
                                setQFProperty(QString("ALV6_TYPE%1").arg(pnum), data[i].token.value, false, true);
                            } else {
                                setError(tr("colon ':' expected, but found '%2'").arg(data[i].filename).arg(data[i].token.value));
                                error=true;
                                break;
                            }
                        } else {
                            // get next token which has to be a value or a quoted string or a linebreak
                            data[i].token=ALV6_getToken(data[i].alv_file, data[i].readingHeader);
                            if (data[i].token.type==ALV6_QUOTED) {
                                value=data[i].token.value;
                            } else if (data[i].token.type==ALV6_VALUE) {
                                value=data[i].token.value;
                            } else if (data[i].token.type!=ALV6_LINEBREAK) {
                                setError(tr("linebreak, number or quoted string expected").arg(data[i].filename));
                                error=true;
                                break;
                            }
                            // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                            if (name.compare("Date",  Qt::CaseInsensitive)==0) {
                                QDate date=ALV6_toDate(value);
                                if (date.year()<1950) date.setDate(date.year()+100, date.month(), date.day());
                                if (i==0) setQFProperty("DATE", date, false, true);
                                else if (date!=getProperty("DATE").toDate()) setQFProperty(QString("DATE%1").arg(i+1), date, false, true);
                            } else if (name.compare("Time",  Qt::CaseInsensitive)==0) {
                                QTime date=ALV6_toTime(value);
                                if (i==0) setQFProperty("TIME", date, false, true);
                                else if (date!=getProperty("TIME").toTime()) setQFProperty(QString("TIME%1").arg(i+1), date, false, true);
                            } else if (name.contains("Duration",  Qt::CaseInsensitive)) {
                                QString propN="DURATION [s]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.compare("Runs",  Qt::CaseInsensitive)==0) {
                                data[i].runs=(int)round(data[i].token.doubleValue);
                            } else if (name.contains("Temperature",  Qt::CaseInsensitive)) {
                                QString propN="TEMPERATURE [K]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Viscosity",  Qt::CaseInsensitive)) {
                                QString propN="VISCOSITY [cp]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Refractive Index",  Qt::CaseInsensitive)) {
                                QString propN="REFRACTIVE_INDEX";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Wavelength",  Qt::CaseInsensitive)) {
                                QString propN="WAVELENGTH [nm]";
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("Angle",  Qt::CaseInsensitive)) {
                                QString propN=QLatin1String("ANGLE [\xB0]");
                                double propV=data[i].token.doubleValue;
                                if (i==0) setQFPropertyIfNotUserEditable(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));
                            } else if (name.contains("MeanCR",  Qt::CaseInsensitive)) {
                                // ignore this property, as it is calculated by this class
                            } else if (name.contains("SampMemo",  Qt::CaseInsensitive)) {
                                QString text=getProperty("SAMPLE_MEMO", "").toString();
                                if (!value.isEmpty()) text=text+tr("\nfile %1\n").arg(i+1)+value;
                                setQFProperty("SAMPLE_MEMO", text, false, true);
                            } else if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                                QString propN="MODE";
                                QString propVS=value;
                                if (i==0) setQFProperty(propN, propVS, false, true);
                                else if (propVS!=getProperty(propN).toString()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propVS).arg(propN));

                                propN="CROSS_CORRELATION";
                                bool propV=(bool)value.contains("CROSS", Qt::CaseInsensitive);
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));

                                propN="DUAL_CHANNEL";
                                propV=value.contains("DUAL", Qt::CaseInsensitive);
                                data[i].isDual=propV;
                                if (i==0) setQFProperty(propN, propV, false, true);
                                else if (propV!=getProperty(propN).toDouble()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propV).arg(propN));


                            } else {
                                QString propN=name;
                                QString propVS=value;
                                if (i==0) setQFProperty(propN, propVS, false, true);
                                else if (propVS!=getProperty(propN).toString()) log_warning(tr("warning in file '%1': value of property '%4' in file (%2) does not match property value of other files (%3).\n").arg(filenames[i]).arg(getProperty(propN).toString()).arg(propVS).arg(propN));
                            }
                        }

                    } else if (data[i].token.type==ALV6_QUOTED) {
                        // we stop reading the header when we meet the first quoted string token
                        // this is possible as every line of the header begins with an unquoted name
                        // token followed by a colon and number/quoted or alineabreak (identifier line!)
                        data[i].readingHeader=false;
                    }
                }
                if (data[i].readingHeader) data[i].token=ALV6_getToken(data[i].alv_file, data[i].readingHeader);
            }
            if (data[i].isDual) {
                if (propertyExists("CHANNEL")) channel=getProperty("CHANNEL", 0).toInt();
            }






            if (!error) {
                while (data[i].token.type!=ALV6_EOF) {
                    bool getNew=true;
                    if (data[i].token.type==ALV6_QUOTED && data[i].token.value.contains("correlation", Qt::CaseInsensitive)) {
                        getNew=false;
                        QVector<QVector<double> > dat;
                        data[i].token=ALV6_readNumberMatrix(data[i].alv_file, &(dat));
                        data[i].dat_corr=dat;
                        //qDebug()<<"  parsing correlation section ... "<<data[i].dat_corr.size()<<data[i].dat_corr[0].size()<<"\n";
                    }
                    if (data[i].token.type==ALV6_QUOTED && data[i].token.value.contains("count", Qt::CaseInsensitive)) {
                        getNew=false;
                        QVector<QVector<double> > dat;
                        data[i].token=ALV6_readNumberMatrix(data[i].alv_file, &(dat));
                        data[i].dat_rate=dat;
                        //qDebug()<<"  parsing rate section ... "<<data[i].dat_rate.size()<<data[i].dat_rate[0].size()<<"\n";

                    }
                    if (getNew) data[i].token=ALV6_getToken(data[i].alv_file, false);
                }
            }
            if (error) break;
        }


        if (!error) {
            int runs=0;
            int ccorrN=0;
            int rrateN=0;
            for (int i=0; i<filenames.size(); i++) {
                runs+=data[i].runs;
                if (data[i].dat_corr.size()>ccorrN) ccorrN=data[i].dat_corr.size();
                if (data[i].dat_rate.size()>rrateN) rrateN=data[i].dat_rate.size();
            }

            resizeCorrelations(ccorrN, runs);
            if (getProperty("CROSS_CORRELATION", false).toBool()||getProperty("DUAL_CHANNEL", false).toBool()) resizeRates(rrateN, runs, 2);
            else resizeRates(rrateN, runs, 1);
            int run0=0;
            for (int ii=0; ii<data.size(); ii++) {
                //qDebug()<<ii<<data[ii].dat_corr.size();
                for (long long  i=0; i<data[ii].dat_corr.size(); i++) {
                    //qDebug()<<i<<"/"<<data[ii].dat_corr.size()<<":   "<<error;
                    QVector<double> d=data[ii].dat_corr[i];
                    if (d.size()<=0) {
                        setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[ii].filename));
                        error=true;
                        //qDebug()<<"ERROR 1 !";
                        break;
                    }
                    if (ii==0) {
                        correlationT[i]=d[0]*timefactor/1000.0;
                    } else {
                        if (correlationT[i]!=d[0]*timefactor/1000.0) {
                            setError(tr("lag time in file '%2' and file '%1' are unequal (expected %3, but read %4).").arg(data[0].filename).arg(data[ii].filename).arg(correlationT[i]).arg(d[0]/1000.0));
                            error=true;
                            //qDebug()<<"ERROR 2 !";
                            break;
                        }
                    }
                    //qDebug()<<correlationT[i];




                    if (data[ii].runs==1) {
                        // one run => there is no average in the file!
                        if (d.size()<=1+channel)  {
                            setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[0].filename));
                            return false;
                        }
                        correlation[run0*correlationN+i]=d[1+channel];
                    } else if (data[ii].runs>1) {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel*(1+data[ii].runs)+data[ii].runs) {
                            setError(tr("too few columns in line %1 of correlation block in file '%2'.").arg(i).arg(data[0].filename));
                            error=true;
                            //qDebug()<<"ERROR 3 !   channel="<<channel<<"   runs="<<data[ii].runs<<"    s.size="<<d.size()<<"   comape="<<1+channel*(1+data[ii].runs)+data[ii].runs;
                            break;
                        }
                        for (long j=0; j<data[ii].runs; j++) {
                            correlation[(run0+j)*correlationN+i]=d[1+channel*(1+data[ii].runs)+(1+j)];
                        }
                    }
                    //qDebug()<<correlationT[i]<<correlation[run0*correlationN+i];
                }

                if (error) break;

                for (long long i=0; i<data[ii].dat_rate.size(); i++) {
                    QVector<double>& d=data[ii].dat_rate[i];
                    if (d.size()<1) {
                        setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                        error=true;
                        break;
                    }
                    if (ii==0) {
                        rateT[i]=d[0];
                    } else {
                        if (rateT[i]!=d[0]) {
                            setError(tr("countrate timepoint in file '%2' and file '%1' are unequal (expected %3, but read %4).").arg(data[0].filename).arg(data[ii].filename).arg(rateT[i]).arg(d[0]));
                            error=true;
                            break;
                        }
                    }



                    if (rateChannels<=1) {
                        if (data[ii].runs==1) {
                            // one run => there is no average in the file!
                            if (d.size()<=1+channel)  {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            rate[run0*rateN+i]=d[1+channel];
                        } else if (data[ii].runs>1) {
                            // multiple runs -> average is in file -> ignore average column
                            if (d.size()<=1+channel) {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            rate[(data[ii].runs+run0-1)*rateN+i]=d[1+channel];
                        }
                    } else {
                        if (runs==1) {
                            // one run => there is no average in the file!
                            if (d.size()<=1+channel)  {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            /*if (data[ii].isDual && channel==1) {
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+run0*rateN+i]=d[1+rateChannels-1-c];
                                }
                            } else {*/
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+run0*rateN+i]=d[1+c];
                                }
                            //}
                            if (getRateChannelsSwapped()) {
                                //qDebug()<<"swapping in "<<getName();
                                qSwap(rate[0*rateN*rateRuns+run0*rateN+i], rate[1*rateN*rateRuns+run0*rateN+i]);
                            }
                        } else if (runs>1) {
                            // multiple runs -> average is in file -> ignore average column
                            if (d.size()<=1+channel) {
                                setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[0].filename));
                                error=true;
                                break;
                            }
                            /*if (data[ii].isDual) {
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+rateChannels-1-c];
                                }
                            } else {*/
                                for (int c=0; c<rateChannels; c++) {
                                    rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+c];
                                }
                            //}
                            if (getRateChannelsSwapped()) {
                                //qDebug()<<"swapping in "<<getName();
                                qSwap(rate[0*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i], rate[1*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]);
                            }
                        }
                    }



                    /*if (rateChannels<=1) {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel) {
                            setError(tr("too few columns in line %1 of rate block in file '%2'.").arg(i).arg(data[ii].filename));
                            error=true;
                            break;
                        }
                        rate[(run0+runs-1)*rateN+i]=d[1+channel];
                    } else {
                        // multiple runs -> average is in file -> ignore average column
                        if (d.size()<=1+channel) {
                            setError(tr("too few columns in line %1 of rate block.").arg(i));
                            error=true;
                            break;
                        }
                        if (data[ii].isDual) {
                            for (int c=0; c<rateChannels; c++) {
                                rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+rateChannels-1-c];
                            }
                        } else {
                            for (int c=0; c<rateChannels; c++) {
                                rate[c*rateN*rateRuns+(run0+data[ii].runs-1)*rateN+i]=d[1+c];
                            }
                        }
                    }*/
                }
                if (error) break;
                run0=run0+data[ii].runs;
            }
        }



        //std::cout<<"parsing data section ... runs="<<runs<<"  channel="<<channel<<std::endl;
        for (int i=0; i<data.size(); i++) {
            if (data[i].alv_file) fclose(data[i].alv_file);
            data[i].alv_file=NULL;
        }

       // std::cout<<"recalc correlations ..."<<std::endl;
        if (!error) {
            recalculateCorrelations();

           // std::cout<<"calc binned rates ..."<<std::endl;
            autoCalcRateN=rateN;
            calcBinnedRate();
            //std::cout<<"DONE !!!"<<std::endl;
            emitRawDataChanged();
            return true;
        } else {
            return false;
        }
    } catch (std::exception& e) {
        setError(tr("error while reading file '1':\n   %2").arg(filenames.value(0, "???")).arg(e.what()));
        return false;
    }

}



struct ALV7FILEDATA  {
    FILE* alv_file;
    QString filename;
    int runs;
    bool isDual;

    bool readingHeader;
    bool findIdentifier;
    ALV7_TOKEN token;
    QVector<QVector<double> > dat_corr;
    QVector<QVector<double> > dat_rate;
};


bool QFRDRFCSData::loadFromALV7000File(QString &filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }
    try {
        double timefactor=getProperty("ALV_TIMEFACTOR", 1.0).toDouble();
        ALV7FILEDATA data;
        data.runs=0;
        data.alv_file=NULL;
        data.filename="";
        data.isDual=false;
        data.readingHeader=true;
        data.findIdentifier=true;
        bool error=false;
        int channel=0;
        bool gotCorr=false, gotRate=false;
        data.filename=filename;
        data.alv_file=fopen(data.filename.toLatin1().data(), "r");

        if (!data.alv_file) {
            setError(tr("could not open file '%1'").arg(data.filename));
            error=true;
            return false;
        }
        if (ferror(data.alv_file)) {
            setError(tr("%2").arg(data.filename).arg(strerror(errno)));
            error=true;
            fclose(data.alv_file);
            return false;
        }
        data.token=ALV7_getToken(data.alv_file, data.readingHeader);
        //if (error) break;


        // first we parse the header (until we find the first quoted string)
        // the data from the header is interpeted and the resulting quickfitDataStoreItem are
        // beeing created.
        while (data.token.type!=ALV7_EOF && data.readingHeader) {
            if (data.findIdentifier) {
                if (data.token.type==ALV7_NAME) {
                    //qDebug()<<"finID: "<<data.token.value;
                    if (data.token.value.contains("ALV-7", Qt::CaseInsensitive)) {
                        setQFProperty("ALV_TYPE", data.token.value, false, true);
                        data.findIdentifier=false;
                    } else if (data.token.type!=ALV7_LINEBREAK && data.token.type!=ALV7_EOF) {
                        setError(tr("did not find file header").arg(data.filename));
                        error =true;
                        break;
                    }
                }
            } else {
                if (data.token.type==ALV7_NAME) { // here we parse "<name> : <value>"
                    QString name=data.token.value; // store <name>
                    QString value;
                    // get next token which has to be a colon':'
                    data.token=ALV7_getToken(data.alv_file, data.readingHeader);
                    if (data.token.type!=ALV7_COLON) {
                        if (name.toLower().startsWith("alv") && name.toLower().endsWith("data")) {
                            int pnum=1;
                            while (propertyExists(QString("ALV7_TYPE%1").arg(pnum))) pnum++;
                            setQFProperty(QString("ALV7_TYPE%1").arg(pnum), data.token.value, false, true);
                        } else {
                            setError(tr("colon ':' expected, but found '%2'").arg(data.filename).arg(data.token.value));
                            error=true;
                            break;
                        }
                    } else {
                        // get next token which has to be a value or a quoted string or a linebreak
                        data.token=ALV7_getToken(data.alv_file, data.readingHeader);
                        if (data.token.type==ALV7_QUOTED) {
                            value=data.token.value;
                        } else if (data.token.type==ALV7_VALUE) {
                            value=data.token.value;
                        } else if (data.token.type!=ALV7_LINEBREAK) {
                            setError(tr("linebreak, number or quoted string expected").arg(data.filename));
                            error=true;
                            break;
                        }
                        // here we check whether this is an interpreted data field (i.e. it is stored in a separate field of item1
                        if (name.compare("Date",  Qt::CaseInsensitive)==0) {
                            QDate date=ALV7_toDate(value);
                            if (date.year()<1950) date.setDate(date.year()+100, date.month(), date.day());
                            setQFProperty("DATE", date, false, true);
                        } else if (name.compare("Time",  Qt::CaseInsensitive)==0) {
                            QTime date=ALV7_toTime(value);
                            setQFProperty("TIME", date, false, true);
                        } else if (name.contains("Duration",  Qt::CaseInsensitive)) {
                            QString propN="DURATION [s]";
                            double propV=data.token.doubleValue;
                            setQFProperty(propN, propV, false, true);
                        } else if (name.compare("Runs",  Qt::CaseInsensitive)==0) {
                            data.runs=(int)round(data.token.doubleValue);
                        } else if (name.contains("Temperature",  Qt::CaseInsensitive)) {
                            QString propN="TEMPERATURE [K]";
                            double propV=data.token.doubleValue;
                            setQFPropertyIfNotUserEditable(propN, propV, false, true);
                        } else if (name.contains("Viscosity",  Qt::CaseInsensitive)) {
                            QString propN="VISCOSITY [cp]";
                            double propV=data.token.doubleValue;
                            setQFPropertyIfNotUserEditable(propN, propV, false, true);
                        } else if (name.contains("Refractive Index",  Qt::CaseInsensitive)) {
                            QString propN="REFRACTIVE_INDEX";
                            double propV=data.token.doubleValue;
                            setQFPropertyIfNotUserEditable(propN, propV, false, true);
                        } else if (name.contains("Wavelength",  Qt::CaseInsensitive)) {
                            QString propN="WAVELENGTH [nm]";
                            double propV=data.token.doubleValue;
                            setQFPropertyIfNotUserEditable(propN, propV, false, true);
                        } else if (name.contains("Angle",  Qt::CaseInsensitive)) {
                            QString propN=QLatin1String("ANGLE [\xB0]");
                            double propV=data.token.doubleValue;
                            setQFPropertyIfNotUserEditable(propN, propV, false, true);
                        } else if (name.contains("MeanCR",  Qt::CaseInsensitive)) {
                            // ignore this property, as it is calculated by this class
                        } else if (name.contains("SampMemo",  Qt::CaseInsensitive)) {
                            QString text=getProperty("SAMPLE_MEMO", "").toString();
                            setQFProperty("SAMPLE_MEMO", text, false, true);
                        } else if (name.compare("Mode",  Qt::CaseInsensitive)==0) {
                            QString propN="MODE";
                            QString propVS=value;
                            setQFProperty(propN, propVS, false, true);

                            unsigned int ccount=0;
                            int inch=0;
                            int channel=0;
                            bool cross=false;
                            bool autoc=false;
                            ALV7_analyzeMode(propVS, ccount, cross, autoc, inch, channel);


                            setQFProperty("CROSS_CORRELATION", cross, false, true);


                            setQFProperty("AUTO_CORRELATION", autoc, false, true);

                            data.isDual=ccount>1;
                            setQFProperty("DUAL_CHANNEL", ccount>1, false, true);

                            setQFProperty("INPUT_CHANNELS", inch, false, true);

                            setQFProperty("CHANNELS", ccount, false, true);


                        } else {
                            setQFProperty(name, value, false, true);
                        }
                    }

                } else if (data.token.type==ALV7_QUOTED) {
                    // we stop reading the header when we meet the first quoted string token
                    // this is possible as every line of the header begins with an unquoted name
                    // token followed by a colon and number/quoted or alineabreak (identifier line!)
                    data.readingHeader=false;
                }
            }
            if (data.readingHeader) data.token=ALV7_getToken(data.alv_file, data.readingHeader);
        }


        channel=getProperty("CHANNEL", 0).toInt();



        if (!error) {
            while (data.token.type!=ALV7_EOF && (!gotCorr || !gotRate)) {
                bool getNew=true;
                //qDebug()<<data.token.value;
                if (data.token.type==ALV7_QUOTED && data.token.value.contains("correlation", Qt::CaseInsensitive)) {
                    getNew=false;
                    QVector<QVector<double> > dat;
                    data.token=ALV7_readNumberMatrix(data.alv_file, &(dat));
                    data.dat_corr=dat;
                    gotCorr=true;
                    //qDebug()<<"  parsing correlation section ... "<<data.dat_corr.size()<<data.dat_corr[0].size()<<"\n";
                    getNew=false;
                }
                if (data.token.type==ALV7_QUOTED && data.token.value.contains("count", Qt::CaseInsensitive)) {
                    getNew=false;
                    QVector<QVector<double> > dat;
                    data.token=ALV7_readNumberMatrix(data.alv_file, &(dat));
                    data.dat_rate=dat;
                    gotRate=true;
                    //qDebug()<<"  parsing rate section ... "<<data.dat_rate.size()<<data.dat_rate[0].size()<<"\n";
                    getNew=false;
                }
                if (getNew) data.token=ALV7_getToken(data.alv_file, false);
            }
        }



        if (!error) {
            QList<int> columns=stringToIntList(getProperty("COLUMNS", "").toString());
            int runs=columns.size();//data.runs;
            int ccorrN=data.dat_corr.size();
            int rrateN=data.dat_rate.size();

            resizeCorrelations(ccorrN, runs);

            for (long long  i=0; i<correlationN; i++) {
                QVector<double> d=data.dat_corr.value(i, QVector<double>());
                correlationT[i]=d.value(0, 0.0)*timefactor/1000.0;
                for (int run=0; run<correlationRuns; run++) {
                    int col=columns.value(run, -1);
                    if (col>=0) {
                        correlation[run*correlationN+i]=d[col];
                        // TODO: CHECK how the layout is for several runs in one file!!!
                    }
                }
            }

            resizeRates(rrateN, runs, qBound(1, getProperty("INPUT_CHANNELS", 1).toInt(), 1000));

            for (long long  i=0; i<rateN; i++) {
                QVector<double> d=data.dat_rate.value(i, QVector<double>());
                rateT[i]=d.value(0, 0.0);
                for (int r=0; r<rateRuns; r++) {
                    if (r==rateRuns-1) {
                        for (int cha=0; cha<rateChannels; cha++) {
                            rate[cha*rateN*rateRuns+r*rateN+i]=d.value(1+cha, 0.0);
                            // TODO: CHECK how the layout is for several runs in one file!!!
                        }
                    }
                }
            }
        }



        //std::cout<<"parsing data section ... runs="<<runs<<"  channel="<<channel<<std::endl;
        if (data.alv_file) fclose(data.alv_file);
        data.alv_file=NULL;

       // std::cout<<"recalc correlations ..."<<std::endl;
        if (!error) {
            recalculateCorrelations();
           // std::cout<<"calc binned rates ..."<<std::endl;
            autoCalcRateN=rateN;
            calcBinnedRate();
            //std::cout<<"DONE !!!"<<std::endl;
            emitRawDataChanged();
            return true;
        } else {
            return false;
        }
    } catch (std::exception& e) {
        setError(tr("error while reading file '1':\n   %2").arg(filename).arg(e.what()));
        return false;
    }
}


QString QFRDRFCSData::getCorrelationRunName(int run) const {
    //if (run<0) return tr("average");
    QString name="";
    if (run<correlationRuns) name= tr("run %1").arg(run);
    if (leaveoutRun(run)) name+=tr(" (excl.)");
    return name;
}

double *QFRDRFCSData::getRate(int channel) const
{
    return &(rate[channel*rateN*rateRuns]);
}

double *QFRDRFCSData::getRateRun(int run, int channel) const
{
    if (run>=0 && run<rateRuns) return &(rate[channel*rateN*rateRuns+run*rateN]);
    return NULL;
}

bool QFRDRFCSData::getRateChannelsSwapped() const
{
    int channel=0;
    if (propertyExists("CHANNELS_SWAPPED")) return getProperty("CHANNELS_SWAPPED", false).toBool();
    if (propertyExists("CHANNEL")) channel=getProperty("CHANNEL", 0).toInt();
    //qDebug()<<"channelsSwapped(): "<<getName()<<channel<<rateChannels;
    if (rateChannels==2 && channel==1) return true;
    return false;
}

void QFRDRFCSData::saveInternal(QXmlStreamWriter& w) const {
    /*w.writeStartElement("internal_correlations");
    double offset=getProperty("CF_CORRECT_OFFSET", 0.0).toDouble();
    QString csv;
    for (int i=0; i<correlationN; i++) {
        csv=csv+CDoubleToQString(correlationT[i]);
        for (int j=0; j<correlationRuns; j++) {
            csv=csv+", "+CDoubleToQString(correlation[j*correlationN+i]+offset);
        }
        csv=csv+"\n";
    }
    w.writeAttribute("mode", getProperty("INTERNAL_CSVMODE", "tccc").toString().toLower());
    w.writeCDATA(csv);
    w.writeEndElement();

    w.writeStartElement("internal_countrate");
    csv="";
    for (int t=0; t<rateN; t++) {
        csv=csv+CDoubleToQString(rateT[t]);
        for (int c=0; c<rateChannels; c++) {
            for (int r=0; r<rateRuns; r++) {
                double* rat=getRateRun(r, c);
                csv=csv+", "+CDoubleToQString(rat[t]);
            }
        }
        csv=csv+"\n";
    }
    w.writeCDATA(csv);
    w.writeEndElement();*/



    double offset=getProperty("CF_CORRECT_OFFSET", 0.0).toDouble();
    w.writeStartElement("internal_correlations_bin");
    w.writeAttribute("correlationN", QString::number(correlationN));
    w.writeAttribute("correlationRuns", QString::number(correlationRuns));
    w.writeAttribute("encoding", "base64");
        w.writeStartElement("tau");
        QVector<double> tmp=arrayToVector(correlationT, correlationN);
        w.writeCDATA(doubleArrayToString_base64(tmp));
        w.writeEndElement();
        w.writeStartElement("correlations");
        tmp=arrayToVector(correlation, correlationN*correlationRuns);
        for (int i=0; i<tmp.size(); i++) tmp[i]=tmp[i]+offset;
        w.writeCDATA(doubleArrayToString_base64(tmp));
        w.writeEndElement();
        w.writeStartElement("correlation_errors");
        tmp=arrayToVector(correlationErrors, correlationN*correlationRuns);
        w.writeCDATA(doubleArrayToString_base64(tmp));
        w.writeEndElement();
    w.writeEndElement();

    w.writeStartElement("internal_countrate_bin");
    w.writeAttribute("encoding", "base64");
    w.writeAttribute("rateRuns", QString::number(rateRuns));
    w.writeAttribute("rateChannels", QString::number(rateChannels));
    w.writeAttribute("rateN", QString::number(rateN));
        w.writeStartElement("time");
        tmp=arrayToVector(rateT, rateN);
        w.writeCDATA(doubleArrayToString_base64(tmp));
        w.writeEndElement();
        w.writeStartElement("counts");
        tmp=arrayToVector(rate, rateRuns*rateChannels*rateN);
        for (int i=0; i<tmp.size(); i++) tmp[i]=tmp[i]+offset;
        w.writeCDATA(doubleArrayToString_base64(tmp));
        w.writeEndElement();
    w.writeEndElement();


}

bool QFRDRFCSData::loadInternal(QDomElement* e) {
    bool okC=false;
    bool okR=false;
    QString csv=getProperty("INTERNAL_CSV", "").toString();
    QString csvCnt=getProperty("INTERNAL_CSV_CNT", "").toString();
    QString mode=getProperty("INTERNAL_CSVMODE", "tccc").toString().toLower();
    int channelsCnt=getProperty("INTERNAL_CSV_CNT_CHANNELS", "1").toInt();
    //qDebug()<<csv;
    deleteProperty("INTERNAL_CSV");
    if (e && csv.isEmpty()) {
        QDomElement de=e->firstChildElement("internal_correlations");
        if (!de.isNull()) {
            mode=de.attribute("mode", mode).toLower();
            csv=de.text();
        }
    }
    deleteProperty("INTERNAL_CSV_CNT");
    if (e && csvCnt.isEmpty()) {
        QDomElement de=e->firstChildElement("internal_countrate");
        if (!de.isNull()) {
            csvCnt=de.text();
        }
    }
    //qDebug()<<csv;
    setQFProperty("INTERNAL_CSVMODE", mode, false, false);
    if (!csv.isEmpty()) {
        //qDebug()<<"parsing csv: "<<csv;

        QTextStream f(&csv);
        QList<QVector<double> > datalist;
        QVector<double> data;
        do {
            data=csvReadline(f, ',', '#', 0);
           // //qDebug()<<"        "<<data;
            if (data.size()>0) datalist.append(data);
        } while (data.size()>0);
        int runs=0;
        if (datalist.size()>0) {
            if (mode=="tcecece" || mode=="1") {
                runs=(datalist[0].size()-1)/2;
            } else {
                runs=datalist[0].size()-1;
            }
        }
        //qDebug()<<"  -> "<<mode;
        //qDebug()<<"  -> "<<datalist.size()<<runs;
        //qDebug()<<"  -> "<<datalist;
        resizeCorrelations(datalist.size(), runs);
        for (int i=0; i<datalist.size(); i++) {
            correlationT[i]=datalist[i].value(0,0.0);
            if (mode=="tcecece" || mode=="1") {
                for (int j=1; j<datalist[i].size(); j=j+2) {
                    if (j+1<datalist[i].size()) {
                        correlation[(j-1)/2*correlationN+i]=datalist[i].value(j,0.0);
                        correlationErrors[(j-1)/2*correlationN+i]=datalist[i].value(j+1,0.0);
                    }
                }
            } else {
                for (int j=1; j<datalist[i].size(); j++) {
                    correlation[(j-1)*correlationN+i]=datalist[i].value(j,0.0);
                    correlationErrors[(j-1)*correlationN+i]=0;
                }
            }
        }
        recalculateCorrelations();
        okC= true;
    }

    //qDebug()<<channelsCnt<<csvCnt;
    if (!csvCnt.isEmpty()) {
        //qDebug()<<"parsing csv: "<<csvCnt;

        QTextStream f(&csvCnt);
        QList<QVector<double> > datalist;
        QVector<double> data;
        do {
            data=csvReadline(f, ',', '#', 0);
            //qDebug()<<"        "<<data;
            if (data.size()>0) datalist.append(data);
        } while (data.size()>0);
        int runs=0;
        int channels=channelsCnt;
        if (datalist.size()>0) {
                runs=(datalist[0].size()-1)/channels;
        }
        //qDebug()<<"  -> "<<datalist.size()<<runs<<channels;
        //qDebug()<<"  -> "<<datalist;
        resizeRates(datalist.size(), runs, channels);
        for (int i=0; i<datalist.size(); i++) {
            rateT[i]=datalist[i].value(0,0.0);
            for (int c=0; c<channels; c++) {
                for (int j=0; j<runs; j++) {
                    double* rat=getRateRun(j, c);
                    const int jidx=1+c*runs+j;
                    if (jidx<datalist[i].size()) {
                        rat[i]=datalist[i].value(jidx,0.0);
                    }
                }
            }
        }
        calcBinnedRate();
        okR= true;
    }

    if (!okC) {
        if (e) {
            QDomElement de=e->firstChildElement("internal_correlations_bin");
            if (!de.isNull()) {
                int N=de.attribute("correlationN", "0").toInt();
                int R=de.attribute("correlationRuns", "0").toInt();
                resizeCorrelations(N, R);
                QDomElement det=de.firstChildElement("tau");
                QDomElement dec=de.firstChildElement("correlations");
                QDomElement dee=de.firstChildElement("correlation_errors");
                QVector<double> tmp;
                if (!det.isNull()) {
                    tmp=stringToDoubleArray_base64(det.text());
                    for (int i=0; i<qMin((long long)tmp.size(), correlationN); i++) correlationT[i]=tmp.value(i, 0.0);
                }
                if (!dec.isNull()) {
                    tmp=stringToDoubleArray_base64(dec.text());
                    for (int i=0; i<qMin((long long)tmp.size(), correlationN*correlationRuns); i++) correlation[i]=tmp.value(i, 0.0);
                }
                if (!dee.isNull()) {
                    tmp=stringToDoubleArray_base64(dee.text());
                    for (int i=0; i<qMin((long long)tmp.size(), correlationN*correlationRuns); i++) correlationErrors[i]=tmp.value(i, 0.0);
                }
                recalculateCorrelations();
                okC= true;
            }
        }
    }

    if (!okR) {
        if (e) {

            QDomElement de=e->firstChildElement("internal_countrate_bin");
            if (!de.isNull()) {
                int C=de.attribute("rateChannels", "0").toInt();
                int N=de.attribute("rateN", "0").toInt();
                int R=de.attribute("rateRuns", "0").toInt();
                resizeRates(N, R, C);
                QDomElement det=de.firstChildElement("time");
                QDomElement dec=de.firstChildElement("counts");
                QVector<double> tmp;
                if (!det.isNull()) {
                    tmp=stringToDoubleArray_base64(det.text());
                    for (int i=0; i<qMin((long long)tmp.size(), rateN); i++) rateT[i]=tmp.value(i, 0.0);
                }
                if (!dec.isNull()) {
                    tmp=stringToDoubleArray_base64(dec.text());
                    for (int i=0; i<qMin((long long)tmp.size(), rateN*rateChannels*rateRuns); i++) rate[i]=tmp.value(i, 0.0);
                }
                calcBinnedRate();
                okR= true;
            }
        }
    }

    return okC;
}

bool QFRDRFCSData::mayDeleteFiles(QStringList &files, QStringList &/*types*/, QStringList &/*descriptions*/) const
{
    QStringList f=this->files;
    for (int i=0; i<files.size(); i++) {
        f.removeAll(files[i]);
    }
    return (f.size()>0);
}

bool QFRDRFCSData::reloadFromFiles() {

    autoCalcRateN=getProperty("AUTO_BINNED_RATE_N", autoCalcRateN).toInt();

    QString filetype=getProperty("FILETYPE", "unknown").toString();
    //std::cout<<"reading data "<<filetype.toStdString()<<" from 1/"<<files.size()<<" '"<<files.join(", ").toStdString()<<"'\n";

    rateMean.clear();
    rateStdDev.clear();
    rateMin.clear();
    rateMax.clear();

    if (filetype.toUpper()=="ALV5000") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        return loadFromALV5000Files(files);
    } else if (filetype.toUpper()=="ALV6000") {
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       return loadFromALV6000Files(files);
    } else if (filetype.toUpper()=="ALV7000") {
        bool res=false;
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       //return loadFromALV7000Files(files);
       if (files.size()==1) {
           res=loadFromALV7000File(files[0]);
       } else if (files.size()>1) {
           loadSeveral(res, files, loadFromALV7000File);
       }
       return res;
    } else if (filetype.toUpper()=="CORRELATOR.COM_SIN") {
        bool res=false;
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       if (files.size()==1) {
           res=loadCorrelatorComSIN(files[0]);
       } else if (files.size()>1) {
           loadSeveral(res, files, loadCorrelatorComSIN);
       }
       return res;
    } else if (filetype.toUpper()=="CONFOCOR3") {
        bool res=false;
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       if (files.size()==1) {
           res=loadConfocor3(files[0]);
       } else if (files.size()>1) {
           loadSeveral(res, files, loadConfocor3);
       }
       return res;
    } else if (filetype.toUpper()=="QF3ASCIICORR") {
       bool res=false;
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       if (files.size()==1) {
           res=loadQF3ASCII(files[0]);
       } else if (files.size()>1) {
           loadSeveral(res, files, loadQF3ASCII);
       }
       return res;
    } else if (filetype.toUpper()=="OLEGKRIECHEVSKYBINARY") {
       if (files.size()<=0) {
          setError(tr("there are no files in the FCS record!"));
          return false;
       }
       bool res=false;
       if (files.size()==1) {
           res=loadOlegData(files[0]);
       } else if (files.size()>1) {
           loadSeveral(res, files, loadOlegData);
       }
       return res;
    } else if (filetype.toUpper()=="ISS_ALBA") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        return loadCorrelationCurvesFromALBA(files);
    } else if (filetype.toUpper()=="PICOQUANT_ASCII_COR") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        bool res=false;
        if (files.size()==1) {
            res=loadCorrelationCurvesFromPicoQuantCOR(files[0]);
        } else if (files.size()>1) {
            loadSeveral(res, files, loadCorrelationCurvesFromPicoQuantCOR);
        }
        return res;
    } else if (filetype.toUpper()=="PICOQUANT_ASCII_FCS_W") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        bool res=false;
        if (files.size()==1) {
            res=loadCorrelationCurvesFromPicoQuantASCIIFCSWeights(files[0]);
        } else if (files.size()>1) {
            loadSeveral(res, files, loadCorrelationCurvesFromPicoQuantASCIIFCSWeights);
        }
        return res;
    } else if (filetype.toUpper()=="PICOQUANT_ASCII_FCS") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        bool res=false;
        if (files.size()==1) {
            res=loadCorrelationCurvesFromPicoQuantASCIIFCSNoWeights(files[0]);
        } else if (files.size()>1) {
            loadSeveral(res, files, loadCorrelationCurvesFromPicoQuantASCIIFCSNoWeights);
        }
        return res;
    } else if (filetype.toUpper()=="DIFFUSION4_SIMRESULTS") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        bool ok=false;
        QStringList cfiles, pfiles;
        for (int i=0; i<files.size(); i++) {
            QString aft=files_types.value(i, "ACF").toUpper();
            if (aft=="ACF" || aft.isEmpty()) cfiles<<files[i];
            if (aft=="RATE") pfiles<<files[i];
        }
        //qDebug()<<"----------------------------------------------------\ncfiles="<<cfiles;
        //qDebug()<<"pfiles="<<pfiles;
        // now try to find additional files and data
        if (!(getProperty("SEARCHED_MORE_FILES", false).toBool()) && cfiles.size()>0) {
            QRegExp rxFCS("(.*)\\_(fcs.+)corr\\.dat");
            rxFCS.setMinimal(true);
            //qDebug()<<"matching "<<cfiles[0]<<" against "<<rxFCS.pattern();
            if (rxFCS.exactMatch(cfiles[0])) {
                QString objectname=rxFCS.cap(2);
                QString basename=rxFCS.cap(1)+"_"+objectname;
                QString btsfile=basename+"bts.dat";
                QString configfile=rxFCS.cap(1)+"_config.ini";
                QString description="";
                double bintime=0;
                if (QFile::exists(configfile)) {
                    jkINIParser2 ini;
                    try {
                        ini.readFile(QDir::toNativeSeparators(configfile).toStdString()); // read in an INI file
                        description=ini.getAsString((objectname+".description").toStdString(), "").c_str();
                        bintime=ini.getAsDouble((objectname+".save_binning_time").toStdString(), ini.getAsDouble("fcs.save_binning_time", 0));
                    } catch (std::exception& e) { // error handling
                    }
                    if (description.size()>0 && !getName().contains(description)) {
                        setName(description+" ("+QFileInfo(cfiles[0]).fileName()+")");
                    }
                }
                //qDebug()<<"----------------------------------------------------\nobjectname="<<objectname;
                //qDebug()<<"basename="<<basename;
                //qDebug()<<"description="<<description;
                //qDebug()<<"new_name="<<getName();
                //qDebug()<<"btsfile="<<btsfile;
                //qDebug()<<"configfile="<<configfile;
                //qDebug()<<"bintime="<<bintime;
                if (bintime>0) {
                    setQFProperty("CSV_RATEFACTOR", 1.0/bintime/1000.0, false, false);
                }
                if (QFile::exists(btsfile)) {
                    pfiles<<btsfile;
                    files<<btsfile;
                    setFileType(files.size()-1, "RATE");

                }
                setQFProperty("SEARCHED_MORE_FILES", true, false, false);
            }
        }

        ok=loadCorrelationCurvesFromCSV(cfiles);
        ok=ok&&loadCountRatesFromCSV(pfiles);
        return ok;
    } else if (filetype.toUpper()=="CSV_CORR") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        return loadCorrelationCurvesFromCSV(files);
    } else if (filetype.toUpper()=="CSV_RATE") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        return loadCountRatesFromCSV(files);
    } else if (filetype.toUpper()=="CSV_CORR_RATE") {
        if (files.size()<=1) {
            setError(tr("there are too few files in the FCS record (2 required)!"));
            return false;
        }
        QStringList cfiles, rfiles;
        bool ok=true;
        int channels=1;
        for (int i=0; i<files.size(); i+=1) {
            QString t=files_types.value(i, "");
            if (t.toUpper()=="CORR"||t.toUpper()=="ACF") {
                cfiles.append(files[i]);
            } else if (t.toUpper()=="CCF") {
                cfiles.append(files[i]);
                channels=2;
            } else if (t.toUpper()=="RATE") {
                rfiles.append(files[i]);
            } else {
                ok=false;
            }
            if (!ok) break;
        }

        if (!ok) {
            cfiles.clear();
            rfiles.clear();
            for (int i=0; i<files.size(); i+=2) {
                if (i+1<files.size()) {
                    cfiles.append(files[i]);
                    rfiles.append(files[i+1]);
                }
            }
        }
        return loadCorrelationCurvesFromCSV(cfiles) && loadCountRatesFromCSV(rfiles, channels);
    }
    return false;
}

QStringList QFRDRFCSData::getAvailableRoles() const
{
    QStringList sl;
    sl<<"ACF"<<"ACF0"<<"ACF1"<<"ACF2"<<"ACF3"<<"FCCS"<<"FCCS0"<<"FCCS1"<<"FCCS01"<<"FCCS02"<<"FCCS03"<<"FCCS10"<<"FCCS12"<<"FCCS13"<<"FCCS20"<<"FCCS21"<<"FCCS23"<<"FCCS30"<<"FCCS31"<<"FCCS32";
    return sl;
}

bool QFRDRFCSData::isRoleUserEditable() const
{
    return  true;
}

double QFRDRFCSData::getSimpleCountrateAverage(int run, int channel, bool swapChannels) const
{
    int ch=channel;
    if (getRateChannelsSwapped()&&swapChannels) {
        if (ch==1) ch=0;
        else if (ch==0) ch=1;
    }
    return getRateMean(run, ch);
}


double QFRDRFCSData::getSimpleCountrateStdDev(int run, int channel, bool swapChannels) const
{
    int ch=channel;
    if (getRateChannelsSwapped()&&swapChannels) {
        if (ch==1) ch=0;
        else if (ch==0) ch=1;
    }
    return getRateStdDev(run, ch);
}

int QFRDRFCSData::getSimpleCountrateChannels() const
{
    return rateChannels;
}

int QFRDRFCSData::getSimpleCountrateRuns() const
{
    return rateRuns;
}


bool QFRDRFCSData::loadOlegData(QString filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }

    bool ok=true;
    //if (filenames.size()<=0) ok=false;
    //for (int i=0; i<filenames.size(); i++) {
        //QString filename=filenames.first();
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            uint16_t version=binfileReadUint16(f);
            uint32_t runs=0;
            uint32_t runsDemanded=0;
            double runtime=0;
            int32_t corrN=0;
            int32_t cntN=0;
            int cntRuns=0;
            double AIClockDivider=0;
            double dt=0;
            double speed=0;
            QVector<double> lags;
            QVector<double> cnt;
            QVector<double> corr;
            QVector<double> cfcr;
            QVector<double> afterpulse;
            QString logbook;
            if (version==0) {
                runsDemanded=binfileReadUint32(f); // read demanded runs
                runs=binfileReadUint32(f); // read performed runs
                runtime=binfileReadDouble(f);
                corrN=binfileReadInt32(f);
                cntN=binfileReadInt32(f);
                lags= binfileReadDoubleArrayV(f, corrN);
                corr= binfileReadDoubleArrayV(f, corrN*runs);
                cntRuns=runs;
                cnt= binfileReadDoubleArrayV(f, cntN*cntRuns);
                for (int64_t i=0; i<cntN*(int32_t)runs; i++) {
                    cnt[i]=cnt[i]/1000.0;
                }
            } else if (version==1) {
                int32_t datatype=binfileReadInt32(f);
                if (datatype==100){
                    int32_t logbooksize=binfileReadInt32(f);
                    logbook=f.read(logbooksize);
                    runsDemanded=binfileReadUint32(f); // read demanded runs
                    runs=binfileReadUint32(f); // read performed runs
                    runtime=binfileReadDouble(f);
                    corrN=binfileReadInt32(f);
                    runs=binfileReadInt32(f);
                    cntN=binfileReadInt32(f);
                    cntRuns=binfileReadInt32(f);
                    lags= binfileReadDoubleArrayV(f, corrN);
                    afterpulse= binfileReadDoubleArrayV(f, corrN);
                    runs=(runs-2)/2;
                    corr=binfileReadDoubleArrayV(f, corrN*runs);
                    cfcr=binfileReadDoubleArrayV(f, corrN*runs);
                    cntN=binfileReadInt32(f);
                    cntRuns=binfileReadInt32(f);
                    cnt=binfileReadDoubleArrayV(f, cntN*cntRuns);
                }
            } else if (version==7) {
                int32_t datatype=binfileReadInt32(f);
                if (datatype==100){
                    int32_t TotalSaves=binfileReadInt32(f);         // A.TotalSaves =fread(fid, 1, 'int32');
                    speed=binfileReadDouble(f);              // A.Speed = fread(fid, 1, 'double');
                    int32_t logbooksize=binfileReadInt32(f);        // LogBookSize=fread(fid, 1, 'int32');
                    logbook=f.read(logbooksize);                    // A.LogBook = fscanf(fid, '%c', LogBookSize);
                    binfileReadInt32(f);                            // A.lag = fread(fid, APSize(2), 'double');
                    corrN=binfileReadInt32(f);
                    lags= binfileReadDoubleArrayV(f, corrN);            // APSize = fread(fid, 2, 'int32');
                    afterpulse= binfileReadDoubleArrayV(f, corrN);      // A.AfterPulse = fread(fid, APSize(2), 'double');
                    AIClockDivider=binfileReadDouble(f); // A.AIClockDivider  = fread(fid, 1, 'double');
                    dt=0;
                    runs=0;
                    for (int s=0; s<TotalSaves; s++) {
                        runsDemanded=binfileReadUint32(f);          // A.NrunsDemanded   =  fread(fid, 1, 'uint32');
                        int32_t lruns=binfileReadInt32(f);                   // A.NrunsPerformed  =  fread(fid, 1, 'int32');
                        runtime=binfileReadDouble(f);               // A.RunTime         =  fread(fid, 1, 'double');
                        lruns=binfileReadInt32(f);                  // ArraysLength      =  fread(fid, 4, 'int32');
                        corrN=binfileReadInt32(f);
                        int32_t lcntRuns=binfileReadInt32(f);
                        cntN=binfileReadInt32(f);

                        lruns=lruns/2;
                        runs=runs+lruns;
                        cntRuns=cntRuns+lcntRuns;
                        //qDebug()<<"s="<<s<<"   runsDemanded="<<runsDemanded<<"   runs="<<lruns<<"   runtime="<<runtime<<"   corrN="<<corrN<<"   runs="<<lruns<<"   cntN="<<cntN<<"   cntRuns="<<lcntRuns;

                        /*
                            if i == 1,
                                A.corrfunc =  [A.corrfunc, CorrArrays(:, 2:(ArraysLength(1)+1)/2)] ;
                                A.CF_CR    =  [A.CF_CR, CorrArrays(:, (ArraysLength(1)+3)/2:ArraysLength(1))] ;
                            else
                                A.corrfunc =  [A.corrfunc, CorrArrays(:, 1:(ArraysLength(1)/2))] ;
                                A.CF_CR    =  [A.CF_CR, CorrArrays(:, (ArraysLength(1)/2+1):ArraysLength(1))] ;
                            end;
                         */
                        if (s==0) {
                            binfileReadDoubleArrayV(f, corrN);              // ignore on first ???
                            corr=binfileReadDoubleArrayV(f, corrN*lruns);
                            cfcr=binfileReadDoubleArrayV(f, corrN*lruns);
                        } else {
                            corr<<binfileReadDoubleArrayV(f, corrN*lruns);
                            cfcr<<binfileReadDoubleArrayV(f, corrN*lruns);
                        }
                        cnt<<binfileReadDoubleArrayV(f, cntN*lcntRuns);       // Trace = fread(fid, [ArraysLength(4), ArraysLength(3)], 'double');
                        dt=binfileReadDouble(f);                         // A.dt              =  fread(fid, 1, 'double');

                        int32_t ca1=binfileReadInt32(f);                  // CoordArrayLength  =  fread(fid, 2, 'int32');
                        int32_t ca2=binfileReadInt32(f);
                        //qDebug()<<"ca1="<<ca1<<"   ca2="<<ca2;
                        if (ca1>0 && ca2>0){
                            QVector<double> ca=binfileReadDoubleArrayV(f, ca1*ca2);                 // CoordArray        =  fread(fid, [CoordArrayLength(2), CoordArrayLength(1)], 'double');%remove tag (before the dot-coma)
                        }
                        int32_t ia1=binfileReadInt32(f);                  // ImageArrayLength  =  fread(fid, 3, 'int32');
                        int32_t ia2=binfileReadInt32(f);
                        int32_t ia3=binfileReadInt32(f);
                        //qDebug()<<"ia1="<<ia1<<"   ia2="<<ia2<<"   ia3="<<ia3;
                        QVector<double> ia=binfileReadDoubleArrayV(f, ia1*ia2*ia3);  //  ImageArray        =  fread(fid, [ImageArrayLength(3)*ImageArrayLength(2), ImageArrayLength(1)], 'double');%remove tag (before the dot-coma)
                    }

                }
            } else {
                setError(tr("error reading Oleg file '%1': unknown version %2").arg(filename).arg(version));
                return false;
            }

            //qDebug()<<"runs="<<runs<<"   cntRuns="<<cntRuns;
            //qDebug()<<"corr.size()="<<corr.size()<<"   lags.size()="<<lags.size()<<"   cnt.size()="<<cnt.size();

            int csize=lags.size();
            // cut off back part of CFs
            bool dontUse=false;
            while (csize>2 && ( dontUse || lags[csize-1]==0.0 || lags[csize-1]==-1.0 || !QFFloatIsOK(lags[csize-1]) || (runtime>0 && lags[csize-1]>runtime))) {
                dontUse=true;
                for (uint32_t r=0; r<runs; r++) {
                    const double cv=corr.value(r*lags.size()+csize-1);
                    if (cv!=0.0 && cv!=-1.0) {
                        dontUse=false;
                        break;
                    }
                }
                csize--;
            }
            int cutfront=0;
            while (cutfront<csize && lags[cutfront]==0) {
                cutfront++;
            }

            csize=csize-cutfront;
            //qDebug()<<"cutfront="<<cutfront<<"   csize="<<csize;

            if (csize>0) {
                resizeCorrelations(csize, runs);
                for (int t=0; t<correlationN; t++) {
                    correlationT[t]=lags.value(cutfront+t, 0);
                }
                long nn=0;
                for (uint32_t r=0; r<runs; r++) {
                    for (int t=0; t<correlationN; t++) {
                        correlation[nn]=corr.value(r*lags.size()+cutfront+t, 0);
                        nn++;
                    }
                }

                resizeRates(cntN,cntRuns, 1);
                if (cntN>0 && cntRuns>0) {
                    for (int t=0; t<rateN; t++) {
                        rateT[t]=double(t)*runtime/double(cntN);
                    }
                    nn=0;
                    for (int32_t r=0; r<rateRuns; r++) {
                        for (int t=0; t<rateN; t++) {
                            rate[nn]=cnt.value(nn, 0)/1000.0;
                            nn++;
                        }
                    }
                }

                if (!getDescription().contains(logbook))  setDescription(getDescription()+QString("\n\n\nlogbook = ")+logbook);

                setQFProperty("file_version", version, false, true);
                setQFProperty("AIClockDivider", AIClockDivider, false, true);
                setQFProperty("dt", dt, false, true);
                setQFProperty("speed", speed, false, true);
                setQFProperty("runtime", runtime, false, true);
            } else {
                setError(tr("error reading Oleg file '%1': no correlation channels found").arg(filename));
                return false;
            }

            f.close();
        }
    //}
    recalculateCorrelations();
    emitRawDataChanged();

    return ok;
}

bool QFRDRFCSData::loadCorrelationCurvesFromPicoQuantCOR(QString filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }

    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        QTextStream txt(&f);
        QList<QVector<double> > corr;
        while (!txt.atEnd()) {
            QString line;
            QVector<double> data=csvReadline(txt, ' ', '#', 0, "\n", "\r", &line);
            bool added=false;
            if (data.size()>=3 && QFFloatIsOK(data[0]) && data[0]>0 && QFFloatIsOK(data[1])) {
                //data.removeFirst();
                corr<<data;
                added=true;
                //qDebug()<<1<<data;
            } else {
                QRegExp rxc("#?([^\\:]+)\\s*\\:\\s*(.+)");
                //qDebug()<<2<<line;
                if (rxc.exactMatch(line) && rxc.cap(1).trimmed().size()>0 && rxc.cap(2).trimmed().size()>0) {
                    QString name=rxc.cap(1).trimmed();
                    QString value=rxc.cap(2).trimmed();
                    setQFProperty(name, qfStringToVariantAutoRecognizeType(value), false, true);
                    //qDebug()<<2<<name<<value<<qfStringToVariantAutoRecognizeType(value);
                }
            }
        }
        f.close();
        if (corr.size()>0) {
            //double taumin=corr[0].at(0)*1e-3;
            resizeCorrelations(corr.size(), 1);
            //qDebug()<<"correlationN="<<correlationN;
            bool cok=false;
            int col=getQFProperty("COLUMNS", "2").toInt(&cok);
            if (!cok) col=2;
            int taucol=1;
            for (int i=0; i<correlationN; i++) {
                if (taucol<corr[i].size()) {
                    correlationT[i]=corr[i].at(taucol);
                }
                if (col<corr[i].size()) {
                    correlation[i]=corr[i].at(col);
                    //if (weights && corr[i].size()>=3) correlationErrors[i]=1/corr[i].at(2);
                } else {
                    correlation[i]=0;
                }
            }
            recalculateCorrelations();
            emitRawDataChanged();
            return true;
        } else {
            setError(tr("Error while reading correlation functions from PicoQuant ASCII file '%1': %2").arg(filename).arg(tr("No valid data found in file")));
            return false;
        }
    } else {
        setError(tr("Error while reading correlation functions from PicoQuant ASCII file '%1': %2").arg(filename).arg(f.errorString()));
        return false;
    }

    return false;
}

bool QFRDRFCSData::analyzeCorrelationCurvesFromPicoQuantCOR(QString filename, int *channels)
{
    if (!QFile::exists(filename)) {
        //qDebug()<<"file does not exist";
        return false;
    }


    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        QTextStream txt(&f);
        while (!txt.atEnd()) {
            QString line;
            QVector<double> data=csvReadline(txt, ' ', '#', 0, "\n", "\r", &line);
            //qDebug()<<line;
            if (data.size()>=3 && QFFloatIsOK(data[0]) && data[0]>0 && QFFloatIsOK(data[1])) {
                //qDebug()<< "  -> "<<data<< "  -> chanels ="<<data.size()-2;
                if (channels) *channels=data.size()-2;
                f.close();
                return true;
            } else {
//                QRegExp rxc("#?(.+)[\\=|\\:](.+)");
//                qDebug()<<2<<line;
//                if (rxc.exactMatch(line) && rxc.cap(1).trimmed().size()>0 && rxc.cap(2).trimmed().size()>0) {
//                    QString name=rxc.cap(1).trimmed();
//                    QString value=rxc.cap(2).trimmed();
//                    setQFProperty(name, qfStringToVariantAutoRecognizeType(value), false, true);
//                    qDebug()<<2<<name<<value<<qfStringToVariantAutoRecognizeType(value);
//                }
            }
            //qDebug()<<txt.atEnd()<<f.atEnd();
        }
        f.close();
    }
    return false;
}

bool QFRDRFCSData::loadCorrelationCurvesFromPicoQuantASCIIFCS(QString filename, bool weights)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }

    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        QTextStream txt(&f);
        QList<QVector<double> > corr;
        while (!txt.atEnd()) {
            QVector<double> data=csvReadline(txt, ' ', '#', 0, "\n", "\r");
            bool added=false;
            if (data.size()>=2 && QFFloatIsOK(data[0]) && data[0]>0 && QFFloatIsOK(data[1])) {
                QVector<double> d;
                d<<data[0]<<data[1];
                corr<<d;
                added=true;
            }
            if (weights && data.size()>=3 && QFFloatIsOK(data[2]) && data[2]>0 && added) {
                corr.last().append(data[2]);
            }
            //if (added) qDebug()<<corr.last();
        }
        f.close();
        if (corr.size()>0) {
            //double taumin=corr[0].at(0)*1e-3;
            resizeCorrelations(corr.size(), 1);
            //qDebug()<<"correlationN="<<correlationN;
            for (int i=0; i<correlationN; i++) {
                if (corr[i].size()>=2) {
                    correlationT[i]=(round(corr[i].at(0)*1e6)/1e6)*1e-3;
                    correlation[i]=corr[i].at(1);
                    if (weights && corr[i].size()>=3) correlationErrors[i]=1/corr[i].at(2);
                }
            }
            recalculateCorrelations();
            emitRawDataChanged();
            return true;
        } else {
            setError(tr("Error while reading correlation functions from PicoQuant ASCII file '%1': %2").arg(filename).arg(tr("No valid data found in file")));
            return false;
        }
    } else {
        setError(tr("Error while reading correlation functions from PicoQuant ASCII file '%1': %2").arg(filename).arg(f.errorString()));
        return false;
    }

    return false;

}

bool QFRDRFCSData::loadCorrelationCurvesFromPicoQuantASCIIFCSWeights(QString filenames)
{
    return loadCorrelationCurvesFromPicoQuantASCIIFCS(filenames, true);
}

bool QFRDRFCSData::loadCorrelationCurvesFromPicoQuantASCIIFCSNoWeights(QString filenames)
{
    return loadCorrelationCurvesFromPicoQuantASCIIFCS(filenames, false);
}

//    datatable2 tab;

//        try {
//            tab.load_csv(d.filename.toStdString(), ' ' , '#', startswith, endswith, firstline);        // load some csv file
//            long long lines=tab.get_line_count();
//            long long columns=tab.get_column_count();
//            //qDebug()<<"lines="<<lines<<"   columns="<<columns<<QString::number(separatorchar);
//            QFPluginServices::getInstance()->log_text(tr("  read CSV file '%1' as FCS record.\n  Found %2 lines and %3 columns.\n").arg(d.filename).arg(lines).arg(columns));
//            if (mode==0) { // tau, corr, corr, ...
//                rruns=rruns+columns-1;
//            } else if (mode==1) { // tau, corr, error, corr, error, ...
//                rruns=rruns+(columns-1)/2;
//            }
//            ccorrN=qMax(lines, ccorrN);
//        } catch(datatable2_exception& e) {   // error handling with exceptions
//            setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(d.filename).arg(QString(e.get_message().c_str())));
//            error=true;
//            break;
//        }
//        data.append(d);
//        if (error) break;
//    }
//    if (!error) {
//        resizeCorrelations(ccorrN, rruns);
//        int run0=0;
//        for (int ii=0; ii<filenames.size(); ii++) {
//            try {
//                long long lines=data[ii].tab->get_line_count();
//                long long columns=data[ii].tab->get_column_count();
//                int runincrement=(columns-1);

//                for (long long l=0; l<lines; l++) {
//                    if (ii==0) correlationT[l]=data[ii].tab->get(0, l)*timefactor;
//                    else if (correlationT[l]!=data[ii].tab->get(0, l)*timefactor) {
//                        setError(tr("Error while reading correlation fucntions from CSV file '%1': time does not match time in file '%2'").arg(data[ii].filename).arg(data[0].filename));
//                        error=true;
//                        break;
//                    }
//                    if (mode==0) { // tau, corr, corr, ...
//                        for (int c=1; c<columns; c++) {
//                            correlation[(run0+c-1)*correlationN+l]=data[ii].tab->get(c, l);
//                        }
//                        runincrement=(columns-1);
//                    } else if (mode==1) { // tau, corr, error, corr, error, ...
//                        for (int c=1; c<columns; c+=2) {
//                            if (c<columns) {
//                                correlation[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c, l);
//                            }
//                            if (c+1<columns) {
//                                correlationErrors[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c+1, l);
//                            } else {
//                                correlationErrors[(run0+(c-1)/2)*correlationN+l]=0;
//                            }
//                        }
//                        runincrement=(columns-1)/2;
//                    }
//                }
//                run0=run0+runincrement;

//            } catch(datatable2_exception& e) {   // error handling with exceptions
//                setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(data[ii].filename).arg(QString(e.get_message().c_str())));
//                error=true;
//                break;
//            }
//            if (error) break;
//        }
//    }

//    for (int ii=0; ii<data.size(); ii++) {
//        delete data[ii].tab;
//    }
//    data.clear();

//    if (!error) {
//        recalculateCorrelations();
//        emitRawDataChanged();
//        return true;
//    } else {
//        return false;
//    }


bool QFRDRFCSData::loadCorrelatorComSIN(QString filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }
    bool ok=true;

    try {
        FLEX_DATA data;
        FLEX_readFile(filename, data);

        int channel=getProperty("CHANNEL", 0).toInt();
        int col=getProperty("FLEX_CF_COLUMN", channel).toInt();
        resizeCorrelations(data.corr_tau.size(),data.runCount);
        for (int t=0; t<correlationN; t++) {
            correlationT[t]=data.corr_tau.value(t, 0);
        }
        long nn=0;
        for (uint32_t r=0; r<data.runCount; r++) {
            for (int t=0; t<correlationN; t++) {
                correlation[nn]=data.corrs[col].value(t, 0)-1.0;
                nn++;
            }
        }
        resizeRates(data.counts_time.size(), data.runCount, data.channelCount);
        for (int t=0; t<rateN; t++) {
            rateT[t]=data.counts_time.value(t, 0);
        }
        nn=0;
        for (int32_t c=0; c<rateChannels; c++) {
            for (int32_t r=0; r<rateRuns; r++) {
                for (int t=0; t<rateN; t++) {
                    rate[nn]=data.counts[c].value(t, 0)/1000.0;
                    nn++;
                }
            }
        }

        QMapIterator<QString, QVariant> it(data.properties);
        while (it.hasNext()) {
            it.next();
            setQFProperty(it.key(), it.value(), false, true);
        }
    } catch (std::exception& E) {
        setError(tr("Error while reading correlation functions from Correlator.com SIN file '%1': %2").arg(filename).arg(QString(E.what())));
        ok=false;
        return ok;
    }

    recalculateCorrelations();
    emitRawDataChanged();

    return ok;
}

struct QFRDRFCSData_confocor3Data {
    int cntRec1;
    int cntRec2;
    int index;
    QFRDRFCSData_confocor3Data() {
        cntRec1=-1;
        cntRec2=-1;
        index=-1;
    }
};

bool QFRDRFCSData::loadConfocor3(QString filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }
    bool ok=true;
    Confocor3Tools reader;
    reader.loadFile(filename);
    if (reader.wasError()) {
        ok=false;
        setError(tr("Error while importing ConfoCor2/3 file '%1':\n    %2\n").arg(filename).arg(reader.getLastErrors().join("\n")));
        return ok;
    } else {
        const Confocor3Tools::ConfocorDataset& d=reader.getData();
        QStringList items_str=getProperty("CONFOCOR3_ITEMS", "").toString().split(',');
        QList<int> items;
        for (int i=0; i<items_str.size(); i++) {
            bool ok=false;
            int jj=items_str[i].trimmed().simplified().toInt(&ok);
            if (ok && (jj>=0) && (jj<d.fcsdatasets.size())) items.append(jj);
        }
        QString role=getProperty("CONFOCOR3_ROLE", "").toString();
        QString group=getProperty("CONFOCOR3_GROUP", "").toString();

        int64_t Nc=INT_MAX, Nr=INT_MAX, runs=0, channels=1;
        QList<QFRDRFCSData_confocor3Data> importData;
        bool chSwap=false;
        if (items.size()>0) {
            for (int i=0; i<items.size(); i++) {
                const Confocor3Tools::FCSDataSet& f=d.fcsdatasets[items[i]];
                if (f.role==role && f.group==group) {
                    if (f.tau.size()>0) Nc=qMin(Nc, (int64_t)f.tau.size());
                    int cntRec1=-1;
                    int cntRec2=-1;
                    if (f.type==Confocor3Tools::fdtCCF) {
                        if (f.recCnt1>=0) {
                            cntRec1=f.recCnt1;
                        }
                        if (f.recCnt2>=0) {
                            cntRec2=f.recCnt2;
                        }
                    } else if (f.type==Confocor3Tools::fdtACF) {
                        cntRec1=items[i];
                        if (f.recCnt1>=0) {
                            cntRec1=f.recCnt1;
                            cntRec2=items[i];
                        } else if (f.recCnt2>=0) {
                            cntRec1=items[i];
                            cntRec2=f.recCnt2;
                        }
                    }
                    if (f.tau.size()>0 && f.corr.size()>0 && f.corr.first().size()>0) {
                        if (cntRec1>=0 && cntRec1<d.fcsdatasets.size() && d.fcsdatasets[cntRec1].time.size()>0) Nr=qMin(Nr, (int64_t)d.fcsdatasets[cntRec1].time.size());
                        if (cntRec2>=0 && cntRec2<d.fcsdatasets.size() && d.fcsdatasets[cntRec2].time.size()>0) Nr=qMin(Nr, (int64_t)d.fcsdatasets[cntRec2].time.size());
                        if (cntRec1>=0 && cntRec2>=0) channels=2;
                        runs++;
                        QFRDRFCSData_confocor3Data dat;
                        dat.cntRec1=cntRec1;
                        dat.cntRec2=cntRec2;
                        dat.index=items[i];
                        importData.append(dat);
                        chSwap=chSwap||f.reverseFCCS;
                    }
                }
            }
            resizeCorrelations(Nc, runs);
            resizeRates(Nc, runs, channels);
            setQFProperty("CHANNELS_SWAPPED", chSwap, false, true);
            if (importData.size()>0) {
                for (int r=0; r<importData.size(); r++) {
                    const Confocor3Tools::FCSDataSet& f=d.fcsdatasets[importData[r].index];
                    setQFProperty("CHANNEL_NAME", f.channel, false, true);
                    setQFProperty("POSITION", f.position, false, true);
                    setQFProperty("KINETIC", f.kinetic, false, true);
                    setQFProperty("ACQUISITION_TIME", f.acqtime, false, true);
                    setQFProperty("RAWDATA_FILE", f.rawdata, false, true);
                    QMapIterator<QString, QVariant> it(f.props);
                    while (it.hasNext()) {
                        it.next();
                        setQFProperty(it.key().toUpper(), it.value(), false, true);
                    }

                    if (r==0) {
                        for (int i=0; i<qMin((int64_t)correlationN, Nc); i++) correlationT[i]=f.tau.value(i, 0.0);
                    } else {
                        bool okT=true;
                        for (int i=0; i<qMin((int64_t)correlationN, Nc); i++) {if (fabs(correlationT[i]-f.tau.value(i, 0.0))>1e-3*correlationT[i]) {okT=false; break;}}
                        if (!okT) {
                            ok=false;
                            setError(tr("Error while importing ConfoCor2/3 file '%1':\n    lag-time-axis in different runs is unequal in record %2 (group=%3, role=%4)!.\n").arg(filename).arg(importData[r].index).arg(group).arg(role));
                            return ok;
                        }
                    }
                    double* corr=getCorrelationRun(r);
                    if (f.corr.size()>0 && f.corr.first().size()>=Nc) {
                        for (int i=0; i<qMin((int64_t)correlationN, Nc); i++) {
                            corr[i]=f.corr.first().value(i, 0.0)-1.0;
                        }
                    } else {
                        ok=false;
                        setError(tr("Error while importing ConfoCor2/3 file '%1':\n    no correlation data found in record %2 (group=%3, role=%4)!.\n").arg(filename).arg(importData[r].index).arg(group).arg(role));
                        return ok;

                    }

                    if (r==0) {
                        for (int i=0; i<qMin((int64_t)rateN, Nr); i++) rateT[i]=f.time.value(i, 0.0);
                    } else {
                        bool okT=true;
                        for (int i=0; i<qMin((int64_t)rateN, Nr); i++) {if (fabs(rateT[i]-f.time.value(i, 0.0))>1e-3*rateT[i]) {okT=false; break;}}
                        if (!okT) {
                            ok=false;
                            setError(tr("Error while importing ConfoCor2/3 file '%1':\n    time-axis in different runs is unequal in record %2 (group=%3, role=%4)!.\n").arg(filename).arg(importData[r].index).arg(group).arg(role));
                            return ok;
                        }
                    }
                    if (importData[r].cntRec1>=0) {
                        double* rat0=getRateRun(r,0);
                        const Confocor3Tools::FCSDataSet& fr1=d.fcsdatasets[importData[r].cntRec1];
                        if (fr1.rate.size()>0 && Nr<=fr1.rate.first().size()){
                            for (int i=0; i<qMin((int64_t)rateN, Nr); i++) {
                                rat0[i]=fr1.rate.first().value(i, 0)/1000.0;
                            }
                        }
                    }
                    if (channels>1 && importData[r].cntRec2>=0) {
                        double* rat1=getRateRun(r, 1);
                        const Confocor3Tools::FCSDataSet& fr1=d.fcsdatasets[importData[r].cntRec2];
                        if (fr1.rate.size()>0 && Nr<=fr1.rate.first().size()){
                            for (int i=0; i<qMin((int64_t)rateN, Nr); i++) {
                                rat1[i]=fr1.rate.first().value(i, 0)/1000.0;
                            }
                        }
                    }


                }
            } else {
                ok=false;
                setError(tr("Error while importing ConfoCor2/3 file '%1':\n    no items selected to import (group=%1, role=%2).\n").arg(filename).arg(group).arg(role));
                return ok;
            }
        } else {
            ok=false;
            setError(tr("Error while importing ConfoCor2/3 file '%1':\n    no items selected to import.\n").arg(filename));
            return ok;
        }
    }

    recalculateCorrelations();
    emitRawDataChanged();
    return ok;
}

bool QFRDRFCSData::loadQF3ASCII(QString filename)
{
    if (!QFile::exists(filename)) {
        QString msg;
        setError(msg=tr("data file '%1' does not exist!\n").arg(filename));
        QFPluginServices::getInstance()->log_warning(msg);
        return false;
    }

    bool ok=true;
    QF3CorrelationDataFormatTool reader;
    reader.loadFile(filename);
    if (reader.wasError()) {
        ok=false;
        setError(tr("Error while importing QF3 ASCII file '%1':\n    %2\n").arg(filename).arg(reader.getErrorMessage()));
        return ok;
    } else {
        int corrT=getProperty("QF3ASCII_CORRELATION_TYPE", 0).toInt();
        QMapIterator<QString, QVariant> it(reader.properties);
        while (it.hasNext()) {
            it.next();
            if (it.key().toLower()!="channel") setQFProperty(it.key(), it.value(), false, true);
        }

        resizeCorrelations(reader.taus.size(), reader.runs);
        resizeRates(reader.times.size(), reader.rateRuns, reader.channels);

        //qDebug()<<filename<<corrT<<correlationN<<correlationRuns<<rateN<<rateRuns<<rateChannels;

        if (correlationN>0 && correlation) {
            for (int i=0; i<correlationN; i++) {
                correlationT[i]=reader.taus.value(i,NAN);
            }
            for (int r=0; r<correlationRuns; r++) {
                double* cr=getCorrelationRun(r);
                for (int i=0; i<correlationN; i++) {
                    cr[i]=reader.getCorrelationEntry(i, r, corrT);
                }
            }
        }

        if (rateN>0 && rate) {
            for (int i=0; i<rateN; i++) {
                rateT[i]=reader.times.value(i,NAN);
            }
            for (int c=0; c<rateChannels; c++) {
                for (int r=0; r<rateRuns; r++) {
                    double* cr=getRateRun(r, c);
                    for (int i=0; i<rateN; i++) {
                        cr[i]=reader.getCountrateEntry(i, r, c);
                    }
                }
            }
        }
    }

    recalculateCorrelations();
    emitRawDataChanged();
    return ok;
}





