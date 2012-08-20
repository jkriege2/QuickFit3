#include "qfrdrnumberandbrightness_data.h"
#include <QtXml>
#include "libtiff_tools.h"
#include "csvtools.h"
#include <QTextStream>
#include "tools.h"
#include "statistics_tools.h"
#include "csvtools.h"
#include "qfrdrimagingfcstools.h"
#include "qfrdrnumberandbrightness_overvieweditor.h"

#define sqr(x) ((x)*(x))

QFRDRNumberAndBrightnessData::QFRDRNumberAndBrightnessData(QFProject* parent):
    QFRawDataRecord(parent)
{
    width=0;
    height=0;

    background=NULL;
    backgroundVariance=NULL;
    image=NULL;
    imageVariance=NULL;
    numberImage=NULL;
    brightnessImage=NULL;
    leaveout=NULL;

}

QFRDRNumberAndBrightnessData::~QFRDRNumberAndBrightnessData()
{
    allocateContents(0,0);
    clearOvrImages();
    clearSelections();
}

int QFRDRNumberAndBrightnessData::getEditorCount()
{
    return 2;
}

QString QFRDRNumberAndBrightnessData::getEditorName(int i)
{
    if (i==0) return tr("number & brightness");
    if (i==1) return tr("overview images");
    return QString("");
}

QFRawDataEditor *QFRDRNumberAndBrightnessData::createEditor(QFPluginServices *services, QFRawDataPropertyEditor *propEditor, int i, QWidget *parent)
{
    if (i==0) return new QFRDRNumberAndBrightnessDataEditor(services, propEditor, parent);
    if (i==1) return new QFRDRNumberAndBrightnessOverviewEditor(services, propEditor, parent);
    return NULL;
}

void QFRDRNumberAndBrightnessData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
    // THIS IS OPTIONAL
}

double *QFRDRNumberAndBrightnessData::getImage() const
{
    return image;
}

double *QFRDRNumberAndBrightnessData::getImageVariance() const
{
    return imageVariance;
}

double *QFRDRNumberAndBrightnessData::getBackground() const
{
    return background;
}

double *QFRDRNumberAndBrightnessData::getBackgroundVariance() const
{
    return backgroundVariance;
}

double *QFRDRNumberAndBrightnessData::getNumberImage() const
{
    return numberImage;
}

double *QFRDRNumberAndBrightnessData::getBrightnessImage() const
{
    return brightnessImage;
}

int QFRDRNumberAndBrightnessData::getWidth() const
{
    return width;
}

int QFRDRNumberAndBrightnessData::getHeight() const
{
    return height;
}

void QFRDRNumberAndBrightnessData::recalcNumberAndBrightness() {
    if (image && imageVariance && background && backgroundVariance && numberImage && brightnessImage) {
        if (getProperty("BACKGROUND_CORRECTED", false).toBool()) {
            for (int i=0; i<width*height; i++) {
                numberImage[i]=sqr(image[i])/(imageVariance[i]-backgroundVariance[i]);
                brightnessImage[i]=(imageVariance[i]-backgroundVariance[i])/(image[i]);
            }
        } else {
            for (int i=0; i<width*height; i++) {
                numberImage[i]=sqr(image[i]-background[i])/(imageVariance[i]-backgroundVariance[i]);
                brightnessImage[i]=(imageVariance[i]-backgroundVariance[i])/(image[i]-background[i]);
            }
        }

        //resultsSetNumberList("number_and_brightness", "particle_number", numberImage, width*height);
        //resultsSetNumberList("number_and_brightness", "particle_brightness", brightnessImage, width*height);
    }
}

bool QFRDRNumberAndBrightnessData::leaveoutRun(int run) const
{
    return maskGet(indexToX(run), indexToY(run));
}


