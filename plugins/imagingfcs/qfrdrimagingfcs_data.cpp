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

#include <QTextStream>

#include "qfrdrimagingfcs_data.h"
#include <QtXml>
#include "libtiff_tools.h"
#include "qfimporterimageseries.h"
#include "csvtools.h"
#include <QTextStream>
#include "tools.h"
#include "qfrdrimagingfcstools.h"
#include "statistics_tools.h"
#include "csvtools.h"
#include "qfimporter.h"
#include "qfimportermanager.h"
#include "yaid_rh.h"
#include "imagetools.h"

#undef DEBUG_SIZES
//#define DEBUG_SIZES
#undef DEBUG_TIMING
//#define DEBUG_TIMING

QFRDRImagingFCSData::QFRDRImagingFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
    //m_dualview=QFRDRImagingFCSData::dvNone;
    role="correlation";
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    N=0;
    width=0;
    height=0;
    widthOvr=0;
    heightOvr=0;
    overviewF=NULL;
    overviewFSTD=NULL;
    overviewF2=NULL;
    overviewF2STD=NULL;
    stat.Avg=NULL;
    stat.StdDev=NULL;
    stat.Min=NULL;
    stat.Max=NULL;
    stat.T=NULL;
    stat.N=0;
    backStat.Avg=NULL;
    backStat.StdDev=NULL;
    backStat.Min=NULL;
    backStat.Max=NULL;
    backStat.T=NULL;
    backStat.N=0;
    ucStat.Avg=NULL;
    ucStat.StdDev=NULL;
    ucStat.Min=NULL;
    ucStat.Max=NULL;
    ucStat.T=NULL;
    ucStat.N=0;

    stat2.Avg=NULL;
    stat2.StdDev=NULL;
    stat2.Min=NULL;
    stat2.Max=NULL;
    stat2.T=NULL;
    stat2.N=0;
    backStat2.Avg=NULL;
    backStat2.StdDev=NULL;
    backStat2.Min=NULL;
    backStat2.Max=NULL;
    backStat2.T=NULL;
    backStat2.N=0;
    ucStat2.Avg=NULL;
    ucStat2.StdDev=NULL;
    ucStat2.Min=NULL;
    ucStat2.Max=NULL;
    ucStat2.T=NULL;
    ucStat2.N=0;

    video=NULL;
    video2=NULL;
    video_width=0;
    video_height=0;
    video_frames=0;
    hasStatistics=false;
    videoUncorrected=NULL;
    videoUncorrected2=NULL;
    videoUncorrected_width=0;
    videoUncorrected_height=0;
    videoUncorrected_frames=0;
    stat.avgCnt=0;
    stat.sigmaCnt=0;
    stat2.avgCnt=0;
    stat2.sigmaCnt=0;
    backStat.avgCnt=0;
    backStat.sigmaCnt=0;
    backStat2.avgCnt=0;
    backStat2.sigmaCnt=0;
    setResultsInitSize(1000);
    setEvaluationIDMetadataInitSize(1000);
}

QFRDRImagingFCSData::~QFRDRImagingFCSData() {
     allocateContents(0,0,0);
     allocateOverviews(0,0);
     allocateStatistics(stat, 0);
     allocateStatistics(stat2, 0);
     allocateStatistics(ucStat, 0);
     allocateStatistics(ucStat2, 0);
     allocateStatistics(backStat, 0);
     allocateStatistics(backStat2, 0);
     clearOvrImages();
     clearSelections();
     if (video) qfFree(video);
     video=NULL;
     video_width=video_height=video_frames=0;
     if (videoUncorrected) qfFree(videoUncorrected);
     videoUncorrected=NULL;
     videoUncorrected_width=videoUncorrected_height=videoUncorrected_frames=0;
}

int QFRDRImagingFCSData::getEditorCount()
{
    return 4;
}

QString QFRDRImagingFCSData::getEditorName(int i) {
    if (i==0) return tr("Parameter Image");
    if (i==1) return tr("Correlation Curves");
    if (i==2) return tr("Countrate/Intensity Trace");
    if (i==3) return tr("Images, Videos & Timetrace");
    return QString("");
}

QFRawDataEditor* QFRDRImagingFCSData::createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i, QWidget* parent) {
    if (i==0) return new QFRDRImagingFCSImageEditor(services, propEditor, parent);
    if (i==1) return new QFRDRImagingFCSDataEditor(services, propEditor, parent);
    if (i==2) return new QFRDRImagingFCSDataEditorCountrate(services, propEditor, parent);
    if (i==3) return new QFRDRImagingFCSOverviewRateEditor(services, propEditor, parent);
    return NULL;
}


QStringList QFRDRImagingFCSData::getExportFiletypes() {
    QStringList sl;
    return sl;
}

QString QFRDRImagingFCSData::getExportDialogTitle() {
    return tr("");
}

QString QFRDRImagingFCSData::getExportDialogFiletypes() {
    return tr("");
}

int QFRDRImagingFCSData::getSegmentCount() const
{
    return getProperty("SEGMENTS", 0).toInt();
}

double QFRDRImagingFCSData::getSegmentDuration() const
{
    return getMeasurementDuration()/double(getSegmentCount());
}

double QFRDRImagingFCSData::getMeasurementDuration() const
{
    if (propertyExists("MEASUREMENT_DURATION_MS")) return getProperty("MEASUREMENT_DURATION_MS", 0.0).toDouble()/1000.0;
    if (propertyExists("FRAME_COUNT") || propertyExists("FRAMECOUNT") || propertyExists("FRAMES")) {
        double frames=getProperty("FRAME_COUNT", getProperty("FRAMECOUNT", getProperty("FRAMES", 0).toLongLong()).toLongLong()).toLongLong();
        return frames*getFrameTime();
    }
    return 0;
}

double QFRDRImagingFCSData::getFrameTime() const
{
    if (propertyExists("FRAMETIME_MS") && getProperty("FRAMETIME_MS", 0).toDouble()>0) return getProperty("FRAMETIME_MS", 0).toDouble()/1000.0;
    if (propertyExists("FRAME_TIME") && getProperty("FRAME_TIME", 0).toDouble()>0) return getProperty("FRAME_TIME", 0).toDouble();
    if (propertyExists("FRAMETIME") && getProperty("FRAMETIME", 0).toDouble()>0) return getProperty("FRAMETIME", 0).toDouble();
    if (tau && N>0) return tau[0];
    return 0;
}

bool QFRDRImagingFCSData::segmentUsed(int segment) const
{
    QStringList segs=getProperty("SEGMENTS_EXCLUDED", "").toString().split(";");
    for (int i=0; i<segs.size(); i++) {
        bool ok=false;
        if (segment==segs[i].toInt(&ok)) {
            if (ok) return false;
        }
    }
    return true;
}

bool QFRDRImagingFCSData::allSegmentsUsed() const
{
    if (!getProperty("SEGMENTS_USABLE", false).toBool()) return true;
    for (int i=0; i<getSegmentCount(); i++) {
        if (!segmentUsed(i)) return false;
    }
    return true;
}

void QFRDRImagingFCSData::setSegmentUsed(int segment, bool used)
{
    if (!getProperty("SEGMENTS_USABLE", false).toBool()) return;
    QStringList segs=getProperty("SEGMENTS_EXCLUDED", "").toString().split(";");
    if (!used) {
        if (!segs.contains(QString::number(segment))) segs.append(QString::number(segment));
    } else {
        segs.removeAll(QString::number(segment));
    }

    setQFProperty("SEGMENTS_EXCLUDED", segs.join(";"));

    //qDebug()<<getID()<<"setSegmentUsed("<<segment<<", "<<used<<"):   "<<segs.join(";");
}

void QFRDRImagingFCSData::recalcSegmentedAverages()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    log_text(tr("recalculating segmented CF averages ... \n"));
    QString filetype=getProperty("FILETYPE", "unknown").toString();
    bool loaded=false;
    for (int i=0; i<files.size(); i++) {
        if (i<files_types.size()) {
            QString ft=files_types[i].toLower().trimmed();
            if (ft=="acf" || ft=="ccf" || ft=="dccf") {
                if (filetype.toUpper()=="VIDEO_CORRELATOR_BIN") {
                    log_text(tr("   reloading %2 file: %1\n").arg(QFileInfo(files[i]).fileName()).arg(ft));
                    loadVideoCorrelatorFileBin(files[i]);
                    loaded=true;
                }
            }
        }
    }

    if (loaded && internalDualViewMode()!=QFRDRImagingFCSData::dvNone) {
        splitCFsForDualView();
    }

    recalcCorrelations();

    emitRawDataChanged();
    log_text(tr("recalculating segmented CF averages ... DONE!\n"));
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImagingFCSData::intWriteData(QXmlStreamWriter& w) const {
	// write data to the project XML file using the QXmlStreamWriter
    QString l=maskToIndexString(',');

    if (l.size()>0) {
        w.writeStartElement("leaveout");
        w.writeAttribute("list", l);
        w.writeEndElement();
    }
    if (selections.size()>0) {
        w.writeStartElement("selections");
        for (int s=0; s<selections.size(); s++) {
            QString l="";
            bool* sel=selections[s].selection;
            int selSize=getImageSelectionWidth()*getImageSelectionHeight();
            for (int i=0; i<selSize; i++) {
                if (sel[i]) {
                    if (l.size()>0) l+=","+QString::number(i);
                    else l+=QString::number(i);
                }
            }

            w.writeStartElement("selection");
            w.writeAttribute("name", selections[s].name);
            w.writeAttribute("list", l);
            w.writeEndElement();
        }
        w.writeEndElement();
    }
}

