#include "qfrdrimagingfcstools.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QtEndian>

int stringToDualViewMode(const QString& dvin) {
    QString dv=dvin.trimmed().toLower();
    if (dv=="none" || dv=="0" || dv=="n") return 0;
    if (dv=="horizontal" || dv=="1" || dv=="h" || dv=="horicontal") return 1;
    if (dv=="vertical" || dv=="2" || dv=="v" || dv=="vertikal") return 2;

    return 0;
}

void readB040SPIMExperimentConfigFile(QSettings& set, double& frametime, double& baseline_offset, QString& backgroundfile, int& image_width, int& image_height, bool& hasPixel, double& pixel_width, double& pixel_height, int& dualViewMode ) {
    if (set.contains("acquisition/image_width")) image_width=set.value("acquisition/image_width", image_width).toInt();
    else if (set.contains("acquisition/camera/image_width")) image_width=set.value("acquisition/camera/image_width", image_width).toInt();
    else if (set.contains("acquisition/acquisition/image_width")) image_width=set.value("acquisition/acquisition/image_width", image_width).toInt();

    if (set.contains("acquisition/image_height")) image_height=set.value("acquisition/image_height", image_height).toInt();
    else if (set.contains("acquisition/camera/image_height")) image_height=set.value("acquisition/camera/image_height", image_height).toInt();
    else if (set.contains("acquisition/acquisition/image_height")) image_height=set.value("acquisition/acquisition/image_height", image_height).toInt();

    if (set.contains("acquisition/pixel_width")) { hasPixel=true; pixel_width=set.value("acquisition/pixel_width", pixel_width).toDouble()*1000.0;}
    else if (set.contains("acquisition/camera/pixel_width")) { hasPixel=true; pixel_width=set.value("acquisition/camera/pixel_width", pixel_width).toDouble()*1000.0;}
    else if (set.contains("acquisition/acquisition/pixel_width")) { hasPixel=true; pixel_width=set.value("acquisition/acquisition/pixel_width", pixel_width).toDouble()*1000.0;}

    if (set.contains("acquisition/pixel_height")) { hasPixel=true; pixel_height=set.value("acquisition/pixel_height", pixel_height).toDouble()*1000.0;}
    else if (set.contains("acquisition/camera/pixel_height")) { hasPixel=true; pixel_height=set.value("acquisition/camera/pixel_height", pixel_height).toDouble()*1000.0;}
    else if (set.contains("acquisition/acquisition/pixel_height")) { hasPixel=true; pixel_height=set.value("acquisition/acquisition/pixel_height", pixel_height).toDouble()*1000.0;}

    if (set.contains("acquisition/frame_time")) frametime=set.value("acquisition/frame_time", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/frame_rate")) frametime=1.0/set.value("acquisition/frame_rate", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/camera/frame_time")) frametime=set.value("acquisition/camera/frame_time", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/camera/frame_rate")) frametime=1.0/set.value("acquisition/camera/frame_rate", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/acquisition/frame_time")) frametime=set.value("acquisition/acquisition/frame_time", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/acquisition/frame_rate")) frametime=1.0/set.value("acquisition/acquisition/frame_rate", frametime).toDouble()*1e6;

    if (set.contains("acquisition/dualview_mode")) dualViewMode=stringToDualViewMode(set.value("acquisition/dualview_mode", "n").toString());
    else if (set.contains("acquisition/acquisition/dualview_mode")) dualViewMode=stringToDualViewMode(set.value("acquisition/acquisition/dualview_mode", "n").toString());

    baseline_offset=set.value("acquisition/baseline_offset", baseline_offset).toDouble();
    //backgroundfile="";
    int fcnt=set.value("files/count", 0).toInt();
    for (int f=0; f<fcnt; f++) {
        QString fn=QFileInfo(set.fileName()).dir().absoluteFilePath(set.value("files/name"+QString::number(f), "").toString());
        //QString ft=set.value("files/type"+QString::number(f), "").toString();
        QString fd=set.value("files/description"+QString::number(f), "").toString();
        if (fd.toLower().simplified().contains("background")) {
            backgroundfile=fn;
        }
    }
}

