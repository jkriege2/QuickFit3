/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfe_dataexportbasics_xlsx.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>
#include "datatools.h"
#include "qfedataxlslibexport.h"

#define LOG_PREFIX QString("qfe_dataexportbasics_xlsx >>> ").toUpper()

QFEDataExportBasicsXLSX::QFEDataExportBasicsXLSX(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFEDataExportBasicsXLSX::~QFEDataExportBasicsXLSX() {

}


void QFEDataExportBasicsXLSX::deinit() {
	/* add code for cleanup here */
    for (int i=0; i<exporters.size(); i++) {
        if (exporters[i])  {
            QFDataExportHandler::deregisterDataWriter(exporters[i]);
            delete exporters[i];
        }
    }
    exporters.clear();
}

void QFEDataExportBasicsXLSX::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFEDataExportBasicsXLSX::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    exporters.append(NULL);
    exporters.append(NULL);
    QFDataExportHandler::registerDataWriter(exporters[0]=new QFEDataXlsxLibExport(true));
    QFDataExportHandler::registerDataWriter(exporters[1]=new QFEDataXlsxLibExport(false));


//    QList<QList<QVariant> > data;
//    data<<QList<QVariant>();
//    data<<QList<QVariant>();
//    data<<QList<QVariant>();
//    data<<QList<QVariant>();
//    data[0]<<0.0<<1.0<<2.0<<3.0;
//    data[1]<<"0.0"<<"1.0"<<"2.0"<<"3.0";
//    data[2]<<1<<2<<QVariant()<<3<<5<<8<<M_PI;
//    QStringList ch;
//    ch<<"col 1"<<"col 2"<<"col 3"<<"empty col 4";
//    exporters[0]->save(data, "testf.xlsx", ch);
//    exporters[1]->save(data, "test.xlsx", ch);
}



void QFEDataExportBasicsXLSX::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDataExportBasicsXLSX::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEDataExportBasicsXLSX::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEDataExportBasicsXLSX::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEDataExportBasicsXLSX::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_dataexportbasics_xlsx, QFEDataExportBasics)