void QFRDRImagingFCSData::intReadData(QDomElement* e) {
    leaveoutClear();
    clearSelections();
    overviewTemp.clear();
	// read data from the project XML file



    width=getExpectedFileWidth();//getProperty("FULL_DV_WIDTH", getProperty("WIDTH", 0).toInt()).toInt();
    height=getExpectedFileHeight();//getProperty("FULL_DV_HEIGHT", getProperty("HEIGHT", 0).toInt()).toInt();

    QString filetype=getProperty("FILETYPE", "unknown").toString();

    // now also load the data file(s) this record is linked to
    // an error is reported when no file is given!
    if (files.size()<=0) {
        setError(tr("there are no files in the %1 record!").arg(getName()));
        return;
    }
    bool dataLoaded=false;
    QString acquisitionSettingsFile=getFileForType("acquisition_settings");
    if (acquisitionSettingsFile.isEmpty()) {
        acquisitionSettingsFile=findB040ExperimentDescriptionForData(getFileForType("acf"));
        if (!QFile::exists(acquisitionSettingsFile)) acquisitionSettingsFile=findB040ExperimentDescriptionForData(getFileForType("ccf"));
        if (!QFile::exists(acquisitionSettingsFile)) acquisitionSettingsFile=findB040ExperimentDescriptionForData(getFileForType("dccf"));
        if (QFile::exists(acquisitionSettingsFile) && getFileForType("acquisition_settings").isEmpty()) addFile(acquisitionSettingsFile, "acquisition_settings", "");
    }
    if (files.size()>0) {
        // now we check whether the experiment config file contains additional files, that may be useful for this object and load them
        // (more overview images ...)
        //qDebug()<<getID()<<"opening qcquisition settings "<<acquisitionSettingsFile;
        if (QFile::exists(acquisitionSettingsFile)) {
            QSettings settings(acquisitionSettingsFile, QSettings::IniFormat);
            //qDebug()<<getID()<<"  ... success!!!";
            loadQFPropertiesFromB040SPIMSettingsFile(settings);

            QStringList lfiles, lfiles_types, lfiles_descriptions;
            appendCategorizedFilesFromB040SPIMConfig(settings, lfiles, lfiles_types, lfiles_descriptions);            
            //qDebug()<<getID()<<lfiles<<"\n"<<lfiles_types;
            for (int i=0; i<lfiles.size(); i++) {
                // now we have to do a really thorough check of all files in files, as the file pathes may be a bit
                // different although they point to the same file.
                bool found=false;
                for (int j=0; j<files.size(); j++) {
                    if (QFileInfo(files[j]).canonicalFilePath()==QFileInfo(lfiles[i]).canonicalFilePath()) {
                        found=true;
                        break;
                    }
                }
                if (!found) {
                    files<<QFileInfo(lfiles[i]).canonicalFilePath();
                    files_types<<lfiles_types.value(i, "");
                    while (files_desciptions.size()+1<files_types.size()) files_desciptions<<"";
                    files_desciptions<<lfiles_descriptions.value(i, "");
                }


            }
        }


        // first we load the correlation curves:
        // so far we do not care about the internal dualView mode. For now all data is loaded and it will be split/discarded afterwards.
        QString loadRH2Overview="";
        QString mainDataFile="";
        for (int i=0; i<files.size(); i++) {
            if (i<files_types.size()) {
                QString ft=files_types[i].toLower().trimmed();
                if (ft=="acf" || ft=="ccf" || ft=="dccf") {
                    if (!dataLoaded) {
                        if (filetype.toUpper()=="VIDEO_CORRELATOR_BIN") {
                            mainDataFile=files[i];
                            loadVideoCorrelatorFileBin(files[i]);
                            setQFProperty("DATAFILE_DATE", QFileInfo(files[i]).created(), false, true);
                            setQFProperty("DATAFILE_ABSOLUTE_PATH", QFileInfo(files[i]).absolutePath(), false, true);
                            dataLoaded=true;
                            //qDebug()<<getID()<<"loaded vidcorbin "<<width<<"x"<<height<<" x "<<N;
                        } else if (filetype.toUpper()=="VIDEO_CORRELATOR") {
                            mainDataFile=files[i];
                            loadVideoCorrelatorFile(files[i]);
                            setQFProperty("DATAFILE_DATE", QFileInfo(files[i]).created(), false, true);
                            setQFProperty("DATAFILE_ABSOLUTE_PATH", QFileInfo(files[i]).absolutePath(), false, true);
                            dataLoaded=true;
                            //qDebug()<<getID()<<"loaded vidcorn "<<width<<"x"<<height<<" x "<<N;
                        } else if (filetype.toUpper()=="RADHARD2") {
                            mainDataFile=files[i];
                            loadRadhard2File(files[i], false);
                            loadRH2Overview=files[i];
                            setQFProperty("DATAFILE_DATE", QFileInfo(files[i]).created(), false, true);
                            setQFProperty("DATAFILE_ABSOLUTE_PATH", QFileInfo(files[i]).absolutePath(), false, true);
                            dataLoaded=true;
                        } else {
                          setError(tr("filetype '%1' is unknown for Imaging FCS data files (file is '%2')").arg(filetype).arg(files[i]));
                          break;
                        }
                    } else {
                        setError(tr("correlation data has already been loaded for Imaging FCS (filetype: %2, this data file is '%1')").arg(files[i]).arg(ft));
                        break;
                    }
                }
            }
        }

        // workaround for bug: try and find .evalsettings.txt file:
        QString evalsettingsFile=getFileForType("eval_settings");
        if (evalsettingsFile.isEmpty() || !QFile::exists(evalsettingsFile)) {
            evalsettingsFile=findEvalsettingsFileForVideoCorrelatorFile(mainDataFile);
        }
        // read missing data from .evalsettngs.txt
        if (QFile::exists(evalsettingsFile)) {

            // add evalsettings file, if it doesn't exist yet
            bool found=false;
            for (int j=0; j<files.size(); j++) {
                if (QFileInfo(files[j]).canonicalFilePath()==QFileInfo(evalsettingsFile).canonicalFilePath()) {
                    found=true;
                    break;
                }
            }
            if (!found) {
                files<<QFileInfo(evalsettingsFile).canonicalFilePath();
                files_types<<"eval_settings";
                while (files_desciptions.size()+1<files_types.size()) files_desciptions<<"";
                files_desciptions<<tr("valuation settings");
            }



            QString role="";
            int dccfid=-1;
            bool isJanBFile=false;
            bool isCross=false;
            bool isDCCF=false;
            QString evalFilenameH="";
            imFCSFilenameHeuristics(mainDataFile, &evalFilenameH, &isCross, &isDCCF, &dccfid, &isJanBFile, &role);
            QMap<QString, QVariant> initParams;
            QStringList paramsReadonly;
            int width;
            int height;
            QStringList lfiles;
            QStringList lfiles_types;
            QStringList lfiles_descriptions;
            QString filename_settings;
            QString filename_acquisition;
            QString filename_overview;
            QString filename_overviewstd;
            QString filename_background;
            QString filename_backgroundstddev;
            bool overviewReal;
            bool ok=readEvalSettingsFile(evalsettingsFile, isDCCF, initParams, paramsReadonly, width, height, lfiles, lfiles_types, lfiles_descriptions, filename_settings, filename_acquisition, filename_overview, filename_overviewstd, filename_background, filename_backgroundstddev, role, dccfid, overviewReal);

            if (ok) {
                QMapIterator<QString, QVariant> it(initParams);
                while (it.hasNext()) {
                    it.next();
                    if (!propertyExists(it.key())) setQFProperty(it.key(), it.value(), paramsReadonly.contains(it.key()));
                }
            }

            for (int i=0; i<lfiles.size(); i++) {
                // now we have to do a really thorough check of all files in files, as the file pathes may be a bit
                // different although they point to the same file.
                bool found=false;
                for (int j=0; j<files.size(); j++) {
                    if (QFileInfo(files[j]).canonicalFilePath()==QFileInfo(lfiles[i]).canonicalFilePath()) {
                        found=true;
                        break;
                    }
                }
                if (!found) {
                    files<<QFileInfo(lfiles[i]).canonicalFilePath();
                    files_types<<lfiles_types.value(i, "");
                    while (files_desciptions.size()+1<files_types.size()) files_desciptions<<"";
                    files_desciptions<<lfiles_descriptions.value(i, "");
                }


            }
        }

        if (role.toUpper()=="FCCS") {
            deleteProperty("DCCF_DELTAX");
            deleteProperty("DCCF_DELTAY");
        }



        // if in internal dualview mode, we have now loaded the full size data
        // and have to split it and resize the internal datastructures
        // Note: afterwards width/height will be changed!
        if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone) {
            splitCFsForDualView();
            //qDebug()<<getID()<<"split "<<width<<"x"<<height<<" x "<<N;
        }

        if (!dataLoaded) {
            setError(tr("did not find a correlation data file (acf, ccf, dccf, ...) for record"));
            return;
        }

        // now we know the final size of the data:
        allocateOverviews(width, height);

        if (!loadRH2Overview.isEmpty()) {
            loadRadhard2File(loadRH2Overview, true);
        }

        if (overviewTemp.size()==width*height) {
            double* tmp=(double*)qfMalloc(width*height*sizeof(double));
            for (int i=0; i<width*height; i++) tmp[i]=overviewTemp.value(i, 0)/tau[0]/1.0e3;
            splitImage(overviewF, overviewF2, tmp, width, height);
            qfFree(tmp);
        }


        // finally we load all other known and useful associated files into memory:
        // NOTE At this point width and height are determined and may no longer be changed!!!
        hasStatistics=false;
        for (int i=0; i<files.size(); i++) {
            if (i<files_types.size()) {
                QString ft=files_types[i].toLower().trimmed();
                if (ft=="overview") {
                    loadOverview(overviewF, overviewF2, files[i]);
                } else if (ft=="overview_std") {
                    loadOverview(overviewFSTD, overviewF2STD, files[i]);
                } else if (ft=="video") {
                    double facOffset=0;
                    double facA=1;
                    if (propertyExists("VIDEO_AVGMIN") && propertyExists("VIDEO_AVGMAX")) {
                        double vidMin=getProperty("VIDEO_AVGMIN").toDouble();
                        double vidMax=getProperty("VIDEO_AVGMAX").toDouble();
                        facA=(vidMax-vidMin)/double(0xFFFF);
                        facOffset=vidMin;
                    }
                    loadVideo(files[i], &video, &video_width, &video_height, &video_frames, facA, facOffset);
                    if (!propertyExists("FRAME_COUNT")) {
                        setQFProperty("FRAME_COUNT", video_frames);
                    }
                } else if (ft=="video_uncorrected") {
                    double facOffset=0;
                    double facA=1;
                    if (propertyExists("UNCORRECTEDVIDEO_MIN") && propertyExists("UNCORRECTEDVIDEO_MAX")) {
                        double vidMin=getProperty("UNCORRECTEDVIDEO_MIN").toDouble();
                        double vidMax=getProperty("UNCORRECTEDVIDEO_MAX").toDouble();
                        facA=(vidMax-vidMin)/double(0xFFFF);
                        facOffset=vidMin;
                    }
                    loadVideo(files[i], &videoUncorrected, &videoUncorrected_width, &videoUncorrected_height, &videoUncorrected_frames, facA, facOffset);
                } else if (ft=="background") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("background frame");
                    ovrImages.append(img);
                } else if (ft=="background_stddev") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("background standard deviation frame");
                    ovrImages.append(img);
                } else if (ft=="display_image") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=files_desciptions.value(i, tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName()));
                    if (img.name.isEmpty()) img.name=tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName());
                    ovrImages.append(img);
                } else if (ft=="overview_before") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    //img.name=tr("overview before acquisition");
                    img.name=tr("overview #%1 before: %2").arg(i+1).arg(files_desciptions.value(i, QFileInfo(files[i]).fileName()));
                    if (propertyExists("ROI_X_START") && propertyExists("ROI_X_END") && propertyExists("ROI_Y_START") && propertyExists("ROI_Y_END")) {
                        QFRDROverviewImageInterface::OverviewImageGeoElement rect;
                        rect.title=tr("ROI");
                        rect.type=QFRDROverviewImageInterface::PIGErectangle;
                        rect.x=qMin(getProperty("ROI_X_START").toDouble(), getProperty("ROI_X_END").toDouble());
                        rect.width=fabs(getProperty("ROI_X_START").toDouble()-getProperty("ROI_X_END").toDouble());
                        rect.y=qMin(getProperty("ROI_Y_START").toDouble(), getProperty("ROI_Y_END").toDouble());
                        rect.height=fabs(getProperty("ROI_Y_START").toDouble()-getProperty("ROI_Y_END").toDouble());
                        img.geoElements.append(rect);;
                    }
                    ovrImages.append(img);
                } else if (ft=="overview_after") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    //img.name=tr("overview after acquisition");
                    img.name=tr("overview #%1 after: %2").arg(i+1).arg(files_desciptions.value(i, QFileInfo(files[i]).fileName()));
                    if (propertyExists("ROI_X_START") && propertyExists("ROI_X_END") && propertyExists("ROI_Y_START") && propertyExists("ROI_Y_END")) {
                        QFRDROverviewImageInterface::OverviewImageGeoElement rect;
                        rect.title=tr("ROI");
                        rect.type=QFRDROverviewImageInterface::PIGErectangle;
                        rect.x=qMin(getProperty("ROI_X_START").toDouble(), getProperty("ROI_X_END").toDouble());
                        rect.width=fabs(getProperty("ROI_X_START").toDouble()-getProperty("ROI_X_END").toDouble());
                        rect.y=qMin(getProperty("ROI_Y_START").toDouble(), getProperty("ROI_Y_END").toDouble());
                        rect.height=fabs(getProperty("ROI_Y_START").toDouble()-getProperty("ROI_Y_END").toDouble());
                        img.geoElements.append(rect);;
                    }
                    ovrImages.append(img);
                } else if (ft=="overview_before_transmission") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("overview before acquisition (transm. illumination)");
                    if (propertyExists("ROI_X_START") && propertyExists("ROI_X_END") && propertyExists("ROI_Y_START") && propertyExists("ROI_Y_END")) {
                        QFRDROverviewImageInterface::OverviewImageGeoElement rect;
                        rect.title=tr("ROI");
                        rect.type=QFRDROverviewImageInterface::PIGErectangle;
                        rect.x=qMin(getProperty("ROI_X_START").toDouble(), getProperty("ROI_X_END").toDouble());
                        rect.width=fabs(getProperty("ROI_X_START").toDouble()- getProperty("ROI_X_END").toDouble());
                        rect.y=qMin(getProperty("ROI_Y_START").toDouble(), getProperty("ROI_Y_END").toDouble());
                        rect.height=fabs(getProperty("ROI_Y_START").toDouble()- getProperty("ROI_Y_END").toDouble());
                        img.geoElements.append(rect);;
                    }
                    ovrImages.append(img);
                } else if (ft=="overview_after_transmission") {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("overview after acquisition (transm. illumination)");
                    if (propertyExists("ROI_X_START") && propertyExists("ROI_X_END") && propertyExists("ROI_Y_START") && propertyExists("ROI_Y_END")) {
                        QFRDROverviewImageInterface::OverviewImageGeoElement rect;
                        rect.title=tr("ROI");
                        rect.type=QFRDROverviewImageInterface::PIGErectangle;
                        rect.x=qMin(getProperty("ROI_X_START").toDouble(), getProperty("ROI_X_END").toDouble());
                        rect.width=fabs(getProperty("ROI_X_START").toDouble()- getProperty("ROI_X_END").toDouble());
                        rect.y=qMin(getProperty("ROI_Y_START").toDouble(), getProperty("ROI_Y_END").toDouble());
                        rect.height=fabs(getProperty("ROI_Y_START").toDouble()- getProperty("ROI_Y_END").toDouble());
                        img.geoElements.append(rect);;
                    }
                    ovrImages.append(img);
                } else if (ft.startsWith("overview_")) {
                    QFRDRImagingFCSData::ovrImageData img;
                    img.id=ft;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=files_desciptions.value(i, ft);
                    if (propertyExists("ROI_X_START") && propertyExists("ROI_X_END") && propertyExists("ROI_Y_START") && propertyExists("ROI_Y_END")) {
                        QFRDROverviewImageInterface::OverviewImageGeoElement rect;
                        rect.title=tr("ROI");
                        rect.type=QFRDROverviewImageInterface::PIGErectangle;
                        rect.x=qMin(getProperty("ROI_X_START").toDouble(), getProperty("ROI_X_END").toDouble());
                        rect.width=fabs(getProperty("ROI_X_START").toDouble()- getProperty("ROI_X_END").toDouble());
                        rect.y=qMin(getProperty("ROI_Y_START").toDouble(), getProperty("ROI_Y_END").toDouble());
                        rect.height=fabs(getProperty("ROI_Y_START").toDouble()- getProperty("ROI_Y_END").toDouble());
                        img.geoElements.append(rect);;
                    }
                    ovrImages.append(img);


                } else {
                    if (internalDualViewMode()==QFRDRImagingFCSData::dvNone) {
                        if (ft=="statistics") {
                            loadStatistics(stat, files[i]);
                            hasStatistics=true;
                        } else if (ft=="background_statistics") {
                            loadStatistics(backStat, files[i]);
                        } else if (ft=="uncorrected_statistics") {
                            loadStatistics(ucStat, files[i]);
                        }
                    } else {
                        //if (internalDualViewModeChannel()==0) {
                            if (ft=="statistics_dv1") {
                                loadStatistics(stat, files[i]);
                                hasStatistics=true;
                            } else if (ft=="background_statistics_dv1") {
                                loadStatistics(backStat, files[i]);
                            } else if (ft=="uncorrected_statistics_dv1") {
                                loadStatistics(ucStat, files[i]);
                            } else if (ft=="statistics_dv2") {
                                loadStatistics(stat2, files[i]);
                                hasStatistics2=true;
                            } else if (ft=="background_statistics_dv2") {
                                loadStatistics(backStat2, files[i]);
                            } else if (ft=="uncorrected_statistics_dv2") {
                                loadStatistics(ucStat2, files[i]);
                            }
                        /*} else if (internalDualViewModeChannel()==1) {
                            if (ft=="statistics_dv1") {
                                loadStatistics(stat2, files[i]);
                                hasStatistics2=true;
                            } else if (ft=="background_statistics_dv1") {
                                loadStatistics(backStat2, files[i]);
                            } else if (ft=="uncorrected_statistics_dv1") {
                                loadStatistics(ucStat2, files[i]);
                            } else if (ft=="statistics_dv2") {
                                loadStatistics(stat, files[i]);
                                hasStatistics=true;
                            } else if (ft=="background_statistics_dv2") {
                                loadStatistics(backStat, files[i]);
                            } else if (ft=="uncorrected_statistics_dv2") {
                                loadStatistics(ucStat, files[i]);
                            }
                        }*/
                    }
                }
            }
        }
        if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone) {
            splitVideosForDualView();
        }

        if (!hasStatistics && !hasStatistics2 && internalDualViewMode()!=QFRDRImagingFCSData::dvNone) {
            for (int i=0; i<files.size(); i++) {
                if (i<files_types.size()) {
                    QString ft=files_types[i].toLower().trimmed();
                    if (ft=="statistics") {
                        loadStatistics(stat, files[i]);
                        hasStatistics=true;
                    } else if (ft=="background_statistics") {
                        loadStatistics(backStat, files[i]);
                    } else if (ft=="uncorrected_statistics") {
                        loadStatistics(ucStat, files[i]);
                    }
                }
            }
        }


    }


    if (!dataLoaded) {
        setError(tr("did not find a correlation data file (acf, ccf, dccf, ...) for record"));
    } else {
        loadPostProcess();
    }

    if (e) {
        QDomElement te=e->firstChildElement("leaveout");
        QString l=te.attribute("list");
        QStringList li=l.split(",");
        for (int i=0; i<li.size(); i++) {
            bool ok=false;
            int lo=li[i].toUInt(&ok);
            if (ok) leaveoutAddRun(lo);
        }
        recalcCorrelations();

        te=e->firstChildElement("selections");
        if (!te.isNull()) {
            int cnt=1;
            te=te.firstChildElement("selection");
            while (!te.isNull()) {
                QString n=te.attribute("name", tr("selection #%1").arg(cnt));
                QString l=te.attribute("list", "");
                int selSize=getImageSelectionWidth()*getImageSelectionHeight();
                QFRDRImagingFCSData::ImageSelection sel;
                sel.selection=(bool*)qfCalloc(selSize, sizeof(bool));
                for (int i=0; i<selSize; i++) sel.selection[i]=false;
                sel.name=n;

                QStringList li=l.split(",");
                for (int i=0; i<li.size(); i++) {
                    bool ok=false;
                    int lo=li[i].toUInt(&ok);
                    if (ok && lo>=0 && lo<selSize) {
                        sel.selection[lo]=true;
                    }
                }

                selections.append(sel);

                te=te.nextSiblingElement("selection");
                cnt++;
            }
        }


    }


    //qDebug()<<"loaded:   "<<width<<"x"<<height<<"  N="<<N<<"   channels="<<getSimpleCountrateChannels();
}