void QFRDRNumberAndBrightnessData::intWriteData(QXmlStreamWriter& w) {
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

void QFRDRNumberAndBrightnessData::intReadData(QDomElement* e) {
    leaveoutClear();
    clearSelections();
    // read data from the project XML file



    width=getProperty("WIDTH", 0).toInt();
    height=getProperty("HEIGHT", 0).toInt();

    allocateContents(width, height);

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
                    while (files_desciptions.size()+1<files_types.size()) files_desciptions<<"";
                    files_desciptions<<lfiles_descriptions[i];
                }


            }
        }

        // finally we load all known and useful associated files into memory:
        for (int i=0; i<files.size(); i++) {
            if (i<files_types.size()) {
                QString ft=files_types[i].toLower().trimmed();
                if (ft=="image") {
                    loadFile(image, files[i]);
                } else if (ft=="image_std") {
                    loadFile(imageVariance, files[i], QFRDRNumberAndBrightnessData::Square);
                } else if (ft=="image_var") {
                    loadFile(imageVariance, files[i], QFRDRNumberAndBrightnessData::NoOperation);
                } else if (ft=="background") {
                    loadFile(background, files[i]);
                } else if (ft=="background_stddev") {
                    loadFile(backgroundVariance, files[i], QFRDRNumberAndBrightnessData::Square);
                } else if (ft=="background_var") {
                    loadFile(backgroundVariance, files[i], QFRDRNumberAndBrightnessData::NoOperation);
                } else if ((ft=="display_image") || (ft=="overview")) {
                    QFRDRNumberAndBrightnessData::ovrImageData img;
                    loadImage(files[i], &(img.image), &(img.width), &(img.height));
                    img.name=files_desciptions.value(i, tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName()));
                    if (img.name.isEmpty()) img.name=tr("image #%1: %2").arg(i+1).arg(QFileInfo(files[i]).fileName());
                    ovrImages.append(img);
                } else if (ft=="overview_before") {
                    QFRDRNumberAndBrightnessData::ovrImageData img;
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
                    QFRDRNumberAndBrightnessData::ovrImageData img;
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
                    QFRDRNumberAndBrightnessData::ovrImageData img;
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
                    QFRDRNumberAndBrightnessData::ovrImageData img;
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
                    QFRDRNumberAndBrightnessData::ovrImageData img;
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
        recalcNumberAndBrightness();

        te=e->firstChildElement("selections");
        if (!te.isNull()) {
            int cnt=1;
            te=te.firstChildElement("selection");
            while (!te.isNull()) {
                QString n=te.attribute("name", tr("selection #%1").arg(cnt));
                QString l=te.attribute("list", "");
                int selSize=getImageSelectionWidth()*getImageSelectionHeight();
                QFRDRNumberAndBrightnessData::ImageSelection sel;
                sel.selection=(bool*)calloc(selSize, sizeof(bool));
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

}

bool QFRDRNumberAndBrightnessData::loadFile(double *target, const QString &filename, QFRDRNumberAndBrightnessData::FileOperations op)
{
    bool ok=false;

    if (!target) return false;

    if (QFile::exists(filename)) {
        TIFF* tif=TIFFOpen(filename.toAscii().data(), "r");
        if (tif) {
            uint32 nx,ny;
            TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
            TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
            if (nx!=width || ny!=height) {
                ok=false;
            } else {
                ok=TIFFReadFrame<double>(tif, target);
            }
            TIFFClose(tif);
        }
    }

    if (!ok && target) {
        for (int i=0; i<width*height; i++) {
            target[i]=0;
        }
    }

    switch(op) {
        case QFRDRNumberAndBrightnessData::TakeSqrt:
            for (int i=0; i<width*height; i++) {
                target[i]=sqrt(target[i]);
            }
            break;
        case QFRDRNumberAndBrightnessData::Square:
            for (int i=0; i<width*height; i++) {
                target[i]=target[i]*target[i];
            }
            break;
    }
    return ok;
}

void QFRDRNumberAndBrightnessData::allocateContents(int x, int y, int N) {
    if (numberImage) free(numberImage);
    if (brightnessImage) free(brightnessImage);
    if (image) free(image);
    if (imageVariance) free(imageVariance);
    if (background) free(background);
    if (backgroundVariance) free(backgroundVariance);
    if (leaveout) free(leaveout);
    image=NULL;
    numberImage=NULL;
    imageVariance=NULL;
    background=NULL;
    backgroundVariance=NULL;
    leaveout=NULL;
    int NN=1;
    if ((x>0) && (y>0) && (NN>0)) {
        image=(double*)calloc(x*y*NN,sizeof(double));
        imageVariance=(double*)calloc(x*y*NN,sizeof(double));
        background=(double*)calloc(x*y*NN,sizeof(double));
        backgroundVariance=(double*)calloc(x*y*NN,sizeof(double));
        numberImage=(double*)calloc(x*y*NN,sizeof(double));
        brightnessImage=(double*)calloc(x*y*NN,sizeof(double));
        leaveout=(bool*)calloc(x*y,sizeof(bool));
        width=x;
        height=y;
        setQFProperty("WIDTH", x, false, true);
        setQFProperty("HEIGHT", y, false, true);
    }
}


void QFRDRNumberAndBrightnessData::loadQFPropertiesFromB040SPIMSettingsFile(QSettings &settings) {
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

void QFRDRNumberAndBrightnessData::clearOvrImages() {
    for (int i=0; i<ovrImages.size(); i++) {
        free(ovrImages[i].image);
    }
    ovrImages.clear();
}


void QFRDRNumberAndBrightnessData::leaveoutClear() {
    maskClear();
}

void QFRDRNumberAndBrightnessData::maskLoad(const QString &filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        maskClear();
        QTextStream str(&f);
        while (!str.atEnd())  {
            QVector<double> d=csvReadline(str, ',', '#', -1);
            if (d.size()==2) {
                int idx=xyToIndex(d[0], d[1]);
                if (idx>=0 && idx<height*width) leaveout[idx]=true;
            }
        }

        f.close();
    }
}

void QFRDRNumberAndBrightnessData::maskSave(const QString &filename) const {
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

void QFRDRNumberAndBrightnessData::maskClear() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=false;
    }
}

void QFRDRNumberAndBrightnessData::maskSetAll() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=true;
    }
}

