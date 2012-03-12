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
    if (project && settings) {
        QFETCSPCImporterDialog* dlgCorrelate=new QFETCSPCImporterDialog(services, settings, parentWidget);
        dlgCorrelate->setProject(project);
        dlgCorrelate->setWindowModality(Qt::WindowModal);
        //qDebug()<<parentWidget,
        dlgCorrelate->exec();



        QList<QPair<QString, QString> > list=dlgCorrelate->getFilesToAdd();

        QList<QPair<QString, QString> >::Iterator it = list.begin();
        services->setProgressRange(0, list.size());
        services->setProgress(0);
        int i=0;
        QModernProgressDialog progress(tr("Loading TCSPC Data ..."), "", NULL);
        progress.setWindowModality(Qt::WindowModal);
        progress.setHasCancel(false);
        progress.open();
        while(it != list.end()) {
            i++;
            services->log_text(tr("loading [%2] '%1' ...\n").arg((*it).first).arg((*it).second));
            progress.setLabelText(tr("loading [%2] '%1' ...\n").arg((*it).first).arg((*it).second));
            QString filename=it->first;
            QString type=it->second.toLower();
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
            }
            //insertVideoCorrelatorFile(filename, overview, filename.toLower().endsWith(".bin"));
            settings->setCurrentRawDataDir(QFileInfo(filename).dir().absolutePath());
            services->setProgress(i);
            QApplication::processEvents();
            ++it;
        }
        progress.accept();
        services->setProgress(0);

        delete dlgCorrelate;
    }
}

void QFETCSPCImporter::insertFCSCSVFile(const QString& filenameFCS, const QString &filenameCR, int channel, const QMap<QString, QVariant> &paramValues, const QStringList &paramReadonly) {
    QStringList sl;
    sl<<filenameFCS<<filenameCR;
    QFRawDataRecord* e=project->addRawData("fcs", QFileInfo(filenameFCS).fileName()+QString(" - CH%1").arg(channel+1), sl, paramValues, paramReadonly);
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
