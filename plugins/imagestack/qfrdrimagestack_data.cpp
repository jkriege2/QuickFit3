#include "qfrdrimagestack_data.h"
#include <QtXml>

QFRDRImageStackData::QFRDRImageStackData(QFProject* parent):
    QFRawDataRecord(parent)
{
    memsize=0;
}

QFRDRImageStackData::~QFRDRImageStackData() {
    clearMemory();
}

void QFRDRImageStackData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
	// THIS IS OPTIONAL
}


void QFRDRImageStackData::intWriteData(QXmlStreamWriter& w) {
	// write data to the project XML file using the QXmlStreamWriter
    /*
    w.writeStartElement("imagestacks");
    w.writeAttribute("imagestack", "123456789");
    w.writeEndElement();
    */
}

void QFRDRImageStackData::intReadData(QDomElement* e) {
	// read data from the project XML file 

	/*
    if (e) {
        QDomElement te=e->firstChildElement("mytag");
        QString data=te.attribute("data");
    }
	*/

    QString stacktype=getProperty("STACKTYPE", "ONEFILEPERCHANNEL").toString().toUpper();
    clearMemory();
    stacks.clear();
    if (stacktype=="ONEFILEPERCHANNEL") {
        if (files.size()>0) {
            getProject()->getServices()->log_text(tr("  - loading images in image stack '%1'\n").arg(getName()));
            // we need two runs: in the first one, we only read the image stack sizes, then we can allocate the memory for the
            // image series and finally read in the data
            for (int f=0; f<files.size(); f++) {
                if (files_types.value(f, "image").toLower()=="image") {
                    QFRDRImageStackData::ImageStack s;
                    s.file=f;
                    stacks.append(s);
                    loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmGetSize);
                    getProject()->getServices()->log_text(tr("    * image stack %1 (file: '%2') size:   width=%3,   height=%4,   frames=%5\n").arg(stacks.size()).arg(files[f]).arg(stacks.last().width).arg(stacks.last().height).arg(stacks.last().frames));
                }
            }
            if (allocateMemory()) {
                getProject()->getServices()->log_text(tr("  - allocated %1 of memory\n").arg(bytestostr(memsize).c_str()));
                for (int f=0; f<files.size(); f++) {
                    if (files_types.value(f, "image").toLower()=="image") {
                        getProject()->getServices()->log_text(tr("    * loading image stack %1 (file: '%2') ...\n").arg(stacks.size()).arg(files[f]));
                        if (loadImageFile(stacks.last(), files[f], QFRDRImageStackData::lmReadData)) {
                        } else {
                            setError(tr("error loading image stack %1 (file: '%2')\n").arg(stacks.size()).arg(files[f]));
                            return;
                        }
                    }
                }
            } else setError(tr("Error allocating %1 of memory!").arg(bytestostr(memsize).c_str()));
        } else setError(tr("there are no files in the %1 record!").arg(getName()));
    } else {
        setError(tr("Error while reading image stack: unknown stack type %1").arg(stacktype));
        return;
    }

}

bool QFRDRImageStackData::loadImageFile(QFRDRImageStackData::ImageStack& stack, QString filename, loadMode mode) {
	bool ok=true;
    QString filetype=getProperty("FILETYPE", "").toString();
    QStringList reader_id=QFRDRImageStackData::getImageReaderIDList(getProject()->getServices());
    int reader_idx=reader_id.indexOf(filetype);
    if (!reader_id.contains(filetype)) {
        setError(tr("Error while reading image: unknown file type %2 (file was: '%1')").arg(filename).arg(filetype));
        return false;
    }

    QFImporterImageSeries* reader=getImageReader(reader_idx, getProject()->getServices());
    if (reader) {
        if (reader->open(filename)) {
            if (mode==QFRDRImageStackData::lmGetSize) {
                stack.channels=1;
                stack.width=reader->frameWidth();
                stack.height=reader->frameHeight();
                stack.frames=reader->countFrames();
            } else {
                int i=0;
                do {
                    double* d=&(stack.data[i*stack.width*stack.height]);
                    reader->readFrameDouble(d);
                    i++;
                } while (i<stack.frames && reader->nextFrame());
            }
            reader->close();
            delete reader;
        } else {
            ok=false;
            setError(tr("Could not open image file '%1' with reader %2").arg(filename).arg(filetype));
        }
    } else {
        ok=false;
        setError(tr("Could not allocate reader %2 for image file '%1'").arg(filename).arg(filetype));
    }

    if (!ok) setError(tr("Error while reading file '%1'").arg(filename));
    return ok;
}

bool QFRDRImageStackData::allocateMemory() {
    clearMemory();
    bool ok=true;
    memsize=0;
    for (int i=0; i<stacks.size(); i++) {
        stacks[i].data=(double*)calloc(stacks[i].channels*stacks[i].frames*stacks[i].height*stacks[i].width, sizeof(double));
        ok=ok && (stacks[i].data!=NULL);
        if (stacks[i].data) {
            memsize+=stacks[i].channels*stacks[i].frames*stacks[i].height*stacks[i].width*sizeof(double);
        }
    }
    if (!ok) clearMemory();
    return ok;
}

void QFRDRImageStackData::clearMemory() {
    for (int i=0; i<stacks.size(); i++) {
        if (stacks[i].data) free(stacks[i].data);
        stacks[i].data=NULL;
    }
    memsize=0;
}

