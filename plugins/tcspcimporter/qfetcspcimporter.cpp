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

#include "qfetcspcimporter.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>
#include "qfetcspcimporterdialog.h"
#include "qmodernprogresswidget.h"
#include <QPair>
#include "qfrawdatarecord.h"
#include"qfetcspcimporterjobthread.h"
#include "qf3correlationdataformattool.h"
#include "qfrdrcolumngraphsinterface.h"

#define LOG_PREFIX QString("tcspcimporter >>> ").toUpper()

QFETCSPCImporter::QFETCSPCImporter(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    dlgCorrelate=NULL;
}

QFETCSPCImporter::~QFETCSPCImporter() {

}


void QFETCSPCImporter::deinit() {
	/* add code for cleanup here */
}

void QFETCSPCImporter::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFETCSPCImporter::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    

	// some example code that may be used to register a menu and a tool button:


    QAction* actStartPlugin=new QAction(QIcon(getIconFilename()), tr("import/process TCSPC ..."), this);
    actStartPlugin->setToolTip(tr("Import TCSPC Files as FCS, CountRates, ..."));
    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    QMenu* extm=services->getMenu("data/rdr");
    if (extm) {
        extm->addAction(actStartPlugin);
    }

}

void QFETCSPCImporter::startPlugin() {
    if (dlgCorrelate) {
        dlgCorrelate->show();
    } else if (project && settings) {
        dlgCorrelate=new QFETCSPCImporterDialog(services, settings, parentWidget);
        dlgCorrelate->setProject(project);
        dlgCorrelate->setWindowModality(Qt::NonModal);
        //qDebug()<<parentWidget,
        dlgCorrelate->show();
        connect(dlgCorrelate, SIGNAL(finished(int)), this, SLOT(correlationDialogClosed()));
    }
}

QFRawDataRecord *QFETCSPCImporter::insertFCSCSVFile(const QString& filenameFCS, const QString &filenameCR, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString& group, const QString& role) {
    QStringList sl, types;
    sl<<filenameFCS;
    types<<"ACF";
    if (QFile::exists(filenameCR)) {
        sl<<filenameCR;
        types<<"RATE";
    }
    QString r="";
    if (!role.isEmpty()) r=QString(" - ")+role;
    QFRawDataRecord* e=project->addRawData("fcs", QFileInfo(filenameFCS).fileName()+r, sl, paramValues, paramReadonly, types);
    e->setRole(role);
    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filenameFCS).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filenameFCS).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
        return NULL;
    }
    return e;
}

QFRawDataRecord *QFETCSPCImporter::insertFCCSCSVFile(const QString& filenameFCS, const QString &filenameCR1, const QString &filenameCR2, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString& group, const QString& role) {
    QStringList sl, types;

    sl<<filenameFCS;
    types<<"CCF";
    if (QFile::exists(filenameCR1)) {
        sl<<filenameCR1;
        types<<"RATE";
    }
    if (QFile::exists(filenameCR2)) {
        sl<<filenameCR2;
        types<<"RATE";
    }

    QString r="";
    if (!role.isEmpty()) r=QString(" - ")+role;
    QFRawDataRecord* e=project->addRawData("fcs", QFileInfo(filenameFCS).fileName()+r, sl, paramValues, paramReadonly, types);
    e->setRole(role);
    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filenameFCS).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filenameFCS).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
        return NULL;
    }
    return e;
}

QFRawDataRecord *QFETCSPCImporter::insertCountRate(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly, const QString& group, const QString& role) {
    QString r="";
    if (!role.isEmpty()) r=QString(" - ")+role;
    QFRawDataRecord* e=project->addRawData("photoncounts", QFileInfo(filename).fileName()+r, QStringList(filename), paramValues, paramReadonly);
    e->setRole(role);
    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
        return NULL;
    }
    return e;
}

QFRawDataRecord *QFETCSPCImporter::insertTable(const QString &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString &group, const QString &role)
{
    QString r="";
    if (!role.isEmpty()) r=QString(" - ")+role;
    QFRawDataRecord* e=project->addRawData("table", QFileInfo(filename).fileName()+r, QStringList(filename), paramValues, paramReadonly);
    e->setRole(role);
    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
        return NULL;
    }
    return e;
}