void readB040SPIMExperimentConfigFileAssociatedFiles(QSettings& set, QStringList& files, QStringList& description,  QStringList& type) {
    int fcnt=set.value("files/count", 0).toInt();
    //qDebug()<<"fcnt="<<fcnt;
    for (int f=0; f<fcnt; f++) {
        QString fn=QFileInfo(set.fileName()).dir().absoluteFilePath(set.value("files/name"+QString::number(f), "").toString());
        QString ft=set.value("files/type"+QString::number(f), "").toString();
        QString fd=set.value("files/description"+QString::number(f), "").toString();
        files.append(fn);
        description.append(fd);
        type.append(ft);
    }
}


void appendCategorizedFilesFromB040SPIMConfig(QSettings& settings, QStringList& files, QStringList& files_types, QStringList& files_descriptions) {
    QStringList inifiles;
    QStringList inifiletypes;
    QStringList inifiledescriptions;
    readB040SPIMExperimentConfigFileAssociatedFiles(settings, inifiles, inifiledescriptions, inifiletypes);
    for (int i=0; i<inifiles.size(); i++) {
        if (inifiletypes[i].toLower().contains("tiff")) {
            if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("before") && inifiledescriptions[i].toLower().contains("transmi")) {
                files<<inifiles[i];
                files_types<<"overview_before_transmission";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("after") && inifiledescriptions[i].toLower().contains("transmi")) {
                files<<inifiles[i];
                files_types<<"overview_after_transmission";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("before")) {
                files<<inifiles[i];
                files_types<<"overview_before";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("after")) {
                files<<inifiles[i];
                files_types<<"overview_after";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("preview")) {
                files<<inifiles[i];
                files_types<<"overview_after";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("background")) {
                files<<inifiles[i];
                files_types<<"overview_background";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("z stack")) {
                files<<inifiles[i];
                files_types<<"zstack";
                files_descriptions<<inifiledescriptions[i];
            } else {
                files<<inifiles[i];
                files_types<<"image";
                files_descriptions<<inifiledescriptions[i];
            }
        } else if (inifiletypes[i].toLower().contains("csv")) {
            if (inifiledescriptions[i].toLower().contains("measureable") && inifiledescriptions[i].toLower().contains("properties") && inifiletypes[i].toLower()=="csv") {
                files<<inifiles[i];
                files_types<<"setup_properties";
                files_descriptions<<inifiledescriptions[i];
            }
        }
    }

}






QString findB040ExperimentDescriptionForData(const QString& filename) {
    QString filenameDisplayed=QFileInfo(filename).absoluteFilePath();

    //////////////////////////////////////////////////////////////////////////////////
    // now we search for a .configuration.ini file describing the selected file
    //////////////////////////////////////////////////////////////////////////////////
    QDir d=QFileInfo(filenameDisplayed).absoluteDir();
    QStringList nameFilters;
    nameFilters<<"*.ini";
    nameFilters<<"*.cfg";
    nameFilters<<"*.txt";
    QString inputconfigfile="";
    d.setNameFilters(nameFilters);
    // could be that the absolute pathes are wrong. In this case we try to get a second guess by finding a file which
    // equals in the name, but not the directory ... the user can correct the data anyways. This second guess is stored
    // in these variables:
    bool hasFirst=false;
    foreach (QString iniFile, d.entryList(QDir::Files)) {
        //QApplication::processEvents();

        QSettings set(d.absoluteFilePath(iniFile), QSettings::IniFormat);
        int fcnt=set.value("files/count", 0).toInt();
        if (fcnt>0) {
            for (int f=0; f<fcnt; f++) {
                QString fn=set.value("files/name"+QString::number(f), "").toString();
                QString ft=set.value("files/type"+QString::number(f), "").toString();
                if (!fn.isEmpty()) {
                    QString fnAbs=d.absoluteFilePath(fn);
                    //qDebug()<<"fn="<<fn<<"  fnAbs="<<fnAbs<<"  fn(filename)="<<QFileInfo(filename).fileName()<<"  fn(fnAbs)="<<QFileInfo(fnAbs).fileName();
                    if (fnAbs==filenameDisplayed) {
                        hasFirst=true;
                        inputconfigfile=d.absoluteFilePath(iniFile);
                    } else if (QFileInfo(fnAbs).fileName()==QFileInfo(filenameDisplayed).fileName()) {
                        hasFirst=true;
                        inputconfigfile=d.absoluteFilePath(iniFile);
                    }
                }
            }

        }
        if (hasFirst) break;
    }

    // if we didn't find a second guess, we try to find a config file with the same
    // basename + one of a set of extensions (newsuffix list) and try to read info from
    // that.
    if (!hasFirst) {
        QString suffix=QFileInfo(filenameDisplayed).suffix();
        QString cfgname;

        QStringList newsuffix;
        newsuffix<<"ini"
                 <<"configuration.ini"
                 <<"settings.ini"
                 <<"settings.txt"
                 <<"cfg";
        for (int i=0; i<newsuffix.size(); i++) {
            cfgname=filenameDisplayed.left(filenameDisplayed.size()-suffix.size())+newsuffix[i];
            if (QFile::exists(cfgname)) {
                hasFirst=true;
                inputconfigfile=cfgname;
            }
        }
    }


    return inputconfigfile;
}


