/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#include "qfrdrnumberandbrightness.h"
#include "qfrdrnumberandbrightness_data.h"
#include "qfrdrnumberandbrightness_dataeditor.h"
#include "qfselectfileslistwidget.h"
#include "qfwizard.h"
#include "qfselectfileslistwidget.h"
#include "qfpluginservices.h"
#include "qfevaluationitemfactory.h"
#include "qfimageplot.h"
#include "imagetools.h"
#include "libtiff_tools.h"

QFRDRNumberAndBrightnessPlugin::QFRDRNumberAndBrightnessPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
    TIFFSetWarningHandler(0);
    TIFFSetErrorHandler(0);
}

QFRDRNumberAndBrightnessPlugin::~QFRDRNumberAndBrightnessPlugin()
{
    //destructor
}

QFRawDataRecord* QFRDRNumberAndBrightnessPlugin::createRecord(QFProject* parent) {
	// factory method: create a QFRawDataRecord objectof the type of this plugin (QFRDRNumberAndBrightnessData)
    return new QFRDRNumberAndBrightnessData(parent);
}


void QFRDRNumberAndBrightnessPlugin::registerToMenu(QMenu* menu) {
    QMenu* m=menu->addMenu(QIcon(getIconFilename()), tr("&Number && Brightness Analysis"));

    QAction* action=new QAction(QIcon(getIconFilename()), tr("&load from imFCS dataset"), parentWidget);
    action->setStatusTip(tr("Insert a new record with data from an imaging FCS evaluation"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertFromImFCSRecord()));
    m->addAction(action);

    action=new QAction(QIcon(getIconFilename()), tr("&load from average+StdDev images (TIFF)"), parentWidget);
    action->setStatusTip(tr("Insert a new record with data from four files, that contain the average/stddev of the image and the backrgound."));
    connect(action, SIGNAL(triggered()), this, SLOT(startNANDBFromPreprocessedFilesWizard()));
    m->addAction(action);

    action=new QAction(QIcon(getIconFilename()), tr("&calculate from image stack(s)"), parentWidget);
    action->setStatusTip(tr("Calculates the average and standard deviation from image stacks and adds N&B records from these datasets to the project."));
    connect(action, SIGNAL(triggered()), this, SLOT(startWizardImgeStack()));
    m->addAction(action);

    QFPluginServices::getInstance()->registerWizard("rdr_wizards", action);
}