QList<QPointer<QFRawDataRecord> > QFETCSPCImporter::insertQF3ASCIICORRFile(const QString &filename, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly, const QString &group)
{
    unsigned int cc=1;
    QList<QPointer<QFRawDataRecord> > res;
    QF3CorrelationDataFormatTool reader;
    reader.loadFile(filename, true);
    if (reader.wasError()) {
        cc=0;
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing QF3 ASCII file '%1':\n%2").arg(filename).arg(reader.getErrorMessage()));
        services->log_error(tr("Error while importing QF3 ASCII file '%1':\n    %2\n").arg(filename).arg(reader.getErrorMessage()));
    } else {
        //services->log_indent();
        services->log_text(tr("  * correlation types: %1\n").arg(reader.correlationTypes));
        services->log_text(tr("  * correlation runs: %1\n").arg(reader.runs));
        services->log_text(tr("  * channels: %1\n").arg(reader.channels));
        services->log_text(tr("  * rate runs: %1\n").arg(reader.rateRuns));
        for (int c=0; c<reader.correlationTypes; c++) {
            QMap<QString, QVariant> p=paramValues;
            QStringList pro=paramReadonly;
            p["QF3ASCII_CORRELATION_TYPE"]=c;
            p["CHANNEL"]=reader.getCorrelationTypePreferredChannel(c);
            pro<<"QF3ASCII_CORRELATION_TYPE";
            QString name=tr("%1 - %2").arg(QFileInfo(filename).fileName()).arg(reader.getRole(c));
            services->log_text(tr("    + adding %1\n").arg(name));
            QFRawDataRecord* e=project->addRawData("fcs", name, QStringList(filename), p, pro);
            if (e) {
                e->setRole(reader.getRole(c));
                e->setFolder(reader.folder);
                e->setGroup(project->addOrFindRDRGroup(group));//project->addOrFindRDRGroup(QFileInfo(filename).fileName()+"_"+reader.group));
                if (e->error()) {
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing QF3 ASCII file '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing QF3 ASCII file '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                } else {
                    res<<e;
                }
            } else {
                QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing QF3 ASCII file '%1':\n%2").arg(filename).arg(tr("could not create RDR")));
                services->log_error(tr("Error while importing QF3 ASCII file '%1':\n    %2\n").arg(filename).arg(tr("could not create RDR")));
            }
        }
        //services->log_unindent();
    }
    return res;
}