QString findEvalsettingsFileForVideoCorrelatorFile(const QString& vidCorrFile) {
    return findFileWithExtensionForVideoCorrelatorFile(vidCorrFile, ".evalsettings.txt");
}


QString findOverviewFileForVideoCorrelatorFile(const QString& vidCorrFile) {
    QString ovr=findFileWithExtensionForVideoCorrelatorFile(vidCorrFile, ".overview_float.tif");
    if (!QFile::exists(ovr) || ovr==vidCorrFile) {
        ovr=findFileWithExtensionForVideoCorrelatorFile(vidCorrFile, ".overview.tif");
    }
    if (!QFile::exists(ovr) || ovr==vidCorrFile) {
        ovr=findFileWithExtensionForVideoCorrelatorFile(vidCorrFile, ".overview_uncorrected.tif");
    }
    if (!QFile::exists(ovr) || ovr==vidCorrFile) {
        ovr="";
    }
    return ovr;
}

QString findFileWithExtensionForVideoCorrelatorFile(const QString& vidCorrFile, const QString& newExtension) {
    QRegExp rxdccf("\\.dccf(\\d+)?\\.(dat|bin)");
    rxdccf.setCaseSensitivity(Qt::CaseInsensitive);
    rxdccf.setMinimal(true);
    QRegExp rxacf("\\.(autocorrelation|acf|autocorrelation0|acf0|autocorrelation1|acf1)\\.(dat|bin)");
    rxacf.setCaseSensitivity(Qt::CaseInsensitive);
    rxacf.setMinimal(true);
    QRegExp rxccf("\\.(crosscorrelation|ccf|fccs|dccf)\\.(dat|bin)");
    rxccf.setCaseSensitivity(Qt::CaseInsensitive);
    rxccf.setMinimal(true);

    QString overview=vidCorrFile;
    overview=overview.replace(rxdccf, newExtension);
    //qDebug()<<"   test: "<<overview;
    if (!QFile::exists(overview) || overview==vidCorrFile) {
        overview=vidCorrFile;
        overview=overview.replace(rxacf, newExtension);
        //qDebug()<<"   test: "<<overview;
    }
    if (!QFile::exists(overview) || overview==vidCorrFile) {
        overview=vidCorrFile;
        overview=overview.replace(rxccf, newExtension);
        //qDebug()<<"   test: "<<overview;
    }
    if (!QFile::exists(overview) || overview==vidCorrFile) overview="";

    return overview;

}