void QFRDRNumberAndBrightnessPlugin::insertFromImFCSRecord() {
    if (project) {
        // file format to import
        QString format_imFCS=tr("imFCS evaluation results (*.evalsettings.txt)");
        QString format_imFCS_short=tr("imFCS evaluation results");
        QStringList formats, formats_short;
        formats<<format_imFCS;
        formats_short<<format_imFCS_short;
        // look into INI which was the last used format
        QString current_format_name=settings->getQSettings()->value("number_and_brightness/current_format_filter", format_imFCS).toString();
        // let the user select some files to import
        QStringList files = qfGetOpenFileNames(parentWidget,
                              tr("Select Data File(s) to Import ..."),
                              settings->getCurrentRawDataDir(),
                              formats.join(";;"), &current_format_name, QFileDialog::ReadOnly);
        // store the format we just used
        settings->getQSettings()->setValue("number_and_brightness/current_format_filter", current_format_name);
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
            if (current_format_name==format_imFCS) {
                QString filename=*it;
                insertImFCSFile(filename);

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



void QFRDRNumberAndBrightnessPlugin::insertProjectRecord(const QString &type, const QString &name, const QString &filename, const QString &description, const QString &directory, const QMap<QString,QVariant>& init_params, const QStringList& init_params_readonly) {
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

bool QFRDRNumberAndBrightnessPlugin::parseSPIMSettings(const QString& filename_settings, QString& description, QMap<QString,QVariant>& initParams, QStringList& paramsReadonly, QStringList& files, QStringList& files_types, QStringList& files_descriptions)
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


void QFRDRNumberAndBrightnessPlugin::insertImFCSFile(const QString& filename) {
    // here we store some initial parameters
    QMap<QString, QVariant> initParams;
    QString filename_overview="";
    QString filename_overviewstd="";
    QString filename_background="";
    QString filename_backgroundstddev="";
    QString filename_settings="";
    QString filename_mask="";
    QString filename_video="";
    QString filename_acquisition="";

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly;

    bool ok=true;
    int width=0;
    int height=0;
    bool overviewReal=false;
    int dualview=0;

    QStringList more_files, more_files_types, more_files_descriptions;



    if (QFile::exists(filename)) {
        QFile file(filename);
        QDir d=QFileInfo(filename).absoluteDir();
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
                        initParams["WIDTH"]=value.toInt();
                        width=value.toInt();
                        paramsReadonly<<"WIDTH";
                    } else if (name=="height") {
                        initParams["HEIGHT"]=value.toInt();
                        height=value.toInt();
                        paramsReadonly<<"HEIGHT";
                    } else if (name=="pixel width") {
                        initParams["PIXEL_WIDTH"]=value.toDouble();
                        paramsReadonly<<"PIXEL_WIDTH";
                    } else if (name=="pixel height") {
                        initParams["PIXEL_HEIGHT"]=value.toDouble();
                        paramsReadonly<<"PIXEL_HEIGHT";
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
                    } else if (name=="baseline") {
                        initParams["BASELINE"]=value.toDouble();
                        paramsReadonly<<"BASELINE";
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
                    } else if (name=="dualview mode") {
                        initParams["DUALVIEW_MODE"]=value.toInt();
                        dualview=value.toInt();
                        paramsReadonly<<"DUALVIEW_MODE";
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
                        if (!overviewReal) filename_overview=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="overview image file real") {
                        filename_overview=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                        overviewReal=true;
                    } else if (name=="overview std image") {
                        filename_overviewstd=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="background image file") {
                        filename_background=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="background stddev") {
                        filename_backgroundstddev=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="video file") {
                        filename_video=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="input description file") {
                        filename_settings=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="input file") {
                        filename_acquisition=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="mask file") {
                        filename_mask=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                    } else if (name=="date/time") {
                        initParams["CORRELATION_DATE"]=value;
                        paramsReadonly<<"CORRELATION_DATE";
                    } else {
                        QString fn=qfCanonicalOrAbsoluteFilePath(d.absoluteFilePath(value));
                        if (QFile::exists(fn) && ((QFileInfo(fn).suffix().toLower()=="tif")||(QFileInfo(fn).suffix().toLower()=="tiff"))) {
                            more_files<<fn;
                            more_files_types<<"display_image";
                            more_files_descriptions<<name;
                        }
                    }
                }
            } while (!line.isNull());
        }

        if (ok) {
            QStringList files, files_types, files_descriptions;
            if (QFile::exists(filename_overview)) {
                files<<filename_overview;
                files_types<<"image";
                files_descriptions<<tr("average image");
            }
            if (QFile::exists(filename_overviewstd)) {
                files<<filename_overviewstd;
                files_types<<"image_std";
                files_descriptions<<tr("standard deviation for overview image");
            }
            if (QFile::exists(filename_video)) {
                files<<filename_video;
                files_types<<"video";
                files_descriptions<<tr("averaged video");
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
            initParams["WIDTH"]=width;
            initParams["HEIGHT"]=height;

            initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
            paramsReadonly<<"IS_OVERVIEW_SCALED";
            initParams["BACKGROUND_CORRECTED"]=true;
            paramsReadonly<<"BACKGROUND_CORRECTED";

            paramsReadonly<<"SELECT_IMAGE_HALF";

            QString dvname="";
            if (dualview==1) {
                dvname=tr(": left");
                initParams["SELECT_IMAGE_HALF"]="left";
                initParams["WIDTH"]=width/2;
            }
            if (dualview==2) {
                dvname=tr(": top");
                initParams["SELECT_IMAGE_HALF"]="top";
                initParams["HEIGHT"]=height/2;
            }
            // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
            QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - N&B")+dvname, files, initParams, paramsReadonly, files_types, files_descriptions);
            if (!filename_acquisition.isEmpty()) {
                e->setFolder(QFileInfo(filename_acquisition).baseName());
            }
            if (!description.isEmpty()) e->setDescription(description);
            if (e->error()) { // when an error occured: remove record and output an error message
                QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                project->deleteRawData(e->getID());
            }
            QString group="";

            if (dualview>0) {
                if (dualview==1) {
                    dvname=tr(": right");
                    initParams["SELECT_IMAGE_HALF"]="right";
                }
                if (dualview==2) {
                    dvname=tr(": bottom");
                    initParams["SELECT_IMAGE_HALF"]="bottom";
                }
                // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
                QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - N&B")+dvname, files, initParams, paramsReadonly, files_types, files_descriptions);
                group=QFileInfo(filename).fileName();
                if (!filename_acquisition.isEmpty()) {
                    e->setFolder(QFileInfo(filename_acquisition).baseName());

                }
                e->setGroup(project->addOrFindRDRGroup(group));
                if (!description.isEmpty()) e->setDescription(description);
                if (e->error()) { // when an error occured: remove record and output an error message
                    QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                    services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                    project->deleteRawData(e->getID());
                }
            }
            if (e && group.size()>0) {
                e->setGroup(project->addOrFindRDRGroup(group));
            }

            if (QFile::exists(filename_settings)) {
                QFileInfo fi(filename_settings);
                insertProjectRecord("rdr_settings", fi.fileName()+tr(" - acquisition settings"), filename_settings, description, QFileInfo(filename_settings).baseName());
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
                    insertProjectRecord("table", fi.fileName(), files[i], description, QFileInfo(files[i]).baseName(), p, roParams);
                }
            }

        } else {
            services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(tr("width an height are not set properly (>0)")));
        }
    }

}

