#include "qfrdrfcsdata.h"
#include <errno.h>
#include <QtXml>
#include "jkiniparser2.h"

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

    autoCalcRateN=200;
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;

    leaveout.clear();
}

QFRDRFCSData::~QFRDRFCSData()
{
    if (correlationT) free(correlationT);
    if (correlation) free(correlation);
    if (correlationErrors) free(correlationErrors);
    if (correlationMean) free(correlationMean);
    if (correlationStdDev) free(correlationStdDev);
    if (rateT) free(rateT);
    if (rate) free(rate);
    if (binnedRateT) free(binnedRateT);
    if (binnedRate) free(binnedRate);
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
    if (correlationT) free(correlationT);
    if (correlation) free(correlation);
    if (correlationMean) free(correlationMean);
    if (correlationStdDev) free(correlationStdDev);
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
    correlationT=(double*)calloc(correlationN, sizeof(double));
    correlationMean=(double*)calloc(correlationN, sizeof(double));
    correlationStdDev=(double*)calloc(correlationN, sizeof(double));
    correlation=(double*)calloc(correlationN*correlationRuns, sizeof(double));
    correlationErrors=(double*)calloc(correlationN*correlationRuns, sizeof(double));
    if (!correlationT || !correlation || !correlationMean || !correlationStdDev || !correlationErrors)
        setError(tr("Error while allocating memory for correlation function data!"));
    emitRawDataChanged();
}

void QFRDRFCSData::resizeRates(long long N, int runs, int channels) {
    //qDebug()<<"resizeRates( N="<<N<<",  runs="<<runs<<",  channels="<<channels<<")";
    if (rateT) free(rateT);
    if (rate) free(rate);
    rateRuns=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;
    rateN=N;
    rateRuns=runs;
    rateChannels=channels;
    rateT=(double*)calloc(rateN, sizeof(double));
    rate=(double*)calloc(rateChannels*rateN*rateRuns, sizeof(double));
    if (!rateT || !rate)
        setError(tr("Error while allocating memory for count rate data!"));
    emitRawDataChanged();
    rateMean.clear();
    rateStdDev.clear();
    rateMin.clear();
    rateMax.clear();
}