int getEvalsettingsDV2Mode(const QString& filename, const QString& datafilename, bool* isFCCS, bool* isACF) {
    int dvMode=0;
    bool fnIsFCCS=false;
    bool fnIsACF=false;
    if (   datafilename.endsWith(".autocorrelation.bin", Qt::CaseInsensitive)
           || datafilename.endsWith(".acf.bin", Qt::CaseInsensitive)
           || datafilename.endsWith(".acf.dat", Qt::CaseInsensitive)
           || datafilename.endsWith(".autocorrelation.dat", Qt::CaseInsensitive)) {
        fnIsACF=true;
    }
    int w=0, h=0, dccfdx=0, dccfdy=0;
    if (QFile::exists(filename)) {
        QFile file(filename);
        QDir d=QFileInfo(filename).absoluteDir();
        //qDebug()<<"readEvalSettings: "<<evalFilename;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString line;
            QRegExp reg("(.*)(:\\s*)([+-]?\\d*\\.?\\d+([eE][+-]?\\d+)?)");
            do {
                line = stream.readLine();

                int colon_idx=line.indexOf(':');
                QString name=line.left(colon_idx).toLower().trimmed();
                QString value=line.mid(colon_idx+1).trimmed();
                if (colon_idx<0) {
                    name="";
                    value="";
                }
                bool isNumber=false;
                if (reg.indexIn(line)>-1) {
                    name=reg.cap(1).toLower().trimmed();
                    value=reg.cap(3);
                    isNumber=true;
                }


                QRegExp rxdccfp("dccf\\s+(\\d+\\s+)?(delta x|delta y|frame width|frame height|role)");
                rxdccfp.setMinimal(true);
                rxdccfp.setCaseSensitivity(Qt::CaseInsensitive);
                bool isDCCFParam=rxdccfp.indexIn(name)>=0;
                QString dccfParam=rxdccfp.cap(2).toLower();
                int dccfParamID=-1;
                if (!rxdccfp.cap(1).isEmpty()) dccfParamID=rxdccfp.cap(1).toInt();


                //qDebug()<<name <<"  =  "<<value;

                //qDebug()<<name <<"  =  "<<value<<"     dccfp="<<isDCCFParam<< " role="<<dccfParam<<" id="<<dccfParamID;
                if (isDCCFParam && dccfParam=="role"){
                    /*qDebug()<<"test dccf role: ID="<<dccfParamID;
                    qDebug()<<"    dccs value: "<<value;
                    qDebug()<<"        datafn: "<<datafilename;
                    qDebug()<<"       endtest: "<<QString(".dccf%1.bin").arg(dccfParamID, 3, 10, QLatin1Char('0'));
                    qDebug()<<"       endtest: "<<QString(".dccf%1.dat").arg(dccfParamID, 3, 10, QLatin1Char('0'));*/
                    if (value.toUpper().trimmed()=="FCCS" && (datafilename.endsWith(QString(".dccf%1.bin").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive)||datafilename.endsWith(QString(".dccf%1.dat").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive))) {
                        fnIsFCCS=true;
                    }
                } else if (isNumber && isDCCFParam && dccfParam=="delta x"  && (datafilename.endsWith(QString(".dccf%1.bin").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive)||datafilename.endsWith(QString(".dccf%1.dat").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive))) {
                    dccfdx=value.toInt();
                } else if (isNumber && isDCCFParam && dccfParam=="delta y"  && (datafilename.endsWith(QString(".dccf%1.bin").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive)||datafilename.endsWith(QString(".dccf%1.dat").arg(dccfParamID, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive))) {
                    dccfdy=value.toInt();
                } else if (isNumber && name=="dualview mode") {
                    dvMode=value.toInt();
                } else if (isNumber && name=="width") {
                    w=value.toInt();
                } else if (isNumber && name=="height") {
                    h=value.toInt();
                } else if (isNumber && name=="dccf delta x") {
                    dccfdx=value.toInt();
                } else if (isNumber && name=="dccf delta y") {
                    dccfdy=value.toInt();

                /*} else if (name=="") {
                    initParams[""]=value.toInt();
                    paramsReadonly<<"";*/
                }

            } while (!line.isNull());
            file.close();
        }
    }
    if (isFCCS) *isFCCS = fnIsFCCS;
    if (isACF) *isACF = fnIsACF;
    return dvMode;
}