QFRawDataRecord *QFRDRNumberAndBrightnessPlugin::insertPreprocessedFiles(const QString &filename_overview, const QString & filename_overviewstd, const QString & filename_background, const QString & filename_backgroundstddev, bool filename_overviewstd_isVar, bool filename_backgroundstddev_isVar, const QMap<QString, QVariant>& iParams, const QStringList& iReadonly, const QString& group)
{
    QFRawDataRecord* res=NULL;
    // here we store some initial parameters
    QMap<QString, QVariant> initParams=iParams;
    QString filename_settings="";
    QString filename_mask="";
    QString filename_video="";
    QString filename_acquisition="";

    // add all properties in initParams that will be readonly
    QStringList paramsReadonly=iReadonly;

    int width=0;
    int height=0;
    bool overviewReal=false;

    QStringList more_files, more_files_types, more_files_descriptions;


    QStringList files, files_types, files_descriptions;
    if (QFile::exists(filename_overview)) {
        files<<filename_overview;
        files_types<<"image";
        files_descriptions<<tr("average image");
    }
    if (QFile::exists(filename_overviewstd)) {
        files<<filename_overviewstd;
        if (filename_overviewstd_isVar) files_types<<"image_var";
        else files_types<<"image_std";
        files_descriptions<<tr("standard deviation for overview image");
    }
    if (QFile::exists(filename_video)) {
        files<<filename_video;
        files_types<<"video";
        files_descriptions<<tr("averaged video");
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
        if (filename_backgroundstddev_isVar) files_types<<"background_var";
        else files_types<<"background_stddev";
        files_descriptions<<tr("background standard deviation frame");
    }

    files<<more_files;
    files_types<<more_files_types;
    files_descriptions<<more_files_descriptions;

    QString description;
    parseSPIMSettings(filename_settings,  description,  initParams,  paramsReadonly,  files,  files_types,  files_descriptions);
    if (width>0) initParams["WIDTH"]=width;
    if (height>0) initParams["HEIGHT"]=height;

    initParams["IS_OVERVIEW_SCALED"]=!overviewReal;
    paramsReadonly<<"IS_OVERVIEW_SCALED";
    initParams["BACKGROUND_CORRECTED"]=true;
    paramsReadonly<<"BACKGROUND_CORRECTED";


    // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
    QFRawDataRecord* e=res=project->addRawData(getID(), QFileInfo(filename_overview).fileName()+QString(" - N&B"), files, initParams, paramsReadonly, files_types, files_descriptions);
    if (!filename_acquisition.isEmpty()) {
        e->setFolder(QFileInfo(filename_acquisition).baseName());

    }
    if (group.size()>0) e->setGroup(project->addOrFindRDRGroup(group));;
    if (!description.isEmpty()) e->setDescription(description);
    if (e->error()) { // when an error occured: remove record and output an error message
        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename_overview).arg(e->errorDescription()));
        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename_overview).arg(e->errorDescription()));
        project->deleteRawData(e->getID());
        res=NULL;
    }

    if (QFile::exists(filename_settings)) {
        QFileInfo fi(filename_settings);
        insertProjectRecord("rdr_settings", fi.fileName()+tr(" - acquisition settings"), filename_settings, description, QFileInfo(filename_settings).baseName());
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
            insertProjectRecord("table", fi.fileName(), files[i], description, QFileInfo(files[i]).baseName(), p, roParams);
        }
    }
    return res;

}




