void QFRDRNumberAndBrightnessData::leaveoutRemoveRun(int run) {
    if (run>=0 && run<width*height) leaveout[run]=false;
}

void QFRDRNumberAndBrightnessData::leaveoutAddRun(int run) {
    if (run>=0 && run<width*height) leaveout[run]=true;
}

bool *QFRDRNumberAndBrightnessData::maskGet() const {
    return leaveout;
}

bool QFRDRNumberAndBrightnessData::maskGet(uint16_t x, uint16_t y) const {
    return leaveout[y*width+x];
}

void QFRDRNumberAndBrightnessData::maskUnset(uint16_t x, uint16_t y, bool value) {
    if (!leaveout) return;
    leaveout[y*width+x]=value;
}

void QFRDRNumberAndBrightnessData::maskToggle(uint16_t x, uint16_t y) {
    leaveout[y*width+x]=!leaveout[y*width+x];
}

void QFRDRNumberAndBrightnessData::maskInvert() {
    if (!leaveout) return;
    for (uint16_t i=0; i<width*height; i++) {
        leaveout[i]=!leaveout[i];
    }
}

void QFRDRNumberAndBrightnessData::maskSet(uint16_t x, uint16_t y) {
    if (!leaveout) return;
    leaveout[y*width+x]=false;
}


int QFRDRNumberAndBrightnessData::getOverviewImageCount() const {
    return 4+ovrImages.size();
}

