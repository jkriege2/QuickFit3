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

#undef DEBUG_SIZES
//#define DEBUG_SIZES
#undef DEBUG_TIMING
//#define DEBUG_TIMING

QFRDRImagingFCSData::QFRDRImagingFCSData(QFProject* parent):
    QFRawDataRecord(parent)
{
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    N=0;
    width=0;
    height=0;
    overviewF=NULL;
    overviewFSTD=NULL;
    leaveout=NULL;
    statAvg=NULL;
    statStdDev=NULL;
    statMin=NULL;
    statMax=NULL;
    statT=NULL;
    statN=0;
    video=NULL;
    video_width=0;
    video_height=0;
    video_frames=0;
    hasStatistics=false;
    statAvgCnt=0;
    statSigmaCnt=0;
    setResultsInitSize(1000);
    setEvaluationIDMetadataInitSize(1000);
}

QFRDRImagingFCSData::~QFRDRImagingFCSData() {
     allocateContents(0,0,0);
     allocateStatistics(0);
     clearOvrImages();
     if (video) free(video);
     video=NULL;
     video_width=video_height=video_frames=0;
}

QString QFRDRImagingFCSData::getEditorName(int i) {
    if (i==0) return tr("Parameter Image");
    if (i==1) return tr("Correlation Curves");
    if (i==2) return tr("Images, Videos & Timetrace");
    return QString("");
};

QFRawDataEditor* QFRDRImagingFCSData::createEditor(QFPluginServices* services,  QFRawDataPropertyEditor *propEditor, int i, QWidget* parent) {
    if (i==0) return new QFRDRImagingFCSImageEditor(services, propEditor, parent);
    if (i==1) return new QFRDRImagingFCSDataEditor(services, propEditor, parent);
    if (i==2) return new QFRDRImagingFCSOverviewRateEditor(services, propEditor, parent);
    return NULL;
};


QStringList QFRDRImagingFCSData::getExportFiletypes() {
    QStringList sl;
    return sl;
};

QString QFRDRImagingFCSData::getExportDialogTitle() {
    return tr("");
}

QString QFRDRImagingFCSData::getExportDialogFiletypes() {
    return tr("");
}

void QFRDRImagingFCSData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImagingFCSData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*if (leaveout.size()>0) {
        QString l="";
        for (int i=0; i<leaveout.size(); i++) {
            if (!l.isEmpty()) l=l+",";
            l=l+QString::number(leaveout[i]);
        }
        w.writeStartElement("leaveout");
        w.writeAttribute("list", l);
        w.writeEndElement();
    }*/
    if (leaveout) {
        QString l="";
        for (int i=0; i<width*height; i++) {
            if (leaveout[i]!=0) {
                if (!l.isEmpty()) l=l+",";
                l=l+QString::number(i);
            }
        }
        if (l.size()>0) {
            w.writeStartElement("leaveout");
            w.writeAttribute("list", l);
            w.writeEndElement();
        }
    }
}

