/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"
#include "yaid_rh.h"
#include "qfrdrimagingfcssimulator.h"

QFRDRImagingFCSPlugin* QFRDRImagingFCSPlugin::instance=NULL;

QFRDRImagingFCSPlugin::QFRDRImagingFCSPlugin(QObject* parent):
    QObject(parent), QFPluginRawDataRecordBase()
{
    if (instance==NULL) instance=this;
    //constructor
    dlgCorrelate=NULL;
    dlgSimulate=NULL;
}

QFRDRImagingFCSPlugin::~QFRDRImagingFCSPlugin()
{
    if (instance==this) instance=NULL;
}

void QFRDRImagingFCSPlugin::deinit() {
}

void QFRDRImagingFCSPlugin::init()
{
    QMenu* tmenu=services->getMenu("tools")->addMenu(QIcon(getIconFilename()), tr("imaging FCS tools"));
    QAction* actOffset=new QAction(tr("subtract &offset of correlation functions"), parentWidget);
    connect(actOffset, SIGNAL(triggered()), this, SLOT(correctOffset()));
    tmenu->addAction(actOffset);

    QFPluginServices::getInstance()->addToHelpFurtherReading("$$INVISIBLEREF_SPIMFCCS$$$$INVISIBLEREF_WOHLANDSPIMFCS$$$$INVISIBLEREF_RIES$$");
    QFPluginServices::getInstance()->appendOrAddHTMLReplacement("FILEFORMATS_LIST", QString("<li><b>ImagingFCS:</b><ul>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/imfcs_dataformats.html#formats_acf\">CSV autocorrelation data</a></li>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/imfcs_dataformats.html#formats_binacf\">binary autocorrelation data</a></li>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/imfcs_dataformats.html#formats_binccf\">binary cross-correlation data</a></li>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/imfcs_dataformats.html#formats_evalsettings\">evaluation settings report</a></li>\n"
                                                                                            "</ul></li>\n").arg(getID()));


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

void QFRDRImagingFCSPlugin::correctOffset()
{

    QFMatchRDRFunctorSelectType* sel=new QFMatchRDRFunctorSelectType("imaging_fcs");
    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(sel,true, parentWidget);
    dlg->setWindowTitle(tr("correct offset in FCS records ..."));
    dlg->setAllowMultiSelect(true);
    dlg->setAllowCreateNew(false);
    dlg->setOnlineHelp(services->getPluginHelpDirectory("imaging_fcs")+"/tutorial_offset.html");

    QFDoubleEdit* edtOffset=new QFDoubleEdit(dlg);
    edtOffset->setValue(0);
    edtOffset->setCheckBounds(false, false);
    dlg->addWidget(tr("offset:"), edtOffset);


    if (dlg->exec()) {
        QList<QPointer<QFRawDataRecord> > sel=dlg->getSelectedRDRs();
        for (int i=0; i<sel.size(); i++) {
            QFRDRImagingFCSData* fcs=qobject_cast<QFRDRImagingFCSData*>(sel[i]);
            if (fcs) {
                fcs->setQFProperty(QString("CORR_OFFSET"), edtOffset->value(), true, true);
            }
        }
        if (QMessageBox::information(parentWidget, tr("corect correlation offset"), tr("The project needs to be saved and reloaded for the offset correction to take effect!\nReload project now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
            QFPluginServices::getInstance()->reloadCurrentProject();
        }

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
        QString format_RH2Cor=tr("RH2 hw correlator dump (*.cor.dat *.valid)");
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
                QString group="";
                QString infile;
                int dv2mode=getEvalsettingsDV2Mode(evalsettings, filename, &isFCCS, &isACF, &infile, &group);
                infile=QFileInfo(infile).completeBaseName();
                if (group.isEmpty()) group=infile;
                if (group.isEmpty()) group=filename;
                if ((dv2mode==0) || !isFCCS || (isFCCS && dv2mode!=1 && dv2mode!=2)) {
                    insertVideoCorrelatorFile(filename, overview, evalsettings, false, "", 0, 0, true, true, group);
                } else if (dv2mode==1||dv2mode==2) {
                    if (isFCCS) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("FCCS"), dv2mode, 0, true, true, group);
                    } else if (isACF) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("ACF0"), dv2mode, 0, true, true, group);
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString("ACF1"), dv2mode, 1, true, true, group);
                    } else {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, false, QString(""), dv2mode, 0, true, true, group);
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
                QString infile, group="";
                int dv2mode=getEvalsettingsDV2Mode(evalsettings, filename, &isFCCS, &isACF, &infile, &group);
                infile=QFileInfo(infile).completeBaseName();
                if (group.isEmpty()) group=infile;
                if (group.isEmpty()) group=filename;

                //qDebug()<<"isFCCS: "<<isFCCS;
                //qDebug()<<"isACF: "<<isACF;
                //qDebug()<<"dv2mode: "<<dv2mode;
                if (dv2mode==1||dv2mode==2) {
                    if (isFCCS) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("FCCS"), dv2mode, 0, true, true, group);
                    } else if (isACF) {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("ACF0"), dv2mode, 0, true, true, group);
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString("ACF1"), dv2mode, 1, true, true, group);
                    } else {
                        insertVideoCorrelatorFile(filename, overview, evalsettings, true, QString(""), dv2mode, 0, true, true, group);
                    }
                } else {
                    insertVideoCorrelatorFile(filename, overview, evalsettings, true, "", 0, 0, true, true, group);
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

QFRawDataRecord* QFRDRImagingFCSPlugin::insertProjectRecord(const QString &type, const QString &name, const QString &filename, const QString& role, const QString& group, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly, const QMap<QString, QVariant> &inputfile_props, const QString &inputfile_comment) {
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

        QMap<QString,QVariant> ipar=inputfile_props;
        QStringList iparro=inputfile_props.keys();
        iparro.append(init_params_readonly);
        QMapIterator<QString,QVariant> it(init_params);
        while (it.hasNext()) {
            it.next();
            ipar[it.key()]=it.value();
        }


        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, role, files,  ipar, iparro, QStringList(), QStringList());
        e->setFolder(directory);
        e->setDescription(description+"\n\n"+inputfile_comment);
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

QFRawDataRecord* QFRDRImagingFCSPlugin::insertProjectRecordFiles(const QString &type, const QString &name, const QStringList &files, const QString& role, const QString& group, const QStringList &filetypes, const QStringList &filedescriptions, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly, const QMap<QString, QVariant> &inputfile_props, const QString &inputfile_comment) {
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
        QMap<QString,QVariant> ipar=inputfile_props;
        QStringList iparro=inputfile_props.keys();
        iparro.append(init_params_readonly);
        QMapIterator<QString,QVariant> it(init_params);
        while (it.hasNext()) {
            it.next();
            ipar[it.key()]=it.value();
        }

        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, role, files,  ipar, iparro, filetypes, filedescriptions);
        e->setFolder(directory);
        e->setDescription(description+"\n\n"+inputfile_comment);
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

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QString& filename_overvieww, const QString& filename_evalsettings, bool binary, const QString& roleIn, int internalDualViewMode, int dualViewID, bool addCorrelations, bool adNandB, const QString& group, const QMap<QString, QVariant>& props, const QString& comment) {

    // here we store some initial parameters
    QMap<QString, QVariant> initParams=props;
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
    QStringList paramsReadonly=props.keys();
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
                    if (!description.isEmpty() || !comment.isEmpty()) e->setDescription(description+"\n\n"+comment);
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
                        if (!description.isEmpty() || !comment.isEmpty()) e->setDescription(description+"\n\n"+comment);

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
                        if (!description.isEmpty() || !comment.isEmpty()) e->setDescription(description+"\n\n"+comment);
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
                    if (!description.isEmpty() || !comment.isEmpty()) e->setDescription(description+"\n\n"+comment);
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
                insertProjectRecordFiles("number_and_brightness", fi.fileName()+tr(" - number & brightness"), ffiles, "", group, ffile_types, ffiles_descriptions, description, folder, init_params, init_params_readonly, props, comment);
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

void QFRDRImagingFCSPlugin::insertNandBFromVideoCorrelatorFile(const QString& evalFilename, const QString &filename_overvieww, const QString &filename_overviewstdw, const QString &filenameBack, const QString &filenameBackStd, int internalDualViewMode, int dualViewID, const QString& roleIn, const QString& group, const QMap<QString, QVariant> &props, const QString &comment)
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
                insertProjectRecordFiles("number_and_brightness", fi.fileName()+tr(" - number & brightness"), ffiles, role, group, ffile_types, ffiles_descriptions, description, folder, init_params, init_params_readonly, props, comment);
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

    imFCSFilenameHeuristics(filename, evalFilename, isCross, isDCCF, dccfid, isJanBFile, role);

}

bool QFRDRImagingFCSPlugin::readEvalSettings(const QString &evalFilename, bool isDCCF, QMap<QString, QVariant> &initParams, QStringList &paramsReadonly, int &width, int &height, QStringList &files, QStringList &files_types, QStringList &files_descriptions, QString &description, QString &filename_settings, QString& filename_acquisition, QString filename_overview, QString& filename_overviewstd, QString& filename_background, QString& filename_backgroundstddev, QString& role, int dccfid)
{

    bool overviewReal=true;
    bool ok=readEvalSettingsFile(evalFilename, isDCCF, initParams, paramsReadonly, width, height, files, files_types, files_descriptions, filename_settings, filename_acquisition, filename_overview, filename_overviewstd, filename_background, filename_backgroundstddev, role, dccfid, overviewReal);
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