void QFRDRNumberAndBrightnessPlugin::startNANDBFromPreprocessedFilesWizard()
{
    QFWizard* wiz=new QFWizard(parentWidget);
    wiz->setWindowTitle(tr("Number and Brightness Wizard"));


    wiz->addPage(wizSelfiles=new QFSelectFilesWizardPage(tr("Files ...")));
    wizSelfiles->setSubTitle(tr("Select the required files for a number and brightness analysis."));
    QStringList filters;
    filters<<tr("TIFF files (*.tif)");
    wizSelfiles->addFileSelection("intensity average:", filters, true);
    wizSelfiles->addFileSelection("intensity std.dev.:", filters, true);
    QComboBox* cmbImageVar=new QComboBox(wizSelfiles);
    cmbImageVar->addItem("standard deviation image");
    cmbImageVar->addItem("variance image");
    cmbImageVar->setCurrentIndex(0);
    wizSelfiles->addRow("", cmbImageVar);
    wizSelfiles->addFileSelection("background average:", filters, false);
    wizSelfiles->addFileSelection("background std.dev.:", filters, false);
    QComboBox* cmbBackVar=new QComboBox(wizSelfiles);
    cmbBackVar->addItem("standard deviation image");
    cmbBackVar->addItem("variance image");
    cmbBackVar->setCurrentIndex(0);
    wizSelfiles->addRow("", cmbBackVar);
    wizSelfiles->setSettingsIDs("number_and_brightness/last_preprocfileswizard_dir", "number_and_brightness/last_preprocfileswizard_filter");
    wizSelfiles->setOnlyOneFormatAllowed(true);

    wiz->addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image stack preview ...")));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot of the average intensity!"));
    wizSelfiles->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(wizSelfiles, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();


    wizPixelSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizPixelSize->setRange(0,100000);
    wizPixelSize->setSuffix(" nm");
    wizPixelSize->setValue(400);
    wizPixelSize->setDecimals(2);

    wizLabWidth=new QLabel(wizLSAnalysisImgPreview);
    wizLabHeight=new QLabel(wizLSAnalysisImgPreview);

    if (wizPixelSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizPixelSize, "number_and_brightness/last_preprocfileswizard/wizPixelSize");
        wizLSAnalysisImgPreview->addRow(tr("pixel size"), wizPixelSize);
    }

    QComboBox* cmbSubSelect=new QComboBox(wizSelfiles);
    cmbSubSelect->addItem("full image");
    cmbSubSelect->addItem("left half");
    cmbSubSelect->addItem("right half");
    cmbSubSelect->addItem("top half");
    cmbSubSelect->addItem("bottom half");
    cmbSubSelect->addItem("left+right half");
    cmbSubSelect->addItem("top+bottom half");
    connect(cmbSubSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(wizSubimagesChanged(int)));
    cmbSubSelect->setCurrentIndex(0);
    wizLSAnalysisImgPreview->addRow("", cmbSubSelect);
    wizLSAnalysisImgPreview->addRow(tr("image width:"), wizLabWidth);
    wizLSAnalysisImgPreview->addRow(tr("image height:"), wizLabHeight);


    QFTextWizardPage* last;
    wiz->addPage(last=new QFTextWizardPage(tr("Finalize"),tr("You completed this wizard. The selected files will now be inserted as a number and brightness raw data record (RDR) into the project."), wiz));
    last->setFinalPage(true);


    if (wiz->exec()) {
        if (wizPixelSize) {
            ProgramOptions::setConfigQDoubleSpinBox(wizPixelSize, "number_and_brightness/last_preprocfileswizard/wizPixelSize");
        }

        QStringList files=wizSelfiles->files();
        QString filterid=wizSelfiles->fileFilterIDs().value(0);
        int subselect=cmbSubSelect->currentIndex();
        //qDebug()<<"OK"<<files<<filters;
        if (files.size()>0 && !filterid.isEmpty()) {

            //qDebug()<<filterid<<files;
            QMap<QString, QVariant> iParams;
            QStringList iReadonly;
            iReadonly<<"PIXEL_WIDTH"<<"PIXEL_HEIGHT"<<"WIDTH"<<"HEIGHT"<<"SELECT_IMAGE_HALF";
            QString ename="";
            if (wizPixelSize) iParams["PIXEL_WIDTH"]=wizPixelSize->value();
            if (wizPixelSize) iParams["PIXEL_HEIGHT"]=wizPixelSize->value();
            iParams["WIDTH"]=wizRDRImageWidth;
            iParams["HEIGHT"]=wizRDRImageHeight;
            if (subselect==1 || subselect==5) {iParams["SELECT_IMAGE_HALF"]="left"; ename+=tr(" - left");}
            if (subselect==2){ iParams["SELECT_IMAGE_HALF"]="right"; ename+=tr(" - right");}
            if (subselect==3 || subselect==6) {iParams["SELECT_IMAGE_HALF"]="top"; ename+=tr(" - top");}
            if (subselect==4) {iParams["SELECT_IMAGE_HALF"]="bottom"; ename+=tr(" - bottom");}
            QString group;
            if (subselect>=5) group=QFileInfo(files[0]).fileName();

            QFRawDataRecord* e=insertPreprocessedFiles(files[0], files[1], files[2], files[3], cmbImageVar->currentIndex()==1, cmbSubSelect->currentIndex()==1, iParams, iReadonly, group);
            if (e)  {
                e->setName(e->getName()+ename);
            }

            if (subselect>=5) {
                ename="";
                if (subselect==5) {iParams["SELECT_IMAGE_HALF"]="right"; ename+=tr(" - right");}
                if (subselect==6) {iParams["SELECT_IMAGE_HALF"]="bottom"; ename+=tr(" - bottom");}
                e=insertPreprocessedFiles(files[0], files[1], files[2], files[3], cmbImageVar->currentIndex()==1, cmbSubSelect->currentIndex()==1, iParams, iReadonly, group);
                if (e)  {
                    e->setName(e->getName()+ename);
                }
            }


        }
    }
    delete wiz;
}


