/*
Copyright (c) 2014
	
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


#include "qfe_nidaqmxreader.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QtPlugin>
#include <iostream>
#include "optionswidget.h"

#define LOG_PREFIX QString("qfe_nidaqmxreader >>> ").toUpper()

QFENIDAQmxReader::QFENIDAQmxReader(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFENIDAQmxReader::~QFENIDAQmxReader() {

}


void QFENIDAQmxReader::deinit() {
    /* add code for cleanup here */
}

QString QFENIDAQmxReader::pluginOptionsName() const
{
    return getName();
}

QIcon QFENIDAQmxReader::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFENIDAQmxReader::createOptionsWidget(QWidget *parent)
{
    OptionsWidget* o= new OptionsWidget(this, parent);
    if (o) {
        connect(o, SIGNAL(writingSettings()), this, SLOT(optionsFinished()));
    }
    //qDebug()<<"*QFE_ALEXControl::createOptionsWidget("<<parent<<"): "<<o;
    return o;
}

void QFENIDAQmxReader::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFENIDAQmxReader::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    

	// some example code that may be used to register a menu and a tool button:
	services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    
    actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Start NI-DAQmx Channel-Reader"), this);
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));

    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

    optionsFinished();

    services->registerSettingsPane(this);

    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));

}

void QFENIDAQmxReader::startPlugin() {
    if (!win) win=new MainWindow(NULL);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
    win->raise();
}

void QFENIDAQmxReader::optionsFinished()
{
    QToolBar* exttb=services->getToolbar("extensions");
    if (exttb) {
        if (ProgramOptions::getConfigValue("qfe_nidaqmxreader/show_toolbar_button", false).toBool()) {
            exttb->addAction(actStartPlugin);
        } else {
            exttb->removeAction(actStartPlugin);
        }
    }
}

void QFENIDAQmxReader::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFENIDAQmxReader::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFENIDAQmxReader::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFENIDAQmxReader::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFENIDAQmxReader::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(qfe_nidaqmxreader, QFENIDAQmxReader)