QVariant QFRDRImageStackData::getEnumeratedProperty(const QString &prop, int stack, QVariant defaultValue) const {
    QVariant res=getProperty(prop, defaultValue);
    if (stack>=0) return getProperty(prop+QString::number(stack), res);
    else return res;
}

QVariant QFRDRImageStackData::getDoubleEnumeratedProperty(const QString &prop, int stack, int channel, QVariant defaultValue) const {
    QVariant res=getProperty(prop, defaultValue);
    if (stack>=0) res=getProperty(prop+QString(stack), res);
    if (stack>=0) res=getProperty(prop+QString(stack), res);
    if (channel>=0) return getProperty(prop+"_"+QString::number(channel), res);
    if (stack>=0 && channel>=0) return  getProperty(prop+QString(stack)+"_"+QString::number(channel), res);
    else return res;
}

int QFRDRImageStackData::getImageStackCount() const {
    return stacks.size();
}

uint32_t QFRDRImageStackData::getImageStackFrames(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.frames;
}

int QFRDRImageStackData::getImageStackWidth(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.width;
}

int QFRDRImageStackData::getImageStackHeight(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.height;
}

int QFRDRImageStackData::getImageStackChannels(int stack) const {
    QFRDRImageStackData::ImageStack stacko=stacks.value(stack, QFRDRImageStackData::ImageStack());
    return stacko.channels;
}

double *QFRDRImageStackData::getImageStack(int stackID, uint32_t frame, uint32_t channel) const {
    QFRDRImageStackData::ImageStack stack=stacks.value(stackID, QFRDRImageStackData::ImageStack());
    if (stack.data==NULL) return NULL;
    else {
        return &(stack.data[channel*stack.width*stack.height*stack.frames+frame*stack.width*stack.height]);
    }
}

double QFRDRImageStackData::getImageStackXUnitFactor(int stack) const {
    return getEnumeratedProperty("XUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackXUnitName(int stack) const {
    return getEnumeratedProperty("XUNIT_NAME", stack, tr("pixels")).toString();
}

QString QFRDRImageStackData::getImageStackXName(int stack) const {
    return getEnumeratedProperty("XAXIS_NAME", stack, "x").toString();
}

double QFRDRImageStackData::getImageStackYUnitFactor(int stack) const {
    return getEnumeratedProperty("YUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackYUnitName(int stack) const {
    return getEnumeratedProperty("YUNIT_NAME", stack, tr("pixels")).toString();
}

QString QFRDRImageStackData::getImageStackYName(int stack) const {
    return getEnumeratedProperty("YAXIS_NAME", stack, "y").toString();
}

double QFRDRImageStackData::getImageStackTUnitFactor(int stack) const {
    return getEnumeratedProperty("TUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackTUnitName(int stack) const {
    return getEnumeratedProperty("TUNIT_NAME", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackTName(int stack) const {
    return getEnumeratedProperty("TAXIS_NAME", stack, "time").toString();
}

double QFRDRImageStackData::getImageStackCUnitFactor(int stack) const {
    return getEnumeratedProperty("CUNIT_FACTOR", stack, 1.0).toDouble();
}

QString QFRDRImageStackData::getImageStackCUnitName(int stack) const {
    return getEnumeratedProperty("YUNIT_NAME", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackCName(int stack) const {
    return getEnumeratedProperty("CAXIS_NAME", stack, tr("channel")).toString();
}

QString QFRDRImageStackData::getImageStackDescription(int stack) const {
    return getEnumeratedProperty("STACK_DESCRIPTION", stack, "").toString();
}

QString QFRDRImageStackData::getImageStackChannelName(int stack, int channel) const {
    return getDoubleEnumeratedProperty("CHANNELNAME_", stack, channel, tr("channel %1").arg(channel+1)).toString();
}

QString QFRDRImageStackData::getImageStackTimepointName(int stack, int t) const {
    return getDoubleEnumeratedProperty("TIMEPOINTNAME_", stack, t, QString("")).toString();
}


QStringList QFRDRImageStackData::getImageFilterList(QFPluginServices* pluginservices)   {
    QStringList l;
    int i=0;
    QFImporterImageSeries* r=NULL;
    while ((r=getImageReader(i, pluginservices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFRDRImageStackData::getImageReaderIDList(QFPluginServices* pluginservices)   {
    return pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();
}

QStringList QFRDRImageStackData::getImageFormatNameList(QFPluginServices* pluginservices)   {
     QStringList l;
     int i=0;
     QFImporterImageSeries* r=NULL;
     while ((r=getImageReader(i, pluginservices))!=NULL) {
         l.append(r->formatName());
         delete r;
         i++;
     }
     return l;
}

QFImporterImageSeries* QFRDRImageStackData::getImageReader(int idx, QFPluginServices* pluginservices)  {
    QFImporterImageSeries* r=NULL;

    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFImporterImageSeries*>(pluginservices->getImporterManager()->createImporter(imp[idx]));
    }

    return r;
}

int QFRDRImageStackData::getImageReaderCount(QFPluginServices* pluginservices)  {
    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();
    return imp.size();
}

QFRDRImageStackData::ImageStack::ImageStack() {
    data=NULL;
    width=0;
    height=0;
    frames=0;
    channels=0;
    file=-1;
}