void QFRDRImagingFCSData::intReadData(QDomElement* e) {
    leaveoutClear();
	// read data from the project XML file



    width=getProperty("WIDTH", 0).toInt();
    height=getProperty("HEIGHT", 0).toInt();
    QString filetype=getProperty("FILETYPE", "unknown").toString();

    // now also load the data file(s) this record is linked to
    // an error is reported when no file is given!
    if (files.size()<=0) {
        setError(tr("there are no files in the %1 record!").arg(getName()));
        return;
    }
    bool dataLoaded=false;
    QString acquisitionSettingsFile=getFileForType("acquisition_settings");
    if (files.size()>0) {
        // now we check whether the experiment config file contains additional files, that may be useful for this object and load them
        // (more overview images ...)
        //qDebug()<<"opening qcquisition settings "<<acquisitionSettingsFile;
        if (QFile::exists(acquisitionSettingsFile)) {
            QSettings settings(acquisitionSettingsFile, QSettings::IniFormat);
            //qDebug()<<"  ... success!!!";
            loadQFPropertiesFromB040SPIMSettingsFile(settings);

            QStringList lfiles, lfiles_types, lfiles_descriptions;
            appendCategorizedFilesFromB040SPIMConfig(settings, lfiles, lfiles_types, lfiles_descriptions);
            //qDebug()<<lfiles<<"\n"<<lfiles_types;
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
                    files_types<<lfiles_types[i];
                }


            }
        }

        // finally we load all known and useful associated files into memory:
        hasStatistics=false;
        for (int i=0; i<files.size(); i++) {
            if (i<files_types.size()) {
                QString ft=files_types[i].toLower().trimmed();
                if (ft=="overview") {
                    loadOverview(files[i]);
                } else if (ft=="overview_std") {
                    loadOverviewSTD(files[i]);
                } else if (ft=="statistics") {
                    loadStatistics(files[i]);
                    hasStatistics=true;
                    statAvgCnt=statisticsAverageVariance(statSigmaCnt, statAvg, statN);
                } else if (ft=="video") {
                    loadVideo(files[i], &video, &video_width, &video_height, &video_frames);
                } else if (ft=="background") {
                    QFRDRImagingFCSData::ovrImageData img;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("background frame");
                    ovrImages.append(img);
                } else if (ft=="background_stddev") {
                    QFRDRImagingFCSData::ovrImageData img;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("background standard deviation frame");
                    ovrImages.append(img);
                } else if (ft=="display_image") {
                    QFRDRImagingFCSData::ovrImageData img;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=files_desciptions.value(i, tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName()));
                    if (img.name.isEmpty()) img.name=tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName());
                    ovrImages.append(img);
                } else if (ft=="overview_before") {
                    QFRDRImagingFCSData::ovrImageData img;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("overview before acquisition");
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
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=tr("overview after acquisition");
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
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=files_desciptions.value(i, ft);
                    ovrImages.append(img);

                } else if (ft=="acf" || ft=="ccf" || ft=="dccf") {
                    if (!dataLoaded) {
                        if (filetype.toUpper()=="VIDEO_CORRELATOR_BIN") {
                            loadVideoCorrelatorFileBin(files[i]);
                            //qDebug()<<i<<ft<<"loading binary Video correlator file '"<<files[i]<<"'\n";
                            dataLoaded=true;
                        } else if (filetype.toUpper()=="VIDEO_CORRELATOR") {
                            loadVideoCorrelatorFile(files[i]);
                            //qDebug()<<i<<ft<<"loading Video correlator file '"<<files[i]<<"'\n";
                            dataLoaded=true;
                        } else if (filetype.toUpper()=="RADHARD2") {
                            loadRadhard2File(files[i]);
                            width=getProperty("WIDTH", 0).toInt();
                            height=getProperty("HEIGHT", 0).toInt();
                            //qDebug()<<i<<ft<<"loading Radhard2 file '"<<files[i]<<"'\n";
                            dataLoaded=true;
                        } else {
                          setError(tr("filetype '%1' is unknown for Imaging FCS data files (file is '%2')").arg(filetype).arg(files[i]));
                          break;
                        }
                    } else {
                        //qDebug()<<i<<ft<<tr("correlation data has already been loaded for Imaging FCS (filetype: %2, this data file is '%1')").arg(files[i]).arg(ft);
                        setError(tr("correlation data has already been loaded for Imaging FCS (filetype: %2, this data file is '%1')").arg(files[i]).arg(ft));
                        break;
                    }
                }
            }
        }


    }

    if (e) {
        QDomElement te=e->firstChildElement("leaveout");
        QString l=te.attribute("list");
        QStringList li=l.split(",");
        //qDebug()<<li;
        for (int i=0; i<li.size(); i++) {
            bool ok=false;
            int lo=li[i].toUInt(&ok);
            if (ok) leaveoutAddRun(lo);
            //qDebug()<<lo<<ok;
        }
        recalcCorrelations();
    }

    if (!dataLoaded) {
        setError(tr("did not find a correlation data file (acf, ccf, dccf, ...) for record"));
    }
}

bool QFRDRImagingFCSData::loadOverview(const QString& filename) {
    bool ok=false;

    if (!overviewF) return false;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            if (nx!=width || ny!=height) {
                ok=false;
            } else {
                ok=TIFFReadFrame<double>(tif, overviewF);
            }
            TIFFClose(tif);
        }
    }

    if (!ok && overviewF) {
        for (int i=0; i<width*height; i++) {
            overviewF[i]=0;
        }
    }
    return ok;
}

