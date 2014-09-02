/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfrdrphotoncounts_data.h"
#include <QtXml>
#include "datatable2.h"
#include "tools.h"
#include "qfrdrphotoncounts_dataeditor.h"
#include "binarydatatools.h"

QFRDRPhotonCountsData::QFRDRPhotonCountsData(QFProject* parent):
    QFRawDataRecord(parent)
{
    averageT=averageBinnedT=1.0;

    rateChannels=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;

    autoCalcRateN=1000;
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;
}

QFRDRPhotonCountsData::~QFRDRPhotonCountsData()
{
    if (rateT) free(rateT);
    if (rate) free(rate);
    if (binnedRateT) free(binnedRateT);
    if (binnedRate) free(binnedRate);

    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;

    rateChannels=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;

}

QFRawDataEditor *QFRDRPhotonCountsData::createEditor(QFPluginServices *services,  QFRawDataPropertyEditor *propEditor, int i, QWidget *parent)
{
    if (i==0) return new QFRDRPhotonCountsDataEditor(services, propEditor, parent);
    return NULL;
}

void QFRDRPhotonCountsData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
    // THIS IS OPTIONAL
}

QString QFRDRPhotonCountsData::getPhotonCountsChannelName(int channel) {
    return channelName.value(channel, tr("channel %1 in file").arg(channel+1));
}

int QFRDRPhotonCountsData::getPhotonCountsChannels()
{
    return rateChannels;
}


void QFRDRPhotonCountsData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*        
	w.writeStartElement("mytag");
	w.writeAttribute("data", "123456789");
	w.writeEndElement();
    */
}

void QFRDRPhotonCountsData::intReadData(QDomElement* e) {

    autoCalcRateN=getProperty("AUTO_BINNED_RATE_N", autoCalcRateN).toInt();

    QString filetype=getProperty("FILETYPE", "unknown").toString();
    //std::cout<<"reading data "<<filetype.toStdString()<<" from 1/"<<files.size()<<" '"<<files.join(", ").toStdString()<<"'\n";

    rateMean.clear();
    rateStdDev.clear();
    rateMin.clear();
    rateMax.clear();
    channelName.clear();

    if (filetype.toUpper()=="CSV") {
        if (files.size()<=0) {
            setError(tr("there are no files in the COUNT RATES record!"));
            return;
        }
        loadCountRatesFromCSV(files[0]);
    } else if (filetype.toUpper()=="BINARY") {
        if (files.size()<=0) {
            setError(tr("there are no files in the COUNT RATES record!"));
            return;
        }
        loadCountRatesFromBinary(files[0]);
    }
}



bool QFRDRPhotonCountsData::loadCountRatesFromCSV(QString filename) {
    char separatorchar=',';
    char commentchar='#';
    std::string d=getProperty("CSV_SEPARATOR", ",").toString().toStdString();
    std::string startswith=getProperty("CSV_STARTSWITH", "").toString().toStdString();
    std::string endswith=getProperty("CSV_ENDSWITH", "").toString().toStdString();
    double timefactor=getProperty("CSV_TIMEFACTOR", 1.0).toDouble();
    double ratefactor=getProperty("CSV_RATEFACTOR", 1.0).toDouble();
    int firstline=getProperty("CSV_FIRSTLINE", 1).toInt();
    if (d.size()>0) separatorchar=d[0];
    d=getProperty("CSV_COMMENT", ",").toString().toStdString();
    if (d.size()>0) commentchar=d[0];
    try {
        datatable2 tab;                   // instanciate
        //std::cout<<"opening CSV: "<<filename.toStdString()<<std::endl;
        tab.load_csv(filename.toStdString(), separatorchar, commentchar, startswith,endswith, firstline);        // load some csv file
        long long lines=tab.get_line_count();
        long long columns=tab.get_column_count();
        resizeRates(lines, columns-1);
        //std::cout<<"resized correlation to: N="<<lines<<", channels="<<columns-1<<std::endl;
        averageT=0;
        for (long long l=0; l<lines; l++) {
            rateT[l]=tab.get(0, l)*timefactor;
            if (l>0) averageT=averageT+fabs(rateT[l]-rateT[l-1]);
            //std::cout<<correlationT[l]<<", ";
            for (int c=1; c<columns; c++) {
                rate[(c-1)*rateN+l]=tab.get(c, l)*ratefactor;
                //std::cout<<correlation[(c-1)*correlationN+l]<<", ";
                //std::cout<<"c="<<c<<std::endl;
            }
            //std::cout<<std::endl;
        }
        averageT=averageT/double(lines-1);
        //std::cout<<"recalculating correlations ... ";
        //std::cout<<"DONE!\n";
    } catch(datatable2_exception& e) {   // error handling with exceptions
        setError(tr("Error while reading correlation functions from CSV file '%1': %2").arg(filename).arg(QString(e.get_message().c_str())));
    }
    //std::cout<<"opening CSV: "<<filename.toStdString()<<" ... DONE!\n";
    emitRawDataChanged();
    if (rateN<1000) autoCalcRateN=rateN;
    calcPhotonCountsBinned();
    return true;
}