bool QFRDRImagingFCSData::loadOverview(double* overviewF, double* overviewF2, const QString& filename) {
    bool ok=false;

    if (!overviewF && !overviewF2) return false;

    //qDebug()<<getID()<<"loadedOverview "<<filename;
    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            double* tmp=(double*)qfMalloc(nx*ny*sizeof(double));
            ok=TIFFReadFrame<double>(tif, tmp);
            TIFFClose(tif);

            if (ok) {

                splitImage(overviewF, overviewF2, tmp, nx, ny);
            }
            qfFree(tmp);
        } else {
            log_warning(tr("WARNING: could not load overview image file '%1'\n").arg(filename));
        }
    } else {
        log_warning(tr("WARNING: could not find overview image file '%1'\n").arg(filename));
    }

    if (!ok && overviewF) {
        for (int i=0; i<width*height; i++) {
            overviewF[i]=0;
        }
    }
    if (!ok && overviewF2) {
        for (int i=0; i<width*height; i++) {
            overviewF2[i]=0;
        }
    }
    double crscaling=getProperty("OVERVIEW_SCALING", 1.0).toDouble();
    if (crscaling!=1.0 && overviewF) {
        for (int i=0; i<width*height; i++) {
            overviewF[i]=overviewF[i]*crscaling;
        }

    }
    if (crscaling!=1.0 && overviewF2) {
        for (int i=0; i<width*height; i++) {
            overviewF2[i]=overviewF2[i]*crscaling;
        }

    }
    //qDebug()<<getID()<<"loadedOverview "<<filename<<"   OK="<<ok;
    return ok;
}


bool QFRDRImagingFCSData::loadImage(const QString& filename, double** data, int* width, int* height) {
    bool ok=false;

    if (*data) qfFree(*data);
    *data=NULL;
    *width=0;
    *height=0;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            *width=nx;
            *height=ny;
            *data=(double*)qfMalloc(nx*ny*sizeof(double));
            ok=TIFFReadFrame<double>(tif, *data);
            TIFFClose(tif);
        } else {
            log_warning(tr("WARNING: could not load overview image file '%1'\n").arg(filename));
        }
    } else {
        log_warning(tr("WARNING: could not find overview image file '%1'\n").arg(filename));
    }

    double crscaling=getProperty("OVERVIEW_SCALING", 1.0).toDouble();
    if (crscaling!=1.0 && (*data)) {
        for (int i=0; i<(*width)*(*height); i++) {
            (*data)[i]=(*data)[i]*crscaling;
        }

    }
    return ok;
}

bool QFRDRImagingFCSData::loadVideo(const QString& filename, double** data, int* width, int* height, uint32_t* frames, double scaleFactor, double scaleOffset) {
    bool ok=false;

    //qDebug()<<filename<<data<<width<<height<<frames;
    if (!data || !width || !height || !frames) return false;

    if (*data) qfFree(*data);
    *data=NULL;
    *width=0;
    *height=0;
    *frames=0;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            *frames=TIFFCountDirectories(tif);
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            uint16 bitspersample;
            uint16 sampleformat = SAMPLEFORMAT_UINT;
            TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
            TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);

            *width=nx;
            *height=ny;
            if (*frames>0 && *width>0 && *height>0) {
                *data=(double*)qfMalloc(nx*ny*(*frames)*sizeof(double));
                uint32_t i=0;
                if (*data) {
                    do {
                        ok=ok & TIFFReadFrame<double>(tif, &((*data)[i*nx*ny]));
                        if (sampleformat == SAMPLEFORMAT_UINT && bitspersample==16 && (scaleFactor!=1.0 || scaleOffset!=0.0)) {
                            for (uint32_t jj=0; jj<nx*ny; jj++) {
                                (*data)[i*nx*ny+jj]=scaleOffset+(*data)[i*nx*ny+jj]*scaleFactor;
                            }
                        }
                        i++;
                    } while (TIFFReadDirectory(tif) && i<=(*frames));
                }
            } else {
                log_warning(tr("WARNING: could not load overview image file '%1'\n").arg(filename));
            }
            TIFFClose(tif);
            //qDebug()<<getID()<<"loading video "<<filename<<"   siez="<<*width<<"x"<<*height<<"   frames="<<*frames;
        } else {
            log_warning(tr("WARNING: could not load overview image file '%1'\n").arg(filename));
        }
    } else {
        log_warning(tr("WARNING: could not find overview image file '%1'\n").arg(filename));
    }
    return ok;
}

bool QFRDRImagingFCSData::loadVideoCorrelatorFile(const QString &filename) {
	bool ok=true;
	QString errorDescription="";

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

	// LOAD YOUR DATAFILE
	//      and set ok=false, if an error occured
	// example:
	QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		ok=false;
	} else {
        int taucolumn=getProperty("TAU_COLUMN", 0).toInt();
        double taufactor=getProperty("TAU_FACTOR", 1).toDouble();
        double corroffset=getProperty("CORR_OFFSET", 0).toDouble();
        if (taucolumn<0) taucolumn=0;
        int corrcolumn=getProperty("CORRELATION_COLUMN", 1).toInt();
        if (corrcolumn<0) corrcolumn=1;
        int correrrcolumn=getProperty("CORRELATION_ERROR_COLUMN", -1).toInt();
        //int maxCol=qMax(corrcolumn, taucolumn);

        QByteArray dataFromFile=file.readAll();
        file.close();
#ifdef DEBUG_TIMING
        //qDebug()<<getID()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif
        //QTextStream stream(&file);
        QTextStream stream(&dataFromFile);
#ifdef DEBUG_TIMING
        //qDebug()<<getID()<<"read file contents into memory buffer     "<<time.elapsed()<<"ms"; time.start();
#endif
        bool last_empty, empty=true;
        QList<QList<QTriple<double, double, double> > > data_matrix;
        QList<QTriple<double, double, double> > current_set;
        int NN=0;
        int runs=0;
        bool isJanBFile=filename.toLower().endsWith(".qf.dat");
        while ((!stream.atEnd()) && (runs<=width*height)) {
            QVector<double> data;
            if (isJanBFile) data=csvReadline(stream, ' ', '#', 0);
            else data=csvReadline(stream, ',', '#', 0);
            last_empty=empty;
            empty=data.isEmpty();
            if (!empty && (corrcolumn<data.size()) && (taucolumn<data.size())) {
                if ((correrrcolumn<data.size()) && (correrrcolumn>=0)) {
                    if (current_set.size()>0 || (current_set.size()<=0 && data[taucolumn]>0))
                        current_set.append(qMakeTriple(data[taucolumn]*taufactor, data[corrcolumn]-corroffset,  data[correrrcolumn]));
                } else  {
                    if (current_set.size()>0 || (current_set.size()<=0 && data[taucolumn]>0))
                        current_set.append(qMakeTriple(data[taucolumn]*taufactor, data[corrcolumn]-corroffset, 0.0));
                    //qDebug()<<getID()<<"  tau="<<data[0]<<"   c="<<data[1];
                }
            }
            if (((last_empty&&empty)||(stream.atEnd()))&&(!current_set.isEmpty())) {
                // this cuts all lines from current_set which are equal to the lastval (if this is 0.0 or 1.0)
                // this cuts away channels where the correlations have not bee calculated!
                double lastval=0.0;
                if (current_set.size()>0) {
                    lastval=current_set[current_set.size()-1].second;
                    if (lastval==0.0 || lastval==1.0) {
                        while ((current_set.size()>10) && (current_set[current_set.size()-1].second==lastval)) current_set.pop_back();
                    }
                }

                data_matrix.append(current_set);
                if (NN<current_set.size()) NN=current_set.size();
                //qDebug()<<getID()<<"runs="<<runs<<"     NN="<<NN<<"     current_set.size()="<<current_set.size()<<"     data_matrix.size()="<<data_matrix.size()<<"    all0="<<all0<<"    all1="<<all1;
                current_set.clear();
                runs++;
                if (runs%50==0) QApplication::processEvents();
            }
            //if (stream.atEnd()) //qDebug()<<getID()<<"runs="<<runs<<"     NN="<<NN<<"     width*height="<<width*height<<"     stream.atEnd()="<<stream.atEnd()<<"    data="<<data;

        }
#ifdef DEBUG_TIMING
        //qDebug()<<getID()<<"parsed CSV     "<<time.elapsed()<<"ms"; time.start();
#endif
        dataFromFile.clear();
        width=getProperty("WIDTH").toInt();
        height=getProperty("HEIGHT").toInt();
        //std::cout<<"width="<<width<<"   height="<<height<<"   NN="<<NN<<std::endl;
        if (NN>0) {
            if (runs+1>=width*height) {
                allocateContents(width, height, NN);
                for (int i=0; i<width*height; i++) {
                    if (i>=data_matrix.size()) {
                        for (int j=0; j<NN; j++) {
                            correlations[i*NN+j]=0;
                            sigmas[i*NN+j]=0;
                        }
                    } else {
                        for (int j=0; j<NN; j++) {
                            //std::cout<<"i="<<i<<" data_matrix.size()="<<data_matrix.size()<<"    j="<<j<<" data_matrix[i].size()="<<data_matrix[i].size()<<std::endl;
                            if (j<data_matrix[i].size()) {
                                tau[j]=data_matrix[i].at(j).first;
                                correlations[i*NN+j]=data_matrix[i].at(j).second;
                                sigmas[i*NN+j]=data_matrix[i].at(j).third;
                            } else {
                                correlations[i*NN+j]=0;
                                sigmas[i*NN+j]=0;
                            }
                        }
                    }
                    bool allZero=true;
                    for (int p=0; p<NN; p++) {
                        if (correlations[i*NN+p]!=0) {
                            allZero=false;
                            break;
                        }
                    }
                    //mask[i]=allZero;
                    maskSetIdx(i, allZero);
                }
                QApplication::processEvents();
                recalcCorrelations();
                QApplication::processEvents();
            } else {
                ok=false;
                errorDescription=tr("too few records in file: %1 records found, but %2 * %3 = %4 required").arg(NN).arg(width).arg(height).arg(width*height);
            }
        } else {
            ok=false;
            errorDescription=tr("no records found in file");
        }
	}
#ifdef DEBUG_TIMING
        //qDebug()<<getID()<<"read into internal arrays     "<<time.elapsed()<<"ms"; time.start();
#endif

	if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}