void QFRDRNumberAndBrightnessPlugin::startWizardImgeStack()
{
    QFWizard* wiz=new QFWizard(parentWidget);
    wiz->setWindowTitle(tr("Number and Brightness from Image Stacks Wizard"));


    wiz->addPage(wizSelfilesList=new QFSelectFilesListWizardPage(tr("Image Stack Files ...")));
    wizSelfilesList->setSubTitle(tr("Select the intensity image stacks, from which to calculate the number and brightness data.\nBelow the files list, you can select a background image stack. Note, that the same background stack will be used for all intensity stacks!"));


    wizSelfilesList->setFilters(QFRDRNumberAndBrightnessData::getImageFilterList(services), QFRDRNumberAndBrightnessData::getImageReaderIDList(services));
    wizSelfilesList->setSettingsIDs("image_stack/last_lightsheetwizard_dir", "image_stack/last_lightsheetwizard_filter");
    wizSelfilesList->setAddOnStartup(false);
    wizSelfilesList->setOnlyOneFormatAllowed(true);

    wiz->addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image stack preview ...")));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot!"));
    wizSelfilesList->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(wizSelfilesList, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizLSAnalysisImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();


    wiz->addPage(wizLSAnalysisImgPreview=new QFImagePlotWizardPage(tr("Image stack preview ...")));
    wizLSAnalysisImgPreview->setSubTitle(tr("Please set the image properties below the overview plot of the average intensity!"));
    wizSelfilesList->setUserOnValidatePage(wizLSAnalysisImgPreview);
    connect(wizSelfilesList, SIGNAL(onValidate(QWizardPage*,QWizardPage*)), this, SLOT(wizImgPreviewOnValidate(QWizardPage*,QWizardPage*)));
    wizLSAnalysisImgPreview->clear();


    wizPixelSize=new QDoubleSpinBox(wizLSAnalysisImgPreview);
    wizPixelSize->setRange(0,100000);
    wizPixelSize->setSuffix(" nm");
    wizPixelSize->setValue(400);
    wizPixelSize->setDecimals(2);

    wizLabWidth=new QLabel(wizLSAnalysisImgPreview);
    wizLabHeight=new QLabel(wizLSAnalysisImgPreview);

    if (wizPixelSize) {
        ProgramOptions::getConfigQDoubleSpinBox(wizPixelSize, "number_and_brightness/last_preprocfileswizard/wizPixelSize");
        wizLSAnalysisImgPreview->addRow(tr("pixel size"), wizPixelSize);
    }

    QComboBox* cmbSubSelect=new QComboBox(wizSelfilesList);
    cmbSubSelect->addItem("full image");
    cmbSubSelect->addItem("left half");
    cmbSubSelect->addItem("right half");
    cmbSubSelect->addItem("top half");
    cmbSubSelect->addItem("bottom half");
    cmbSubSelect->addItem("left+right half");
    cmbSubSelect->addItem("top+bottom half");
    connect(cmbSubSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(wizSubimagesChanged(int)));
    cmbSubSelect->setCurrentIndex(0);
    wizLSAnalysisImgPreview->addRow("", cmbSubSelect);
    wizLSAnalysisImgPreview->addRow(tr("image width:"), wizLabWidth);
    wizLSAnalysisImgPreview->addRow(tr("image height:"), wizLabHeight);


    QFTextWizardPage* last;
    wiz->addPage(last=new QFTextWizardPage(tr("Finalize"),tr("You completed this wizard. The selected files will now be inserted as a number and brightness raw data record (RDR) into the project."), wiz));
    last->setFinalPage(true);


    if (wiz->exec()) {
        if (wizPixelSize) {
            ProgramOptions::setConfigQDoubleSpinBox(wizPixelSize, "number_and_brightness/last_preprocfileswizard/wizPixelSize");
        }

        QStringList files=wizSelfilesList->files();
        QString filterid=wizSelfilesList->fileFilterIDs().value(0);
        int subselect=cmbSubSelect->currentIndex();
        //qDebug()<<"OK"<<files<<filters;
        if (files.size()>0 && !filterid.isEmpty()) {

            //qDebug()<<filterid<<files;
            QMap<QString, QVariant> iParams;
            QStringList iReadonly;
            iReadonly<<"PIXEL_WIDTH"<<"PIXEL_HEIGHT"<<"WIDTH"<<"HEIGHT"<<"SELECT_IMAGE_HALF";
            QString ename="";
            if (wizPixelSize) iParams["PIXEL_WIDTH"]=wizPixelSize->value();
            if (wizPixelSize) iParams["PIXEL_HEIGHT"]=wizPixelSize->value();
            iParams["WIDTH"]=wizRDRImageWidth;
            iParams["HEIGHT"]=wizRDRImageHeight;
            if (subselect==1 || subselect==5) {iParams["SELECT_IMAGE_HALF"]="left"; ename+=tr(" - left");}
            if (subselect==2){ iParams["SELECT_IMAGE_HALF"]="right"; ename+=tr(" - right");}
            if (subselect==3 || subselect==6) {iParams["SELECT_IMAGE_HALF"]="top"; ename+=tr(" - top");}
            if (subselect==4) {iParams["SELECT_IMAGE_HALF"]="bottom"; ename+=tr(" - bottom");}
            QString group;
            if (subselect>=5) group=QFileInfo(files[0]).fileName();

            /*QFRawDataRecord* e=insertPreprocessedFiles(files[0], files[1], files[2], files[3], cmbImageVar->currentIndex()==1, cmbSubSelect->currentIndex()==1, iParams, iReadonly, group);
            if (e)  {
                e->setName(e->getName()+ename);
            }

            if (subselect>=5) {
                ename="";
                if (subselect==5) {iParams["SELECT_IMAGE_HALF"]="right"; ename+=tr(" - right");}
                if (subselect==6) {iParams["SELECT_IMAGE_HALF"]="bottom"; ename+=tr(" - bottom");}
                e=insertPreprocessedFiles(files[0], files[1], files[2], files[3], cmbImageVar->currentIndex()==1, cmbSubSelect->currentIndex()==1, iParams, iReadonly, group);
                if (e)  {
                    e->setName(e->getName()+ename);
                }
            }*/


        }
    }
    delete wiz;
}






















