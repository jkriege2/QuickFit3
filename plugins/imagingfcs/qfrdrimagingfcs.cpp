#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"
#include "qmodernprogresswidget.h"
#include "qfrdrimagingfcstools.h"
#include "yaid_rh.h"

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
        dlgCorrelate->setWindowModality(Qt::NonModal);
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
            } else if (current_format_name==format_RH2Cor) {
                QString filename=*it;
                insertRH2CorFile(filename);
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

void QFRDRImagingFCSPlugin::insertProjectRecord(const QString &type, const QString &name, const QString &filename, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly) {
    bool found=false;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord*  r=project->getRawDataByNum(i);
        if (r->getType()==type && r->getName()==name && r->getFolder()==directory) {
            found=true;
            break;
        }
    }
    if (!found && project->getRawDataRecordFactory()->contains(type)) {
        QStringList files, files_types;
        files<<filename;

        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, files,  init_params, init_params_readonly, QStringList(), QStringList());
        e->setFolder(directory);
        e->setDescription(description);
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
}

void QFRDRImagingFCSPlugin::insertProjectRecordFiles(const QString &type, const QString &name, const QStringList &files, const QStringList &filetypes, const QStringList &filedescriptions, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly) {
    bool found=false;
    for (int i=0; i<project->getRawDataCount(); i++) {
        QFRawDataRecord*  r=project->getRawDataByNum(i);
        if (r->getType()==type && r->getName()==name && r->getFolder()==directory) {
            found=true;
            break;
        }
    }
    if (!found && project->getRawDataRecordFactory()->contains(type)) {

        // insert new record:                  type ID, name for record,                                  list of files,    initial parameters, which parameters are readonly?
        QFRawDataRecord* e=project->addRawData(type, name, files,  init_params, init_params_readonly, filetypes, filedescriptions);
        e->setFolder(directory);
        e->setDescription(description);
        if (e->error()) { // when an error occured: remove record and output an error message
            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(files.value(0, "---")).arg(e->errorDescription()));
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(files.value(0, "---")).arg(e->errorDescription()));
            project->deleteRawData(e->getID());
        }
    }
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

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QString& filename_overvieww, bool binary) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview=filename_overvieww;
    QString filename_overviewstd="";
    QString filename_background="";
    QString filename_backgroundstddev="";
    QString filename_statistics="";
    QString filename_backstatistics="";
    QString filename_uncorrectedstatistics="";
    QString filename_settings="";
    QString filename_acquisition="";
    QString filename_mask="";
    QString filename_video="";

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"IS_OVERVIEW_SCALED";

    bool ok=true;
    bool isJanBFile=false;

    QStringList more_files, more_files_types, more_files_descriptions;


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
        bool overviewReal=false;
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
                        } else if (name=="pixel width") {
                            initParams["PIXEL_WIDTH"]=value.toDouble();
                            paramsReadonly<<"PIXEL_WIDTH";
                        } else if (name=="pixel height") {
                            initParams["PIXEL_HEIGHT"]=value.toDouble();
                            paramsReadonly<<"PIXEL_HEIGHT";
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
                        } else if (name=="video avgmin") {
                            initParams["VIDEO_AVGMIN"]=value.toDouble();
                            paramsReadonly<<"VIDEO_AVGMIN";
                        } else if (name=="video avgmax") {
                            initParams["VIDEO_AVGMAX"]=value.toDouble();
                            paramsReadonly<<"VIDEO_AVGMAX";
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
                            filename_video=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="statistics file") {
                            filename_statistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="background statistics file") {
                            filename_backstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="uncorrected statistics file") {
                            filename_uncorrectedstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="input description file") {
                            filename_settings=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="input file") {
                            filename_acquisition=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="mask file") {
                            filename_mask=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                        } else if (name=="date/time") {
                            initParams["CORRELATION_DATE"]=value;
                            paramsReadonly<<"CORRELATION_DATE";
                        } else {
                            QString fn=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                            if (QFile::exists(fn) && ((QFileInfo(fn).suffix().toLower()=="tif")||(QFileInfo(fn).suffix().toLower()=="tiff"))) {
                                more_files<<fn;
                                more_files_types<<"display_image";
                                more_files_descriptions<<name;
                            }
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
                files_descriptions<<tr("average image");
            }
            if (QFile::exists(filename_overviewstd)) {
                files<<filename_overviewstd;
                files_types<<"overview_std";
                files_descriptions<<tr("standard deviation for overview image");
            }
            if (QFile::exists(filename_video)) {
                files<<filename_video;
                files_types<<"video";
                files_descriptions<<tr("averaged video");
            }
            if (QFile::exists(filename_statistics)) {
                files<<filename_statistics;
                files_types<<"statistics";
                files_descriptions<<tr("statistics data");
            }
            if (QFile::exists(filename_backstatistics)) {
                files<<filename_backstatistics;
                files_types<<"background_statistics";
                files_descriptions<<tr("background statistics data");
            }
            if (QFile::exists(filename_uncorrectedstatistics)) {
                files<<filename_uncorrectedstatistics;
                files_types<<"uncorrected_statistics";
                files_descriptions<<tr("uncorrected statistics data");
            }
            if (QFile::exists(filename_mask)) {
                files<<filename_mask;
                files_types<<"mask";
                files_descriptions<<tr("mask");
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

            files<<more_files;
            files_types<<more_files_types;
            files_descriptions<<more_files_descriptions;

            QString description;
            parseSPIMSettings(filename_settings,  description,  initParams,  paramsReadonly,  files,  files_types,  files_descriptions);
            /*if (QFile::exists(filename_settings)) {
                QSettings settings(filename_settings, QSettings::IniFormat);
                appendCategorizedFilesFromB040SPIMConfig(settings, files, files_types, files_descriptions);

                if (settings.contains("acquisition/exposure_time")) {
                    initParams["EXPOSURE_TIME"]=settings.value("acquisition/exposure_time").toDouble();
                    paramsReadonly<<"EXPOSURE_TIME";
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

            }*/

            initParams["WIDTH"]=width;
            initParams["HEIGHT"]=height;

            initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
            paramsReadonly<<"IS_OVERVIEW_SCALED";


            int columns=1;
            if (!binary) columns=checkColumns(filename);
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
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - ACF"), files, initParams, paramsReadonly, files_types, files_descriptions);
                if (!filename_acquisition.isEmpty()) {
                    e->setFolder(QFileInfo(filename_acquisition).baseName());
                }
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
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - CCF %1").arg(c), files, initParams, paramsReadonly, files_types, files_descriptions);
                    if (!filename_acquisition.isEmpty()) {
                        e->setFolder(QFileInfo(filename_acquisition).baseName());
                    }
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
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - CCF %1").arg(c), files, initParams, paramsReadonly, files_types, files_descriptions);
                    if (!filename_acquisition.isEmpty()) {
                        e->setFolder(QFileInfo(filename_acquisition).baseName());
                    }
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
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - DCCF"), files, initParams, paramsReadonly, files_types, files_descriptions);
                if (!filename_acquisition.isEmpty()) {
                    e->setFolder(QFileInfo(filename_acquisition).baseName());
                }
                if (!description.isEmpty()) e->setDescription(description);
                if (e->error()) { // when an error occured: remove record and output an error message
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                }
            }


            if (QFile::exists(filename_overview) && QFile::exists(filename_overviewstd) && QFile::exists(filename_background) && QFile::exists(filename_backgroundstddev)) {
                QFileInfo fi(filename_overview);
                qDebug()<<fi.fileName();
                QStringList ffiles, ffile_types, ffiles_descriptions;
                ffiles<<filename_overview<<filename_overviewstd<<filename_background<<filename_backgroundstddev;
                ffile_types<<"image"<<"image_std"<<"background"<<"background_stddev";
                ffiles_descriptions<<""<<""<<""<<"";
                ffiles<<more_files;
                ffile_types<<more_files_types;
                ffiles_descriptions<<more_files_descriptions;

                qDebug()<<ffiles.size()<<"::: "<<ffiles;
                qDebug()<<ffile_types.size()<<"::: "<<ffile_types;
                qDebug()<<ffiles_descriptions.size()<<"::: "<<ffiles_descriptions;
                QMap<QString,QVariant>& init_params=initParams;
                QStringList init_params_readonly=paramsReadonly;
                init_params["BACKGROUND_CORRECTED"]=true;
                init_params_readonly<<"BACKGROUND_CORRECTED";
                insertProjectRecordFiles("number_and_brightness", fi.fileName()+tr(" - number & brightness"), ffiles, ffile_types, ffiles_descriptions, description, QFileInfo(filename_acquisition).baseName(), init_params, init_params_readonly);
            }

            if (QFile::exists(filename_settings)) {
                QFileInfo fi(filename_settings);
                insertProjectRecord("rdr_settings", fi.fileName()+tr(" - acquisition settings"), filename_settings, description, QFileInfo(filename_acquisition).baseName());
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
                    insertProjectRecord("table", fi.fileName(), files[i], description, QFileInfo(filename_acquisition).baseName(), p, roParams);
                }
            }



        } else {
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(tr("width an height are not set properly (>0)")));
        }
    }

}



void QFRDRImagingFCSPlugin::insertRH2CorFile(const QString& filename) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE"<<"IS_OVERVIEW_SCALED";

    bool ok=true, okk=true;

    int height=QInputDialog::getInt(NULL, tr("Radhard2 image properties"), tr("Height [ROI]= "), 32, 1, 100000, 1, &okk);
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
        fa->findFrames(0,1024*16,NULL,1);
        fa->findGhostFrames(1);
        if(QFile::exists(filenameValid)){
            QMessageBox::critical(NULL,tr("RH2 Correlation File Importer"),tr("The analyzed file %1 already exists.\nPlease delete it!").arg(filenameValid));
            return;
        }else{
            fa->writeFileFromMem(filenameValid.toLocal8Bit().constData(),1);
        }

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
        QString filename_settings=findB040ExperimentDescriptionForData(filename);
        if (QFile::exists(filename_settings)) parseSPIMSettings(filename_settings,  description,  initParams,  paramsReadonly,  files,  files_types,  files_descriptions);


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