bool QFRDRImagingFCSData::loadVideoCorrelatorFileBin(const QString &filename) {
    bool ok=true;
    QString errorDescription="";

    overviewTemp.clear();

#ifdef DEBUG_TIMING
    QElapsedTimer time;
    time.start();
#endif

    // LOAD YOUR DATAFILE
    //      and set ok=false, if an error occured
    // example:
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        ok=false;
    } else {
        uint32_t corr_set=getProperty("CORRELATION_SET", 0).toUInt();
        double taufactor=getProperty("TAU_FACTOR", 1).toDouble();
        double corroffset=getProperty("CORR_OFFSET", 0).toDouble();
#ifdef DEBUG_TIMING
        //qDebug()<<getID()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif
        QByteArray file_id=file.read(10);
        if (file_id=="QF3.0imFCS" || file_id=="QF3.1imFCS") {
            uint32_t fwidth=binfileReadUint32(file);
            uint32_t fheight=binfileReadUint32(file);
            uint32_t fcorrN=binfileReadUint32(file);
            uint32_t fN=binfileReadUint32(file);
            uint32_t fsets=binfileReadUint32(file);

            setQFProperty("WIDTH", fwidth, false, true);
            setQFProperty("HEIGHT", fwidth, false, true);
            setQFProperty("SEGMENTS_USABLE", file_id!="QF3.0imFCS", false, true);
            allocateContents(fwidth, fheight, fN);

            binfileReadDoubleArray(file, tau, N);
            bool tauHas0=false;
            if (N>1 && tau[0]<=0.0) {
                tauHas0=true;
                N=N-1;
                for (int i=0; i<N-1; i++) {
                    tau[i]=tau[i+1];
                }
                //overviewTemp.resize(width*height);
            }

            if (taufactor!=1) for (int i=0; i<N; i++) { tau[i]=tau[i]*taufactor; }
            if (corr_set==0 && fcorrN==1) {
                for (long long p=0; p<width*height; p++) {
                    binfileReadDoubleArray(file, &(correlations[p*N]), fN);
                    if (tauHas0) {
                        //overviewTemp[p]=correlations[p*N];
                        for (int i=0; i<N; i++) {
                            correlations[p*N+i]=correlations[p*N+i+1];
                        }
                    }
                    if (corroffset!=0) for (int i=0; i<N; i++) { correlations[p*N+i]=correlations[p*N+i]-corroffset; }
                    if (fsets>1) {
                        binfileReadDoubleArray(file, &(sigmas[p*N]), fN);
                        if (tauHas0) { for (int i=0; i<N; i++) { sigmas[p*N+i]=sigmas[p*N+i+1]; } }
                    }

                    bool allZero=true;
                    for (int i=0; i<N; i++) {
                        if (correlations[p*N+i]!=0) {
                            allZero=false;
                            break;
                        }
                    }
                    //mask[p]=allZero;
                    maskSetIdx(p, allZero);

                }
                QApplication::processEvents();
            } else {
                for (long long p=0; p<width*height; p++) {
                    for (uint32_t cf=0; cf<fcorrN; cf++) {
                        if (cf==corr_set) {
                            binfileReadDoubleArray(file, &(correlations[p*N]), fN);
                            if (tauHas0) { for (int i=0; i<N; i++) { correlations[p*N+i]=correlations[p*N+i+1]; } }
                            if (corroffset!=0) for (int i=0; i<N; i++) { correlations[p*N+i]=correlations[p*N+i]-corroffset; }
                            if (fsets>1) {
                                binfileReadDoubleArray(file, &(sigmas[p*N]), fN);
                                if (tauHas0) { for (int i=0; i<N; i++) { sigmas[p*N+i]=sigmas[p*N+i+1]; } }
                            }

                            bool allZero=true;
                            for (int i=0; i<N; i++) {
                                if (correlations[p*N+i]!=0 && correlations[p*N+i]!=-1) {
                                    allZero=false;
                                    break;
                                }
                            }
                            //mask[p]=allZero;
                            maskSetIdx(p, allZero);
                        } else {
                            file.seek(file.pos()+N*sizeof(double));
                            if (fsets>1) file.seek(file.pos()+N*sizeof(double));
                        }
                    }
                    QApplication::processEvents();
                }
            }

            long long errorFilesizeFactor=1;
            if (fsets>1) errorFilesizeFactor=2;
            //qDebug()<<getID()<<"filepos before segments: "<<file.pos()<<"  ==  "<<10 + 5*4 + N*sizeof(double) + width*height*N*sizeof(double)*fcorrN*errorFilesizeFactor;
            //file.seek(10 + 5*4 + N*sizeof(double) + width*height*N*sizeof(double)*fcorrN*errorFilesizeFactor);
            if (!file.atEnd()) {
                uint32_t segments=binfileReadUint32(file);
                setQFProperty("SEGMENTS", segments, false, true);
                long long expSize=10 + 5*4 + N*sizeof(double) + width*height*N*sizeof(double)*fcorrN*errorFilesizeFactor+segments*width*height*N*fcorrN*sizeof(double)+4;
                //qDebug()<<getID()<<"filesize="<<file.size()<<"   expected size="<<expSize<<"   DELTA="<<file.size()-expSize;
                //qDebug()<<getID()<<QFileInfo(filename).fileName()<<":\n    segments="<<segments<<"   width="<<width<<"   height="<<height<<"   fcorrN="<<fcorrN<<"   fN="<<fN<<"   fsets="<<fsets<<"   corr_set="<<corr_set<<"   corroffset="<<corroffset;
                if (!allSegmentsUsed() && segments>0) {
                    //qDebug()<<getID()<<"reloading segments ...";
                    for (long long p=0; p<width*height*N; p++) {
                        correlations[p]=sigmas[p]=0;
                    }

                    double* dummy=(double*)qfCalloc(N, sizeof(double));

                    for (long long p=0; p<width*height; p++) {
                        double count=0;
                        for (long long seg=0; seg<segments; seg++) {
                            if (segmentUsed(seg)) {
                                for (uint32_t cf=0; cf<fcorrN; cf++) {
                                    if (cf==corr_set) {
                                        binfileReadDoubleArray(file, dummy, fN); //&(correlations[p*N]), N);
                                        if (tauHas0) { for (int i=0; i<N; i++) { dummy[i]=dummy[i+1]; } }
                                        if (corroffset!=0) for (int i=0; i<N; i++) { dummy[i]=dummy[i]-corroffset; }
                                        //QString txt="";
                                        //for (int i=0; i<5; i++) txt=txt+QString("%1, ").arg(dummy[i]);
                                        //qDebug()<<getID()<<"p="<<p<<" seg="<<seg<<":  "<<txt;

                                        for (int i=0; i<N; i++) {
                                            correlations[p*N+i]=correlations[p*N+i]+dummy[i];
                                            sigmas[p*N+i]=sigmas[p*N+i]+dummy[i]*dummy[i];
                                        }
                                        count++;

                                    } else {
                                        file.seek(file.pos()+N*sizeof(double));
                                    }
                                }
                            } else {
                                //qDebug()<<getID()<<"excluding segment "<<seg;
                                file.seek(file.pos()+N*fcorrN*sizeof(double));
                            }
                        }

                        //qDebug()<<getID()<<"p="<<p<<"   count="<<count<<"   eof:"<<file.atEnd();

                        for (int i=0; i<N; i++) {
                            if (count>1) sigmas[p*N+i]=sqrt((sigmas[p*N+i]-correlations[p*N+i]*correlations[p*N+i]/count)/(count-1.0));
                            else sigmas[p*N+i]=0;
                            correlations[p*N+i]=correlations[p*N+i]/count;
                        }

                        bool allZero=true;
                        for (int i=0; i<N; i++) {
                            if (correlations[p*N+i]!=0 && correlations[p*N+i]!=-1) {
                                allZero=false;
                                break;
                            }
                        }
                        //mask[p]=allZero;
                        maskSetIdx(p, allZero);

                        QApplication::processEvents();
                    }

                    qfFree(dummy);

                }
            }

            QApplication::processEvents();
            recalcCorrelations();
            QApplication::processEvents();
            ok=(file.error()==QFile::NoError);
            if (!ok) {
                errorDescription=tr("error reading data from binary file '%1': %2").arg(filename).arg(file.errorString());
            }
        } else {
            ok=false;
            errorDescription=tr("binary file '%3' did not start with the correct file id: found '%1' but expected '%2'").arg(QString(file_id)).arg(QString("QF3.0imFCS' or 'QF3.1imFCS'")).arg(filename);
        }
        file.close();

    }
#ifdef DEBUG_TIMING
    //qDebug()<<getID()<<"read into internal arrays     "<<time.elapsed()<<"ms"; time.start();
#endif

    if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}

bool QFRDRImagingFCSData::loadRadhard2File(const QString& filename, bool loadOverview) {
    int64_t height=getExpectedFileHeight();
    int64_t width=getExpectedFileWidth();
    int64_t steps=getProperty("STEPS", 0).toInt();
    double corroffset=getProperty("CORR_OFFSET", 0).toDouble();

    if (!propertyExists("IS_OVERVIEW_SCALED")) setQFProperty("IS_OVERVIEW_SCALED", false, false, true);

    //qDebug()<<getID()<<"loadRadhard2File("<<sizeof(int)<<height<<width<<steps<<height*width*steps;

    if((height*width*steps)==0)return false;

    // LOAD FILE
    yaid_rh::corFileReader *cfr=new yaid_rh::corFileReader(filename.toLocal8Bit().constData(),width,height,steps);
    cfr->processFrames(1);

    //qDebug()<<getID()<<"loadRadhard2File("<<cfr->getTotalLagCount();
    if (!loadOverview) {
        allocateContents(width,height,cfr->getTotalLagCount()-1);
        //load correlation data
        for(int i=0; i<width*height; i++) {
            for (uint64_t j=1; j<cfr->getTotalLagCount(); j++) {
                tau[j-1]=cfr->getTau(i,j)*1e-6;
                correlations[i*(cfr->getTotalLagCount()-1)+j-1]=cfr->getVal(i,j)-corroffset;
                sigmas[i*(cfr->getTotalLagCount()-1)+j-1]=0;
            }
            bool allZero=true;
            for (int j=0; j<N; j++) {
                if (correlations[i*N+j]!=0) {
                    allZero=false;
                    break;
                }
            }
            //mask[i]=allZero;
            maskSetIdx(i, allZero);

        }

        QApplication::processEvents();
        recalcCorrelations();
        QApplication::processEvents();
    } else {
        double* tmp=(double*)qfMalloc(width*height*sizeof(double));
        double* tmpStd=(double*)qfMalloc(width*height*sizeof(double));
        for(int i=0; i<width*height; i++) {
            tmp[i]=cfr->getRaw(i,0)/(double(steps)*tau[0])/1.0e3;
            tmpStd[i]=0;
        }
        splitImage(overviewF, overviewF2, tmp, width, height);
        splitImage(overviewFSTD, overviewF2STD, tmpStd, width, height);
        qfFree(tmp);
        qfFree(tmpStd);
    }
    delete cfr;

    //Sfree(corValues);

    return true;
}

int QFRDRImagingFCSData::getCorrelationRuns() const {
    return width*height;
}

long long QFRDRImagingFCSData::getCorrelationN() const {
    return N;
}

double* QFRDRImagingFCSData::getCorrelationT() const {
    return tau;
}

double* QFRDRImagingFCSData::getCorrelation() const {
    return correlations;
}

double* QFRDRImagingFCSData::getCorrelationRunErrors() const {
    return sigmas;
}

bool QFRDRImagingFCSData::dualViewModeUserEditable() const
{
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone) return false;
    return false;
}

QFRDRImagingFCSData::DualViewMode QFRDRImagingFCSData::dualViewMode() const
{
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone || isDCCF() || !dualViewModeUserEditable()) return QFRDRImagingFCSData::dvNone;

    //if ()

    QString dv=getProperty("DUALVIEW_MODE", "none").toString().toLower();


    QFRDRImagingFCSData::DualViewMode m_dualview=QFRDRImagingFCSData::dvNone;
    if (dv=="none" || dv=="0" || dv=="n") m_dualview=QFRDRImagingFCSData::dvNone;
    if (dv=="horizontal" || dv=="1" || dv=="h" || dv=="horicontal") m_dualview=QFRDRImagingFCSData::dvHorizontal;
    if (dv=="vertical" || dv=="2" || dv=="v" || dv=="vertikal") m_dualview=QFRDRImagingFCSData::dvVertical;
    //qDebug()<<getID()<<dv<<m_dualview;

    return m_dualview;
}

void QFRDRImagingFCSData::setDualViewMode(QFRDRImagingFCSData::DualViewMode mode)
{
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone || isDCCF()) return;
    QFRDRImagingFCSData::DualViewMode m_dualview=mode;
    if (m_dualview==QFRDRImagingFCSData::dvNone) setQFProperty("DUALVIEW_MODE", "none", false);
    if (m_dualview==QFRDRImagingFCSData::dvHorizontal) setQFProperty("DUALVIEW_MODE", "horicontal", false);
    if (m_dualview==QFRDRImagingFCSData::dvVertical) setQFProperty("DUALVIEW_MODE", "vertical", false);
}

bool QFRDRImagingFCSData::isFCCS() const
{
    bool fccs=getRole().toLower().startsWith("fccs");
    if (!fccs &&
            ((internalDualViewMode()!=QFRDRImagingFCSData::dvHorizontal && getProperty("DCCF_DELTAX",1).toInt()==getProperty("WIDTH", 0).toInt())
            ||(internalDualViewMode()!=QFRDRImagingFCSData::dvVertical && getProperty("DCCF_DELTAY",1).toInt()==getProperty("HEIGHT", 0).toInt()))) {
        fccs=true;
    }
    return fccs;
}

bool QFRDRImagingFCSData::isDCCF() const
{
    bool fccs=getRole().toLower().startsWith("dccf");
    return fccs;
}

int QFRDRImagingFCSData::getDCCFDeltaX() const
{
    return getProperty("DCCF_DELTAX",0).toInt();
}

int QFRDRImagingFCSData::getDCCFDeltaY() const
{
    return getProperty("DCCF_DELTAY",0).toInt();
}

bool QFRDRImagingFCSData::isACF() const
{
    bool acf=getRole().toLower().startsWith("acf");
    if (acf && isFCCS()) {
        acf=false;
    }
    return acf;
}

bool QFRDRImagingFCSData::overviewImagesSwapped() const
{
    return getProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", false).toBool();
}

int QFRDRImagingFCSData::leaveoutGetRunCount() const
{
    return getCorrelationRuns();
}

bool QFRDRImagingFCSData::leaveoutRun(int run) const {
    if (run<0) return false;
    return maskGet(runToX(run), runToY(run));
}

double* QFRDRImagingFCSData::getCorrelationRun(int run) const {
    if (run<0) return getCorrelationMean();
    return &(correlations[run*N]);
}

double* QFRDRImagingFCSData::getCorrelationRunError(int run) const {
    if (run<0) return getCorrelationStdDev();
    return &(sigmas[run*N]);
}

QString QFRDRImagingFCSData::getCorrelationRunName(int run) const {
    if (run<0) return QObject::tr("average");
    int x=runToX(run);
    int y=runToY(run);
    return QString("[%1, %2]").arg(x).arg(y);
}

double* QFRDRImagingFCSData::getCorrelationMean() const {
    return correlationMean;
}

double* QFRDRImagingFCSData::getCorrelationStdDev() const {
    return correlationStdDev;
}

bool QFRDRImagingFCSData::isCorrelationRunVisible(int run) const {
    if (run<0) return false;
    return !leaveoutRun(run);
}

void QFRDRImagingFCSData::allocateContents(int x, int y, int N) {
    if (x==width && y==height && N==this->N) return;
    if (correlations) qfFree(correlations);
    if (correlationMean) qfFree(correlationMean);
    if (correlationStdDev) qfFree(correlationStdDev);
    if (sigmas) qfFree(sigmas);
    if (tau) qfFree(tau);
    maskDelete();

    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    //qDebug()<<getID()<<"allocateContents "<<x<<"*"<<y<<"  *"<<N;
    if ((x>0) && (y>0) && (N>0)) {
        maskInit(x,y);
        correlations=(double*)qfCalloc(x*y*N,sizeof(double));
        sigmas=(double*)qfCalloc(x*y*N,sizeof(double));
        correlationMean=(double*)qfCalloc(N,sizeof(double));
        correlationStdDev=(double*)qfCalloc(N,sizeof(double));
        width=x;
        height=y;
        tau=(double*)qfCalloc(N,sizeof(double));
        this->N=N;
        setQFProperty("WIDTH", x, false, true);
        setQFProperty("HEIGHT", y, false, true);
    }
#ifdef DEBUG_SIZES
    //qDebug()<<getID()<<"allocateContents( x="<<x<<" y="<<y<<" N="<<N<<"):  "<<bytestostr(N*x*y*2*sizeof(double)+x*y*sizeof(bool)+x*y*sizeof(double)+x*y*sizeof(uint16_t)+N*3*sizeof(double)).c_str();
#endif
}

void QFRDRImagingFCSData::allocateOverviews(int x, int y) {
    if (x==widthOvr && y==heightOvr) return;
    if (overviewF) qfFree(overviewF);
    if (overviewFSTD) qfFree(overviewFSTD);
    if (overviewF2) qfFree(overviewF2);
    if (overviewF2STD) qfFree(overviewF2STD);
    overviewF=NULL;
    overviewF2=NULL;
    overviewFSTD=NULL;
    overviewF2STD=NULL;
    widthOvr=0;
    heightOvr=0;

    //qDebug()<<getID()<<"allocateOverviews "<<x<<"*"<<y;
    if ((x>0) && (y>0)) {
        widthOvr=x;
        heightOvr=y;


        overviewF=(double*)qfCalloc(x*y,sizeof(double));
        overviewFSTD=(double*)qfCalloc(x*y,sizeof(double));
        if (isFCCS() || isDCCF() || internalDualViewMode()!=QFRDRImagingFCSData::dvNone /*&& !getRole().toLower().startsWith("acf")*/) {
            overviewF2=(double*)qfCalloc(x*y,sizeof(double));
            overviewF2STD=(double*)qfCalloc(x*y,sizeof(double));
        }
    }
}

void QFRDRImagingFCSData::allocateStatistics(QFRDRImagingFCSData::statisticsData &stat, uint32_t N)
{
    if (stat.Avg) qfFree(stat.Avg);
    if (stat.StdDev) qfFree(stat.StdDev);
    if (stat.T) qfFree(stat.T);
    if (stat.Min) qfFree(stat.Min);
    if (stat.Max) qfFree(stat.Max);
    stat.N=N;
    stat.Avg=NULL;
    stat.T=NULL;
    stat.StdDev=NULL;
    stat.Min=NULL;
    stat.Max=NULL;
    stat.avgCnt=0;
    stat.sigmaCnt=0;
    if (stat.N>0) {
        stat.Avg=(double*)qfCalloc(N, sizeof(double));
        stat.StdDev=(double*)qfCalloc(N, sizeof(double));
        stat.Min=(double*)qfCalloc(N, sizeof(double));
        stat.Max=(double*)qfCalloc(N, sizeof(double));
        stat.T=(double*)qfCalloc(N, sizeof(double));
    }
#ifdef DEBUG_SIZES
    qDebug()<<getID()<<"allocateStatistics( N="<<N<<"):  "<<bytestostr(statN*5*sizeof(double)).c_str();
#endif
}

