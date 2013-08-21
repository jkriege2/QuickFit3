#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"
#include "yaid_rh.h"
#include "qfrdrimagingfcssimulator.h"

QFRDRImagingFCSPlugin::QFRDRImagingFCSPlugin(QObject* parent):
    QObject(parent), QFPluginRawDataRecordBase()
{
    //constructor
    dlgCorrelate=NULL;
    dlgSimulate=NULL;
}

QFRDRImagingFCSPlugin::~QFRDRImagingFCSPlugin()
{
}

void QFRDRImagingFCSPlugin::deinit() {
}

void QFRDRImagingFCSPlugin::imfcsCorrRemoteUserSelectFile()
{
    correlateAndInsert();
    dlgCorrelate->userSelectFile();
}

void QFRDRImagingFCSPlugin::imfcsCorrRemoteSelectFile(const QString &filename)
{
    correlateAndInsert();
    dlgCorrelate->openFile(filename);
}

void QFRDRImagingFCSPlugin::imfcsCorrRemoteAddJob()
{
    correlateAndInsert();
    dlgCorrelate->clickAddJob();
}

void QFRDRImagingFCSPlugin::imfcsCorrRemoteAddJobSeries(const QString &parameter, double start, double end, double inc)
{
    correlateAndInsert();
    dlgCorrelate->clickAddJobSeries(parameter, start, end, inc);
}

QFRawDataRecord* QFRDRImagingFCSPlugin::createRecord(QFProject* parent) {
    // factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRImagingFCSData)
    return new QFRDRImagingFCSData(parent);
}

void QFRDRImagingFCSPlugin::setProject(QFProject* project) {
    QFPluginRawDataRecordBase::setProject(project);
}


void QFRDRImagingFCSPlugin::registerToMenu(QMenu* menu) {
    QMenu* m=menu->addMenu(QIcon(getIconFilename()), tr("&imFCS: Imaging FCS"));

    // create menu entries to insert data with this type
    QAction* action=new QAction(QIcon(getIconFilename()), tr("&load imFCS dataset"), parentWidget);
    action->setStatusTip(tr("Insert a new imaging FCS record"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRecord()));
    m->addAction(action);

    QAction* actCorrelate=new QAction(QIcon(":/imaging_fcs/qfrdrimagingfcs_correlate.png"), tr("&correlate images and insert"), parentWidget);
    actCorrelate->setStatusTip(tr("Correlate an image series and insert the result into the current project"));
    connect(actCorrelate, SIGNAL(triggered()), this, SLOT(correlateAndInsert()));
    m->addAction(actCorrelate);

    QAction* actSimulate=new QAction(QIcon(":/imaging_fcs/qfrdrimagingfcs_simulate.png"), tr("&simulate images for correlation"), parentWidget);
    actSimulate->setStatusTip(tr("Simulates an image series for ImFCS that can afterwards be correlated"));
    connect(actSimulate, SIGNAL(triggered()), this, SLOT(simulateForCorrelation()));
    m->addAction(actSimulate);
    //actSimulate->setEnabled(false);

}

void QFRDRImagingFCSPlugin::correlateAndInsert() {
    if (dlgCorrelate) {
        dlgCorrelate->show();
        dlgCorrelate->setProject(project);
    } else if (project && settings) {
        dlgCorrelate=new QFRDRImagingFCSCorrelationDialog(services, settings, parentWidget);
        dlgCorrelate->setProject(project);
        dlgCorrelate->setWindowModality(Qt::NonModal);
        //qDebug()<<parentWidget,
        dlgCorrelate->show();

        connect(dlgCorrelate, SIGNAL(finished(int)), this, SLOT(importCorrelationsFromDialog()));
        connect(dlgCorrelate, SIGNAL(runSimulation()), this, SLOT(simulateForCorrelation()));

    }
}