void QFRDRNumberAndBrightnessPlugin::wizImgPreviewOnValidate(QWizardPage *page, QWizardPage *userPage)
{
    QFImagePlotWizardPage* plot=qobject_cast<QFImagePlotWizardPage*>(userPage);
    QFSelectFilesWizardPage* files=qobject_cast<QFSelectFilesWizardPage*>(page);
    if (plot && files) {
        QFImporter::FileInfo info;
        double* image=NULL;
        int width=0, height=0;
        QString filter="imageimporter_libtiff";
        plot->setImage(files->files().value(0, ""), filter, -1, image, width, height, &info);
        if (wizPixelSize) {
            if (info.properties.contains("PIXEL_WIDTH")) {
                wizPixelSize->setValue(info.properties["PIXEL_WIDTH"].toDouble());
            } else if (info.properties.contains("PIXEL_HEIGHT")) {
                wizPixelSize->setValue(info.properties["PIXEL_HEIGHT"].toDouble());
            }
        }

        wizFileImageWidth=width;
        wizFileImageHeight=height;
        wizRDRImageWidth=wizFileImageWidth;
        wizRDRImageHeight=wizFileImageHeight;
        if (wizLabWidth) {
            wizLabWidth->setText(QString::number(width));
        }
        if (wizLabHeight) {
            wizLabHeight->setText(QString::number(height));
        }

        if (wizMaskSize) wizMaskChanged(wizMaskSize->value());
    }
}

