#include "qfetcspcimporter.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "qfetcspcimporterdialog.h"
#include "qmodernprogresswidget.h"
#include <QPair>
#include "qfrawdatarecord.h"

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

void QFETCSPCImporter::insertFCSCSVFile(const QString& filenameFCS, const QString &filenameCR, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly) {
    QStringList sl;
    sl<<filenameFCS<<filenameCR;
    QFRawDataRecord* e=project->addRawData("fcs", QFileInfo(filenameFCS).fileName(), sl, paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filenameFCS).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filenameFCS).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFETCSPCImporter::insertCountRate(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
    QFRawDataRecord* e=project->addRawData("photoncounts", QFileInfo(filename).fileName(), QStringList(filename), paramValues, paramReadonly);
    if (e->error()) {
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
    }
}

void QFETCSPCImporter::correlationDialogClosed() {
    if (!dlgCorrelate) return;


    QList<QPair<QStringList, QString> > list=dlgCorrelate->getFilesToAdd();

    QList<QPair<QStringList, QString> >::Iterator it = list.begin();
    services->setProgressRange(0, list.size());
    services->setProgress(0);
    int i=0;
    QModernProgressDialog progress(tr("Loading TCSPC Data ..."), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();
    while(it != list.end()) {
        i++;
        QString filename=it->first.value(0,"");
        QString type=it->second.toLower();

        services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(type));
        progress.setLabelText(tr("loading [%2] '%1' ...\n").arg(filename).arg(type));
        QApplication::processEvents();
        QMap<QString, QVariant> initParams;
        QStringList paramsReadonly;
        if (type=="photoncounts") {
            initParams["FILETYPE"]="CSV";
            paramsReadonly<<"FILETYPE";
            insertCountRate(filename, initParams, paramsReadonly);
        } else if (type=="photoncounts_binary") {
            initParams["FILETYPE"]="BINARY";
            paramsReadonly<<"FILETYPE";
            insertCountRate(filename, initParams, paramsReadonly);
        } else if (type=="fcs_csv") {
            initParams["FILETYPE"]="CSV_CORR";
            if (it->first.value(1,"")!="") initParams["FILETYPE"]="CSV_CORR_RATE";
            initParams["CSV_SEPARATOR"]=QString(",");
            initParams["CSV_COMMENT"]=QString("#");
            initParams["CROSS_CORRELATION"]=false;
            paramsReadonly<<"FILETYPE"<<"CSV_COMMENT"<<"CSV_SEPARATOR"<<"CROSS_CORRELATION";
            insertFCSCSVFile(filename, it->first.value(1,""), initParams, paramsReadonly);
        } else if (type=="fcs_cross_csv") {
            initParams["FILETYPE"]="CSV_CORR";
            if (it->first.value(1,"")!="") initParams["FILETYPE"]="CSV_CORR_RATE";
            initParams["CSV_SEPARATOR"]=QString(",");
            initParams["CSV_COMMENT"]=QString("#");
            initParams["CROSS_CORRELATION"]=true;
            paramsReadonly<<"FILETYPE"<<"CSV_COMMENT"<<"CSV_SEPARATOR"<<"CROSS_CORRELATION";
            insertFCSCSVFile(filename, it->first.value(1,""), initParams, paramsReadonly);
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