void QFRDRImagingFCSPlugin::importCorrelationsFromDialog() {

    disconnect(dlgCorrelate, SIGNAL(finished(int)), this, SLOT(importCorrelationsFromDialog()));

    QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> list=dlgCorrelate->getFilesToAdd();

    QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo>::Iterator it = list.begin();
    services->setProgressRange(0, list.size());
    services->setProgress(0);
    int i=0;
    QModernProgressDialog progress(tr("Loading imFCS Data ..."), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();
    while(it != list.end()) {
        i++;
        services->log_text(tr("loading '%1' [%2] ...\n").arg(it->filename).arg(it->role));
        progress.setLabelText(tr("loading '%1' [%2] ...\n").arg(it->filename).arg(it->role));
        QString filename=it->filename;
        QString overview="";
        QApplication::processEvents();
        if (it->filetype==QFRDRImagingFCSCorrelationJobThread::ftCorrelation) {
            insertVideoCorrelatorFile(filename, overview, QString(""), filename.toLower().endsWith(".bin"), it->role, it->internalDualViewMode, it->dualViewID, true, true, it->group);
        } else if (it->filetype==QFRDRImagingFCSCorrelationJobThread::ftNandB) {
            insertNandBFromVideoCorrelatorFile(it->filenameEvalSettings, it->filename, it->filenameVar, it->filenameBack, it->filenameBackVar, it->internalDualViewMode, it->dualViewID, it->role, it->group);
        }
        settings->setCurrentRawDataDir(QFileInfo(it->filename).dir().absolutePath());
        services->setProgress(i);
        QApplication::processEvents();
        ++it;
    }
    progress.accept();
    services->setProgress(0);

    dlgCorrelate->deleteLater();
    dlgCorrelate=NULL;
}

void QFRDRImagingFCSPlugin::simulateForCorrelation()
{
    QFRDRImagingFCSSimulator* sim=new QFRDRImagingFCSSimulator(parentWidget);
    if (dlgSimulate) {
        dlgSimulate->show();
    }else if (project && settings) {
        dlgSimulate=new QFRDRImagingFCSSimulator(parentWidget);
        dlgSimulate->setWindowModality(Qt::NonModal);
        //qDebug()<<parentWidget,
        dlgSimulate->show();

        connect(dlgSimulate, SIGNAL(accepted()), this, SLOT(importCorrelationsFromSimulation()));
    }
}

void QFRDRImagingFCSPlugin::importCorrelationsFromSimulation()
{
    correlateAndInsert();
    if (dlgCorrelate) {
        dlgCorrelate->openFile(dlgSimulate->getSimulationFilename());
        dlgSimulate->deleteLater();
        dlgSimulate=NULL;
    }
}


void QFRDRImagingFCSPlugin::insertRecord() {
    if (project) {
        QRegExp rxdccf("\\.dccf(\\d+)?\\.(dat|bin)");
        rxdccf.setCaseSensitivity(Qt::CaseInsensitive);
        rxdccf.setMinimal(true);

        // file format to import
        QString format_binVideoCorrelator=tr("binary imFCS Correlations (*.autocorrelation.bin *.acf.bin *.crosscorrelation.bin *.ccf.bin *.dccf.bin *.bin)");
        QString format_binVideoCorrelator_short=tr("binary imFCS correlation data");
        QString format_videoCorrelator=tr("CSV imFCS Correlations (*.autocorrelation.dat *.acf.dat *.crosscorrelation.dat *.ccf.dat *.dccf.dat *.qf.dat *.dat)");
        QString format_videoCorrelator_short=tr("CSV imFCS correlation data");
        QString format_RH2Cor=tr("RH2 hw correlator dump (*.cor.dat)");
        QString format_RH2Cor_short=tr("RH2 hw cor dump");
        QStringList formats, formats_short;
        formats<<format_binVideoCorrelator<<format_videoCorrelator<<format_RH2Cor;
        formats_short<<format_binVideoCorrelator_short<<format_videoCorrelator_short<<format_RH2Cor_short;
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("imaging_fcs/current_format_filter", format_videoCorrelator).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              formats.join(";;"), &current_format_name, QFileDialog::ReadOnly);
        // store the format we just used
        settings->getQSettings()->setValue("imaging_fcs/current_format_filter", current_format_name);
        QString current_format_name_short=formats_short.value(formats.indexOf(current_format_name), "???");

        // now we iterate over all files and use QuickFit's progress bar interface (see plugin services)
        QStringList list = files;
        QStringList::Iterator it = list.begin();
        services->setProgressRange(0, list.size());
        services->setProgress(0);
        int i=0;
        QModernProgressDialog progress(tr("Loading imFCS Data ..."), "", NULL);
        progress.setWindowModality(Qt::WindowModal);
        progress.setHasCancel(false);
        progress.open();
        //qDebug()<<current_format_name;
        while(it != list.end()) {
            i++;
            services->log_text(tr("loading [%2] '%1' ...\n").arg(*it).arg(current_format_name_short));
            progress.setLabelText(tr("loading [%2] '%1' ...\n").arg(*it).arg(current_format_name_short));
            QApplication::processEvents();
            if (current_format_name==format_videoCorrelator) {
                QString filename=*it;
                QString overview=findOverviewFileForVideoCorrelatorFile(filename);
                QString evalsettings=findEvalsettingsFileForVideoCorrelatorFile(filename);
                bool isFCCS=false;
                bool isACF=false;
                int dv2mode=getEvalsettingsDV2Mode(evalsettings, filename, &isFCCS, &isACF);
                if ((dv2mode==0) || !isFCCS || (isFCCS && dv2mode!=1 && dv2mode!=2)) {
                    insertVideoCorrelatorFile(filename, overview, evalsettings, false, "", 0, 0, true, true, QFileInfo(filename).completeBaseName());
                } else if (dv2mode==1||dv2mode==2) {
                    if (isFCCS) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("FCCS"), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                    } else if (isACF) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("ACF0"), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("ACF1"), dv2mode, 1, true, true, QFileInfo(filename).completeBaseName());
                    } else {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString(""), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                    }
                }
            } else if (current_format_name==format_binVideoCorrelator) {
                QString filename=*it;
                //qDebug()<<"file: "<<filename;
                QString overview=findOverviewFileForVideoCorrelatorFile(filename);
                //qDebug()<<"ovr: "<<overview;
                QString evalsettings=findEvalsettingsFileForVideoCorrelatorFile(filename);
                //qDebug()<<"evalset: "<<evalsettings;
                bool isFCCS=false;
                bool isACF=false;
                int dv2mode=getEvalsettingsDV2Mode(evalsettings, filename, &isFCCS, &isACF);
                //qDebug()<<"isFCCS: "<<isFCCS;
                //qDebug()<<"isACF: "<<isACF;
                //qDebug()<<"dv2mode: "<<dv2mode;
                if (dv2mode==1||dv2mode==2) {
                    if (isFCCS) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("FCCS"), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                    } else if (isACF) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("ACF0"), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("ACF1"), dv2mode, 1, true, true, QFileInfo(filename).completeBaseName());
                    } else {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString(""), dv2mode, 0, true, true, QFileInfo(filename).completeBaseName());
                    }
                } else {
                    insertVideoCorrelatorFile(filename, overview, evalsettings, true, "", 0, 0, true, true, QFileInfo(filename).completeBaseName());
                }
            } else if (current_format_name==format_RH2Cor) {
                QString filename=*it;
                insertRH2CorFile(filename, "ACF");
            }
            settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
            services->setProgress(i);
            QApplication::processEvents();
            ++it;
        }
        progress.accept();
        services->setProgress(0);
    }
}

int QFRDRImagingFCSPlugin::checkColumns(QString filename) {
    int result=0;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while ((!stream.atEnd()) && (result<=0)) {
            QVector<double> data=csvReadline(stream, ',', '#', 0);
            result=data.size();
        }
        file.close();
    }
    return result;
}

QFRawDataRecord* QFRDRImagingFCSPlugin::insertProjectRecord(const QString &type, const QString &name, const QString &filename, const QString& role, const QString& group, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly) {
    bool found=false;
    QFRawDataRecord*  r;
    for (int i=0; i<project->getRawDataCount(); i++) {
        r=project->getRawDataByNum(i);
        if (r->getType()==type && r->getName()==name && r->getFolder()==directory) {
            found=true;
            break;
        }
    }
    if (!found && project->getRawDataRecordFactory()->contains(type)) {
        QStringList files, files_types;
        files<<filename;

        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, role, files,  init_params, init_params_readonly, QStringList(), QStringList());
        e->setFolder(directory);
        e->setDescription(description);
        if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
            e=NULL;
        }
        return e;
    }
    if (found) return r;
    return NULL;
}

