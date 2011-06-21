#include "qfrdr_data.h"


QFRDRData::QFRDRData(QFProject* parent):
    QFRawDataRecord(parent)
{
}

QFRDRData::~QFRDRData()
{
}

void QFRDRData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*        
	w.writeStartElement("mytag");
	w.writeAttribute("data", "123456789");
	w.writeEndElement();
    */
}

void QFRDRData::intReadData(QDomElement* e) {
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

bool QFRDRData::loadDataFile(QString filename) {
	bool ok=true;
	
	// LOAD YOUR DATAFILE 
	//      and set ok=false, if an error occured
	// example:
	QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {         
		ok=false;
	} else {
		data=file.readLine();
	}
	
	if (!ok) setError(tr("Error while reading file '%1'").arg(filename));
    return ok;
}

QString QFRDRData::getData() const { 
	return data; 
}