void QFRDRFCSData::resizeBinnedRates(long long N) {
    //qDebug()<<"resizeBinnedRates( N="<<N<<"),  rateChannels="<<rateChannels<<",  rateRuns="<<rateRuns;
    if (binnedRateT) free(binnedRateT);
    if (binnedRate) free(binnedRate);
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;
    binnedRateN=N;
    if (N>0) {
        binnedRateT=(double*)calloc(binnedRateN, sizeof(double));
        binnedRate=(double*)calloc(rateChannels*binnedRateN*rateRuns, sizeof(double));
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
    QString filter=tr("All Files (*.*)");
    QString filetype=getProperty("FILETYPE", "unknown").toString().toUpper();
    if (filetype=="ALV5000") filter=tr("ALV-5000 file (*.asc)");
    if (filetype=="ALV6000") filter=tr("ALV-6000 file (*.asc)");
    if (filetype=="CSV_CORR") filter=tr("ASCII Data Files (*.txt *.dat *.csv)");
    if (filetype=="ISS_ALBA") filter=tr("ISS Alba Files (*.csv)");
    if (filetype=="DIFFUSION4_SIMRESULTS") filter=tr("Diffusion4 simulation results (*corr.dat *bts.dat)");


    if (filetype!="INTERNAL") {
        files = qfGetOpenFileNames(NULL,
                              tr("Select FCS Data File(s) to Import ..."),
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
/*    if (!datamodel) return;
    QString f=format.toUpper();
    if (f=="CSV") {
        datamodel->saveCSV(filename);
    } else if (f=="SLK" || f=="SYLK") {
        datamodel->saveSYLK(filename);
    }*/
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


void QFRDRFCSData::intWriteData(QXmlStreamWriter& w) {
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
    QString filetype=getProperty("FILETYPE", "unknown").toString().toUpper();
    if (filetype.toUpper()=="INTERNAL") {
        loadInternal(e);
    } else {
        reloadFromFiles();
    }

    //std::cout<<"intReadData ended\n";
}

struct CSVDATA {
    datatable2* tab;
    QString filename;
};

bool QFRDRFCSData::loadCountRatesFromCSV(QStringList filenames, int rateChannels) {
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
    if (d.size()>0) separatorchar=d[0];
    d=getProperty("CSV_COMMENT", "#").toString().toStdString();
    if (d.size()>0) commentchar=d[0];

    long long ccorrN=0;
    long long rruns=0;
    bool error=false;

    for (int ii=0; ii<filenames.size(); ii++) {
        CSVDATA d;
        d.filename=filenames[ii];

        try {
            d.tab=new datatable2();
            d.tab->load_csv(d.filename.toStdString(), separatorchar, commentchar, startswith, endswith, firstline);        // load some csv file
            long long lines=d.tab->get_line_count();
            long long columns=d.tab->get_column_count();
            if (mode==0) { // tau, corr, corr, ...
                rruns=rruns+columns-1;
            } else if (mode==1) { // tau, corr, error, corr, error, ...
                rruns=rruns+(columns-1)/2;
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
                            if (c+1<columns) {
                                correlation[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c, l);
                                correlationErrors[(run0+(c-1)/2)*correlationN+l]=data[ii].tab->get(c+1, l);
                            }
                        }
                        runincrement=(columns-1)/2;
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
            } while ((!line.isNull()) && (!line.contains("[data]")));
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
        data.append(init);
        data[i].filename=filenames[i];
        data[i].alv_file=fopen(data[i].filename.toAscii().data(), "r");

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
                            QString propN="ANGLE [°]";
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
                    correlationT[i]=d[0]/1000.0;
                } else {
                    if (correlationT[i]!=d[0]/1000.0) {
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
                        /*if (data[ii].isDual&&channel==1) {
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
        data.append(init);
        data[i].filename=filenames[i];
        data[i].alv_file=fopen(data[i].filename.toAscii().data(), "r");

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
                            QString propN="ANGLE [°]";
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
                    correlationT[i]=d[0]/1000.0;
                } else {
                    if (correlationT[i]!=d[0]/1000.0) {
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
                            qDebug()<<"swapping in "<<getName();
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
                            qDebug()<<"swapping in "<<getName();
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
}


QString QFRDRFCSData::getCorrelationRunName(int run) const {
    //if (run<0) return tr("average");
    if (run<correlationRuns) return tr("run %1").arg(run);
    return QString("");
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
    if (propertyExists("CHANNEL")) channel=getProperty("CHANNEL", 0).toInt();
    //qDebug()<<"channelsSwapped(): "<<getName()<<channel<<rateChannels;
    if (rateChannels==2 && channel==1) return true;
    return false;
}

void QFRDRFCSData::saveInternal(QXmlStreamWriter& w) const {
    w.writeStartElement("internal_correlations");
    QString csv;
    for (int i=0; i<correlationN; i++) {
        csv=csv+CDoubleToQString(correlationT[i]);
        for (int j=0; j<correlationRuns; j++) {
            csv=csv+", "+CDoubleToQString(correlation[j*correlationN+i]);
        }
        csv=csv+"\n";
    }
    w.writeAttribute("mode", getProperty("INTERNAL_CSVMODE", "tccc").toString().toLower());
    w.writeCDATA(csv);
    w.writeEndElement();

}

bool QFRDRFCSData::loadInternal(QDomElement* e) {
    QString csv=getProperty("INTERNAL_CSV", "").toString();
    QString mode=getProperty("INTERNAL_CSVMODE", "tccc").toString().toLower();
    //qDebug()<<csv;
    deleteProperty("INTERNAL_CSV");
    if (e && csv.isEmpty()) {
        QDomElement de=e->firstChildElement("internal_correlations");
        if (!de.isNull()) {
            mode=de.attribute("mode", mode).toLower();
            csv=de.text();
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
        return true;
    }
    return false;
}

bool QFRDRFCSData::mayDeleteFiles(QStringList &files, QStringList &types, QStringList &descriptions) const
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
    } else if (filetype.toUpper()=="ISS_ALBA") {
        if (files.size()<=0) {
            setError(tr("there are no files in the FCS record!"));
            return false;
        }
        return loadCorrelationCurvesFromALBA(files);
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
    sl<<"ACF0"<<"ACF1"<<"ACF2"<<"ACF3"<<"FCCS";
    /*sl<<"correlation";
    sl<<"acf";
    sl<<"ccf";
    sl<<"dccf";
    sl<<"fcs_red";
    sl<<"fcs_green";
    sl<<"fcs0";
    sl<<"fcs1";
    sl<<"fccs";
    sl<<"ccf(-1,0)";
    sl<<"ccf(1,0)";
    sl<<"ccf(0,-1)";
    sl<<"ccf(0,1)";*/
    return sl;
}

bool QFRDRFCSData::isRoleUserEditable() const
{
    return  false;
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






