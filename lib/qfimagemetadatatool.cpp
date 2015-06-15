/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-25 17:54:41 +0200 (Do, 25 Sep 2014) $  (revision $Rev: 3506 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfimagemetadatatool.h"


#define READ_PROP_DOUBLE(dePixel, ATTR_NAME, PROP_NAME, factor) \
    if (dePixel.hasAttribute(ATTR_NAME) && !data.contains(PROP_NAME)) {\
        bool ok=false;\
        double val=dePixel.attribute(ATTR_NAME).toDouble(&ok)*double(factor);\
        if (ok) {\
            data[PROP_NAME]=val;\
        }\
    } else { \
        QDomElement deChild=dePixel.firstChildElement(ATTR_NAME); \
        if (!deChild.isNull() && deChild.text().size()>0) {\
            bool ok=false;\
            double val=deChild.text().toDouble(&ok)*double(factor);\
            if (ok) {\
                data[PROP_NAME]=val;\
            }\
        }\
    }

#define READ_PROP_INT(dePixel, ATTR_NAME, PROP_NAME) \
    if (dePixel.hasAttribute(ATTR_NAME) && !data.contains(PROP_NAME)) {\
        bool ok=false;\
        int val=dePixel.attribute(ATTR_NAME).toInt(&ok);\
        if (ok) {\
            data[PROP_NAME]=val;\
        }\
    } else { \
        QDomElement deChild=dePixel.firstChildElement(ATTR_NAME); \
        if (!deChild.isNull() && deChild.text().size()>0) {\
            bool ok=false;\
            int val=deChild.text().toInt(&ok);\
            if (ok) {\
                data[PROP_NAME]=val;\
            }\
        }\
    }

#define READ_PROP_STRING(dePixel, ATTR_NAME, PROP_NAME) \
    if (dePixel.hasAttribute(ATTR_NAME) && !data.contains(PROP_NAME)) {\
        bool ok=false;\
        QString val=dePixel.attribute(ATTR_NAME);\
        if (ok) {\
            data[PROP_NAME]=val;\
        }\
    } else { \
        QDomElement deChild=dePixel.firstChildElement(ATTR_NAME); \
        if (!deChild.isNull() && deChild.text().size()>0) {\
            data[PROP_NAME]=deChild.text();\
        }\
    }

void qfimdtStoreOMEMetaDataTree(QMap<QString, QVariant> &data, QDomElement de, const QString &prefix, const QStringList& exclude) {
    QString nn=de.tagName();
    QString p=prefix+nn;
    QDomNamedNodeMap atrs=de.attributes();
    for (int i=0; i<atrs.size(); i++) {
        QString n=atrs.item(i).toAttr().name();
        QString v=atrs.item(i).toAttr().value();
        if (!exclude.contains(n)) {
            data[p+"."+n]=v;
        }
    }
    QDomNodeList chlds=de.childNodes();
    for (int i=0; i<chlds.size(); i++) {
        QDomNode cn=chlds.item(i);
        if (cn.isElement()) {
            QDomElement ce=cn.toElement();
            if (ce.hasChildNodes()) {
                qfimdtStoreOMEMetaDataTree(data, ce, p+"/");
            } else if (ce.text().size()>0){
                data[p+"."+ce.nodeName()]=ce.text();
            }
        }
    }


}

bool qfimdtGetOMEMetaData(QMap<QString, QVariant> &data, const QByteArray &OME)
{
    QDomDocument ome;
    if (ome.setContent(OME)) {
        QDomElement b=ome.documentElement();
        if (b.tagName()=="OME") {
            QStringList excl;
            excl<<"BinData"<<"TiffData";
            qfimdtStoreOMEMetaDataTree(data, b, "", excl);
            QDomElement deImage=b.firstChildElement("Image");
            int iCnt=0;
            //bool hasReadPixels=false;
            while (!deImage.isNull()) {
                if (iCnt==1) {
                    READ_PROP_STRING(deImage, "AcquiredDate", "DATE");
                    READ_PROP_STRING(deImage, "Description", "DESCRIPTION");
                    READ_PROP_STRING(deImage, "Name", "NAME");
                    QDomElement dePixel=deImage.firstChildElement("Pixels");
                    int pCnt=0;
                    while (!dePixel.isNull()) {
                        READ_PROP_DOUBLE(dePixel, "PhysicalSizeX", "PIXEL_WIDTH", 1e3);
                        READ_PROP_DOUBLE(dePixel, "PhysicalSizeY", "PIXEL_HEIGHT", 1e3);
                        READ_PROP_DOUBLE(dePixel, "PhysicalSizeZ", "STEPSIZE", 1e3);
                        READ_PROP_DOUBLE(dePixel, "PhysicalSizeZ", "DELTAZ", 1e3);
                        READ_PROP_DOUBLE(dePixel, "TimeIncrement", "FRAMETIME", 1.0);
                        READ_PROP_INT(dePixel, "SizeT", "FRAMES");

                        dePixel=dePixel.nextSiblingElement("Pixels");
                        pCnt++;
                    }
                }
                deImage=deImage.nextSiblingElement("Image");
                iCnt++;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}


bool qfimdtGetImageJMetaData(QMap<QString, QVariant> &data, const QByteArray &meta)
{
    if(meta.contains("ImageJ=") || meta.contains("ImageJ =")) {
        {
            QRegExp rxImages("images\\s*\\=\\s*(\\d+)");
            if (rxImages.indexIn(meta)>=0) {
                data["FRAMES"]=rxImages.cap(1).toInt();
            }
        }
        {
            QRegExp rxImages("frames\\s*\\=\\s*(\\d+)");
            if (rxImages.indexIn(meta)>=0) {
                data["FRAMES"]=rxImages.cap(1).toInt();
            }
        }
        {
            QRegExp rxImages("spacing\\s*\\=\\s*([\\d.eE\\+\\-]+)");
            if (rxImages.indexIn(meta)>=0) {
                data["DELTAZ"]=data["STEPSIZE"]=rxImages.cap(1).toDouble();
            }
        }
        {
            QRegExp rxImages("finterval\\s*\\=\\s*([\\d.eE\\+\\-]+)");
            if (rxImages.indexIn(meta)>=0) {
                data["FRAMETIME"]=rxImages.cap(1).toDouble();
            }
        }
        return true;
    }
    return false;
}


bool qfimdtGetTinyTIFFMetaData(QMap<QString, QVariant> &data, const QByteArray &meta)
{
    if(meta.contains("TinyTIFFWriter_version=") || meta.contains("TinyTIFFWriter_version =") || meta.contains("JKLIBTIFF_version=") || meta.contains("JKLIBTIFF_version =")) {
        QRegExp rxImages("(images|frames|pixel_width|pixel_height|deltaz|frametime)\\s*\\=\\s*([^\\s]+)");
        rxImages.setCaseSensitivity(Qt::CaseInsensitive);
        if (rxImages.indexIn(meta)>=0) {
            QString id=rxImages.cap(1);
            if (id=="images" || id=="frames") {
                data["FRAMES"]=rxImages.cap(2).toInt();
            } else if (id=="pixel_width") {
                data["PIXEL_WIDTH"]=rxImages.cap(2).toDouble();
            } else if (id=="pixel_height") {
                data["PIXEL_HEIGHT"]=rxImages.cap(2).toDouble();
            } else if (id=="frametime") {
                data["FRAMETIME"]=rxImages.cap(2).toDouble();
            } else if (id=="deltaz") {
                data["DELTAZ"]=data["STEPSIZE"]=rxImages.cap(2).toDouble();
            }
        }
        return true;
    }
    return false;
}


QByteArray qfimdtBuildImageJMetaData(int frames, double /*deltaX*/, double /*deltaY*/, double deltaZ, const QString &axisunit, const QString &/*comment*/)
{
    QByteArray res="ImageJ=1.48v";
    if (frames>0) res+=QString("\nframes=%1").arg(frames).toLatin1();
    if (deltaZ>0) res+=QString("\nspacing=%1").arg(deltaZ).toLatin1();
    if (!axisunit.isEmpty()) res+=QString("\nunit=%1").arg(axisunit).toLatin1();
    //if (frames>0) res+=QString("\nimages=%1").arg(frames).toLatin1();
    return res;
}


QString findB040ExperimentDescriptionForData(const QString& filename, QString* description) {
    QString filenameDisplayed=QFileInfo(filename).absoluteFilePath();

    //qDebug()<<"findB040ExperimentDescriptionForData("<<filename<<"):  "<<filenameDisplayed;

    //////////////////////////////////////////////////////////////////////////////////
    // now we search for a .configuration.ini file describing the selected file
    //////////////////////////////////////////////////////////////////////////////////
    QDir d=QFileInfo(filenameDisplayed).absoluteDir();
    QStringList nameFilters;
    nameFilters<<"*.ini";
    nameFilters<<"*.cfg";
    nameFilters<<"*.txt";
    QString inputconfigfile="";
    QString filedesc="";
    QString filetype="";
    d.setNameFilters(nameFilters);
    // could be that the absolute pathes are wrong. In this case we try to get a second guess by finding a file which
    // equals in the name, but not the directory ... the user can correct the data anyways. This second guess is stored
    // in these variables:
    bool hasFirst=false;
    QString iniFile;
    foreach (iniFile, d.entryList(QDir::Files)) {
        //QApplication::processEvents();

        QSettings set(d.absoluteFilePath(iniFile), QSettings::IniFormat);
        int fcnt=set.value("files/count", 0).toInt();
        if (fcnt>0) {
            for (int f=0; f<fcnt; f++) {
                QString fn=set.value("files/name"+QString::number(f), "").toString();
                //QString ft=set.value("files/type"+QString::number(f), "").toString();
                //QString fd=set.value("files/description"+QString::number(f), "").toString();
                if (!fn.isEmpty()) {
                    QString fnAbs=d.absoluteFilePath(fn);
                    //qDebug()<<"fn="<<fn<<"  fnAbs="<<fnAbs<<"  fn(filename)="<<QFileInfo(filename).fileName()<<"  fn(fnAbs)="<<QFileInfo(fnAbs).fileName();
                    if (fnAbs==filenameDisplayed) {
                        //filedesc=fd;
                        //filetype=ft;
                        hasFirst=true;
                        inputconfigfile=d.absoluteFilePath(iniFile);
                    } else if (QFileInfo(fnAbs)==QFileInfo(filenameDisplayed)) {
                        hasFirst=true;
                        //filedesc=fd;
                        //filetype=ft;
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

    // if the last step also didn't work, we also try a small modifictaion of the basename
    if (!hasFirst) {
        bool ok=true;
        if (filenameDisplayed.toLower().endsWith("bin.acf.bin")) filenameDisplayed=filenameDisplayed.left(filenameDisplayed.size()-11);
        else if (filenameDisplayed.toLower().endsWith("bin.ccf.bin")) filenameDisplayed=filenameDisplayed.left(filenameDisplayed.size()-11);
        else if (filenameDisplayed.toLower().endsWith("bin.fccs.bin")) filenameDisplayed=filenameDisplayed.left(filenameDisplayed.size()-12);
        else if (filenameDisplayed.toLower().endsWith("bin.dccf.bin")) filenameDisplayed=filenameDisplayed.left(filenameDisplayed.size()-12);
        else ok=false;

        if (ok) {
            QString cfgname;

            QStringList newsuffix;
            newsuffix<<"ini"
                     <<"configuration.ini"
                     <<"settings.ini"
                     <<"settings.txt"
                     <<"cfg";
            for (int i=0; i<newsuffix.size(); i++) {
                cfgname=filenameDisplayed.left(filenameDisplayed.size())+newsuffix[i];
                if (QFile::exists(cfgname)) {
                    hasFirst=true;
                    inputconfigfile=cfgname;
                }
            }
        }
    }
    //qDebug()<<"  return: "<<hasFirst<<inputconfigfile;

    // search for a file description in the config file
    if (!inputconfigfile.isEmpty()) {
        QSettings set(inputconfigfile, QSettings::IniFormat);
        int fcnt=set.value("files/count", 0).toInt();
        if (fcnt>0) {
            for (int f=0; f<fcnt; f++) {
                QString fn=set.value("files/name"+QString::number(f), "").toString();
                QString ft=set.value("files/type"+QString::number(f), "").toString();
                QString fd=set.value("files/description"+QString::number(f), "").toString();
                if (!fn.isEmpty()) {
                    QString fnAbs=d.absoluteFilePath(fn);
                    if (fnAbs==filenameDisplayed) {
                        filedesc=fd;
                        filetype=ft;
                        hasFirst=true;
                    } else if (QFileInfo(fnAbs).fileName()==QFileInfo(filenameDisplayed).fileName()) {
                        hasFirst=true;
                        filedesc=fd;
                        filetype=ft;
                    }
                }
            }

        }
    }

    if (description && !inputconfigfile.isEmpty()) *description=filedesc;
    return inputconfigfile;
}

#define qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX(fileprop, dataprop, toType) \
        if (!data.contains(dataprop) && settings.contains(QString(fileprop))) { \
            data[dataprop]=settings.value(QString(fileprop)).toType(); \
        }

#define qfimdtGetQuickFitMetaData_FINDPROP(fileprop, dataprop, toType) {\
        bool found=false; \
        QString prefix; \
        foreach(prefix, prefixes) { \
            if (!found && !data.contains(dataprop) && settings.contains(prefix+QString(fileprop))) { \
                data[dataprop]=settings.value(prefix+QString(fileprop)).toType(); \
                found=true; \
            }\
        }\
    }
#define qfimdtGetQuickFitMetaData_FINDPROPSCALE(fileprop, dataprop, toType,scale) {\
        bool found=false; \
        QString prefix; \
        foreach(prefix, prefixes) { \
            if (!found && !data.contains(dataprop) && settings.contains(prefix+QString(fileprop))) { \
                data[dataprop]=settings.value(prefix+QString(fileprop)).toType()*scale; \
                found=true; \
            }\
        }\
    }
#define qfimdtGetQuickFitMetaData_FINDPROPINVSCALE(fileprop, dataprop, toType,scale) {\
        bool found=false; \
        QString prefix; \
        foreach(prefix, prefixes) { \
            if (!found && !data.contains(dataprop) && settings.contains(prefix+QString(fileprop))) { \
                data[dataprop]=1.0/settings.value(prefix+QString(fileprop)).toType()*scale; \
                found=true; \
            }\
        }\
    }

bool qfimdtGetQuickFitMetaData(QMap<QString, QVariant>& data, const QString &filename, QString* configfile_out, QString* filedescription_out, bool filenameIsINI) {
    bool res=false;
    QString configfile, filedescription;

    // find config file
    if (filenameIsINI) {
        configfile=filename;
    } else {
        configfile=findB040ExperimentDescriptionForData(filename, &filedescription);
    }

    // check for good result
    res=QFile::exists(configfile);


    if (res) {
        QSettings settings(configfile, QSettings::IniFormat);
        // read props from config file
        QString prefix1="acquisition/";
        QString prefix2="acquisition/acquisition/";
        QString prefix3="acquisition/camera/";
        QString prefix4="setup/";

        int lightpath=-1;
        int camera=-1;
        if (!filenameIsINI) {
            QString fd=filedescription.toLower().simplified().trimmed();
            QString start;
            if (fd.startsWith("overview before")) {
                QRegExp rxLP("overview before.*lightpath\\s*(\\d+)\\s+", Qt::CaseInsensitive);
                if (rxLP.indexIn(fd)>=0) {
                    lightpath=rxLP.cap(1).toInt();
                }
                start="overview";
                if (lightpath>=1) {
                    start=start+QString("_lightpath%1").arg(lightpath);
                }
            }
            if (fd.startsWith("overview after")) {
                QRegExp rxLP("overview after.*lightpath\\s*(\\d+)\\s+", Qt::CaseInsensitive);
                if (rxLP.indexIn(fd)>=0) {
                    lightpath=rxLP.cap(1).toInt();
                }
                start="overview_after";
                if (lightpath>=1) {
                    start=start+QString("_lightpath%1").arg(lightpath);
                }
            }
            if (fd.contains(" background ")) {
                QRegExp rxLP("background.*lightpath\\s*(\\d+)\\s+", Qt::CaseInsensitive);
                if (rxLP.indexIn(fd)>=0) {
                    lightpath=rxLP.cap(1).toInt();
                }
                start="background";
                if (lightpath>=1) {
                    start=start+QString("_lightpath%1").arg(lightpath);
                }
            }
            QRegExp rxCAMERA("camera\\s*(\\d+)", Qt::CaseInsensitive);
            if (rxCAMERA.indexIn(fd)>=0) {
                camera=rxCAMERA.cap(1).toInt();
            }

            if (!start.isEmpty()) {
                prefix1="acquisition/"+start+"/";
                prefix2="acquisition/"+start+"/camera/";
                prefix3="acquisition/";
            }
            data["EXP_CONFIG_FILE"]=QFileInfo(filename).absoluteDir().relativeFilePath(configfile);
        }

        QStringList prefixes;
        prefixes<<prefix1<<prefix2<<prefix3<<prefix4;



        qfimdtGetQuickFitMetaData_FINDPROP("duration_milliseconds", "MEASUREMENT_DURATION_MS", toDouble);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("duration", "MEASUREMENT_DURATION_MS", toDouble, 1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("frame_time", "FRAMETIME_MS", toDouble, 1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPINVSCALE("frame_rate", "FRAMETIME_MS", toDouble, 1000.0);
        if (data.contains("FRAMETIME_MS") && !data.contains("FRAMETIME")) data["FRAMETIME"]=data["FRAMETIME_MS"].toDouble()/1000.0;
        qfimdtGetQuickFitMetaData_FINDPROP("magnification", "MAGNIFICATION", toDouble);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("camera_pixel_width", "CAMERA_PIXEL_WIDTH", toDouble, 1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("camera_pixel_height", "CAMERA_PIXEL_HEIGHT", toDouble, 1000.0);

        qfimdtGetQuickFitMetaData_FINDPROP("roi_xstart", "ROI_X_START", toInt);
        qfimdtGetQuickFitMetaData_FINDPROP("roi_xend", "ROI_X_END", toInt);
        qfimdtGetQuickFitMetaData_FINDPROP("roi_ystart", "ROI_Y_START", toInt);
        qfimdtGetQuickFitMetaData_FINDPROP("roi_yend", "ROI_Y_END", toInt);

        qfimdtGetQuickFitMetaData_FINDPROP("dualview_mode", "DUALVIEW_MODE", toString);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("axis1/stack_delta", "MULTIFRAMESTACK_DELTA", toDouble,1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("stack_delta", "MULTIFRAMESTACK_DELTA", toDouble,1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("axis1/stack_start", "MULTIFRAMESTACK_START", toDouble,1000.0);
        qfimdtGetQuickFitMetaData_FINDPROPSCALE("stack_start", "MULTIFRAMESTACK_START", toDouble,1000.0);
        qfimdtGetQuickFitMetaData_FINDPROP("axis1/type", "MULTIFRAMESTACK_TYPE", toDouble);
        qfimdtGetQuickFitMetaData_FINDPROP("type", "MULTIFRAMESTACK_TYPE", toDouble);

        if (data.contains("MULTIFRAMESTACK_DELTA")) data["DELTAZ"]=data["MULTIFRAMESTACK_DELTA"];
        if (data.contains("MULTIFRAMESTACK_START")) data["MULTIFRAMESTACK_START_MICROMETER"]=data["MULTIFRAMESTACK_START"].toDouble()/1000.0;
        if (data.contains("MULTIFRAMESTACK_DELTA")) data["MULTIFRAMESTACK_DELTA_MICROMETER"]=data["MULTIFRAMESTACK_DELTA"].toDouble()/1000.0;

        {
            bool found=false;
            QString prefix;
            foreach(prefix, prefixes) {
                if (!found && !data.contains("PIXEL_WIDTH")  ) {
                    double mag=settings.value(prefix+"magnification", data.value("MAGNIFICATION", 1.0).toDouble()).toDouble();
                    double cpw=settings.value(prefix+"camera_pixel_width", 0.0).toDouble();
                    double pw=settings.value(prefix+"pixel_width", 0.0).toDouble();

                    if ((cpw>0) && (mag!=1)) { data["PIXEL_WIDTH"]=cpw/mag*1000.0; found=true; }
                    else if (pw>0) { data["PIXEL_WIDTH"]=pw*1000.0; found=true; }

                    //qDebug()<<configfile<<mag<<data.value("MAGNIFICATION", 1.0).toDouble()<<cpw<<settings.value(prefix+"pixel_width").toDouble()<<pw<<data["PIXEL_WIDTH"];
                }
            }
        }
        {
            bool found=false;
            QString prefix;
            foreach(prefix, prefixes) {
                if (!found && !data.contains("PIXEL_HEIGHT")  ) {
                    double mag=settings.value(prefix+"magnification", data.value("MAGNIFICATION", 1.0).toDouble()).toDouble();
                    double cpw=settings.value(prefix+"camera_pixel_height", 0.0).toDouble();
                    double pw=settings.value(prefix+"pixel_height", 0.0).toDouble();
                    if ((cpw>0) && (mag!=1)) { data["PIXEL_HEIGHT"]=cpw/mag*1000.0; found=true; }
                    else if (pw>0) { data["PIXEL_HEIGHT"]=pw*1000.0; found=true; }

                    found=true;
                }
            }
        }
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/cell", "EXP_CELL", toInt)
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/plate", "EXP_PLATE", toInt)
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/well", "EXP_WELL", toInt)
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/samplename", "EXP_SAMPLENAME", toString)
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/date", "EXP_DATE", toString)
        qfimdtGetQuickFitMetaData_FINDPROPNOPREFIX("experiment/start_time", "EXP_DATE", toString)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser1/line1/measured_power", "EXP_LASER1_MEASURED", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser2/line1/measured_power", "EXP_LASER2_MEASURED", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser3/line1/measured_power", "EXP_LASER3_MEASURED", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/transmission/line1/measured_power", "EXP_TRANSMISSION_MEASURED", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/lightpath_name", "EXP_LIGHTPATH_NAME", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser1/line1/set_power", "EXP_LASER1_SET", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/detection/name", "EXP_OBJECTIVE_DETECTION", toString)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/detection/NA", "EXP_OBJECTIVE_DETECTION_NA", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/detection/magnification", "EXP_OBJECTIVE_DETECTION_MAGNIFICATION", toDouble)

        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens/name", "EXP_TUBELENS_DETECTION", toString)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens/NA", "EXP_TUBELENS_NA", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens/magnification", "EXP_TUBELENS_MAGNIFICATION", toDouble)

        if (camera>0) {
            qfimdtGetQuickFitMetaData_FINDPROP(QString("setup/objectives/tube_lens%1/name").arg(camera), "EXP_TUBELENS_DETECTION", toString)
            qfimdtGetQuickFitMetaData_FINDPROP(QString("setup/objectives/tube_lens%1/NA").arg(camera), "EXP_TUBELENS_NA", toDouble)
            qfimdtGetQuickFitMetaData_FINDPROP(QString("setup/objectives/tube_lens%1/magnification").arg(camera), "EXP_TUBELENS_MAGNIFICATION", toDouble)
        }
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens1/name", "EXP_TUBELENS_DETECTION", toString)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens1/NA", "EXP_TUBELENS_NA", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens1/magnification", "EXP_TUBELENS_MAGNIFICATION", toDouble)

        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens2/name", "EXP_TUBELENS_DETECTION", toString)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens2/NA", "EXP_TUBELENS_NA", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("setup/objectives/tube_lens2/magnification", "EXP_TUBELENS_MAGNIFICATION", toDouble)

        if (!data.contains("EXP_LASER1_SET")) data["EXP_LASER1_SET"]=0.0;
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser2/line1/set_power", "EXP_LASER2_SET", toDouble)
        if (!data.contains("EXP_LASER1_SET")) data["EXP_LASER2_SET"]=0.0;
        qfimdtGetQuickFitMetaData_FINDPROP("setup/laser3/line1/set_power", "EXP_LASER3_SET", toDouble)
        if (!data.contains("EXP_LASER1_SET")) data["EXP_LASER3_SET"]=0.0;
        qfimdtGetQuickFitMetaData_FINDPROP("emgain_enabled", "EXP_CAMERA_EMGAIN_ENABLED", toBool)
        if ((data.contains("EXP_CAMERA_EMGAIN_ENABLED") && data["EXP_CAMERA_EMGAIN_ENABLED"].toBool()) || (!data.contains("EXP_CAMERA_EMGAIN_ENABLED"))) {
            qfimdtGetQuickFitMetaData_FINDPROP("emgain", "EXP_CAMERA_EMGAIN", toDouble)
        } else {
            if (!data.contains("EXP_CAMERA_EMGAIN")) data["EXP_CAMERA_EMGAIN"]=0;
        }
        data.remove("EXP_CAMERA_EMGAIN_ENABLED");
        qfimdtGetQuickFitMetaData_FINDPROP("preamplifier_gain", "EXP_CAMERA_ANALOG_GAIN", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("analog_gain", "EXP_CAMERA_ANALOG_GAIN", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("gain", "EXP_CAMERA_ANALOG_GAIN", toDouble)
        if (!data.contains("EXP_CAMERA_ANALOG_GAIN")) data["EXP_CAMERA_ANALOG_GAIN"]=1;

        qfimdtGetQuickFitMetaData_FINDPROP("exposure_time", "EXP_CAMERA_EXPOSURE", toDouble)
        qfimdtGetQuickFitMetaData_FINDPROP("exposure", "EXP_CAMERA_EXPOSURE", toDouble)


        // write results to call-by-ref parameters
        if (configfile_out) *configfile_out=configfile;
        if (filedescription_out) *filedescription_out=filedescription;
    }
    return res;
}