QFRawDataRecord* QFRDRImagingFCSPlugin::insertProjectRecordFiles(const QString &type, const QString &name, const QStringList &files, const QString& role, const QString& group, const QStringList &filetypes, const QStringList &filedescriptions, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly) {
    bool found=false;
    QFRawDataRecord*  r=NULL;
    for (int i=0; i<project->getRawDataCount(); i++) {
        r=project->getRawDataByNum(i);
        if (r->getType()==type && r->getName()==name && r->getFolder()==directory) {
            found=true;
            break;
        }
    }
    if (!found && project->getRawDataRecordFactory()->contains(type)) {

        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, role, files,  init_params, init_params_readonly, filetypes, filedescriptions);
        e->setFolder(directory);
        e->setDescription(description);
        if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(files.value(0, "---")).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(files.value(0, "---")).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
            return NULL;
        }
        return e;
    }
    if (found) return r;
    return NULL;
}

bool QFRDRImagingFCSPlugin::parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions)
{
    if (QFile::exists(filename_settings)) {
        QSettings settings(filename_settings, QSettings::IniFormat);
        appendCategorizedFilesFromB040SPIMConfig(settings, files, files_types, files_descriptions);

        if (settings.contains("acquisition/exposure_time")) {
            initParams["EXPOSURE_TIME"]=settings.value("acquisition/exposure_time").toDouble();
            paramsReadonly<<"EXPOSURE_TIME";
        }

        if (settings.contains("acquisition/duration")) {
            initParams["DURATION"]=settings.value("acquisition/duration").toDouble();
            paramsReadonly<<"DURATION";
        }
        if (settings.contains("acquisition/frame_time")) {
            initParams["FRAME_TIME"]=settings.value("acquisition/frame_time").toDouble();
            paramsReadonly<<"FRAME_TIME";
        }
        if (settings.contains("acquisition/pixel_height") && !initParams.contains("PIXEL_HEIGHT")) {
            initParams["PIXEL_HEIGHT"]=settings.value("acquisition/pixel_height").toDouble()*1000.0;
            paramsReadonly<<"PIXEL_HEIGHT";
        }
        if (settings.contains("acquisition/pixel_width") && !initParams.contains("PIXEL_WIDTH")) {
            initParams["PIXEL_WIDTH"]=settings.value("acquisition/pixel_width").toDouble()*1000.0;
            paramsReadonly<<"PIXEL_WIDTH";
        }
        if (settings.contains("experiment/title")) {
            description=description+tr("%1:\n\n").arg(settings.value("experiment/title").toString());
        }
        if (settings.contains("experiment/experimenter")) {
            description=description+tr("- experimenter: %1\n").arg(settings.value("experiment/experimenter").toString());
        }
        if (settings.contains("experiment/labbook")) {
            description=description+tr("- labbook page: %1\n").arg(settings.value("experiment/labbook").toString());
        }
        if (settings.contains("experiment/id")) {
            description=description+tr("- ID: %1\n").arg(settings.value("experiment/id").toString());
        }
        if (settings.contains("experiment/start_time")) {
            description=description+tr("- date/time: %1\n").arg(settings.value("experiment/start_time").toString());
        }
        if (settings.contains("experiment/description")) {
            description=description+tr("- description: %1\n").arg(settings.value("experiment/description").toString());
        }
        if (settings.contains("experiment/sample")) {
            description=description+tr("- sample: %1\n").arg(settings.value("experiment/sample").toString());
        }
        return true;
    }
    return false;
}

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QString& filename_overvieww, const QString& filename_evalsettings, bool binary, const QString& roleIn, int internalDualViewMode, int dualViewID, bool addCorrelations, bool adNandB, const QString& group) {

    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview=filename_overvieww;
    QString filename_overviewstd="";
    QString filename_background="";
    QString filename_backgroundstddev="";
    QString filename_settings="";
    QString filename_acquisition="";
    QString evalFilename=filename_evalsettings;
    QString role=roleIn;

    int dccfid=-1;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"IS_OVERVIEW_SCALED"<<"INTERNAL_DUALVIEW_MODE"<<"INTERNAL_DUALVIEW_MODE_CHANNEL";

    bool ok=true;
    bool isJanBFile=false;

    QStringList more_files, more_files_types, more_files_descriptions;

    initParams["INTERNAL_DUALVIEW_MODE"]="none";
    if (internalDualViewMode==1) initParams["INTERNAL_DUALVIEW_MODE"]="horizontal";
    if (internalDualViewMode==2) initParams["INTERNAL_DUALVIEW_MODE"]="vertical";
    initParams["INTERNAL_DUALVIEW_MODE_CHANNEL"]=dualViewID;

    // set whatever you want (FILETYPE is just an example)!
    if (binary) initParams["FILETYPE"]="VIDEO_CORRELATOR_BIN";
    else initParams["FILETYPE"]="VIDEO_CORRELATOR";
    bool isCross=false;
    bool isDCCF=false;

    QString evalFilenameH="";
    filenameHeuristics(filename, &evalFilenameH, &isCross, &isDCCF, &dccfid, &isJanBFile, &role);
    if (!QFile::exists(evalFilename) && QFile::exists(evalFilenameH)) evalFilename=evalFilenameH;


    if (QFile::exists(filename)) {
        int width=0;
        int height=0;
        bool ok=true;
        QString description;
        if (!readEvalSettings(evalFilename, isDCCF, initParams, paramsReadonly, width, height, more_files, more_files_types, more_files_descriptions, description, filename_settings, filename_acquisition, filename_overview, filename_overviewstd, filename_background, filename_backgroundstddev, role, dccfid)) {
            ok=false;
        }

        if (ok) {
            QStringList files, files_types, files_descriptions;


            files<<more_files;
            files_types<<more_files_types;
            files_descriptions<<more_files_descriptions;


            int columns=1;
            if (!binary) columns=checkColumns(filename);

            QString folder=QFileInfo(filename_acquisition).baseName();

            if (addCorrelations) {

                if (!isCross && !isDCCF) {
                    if (!binary) {
                        initParams["CORRELATION_COLUMN"]=1;
                        if (isJanBFile && columns>=7) initParams["CORRELATION_COLUMN"]=7;
                        if (isJanBFile && columns==2) initParams["TAU_FACTOR"]=1e-6;
                    }
                    files.prepend(filename);
                    files_types.prepend("acf");
                    files_descriptions.prepend("");
                    //qDebug()<<"filename_overview: "<<filename_overview;

                    if (columns>2 && !isJanBFile && !binary){
                        initParams["CORRELATION_ERROR_COLUMN"]=2;
                    }
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - %1").arg(role), role, files, initParams, paramsReadonly, files_types, files_descriptions);
                    e->setFolder(folder);
                    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
                    if (!description.isEmpty()) e->setDescription(description);
                    if (e->error()) { // when an error occured: remove record and output an error message
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        project->deleteRawData(e->getID());
                    }
                } else if (isCross && !binary) {

                    files.prepend(filename);
                    files_types.prepend("ccf");
                    files_descriptions.prepend("");
                    for (int c=1; c<=qMin(4,columns); c++) {
                        initParams["CORRELATION_COLUMN"]=c;

                        if (columns>4) initParams["CORRELATION_ERROR_COLUMN"]=c+4;
                        // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - %1").arg(role), role, files, initParams, paramsReadonly, files_types, files_descriptions);
                        e->setFolder(folder);
                        if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
                        if (!description.isEmpty()) e->setDescription(description);

                        if (e->error()) { // when an error occured: remove record and output an error message
                            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                            project->deleteRawData(e->getID());
                        }
                    }
                } else if (isCross && binary) {

                    files.prepend(filename);
                    files_types.prepend("ccf");
                    files_descriptions.prepend("");
                    for (int c=1; c<=4; c++) {
                        initParams["CORRELATION_SET"]=c-1;

                        // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - %1").arg(role), role, files, initParams, paramsReadonly, files_types, files_descriptions);
                        e->setFolder(folder);
                        if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
                        if (!description.isEmpty()) e->setDescription(description);
                        if (e->error()) { // when an error occured: remove record and output an error message
                            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                            project->deleteRawData(e->getID());
                        }
                    }
                } else if (isDCCF) {

                    if (!binary) initParams["CORRELATION_COLUMN"]=1;
                    files.prepend(filename);
                    files_types.prepend("dccf");
                    files_descriptions.prepend("");

                    if (columns>2 && !binary) initParams["CORRELATION_ERROR_COLUMN"]=2;
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    if (role.startsWith("DCCF") && initParams.contains("DCCF_DELTAX") && initParams.contains("DCCF_DELTAY")) {
                        role=QString("DCCF(%1,%2)").arg(initParams.value("DCCF_DELTAX", 0).toInt()).arg(initParams.value("DCCF_DELTAY", 0).toInt());

                    }

                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - %1").arg(role), role, files, initParams, paramsReadonly, files_types, files_descriptions);
                    e->setFolder(folder);
                    if (!group.isEmpty()) e->setGroup(project->addOrFindRDRGroup(group));
                    if (!description.isEmpty()) e->setDescription(description);
                    if (e->error()) { // when an error occured: remove record and output an error message
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        project->deleteRawData(e->getID());
                    }
                }
            }


            if (adNandB && QFile::exists(filename_overview) && QFile::exists(filename_overviewstd) && QFile::exists(filename_background) && QFile::exists(filename_backgroundstddev)) {
                QFileInfo fi(filename_overview);
                //qDebug()<<fi.fileName();
                QStringList ffiles, ffile_types, ffiles_descriptions;
                ffiles<<filename_overview<<filename_overviewstd<<filename_background<<filename_backgroundstddev;
                ffile_types<<"image"<<"image_std"<<"background"<<"background_stddev";
                ffiles_descriptions<<""<<""<<""<<"";
                ffiles<<more_files;
                ffile_types<<more_files_types;
                ffiles_descriptions<<more_files_descriptions;

                QMap<QString,QVariant>& init_params=initParams;
                QStringList init_params_readonly=paramsReadonly;
                init_params["BACKGROUND_CORRECTED"]=true;
                init_params_readonly<<"BACKGROUND_CORRECTED";
                insertProjectRecordFiles("number_and_brightness", fi.fileName()+tr(" - number & brightness"), ffiles, "", group, ffile_types, ffiles_descriptions, description, folder, init_params, init_params_readonly);
            }

            if (QFile::exists(filename_settings)) {
                QFileInfo fi(filename_settings);
                insertProjectRecord("rdr_settings", fi.fileName()+tr(" - acquisition settings"), filename_settings, "", group, description, folder);
            }

            for (int i=0; i<files_types.size(); i++) {
                if (QFile::exists(files[i]) && files_types[i].toLower().trimmed()=="setup_properties") {
                    QFileInfo fi(files[i]);
                    QMap<QString, QVariant> p;
                    p["column_separator"]=",";
                    p["decimal_separator"]=".";
                    p["comment_start"]="#";
                    p["header_start"]="#!";

                    QStringList roParams;
                    roParams<<"column_separator"<<"decimal_separator"<<"comment_start"<<"header_start";
                    insertProjectRecord("table", fi.fileName(), files[i], "", group, description, folder, p, roParams);
                }
            }



        } else {
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(tr("width an height are not set properly (>0)")));
        }
    }

}