bool QFRDRImagingFCSData::loadOverviewSTD(const QString& filename) {
    bool ok=false;
    //qDebug()<<"load: overviewFSTD="<<overviewFSTD<<filename;

    if (!overviewFSTD) return false;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            if (nx!=width || ny!=height) {
                ok=false;
            } else {
                ok=TIFFReadFrame<double>(tif, overviewFSTD);
            }
            TIFFClose(tif);
        }
    }

    if (!ok && overviewFSTD) {
        for (int i=0; i<width*height; i++) {
            overviewFSTD[i]=0;
        }
    }
    return ok;
}

bool QFRDRImagingFCSData::loadImage(const QString& filename, double** data, int* width, int* height) {
    bool ok=false;

    if (*data) free(*data);
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
            *data=(double*)malloc(nx*ny*sizeof(double));
            ok=TIFFReadFrame<double>(tif, *data);
            TIFFClose(tif);
        }
    }
    return ok;
}

bool QFRDRImagingFCSData::loadVideo(const QString& filename, double** data, int* width, int* height, uint32_t* frames) {
    bool ok=false;

    if (*data) free(*data);
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
            *width=nx;
            *height=ny;
            *data=(double*)malloc(nx*ny*(*frames)*sizeof(double));
            uint32_t i=0;
            do {
                ok=ok & TIFFReadFrame<double>(tif, &((*data)[i*nx*ny]));
                i++;
            } while (TIFFReadDirectory(tif) && i<=(*frames));
            TIFFClose(tif);
        }
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
        //qDebug()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif
        //QTextStream stream(&file);
        QTextStream stream(&dataFromFile);
#ifdef DEBUG_TIMING
        //qDebug()<<"read file contents into memory buffer     "<<time.elapsed()<<"ms"; time.start();
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
                    //qDebug()<<"  tau="<<data[0]<<"   c="<<data[1];
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
                //qDebug()<<"runs="<<runs<<"     NN="<<NN<<"     current_set.size()="<<current_set.size()<<"     data_matrix.size()="<<data_matrix.size()<<"    all0="<<all0<<"    all1="<<all1;
                current_set.clear();
                runs++;
                if (runs%50==0) QApplication::processEvents();
            }
            //if (stream.atEnd()) //qDebug()<<"runs="<<runs<<"     NN="<<NN<<"     width*height="<<width*height<<"     stream.atEnd()="<<stream.atEnd()<<"    data="<<data;

        }
#ifdef DEBUG_TIMING
        //qDebug()<<"parsed CSV     "<<time.elapsed()<<"ms"; time.start();
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
        //qDebug()<<"read into internal arrays     "<<time.elapsed()<<"ms"; time.start();
#endif

	if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}











