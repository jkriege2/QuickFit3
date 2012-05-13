#include "qfrdrsettings_data.h"
#include <QtXml>

QFRDRSettingsData::QFRDRSettingsData(QFProject* parent):
    QFRawDataRecord(parent)
{
    data=NULL;
}

QFRDRSettingsData::~QFRDRSettingsData()
{
    if (data) delete data;
}

void QFRDRSettingsData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
    // THIS IS OPTIONAL
}

QSettings *QFRDRSettingsData::getData() const
{
    return data;
}


void QFRDRSettingsData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*        
	w.writeStartElement("mytag");
	w.writeAttribute("data", "123456789");
	w.writeEndElement();
    */
}

void QFRDRSettingsData::intReadData(QDomElement* e) {
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

bool QFRDRSettingsData::loadDataFile(QString filename) {
	bool ok=true;
	
	// LOAD YOUR DATAFILE 
	//      and set ok=false, if an error occured
	// example:
    if (data) delete data;
    data=new QSettings(filename, QSettings::IniFormat);
	
	if (!ok) setError(tr("Error while reading file '%1'").arg(filename));
    return ok;
}