void QFRDRImagingFCSPlugin::insertNandBFromVideoCorrelatorFile(const QString& evalFilename, const QString &filename_overvieww, const QString &filename_overviewstdw, const QString &filenameBack, const QString &filenameBackStd, int internalDualViewMode, int dualViewID, const QString& roleIn, const QString& group)
{

    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview=filename_overvieww;
    QString filename_overviewstd=filename_overviewstdw;
    QString filename_background=filenameBack;
    QString filename_backgroundstddev=filenameBackStd;
    QString filename_settings="";
    QString filename_acquisition="";
    QString role=roleIn;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"IS_OVERVIEW_SCALED"<<"INTERNAL_DUALVIEW_MODE"<<"INTERNAL_DUALVIEW_MODE_CHANNEL";


    QStringList more_files, more_files_types, more_files_descriptions;

    initParams["INTERNAL_DUALVIEW_MODE"]="none";
    if (internalDualViewMode==1) initParams["INTERNAL_DUALVIEW_MODE"]="horizontal";
    if (internalDualViewMode==2) initParams["INTERNAL_DUALVIEW_MODE"]="vertical";
    initParams["INTERNAL_DUALVIEW_MODE_CHANNEL"]=dualViewID;


    //qDebug()<<"insertN&B: evalFilename="<<evalFilename;
    if (QFile::exists(evalFilename)) {
        int width=0;
        int height=0;
        bool ok=true;
        QString description;
        if (!readEvalSettings(evalFilename, false, initParams, paramsReadonly, width, height, more_files, more_files_types, more_files_descriptions, description, filename_settings, filename_acquisition, filename_overview, filename_overviewstd, filename_background, filename_backgroundstddev, role, -1)) {
            ok=false;
        }
        //qDebug()<<"insertN&B: evalFilename="<<evalFilename<<"   ok="<<ok;

        if (ok) {
            QStringList files, files_types, files_descriptions;


            files<<more_files;
            files_types<<more_files_types;
            files_descriptions<<more_files_descriptions;

            if (internalDualViewMode==1) {
                initParams["WIDTH"]=initParams["WIDTH"].toInt()/2;
            } else if (internalDualViewMode==2) {
                initParams["HEIGHT"]=initParams["HEIGHT"].toInt()/2;
            }

            QString folder=QFileInfo(filename_acquisition).baseName();

            /*qDebug()<<"insertN&B: folder="<<folder;
            qDebug()<<"insertN&B: filename_overvieww="<<filename_overvieww<<"  exists: "<<QFile::exists(filename_overvieww);
            qDebug()<<"insertN&B: filename_overviewstdw="<<filename_overviewstdw<<"  exists: "<<QFile::exists(filename_overviewstdw);
            qDebug()<<"insertN&B: filenameBack="<<filenameBack<<"  exists: "<<QFile::exists(filenameBack);
            qDebug()<<"insertN&B: filenameBackStd="<<filenameBackStd<<"  exists: "<<QFile::exists(filenameBackStd);*/

            if (QFile::exists(filename_overvieww) && QFile::exists(filename_overviewstdw)) {
                QFileInfo fi(filename_overvieww);
                //qDebug()<<fi.fileName();
                QStringList ffiles, ffile_types, ffiles_descriptions;
                ffiles<<filename_overvieww<<filename_overviewstdw;
                ffile_types<<"image"<<"image_std";
                ffiles_descriptions<<""<<"";
                if (QFile::exists(filenameBack)) {
                    ffiles<<filenameBack;
                    ffile_types<<"background";
                    ffiles_descriptions<<"";
                }
                if (QFile::exists(filenameBackStd)) {
                    ffiles<<filenameBackStd;
                    ffile_types<<"background_stddev";
                    ffiles_descriptions<<"";
                }
                ffiles<<more_files;
                ffile_types<<more_files_types;
                ffiles_descriptions<<more_files_descriptions;

                QMap<QString,QVariant>& init_params=initParams;
                QStringList init_params_readonly=paramsReadonly;
                init_params["BACKGROUND_CORRECTED"]=true;
                init_params_readonly<<"BACKGROUND_CORRECTED";
                insertProjectRecordFiles("number_and_brightness", fi.fileName()+tr(" - number & brightness"), ffiles, role, group, ffile_types, ffiles_descriptions, description, folder, init_params, init_params_readonly);
            }

            if (QFile::exists(filename_settings)) {
                QFileInfo fi(filename_settings);
                insertProjectRecord("rdr_settings", fi.fileName()+tr(" - acquisition settings"), filename_settings, "", group, description, folder);
            }

            for (int i=0; i<files_types.size(); i++) {
                if (QFile::exists(files[i]) && files_types[i].toLower().trimmed()=="setup_properties") {
                    QFileInfo fi(files[i]);
                    QMap<QString, QVariant> p;
                    p["column_separator"]=",";
                    p["decimal_separator"]=".";
                    p["comment_start"]="#";
                    p["header_start"]="#!";

                    QStringList roParams;
                    roParams<<"column_separator"<<"decimal_separator"<<"comment_start"<<"header_start";
                    insertProjectRecord("table", fi.fileName(), files[i], "", group, description, folder, p, roParams);
                }
            }



        } else {
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename_overvieww).arg(tr("width an height are not set properly (>0)")));
        }
    }
}

