#include "qfrdrimagingfcs_data.h"


QFRDRImagingFCSData::QFRDRImagingFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    N=0;
    width=0;
    height=0;
    runs=0;
}

QFRDRImagingFCSData::~QFRDRImagingFCSData() {
     allocateContents(0,0,0,0);
}

void QFRDRImagingFCSData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImagingFCSData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    if (leaveout.size()>0) {
        QString l="";
        for (int i=0; i<leaveout.size(); i+=2) {
            if (i+1<leaveout.size()) {
                if (!l.isEmpty()) l=l+",";
                l=l+QString::number(leaveout[i].first)+","+QString::number(leaveout[i+1].second);
            }
        }
        w.writeStartElement("leaveout");
        w.writeAttribute("list", l);
        w.writeEndElement();
    }
}

void QFRDRImagingFCSData::intReadData(QDomElement* e) {
    leaveout.clear();
	// read data from the project XML file
    leaveout.clear();
    if (e) {
        QDomElement te=e->firstChildElement("leaveout");
        QString l=te.attribute("list");
        QStringList li=l.split(",");
        for (int i=0; i<li.size(); i+=2) {
            if (i+1<li.size()) {
                bool ok=false, ok2=false;
                int lo=li[i].toUInt(&ok);
                int lo2=li[i+1].toUInt(&ok2);
                if (ok && ok2) leaveout.append(qMakePair(lo, lo2));
            }
        }
    }

	width=getProperty("WIDTH").toInt();
	height=getProperty("HEIGHT").toInt();
	runs=getProperty("RUNS").toInt();
	QString filetype=getProperty("FILETYPE", "unknown").toString();

	// now also load the data file(s) this record is linked to
	// an error is reported when no file is given!
	if (files.size()<=0) {
		setError(tr("there are no files in the %1 record!").arg(getName()));
		return;
	}
	if (filetype.toUpper()=="VIDEO_CORRELATOR") {
        loadVideoCorrelatorFile(files[0]);
	} else {
	    setError(tr("filetype '%1' is unknown for Imaging FCS data files (file is '%2')").arg(filetype).arg(files[0]));
	}
}

bool QFRDRImagingFCSData::loadVideoCorrelatorFile(QString filename) {
	bool ok=true;
	QString errorDescription="";

	// LOAD YOUR DATAFILE
	//      and set ok=false, if an error occured
	// example:
	QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		ok=false;
	} else {
        int taucolumn=getProperty("TAU_COLUMN", 0).toInt();
        if (taucolumn<0) taucolumn=0;
        int corrcolumn=getProperty("CORRELATION_COLUMN", 1).toInt();
        if (corrcolumn<0) corrcolumn=1;
        int maxCol=qMax(corrcolumn, taucolumn);

		QTextStream stream(&file);
		bool last_empty, empty;
		QVector<QVector<QPair<double, double> > > data_matrix;
		QVector<QPair<double, double> > current_set;
		int NN=0;
		int measurements=0;
		while ((!stream.atEnd()) && (measurements<=width*height)) {
            QVector<double> data=csvReadline(stream, ',', '#');
            last_empty=empty;
            empty=data.isEmpty();
            if ((!empty) && (corrcolumn<data.size()) && (taucolumn<data.size())) {
                current_set.append(qMakePair(data[taucolumn], data[corrcolumn]));
                //qDebug()<<"  tau="<<data[0]<<"   c="<<data[1];
            }
            if (((last_empty&&empty)||(stream.atEnd()))&&(!current_set.isEmpty())) {
                data_matrix.append(current_set);
                if (NN<current_set.size()) NN=current_set.size();
                //qDebug()<<"measurements="<<measurements<<"     NN="<<NN<<"     current_set.size()="<<current_set.size()<<"     data_matrix.size()="<<data_matrix.size();
                current_set.clear();
                measurements++;
                QApplication::processEvents();
            }
            //if (stream.atEnd()) qDebug()<<"measurements="<<measurements<<"     NN="<<NN<<"     width*height="<<width*height<<"     stream.atEnd()="<<stream.atEnd()<<"    data="<<data;

		}
        width=getProperty("WIDTH").toInt();
        height=getProperty("HEIGHT").toInt();
        runs=getProperty("RUNS").toInt();
        //std::cout<<"width="<<width<<"   height="<<height<<"   NN="<<NN<<std::endl;
        if (NN>0) {
            if (measurements+1>=width*height) {
                allocateContents(width, height, runs, NN);
                for (int i=0; i<width*height; i++) {
                    if (i>=data_matrix.size()) {
                        for (int j=0; j<NN; j++) {
                            correlations[i*NN+j]=1;
                            sigmas[i*NN+j]=0;
                        }
                    } else {
                        for (int j=0; j<NN; j++) {
                            //std::cout<<"i="<<i<<" data_matrix.size()="<<data_matrix.size()<<"    j="<<j<<" data_matrix[i].size()="<<data_matrix[i].size()<<std::endl;
                            if (j<data_matrix[i].size()) {
                                tau[j]=data_matrix[i].at(j).first;
                                correlations[i*NN+j]=data_matrix[i].at(j).second;
                                sigmas[i*NN+j]=0;
                            } else {
                                correlations[i*NN+j]=1;
                                sigmas[i*NN+j]=0;
                            }
                        }
                    }
                }
                QApplication::processEvents();
                recalcCorrelations();
                QApplication::processEvents();
            } else {
                ok=false;
                errorDescription=tr("too few records in file: %1 records found, but %2 * %3 = %4 required").arg(NN).arg(width).arg(height).arg(width*height);
            }
        } else {
            ok=false;
            errorDescription=tr("no records found in file");
        }
	}

	if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}