void QFRDRImagingFCSData::recalcCorrelations() {
    if (correlations && correlationMean && correlationStdDev) {
        if (sigmas) {
            for (int p=0; p<width*height; p++) {
                double m=0, mc=0;
                for (int i=0; i<N; i++) {
                    if (sigmas[p*N+i]>100.0*DBL_MIN) {m =m+sigmas[p*N+i]; mc++; }
                }
                if (abs(N-mc)<double(N)/20.0) {
                    for (int i=0; i<N; i++) {
                        if (fabs(sigmas[p*N+i])<=100.0*DBL_MIN) sigmas[p*N+i]=m;
                    }
                }
            }
        }

        for (int i=0; i<N; i++) {
            double norm=0;
            double sum=0;
            double sum2=0;
            for (int j=0; j<width*height; j++) {
                const double& v=correlations[j*N+i];
                if (QFFloatIsOK(v) && !maskGetIdx(j)) {
                    sum+=v;
                    sum2+=v*v;
                    norm++;
                }
            }
            if (norm>0) {
                correlationMean[i]=sum/norm;
                correlationStdDev[i]=sqrt(sum2/norm-sum*sum/norm/norm);
            } else {
                correlationMean[i]=0;
                correlationStdDev[i]=0;
            }

            double m=0, mc=0;
            for (int i=0; i<N; i++) {
                if (correlationStdDev[i]>100.0*DBL_MIN) {m =m+correlationStdDev[i]; mc++; }
            }
            if (abs(N-mc)<double(N)/20.0) {
                for (int i=0; i<N; i++) {
                    if (fabs(correlationStdDev[i])<=100.0*DBL_MIN) correlationStdDev[i]=m;
                }
            }
        }
    }
}

uint32_t QFRDRImagingFCSData::getStatisticsN(int channel) const {
    if (channel==0) return stat.N;
    if (channel==1) return stat2.N;
    return 0;
}

double *QFRDRImagingFCSData::getStatisticsMean(int channel) const {
    if (channel==0) return stat.Avg;
    if (channel==1) return stat2.Avg;
    return NULL;
}

double *QFRDRImagingFCSData::getStatisticsT(int channel) const {
    if (channel==0) return stat.T;
    if (channel==1) return stat2.T;
    return NULL;
}

double *QFRDRImagingFCSData::getStatisticsStdDev(int channel) const {
    if (channel==0) return stat.StdDev;
    if (channel==1) return stat2.StdDev;
    return NULL;
}

double *QFRDRImagingFCSData::getStatisticsMin(int channel) const {
    if (channel==0) return stat.Min;
    if (channel==1) return stat2.Min;
    return NULL;
}

double *QFRDRImagingFCSData::getStatisticsMax(int channel) const {
    if (channel==0) return stat.Max;
    if (channel==1) return stat2.Max;
    return NULL;
}

bool QFRDRImagingFCSData::loadStatistics(statisticsData &stat, const QString &filename) {
    QFile f(filename);
    bool ok=false;
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream txt(&f);
        QVector<double> mean, stddev, min, max, time;
        double timescaling=getProperty("STATISTICS_TIME_SCALING", 1.0).toDouble();
        double crscaling=getProperty("STATISTICS_COUNTRATE_SCALING", 1.0).toDouble();
        while (!txt.atEnd()) {
            QVector<double> line=csvReadline(txt, ',', '#');
            if (line.size()>1) {
                time.append(line[0]*timescaling);
                mean.append(line[1]*crscaling);
                stddev.append(line.value(2, 0)*crscaling);
                min.append(line.value(3, 0)*crscaling);
                max.append(line.value(4, 0)*crscaling);
            }
        }
        //qDebug()<<getID()<<"line read: "<<time.size();
        allocateStatistics(stat, time.size());

        for (uint32_t i=0; i<stat.N; i++) {
            stat.T[i]=time[i];
            stat.Avg[i]=mean[i];
            stat.StdDev[i]=stddev[i];
            stat.Min[i]=min[i];
            stat.Max[i]=max[i];
        }


        f.close();
        ok=true;
    }
    if (stat.N>0) stat.avgCnt=statisticsAverageVariance(stat.sigmaCnt, stat.Avg, stat.N);
    return ok;
}



uint32_t QFRDRImagingFCSData::getBackgroundStatisticsN(int channel) const {
    if (channel==0) return backStat.N;
    if (channel==1) return backStat2.N;
    return 0;
}

double *QFRDRImagingFCSData::getBackgroundStatisticsMean(int channel) const {
    if (channel==0) return backStat.Avg;
    if (channel==1) return backStat2.Avg;
    return 0;

}

double *QFRDRImagingFCSData::getBackgroundStatisticsT(int channel) const {
    if (channel==0) return backStat.T;
    if (channel==1) return backStat2.T;
    return 0;
}


double *QFRDRImagingFCSData::getBackgroundStatisticsStdDev(int channel) const {
    if (channel==0) return backStat.StdDev;
    if (channel==1) return backStat2.StdDev;
    return 0;
}

double *QFRDRImagingFCSData::getBackgroundStatisticsMin(int channel) const {
    if (channel==0) return backStat.Min;
    if (channel==1) return backStat2.Min;
    return 0;
}

double *QFRDRImagingFCSData::getBackgroundStatisticsMax(int channel) const {
    if (channel==0) return backStat.Max;
    if (channel==1) return backStat2.Max;
    return 0;

}




uint32_t QFRDRImagingFCSData::getUncorrectedStatisticsN(int channel) const {
    if (channel==0) return ucStat.N;
    if (channel==1) return ucStat2.N;
    return 0;
}

double *QFRDRImagingFCSData::getUncorrectedStatisticsMean(int channel) const {
    if (channel==0) return ucStat.Avg;
    if (channel==1) return ucStat2.Avg;
    return 0;
}

double *QFRDRImagingFCSData::getUncorrectedStatisticsT(int channel) const {
    if (channel==0) return ucStat.T;
    if (channel==1) return ucStat2.T;
    return 0;
}


double *QFRDRImagingFCSData::getUncorrectedStatisticsStdDev(int channel) const {
    if (channel==0) return ucStat.StdDev;
    if (channel==1) return ucStat2.StdDev;
    return 0;
}

double *QFRDRImagingFCSData::getUncorrectedStatisticsMin(int channel) const {
    if (channel==0) return ucStat.Min;
    if (channel==1) return ucStat2.Min;
    return 0;
}

double *QFRDRImagingFCSData::getUncorrectedStatisticsMax(int channel) const {
    if (channel==0) return ucStat.Max;
    if (channel==1) return ucStat2.Max;
    return 0;
}