bool QFRDRImagingFCSData::loadVideoCorrelatorFileBin(const QString &filename) {
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
    if (!file.open(QIODevice::ReadOnly)) {
        ok=false;
    } else {
        uint32_t corr_set=getProperty("CORRELATION_SET", 0).toUInt();
        double taufactor=getProperty("TAU_FACTOR", 1).toDouble();
        double corroffset=getProperty("CORR_OFFSET", 0).toDouble();
#ifdef DEBUG_TIMING
        //qDebug()<<"opened file     "<<time.elapsed()<<"ms"; time.start();
#endif
        QByteArray file_id=file.read(10);
        if (file_id=="QF3.0imFCS") {
            uint32_t fwidth=binfileReadUint32(file);
            uint32_t fheight=binfileReadUint32(file);
            uint32_t fcorrN=binfileReadUint32(file);
            uint32_t fN=binfileReadUint32(file);
            uint32_t fsets=binfileReadUint32(file);

            setQFProperty("WIDTH", fwidth, false, true);
            setQFProperty("HEIGHT", fwidth, false, true);
            allocateContents(fwidth, fheight, fN);

            binfileReadDoubleArray(file, tau, N);
            if (taufactor!=1) for (int i=0; i<N; i++) { tau[i]=tau[i]*taufactor; }
            if (corr_set==0 && fcorrN==1) {
                for (long long p=0; p<width*height; p++) {
                    binfileReadDoubleArray(file, &(correlations[p*N]), N);
                    if (corroffset!=0) for (int i=0; i<N; i++) { correlations[p*N+i]=correlations[p*N+i]-corroffset; }
                    if (fsets>1) binfileReadDoubleArray(file, &(sigmas[p*N]), N);
                }
                QApplication::processEvents();
            } else {
                for (uint32_t cf=0; cf<fcorrN; cf++) {
                    if (cf==corr_set) {
                        for (long long p=0; p<width*height; p++) {
                            binfileReadDoubleArray(file, &(correlations[p*N]), N);
                            if (corroffset!=0) for (int i=0; i<N; i++) { correlations[p*N+i]=correlations[p*N+i]-corroffset; }
                            if (fsets>1) binfileReadDoubleArray(file, &(sigmas[p*N]), N);
                        }
                    } else {
                        file.seek(file.pos()+width*height*N*sizeof(double));
                        if (fsets>1) file.seek(file.pos()+width*height*N*sizeof(double));
                    }
                    QApplication::processEvents();
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
            errorDescription=tr("binary file '%3' did not start with the correct file id: found '%1' but expected '%2'").arg(QString(file_id)).arg(QString("QF3.0imFCS")).arg(filename);
        }
        file.close();

    }
#ifdef DEBUG_TIMING
    //qDebug()<<"read into internal arrays     "<<time.elapsed()<<"ms"; time.start();
#endif

    if (!ok) setError(tr("Error while reading file '%1': %2").arg(filename).arg(errorDescription));
    return ok;
}

bool QFRDRImagingFCSData::loadRadhard2File(const QString& filename) {
    int height=getProperty("HEIGHT", 0).toInt();
    int width=getProperty("WIDTH", 0).toInt();
    int steps=getProperty("STEPS", 0).toInt();

    if (!propertyExists("IS_OVERVIEW_SCALED")) setQFProperty("IS_OVERVIEW_SCALED", false, false, true);

    if((height*width*steps)==0)return false;

    // LOAD FILE
    yaid_rh::corFileReader *cfr=new yaid_rh::corFileReader(filename.toLocal8Bit().constData(),width,height,steps);
    cfr->processFrames(1);

    allocateContents(width,height,cfr->getTotalLagCount()-1);
    //load correlation data
    for(int i=0; i<width*height; i++) {
        for (int j=1; j<cfr->getTotalLagCount(); j++) {
            tau[j-1]=cfr->getTau(i,j)*1e-6;
            correlations[i*(cfr->getTotalLagCount()-1)+j-1]=cfr->getVal(i,j);
            sigmas[i*(cfr->getTotalLagCount()-1)+j-1]=0;
        }
    }
    for(int i=0; i<width*height; i++) {
        overviewF[i]=cfr->getRaw(i,0)/(double(steps)*tau[0])/1.0e3;
        overviewFSTD[i]=0;
    }
    delete cfr;

    //Sfree(corValues);

    QApplication::processEvents();
    recalcCorrelations();
    QApplication::processEvents();

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

bool QFRDRImagingFCSData::leaveoutRun(int run) const {
    return maskGet(runToX(run), runToY(run));
}

double* QFRDRImagingFCSData::getCorrelationRun(int run) const {
    return &(correlations[run*N]);
}

double* QFRDRImagingFCSData::getCorrelationRunError(int run) const {
    return &(sigmas[run*N]);
}

QString QFRDRImagingFCSData::getCorrelationRunName(int run) const {
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
    return !leaveoutRun(run);
}

void QFRDRImagingFCSData::allocateContents(int x, int y, int N) {
    if (correlations) free(correlations);
    if (correlationMean) free(correlationMean);
    if (correlationStdDev) free(correlationStdDev);
    if (sigmas) free(sigmas);
    if (tau) free(tau);
    if (overviewF) free(overviewF);
    if (overviewFSTD) free(overviewFSTD);
    if (leaveout) free(leaveout);
    correlations=NULL;
    correlationMean=NULL;
    correlationStdDev=NULL;
    sigmas=NULL;
    tau=NULL;
    overviewF=NULL;
    leaveout=NULL;
    overviewFSTD=NULL;
    //qDebug()<<"freed overviewFSTD="<<overviewFSTD;
    if ((x>0) && (y>0) && (N>0)) {
        correlations=(double*)calloc(x*y*N,sizeof(double));
        sigmas=(double*)calloc(x*y*N,sizeof(double));
        correlationMean=(double*)calloc(N,sizeof(double));
        correlationStdDev=(double*)calloc(N,sizeof(double));
        overviewF=(double*)calloc(x*y,sizeof(double));
        overviewFSTD=(double*)calloc(x*y,sizeof(double));
        //qDebug()<<"alloced: overviewFSTD="<<overviewFSTD;
        leaveout=(bool*)calloc(x*y,sizeof(bool));
        tau=(double*)calloc(N,sizeof(double));
        width=x;
        height=y;
        this->N=N;
        setQFProperty("WIDTH", x, false, true);
        setQFProperty("HEIGHT", y, false, true);
    }
#ifdef DEBUG_SIZES
    //qDebug()<<"allocateContents( x="<<x<<" y="<<y<<" N="<<N<<"):  "<<bytestostr(N*x*y*2*sizeof(double)+x*y*sizeof(bool)+x*y*sizeof(double)+x*y*sizeof(uint16_t)+N*3*sizeof(double)).c_str();
#endif
}

void QFRDRImagingFCSData::allocateStatistics(uint32_t N) {
    if (statAvg) free(statAvg);
    if (statStdDev) free(statStdDev);
    if (statT) free(statT);
    if (statMin) free(statMin);
    if (statMax) free(statMax);
    statN=N;
    statAvg=NULL;
    statT=NULL;
    statStdDev=NULL;
    statMin=NULL;
    statMax=NULL;
    if (statN>0) {
        statAvg=(double*)calloc(statN, sizeof(double));
        statStdDev=(double*)calloc(statN, sizeof(double));
        statMin=(double*)calloc(statN, sizeof(double));
        statMax=(double*)calloc(statN, sizeof(double));
        statT=(double*)calloc(statN, sizeof(double));
    }
#ifdef DEBUG_SIZES
    //qDebug()<<"allocateStatistics( N="<<N<<"):  "<<bytestostr(statN*5*sizeof(double)).c_str();
#endif

}

void QFRDRImagingFCSData::recalcCorrelations() {
    if (correlations && correlationMean && correlationStdDev) {
        for (int i=0; i<N; i++) {
            double norm=0;
            double sum=0;
            double sum2=0;
            for (int j=0; j<width*height; j++) {
                const double& v=correlations[j*N+i];
                if (QFFloatIsOK(v) && !leaveout[j]) {
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
        }
    }
}

uint32_t QFRDRImagingFCSData::getStatisticsN() const {
    return statN;
}

double *QFRDRImagingFCSData::getStatisticsMean() const {
    return statAvg;
}

double *QFRDRImagingFCSData::getStatisticsT() const {
    return statT;
}

double *QFRDRImagingFCSData::getStatisticsStdDev() const {
    return statStdDev;
}

double *QFRDRImagingFCSData::getStatisticsMin() const {
    return statMin;
}

bool QFRDRImagingFCSData::loadStatistics(const QString &filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream txt(&f);
        QVector<double> mean, stddev, min, max, time;
        while (!txt.atEnd()) {
            QVector<double> line=csvReadline(txt, ',', '#');
            if (line.size()>1) {
                time.append(line[0]);
                mean.append(line[1]);
                stddev.append(line.value(2, 0));
                min.append(line.value(3, 0));
                max.append(line.value(4, 0));
            }
        }
        //qDebug()<<"line read: "<<time.size();
        allocateStatistics(time.size());

        for (uint32_t i=0; i<statN; i++) {
            statT[i]=time[i];
            statAvg[i]=mean[i];
            statStdDev[i]=stddev[i];
            statMin[i]=min[i];
            statMax[i]=max[i];
        }


        f.close();
        return true;
    }
    return false;
}

void QFRDRImagingFCSData::loadQFPropertiesFromB040SPIMSettingsFile(QSettings &settings) {
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/duration_milliseconds")) {
        setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/duration_milliseconds").toDouble(), true, true);
    }
    if (!propertyExists("MEASUREMENT_DURATION_MS") && settings.contains("acquisition/duration")) {
        setQFProperty("MEASUREMENT_DURATION_MS", settings.value("acquisition/duration").toDouble()*1000.0, true, true);
    }
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/frame_time")) {
        setQFProperty("FRAMETIME_MS", settings.value("acquisition/frame_time").toDouble()*1000.0, true, true);
    }
    if (!propertyExists("FRAMETIME_MS") && settings.contains("acquisition/frame_rate")) {
        setQFProperty("FRAMETIME_MS", 1.0/settings.value("acquisition/frame_rate").toDouble()*1000.0, true, true);
    }
    if (!propertyExists("MAGNIFICATION") && settings.contains("acquisition/magnification")) {
        setQFProperty("MAGNIFICATION", settings.value("acquisition/magnification").toDouble(), true, true);
    }
    if (!propertyExists("ROI_X_START") && settings.contains("acquisition/roi_xstart")) {
        setQFProperty("ROI_X_START", settings.value("acquisition/roi_xstart").toInt(), true, true);
    }
    if (!propertyExists("ROI_X_END") && settings.contains("acquisition/roi_xend")) {
        setQFProperty("ROI_X_END", settings.value("acquisition/roi_xend").toInt(), true, true);
    }
    if (!propertyExists("ROI_Y_START") && settings.contains("acquisition/roi_ystart")) {
        setQFProperty("ROI_Y_START", settings.value("acquisition/roi_ystart").toInt(), true, true);
    }
    if (!propertyExists("ROI_Y_END") && settings.contains("acquisition/roi_yend")) {
        setQFProperty("ROI_Y_END", settings.value("acquisition/roi_yend").toInt(), true, true);
    }
    if (!propertyExists("PIXEL_WIDTH") && settings.contains("acquisition/pixel_width")) {
        double mag=settings.value("acquisition/magnification", 1.0).toDouble();
        double cpw=settings.value("acquisition/camera_pixel_width", settings.value("acquisition/pixel_width").toDouble()).toDouble();
        double pw=settings.value("acquisition/pixel_width").toDouble();
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

}

double QFRDRImagingFCSData::getTauMin() const {
    if (!tau|| N<=0) return 1;
    int i=0;
    while (i<N-1 && tau[i]==0) i++;
    return tau[i];
}

void QFRDRImagingFCSData::clearOvrImages() {
    for (int i=0; i<ovrImages.size(); i++) {
        free(ovrImages[i].image);
    }
    ovrImages.clear();
}



















int QFRDRImagingFCSData::getImageFromRunsWidth() const {
    return width;
}

int QFRDRImagingFCSData::getImageFromRunsHeight() const {
    return height;
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

double* QFRDRImagingFCSData::getImageFromRunsPreview() const {
    return overviewF;
}

void QFRDRImagingFCSData::leaveoutClear() {
    maskClear();
}

void QFRDRImagingFCSData::maskLoad(const QString &filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        maskClear();
        QTextStream str(&f);
        while (!str.atEnd())  {
            QVector<double> d=csvReadline(str, ',', '#', -1);
            if (d.size()==2) {
                int idx=xyToRun(d[0], d[1]);
                if (idx>=0 && idx<height*width) leaveout[idx]=true;
            }
        }

        f.close();
    }
}

void QFRDRImagingFCSData::maskSave(const QString &filename) const {
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream str(&f);
        for (uint16_t y=0; y<height; y++) {
            for (uint16_t x=0; x<width; x++) {
                if (leaveout[y*width+x]) {
                    str<<x<<", "<<y<<"\n";
                }
            }
        }

        f.close();
    }
}

void QFRDRImagingFCSData::maskClear() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=false;
    }
}

