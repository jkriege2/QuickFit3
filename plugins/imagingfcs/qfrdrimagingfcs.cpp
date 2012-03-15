#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"

QFRDRImagingFCSPlugin::QFRDRImagingFCSPlugin(QObject* parent):
    QObject(parent), QFPluginRawDataRecordBase()
{
    //constructor
    dlgCorrelate=NULL;
}

QFRDRImagingFCSPlugin::~QFRDRImagingFCSPlugin()
{
}

void QFRDRImagingFCSPlugin::deinit() {
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
}

void QFRDRImagingFCSPlugin::correlateAndInsert() {
    if (dlgCorrelate) dlgCorrelate->show();
    else if (project && settings) {
        dlgCorrelate=new QFRDRImagingFCSCorrelationDialog(services, settings, parentWidget);
        dlgCorrelate->setProject(project);
        dlgCorrelate->setWindowModality(Qt::WindowModal);
        //qDebug()<<parentWidget,
        dlgCorrelate->show();

        connect(dlgCorrelate, SIGNAL(finished(int)), this, SLOT(importCorrelationsFromDialog()));

    }
}

void QFRDRImagingFCSPlugin::importCorrelationsFromDialog() {

    disconnect(dlgCorrelate, SIGNAL(finished(int)), this, SLOT(importCorrelationsFromDialog()));

    QStringList list=dlgCorrelate->getFilesToAdd();

    QStringList::Iterator it = list.begin();
    services->setProgressRange(0, list.size());
    services->setProgress(0);
    int i=0;
    QModernProgressDialog progress(tr("Loading imFCS Data ..."), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.open();
    while(it != list.end()) {
        i++;
        services->log_text(tr("loading '%1' ...\n").arg(*it));
        progress.setLabelText(tr("loading '%1' ...\n").arg(*it));
        QString filename=*it;
        QString overview="";
        QApplication::processEvents();
        insertVideoCorrelatorFile(filename, overview, filename.toLower().endsWith(".bin"));
        settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
        services->setProgress(i);
        QApplication::processEvents();
        ++it;
    }
    progress.accept();
    services->setProgress(0);

    dlgCorrelate->deleteLater();
    dlgCorrelate=NULL;
}


void QFRDRImagingFCSPlugin::insertRecord() {
    if (project) {
        // file format to import
        QString format_binVideoCorrelator=tr("binary imFCS Correlations (*.autocorrelation.bin *.acf.bin *.crosscorrelation.bin *.ccf.bin *.dccf.bin)");
        QString format_binVideoCorrelator_short=tr("binary imFCS correlation data");
        QString format_videoCorrelator=tr("CSV imFCS Correlations (*.autocorrelation.dat *.acf.dat *.crosscorrelation.dat *.ccf.dat *.dccf.dat *.qf.dat)");
        QString format_videoCorrelator_short=tr("CSV imFCS correlation data");
        QString format_Radhard2=tr("SPAD array Correlations (*.dat)");
        QString format_Radhard2_short=tr("SPAD array data");
        QStringList formats, formats_short;
        formats<<format_binVideoCorrelator<<format_videoCorrelator<<format_Radhard2;
        formats_short<<format_binVideoCorrelator_short<<format_videoCorrelator_short<<format_Radhard2_short;
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
                QString overview=filename;
                overview=overview.replace(".autocorrelation.dat", ".overview.tif");
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".crosscorrelation.dat", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".dccf.dat", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".ccf.dat", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".acf.dat", ".overview.tif");
                }
                if (!QFile::exists(overview)) overview="";
                insertVideoCorrelatorFile(filename, overview, false);
            } else if (current_format_name==format_binVideoCorrelator) {
                QString filename=*it;
                QString overview=filename;
                overview=overview.replace(".autocorrelation.bin", ".overview.tif");
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".crosscorrelation.bin", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".dccf.bin", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".ccf.bin", ".overview.tif");
                }
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".acf.bin", ".overview.tif");
                }
                if (!QFile::exists(overview)) overview="";
                insertVideoCorrelatorFile(filename, overview, true);
            } else if (current_format_name==format_Radhard2) {
                QString filename=*it;
                insertRadhard2File(filename);
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

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QString& filename_overvieww, bool binary) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview=filename_overvieww;
    QString filename_background="";
    QString filename_statistics="";
    QString filename_settings="";
    QString filename_acquisition="";
    QString filename_mask="";
    QString filename_video="";

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE";

    bool ok=true;
    bool isJanBFile=false;



    // set whatever you want (FILETYPE is just an example)!
    if (binary) initParams["FILETYPE"]="VIDEO_CORRELATOR_BIN";
    else initParams["FILETYPE"]="VIDEO_CORRELATOR";
    QString evalFilename="";
    bool isCross=false;
    bool isDCCF=false;
    if (filename.endsWith(".autocorrelation.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".autocorrelation.dat", ".evalsettings.txt");
    } else if (filename.endsWith(".acf.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".acf.dat", ".evalsettings.txt");
    } else if (filename.endsWith(".crosscorrelation.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".crosscorrelation.dat", ".evalsettings.txt");
        isCross=true;
    } else if (filename.endsWith(".ccf.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".ccf.dat", ".evalsettings.txt");
        isCross=true;
    } else if (filename.endsWith(".qf.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".qf.dat", ".evalsettings.txt");
        isJanBFile=true;
    } else if (filename.endsWith(".dccf.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".dccf.dat", ".evalsettings.txt");
        isDCCF=true;
    } else if (filename.endsWith(".autocorrelation.bin")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".autocorrelation.bin", ".evalsettings.txt");
    } else if (filename.endsWith(".acf.bin")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".acf.bin", ".evalsettings.txt");
    } else if (filename.endsWith(".crosscorrelation.bin")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".crosscorrelation.bin", ".evalsettings.txt");
        isCross=true;
    } else if (filename.endsWith(".ccf.bin")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".ccf.bin", ".evalsettings.txt");
        isCross=true;
    } else if (filename.endsWith(".dccf.bin")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".dccf.bin", ".evalsettings.txt");
        isDCCF=true;
    }
    //qDebug()<<filename<<isJanBFile;

    if (QFile::exists(filename)) {
        int width=0;
        int height=0;
        if (QFile::exists(evalFilename)) {
            QFile file(evalFilename);
            QDir d=QFileInfo(evalFilename).absoluteDir();
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
                         } else if (name=="dccf frame width") {
                            if (isDCCF) {
                                initParams["WIDTH"]=value.toInt();
                                width=value.toInt();
                                paramsReadonly<<"WIDTH";
                            }
                        } else if (name=="dccf frame height") {
                            if (isDCCF) {
                                initParams["HEIGHT"]=value.toInt();
                                height=value.toInt();
                                paramsReadonly<<"HEIGHT";
                            }
                        } else if (name=="reading frame count") {
                            initParams["FRAME_COUNT"]=value.toInt();
                            paramsReadonly<<"FRAME_COUNT";
                        } else if (name=="dccf delta x") {
                            initParams["DCCF_DELTAX"]=value.toInt();
                            paramsReadonly<<"DCCF_DELTAX";
                        } else if (name=="dccf delta y") {
                            initParams["DCCF_DELTAY"]=value.toInt();
                            paramsReadonly<<"DCCF_DELTAY";
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
                        } else if (name=="video sum up") {
                            initParams["VIDEO_AVGFRAMES"]=value.toInt();
                            paramsReadonly<<"VIDEO_AVGFRAMES";
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
                            filename_overview=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="background image file") {
                            filename_background=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="video file") {
                            filename_video=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="statistics file") {
                            filename_statistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="input description file") {
                            filename_settings=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="input file") {
                            filename_acquisition=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="mask file") {
                            filename_mask=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="date/time") {
                            initParams["CORRELATION_DATE"]=value;
                            paramsReadonly<<"CORRELATION_DATE";
                        }
                    }
                } while (!line.isNull());
            }
        }

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
            QStringList files, files_types, files_descriptions;
            if (QFile::exists(filename_overview)) {
                files<<filename_overview;
                files_types<<"overview";
            }
            if (QFile::exists(filename_video)) {
                files<<filename_video;
                files_types<<"video";
            }
            if (QFile::exists(filename_statistics)) {
                files<<filename_statistics;
                files_types<<"statistics";
            }
            if (QFile::exists(filename_mask)) {
                files<<filename_mask;
                files_types<<"mask";
            }
            if (QFile::exists(filename_settings)) {
                files<<filename_settings;
                files_types<<"acquisition_settings";
            }
            if (QFile::exists(filename_acquisition)) {
                files<<filename_acquisition;
                files_types<<"input";
            }
            if (QFile::exists(filename_background)) {
                files<<filename_background;
                files_types<<"background";
            }

            if (QFile::exists(filename_settings)) {
                QSettings settings(filename_settings, QSettings::IniFormat);
                appendCategorizedFilesFromB040SPIMConfig(settings, files, files_types, files_descriptions);
            }

            initParams["WIDTH"]=width;
            initParams["HEIGHT"]=height;


            int columns=1;
            if (!binary) columns=checkColumns(filename);
            if (!isCross && !isDCCF) {
                if (!binary) {
                    initParams["CORRELATION_COLUMN"]=1;
                    if (isJanBFile) initParams["CORRELATION_COLUMN"]=7;
                }
                files.prepend(filename);
                files_types.prepend("acf");
                //qDebug()<<"filename_overview: "<<filename_overview;

                if (columns>2 && !isJanBFile && !binary){
                    initParams["CORRELATION_ERROR_COLUMN"]=2;
                }
                // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - ACF"), files, initParams, paramsReadonly, files_types);
                if (!filename_acquisition.isEmpty()) {
                    e->setFolder(QFileInfo(filename_acquisition).baseName());
                }
                if (e->error()) { // when an error occured: remove record and output an error message
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                }
            } else if (isCross && !binary) {

                files.prepend(filename);
                files_types.prepend("ccf");
                for (int c=1; c<=qMin(4,columns); c++) {
                    initParams["CORRELATION_COLUMN"]=c;

                    if (columns>4) initParams["CORRELATION_ERROR_COLUMN"]=c+4;
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - CCF %1").arg(c), files, initParams, paramsReadonly, files_types);
                    if (!filename_acquisition.isEmpty()) {
                        e->setFolder(QFileInfo(filename_acquisition).baseName());
                    }
                    if (e->error()) { // when an error occured: remove record and output an error message
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        project->deleteRawData(e->getID());
                    }
                }
            } else if (isCross && binary) {

                files.prepend(filename);
                files_types.prepend("ccf");
                for (int c=1; c<=4; c++) {
                    initParams["CORRELATION_SET"]=c-1;

                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - CCF %1").arg(c), files, initParams, paramsReadonly, files_types);
                    if (!filename_acquisition.isEmpty()) {
                        e->setFolder(QFileInfo(filename_acquisition).baseName());
                    }
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

                if (columns>2 && !binary) initParams["CORRELATION_ERROR_COLUMN"]=2;
                // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - DCCF"), files, initParams, paramsReadonly, files_types);
                if (!filename_acquisition.isEmpty()) {
                    e->setFolder(QFileInfo(filename_acquisition).baseName());
                }
                if (e->error()) { // when an error occured: remove record and output an error message
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                }
            }


        } else {
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(tr("width an height are not set properly (>0)")));
        }
    }

}



void QFRDRImagingFCSPlugin::insertRadhard2File(const QString& filename) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE";

    //bool ok=true;



    // set whatever you want (FILETYPE is just an example)!
    initParams["FILETYPE"]="RADHARD2";

    if (QFile::exists(filename)) {
        QStringList files, files_types;
        files<<filename;
        files_types<<"acf";
        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - ACF"), files,            initParams,         paramsReadonly, files_types);
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }

}

Q_EXPORT_PLUGIN2(imaging_fcs, QFRDRImagingFCSPlugin)