bool QFRDRPhotonCountsData::loadCountRatesFromBinary(QString filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        QByteArray intro=f.read(10);
        if (intro=="QF3.0CNTRT") {
            uint16_t channels=binfileReadUint16(f);
            uint64_t itemsPerChannel=binfileReadUint64(f);
            double deltaT=binfileReadDouble(f);

            channelName.clear();
            for (uint16_t i=0; i<channels; i++) {
                channelName[i]=tr("channel %1").arg(binfileReadUint16(f));
            }

            resizeRates(itemsPerChannel, channels);
            averageT=deltaT;
            uint32_t chunkSize=qMin(itemsPerChannel, (uint64_t)1024)*channels;
            uint16_t* temp=(uint16_t*)malloc(chunkSize*sizeof(uint16_t));

            for (register uint64_t i=0; i<itemsPerChannel; i++) {
                rateT[i]=double(i)*deltaT;
            }
            for (register uint64_t i=0; i<itemsPerChannel*channels; i=i+chunkSize) {
                uint32_t chunksize_local=chunkSize;
                if (i+chunkSize>itemsPerChannel*channels) {
                    chunksize_local=(itemsPerChannel*channels-i);
                }
                binfileReadUInt16Array(f, temp, chunksize_local);
                for (register uint32_t j=0; j<chunksize_local; j=j+1) {
                    uint32_t channel=j%channels;
                    uint32_t idx=(i+j)/channels;
                    rate[channel*itemsPerChannel+idx]=temp[j];
                }
                /*for (register uint16_t j=0; j<channels; j++) {
                    uint16_t d=binfileReadUint16(f);
                    rate[j*itemsPerChannel+i]=d;
                }*/
                if (f.atEnd()) break;
            }
            free(temp);

            emitRawDataChanged();
            if (rateN<1000) autoCalcRateN=rateN;
            calcPhotonCountsBinned();
            return true;

        } else {
            setError(tr("Error reading binary photoncounts file '%1': wrong file intro found ('%1' instead of 'QF3.0CNTRT'").arg(filename).arg(QString(intro)));
            return false;
        }
    }
    return false;
}


double QFRDRPhotonCountsData::calcPhotonCountsMean(int channel) {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return 0.0;
    }
    double m=0.0;
    if (channel<rateChannels) {
        for (long long i=0; i<rateN; i++) {
            m=m+rate[channel*rateN+i];
        }
    }
    double me= m/(double)rateN;
    rateMean[channel]=me;
    return me;
}



double QFRDRPhotonCountsData::getPhotonCountsMean(int channel, QFRDRPhotonCountsInterface::PhotonCountsUnits unit) {
    if (rateN<=0 || !rate) {
        return 0.0;
    }
    if (channel<0) {
        double cnt=0;
        double sum=0;
        for (int i=0; i<rateN; i++) {
            double r=getPhotonCountsMean(i, unit);
            if (r>0) {
                sum=sum+r;
                cnt++;
            }
        }
        return sum/cnt;
    }
    if (!rateMean.contains(channel)) {
        rateMean[channel]=calcPhotonCountsMean(channel);
    }
    return rateMean[channel]*getPhotonCountsUnit(unit);
}

double QFRDRPhotonCountsData::getPhotonCountsStdDev(int channel, QFRDRPhotonCountsInterface::PhotonCountsUnits unit) {
   if (rateN<0 || !rate) {
        return 0.0;
    }
    if (channel<0) {
        double cnt=0;
        double sum=0;
        for (int i=0; i<rateN; i++) {
            double r=getPhotonCountsStdDev(i, unit);
            if (r>0) {
                sum=sum+r;
                cnt++;
            }
        }
        return sum/cnt;
    }
    if (!rateStdDev.contains(channel)) {
        rateStdDev[channel]=calcPhotonCountsStdDev(channel);
    }
    return rateStdDev[channel]*getPhotonCountsUnit(unit);
}

double QFRDRPhotonCountsData::calcPhotonCountsStdDev(int channel) {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return 0.0;
    }
    double m=0.0;
    double s=0.0;
    if (channel<rateChannels && channel>=0) {
        for (long long i=0; i<rateN; i++) {
            double v=rate[channel*rateN+i];
            m=m+v;
            s=s+v*v         ;
        }
    }
    double sd=sqrt(s/(double)rateN-m*m/(double)rateN/(double)rateN);
    rateStdDev[channel]=sd;
    return sd;
}