void QFRDRImagingFCSData::maskSetAll() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=true;
    }
}

void QFRDRImagingFCSData::leaveoutRemoveRun(int run) {
    if (run>=0 && run<width*height) leaveout[run]=false;
}

void QFRDRImagingFCSData::leaveoutAddRun(int run) {
    if (run>=0 && run<width*height) leaveout[run]=true;
}

bool *QFRDRImagingFCSData::maskGet() const {
    return leaveout;
}

bool QFRDRImagingFCSData::maskGet(uint16_t x, uint16_t y) const {
    return leaveout[y*width+x];
}

void QFRDRImagingFCSData::maskUnset(uint16_t x, uint16_t y, bool value) {
    if (!leaveout) return;
    leaveout[y*width+x]=value;
}

void QFRDRImagingFCSData::maskToggle(uint16_t x, uint16_t y) {
    leaveout[y*width+x]=!leaveout[y*width+x];
}

void QFRDRImagingFCSData::maskInvert() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=!leaveout[i];
    }
}

void QFRDRImagingFCSData::maskSet(uint16_t x, uint16_t y) {
    if (!leaveout) return;
    leaveout[y*width+x]=false;
}

double *QFRDRImagingFCSData::getStatisticsMax() const {
    return statMax;
}





int QFRDRImagingFCSData::getPreviewImageCount() const {
    return 2+ovrImages.size();
}