void QFRDRImagingFCSPlugin::filenameHeuristics(const QString &filename, QString* evalFilename, bool *isCross, bool *isDCCF, int* dccfid, bool* isJanBFile, QString* role)
{
    QRegExp rxdccf("\\.dccf(\\d+)?\\.(dat|bin)");
    rxdccf.setCaseSensitivity(Qt::CaseInsensitive);
    rxdccf.setMinimal(true);
    QString intRole="";

    if (filename.endsWith(".autocorrelation.dat")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".autocorrelation.dat", ".evalsettings.txt");
            intRole="ACF";
        }
    } else if (filename.endsWith(".acf.dat")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".acf.dat", ".evalsettings.txt");
            intRole="ACF";
        }
    } else if (filename.endsWith(".crosscorrelation.dat")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".crosscorrelation.dat", ".evalsettings.txt");
            intRole="CCF";
        }
        if (isCross) *isCross=true;
    } else if (filename.endsWith(".ccf.dat")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".ccf.dat", ".evalsettings.txt");
            intRole="CCF";
        }
        if (isCross) *isCross=true;
    } else if (filename.endsWith(".qf.dat")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".qf.dat", ".evalsettings.txt");
            intRole="ACF";
        }
        if (isJanBFile) *isJanBFile=true;
    } else if (rxdccf.indexIn(filename)>=0) {//filename.endsWith(".dccf.dat")) {
        if (isDCCF) *isDCCF=true;
        if (rxdccf.cap(1).size()>0 && dccfid) *dccfid=rxdccf.cap(1).toInt();
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(rxdccf, ".evalsettings.txt");
        }
        intRole="DCCF";
    } else if (filename.endsWith(".autocorrelation.bin")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".autocorrelation.bin", ".evalsettings.txt");
        }
        intRole="ACF";
    } else if (filename.endsWith(".acf.bin")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".acf.bin", ".evalsettings.txt");
        }
        intRole="ACF";
    } else if (filename.endsWith(".crosscorrelation.bin")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".crosscorrelation.bin", ".evalsettings.txt");
        }
        if (isCross) *isCross=true;
        intRole="CCF";
    } else if (filename.endsWith(".ccf.bin")) {
        if (evalFilename) {
            *evalFilename=filename;
            *evalFilename=evalFilename->replace(".ccf.bin", ".evalsettings.txt");
        }
        if (isCross) *isCross=true;
        intRole="CCF";
    /*} else if (filename.endsWith(".dccf.bin")) {
        isDCCF=true;
        evalFilename=filename;
        *evalFilename=evalFilename->replace(".dccf.bin", ".evalsettings.txt");*/
    }
    if (role) {
        if (role->isEmpty()) {
            *role=intRole;
            if (*role=="DCCF" && dccfid) *role=QString("DCCF%1").arg(*dccfid);
        }
    }
}