int QFRDRNumberAndBrightnessData::getOverviewImageWidth(int image) const {
    if (image<0) return 0;
    if (image<4) return width;
    if (image-4<ovrImages.size()) return ovrImages[image-4].width;
    return 0;
}

int QFRDRNumberAndBrightnessData::getOverviewImageHeight(int image) const {
    if (image<0) return 0;
    if (image<4) return height;
    if (image-4<ovrImages.size()) return ovrImages[image-4].height;
    return 0;
}

QString QFRDRNumberAndBrightnessData::getOverviewImageName(int image) const {
    if (image==0) return tr("overview image (time average)");
    if (image==1) return tr("variance of overview image (time average)");
    if (image==2) return tr("background image (time average)");
    if (image==3) return tr("variance of background image (time average)");
    if (image-4<ovrImages.size()) return ovrImages[image-4].name;
    return QString("");
}

double *QFRDRNumberAndBrightnessData::getOverviewImage(int img) const {
    if (img==0) return image;
    if (img==1) return imageVariance;
    if (img==2) return background;
    if (img==3) return backgroundVariance;
    if (img-4<ovrImages.size()) return ovrImages[img-4].image;
    return NULL;
}

QList<QFRDROverviewImageInterface::OverviewImageGeoElement> QFRDRNumberAndBrightnessData::getOverviewImageAnnotations(int image) const {
    QList<QFRDROverviewImageInterface::OverviewImageGeoElement> result;
    if (image>3 && image-4<=ovrImages.size()) return ovrImages[image-2].geoElements;
    return result;
}













int QFRDRNumberAndBrightnessData::getImageSelectionCount() const
{
    return selections.size();
}

QString QFRDRNumberAndBrightnessData::getImageSelectionName(int selection) const
{
    return selections.value(selection, QFRDRNumberAndBrightnessData::ImageSelection()).name;
}

bool *QFRDRNumberAndBrightnessData::loadImageSelection(int selection) const
{
    return selections.value(selection, QFRDRNumberAndBrightnessData::ImageSelection()).selection;
}

void QFRDRNumberAndBrightnessData::addImageSelection(bool *selection, const QString &name)
{
    QFRDRNumberAndBrightnessData::ImageSelection s;
    s.name=name;
    s.selection=(bool*)calloc(getImageSelectionHeight()*getImageSelectionWidth(), sizeof(bool));
    memcpy(s.selection, selection, getImageSelectionHeight()*getImageSelectionWidth()*sizeof(bool));
    selections.append(s);
}

void QFRDRNumberAndBrightnessData::deleteImageSelection(int selection)
{
    if (selection>=0 && selection<selections.size()) {
        selections.removeAt(selection);
    }
}

void QFRDRNumberAndBrightnessData::setImageSelectionName(int selection, const QString &name)
{
    if (selection>=0 && selection<selections.size()) {
         QFRDRNumberAndBrightnessData::ImageSelection s=selections[selection];
         s.name=name;
         selections[selection]=s;
    }
}

int QFRDRNumberAndBrightnessData::getImageSelectionHeight() const
{
    return height;
}

int QFRDRNumberAndBrightnessData::getImageSelectionWidth() const
{
    return width;
}

void QFRDRNumberAndBrightnessData::clearSelections()
{
    for (int i=0; i<selections.size(); i++) {
        if (selections[i].selection) delete selections[i].selection;
    }
    selections.clear();
}



bool QFRDRNumberAndBrightnessData::loadImage(const QString& filename, double** data, int* width, int* height) {
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

bool QFRDRNumberAndBrightnessData::loadVideo(const QString& filename, double** data, int* width, int* height, uint32_t* frames) {
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

int QFRDRNumberAndBrightnessData::xyToIndex(int x, int y) const {
    return y*width+x;
}

int QFRDRNumberAndBrightnessData::indexToX(int run) const {
    return run%width;
}

int QFRDRNumberAndBrightnessData::indexToY(int run) const {
    return run/width;
}