int QFRDRImagingFCSData::getPreviewImageWidth(int image) const {
    if (image==0) return getImageFromRunsWidth();
    if (image==1) return getImageFromRunsWidth();
    if (image-1<=ovrImages.size()) return ovrImages[image-2].width;
    return 0;
}

int QFRDRImagingFCSData::getPreviewImageHeight(int image) const {
    if (image==0) return getImageFromRunsHeight();
    if (image==1) return getImageFromRunsHeight();
    if (image-1<=ovrImages.size()) return ovrImages[image-2].height;
    return 0;
}

QString QFRDRImagingFCSData::getPreviewImageName(int image) const {
    if (image==0) return tr("overview image (time average)");
    if (image==1) return tr("standard deviation overview image (time average)");
    if (image-1<=ovrImages.size()) return ovrImages[image-2].name;
    return QString("");
}

double *QFRDRImagingFCSData::getPreviewImage(int image) const {
    if (image==0) return overviewF;
    //qDebug()<<"overviewFSTD="<<overviewFSTD;
    if (image==1) return overviewFSTD;
    if (image-1<=ovrImages.size()) return ovrImages[image-2].image;
    return NULL;
}

QList<QFRDROverviewImageInterface::OverviewImageGeoElement> QFRDRImagingFCSData::getPreviewImageGeoElements(int image) const {
    QList<QFRDROverviewImageInterface::OverviewImageGeoElement> result;
    if (image>1 && image<=ovrImages.size()) return ovrImages[image-2].geoElements;
    return result;
}