bool QFRDRImagingFCSPlugin::readEvalSettings(const QString &evalFilename, bool isDCCF, QMap<QString, QVariant> &initParams, QStringList &paramsReadonly, int &width, int &height, QStringList &files, QStringList &files_types, QStringList &files_descriptions, QString &description, QString &filename_settings, QString& filename_acquisition, QString filename_overview, QString& filename_overviewstd, QString& filename_background, QString& filename_backgroundstddev, QString& role, int dccfid)
{
    QFile file(evalFilename);
    bool overviewReal=false;
    QDir d=QFileInfo(evalFilename).absoluteDir();
    //qDebug()<<"readEvalSettings: "<<evalFilename;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString line;
        QRegExp reg("(.*)(:\\s*)([+-]?\\d*\\.?\\d+([eE][+-]?\\d+)?)");
        do {
            line = stream.readLine();
            if (reg.indexIn(line)>-1) {
                QString name=reg.cap(1).toLower().trimmed();
                QString value=reg.cap(3);

                //qDebug()<<name <<"  =  "<<value;
                QRegExp rxdccfp("dccf\\s+(\\d+\\s+)?(delta x|delta y|frame width|frame height|role)");
                rxdccfp.setMinimal(true);
                rxdccfp.setCaseSensitivity(Qt::CaseInsensitive);
                bool isDCCFParam=rxdccfp.indexIn(name)>=0;
                QString dccfParam=rxdccfp.cap(2).toLower();
                int dccfParamID=-1;
                if (!rxdccfp.cap(1).isEmpty()) dccfParamID=rxdccfp.cap(1).toInt();

                if (name=="width") {
                    if (!isDCCF) {
                        initParams["WIDTH"]=value.toInt();
                        width=value.toInt();
                        paramsReadonly<<"WIDTH";
                    }
                } else if (name=="height") {
                    if (!isDCCF) {
                        initParams["HEIGHT"]=value.toInt();
                        height=value.toInt();
                        paramsReadonly<<"HEIGHT";
                    }
                } else if (isDCCFParam && dccfParamID==dccfid && dccfParam=="frame width"){ //(name=="dccf frame width") {
                    if (isDCCF) {
                        initParams["WIDTH"]=value.toInt();
                        width=value.toInt();
                        paramsReadonly<<"WIDTH";
                    }
                } else if (isDCCFParam && dccfParamID==dccfid && dccfParam=="frame height"){ //(name=="dccf frame height") {
                    if (isDCCF) {
                        initParams["HEIGHT"]=value.toInt();
                        height=value.toInt();
                        paramsReadonly<<"HEIGHT";
                    }
                } else if (name=="pixel width") {
                    initParams["PIXEL_WIDTH"]=value.toDouble();
                    paramsReadonly<<"PIXEL_WIDTH";
                } else if (name=="pixel height") {
                    initParams["PIXEL_HEIGHT"]=value.toDouble();
                    paramsReadonly<<"PIXEL_HEIGHT";
                } else if (name=="reading frame count") {
                    initParams["FRAME_COUNT"]=value.toInt();
                    paramsReadonly<<"FRAME_COUNT";
                } else if (isDCCFParam && dccfParamID==dccfid && dccfParam=="delta x"){ //(name=="dccf delta x") {
                    initParams["DCCF_DELTAX"]=value.toInt();
                    paramsReadonly<<"DCCF_DELTAX";
                } else if (isDCCFParam && dccfParamID==dccfid && dccfParam=="delta y"){ //(name=="dccf delta y") {
                    initParams["DCCF_DELTAY"]=value.toInt();
                    paramsReadonly<<"DCCF_DELTAY";
                } else if (isDCCFParam && dccfParamID==dccfid && dccfParam=="role"){ //(name=="dccf frame height") {
                    initParams["DCCF_ROLE"]=value;
                    if (isDCCF) role=value;
                    paramsReadonly<<"DCCF_ROLE";
                } else if (name=="frame count") {
                    initParams["FRAME_COUNT"]=value.toInt();
                    paramsReadonly<<"FRAME_COUNT";
                } else if (name=="first frame") {
                    initParams["FIRST_FRAME"]=value.toInt();
                    paramsReadonly<<"FIRST_FRAME";
                } else if (name=="last frame") {
                    initParams["LAST_FRAME"]=value.toInt();
                    paramsReadonly<<"LAST_FRAME";
                } else if (name=="summing over # frames") {
                    initParams["SUMMED_FRAMES"]=value.toInt();
                    paramsReadonly<<"SUMMED_FRAMES";
                } else if (name=="correlation segments") {
                    initParams["SEGMENTS"]=value.toInt();
                    paramsReadonly<<"SEGMENTS";
                } else if (name=="frametime (ms)") {
                    initParams["FRAMETIME_MS"]=value.toInt();
                    paramsReadonly<<"FRAMETIME_MS";
                } else if (name=="frametime (s)") {
                    initParams["FRAMETIME"]=value.toInt();
                    paramsReadonly<<"FRAMETIME";
                } else if (name=="correlator s") {
                    initParams["CORR_S"]=value.toInt();
                    paramsReadonly<<"CORR_S";
                } else if (name=="correlator p") {
                    initParams["CORR_P"]=value.toInt();
                    paramsReadonly<<"CORR_P";
                } else if (name=="correlator m") {
                    initParams["CORR_M"]=value.toInt();
                    paramsReadonly<<"CORR_M";
                } else if (name=="baseline") {
                    initParams["BASELINE"]=value.toDouble();
                    paramsReadonly<<"BASELINE";
                } else if (name=="statistics over") {
                    initParams["STATISTICS_SUMFRAMES"]=value.toInt();
                    paramsReadonly<<"STATISTICS_SUMFRAMES";
                } else if (name=="duration [s]") {
                    initParams["EVALUATION_DURATION"]=value.toInt();
                    paramsReadonly<<"EVALUATION_DURATION";
                } else if (name=="binning") {
                    initParams["BINNING"]=value.toInt();
                    paramsReadonly<<"BINNING";
                } else if (name=="crop x0") {
                    initParams["CROP_X0"]=value.toInt();
                    paramsReadonly<<"CROP_X0";
                } else if (name=="crop x1") {
                    initParams["CROP_X1"]=value.toInt();
                    paramsReadonly<<"CROP_X1";
                } else if (name=="crop y0") {
                    initParams["CROP_Y0"]=value.toInt();
                    paramsReadonly<<"CROP_Y0";
                } else if (name=="crop y1") {
                    initParams["CROP_Y1"]=value.toInt();
                    paramsReadonly<<"CROP_Y1";
                } else if (name=="bleach correction") {
                    initParams["BLEACH_CORRECTION"]=value;
                    paramsReadonly<<"BLEACH_CORRECTION";
                } else if (name=="bleach decay constant [s]") {
                    initParams["BLEACH_CORRECTION_DECAY_SECS"]=value.toDouble();
                    paramsReadonly<<"BLEACH_CORRECTION_DECAY_SECS";
                } else if (name=="bleach decay constant [pix]") {
                    initParams["BLEACH_CORRECTION_DECAY_PIX"]=value.toDouble();
                    paramsReadonly<<"BLEACH_CORRECTION_DECAY_PIX";
                } else if (name=="bleach offset B") {
                    initParams["BLEACH_CORRECTION_FIT_OFFSET"]=value.toInt();
                    paramsReadonly<<"BLEACH_CORRECTION_FIT_OFFSET";
                } else if (name=="bleach amplitude A") {
                    initParams["BLEACH_CORRECTION_FIT_AMPLITUDE"]=value.toInt();
                    paramsReadonly<<"BLEACH_CORRECTION_FIT_AMPLITUDE";
                } else if (name=="video uncorrected avgmin") {
                    initParams["UNCORRECTEDVIDEO_MIN"]=value.toDouble();
                    paramsReadonly<<"UNCORRECTEDVIDEO_MIN";
                } else if (name=="video uncorrected avgmax") {
                    initParams["UNCORRECTEDVIDEO_MAX"]=value.toDouble();
                    paramsReadonly<<"UNCORRECTEDVIDEO_MAX";
                } else if (name=="video sum up") {
                    initParams["VIDEO_AVGFRAMES"]=value.toInt();
                    paramsReadonly<<"VIDEO_AVGFRAMES";
                } else if (name=="video avgmin") {
                    initParams["VIDEO_AVGMIN"]=value.toDouble();
                    paramsReadonly<<"VIDEO_AVGMIN";
                } else if (name=="video avgmax") {
                    initParams["VIDEO_AVGMAX"]=value.toDouble();
                    paramsReadonly<<"VIDEO_AVGMAX";

                /*} else if (name=="") {
                    initParams[""]=value.toInt();
                    paramsReadonly<<"";*/
                }
            }
            int colon_idx=line.indexOf(':');
            if (colon_idx>=0) {
                QString name=line.left(colon_idx).toLower().trimmed();
                QString value=line.mid(colon_idx+1).trimmed();
                //qDebug()<<name<<value;
                if (name=="overview image file") {
                    if (!overviewReal) filename_overview=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="overview image file real") {
                    filename_overview=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    overviewReal=true;
                } else if (name=="overview std image") {
                    filename_overviewstd=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="background image file") {
                    filename_background=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="background stddev") {
                    filename_backgroundstddev=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="video file") {
                    QString filename_video=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_video)) {
                        files<<filename_video;
                        files_types<<"video";
                        files_descriptions<<tr("time-binned video");
                    }
                } else if (name=="uncorrected video file") {
                    QString filename_videoUncorrected=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_videoUncorrected)) {
                        files<<filename_videoUncorrected;
                        files_types<<"video_uncorrected";
                        files_descriptions<<tr("uncorrected time-binned video");
                    }
                } else if (name=="statistics file") {
                    QString filename_statistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics)) {
                        files<<filename_statistics;
                        files_types<<"statistics";
                        files_descriptions<<tr("statistics data");
                    }
                } else if (name=="background statistics file") {
                    QString filename_backstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics)) {
                        files<<filename_backstatistics;
                        files_types<<"background_statistics";
                        files_descriptions<<tr("background statistics data");
                    }
                } else if (name=="uncorrected statistics file") {
                    QString filename_uncorrectedstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics)) {
                        files<<filename_uncorrectedstatistics;
                        files_types<<"uncorrected_statistics";
                        files_descriptions<<tr("uncorrected statistics data");
                    }
                } else if (name=="statistics file dv1") {
                    QString filename_statistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics_dv1)) {
                        files<<filename_statistics_dv1;
                        files_types<<"statistics_dv1";
                        files_descriptions<<tr("statistics data, DualView side 1");
                    }
                } else if (name=="background statistics file dv1") {
                    QString filename_backstatistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics_dv1)) {
                        files<<filename_backstatistics_dv1;
                        files_types<<"background_statistics_dv1";
                        files_descriptions<<tr("background statistics data, DualView side 1");
                    }
                } else if (name=="uncorrected statistics file dv1") {
                    QString filename_uncorrectedstatistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics_dv1)) {
                        files<<filename_uncorrectedstatistics_dv1;
                        files_types<<"uncorrected_statistics_dv1";
                        files_descriptions<<tr("uncorrected statistics data, DualView side 1");
                    }
                } else if (name=="statistics file dv2") {
                    QString filename_statistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics_dv2)) {
                        files<<filename_statistics_dv2;
                        files_types<<"statistics_dv2";
                        files_descriptions<<tr("statistics data, DualView side 2");
                    }
                } else if (name=="background statistics file dv2") {
                    QString filename_backstatistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics_dv2)) {
                        files<<filename_backstatistics_dv2;
                        files_types<<"background_statistics_dv2";
                        files_descriptions<<tr("background statistics data, DualView side 2");
                    }
                } else if (name=="uncorrected statistics file dv2") {
                    QString filename_uncorrectedstatistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics_dv2)) {
                        files<<filename_uncorrectedstatistics_dv2;
                        files_types<<"uncorrected_statistics_dv2";
                        files_descriptions<<tr("uncorrected statistics data, DualView side 2");
                    }
                } else if (name=="input description file") {
                    filename_settings=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="input file") {
                    filename_acquisition=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                } else if (name=="mask file") {
                    QString filename_mask=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_mask)) {
                        files<<filename_mask;
                        files_types<<"mask";
                        files_descriptions<<tr("mask");
                    }
                } else if (name=="date/time") {
                    initParams["CORRELATION_DATE"]=value;
                    paramsReadonly<<"CORRELATION_DATE";
                } else {
                    QString fn=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(fn) && ((QFileInfo(fn).suffix().toLower()=="tif")||(QFileInfo(fn).suffix().toLower()=="tiff"))) {
                        files<<fn;
                        files_types<<"display_image";
                        files_descriptions<<name;
                    }
                }
            }
        } while (!line.isNull());
    } else {
        return false;
    }



    if (QFile::exists(filename_overview)) {
        files<<filename_overview;
        files_types<<"overview";
        files_descriptions<<tr("average image");
    }
    initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
    paramsReadonly<<"IS_OVERVIEW_SCALED";
    if (QFile::exists(filename_overviewstd)) {
        files<<filename_overviewstd;
        files_types<<"overview_std";
        files_descriptions<<tr("standard deviation for overview image");
    }
    if (QFile::exists(filename_background)) {
        files<<filename_background;
        files_types<<"background";
        files_descriptions<<tr("background frame");
    }
    if (QFile::exists(filename_backgroundstddev)) {
        files<<filename_backgroundstddev;
        files_types<<"background_stddev";
        files_descriptions<<tr("background standard deviation frame");
    }
    if (QFile::exists(filename_settings)) {
        files<<filename_settings;
        files_types<<"acquisition_settings";
        files_descriptions<<tr("acquisition settings");
    }
    if (QFile::exists(filename_acquisition)) {
        files<<filename_acquisition;
        files_types<<"input";
        files_descriptions<<tr("input dataset");
    }

    bool ok=true;
    if (width<=0) {
        bool okk=true;
        int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("width = "), 0, 1, 2147483647, 1, &okk);
        if (okk) { width=wwidth;  } else ok=false;
    }
    if (ok && (height<=0)) {
        bool okk=true;
        int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("height = "), 0, 1, 2147483647, 1, &okk);
        if (okk) { height=wwidth; } else ok=false;
    }
    if (ok) {
        description="";
        parseSPIMSettings(filename_settings,  description,  initParams,  paramsReadonly,  files,  files_types,  files_descriptions);


        initParams["WIDTH"]=width;
        initParams["HEIGHT"]=height;

        initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
        paramsReadonly<<"IS_OVERVIEW_SCALED";
    }

    // if this is an FCCS record, we don't want the DCCS_DELTAX/Y properties, as this the whift by half an image represents NO shift between the foci!
    if (role.toUpper().startsWith("FCCS")) {
        initParams["FCCS_CORRELATIONCHIFTX"]=initParams.value("DCCF_DELTAX", 0);
        initParams["FCCS_CORRELATIONCHIFTY"]=initParams.value("DCCF_DELTAY", 0);
        paramsReadonly<<"FCCS_CORRELATIONCHIFTX"<<"FCCS_CORRELATIONCHIFTY";
        initParams.remove("DCCF_DELTAX");
        initParams.remove("DCCF_DELTAY");
    }
    return ok;
}