void QFRDRImagingFCSData::loadQFPropertiesFromB040SPIMSettingsFile(QSettings &settings) {
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/duration_milliseconds")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/duration_milliseconds").toDouble(), true, true);
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/acquisition/duration_milliseconds")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/acquisition/duration_milliseconds").toDouble(), true, true);
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/camera/duration_milliseconds")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/camera/duration_milliseconds").toDouble(), true, true);

    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/duration")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/duration").toDouble()*1000.0, true, true);
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/acquisition/duration")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/acquisition/duration").toDouble()*1000.0, true, true);
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/camera/duration")) setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/camera/duration").toDouble()*1000.0, true, true);

    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/frame_time")) setQFProperty("FRAMETIME_MS", settings.value("acquisition/frame_time").toDouble()*1000.0, true, true);
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/acquisition/frame_time")) setQFProperty("FRAMETIME_MS", settings.value("acquisition/acquisition/frame_time").toDouble()*1000.0, true, true);
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/camera/frame_time")) setQFProperty("FRAMETIME_MS", settings.value("acquisition/camera/frame_time").toDouble()*1000.0, true, true);

    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/frame_rate")) setQFProperty("FRAMETIME_MS", 1.0/settings.value("acquisition/frame_rate").toDouble()*1000.0, true, true);
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/acquisition/frame_rate")) setQFProperty("FRAMETIME_MS", 1.0/settings.value("acquisition/acquisition/frame_rate").toDouble()*1000.0, true, true);
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/camera/frame_rate")) setQFProperty("FRAMETIME_MS", 1.0/settings.value("acquisition/camera/frame_rate").toDouble()*1000.0, true, true);

    if (!propertyExists("MAGNIFICATION") && settings.contains("acquisition/magnification")) setQFProperty("MAGNIFICATION", settings.value("acquisition/magnification").toDouble(), true, true);
    if (!propertyExists("MAGNIFICATION") && settings.contains("acquisition/acquisition/magnification")) setQFProperty("MAGNIFICATION", settings.value("acquisition/acquisition/magnification").toDouble(), true, true);
    if (!propertyExists("MAGNIFICATION") && settings.contains("acquisition/camera/magnification")) setQFProperty("MAGNIFICATION", settings.value("acquisition/camera/magnification").toDouble(), true, true);



    if (!propertyExists("ROI_X_START") && settings.contains("acquisition/roi_xstart"))  setQFProperty("ROI_X_START", settings.value("acquisition/roi_xstart").toInt(), true, true);
    if (!propertyExists("ROI_X_START") && settings.contains("acquisition/acquisition/roi_xstart"))  setQFProperty("ROI_X_START", settings.value("acquisition/acquisition/roi_xstart").toInt(), true, true);
    if (!propertyExists("ROI_X_START") && settings.contains("acquisition/camera/roi_xstart"))  setQFProperty("ROI_X_START", settings.value("acquisition/camera/roi_xstart").toInt(), true, true);

    if (!propertyExists("ROI_X_END") && settings.contains("acquisition/roi_xend"))  setQFProperty("ROI_X_END", settings.value("acquisition/roi_xend").toInt(), true, true);
    if (!propertyExists("ROI_X_END") && settings.contains("acquisition/acquisition/roi_xend"))  setQFProperty("ROI_X_END", settings.value("acquisition/acquisition/roi_xend").toInt(), true, true);
    if (!propertyExists("ROI_X_END") && settings.contains("acquisition/camera/roi_xend"))  setQFProperty("ROI_X_END", settings.value("acquisition/camera/roi_xend").toInt(), true, true);

    if (!propertyExists("ROI_Y_START") && settings.contains("acquisition/roi_ystart"))  setQFProperty("ROI_Y_START", settings.value("acquisition/roi_ystart").toInt(), true, true);
    if (!propertyExists("ROI_Y_START") && settings.contains("acquisition/acquisition/roi_ystart"))  setQFProperty("ROI_Y_START", settings.value("acquisition/acquisition/roi_ystart").toInt(), true, true);
    if (!propertyExists("ROI_Y_START") && settings.contains("acquisition/camera/roi_ystart"))  setQFProperty("ROI_Y_START", settings.value("acquisition/camera/roi_ystart").toInt(), true, true);

    if (!propertyExists("ROI_Y_END") && settings.contains("acquisition/roi_yend")) setQFProperty("ROI_Y_END", settings.value("acquisition/roi_yend").toInt(), true, true);
    if (!propertyExists("ROI_Y_END") && settings.contains("acquisition/acquisition/roi_yend")) setQFProperty("ROI_Y_END", settings.value("acquisition/acquisition/roi_yend").toInt(), true, true);
    if (!propertyExists("ROI_Y_END") && settings.contains("acquisition/camera/roi_yend")) setQFProperty("ROI_Y_END", settings.value("acquisition/camera/roi_yend").toInt(), true, true);

    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/dualview_mode"))  setQFProperty("DUALVIEW_MODE", settings.value("acquisition/dualview_mode").toString(), false, true);
    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/acquisition/dualview_mode"))  setQFProperty("DUALVIEW_MODE", settings.value("acquisition/acquisition/dualview_mode").toString(), false, true);
    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/camera/dualview_mode"))  setQFProperty("DUALVIEW_MODE", settings.value("acquisition/camera/dualview_mode").toString(), false, true);

    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/dualview_mode")) setQFProperty("DUALVIEW_MODE", settings.value("acquisition/dualview_mode").toString(), false, true);
    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/acquisition/acquisition/dualview_mode")) setQFProperty("DUALVIEW_MODE", settings.value("acquisition/acquisition/dualview_mode").toString(), false, true);
    if (!propertyExists("DUALVIEW_MODE") && settings.contains("acquisition/camera/dualview_mode")) setQFProperty("DUALVIEW_MODE", settings.value("acquisition/camera/dualview_mode").toString(), false, true);

    if (!propertyExists("PIXEL_WIDTH") && settings.contains("acquisition/pixel_width")) {
        double mag=settings.value("acquisition/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/camera_pixel_width", settings.value("acquisition/pixel_width").toDouble()).toDouble();
        double pw=settings.value("acquisition/pixel_width").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_WIDTH", cpw/mag, true, true);
        else setQFProperty("PIXEL_WIDTH", pw, true, true);
    }
    if (!propertyExists("PIXEL_WIDTH") && settings.contains("acquisition/acquisition/pixel_width")) {
        double mag=settings.value("acquisition/acquisition/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/acquisition/camera_pixel_width", settings.value("acquisition/acquisition/pixel_width").toDouble()).toDouble();
        double pw=settings.value("acquisition/acquisition/pixel_width").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_WIDTH", cpw/mag, true, true);
        else setQFProperty("PIXEL_WIDTH", pw, true, true);
    }
    if (!propertyExists("PIXEL_WIDTH") && settings.contains("acquisition/camera/pixel_width")) {
        double mag=settings.value("acquisition/camera/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/camera/camera_pixel_width", settings.value("acquisition/camera/pixel_width").toDouble()).toDouble();
        double pw=settings.value("acquisition/camera/pixel_width").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_WIDTH", cpw/mag, true, true);
        else setQFProperty("PIXEL_WIDTH", pw, true, true);
    }


    if (!propertyExists("PIXEL_HEIGHT") && settings.contains("acquisition/pixel_height")) {
        double mag=settings.value("acquisition/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/camera_pixel_height", settings.value("acquisition/pixel_height").toDouble()).toDouble();
        double pw=settings.value("acquisition/pixel_height").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_HEIGHT", cpw/mag, true, true);
        else setQFProperty("PIXEL_HEIGHT", pw, true, true);
    }
    if (!propertyExists("PIXEL_HEIGHT") && settings.contains("acquisition/acquisition/pixel_height")) {
        double mag=settings.value("acquisition/acquisition/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/acquisition/camera_pixel_height", settings.value("acquisition/acquisition/pixel_height").toDouble()).toDouble();
        double pw=settings.value("acquisition/acquisition/pixel_height").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_HEIGHT", cpw/mag, true, true);
        else setQFProperty("PIXEL_HEIGHT", pw, true, true);
    }
    if (!propertyExists("PIXEL_HEIGHT") && settings.contains("acquisition/camera/pixel_height")) {
        double mag=settings.value("acquisition/camera/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/camera/camera_pixel_height", settings.value("acquisition/camera/pixel_height").toDouble()).toDouble();
        double pw=settings.value("acquisition/camera/pixel_height").toDouble();
        if (fabs(cpw/mag-pw)<0.01*fabs(pw) && (mag>1)) setQFProperty("PIXEL_HEIGHT", cpw/mag, true, true);
        else setQFProperty("PIXEL_HEIGHT", pw, true, true);
    }

    QString p, p1, pb;
    if (!propertyExists("EXP_CELL") && settings.contains("experiment/cell")) setQFProperty("EXP_CELL", settings.value("experiment/cell").toInt(), false, true);
    if (!propertyExists("EXP_PLATE") && settings.contains("experiment/plate")) setQFProperty("EXP_PLATE", settings.value("experiment/plate").toInt(), false, true);
    if (!propertyExists("EXP_WELL") && settings.contains("experiment/well")) setQFProperty("EXP_WELL", settings.value("experiment/well").toInt(), false, true);
    if (!propertyExists("EXP_SAMPLENAME") && settings.contains("experiment/samplename")) setQFProperty("EXP_SAMPLENAME", settings.value("experiment/samplename").toString(), false, true);
    if (!propertyExists("EXP_DATE") && settings.contains("experiment/date")) setQFProperty("EXP_DATE", settings.value("experiment/date").toString(), false, true);
    if (!propertyExists("EXP_DATE") && settings.contains("experiment/start_time")) setQFProperty("EXP_DATE", settings.value("experiment/start_time").toString(), false, true);
    if (!propertyExists("EXP_LASER1_MEASURED") && settings.contains("acquisition/acquisition/setup/laser1/line1/measured_power")) setQFProperty("EXP_LASER1_MEASURED", settings.value("acquisition/acquisition/setup/laser1/line1/measured_power").toDouble(), false, true);
    if (!propertyExists("EXP_LASER1_SET") && settings.contains("acquisition/acquisition/setup/laser1/line1/set_power") && settings.contains("acquisition/acquisition/setup/laser1/line1/enabled")) {
        if (settings.value("acquisition/acquisition/setup/laser1/line1/enabled").toBool())
            setQFProperty("EXP_LASER1_SET", settings.value("acquisition/acquisition/setup/laser1/line1/set_power").toDouble(), false, true);
        else
            setQFProperty("EXP_LASER1_SET", 0.0, false, true);
    }
    if (!propertyExists("EXP_LASER2_MEASURED") && settings.contains("acquisition/acquisition/setup/laser2/line1/measured_power")) setQFProperty("EXP_LASER2_MEASURED", settings.value("acquisition/acquisition/setup/laser2/line1/measured_power").toDouble(), false, true);
    if (!propertyExists("EXP_LASER2_SET") && settings.contains("acquisition/acquisition/setup/laser2/line1/set_power") && settings.contains("acquisition/acquisition/setup/laser2/line1/enabled")) {
        if (settings.value("acquisition/acquisition/setup/laser2/line1/enabled").toBool())
            setQFProperty("EXP_LASER2_SET", settings.value("acquisition/acquisition/setup/laser2/line1/set_power").toDouble(), false, true);
        else
            setQFProperty("EXP_LASER2_SET", 0.0, false, true);
    }
    if (!propertyExists(p="EXP_CAMERA_EMGAIN")){
        if (settings.contains(pb="acquisition/acquisition/emgain_enabled")) {
            if (settings.value(pb).toBool()) {
                if (settings.contains(p1="acquisition/acquisition/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
                if (settings.contains(p1="acquisition/camera/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
                else if (settings.contains(p1="acquisition/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
            } else {
                setQFProperty(p, 0.0, false, true);
            }
        } else if (settings.contains(pb="acquisition/emgain_enabled")) {
            if (settings.value(pb).toBool()) {
                if (settings.contains(p1="acquisition/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
                else if (settings.contains(p1="acquisition/acquisition/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
                else if (settings.contains(p1="acquisition/camera/emgain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
            } else {
                setQFProperty(p, 0.0, false, true);
            }
        }
    }
    if (!propertyExists(p="EXP_CAMERA_EXPOSURE")){
        if (settings.contains(p1="acquisition/acquisition/exposure_time")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/exposure_time")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/camera/exposure_time")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/acquisition/exposure")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/camera/exposure")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/exposure")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
    }
    if (!propertyExists(p="EXP_CAMERA_ANALOG_GAIN")){
        if (settings.contains(p1="acquisition/acquisition/preamplifier_gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/camera/preamplifier_gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/preamplifier_gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/acquisition/gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/camera/gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
        else if (settings.contains(p1="acquisition/gain")) setQFProperty(p, settings.value(p1).toDouble(), false, true);
    }

}

double QFRDRImagingFCSData::getTauMin() const {
    if (!tau|| N<=0) return 1;
    int i=0;
    while (i<N-1 && tau[i]==0) i++;
    return tau[i];
}

void QFRDRImagingFCSData::clearOvrImages() {
    for (int i=0; i<ovrImages.size(); i++) {
        qfFree(ovrImages[i].image);
    }
    ovrImages.clear();
}

int QFRDRImagingFCSData::getExpectedFileWidth() const
{
    return getProperty("FULL_DV_WIDTH", getProperty("WIDTH", 0).toInt()).toInt();
}
int QFRDRImagingFCSData::getExpectedFileHeight() const
{
    return getProperty("FULL_DV_HEIGHT", getProperty("HEIGHT", 0).toInt()).toInt();
}

void QFRDRImagingFCSData::splitImage(double* overviewF, double* overviewF2, const double* inputImage, uint32_t nx, uint32_t ny)
{
    if (!isDCCF()) {
        //qDebug()<<getName()<<"\n  splitImage(overviewF="<<overviewF<<",  overviewF2="<<overviewF2<<",  nx="<<nx<<",  ny="<<ny<<")   width = "<<width<<"   height = "<<height<<"   intDV2="<<internalDualViewMode()<<",channel="<<internalDualViewModeChannel();
        if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal && (int64_t)nx>=2*width && (int64_t)ny==height) {
            int shift1=0;
            int shift2=width;
            double* out1=overviewF;
            double* out2=overviewF2;
            if (overviewF2 && !isFCCS()) {
                //out2=NULL;
                if (internalDualViewModeChannel()==0) {
                    shift1=0;
                    shift2=width;
                } else {
                    shift1=width;
                    shift2=0;
                }
                if (!propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL")) {
                    setQFProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", internalDualViewModeChannel()!=0, false);
                }
            }
            if (out1) {
                for (int y=0; y<height; y++) {
                    for (int x=0; x<width; x++) {
                        const int idxIn=y*nx+x+shift1;
                        const int idxOut=y*width+x;
                        out1[idxOut]=inputImage[idxIn];
                    }
                }
                //qDebug()<<"writing out1["<<out2<<"] ="<<out1[0]<<out1[1]<<out1[2];
            }
            if (out2) {
                for (int y=0; y<height; y++) {
                    for (int x=0; x<width; x++) {
                        const int idxIn=y*nx+x+shift2;
                        const int idxOut=y*width+x;
                        out2[idxOut]=inputImage[idxIn];
                    }
                }
                //qDebug()<<"writing out2["<<out2<<"] ="<<out2[0]<<out2[1]<<out2[2];
            }
            return;
        } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical && (int64_t)nx==width && (int64_t)ny>=2*height) {
            int shift1=0;
            int shift2=height;
            double* out1=overviewF;
            double* out2=overviewF2;
            if (overviewF2 && !isFCCS()) {
                //out2=NULL;
                if (internalDualViewModeChannel()==0) {
                    shift1=0;
                    shift2=height;
                } else {
                    shift1=height;
                    shift2=0;
                }
                if (!propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL")) {
                    setQFProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", internalDualViewModeChannel()!=0, false);
                }
            }
            if (out1) {
                for (int y=0; y<height; y++) {
                    for (int x=0; x<width; x++) {
                        const int idxIn=(y+shift1)*nx+x;
                        const int idxOut=y*width+x;
                        out1[idxOut]=inputImage[idxIn];
                    }
                }
            }
            if (out2) {
                for (int y=0; y<height; y++) {
                    for (int x=0; x<width; x++) {
                        const int idxIn=(y+shift2)*nx+x;
                        const int idxOut=y*width+x;
                        out2[idxOut]=inputImage[idxIn];
                    }
                }
            }
            return;
        }
    } else if (isDCCF()) {
        if ((int64_t)nx==width && (int64_t)ny==height) {
            if (overviewF) { memcpy(overviewF, inputImage, nx*ny*sizeof(double)); }
            if (overviewF2) {
                int dx=getDCCFDeltaX();
                int dy=getDCCFDeltaY();
                for (int run=0; run<width*height; run++) {
                    int ry=run/width;
                    int rx=run%width;
                    int rund=(ry+dy)*width+(rx+dx);
                    overviewF2[run]=0;
                    if (rund>=0 && rund<width*height) {
                        overviewF2[run]=inputImage[rund];
                    }
                }
            }
        }
        return;
    }

    if ((int64_t)nx==width && (int64_t)ny==height) {
        if (overviewF) { memcpy(overviewF, inputImage, nx*ny*sizeof(double)); }//qDebug()<<"  !!!copied overviewF!!!"; }
        if (overviewF2) { memcpy(overviewF2, inputImage, nx*ny*sizeof(double)); }//qDebug()<<"  !!!copied overviewF2!!!"; }
    } else {
        //qDebug()<<"  !!!didn't copy!!!";
    }
}

double QFRDRImagingFCSData::readValueFromPossiblySplitImage(const double *inputImage, uint32_t nx, uint32_t ny, int oldIndex, bool readOtherChannel)
{
    //qDebug()<<getID()<<"splitImage(overviewF="<<overviewF<<",  overviewF2="<<overviewF2<<",  nx="<<nx<<",  ny="<<ny<<")   width = "<<width<<"   height = "<<height<<"   intDV2="<<internalDualViewMode()<<",channel="<<internalDualViewModeChannel();
    if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal && (int64_t)nx>=2*width && (int64_t)ny==height) {
        int shift1=0;
        if (!isFCCS()) {
            if (internalDualViewModeChannel()==0) {
                shift1=0;
            } else {
                shift1=width;
            }
        }
        if (readOtherChannel) {
            if (shift1==0) shift1=width;
            else if (shift1==width) shift1=0;
        }
        if (inputImage) {
            int y=oldIndex/width;
            int x=oldIndex%width;
            const int idxIn=y*nx+x+shift1;
            return inputImage[idxIn];
            //qDebug()<<"writing out1["<<out2<<"] ="<<out1[0]<<out1[1]<<out1[2];
        }
    } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical && (int64_t)nx==width && (int64_t)ny>=2*height) {
        int shift1=0;
        if (!isFCCS()) {
            if (internalDualViewModeChannel()==0) {
                shift1=0;
            } else {
                shift1=height;
            }
        }
        if (readOtherChannel) {
            if (shift1==0) shift1=height;
            else if (shift1==height) shift1=0;
        }
        if (inputImage) {
            int y=oldIndex/width;
            int x=oldIndex%width;
            const int idxIn=(y+shift1)*nx+x;
            return inputImage[idxIn];
        }

    } else {
        if ((int64_t)nx==width && (int64_t)ny==height) {
            return inputImage[oldIndex];
        } else {
            return NAN;
        }
    }
    return NAN;
}

float QFRDRImagingFCSData::readValueFromPossiblySplitImage(const float *inputImage, uint32_t nx, uint32_t ny, int oldIndex, bool readOtherChannel)
{
    //qDebug()<<getID()<<"splitImage(overviewF="<<overviewF<<",  overviewF2="<<overviewF2<<",  nx="<<nx<<",  ny="<<ny<<")   width = "<<width<<"   height = "<<height<<"   intDV2="<<internalDualViewMode()<<",channel="<<internalDualViewModeChannel();
    if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal && (int64_t)nx>=2*width && (int64_t)ny==height) {
        int shift1=0;
        if (!isFCCS()) {
            if (internalDualViewModeChannel()==0) {
                shift1=0;
            } else {
                shift1=width;
            }
        }
        if (readOtherChannel) {
            if (shift1==0) shift1=width;
            else if (shift1==width) shift1=0;
        }
        if (inputImage) {
            int y=oldIndex/width;
            int x=oldIndex%width;
            const int idxIn=y*nx+x+shift1;
            return inputImage[idxIn];
            //qDebug()<<"writing out1["<<out2<<"] ="<<out1[0]<<out1[1]<<out1[2];
        }
    } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical && (int64_t)nx==width && (int64_t)ny>=2*height) {
        int shift1=0;
        if (!isFCCS()) {
            if (internalDualViewModeChannel()==0) {
                shift1=0;
            } else {
                shift1=height;
            }
        }
        if (readOtherChannel) {
            if (shift1==0) shift1=height;
            else if (shift1==height) shift1=0;
        }
        if (inputImage) {
            int y=oldIndex/width;
            int x=oldIndex%width;
            const int idxIn=(y+shift1)*nx+x;
            return inputImage[idxIn];
        }

    } else {
        if ((int64_t)nx==width && (int64_t)ny==height) {
            return inputImage[oldIndex];
        } else {
            return NAN;
        }
    }
    return NAN;
}


















bool QFRDRImagingFCSData::indexIsDualView2(int32_t sel) {
    int x=runToX(sel);
    int y=runToY(sel);
    if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal) {
        return x>getImageFromRunsWidth()/2;
    } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical) {
        return y>getImageFromRunsHeight()/2;
    }
    return false;
}


int QFRDRImagingFCSData::getImageFromRunsWidth() const {
    return width;
}

int QFRDRImagingFCSData::getImageFromRunsHeight() const {
    return height;
}

int QFRDRImagingFCSData::getImageFromRunsChannels() const
{

    if (internalDualViewMode()==QFRDRImagingFCSData::dvNone) {
        if (isDCCF() && overviewF2) return 2;
        return 1;
    } else {
        if (overviewF2) return 2;
        //if ((overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL")&&getProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", false).toBool())) || isFCCS()) return 2;
        return 1;
    }
}

int QFRDRImagingFCSData::getImageFromRunsChannelsAdvised() const
{
    if (internalDualViewMode()==QFRDRImagingFCSData::dvNone) {
        return 1;
    } else {
        if ((overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL")) && !isACF()) || isFCCS()) return 2;
        //if ((overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL")&&getProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", false).toBool()) && !isACF()) || isFCCS()) return 2;
        return 1;
    }
}

int QFRDRImagingFCSData::xyToRun(int x, int y) const {
    return y*width+x;
}

int QFRDRImagingFCSData::runToX(int run) const {
    return run%width;
}

int QFRDRImagingFCSData::runToY(int run) const {
    return run/width;
}

int QFRDRImagingFCSData::xyToIndex(int x, int y) const {
    return (y*width+x)*N;
}

double* QFRDRImagingFCSData::getImageFromRunsPreview(int channel) const {
    if (channel==0) return overviewF;
    if (channel==1) return overviewF2;
    return NULL;
}

void QFRDRImagingFCSData::leaveoutClear() {
    maskClear();
}


void QFRDRImagingFCSData::leaveoutRemoveRun(int run) {
    maskSetIdx(run, false);
    //if (run>=0 && run<width*height) mask[run]=false;
}

void QFRDRImagingFCSData::leaveoutAddRun(int run) {
    maskSetIdx(run, true);
    //if (run>=0 && run<width*height) mask[run]=true;
}







int QFRDRImagingFCSData::getOverviewImageCount() const {
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        return 4+ovrImages.size();
    }
    return 2+ovrImages.size();
}

int QFRDRImagingFCSData::getOverviewImageWidth(int image) const {
    if (image==0) return getImageFromRunsWidth();
    if (image==1) return getImageFromRunsWidth();
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if (image==2) return getImageFromRunsWidth();
        if (image==3) return getImageFromRunsWidth();
        if (image-3<=ovrImages.size()) return ovrImages[image-4].width;
        return 0;
    }
    if (image-1<=ovrImages.size()) return ovrImages[image-2].width;
    return 0;
}

int QFRDRImagingFCSData::getOverviewImageHeight(int image) const {
    if (image==0) return getImageFromRunsHeight();
    if (image==1) return getImageFromRunsHeight();
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if (image==2) return getImageFromRunsHeight();
        if (image==3) return getImageFromRunsHeight();
        if (image-3<=ovrImages.size()) return ovrImages[image-4].height;
        return 0;

    }
    if (image-1<=ovrImages.size()) return ovrImages[image-2].height;
    return 0;
}

QString QFRDRImagingFCSData::getOverviewImageName(int image) const {
    if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if ((image==0 && !overviewImagesSwapped())
          ||(image==2 && overviewImagesSwapped())) return tr("top overview image (time average)");
        if ((image==1 && !overviewImagesSwapped())
          ||(image==3 && overviewImagesSwapped())) return tr("top standard deviation overview image (time average)");
        if ((image==2 && !overviewImagesSwapped())
          ||(image==0 && overviewImagesSwapped())) return tr("bottom overview image (time average)");
        if ((image==3 && !overviewImagesSwapped())
          ||(image==1 && overviewImagesSwapped())) return tr("bottom standard deviation overview image (time average)");
        if (image-3<=ovrImages.size()) return ovrImages[image-4].name;
        return QString("");

    }
    if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if ((image==0 && !overviewImagesSwapped())
          ||(image==2 && overviewImagesSwapped())) return tr("left overview image (time average)");
        if ((image==1 && !overviewImagesSwapped())
          ||(image==3 && overviewImagesSwapped())) return tr("left standard deviation overview image (time average)");
        if ((image==2 && !overviewImagesSwapped())
          ||(image==0 && overviewImagesSwapped())) return tr("right overview image (time average)");
        if ((image==3 && !overviewImagesSwapped())
          ||(image==1 && overviewImagesSwapped())) return tr("right standard deviation overview image (time average)");
        /*if (image==0) return tr("left overview image (time average)");
        if (image==1) return tr("left standard deviation overview image (time average)");
        if (image==2) return tr("right overview image (time average)");
        if (image==3) return tr("right standard deviation overview image (time average)");*/
        if (image-3<=ovrImages.size()) return ovrImages[image-4].name;
        return QString("");

    }
    if (image==0) return tr("overview image (time average)");
    if (image==1) return tr("standard deviation overview image (time average)");
    if (image-1<=ovrImages.size()) return ovrImages[image-2].name;
    return QString("");
}

QString QFRDRImagingFCSData::getOverviewImageID(int image) const {
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if (image>3 && image-3<=ovrImages.size()) return ovrImages[image-4].id;
        return QString("");
    }
    if (image>1 && image-1<=ovrImages.size()) return ovrImages[image-2].id;
    return QString("");
}