int QFRDRImagingFCSData::getImageStackCount() const {
    return 1;
}

uint32_t QFRDRImagingFCSData::getImageStackFrames(int stack) const {
    if (stack==0) return video_frames;
    return 0;
}

int QFRDRImagingFCSData::getImageStackWidth(int stack) const {
    if (stack==0) return video_width;
    return 0;
}

int QFRDRImagingFCSData::getImageStackHeight(int stack) const {
    if (stack==0) return video_height;
    return 0;
}

int QFRDRImagingFCSData::getImageStackChannels(int stack) const {
    if (stack==0) return 1;
    return 0;
}

double *QFRDRImagingFCSData::getImageStack(int stack, uint32_t frame, uint32_t channel) const {
    if (stack==0) return &(video[frame*video_width*video_height]);
    return NULL;
}

double QFRDRImagingFCSData::getImageStackXUnitFactor(int stack) const {
    return getProperty("PIXEL_WIDTH", 1.0).toDouble();
}

QString QFRDRImagingFCSData::getImageStackXUnitName(int stack) const {
    return QString("micrometer");
}

QString QFRDRImagingFCSData::getImageStackXName(int stack) const {
    return tr("x");
}

double QFRDRImagingFCSData::getImageStackYUnitFactor(int stack) const {
    return getProperty("PIXEL_HEIGHT", 1.0).toDouble();
}

QString QFRDRImagingFCSData::getImageStackYUnitName(int stack) const {
    return QString("micrometer");
}

QString QFRDRImagingFCSData::getImageStackYName(int stack) const {
    return QString("y");
}

double QFRDRImagingFCSData::getImageStackTUnitFactor(int stack) const {
    if (video_frames<=0) return 1;
    double stat=1;
    if (statT) stat=(statT[statN]-statT[0]);
    return getProperty("MEASUREMENT_DURATION_MS", stat*1000.0).toDouble()/1000.0/double(video_frames);
    return 1;
}

QString QFRDRImagingFCSData::getImageStackTUnitName(int stack) const {
    if (stack==0) return tr("seconds");
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackTName(int stack) const {
    if (stack==0) return tr("time");
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
    if (stack==0) return tr("averaged video");
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackChannelName(int stack, int channel) const {
    return QString("");
}

QString QFRDRImagingFCSData::getImageStackTimepointName(int stack, int t) const {
    return QString("%1%2").arg(getImageStackTUnitFactor(stack)*t).arg(getImageStackTUnitName(stack));
}

double QFRDRImagingFCSData::getSimpleCountrateAverage(int run) const {
    if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewF) {
        if (run>=0) return overviewF[run];
    }
    if (hasStatistics) return statAvgCnt/getTauMin()/1000.0;
    return 0;
}

double QFRDRImagingFCSData::getSimpleCountrateVariance(int run) const {
    if (!getProperty("IS_OVERVIEW_SCALED", true).toBool() && overviewFSTD) {
        if (run>=0 && run<width*height) return overviewFSTD[run];
    }
    if (hasStatistics) return sqrt(statSigmaCnt)/getTauMin()/1000.0;
    return 0;
}