void QFRDRNumberAndBrightnessPlugin::wizMaskChanged(int masksize)
{
    if (wizLSAnalysisImgPreview) {
        wizLSAnalysisImgPreview->getImagePlot()->setMaskAround(masksize);
    }
}

void QFRDRNumberAndBrightnessPlugin::wizSubimagesChanged(int index)
{
    int newwidth, newheight;
    if (index==0) {
        newwidth=wizFileImageWidth;
        newheight=wizFileImageHeight;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI();
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI2();
    } else if (index==1) {
        newwidth=wizFileImageWidth;
        newheight=wizFileImageHeight/2;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(0,0,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI2();
    } else if (index==2) {
        newwidth=wizFileImageWidth;
        newheight=wizFileImageHeight/2;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(newwidth,0,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI2();
    } else if (index==3) {
        newwidth=wizFileImageWidth/2;
        newheight=wizFileImageHeight;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(0,0,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI2();
    } else if (index==4) {
        newwidth=wizFileImageWidth/2;
        newheight=wizFileImageHeight;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(0,newheight,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->resetROI2();
    } else if (index==5) {
        newwidth=wizFileImageWidth/2;
        newheight=wizFileImageHeight;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(0,0,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI2(newwidth,0,newwidth, newheight);
    } else if (index==6) {
        newwidth=wizFileImageWidth;
        newheight=wizFileImageHeight/2;
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI(0,0,newwidth, newheight);
        if (wizLSAnalysisImgPreview) wizLSAnalysisImgPreview->getImagePlot()->setROI2(0,newheight,newwidth, newheight);
    }

    if (wizLabWidth) {
        wizLabWidth->setText(tr("%1   [file width: %2]").arg(newwidth).arg(wizFileImageWidth));
    }
    if (wizLabHeight) {
        wizLabHeight->setText(tr("%1   [file width: %2]").arg(newheight).arg(wizFileImageHeight));
    }
    wizRDRImageWidth=newwidth;
    wizRDRImageHeight=newheight;

}


void QFRDRNumberAndBrightnessPlugin::wizLSAnalysisImgPreviewOnValidate(QWizardPage *page, QWizardPage *userPage)
{
    QFImagePlotWizardPage* plot=qobject_cast<QFImagePlotWizardPage*>(userPage);
    QFSelectFilesListWizardPage* files=qobject_cast<QFSelectFilesListWizardPage*>(page);
    if (plot && files) {
        QFImporter::FileInfo info;
        double* image=NULL;
        int width=0, height=0;
        plot->setImage(files->files().value(0, ""), files->fileFilterIDs().value(0, ""), -1, image, width, height, &info);
        if (wizPixelSize) {
            if (info.properties.contains("PIXEL_WIDTH")) {
                wizPixelSize->setValue(info.properties["PIXEL_WIDTH"].toDouble());
            } else if (info.properties.contains("PIXEL_HEIGHT")) {
                wizPixelSize->setValue(info.properties["PIXEL_HEIGHT"].toDouble());
            }
        }


        if (wizLSAnalysiscmbFitDir && image && width>0 && height>0)  {
            QFImageSymmetry s=qfGetImageSymetry(image, width, height);
            if (s==qfisVertical) wizLSAnalysiscmbFitDir->setCurrentIndex(0);
            if (s==qfisHorizonal) wizLSAnalysiscmbFitDir->setCurrentIndex(1);
        }
        if (wizLSAnalysiscmbStackMode && image && width>0 && height>0) {
            double crossLR=0, crossTB=0;
            QFImageSymmetry s=qfGetImageSymetry(image, width, height, &crossLR, &crossTB);
            if (crossLR>0.6 && crossTB<0.6) {
                wizLSAnalysiscmbStackMode->setCurrentIndex(2);
            } else if (crossLR<0.6 && crossTB>0.6) {
                wizLSAnalysiscmbStackMode->setCurrentIndex(3);
            } else if (crossLR<0.6 && crossTB<0.6) {
                if (files->getSelFilesWidget()->files().size()>1) wizLSAnalysiscmbStackMode->setCurrentIndex(0);
                else  wizLSAnalysiscmbStackMode->setCurrentIndex(1);
            }
            const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(wizLSAnalysiscmbStackMode->model());
            if (model) {
                QStandardItem* item = model->item(0);
                bool disable=files->getSelFilesWidget()->files().size()<=1;
                item->setFlags(disable ? item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled) : Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            }
        }
        if (image && width>0 && height>0) {
            qfFree(image);
        }
        if (wizMaskSize) wizMaskChanged(wizMaskSize->value());
    }
}


Q_EXPORT_PLUGIN2(target_id, QFRDRNumberAndBrightnessPlugin)