double *QFRDRImagingFCSData::getOverviewImage(int image) const {
    if (image==0) return overviewF;
    if (image==1) return overviewFSTD;
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if (image==2) return overviewF2;
        if (image==3) return overviewF2STD;
        if (image-3<=ovrImages.size()) return ovrImages[image-4].image;
        return NULL;
    }
    if (image-1<=ovrImages.size()) return ovrImages[image-2].image;
    return NULL;
}

QList<QFRDROverviewImageInterface::OverviewImageGeoElement> QFRDRImagingFCSData::getOverviewImageAnnotations(int image) const {
    QList<QFRDROverviewImageInterface::OverviewImageGeoElement> result;
    if (internalDualViewMode()!=QFRDRImagingFCSData::dvNone && overviewF2 && (propertyExists("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL") || isFCCS())) {
        if (image>3 && image-4<ovrImages.size()) return ovrImages[image-4].geoElements;
    } else {
        if (image>1 && image-2<ovrImages.size()) return ovrImages[image-2].geoElements;
    }
    return result;
}

int QFRDRImagingFCSData::getImageStackCount() const {
    return 2;
}

uint32_t QFRDRImagingFCSData::getImageStackFrames(int stack) const {
    if (stack==0) return video_frames;
    if (stack==1) return videoUncorrected_frames;
    return 0;
}

int QFRDRImagingFCSData::getImageStackWidth(int stack) const {
    if (stack==0) return video_width;
    if (stack==1) return videoUncorrected_width;
    return 0;
}

int QFRDRImagingFCSData::getImageStackHeight(int stack) const {
    if (stack==0) return video_height;
    if (stack==1) return videoUncorrected_height;
    return 0;
}

int QFRDRImagingFCSData::getImageStackChannels(int stack) const {
    if (isFCCS()) {
        if (stack==0) return 2;
        if (stack==1) return 2;
    } else {
        if (stack==0) return 1;
        if (stack==1) return 1;
    }
    return 0;
}

double *QFRDRImagingFCSData::getImageStack(int stack, uint32_t frame, uint32_t channel) const {
    if (isFCCS()) {
        if (stack==0) {
            if (channel==0) return &(video[frame*video_width*video_height]);
            if (channel==1) return &(video2[frame*video_width*video_height]);
        }
        if (stack==1) {
            if (channel==0) return &(videoUncorrected[frame*videoUncorrected_width*videoUncorrected_height]);
            if (channel==1) return &(videoUncorrected2[frame*videoUncorrected_width*videoUncorrected_height]);
        }
    } else {
        if (stack==0) return &(video[frame*video_width*video_height]);
        if (stack==1) return &(videoUncorrected[frame*videoUncorrected_width*videoUncorrected_height]);
    }
    return NULL;
}

double QFRDRImagingFCSData::getImageStackXUnitFactor(int stack) const {
    return 1;//getProperty("PIXEL_WIDTH", 1.0).toDouble();
}

QString QFRDRImagingFCSData::getImageStackXUnitName(int stack) const {
    return tr("pixel"); //QString("micrometer");
}

QString QFRDRImagingFCSData::getImageStackXName(int stack) const {
    return tr("x");
}

double QFRDRImagingFCSData::getImageStackYUnitFactor(int stack) const {
    return 1; //getProperty("PIXEL_HEIGHT", 1.0).toDouble();
}

QString QFRDRImagingFCSData::getImageStackYUnitName(int stack) const {
    return tr("pixel"); //QString("micrometer");
}

QString QFRDRImagingFCSData::getImageStackYName(int stack) const {
    return QString("y");
}

double QFRDRImagingFCSData::getImageStackTUnitFactor(int stack) const {
    if (stack==0) {
        if (video_frames<=0) return 1;
        if (propertyExists("VIDEO_FRAMETIME")) {
            return getProperty("VIDEO_FRAMETIME", 1.0).toDouble();
        }
        return getMeasurementDuration()/double(video_frames);
    } else if (stack==1) {
        if (videoUncorrected_frames<=0) return 1;
        if (propertyExists("VIDEO_FRAMETIME")) {
            return getProperty("VIDEO_FRAMETIME", 1.0).toDouble();
        }
        return getMeasurementDuration()/double(videoUncorrected_frames);
    }
    return 1;
}

QString QFRDRImagingFCSData::getImageStackTUnitName(int stack) const {
    if (stack==0) return tr("seconds");
    if (stack==1) return tr("seconds");
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackTName(int stack) const {
    if (stack==0) return tr("time");
    if (stack==1) return tr("time");
    return QString("");

}

double QFRDRImagingFCSData::getImageStackCUnitFactor(int stack) const {
    return 1;
}

QString QFRDRImagingFCSData::getImageStackCUnitName(int stack) const {
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackCName(int stack) const
{
    return tr("channel");
}

QString QFRDRImagingFCSData::getImageStackDescription(int stack) const {
    if (stack==0) return tr("time-binned video (%1 binned frames)").arg(getProperty("VIDEO_AVGFRAMES", "?").toString());
    if (stack==1) return tr("uncorrected time-binned video (%1 binned frames)").arg(getProperty("VIDEO_AVGFRAMES", "?").toString());
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackChannelName(int stack, int channel) const {
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackTimepointName(int stack, int t) const {
    return QString("%1%2").arg(getImageStackTUnitFactor(stack)*t).arg(getImageStackTUnitName(stack));
}

double QFRDRImagingFCSData::getSimpleCountrateAverage(int run, int channel, bool swapChannels) const {
    int ch=channel;
    if (overviewImagesSwapped()&&!swapChannels) {
        if (ch==1) ch=0;
        else if (ch==0) ch=1;
    }
    //qDebug()<<"getSimpleCountrateAverage("<<getRole()<<" r="<<run<<" c="<<channel<<" c_read="<<ch<<" swap="<<swapChannels<<" ovrSwapped="<<overviewImagesSwapped()<<")   overviewF="<<overviewF<<" overviewF2="<<overviewF2<<" scaled="<<getProperty("IS_OVERVIEW_SCALED", true).toBool();
    if (run>=0&&ch==0) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF) {
            if (run>=0) {
                //qDebug()<<"       F => "<<overviewF[run]/getTauMin()/1000.0<<"   (tauMin="<<getTauMin()<<")";
                return overviewF[run]/getTauMin()/1000.0;
            }
        } else if (overviewF) {
            return overviewF[run];
        }
    }
    if (run<0&&channel==0) {
        if (swapChannels && overviewImagesSwapped()) {
            if (run==-2) return  backStat2.avgCnt/getTauMin()/1000.0;
            if (hasStatistics2) return stat2.avgCnt/getTauMin()/1000.0;
        } else {
            if (run==-2) return  backStat.avgCnt/getTauMin()/1000.0;
            if (hasStatistics) return stat.avgCnt/getTauMin()/1000.0;
        }
    }
    /*if (isDCCF()) {
        int dx=getDCCFDeltaX();
        int dy=getDCCFDeltaY();
        int ry=run/widthOvr;
        int rx=run%widthOvr;
        int rund=(ry+dy)*widthOvr+(rx+dx);
        if (dx>0 || dy>0) {
            if (rund>=0 && rund<widthOvr*heightOvr && ch==1) {
                if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF2) {
                    if (rund>=0) {
                        return overviewF2[rund]/getTauMin()/1000.0;
                    } else if (overviewF2) {
                        return overviewF2[rund];
                    }
                }
            }
        }
    }*/

    if (run>=0&&ch==1) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF2) {
            if (run>=0) {
                return overviewF2[run]/getTauMin()/1000.0;
            } else if (overviewF2) {
                return overviewF2[run];
            }
        }
    }

    if (run<0&&channel==1) {
        if (swapChannels && overviewImagesSwapped()) {
            if (run==-2) return  backStat.avgCnt/getTauMin()/1000.0;
            if (hasStatistics) return stat.avgCnt/getTauMin()/1000.0;
        } else {
            if (run==-2) return  backStat2.avgCnt/getTauMin()/1000.0;
            if (hasStatistics2) return stat2.avgCnt/getTauMin()/1000.0;
        }
    }
    return 0;
}

double QFRDRImagingFCSData::getSimpleCountrateStdDev(int run, int channel, bool swapChannels) const {
    int ch=channel;
    if (overviewImagesSwapped()&&!swapChannels) {
        if (ch==1) ch=0;
        else if (ch==0) ch=1;
    }
/*    if (ch==0) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewFSTD) {
            if (run>=0 && run<width*height) return overviewFSTD[run]/getTauMin()/1000.0;
        }
        if (run==-2) return sqrt(backStat.sigmaCnt)/getTauMin()/1000.0;
        if (hasStatistics) return sqrt(stat.sigmaCnt)/getTauMin()/1000.0;
    } else if (ch==1) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF2STD) {
            if (run>=0 && run<width*height) return overviewF2STD[run]/getTauMin()/1000.0;
        }
        if (run==-2) return sqrt(backStat2.sigmaCnt)/getTauMin()/1000.0;
        if (hasStatistics2) return sqrt(stat2.sigmaCnt)/getTauMin()/1000.0;
    }*/

    if (run>=0&&ch==0) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewFSTD) {
            if (run>=0) return overviewFSTD[run]/getTauMin()/1000.0;
        } else if (overviewFSTD) {
            return overviewFSTD[run];
        }
    }
    if (run<0&&channel==0) {
        if (swapChannels && overviewImagesSwapped()) {
            if (run==-2) return  backStat2.sigmaCnt/getTauMin()/1000.0;
            if (hasStatistics2) return stat2.sigmaCnt/getTauMin()/1000.0;
        } else {
            if (run==-2) return  backStat.sigmaCnt/getTauMin()/1000.0;
            if (hasStatistics) return stat.sigmaCnt/getTauMin()/1000.0;
        }
    }

    /*if (isDCCF()) {
        int dx=getDCCFDeltaX();
        int dy=getDCCFDeltaY();
        int ry=run/widthOvr;
        int rx=run%widthOvr;
        int rund=(ry+dy)*widthOvr+(rx+dx);
        if (dx>0 || dy>0) {
            if (rund>=0 && rund<widthOvr*heightOvr && ch==1) {
                if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF2STD) {
                    if (rund>=0) {
                        return overviewF2STD[rund]/getTauMin()/1000.0;
                    } else if (overviewF2STD) {
                        return overviewF2STD[rund];
                    }
                }
            }
        }
    }*/

    if (run>=0&&ch==1) {
        if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF2STD) {
            if (run>=0) return overviewF2STD[run]/getTauMin()/1000.0;
        } else if (overviewF2STD) {
            return overviewF2STD[run];
        }
        //if (run==-2) return  backStat2.avgCnt/getTauMin()/1000.0;
        //if (hasStatistics2) return stat2.avgCnt/getTauMin()/1000.0;
    }
    if (run<0&&channel==1) {
        if (swapChannels && overviewImagesSwapped()) {
            if (run==-2) return  backStat.sigmaCnt/getTauMin()/1000.0;
            if (hasStatistics) return stat.sigmaCnt/getTauMin()/1000.0;
        } else {
            if (run==-2) return  backStat2.sigmaCnt/getTauMin()/1000.0;
            if (hasStatistics2) return stat2.sigmaCnt/getTauMin()/1000.0;
        }
    }

    return 0;
}

