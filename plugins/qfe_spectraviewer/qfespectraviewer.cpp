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

#include "qfespectraviewer.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>
#include "qfespectraviewerdialog.h"
#include "optionswidget.h"
#include "spectrummanager.h"


#define LOG_PREFIX QString("qfe_spectraviewer >>> ").toUpper()

QFESpectraViewer::QFESpectraViewer(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlg=NULL;
    manager=new SpectrumManager();
    loadThread=new QFESpectraViewerLoadThread(manager, this);
    connect(loadThread, SIGNAL(slog_text(QString)), this, SLOT(doLog(QString)));

}

QFESpectraViewer::~QFESpectraViewer() {

}


void QFESpectraViewer::deinit() {
    /* add code for cleanup here */
    if (dlg) dlg->writeSettings();
    if (dlg) {
        dlg->close();
        delete dlg;
        dlg=NULL;
    }
    delete manager;
    manager=NULL;
}

QString QFESpectraViewer::pluginOptionsName() const
{
    return getName();
}

QIcon QFESpectraViewer::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFESpectraViewer::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

SpectrumManager *QFESpectraViewer::getSpectrumManager() const
{
    return manager;
}

void QFESpectraViewer::reloadDatabases()
{
    if (loadThread->isRunning()) return;
    log_text(intReloadDatabases(manager, getID()));
}

QString QFESpectraViewer::intReloadDatabases(SpectrumManager *manager, const QString& id)
{
    QString result="";
    if (!manager) {
        result+=tr("   INTERNAL ERROR: no fluorophore manager available\n");
        return result;
    }
    result+=(tr("   loading databases from:\n"));
    QStringList directories;
    QList<bool> readonly;
    directories<<QFPluginServices::getInstance()->getPluginAssetsDirectory(id);
    readonly<<true;
    directories<<ProgramOptions::getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getPluginConfigDirectory("qfe_spectraviewer")).toString();
    readonly<<false;
    for (int i=0; i<directories.size(); i++) {
        QString ro="";
        if (readonly.value(i, false)) ro=tr(" [readonly]");
        result+=(tr("     - %1%2\n").arg(directories[i]).arg(ro));
    }

    try {
        manager->clear();
    } catch (std::exception& E) {
        result+=tr("   ERROR when clearing manager\n");
    }

    result+=(tr("   loading fluorophore database ...\n"));
    try {
        manager->loadFluorophoreDatabase("fluorophors.ini", directories);
    } catch (std::exception& E) {
        result+=tr("      ERROR \n");
    }

    result+=(tr("   loading filters database ...\n"));
    try {
        manager->loadFilterDatabase("filters.ini", directories);
    } catch (std::exception& E) {
        result+=tr("      ERROR \n");
    }
    result+=(tr("   loading lightsources database ...\n"));
    try {
        manager->loadLightSourceDatabase("ligtsources.ini", directories);
    } catch (std::exception& E) {
        result+=tr("      ERROR \n");
    }
    result+=(tr("   loading detectors database ...\n"));
    try {
        manager->loadDetectorDatabase("detectors.ini", directories);
    } catch (std::exception& E) {
        result+=tr("      ERROR \n");
    }
    result+=(tr("   loading laserline database ...\n"));
    try {
        manager->loadLaserlinesDatabase("laserlines.ini", directories);
    } catch (std::exception& E) {
        result+=tr("      ERROR \n");
    }

    result+=(tr("   loaded %1 fluorophores\n").arg(manager->getFluorophoreCount()));
    result+=(tr("   loaded %1 filters\n").arg(manager->getFilterCount()));
    result+=(tr("   loaded %1 lightsources\n").arg(manager->getLightSourceCount()));
    result+=(tr("   loaded %1 detectors\n").arg(manager->getDetectorCount()));
    result+=(tr("   loaded %1 spectra\n").arg(manager->getSpectraCount()));
    return result;
}

void QFESpectraViewer::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFESpectraViewer::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    
	// some example code that may be used to register a menu and a tool button:
    log_text(tr("initializing ...\n"));
    log_text(tr("   registering plugin ...\n"));
    actStartPlugin=new QFActionWithNoMenuRole(QIcon(getIconFilename()), tr("Spectra Viewer"), this);
    actStartPlugin->setEnabled(false);
    actStartPlugin->setToolTip(tr("SpectraViewer: loading spectra database ... \n ... please wait until data is loaded, before starting this plugin!"));
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(showViewer()));
    connect(loadThread, SIGNAL(enableAction(bool)), this, SLOT(loadThreadFinished(bool)));
    QToolBar* exttb=services->getToolbar("tools");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("tools");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

    //log_text(QFESpectraViewer::intReloadDatabases(manager, getID()));
    log_text(tr("initializing ... DONE\n"));
    loadThread->start();
}

void QFESpectraViewer::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFESpectraViewer::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFESpectraViewer::log_text(QString message) {
    if (services) services->log_global_text(LOG_PREFIX+message);
}

void QFESpectraViewer::log_warning(QString message) {
    if (services) services->log_global_warning(LOG_PREFIX+message);
}

void QFESpectraViewer::log_error(QString message) {
    if (services) services->log_global_error(LOG_PREFIX+message);
}

void QFESpectraViewer::showViewer()
{
    if (loadThread->isRunning()) return;
    if (!dlg) dlg=new QFESpectraViewerDialog(this, NULL);
    dlg->show();
    dlg->raise();
}

void QFESpectraViewer::doLog(const QString &text)
{
    log_text(text);
}

void QFESpectraViewer::loadThreadFinished(bool enabled)
{
    if (enabled) {
        actStartPlugin->setToolTip(tr("start Spectra Viewer ..."));
    }
    actStartPlugin->setEnabled(enabled);
}



QFESpectraViewerLoadThread::QFESpectraViewerLoadThread(SpectrumManager *manager, QFESpectraViewer *parent): QThread(parent)
{
    this->manager=manager;
    sv=parent->getID();
}

/*void QFESpectraViewerLoadThread::log_text(const QString &text)
{
    emit slog_text(text);
}*/

void QFESpectraViewerLoadThread::run()
{
    try {
        emit slog_text(QFESpectraViewer::intReloadDatabases(manager, sv));
    } catch(std::exception& E) {
        emit slog_text(tr("error loading spectra viewer database: %1\n").arg(E.what()));
    }
    emit enableAction(true);
}



Q_EXPORT_PLUGIN2(qfe_spectraviewer, QFESpectraViewer)

