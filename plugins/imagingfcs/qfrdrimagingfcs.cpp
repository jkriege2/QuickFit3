#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"
#include "qmodernprogresswidget.h"

QFRDRImagingFCSPlugin::QFRDRImagingFCSPlugin(QObject* parent):
    QObject(parent), QFPluginRawDataRecordBase()
{
    //constructor
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
    if (project && settings) {
        QFRDRImagingFCSCorrelationDialog* dlgCorrelate=new QFRDRImagingFCSCorrelationDialog(services, settings, parentWidget);
        dlgCorrelate->setProject(project);
        dlgCorrelate->exec();



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
            insertVideoCorrelatorFile(filename, overview);
            settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
            services->setProgress(i);
            QApplication::processEvents();
            ++it;
        }
        progress.accept();
        services->setProgress(0);

        delete dlgCorrelate;
    }
}

void QFRDRImagingFCSPlugin::insertRecord() {
    if (project) {
        // file format to import
        QString format_videoCorrelator=tr("VideoCorrelator Autocorrelations (*.autocorrelation.dat; *.crosscorrelation.dat)");
        QString format_Radhard2=tr("SPAD array Correlations (*.dat)");
        QStringList formats;
        formats<<format_videoCorrelator<<format_Radhard2;
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("imaging_fcs/current_format_filter", format_videoCorrelator).toString();
        // let the user select some files to import
        QStringList files = QFileDialog::getOpenFileNames(parentWidget,
                              tr("Select Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              formats.join(";;"), &current_format_name);
        // store the format we just used
        settings->getQSettings()->setValue("imaging_fcs/current_format_filter", current_format_name);

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
        while(it != list.end()) {
            i++;
            services->log_text(tr("loading [%2] '%1' ...\n").arg(*it).arg(current_format_name));
            progress.setLabelText(tr("loading [%2] '%1' ...\n").arg(*it).arg(current_format_name));
            QApplication::processEvents();
            if (current_format_name==format_videoCorrelator) {
                QString filename=*it;
                QString overview=filename;
                overview=overview.replace(".autocorrelation.dat", ".overview.tif");
                if (!QFile::exists(overview)) {
                    overview=filename;
                    overview=overview.replace(".crosscorrelation.dat", ".overview.tif");
                }
                if (!QFile::exists(overview)) overview="";
                insertVideoCorrelatorFile(filename, overview);
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

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QString& filename_overvieww) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview=filename_overvieww;

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;
    paramsReadonly<<"FILETYPE";

    bool ok=true;



    // set whatever you want (FILETYPE is just an example)!
    initParams["FILETYPE"]="VIDEO_CORRELATOR";
    QString evalFilename="";
    QString evalFilename1="";
    bool isCross=false;
    if (filename.endsWith(".autocorrelation.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".autocorrelation.dat", ".evalsettings.txt");
        evalFilename1=filename;
        evalFilename1=evalFilename1.replace(".autocorrelation.dat", ".configuration.ini");
    } else if (filename.endsWith(".crosscorrelation.dat")) {
        evalFilename=filename;
        evalFilename=evalFilename.replace(".crosscorrelation.dat", ".evalsettings.txt");
        evalFilename1=filename;
        evalFilename1=evalFilename1.replace(".crosscorrelation.dat", ".configuration.ini");
        isCross=true;
    }

    if (QFile::exists(filename)) {
        int width=0;
        int height=0;
        if (QFile::exists(evalFilename)) {
            QFile file(evalFilename);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                QString line;
                QRegExp reg("(.*)(:\\s*)(\\d+)");
                do {
                    line = stream.readLine();
                    if (reg.indexIn(line)>-1) {
                        QString name=reg.cap(1).toLower().trimmed();
                        QString value=reg.cap(3);

                        if (name=="width") {
                            initParams["WIDTH"]=value.toInt();
                            width=value.toInt();
                            paramsReadonly<<"WIDTH";
                        } else if (name=="height") {
                            initParams["HEIGHT"]=value.toInt();
                            height=value.toInt();
                            paramsReadonly<<"HEIGHT";
                        } else if (name=="reading frame count") {
                            initParams["FRAME_COUNT"]=value.toInt();
                            paramsReadonly<<"FRAME_COUNT";
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
                            filename_overview=QFileInfo(value).canonicalFilePath();
                        } else if (name=="date/time") {
                            initParams["CORRELATION_DATE"]=value;
                            paramsReadonly<<"CORRELATION_DATE";
                        }
                    }
                } while (!line.isNull());
            }
        }
        if (QFile::exists(evalFilename1)) {

        }
        if (width<=0) {
            bool okk=true;
            int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("width = "), 0, 1, 2147483647, 1, &okk);
            if (okk) { width=wwidth; ok=false; }
        }
        if (ok && (height<=0)) {
            bool okk=true;
            int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("height = "), 0, 1, 2147483647, 1, &okk);
            if (okk) { height=wwidth; ok=false; }
        }
        if (ok) {
            initParams["WIDTH"]=width;
            initParams["HEIGHT"]=height;
            initParams["TAU_COLUMN"]=0;
            int columns=checkColumns(filename);
            if (!isCross) {
                initParams["CORRELATION_COLUMN"]=1;
                QStringList files, files_types;
                files<<filename;
                files_types<<"acf";
                //qDebug()<<"filename_overview: "<<filename_overview;
                if (QFile::exists(filename_overview)) {
                    files<<filename_overview;
                    files_types<<"overview";
                }
                if (columns>2) initParams["CORRELATION_ERROR_COLUMN"]=2;
                // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - ACF"), files, initParams, paramsReadonly, files_types);
                if (e->error()) { // when an error occured: remove record and output an error message
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                }
            } else {

                for (int c=1; c<=qMin(4,columns); c++) {
                    initParams["CORRELATION_COLUMN"]=c;
                    QStringList files, files_types;
                    files<<filename;
                    files_types<<"ccf";
                    if (QFile::exists(filename_overview)) {
                        files<<filename_overview;
                        files_types<<"overview";
                    }
                    if (columns>4) initParams["CORRELATION_ERROR_COLUMN"]=c+4;
                    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                    QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - CCF %1").arg(c), files, initParams, paramsReadonly, files_types);
                    if (e->error()) { // when an error occured: remove record and output an error message
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                        project->deleteRawData(e->getID());
                    }
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

    bool ok=true;



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
