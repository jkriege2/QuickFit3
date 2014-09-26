/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-25 17:54:41 +0200 (Do, 25 Sep 2014) $  (revision $Rev: 3506 $)

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
        QRegExp rxImages("images\\s*\\=\\s*(\\d+)");
        if (rxImages.indexIn(meta)>=0) {
            data["FRAMES"]=rxImages.cap(1).toInt();
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