void QFRDRImagingFCSPlugin::insertRH2CorFile(const QString& filename, const QString& role) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"IS_OVERVIEW_SCALED";

    bool ok=true, okk=true;

    int roi_height=32;

    QString configFile=findB040ExperimentDescriptionForData(filename);
    if (QFile::exists(configFile)) {
        QSettings settings(configFile, QSettings::IniFormat);
        int roi_start=settings.value("acquisition/acquisition/roi_ystart", settings.value("acquisition/acquisition/roi_xstart", settings.value("acquisition/roi_ystart", settings.value("acquisition/roi_xstart", 0)))).toInt();
        int roi_end=settings.value("acquisition/acquisition/roi_yend", settings.value("acquisition/acquisition/roi_xend", settings.value("acquisition/roi_yend", settings.value("acquisition/roi_xend", 31)))).toInt();
        roi_height=roi_end-roi_start+1;
        if (roi_height<=0) roi_height=32;
        /*qDebug()<<"read RH2 config: "<<configFile;
        qDebug()<<"roi_start:  "<<roi_start;
        qDebug()<<"roi_end:  "<<roi_end;
        qDebug()<<"roi_height:  "<<roi_height;*/
    }

    int height=QInputDialog::getInt(NULL, tr("Radhard2 image properties"), tr("Height [ROI]= "), roi_height, 1, 100000, 1, &okk);
    ok=ok&&okk;
    int width=32;
    ok=ok&&okk;

    // set whatever you want (FILETYPE is just an example)!
    initParams["FILETYPE"]="RADHARD2";
    initParams["WIDTH"]=width;
    initParams["HEIGHT"]=height;
    initParams["IS_OVERVIEW_SCALED"]=false;

    paramsReadonly<<"WIDTH"<<"HEIGHT";

    QString filenameValid=filename+".valid";

    //First we check the file
    if(QFile::exists(filename)){
        yaid_rh::frameAnalyzer *fa=new yaid_rh::frameAnalyzer(filename.toLocal8Bit().constData());
        fa->findFrames(0,1024*1024,NULL,1);
        fa->findGhostFrames(1);
        if(QFile::exists(filenameValid)){
            QMessageBox::StandardButtons res=QMessageBox::critical(NULL,tr("RH2 Correlation File Importer"),tr("The analyzed file %1 already exists.\nDelete?").arg(filenameValid), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
            if (res==QMessageBox::Yes) {
                if (!QFile::remove(filenameValid)) {
                    QMessageBox::critical(NULL,tr("RH2 Correlation File Importer"),tr("Could not delete the file %1!\nPlease delete by hand.").arg(filenameValid));
                    return;
                }
            }
        }
            fa->writeFileFromMem(filenameValid.toLocal8Bit().constData(),1);

        int steps=fa->getSteps();
        delete fa;
        fa=NULL;

        initParams["STEPS"]=steps;
        paramsReadonly<<"STEPS";
    }

    if (ok && QFile::exists(filename)) {
        QStringList files, files_types,files_descriptions;
        files<<filename+".valid";
        files_types<<"acf";
        files_descriptions<<tr("correlation data from SPAD array");
        QString description="";
        QString filename_settings=configFile;
        if (QFile::exists(filename_settings)) parseSPIMSettings(filename_settings,  description,  initParams,  paramsReadonly,  files,  files_types,  files_descriptions);


        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - %1").arg(role), role, files,            initParams,         paramsReadonly, files_types);
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }

}

Q_EXPORT_PLUGIN2(imaging_fcs, QFRDRImagingFCSPlugin)

