#include "qfrdrimagingfcs_data.h"


QFRDRImagingFCSData::QFRDRImagingFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
}

QFRDRImagingFCSData::~QFRDRImagingFCSData()
{
}

void QFRDRImagingFCSData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImagingFCSData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*
	w.writeStartElement("mytag");
	w.writeAttribute("data", "123456789");
	w.writeEndElement();
    */
}

void QFRDRImagingFCSData::intReadData(QDomElement* e) {
	// read data from the project XML file

	/*
    if (e) {
        QDomElement te=e->firstChildElement("mytag");
        QString data=te.attribute("data");
    }
	*/

	// now also load the data file(s) this record is linked to
	// an error is reported when no file is given!
	if (files.size()<=0) {
		setError(tr("there are no files in the %1 record!").arg(getName()));
		return;
	}
	loadDataFile(files[0]);
}

bool QFRDRImagingFCSData::loadDataFile(QString filename) {
	bool ok=true;

	// LOAD YOUR DATAFILE
	//      and set ok=false, if an error occured
	// example:
	QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		ok=false;
	} else {
		//data=file.readLine();
	}

	if (!ok) setError(tr("Error while reading file '%1'").arg(filename));
    return ok;
}









int QFRDRImagingFCSData::getCorrelationRuns() {
    return width*height;
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

double* QFRDRImagingFCSData::getCorrelationRun(int run) {
    return &(correlations[run*N]);
}

double* QFRDRImagingFCSData::getCorrelationRunError(int run) {
    return &(sigmas[run*N]);
}

QString QFRDRImagingFCSData::getCorrelationRunName(int run) {
    int x=runToX(run);
    int y=runToY(run);
    return QString("[%1, %2]").arg(x).arg(y);
}

double* QFRDRImagingFCSData::getCorrelationMean() {
    return correlationMean;
}

double* QFRDRImagingFCSData::getCorrelationStdDev() {
    return correlationStdDev;
}

bool QFRDRImagingFCSData::isCorrelationRunVisible(int run) {
    return true;
}

void QFRDRImagingFCSData::allocateContents(int x, int y, int N) {
}