int QFRDRImagingFCSData::getCorrelationRuns() {
    return runs;
}

long long QFRDRImagingFCSData::getCorrelationN() {
    return N;
}

double* QFRDRImagingFCSData::getCorrelationT() {
    return tau;
}

double* QFRDRImagingFCSData::getCorrelation() {
    return correlations;
}

double* QFRDRImagingFCSData::getCorrelationRunErrors() {
    return sigmas;
}

double* QFRDRImagingFCSData::getCorrelationRun(int run, int measurement) {
    return &(correlations[measurement*runs*N + run*N]);
}

double* QFRDRImagingFCSData::getCorrelationRunError(int run, int measurement) {
    return &(sigmas[measurement*runs*N + run*N]);
}

QString QFRDRImagingFCSData::getCorrelationRunName(int run, int measurement) {
    return tr("run %1").arg(run);
}

int QFRDRImagingFCSData::getCorrelationMeasurements() {
    return width*height;
}

QString QFRDRImagingFCSData::getCorrelationMeasurementsName(int measurement) {
    int x=measurementToX(measurement);
    int y=measurementToY(measurement);
    return QString("[%1, %2]").arg(x).arg(y);
}

double* QFRDRImagingFCSData::getCorrelationMean(int measurement) {
    return &correlationMean[measurement*N];
}

double* QFRDRImagingFCSData::getCorrelationStdDev(int measurement) {
    return &correlationStdDev[measurement*N];
}

bool QFRDRImagingFCSData::isCorrelationRunVisible(int run, int measurement) {
    return !leaveoutRun(run, measurement);
}

void QFRDRImagingFCSData::allocateContents(int x, int y, int runs, int N) {
    if (correlations) free(correlations);
    if (correlationMean) free(correlationMean);
    if (correlationStdDev) free(correlationStdDev);
    if (sigmas) free(sigmas);
    if (tau) free(tau);
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    if ((x>0) && (y>0) && (N>0) && (runs>0)) {
        correlations=(double*)calloc(x*y*runs*N,sizeof(double));
        sigmas=(double*)calloc(x*y*runs*N,sizeof(double));
        correlationMean=(double*)calloc(runs*N,sizeof(double));
        correlationStdDev=(double*)calloc(runs*N,sizeof(double));
        tau=(double*)calloc(N,sizeof(double));
        width=x;
        height=y;
        this->N=N;
        this->runs=runs;
        setProperty("WIDTH", x);
        setProperty("HEIGHT", y);
        setProperty("RUNS", runs);
    }
}

void QFRDRImagingFCSData::recalcCorrelations() {
    if (correlations && correlationMean && correlationStdDev) {
        for (int n=0; n<N; n++) {
            for (int m=0; m<width*height; m++) {
                double norm=0;
                double sum=0;
                double sum2=0;
                for (int r=0; r<runs; r++) {
                    const double& v=correlations[m*N*runs + r*N + n];
                    sum+=v;
                    sum2+=v*v;
                    norm++;
                }
                if (norm>0) {
                    correlationMean[m*N+n]=sum/norm;
                    correlationStdDev[m*N+n]=sqrt(sum2/norm-sum*sum/norm/norm);
                } else {
                    correlationMean[m*N+n]=0;
                    correlationStdDev[m*N+n]=0;
                }
            }
        }
    }
}
