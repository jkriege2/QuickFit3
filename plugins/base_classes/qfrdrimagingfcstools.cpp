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
                files_types<<"image_other";
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

int getEvalsettingsDV2Mode(const QString& filename, const QString& datafilename, bool* isFCCS, bool* isACF, QString* inputFilename) {
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
                } else if (name=="input file") {
                    if (inputFilename) *inputFilename=value;

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






bool readEvalSettingsFile(const QString &evalFilename, bool isDCCF, QMap<QString, QVariant> &initParams, QStringList &paramsReadonly, int &width, int &height, QStringList &files, QStringList &files_types, QStringList &files_descriptions, QString &filename_settings, QString& filename_acquisition, QString filename_overview, QString& filename_overviewstd, QString& filename_background, QString& filename_backgroundstddev, QString& role, int dccfid, bool& overviewReal)
{
    QFile file(evalFilename);
    overviewReal=false;
    QDir d=QFileInfo(evalFilename).absoluteDir();
    //qDebug()<<"readEvalSettings: "<<evalFilename;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString line;
        QRegExp reg("(.*)(:\\s*)([+-]?\\d*\\.?\\d+([eE][+-]?\\d+)?)");
        QString possibleBleachOffset="";
        bool hasBleachOffset=false;
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
                    initParams["FRAMETIME_MS"]=value.toDouble();
                    paramsReadonly<<"FRAMETIME_MS";
                } else if (name=="frametime (s)") {
                    initParams["FRAMETIME"]=value.toDouble();
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
                        files_descriptions<<QObject::tr("time-binned video");
                    }
                } else if (name=="uncorrected video file") {
                    QString filename_videoUncorrected=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_videoUncorrected)) {
                        files<<filename_videoUncorrected;
                        files_types<<"video_uncorrected";
                        files_descriptions<<QObject::tr("uncorrected time-binned video");
                    }
                } else if (name=="statistics file") {
                    QString filename_statistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics)) {
                        files<<filename_statistics;
                        files_types<<"statistics";
                        files_descriptions<<QObject::tr("statistics data");
                    }
                } else if (name=="background statistics file") {
                    QString filename_backstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics)) {
                        files<<filename_backstatistics;
                        files_types<<"background_statistics";
                        files_descriptions<<QObject::tr("background statistics data");
                    }
                } else if (name=="uncorrected statistics file") {
                    QString filename_uncorrectedstatistics=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics)) {
                        files<<filename_uncorrectedstatistics;
                        files_types<<"uncorrected_statistics";
                        files_descriptions<<QObject::tr("uncorrected statistics data");
                    }
                } else if (name=="statistics file dv1") {
                    QString filename_statistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics_dv1)) {
                        files<<filename_statistics_dv1;
                        files_types<<"statistics_dv1";
                        files_descriptions<<QObject::tr("statistics data, DualView side 1");
                    }
                } else if (name=="background statistics file dv1") {
                    QString filename_backstatistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics_dv1)) {
                        files<<filename_backstatistics_dv1;
                        files_types<<"background_statistics_dv1";
                        files_descriptions<<QObject::tr("background statistics data, DualView side 1");
                    }
                } else if (name=="uncorrected statistics file dv1") {
                    QString filename_uncorrectedstatistics_dv1=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics_dv1)) {
                        files<<filename_uncorrectedstatistics_dv1;
                        files_types<<"uncorrected_statistics_dv1";
                        files_descriptions<<QObject::tr("uncorrected statistics data, DualView side 1");
                    }
                } else if (name=="statistics file dv2") {
                    QString filename_statistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_statistics_dv2)) {
                        files<<filename_statistics_dv2;
                        files_types<<"statistics_dv2";
                        files_descriptions<<QObject::tr("statistics data, DualView side 2");
                    }
                } else if (name=="background statistics file dv2") {
                    QString filename_backstatistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_backstatistics_dv2)) {
                        files<<filename_backstatistics_dv2;
                        files_types<<"background_statistics_dv2";
                        files_descriptions<<QObject::tr("background statistics data, DualView side 2");
                    }
                } else if (name=="uncorrected statistics file dv2") {
                    QString filename_uncorrectedstatistics_dv2=QFileInfo(d.absoluteFilePath(value)).canonicalFilePath();
                    if (QFile::exists(filename_uncorrectedstatistics_dv2)) {
                        files<<filename_uncorrectedstatistics_dv2;
                        files_types<<"uncorrected_statistics_dv2";
                        files_descriptions<<QObject::tr("uncorrected statistics data, DualView side 2");
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
                        files_descriptions<<QObject::tr("mask");
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
                        if (fn.endsWith("bleachpoly2.tif")) {
                            possibleBleachOffset=fn;
                            possibleBleachOffset=possibleBleachOffset.replace("bleachpoly2.tif", "bleachpolypolyshift.tif");
                        }
                        if (fn.endsWith("bleachpolypolyshift.tif")) {
                            hasBleachOffset=true;
                        }
                    }

                }
            }
        } while (!line.isNull());

        if (!hasBleachOffset && !possibleBleachOffset.isEmpty() && QFile::exists(possibleBleachOffset)) {
            files<<possibleBleachOffset;
            files_types<<"display_image";
            files_descriptions<<"bleach polynomial shift file";
        }
    } else {
        return false;
    }



    if (QFile::exists(filename_overview)) {
        files<<filename_overview;
        files_types<<"overview";
        files_descriptions<<QObject::tr("average image");
    }
    initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
    paramsReadonly<<"IS_OVERVIEW_SCALED";
    if (QFile::exists(filename_overviewstd)) {
        files<<filename_overviewstd;
        files_types<<"overview_std";
        files_descriptions<<QObject::tr("standard deviation for overview image");
    }
    if (QFile::exists(filename_background)) {
        files<<filename_background;
        files_types<<"background";
        files_descriptions<<QObject::tr("background frame");
    }
    if (QFile::exists(filename_backgroundstddev)) {
        files<<filename_backgroundstddev;
        files_types<<"background_stddev";
        files_descriptions<<QObject::tr("background standard deviation frame");
    }
    if (QFile::exists(filename_settings)) {
        files<<filename_settings;
        files_types<<"acquisition_settings";
        files_descriptions<<QObject::tr("acquisition settings");
    }
    if (QFile::exists(filename_acquisition)) {
        files<<filename_acquisition;
        files_types<<"input";
        files_descriptions<<QObject::tr("input dataset");
    }

    bool ok=true;
    if (width<=0) {
        ok=false;
    }
    if (ok && (height<=0)) {
        ok=false;
    }
    if (ok) {


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



void imFCSFilenameHeuristics(const QString &filename, QString* evalFilename, bool *isCross, bool *isDCCF, int* dccfid, bool* isJanBFile, QString* role)
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