void QFRDRPhotonCountsData::calcPhotonCountsMinMax(int channel, double& min, double& max) {
    if (rateN<=0 || !rate) {
       // std::cout<<"error at start of recalculateCorrelations()\n";
        return;
    }
    min=max=0;
    if (channel<rateChannels) {
        min=rate[channel*rateN];
        max=min;
        for (long long i=0; i<rateN; i++) {
            double r=rate[channel*rateN+i];
            if (r>max) max=r;
            if (r<min) min=r;
        }
    }
    rateMin[channel]=min;
    rateMax[channel]=max;
}

void QFRDRPhotonCountsData::getPhotonCountsMinMax(int channel, double &min, double &max, QFRDRPhotonCountsInterface::PhotonCountsUnits unit) {
    min=0;
    max=0;
    if (!(rateMin.contains(channel) && rateMax.contains(channel))) {
        calcPhotonCountsMinMax(channel, min, max);
    }
    min=rateMin.value(channel, 0)*getPhotonCountsUnit(unit);
    max=rateMax.value(channel, 0)*getPhotonCountsUnit(unit);
}

void QFRDRPhotonCountsData::setPhotonCountsBinnedN(int value) {
     autoCalcRateN=value;
     setIntProperty("AUTO_BINNED_RATE_N", value, false, false);
     calcPhotonCountsBinned();
}

int QFRDRPhotonCountsData::GetPhotonCountsBinnedNFactor() {
    return autoCalcRateN;
}

void QFRDRPhotonCountsData::resizeRates(long long N, int channels) {
    if (rateT) free(rateT);
    if (rate) free(rate);
    rateChannels=0;
    rateN=0;
    rateT=NULL;
    rate=NULL;
    rateN=N;
    rateChannels=channels;
    rateT=(double*)calloc(rateN, sizeof(double));
    rate=(double*)calloc(rateN*rateChannels, sizeof(double));
    if (!rateT || !rate)
        setError(tr("Error while allocating memory for count rate data!"));
    emitRawDataChanged();
    rateMean.clear();
    rateStdDev.clear();
    rateMin.clear();
    rateMax.clear();
}

void QFRDRPhotonCountsData::resizeBinnedRates(long long N) {
    if (binnedRateT) free(binnedRateT);
    if (binnedRate) free(binnedRate);
    binnedRateN=0;
    binnedRateT=NULL;
    binnedRate=NULL;
    binnedRateN=N;
    if (N>0) {
        binnedRateT=(double*)calloc(binnedRateN, sizeof(double));
        binnedRate=(double*)calloc(binnedRateN*rateChannels, sizeof(double));
        if (!binnedRateT || !binnedRate)
            setError(tr("Error while allocating memory for binned count rate data!"));
        setIntProperty("BINNED_RATE_N", binnedRateN, false, false);
    }
    emitRawDataChanged();
}

void QFRDRPhotonCountsData::calcPhotonCountsBinned() {
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
        for (long channel=0; channel<rateChannels; channel++) {
            averageBinnedT=0;
            for (long long i=0; i<binnedRateN; i++) {
                long long jmin=i*d;
                long long jmax=mmin(rateN, (i+1)*d)-1;
                double sum=0;
                for (long long j=jmin; j<=jmax; j++) {
                    sum=sum+rate[channel*rateN+j];
                }
                binnedRate[channel*binnedRateN+i]=sum;
                binnedRateT[i]=rateT[jmin];
            }
            averageBinnedT=averageT*double(rateN)/double(binnedRateN);
            //qDebug()<<"averageT="<<averageT<<"   averageBinnedT="<<averageBinnedT;
            for (long long i=0; i<binnedRateN; i++) {
                binnedRate[channel*binnedRateN+i]=binnedRate[channel*binnedRateN+i]/averageBinnedT/1000.0;
            }
        }
    }
    emitRawDataChanged();
}

double QFRDRPhotonCountsData::getPhotonCountsUnit(QFRDRPhotonCountsInterface::PhotonCountsUnits unit) {
    //qDebug()<<"averageT="<<averageT;
    if (unit==QFRDRPhotonCountsInterface::KiloHertz) return 1.0/(averageT*1000.0);
    return 1.0;
}

double QFRDRPhotonCountsData::getPhotonCountsBinnedUnit(QFRDRPhotonCountsInterface::PhotonCountsUnits unit) {
    //qDebug()<<"averageBinnedT="<<averageBinnedT;
    if (unit==QFRDRPhotonCountsInterface::KiloHertz) return 1.0/(averageBinnedT*1000.0);
    return 1.0;

}
