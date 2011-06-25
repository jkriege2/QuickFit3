#include "qfrdrimagingfcs.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcs_dataeditor.h"

QFRDRImagingFCSPlugin::QFRDRImagingFCSPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
}

QFRDRImagingFCSPlugin::~QFRDRImagingFCSPlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRImagingFCSPlugin::createRecord(QFProject* parent) {
	// factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRImagingFCSData)
    return new QFRDRImagingFCSData(parent);
}


void QFRDRImagingFCSPlugin::registerToMenu(QMenu* menu) {
	// create menu entries to insert data with this type
    QAction* action=new QAction(QIcon(getIconFilename()), tr("Insert Imaging FCS dataset (image of ACFs/CCFs)"), parentWidget);
    action->setStatusTip(tr("Insert a newimaging FCS record"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRecord()));
    menu->addAction(action);
}


void QFRDRImagingFCSPlugin::insertRecord() {
    if (project) {
        // file format to import
		QString format_name=tr("VideoCorrelator Data File (*.dat)");
        // look into INI which was the last used format
		QString current_format_name=settings->getQSettings()->value("imaging_fcs/current_format_filter", format_name).toString();
        // let the user select some files to import
		QStringList files = QFileDialog::getOpenFileNames(parentWidget,
                              tr("Select Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              format_name, &current_format_name);
        // store the format we just used
		settings->getQSettings()->setValue("imaging_fcs/current_format_filter", current_format_name);

		// now we iterate over all files and use QuickFit's progress bar interface (see plugin services)
        QStringList list = files;
        QStringList::Iterator it = list.begin();
        services->setProgressRange(0, list.size());
        services->setProgress(0);
        int i=0;
        while(it != list.end()) {
            i++;
            if (current_format_name==format_name) {
                // here we store some initial parameters
                QMap<QString, QVariant> initParams;

                // add all properties in initParams that will be readonly
                QStringList paramsReadonly;
                paramsReadonly<<"FILETYPE";

                bool ok=true;



                // set whatever you want (FILETYPE is just an example)!
                if (current_format_name==format_name) initParams["FILETYPE"]="VIDEO_CORRELATOR";
                QString filename=*it;
                QString evalFilename="";
                if (filename.endsWith(".autocorrelation.dat")) {
                    evalFilename=*it;
                    evalFilename=evalFilename.replace(".autocorrelation.dat", ".evalsettings.txt");
                } else if (filename.endsWith(".crosscorrelation.dat")) {
                    evalFilename=*it;
                    evalFilename=evalFilename.replace(".crosscorrelation.dat", ".evalsettings.txt");
                }

                if (QFile::exists(filename)) {
                    services->log_text(tr("loading [%2] '%1' ...\n").arg(filename).arg(current_format_name));
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
                            } while (!line.isNull());
                        }
                    }
                    if (width<=0) {
                        bool okk=true;
                        int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("width = "), 0, 1, 2147483647, 1, &okk);
                        if (okk) { width=wwidth; ok=false; }
                    }
                    if (ok && (height<=0)) {
                        bool okk=true;
                        int wwidth=QInputDialog::getInt ( NULL, tr("Import Video Correlator Data"), tr("width = "), 0, 1, 2147483647, 1, &okk);
                        if (okk) { height=wwidth; ok=false; }
                    }
                    if (ok) {
                        initParams["WIDTH"]=width;
                        initParams["HEIGHT"]=height;
                        // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                        QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName(), QStringList(filename), initParams, paramsReadonly);
                        if (e->error()) { // when an error occured: remove record and output an error message
                            QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                            project->deleteRawData(e->getID());
                        }
                    } else {
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(tr("width an height are not set properly (>0)")));
                    }
                    settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                }
                ++it;
            }
            services->setProgress(i);
            QApplication::processEvents();
        }
        services->setProgress(0);
    }
}

void QFRDRImagingFCSPlugin::insertVideoCorrelatorFile(const QString& filename, const QMap<QString, QVariant>& paramValues, const QStringList& paramReadonly) {
}



Q_EXPORT_PLUGIN2(imaging_fcs, QFRDRImagingFCSPlugin)