void QFETCSPCImporter::correlationDialogClosed() {
    if (!dlgCorrelate) return;


    QList<QFETCSPCImporterJobThreadAddFileProps > list=dlgCorrelate->getFilesToAdd();

    QList<QFETCSPCImporterJobThreadAddFileProps >::Iterator it = list.begin();
    services->setProgressRange(0, list.size());
    services->setProgress(0);
    int i=0;
    QModernProgressDialog progress(tr("Loading TCSPC Data ..."), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();
    while(it != list.end()) {
        i++;
        QString filename=it->files.value(0,"");
        QString type=it->type.toLower();

        services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(type));
        progress.setLabelText(tr("loading [%2] '%1' ...\n").arg(filename).arg(type));
        QApplication::processEvents();
        QMap<QString, QVariant> initParams=it->props;
        QStringList paramsReadonly=it->props.keys();
        QFRawDataRecord* rdr=NULL;
        QList<QPointer<QFRawDataRecord> > rdrs;
        if (type=="photoncounts") {
            initParams["FILETYPE"]="CSV";
            paramsReadonly<<"FILETYPE";
            if (QFile::exists(filename) && QFileInfo(filename).size()>0) rdr=insertCountRate(filename, initParams, paramsReadonly);
        } else if (type=="photoncounts_binary") {
            initParams["FILETYPE"]="BINARY";
            paramsReadonly<<"FILETYPE";
            if (QFile::exists(filename) && QFileInfo(filename).size()>0) rdr=insertCountRate(filename, initParams, paramsReadonly);
        } else if (type=="fcs_csv") {
            initParams["FILETYPE"]="CSV_CORR";
            if (it->files.value(1,"")!="") initParams["FILETYPE"]="CSV_CORR_RATE";
            initParams["CSV_SEPARATOR"]=QString(",");
            initParams["CSV_COMMENT"]=QString("#");
            initParams["CROSS_CORRELATION"]=false;
            paramsReadonly<<"FILETYPE"<<"CSV_COMMENT"<<"CSV_SEPARATOR"<<"CROSS_CORRELATION";
            rdr=insertFCSCSVFile(filename, it->files.value(1,""), initParams, paramsReadonly,it->group, it->role);
        } else if (type=="fcs_cross_csv") {
            initParams["FILETYPE"]="CSV_CORR";
            if (it->files.value(1,"")!="") initParams["FILETYPE"]="CSV_CORR_RATE";
            initParams["CSV_SEPARATOR"]=QString(",");
            initParams["CSV_COMMENT"]=QString("#");
            initParams["CROSS_CORRELATION"]=true;
            paramsReadonly<<"FILETYPE"<<"CSV_COMMENT"<<"CSV_SEPARATOR"<<"CROSS_CORRELATION";

            rdr=insertFCCSCSVFile(filename, it->files.value(1,""), it->files.value(2,""), initParams, paramsReadonly,it->group, it->role);
        } else if (type=="qf3asciicorr") {
            initParams["FILETYPE"]="QF3ASCIICORR";
            paramsReadonly<<"FILETYPE";
            rdr=NULL;
            rdrs=insertQF3ASCIICORRFile(filename, initParams, paramsReadonly, it->group);
        } else if (type=="lifetime_csv") {
            initParams["READFILE_FILEFORMAT"]="CSV";
            initParams["FILETYPE"]="CSV";
            paramsReadonly<<"READFILE_FILEFORMAT"<<"FILETYPE";
            rdr=NULL;
            rdr=insertTable(filename, initParams, paramsReadonly, it->group);
            QFRDRColumnGraphsInterface* cg=dynamic_cast<QFRDRColumnGraphsInterface*>(rdr);
            if (cg && it->dataForInsert.value(0).isValid() && it->dataForInsert.value(1).isValid()) {
                int segs=it->dataForInsert.value(0).toInt();
                int chs=it->dataForInsert.value(1).toInt();
                if (segs==1) {
                    int g=cg->colgraphAddPlot(tr("Lifetime Histograms"), tr("arrival time delaty t [ns]"), tr("photon frequency"));
                    cg->colgraphSetPlotXRange(g, 0, it->dataForInsert.value(2).toDouble());
                    cg->colgraphSetPlotYRange(g, -1, it->dataForInsert.value(3).toDouble());
                    for (int i=0; i<chs; i++) {
                        cg->colgraphAddGraph(g, 0, i+1, QFRDRColumnGraphsInterface::cgtLines, tr("channel %1").arg(i));
                    }
                } else {
                    for (int i=0; i<chs; i++) {
                        int g=cg->colgraphAddPlot(tr("Lifetime Histograms, channel %1").arg(i), tr("arrival time delaty t [ns]"), tr("photon frequency"));
                        cg->colgraphSetPlotXRange(g, 0, it->dataForInsert.value(2).toDouble());
                        cg->colgraphSetPlotYRange(g, -1, it->dataForInsert.value(3).toDouble());
                        for (int s=0; s<segs; s++) {
                            cg->colgraphAddGraph(g, 0, 1+s*chs+i, QFRDRColumnGraphsInterface::cgtLines, tr("segment %1").arg(s));
                        }
                    }
                }
            }
        }
        if (rdr) {
            rdr->setDescription(rdr->getDescription()+QString("\n\n")+it->comment);
            rdr->setGroup(rdr->getProject()->addOrFindRDRGroup(it->group));
            for (int ii=0; ii<rdrs.size(); ii++) {
                if (rdrs[ii]) {
                    rdrs[ii]->setDescription(rdrs[ii]->getDescription()+QString("\n\n")+it->comment);
                    rdrs[ii]->setGroup(rdrs[ii]->getProject()->addOrFindRDRGroup(it->group));
                }
            }
        }
        //insertVideoCorrelatorFile(filename, overview, filename.toLower().endsWith(".bin"));
        settings->setCurrentRawDataDir(QFileInfo(filename).dir().absolutePath());
        services->setProgress(i);
        QApplication::processEvents();
        ++it;
    }
    progress.accept();
    services->setProgress(0);

    dlgCorrelate->deleteLater();
    dlgCorrelate=NULL;
}

void QFETCSPCImporter::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFETCSPCImporter::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFETCSPCImporter::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFETCSPCImporter::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFETCSPCImporter::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(tcspcimporter, QFETCSPCImporter)
