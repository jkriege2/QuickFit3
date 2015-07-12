/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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
    Q_UNUSED(format);

    Q_UNUSED(filename);

	// here you may export the data of the record into the specified format (see getExportFiletypes() )
    // THIS IS OPTIONAL
}

bool QFRDRSettingsData::showNextPreviousOfSameRoleButton() const
{
    return false;
}

QSettings *QFRDRSettingsData::getData() const
{
    return data;
}


void QFRDRSettingsData::intWriteData(QXmlStreamWriter& w) const {
    Q_UNUSED(w);

	// write data to the project XML file using the QXmlStreamWriter
    /*        
	w.writeStartElement("mytag");
	w.writeAttribute("data", "123456789");
	w.writeEndElement();
    */
}

void QFRDRSettingsData::intReadData(QDomElement* e) {
    Q_UNUSED(e);

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
