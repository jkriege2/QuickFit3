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

QFRDRNumberAndBrightnessPlugin::QFRDRNumberAndBrightnessPlugin(QObject* parent):
    QObject(parent)
{
    //constructor
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
    QMenu* m=menu->addMenu(QIcon(getIconFilename()), tr("&Number & Brightness Analysis"));

    QAction* action=new QAction(QIcon(getIconFilename()), tr("&load from imFCS dataset"), parentWidget);
    action->setStatusTip(tr("Insert a new record with data from an imaging FCS evaluation"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertFromImFCSRecord()));
    m->addAction(action);

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


            // insert new record:                  type ID, name for record,           list of files,    initial parameters, which parameters are readonly?
            QFRawDataRecord* e=project->addRawData(getID(), QFileInfo(filename).fileName()+QString(" - N&B"), files, initParams, paramsReadonly, files_types, files_descriptions);
            if (!filename_acquisition.isEmpty()) {
                e->setFolder(QFileInfo(filename_acquisition).baseName());
            }
            if (!description.isEmpty()) e->setDescription(description);
            if (e->error()) { // when an error occured: remove record and output an error message
                QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(filename).arg(e->errorDescription()));
                services->log_error(tr("Error while importing '%1':\n    %2\n").arg(filename).arg(e->errorDescription()));
                project->deleteRawData(e->getID());
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


Q_EXPORT_PLUGIN2(target_id, QFRDRNumberAndBrightnessPlugin)