int QFRDRImagingFCSData::getSimpleCountrateChannels() const
{
    return getImageFromRunsChannels();
}

















int QFRDRImagingFCSData::getImageSelectionCount() const
{
    return selections.size();
}

QString QFRDRImagingFCSData::getImageSelectionName(int selection) const
{
    return selections.value(selection, QFRDRImagingFCSData::ImageSelection()).name;
}

bool *QFRDRImagingFCSData::loadImageSelection(int selection) const
{
    return selections.value(selection, QFRDRImagingFCSData::ImageSelection()).selection;
}

void QFRDRImagingFCSData::addImageSelection(bool *selection, const QString &name)
{
    QFRDRImagingFCSData::ImageSelection s;
    s.name=name;
    s.selection=(bool*)qfCalloc(getImageSelectionHeight()*getImageSelectionWidth(), sizeof(bool));
    memcpy(s.selection, selection, getImageSelectionHeight()*getImageSelectionWidth()*sizeof(bool));
    selections.append(s);
}

void QFRDRImagingFCSData::deleteImageSelection(int selection)
{
    if (selection>=0 && selection<selections.size()) {
        selections.removeAt(selection);
    }
}

void QFRDRImagingFCSData::setImageSelectionName(int selection, const QString &name)
{
    if (selection>=0 && selection<selections.size()) {
         QFRDRImagingFCSData::ImageSelection s=selections[selection];
         s.name=name;
         selections[selection]=s;
    }
}

int QFRDRImagingFCSData::getImageSelectionHeight() const
{
    return getImageFromRunsHeight();
}

QStringList QFRDRImagingFCSData::getAvailableRoles() const {
    QStringList sl;
    /*sl<<"correlation";
    sl<<"acf";
    sl<<"ccf";
    sl<<"dccf";
    sl<<"fcs_red";
    sl<<"fcs_green";
    sl<<"fcs0";
    sl<<"fcs1";
    sl<<"fccs";
    sl<<"ccf(-1,0)";
    sl<<"ccf(1,0)";
    sl<<"ccf(0,-1)";
    sl<<"ccf(0,1)";*/
    return sl;
}

bool QFRDRImagingFCSData::isRoleUserEditable() const
{
    return false;
}

QFRDRImagingFCSData *QFRDRImagingFCSData::getRoleFromThisGroup(const QString &role)
{
    QList<QFRDRImagingFCSData*> items=filterListForClass<QFRawDataRecord, QFRDRImagingFCSData>(getRecordsWithRoleFromGroup(role));
    if (items.size()>0) return items.first();
    return NULL;
}

bool QFRDRImagingFCSData::doCopyFileForExport(const QString &filename, const QString &fileType, QString &newFilename, const QList<QFProject::FileCopyList> *filecopylist, const QString &subfoldername) const
{
    if (fileType.toLower()=="input") return false;
    return true;
}


template<class T>
void QFRDRImagingFCSData_bincf(T* data, T* sigma, int width, int height, int N, int binning) {
    if (width<=0 || height<=0 || N<=0 || !data) return;
    if (binning<=1) return;
    T* temp=duplicateArray(data, width*height*N);
    const int64_t nw=width/binning;
    const int64_t nh=height/binning;
    for (int64_t i=0; i<width*height*N; i++) {
        data[i]=0;
        sigma[i]=0;
    }
    //working implementation
    for (int64_t f=0; f<N; f++) {
        for (int64_t y=0; y<height; y++) {
            const int64_t ny=y/binning;
            for (int64_t x=0; x<width; x++) {
                const int64_t nx=x/binning;
                data[ny*nw*N+nx*N+f]=data[ny*nw*N+nx*N+f]+temp[y*width*N+x*N+f]*(1.0/(binning*binning));
                sigma[ny*nw*N+nx*N+f]=sigma[ny*nw*N+nx*N+f]+qfSqr(temp[y*width*N+x*N+f]);
            }
        }
    }


    double norm=binning*binning;
    for (int64_t i=0; i<width*height*N; i++) {
        sigma[i]=sqrt((sigma[i]-data[i]*data[i]/norm)/(norm-1.0));
        data[i]= data[i]/norm;
    }
    qfFree(temp);
}

template<class T>
void QFRDRImagingFCSData_avgcf(T* data, T* sigma, int width, int height, int N, int binning) {
    if (width<=0 || height<=0 || N<=0 || !data) return;
    if (binning<=1) return;
    T* temp=duplicateArray(data, width*height*N);
    for (int64_t i=0; i<width*height*N; i++) {
        data[i]=0;
        sigma[i]=0;
    }
    //working implementation
    for (int64_t n=0; n<N; n++) {
        for (int64_t y=0; y<height; y++) {
            for(int64_t x=0; x<width; x++) {
                unsigned int count=0;
                for(int64_t y1=0; y1<binning; y1++){
                    for(int64_t x1=0; x1<binning; x1++){
                        const int64_t x2= x+x1-((binning-1)/2);
                        const int64_t y2= y+y1-((binning-1)/2);
                        if((x2>=0)&&(x2<width)&&(y2>=0)&&(y2<height)){
                             data[y*width*N+x*N+n]= data[y*width*N+x*N+n]+      temp[y2*width*N+x2*N+n];
                            sigma[y*width*N+x*N+n]=sigma[y*width*N+x*N+n]+qfSqr(temp[y2*width*N+x2*N+n]);
                            count++;
                        }
                    }
                }
                const double norm=count;
                sigma[y*width*N+x*N+n]=sqrt((sigma[y*width*N+x*N+n]-data[y*width*N+x*N+n]*data[y*width*N+x*N+n]/norm)/(norm-1.0));
                data[y*width*N+x*N+n]/=norm;

            }
        }
    }


    /*double norm=binning*binning;
    for (int64_t i=0; i<width*height*N; i++) {
        sigma[i]=sqrt((sigma[i]-data[i]*data[i]*norm)/(norm-1.0));
        //data[i]= data[i]/norm;
    }*/
    qfFree(temp);
}



void QFRDRImagingFCSData::loadPostProcess()
{
    double bin=getProperty("POSTPROCESS_BINNING", 1).toInt();
    bool interleave=getProperty("POSTPROCESS_BINNING_INTERLEAVED", false).toBool();

    if (!interleave && propertyExists("POSTPROCESS_BINNING_AVG")) {
        bin=getProperty("POSTPROCESS_BINNING_AVG", 1).toInt();
        interleave=true;
    }

    if (bin>1 && correlations && sigmas && width>0 && height>0 && N>0) {
        if (interleave) {
            log_text(tr("calculating %2x binning with avaraging imFCS record '%1' ...\n").arg(getName()).arg(bin));
            QFRDRImagingFCSData_avgcf(correlations, sigmas, width, height, N, bin);
            log_text(tr("calculating %2x binning with avaraging imFCS record '%1' ... DONE!\n").arg(getName()).arg(bin));
            recalcCorrelations();

        } else {
            log_text(tr("calculating %2x binning imFCS record '%1' ...\n").arg(getName()).arg(bin));
            if (overviewF) qfVideoBinInFrame(overviewF, widthOvr, heightOvr, 1, bin);
            if (overviewF2) qfVideoBinInFrame(overviewF2, widthOvr, heightOvr, 1, bin);
            if (overviewFSTD) qfVideoBinInFrame(overviewFSTD, widthOvr, heightOvr, 1, bin);
            if (overviewF2STD) qfVideoBinInFrame(overviewF2STD, widthOvr, heightOvr, 1, bin);
            widthOvr=widthOvr/bin;
            heightOvr=heightOvr/bin;

            if (video) qfVideoBinInFrame(video, video_width, video_height, video_frames, bin);
            if (video2) qfVideoBinInFrame(video2, video_width, video_height, video_frames, bin);
            video_width=video_width/bin;
            video_height=video_height/bin;
            if (videoUncorrected) qfVideoBinInFrame(videoUncorrected, videoUncorrected_width, videoUncorrected_height, videoUncorrected_frames, bin);
            if (videoUncorrected2) qfVideoBinInFrame(videoUncorrected2, videoUncorrected_width, videoUncorrected_height, videoUncorrected_frames, bin);
            videoUncorrected_width=videoUncorrected_width/bin;
            videoUncorrected_height=videoUncorrected_height/bin;

            QFRDRImagingFCSData_bincf(correlations, sigmas, width, height, N, bin);
            width=width/bin;
            height=height/bin;
            maskInit(width, height);
            log_text(tr("calculating %2x binning imFCS record '%1' ... DONE!\n").arg(getName()).arg(bin));
            recalcCorrelations();
        }
    }
}

int QFRDRImagingFCSData::internalDualViewModeChannel() const
{
    return getProperty("INTERNAL_DUALVIEW_MODE_CHANNEL", 0).toInt();
}
QFRDRImagingFCSData::DualViewMode QFRDRImagingFCSData::internalDualViewMode() const
{
    QString dv=getProperty("INTERNAL_DUALVIEW_MODE", "none").toString().toLower();


    QFRDRImagingFCSData::DualViewMode m_dualview=QFRDRImagingFCSData::dvNone;
    if (dv=="none" || dv=="0" || dv=="n") m_dualview=QFRDRImagingFCSData::dvNone;
    if (dv=="horizontal" || dv=="1" || dv=="h" || dv=="horicontal") m_dualview=QFRDRImagingFCSData::dvHorizontal;
    if (dv=="vertical" || dv=="2" || dv=="v" || dv=="vertikal") m_dualview=QFRDRImagingFCSData::dvVertical;

    return m_dualview;
}

void QFRDRImagingFCSData::setInternalDualViewMode(QFRDRImagingFCSData::DualViewMode mode)
{
    QFRDRImagingFCSData::DualViewMode m_dualview=mode;
    if (m_dualview==QFRDRImagingFCSData::dvNone) setQFProperty("INTERNAL_DUALVIEW_MODE", "none", false);
    if (m_dualview==QFRDRImagingFCSData::dvHorizontal) setQFProperty("INTERNAL_DUALVIEW_MODE", "horicontal", false);
    if (m_dualview==QFRDRImagingFCSData::dvVertical) setQFProperty("INTERNAL_DUALVIEW_MODE", "vertical", false);
}


int QFRDRImagingFCSData::getImageSelectionWidth() const
{
    return getImageFromRunsWidth();
}

void QFRDRImagingFCSData::clearSelections()
{
    for (int i=0; i<selections.size(); i++) {
        if (selections[i].selection) delete selections[i].selection;
    }
    selections.clear();
}


void QFRDRImagingFCSData::splitCFsForDualView() {
    if (!correlations || !tau) return;
    double* oldC=duplicateArray(correlations, width*height*N);
    double* oldS=duplicateArray(sigmas, width*height*N);
    bool* oldL=duplicateArray(maskGet(), width*height);
    double* oldTau=duplicateArray(tau, N);
    //qDebug()<<oldC<<correlations;
    uint32_t oldWidth=width;
    uint32_t oldHeight=height;
    if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal) {
        allocateContents(width/2, height, N);
        int shift=0;
        if (internalDualViewModeChannel()==1) { shift=width; }
        //qDebug()<<"### splitCFsForDualView:    width="<<width<<"  height="<<height<<"  shiftX="<<shift;
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                const int idxIn=(y*oldWidth+(x+shift))*N;
                const int idxOut=(y*width+x)*N;
                memcpy(&(correlations[idxOut]), &(oldC[idxIn]), N*sizeof(double));
                memcpy(&(sigmas[idxOut]), &(oldS[idxIn]), N*sizeof(double));
                //qDebug()<<"("<<x<<","<<y<<"):   idxIn="<<idxIn<<"  =>  idxOut="<<idxOut<<"   N="<<N<<"   out="<<correlations[idxOut]<<"  <- "<<oldC[idxIn];
                maskSet(x, y, oldL[y*oldWidth+x+shift]);
            }
        }
    } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical) {
        allocateContents(width, height/2, N);
        int shift=0;
        if (internalDualViewModeChannel()==1) { shift=height; }
        //qDebug()<<"### splitCFsForDualView:    width="<<width<<"  height="<<height<<"  shiftY="<<shift;
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                const int idxIn=(y+shift)*oldWidth*N + x*N;
                const int idxOut=y*width*N+x*N;
                memcpy(&(correlations[idxOut]), &(oldC[idxIn]), N*sizeof(double));
                memcpy(&(sigmas[idxOut]), &(oldS[idxIn]), N*sizeof(double));
                maskSet(x, y, oldL[y*oldWidth+x+shift]);
            }
        }
    }
    memcpy(tau, oldTau, N*sizeof(double));
    setQFProperty("WIDTH", width, false, true);
    setQFProperty("HEIGHT", height, false, true);
    qfFree(oldC);
    qfFree(oldS);
    qfFree(oldL);
    qfFree(oldTau);
    recalcCorrelations();
}

void QFRDRImagingFCSData::splitVideosForDualView()
{
   splitVideo(video, video2, video_width, video_height, video_frames);
   splitVideo(videoUncorrected, videoUncorrected2, videoUncorrected_width, videoUncorrected_height, videoUncorrected_frames);
}

void QFRDRImagingFCSData::splitVideo(double *video, double *&video2, int &width, int &height, uint32_t frames)
{
    int oldWidth=width;
    int oldHeight=height;
    double* temp=duplicateArray(video, width*height*frames);
    if (temp) {
        int shiftX1=0;
        int shiftX2=0;
        int shiftY1=0;
        int shiftY2=0;
        if (internalDualViewMode()==QFRDRImagingFCSData::dvHorizontal) {
            width=width/2;
            if (internalDualViewModeChannel()==0 || isFCCS()) {
                shiftX1=0;
                shiftX2=width;
            }
            if (internalDualViewModeChannel()==1 && !isFCCS()) {
                shiftX1=width;
                shiftX2=0;
            }
        } else if (internalDualViewMode()==QFRDRImagingFCSData::dvVertical) {
            height=height/2;
            if (internalDualViewModeChannel()==0 || isFCCS()) {
                shiftY1=0;
                shiftY2=height;
            }
            if (internalDualViewModeChannel()==1 && !isFCCS()) {
                shiftY1=height;
                shiftY2=0;
            }
        }
        video2=video+width*height*frames;

        for (uint32_t f=0; f<frames; f++) {
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    const int idx=f*width*height+y*width+x;
                    const int idx1=f*oldWidth*oldHeight+(y+shiftY1)*oldWidth+x+shiftX1;
                    const int idx2=f*oldWidth*oldHeight+(y+shiftY2)*oldWidth+x+shiftX2;
                    video[idx]=temp[idx1];
                    video2[idx]=temp[idx2];
                }
            }
        }

        qfFree(temp);
    } else {
        log_warning(tr("WARNING could not split image in record '%1' (out of memory).\n").arg(getName()));
    }
}

void QFRDRImagingFCSData::maskMaskChangedEvent()
{
    recalcCorrelations();
}




