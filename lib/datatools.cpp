/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "datatools.h"
#include "qftools.h"
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include "programoptions.h"

QList<QVector<double> > dataRotate(const QList<QVector<double> >& data) {
    QList<QVector<double> > result;
    int cols=0;
    for (int i=0; i<data.size(); i++) {
        if (cols<data[i].size()) cols=data[i].size();
    }
    QVector<double> dEmpty(data.size(), NAN);
    for (int i=0; i<cols; i++) {
        result<<dEmpty;
    }
    for (int i=0; i<data.size(); i++) {
        for (int r=0; r<data[i].size(); r++) {
            result[r].operator[](i)=data[i].at(r);
        }
    }
    return result;
}

QList<QList<double> > dataRotate(const QList<QList<double> >& data) {
    QList<QList<double> > result;
    int cols=0;
    for (int i=0; i<data.size(); i++) {
        if (cols<data[i].size()) cols=data[i].size();
    }
    QList<double> dEmpty;
    for (int i=0; i<data.size(); i++) dEmpty<<NAN;
    for (int i=0; i<cols; i++) {
        result<<dEmpty;
    }
    for (int i=0; i<data.size(); i++) {
        for (int r=0; r<data[i].size(); r++) {
            result[r].operator[](i)=data[i].at(r);
        }
    }
    return result;
}

QList<QList<QVariant> > dataRotate(const QList<QList<QVariant> >& data) {
    QList<QList<QVariant> > result;
    int cols=0;
    for (int i=0; i<data.size(); i++) {
        if (cols<data[i].size()) cols=data[i].size();
    }
    QList<QVariant> dEmpty;
    for (int i=0; i<data.size(); i++) dEmpty<<QVariant();
    for (int i=0; i<cols; i++) {
        result<<dEmpty;
    }
    for (int i=0; i<data.size(); i++) {
        for (int r=0; r<data[i].size(); r++) {
            result[r].operator[](i)=data[i].at(r);
        }
    }
    return result;
}

void dataExpand(QList<QList<QVariant> >& data, QStringList* columnsNames) {
    QList<QList<QVariant> > d;
    for (int i=0; i<d.size(); i++) d.append(QList<QVariant>());
    QStringList cn;
    //if (columnsNames) cn = (*columnsNames);
    int colid=0;
    for (int c=0; c<data.size(); c++) {
        int cols=1;
        bool hasErrors=false;
        for (int r=0; r<data[c].size(); r++) {
            const QVariant& v=data[c].at(r);
            //qDebug()<<"c="<<c<<"r="<<r<<" = "<<v;
            switch(v.type()) {
                case QVariant::Line:
                case QVariant::LineF:
                    cols=qMax(cols, 4);
                    break;
                case QVariant::Point:
                case QVariant::PointF:
                case QVariant::Size:
                case QVariant::SizeF:
                    cols=qMax(cols, 2);
                    break;
                case QVariant::Vector2D:
                    cols=qMax(cols, 2);
                    break;
                case QVariant::Vector3D:
                    cols=qMax(cols, 3);
                    break;
                case QVariant::Vector4D:
                    cols=qMax(cols, 4);
                    break;
                case QVariant::QVariant::Matrix4x4:
                    cols=qMax(cols, 16);
                    break;
                case QVariant::List: {
                    QVariantList vl=v.toList();
                    if (vl.size()>0 && (vl.first().type()==QVariant::Point || vl.first().type()==QVariant::PointF)) {
                        cols=qMax(cols, 2*vl.size());
                    } else {
                        cols=qMax(cols, vl.size());
                    }
                    } break;
                case QVariant::StringList:
                    cols=qMax(cols, v.toStringList().size());
                    break;
                default:
                    break;

            }
        }
        for (int i=0; i<cols; i++) {
            QList<QVariant> newcol;
            for (int r=0; r<data[c].size(); r++) {
                const QVariant& v=data[c].at(r);
                QVariant vo=v;
                switch(v.type()) {
                    case QVariant::Line:
                    case QVariant::LineF:
                        if (i==0) vo=v.toLineF().x1();
                        else if (i==1) vo=v.toLineF().y1();
                        else if (i==2) vo=v.toLineF().x2();
                        else if (i==3) vo=v.toLineF().y2();
                        break;
                    case QVariant::Point:
                    case QVariant::PointF:
                        if (i==0) vo=v.toPointF().x();
                        else if (i==1) vo=v.toPointF().y();
                        break;
                    case QVariant::Size:
                    case QVariant::SizeF:
                        if (i==0) vo=v.toSizeF().width();
                        else if (i==1) vo=v.toSizeF().height();
                        break;
                    case QVariant::Vector2D:
                        if (i==0) vo=v.value<QVector2D>().x();
                        else if (i==1) vo=v.value<QVector2D>().y();
                        break;
                    case QVariant::Vector3D:
                        if (i==0) vo=v.value<QVector3D>().x();
                        else if (i==1) vo=v.value<QVector3D>().y();
                        else if (i==2) vo=v.value<QVector3D>().z();
                        break;
                    case QVariant::Vector4D:
                        if (i==0) vo=v.value<QVector4D>().x();
                        else if (i==1) vo=v.value<QVector4D>().y();
                        else if (i==2) vo=v.value<QVector4D>().z();
                        else if (i==3) vo=v.value<QVector4D>().w();
                        break;
                    case QVariant::QVariant::Matrix4x4: {                            
                            #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                            float* dd=v.value<QMatrix4x4>().data();
                            #else
                            qreal* dd=v.value<QMatrix4x4>().data();
                            #endif
                            if (i<16 && i>=0) vo=dd[i];
                        } break;
                    case QVariant::List: {
                        QVariantList vl=v.toList();
                        if (vl.size()>0 && (vl.first().type()==QVariant::Point || vl.first().type()==QVariant::PointF)) {
                            if (i>=vl.size()) vo=vl.value(i-vl.size(), QVariant()).toPointF().y();
                            else vo=vl.value(i, QVariant()).toPointF().x();
                        } else {
                            vo=v.toList().value(i, QVariant());
                        }
                        } break;
                    case QVariant::StringList:
                        if (i<v.toStringList().size()) vo=v.toStringList().value(i, "");
                        break;
                    default:
                        break;

                }
                newcol<<vo;
            }
            d.append(newcol);
        }
        if (columnsNames) {
            cn<<columnsNames->value(c, "");
            for (int i=0; i<cols-1; i++) {
                cn<<"";
            }
        }
        colid=colid+cols;
    }

    if (columnsNames) *columnsNames=cn;
    data=d;
}

void dataReduce(QList<QList<QVariant> >& data, QStringList* columnsNames) {
    QList<QList<QVariant> > d;
    for (int i=0; i<d.size(); i++) d.append(QList<QVariant>());
    QStringList cn;
    //if (columnsNames) cn = (*columnsNames);
    int colid=0;
    for (int c=0; c<data.size(); c++) {
        int i=0;
        {
            QList<QVariant> newcol;
            for (int r=0; r<data[c].size(); r++) {
                const QVariant& v=data[c].at(r);
                QVariant vo=v;
                switch(v.type()) {
                    case QVariant::Line:
                    case QVariant::LineF:
                        if (i==0) vo=v.toLineF().x1();
                        else if (i==1) vo=v.toLineF().y1();
                        else if (i==2) vo=v.toLineF().x2();
                        else if (i==3) vo=v.toLineF().y2();
                        break;
                    case QVariant::Point:
                    case QVariant::PointF:
                        if (i==0) vo=v.toPointF().x();
                        else if (i==1) vo=v.toPointF().y();
                        break;
                    case QVariant::Size:
                    case QVariant::SizeF:
                        if (i==0) vo=v.toSizeF().width();
                        else if (i==1) vo=v.toSizeF().height();
                        break;
                    case QVariant::Vector2D:
                        if (i==0) vo=v.value<QVector2D>().x();
                        else if (i==1) vo=v.value<QVector2D>().y();
                        break;
                    case QVariant::Vector3D:
                        if (i==0) vo=v.value<QVector3D>().x();
                        else if (i==1) vo=v.value<QVector3D>().y();
                        else if (i==2) vo=v.value<QVector3D>().z();
                        break;
                    case QVariant::Vector4D:
                        if (i==0) vo=v.value<QVector4D>().x();
                        else if (i==1) vo=v.value<QVector4D>().y();
                        else if (i==2) vo=v.value<QVector4D>().z();
                        else if (i==3) vo=v.value<QVector4D>().w();
                        break;
                    case QVariant::QVariant::Matrix4x4: {
                            #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                            float* dd=v.value<QMatrix4x4>().data();
                            #else
                            qreal* dd=v.value<QMatrix4x4>().data();
                            #endif
                            if (i<16 && i>=0) vo=dd[i];
                        } break;
                    case QVariant::List: {
                        QVariantList vl=v.toList();
                        if (vl.size()>0 && (vl.first().type()==QVariant::Point || vl.first().type()==QVariant::PointF)) {
                            if (i>=vl.size()) vo=vl.value(i-vl.size(), QVariant()).toPointF().y();
                            else vo=vl.value(i, QVariant()).toPointF().x();
                        } else {
                            vo=v.toList().value(i, QVariant());
                        }
                        } break;
                    case QVariant::StringList:
                        if (i<v.toStringList().size()) vo=v.toStringList().value(i, "");
                        break;
                    default:
                        break;

                }
                newcol<<vo;
            }
            d.append(newcol);
        }
        if (columnsNames) {
            cn<<columnsNames->value(c, "");

        }
        colid=colid+1;
    }

    if (columnsNames) *columnsNames=cn;
    data=d;
}



QString toSYLK(const QList<QVector<double> >& data, const QStringList& columnNames, const QStringList& rowNames) {
    QString out;
    out+="ID;P\n";

    int offset=0;
    int roffset=0;
    QChar stringDelimiter='"';
    if (rowNames.size()>0) offset=1;
    if (columnNames.size()>0) roffset=1;

    int cols=data.size();
    int rows=0;
    for (int i=0; i<data.size(); i++) {
        rows=qMax(rows, data[i].size());
    }
    if (columnNames.size()>0) {
        for (int i=0; i<cols; i++) {
            QString h=columnNames.value(i, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;X%1;Y%2\nC;Y%1;X%2;K\"%3\"\n").arg(i+offset+1).arg(1).arg(h);
        }
    }

    for (int r=0; r<rows; r++) {
        if (rowNames.size()>0) {
            QString h=rowNames.value(r, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;X%1;Y%2\nC;Y%1;X%2;K\"%3\"\n").arg(1).arg(r+roffset+1).arg(h);
        }
        for (int c=0; c<cols; c++) {
            if (c<data[c].size()) out+=QString("C;X%1;Y%2;K%3\n").arg(c+offset+1).arg(r+roffset+1).arg(doubleToQString(data[c].value(r, 0), 16, 'g', '.'));
        }
    }
    out+="F;C1;SDSR\n";
    out+="F;R1;SDS\n";
    out+="F;R2;SDSB\n";
    out+="E\n";

    return out;
}


QString toSYLK(const QList<QList<QVariant> >& data, const QStringList& columnNames, const QStringList& rowNames) {
    QString out;
    out+="ID;P\n";

    int offset=0;
    int roffset=0;
    QChar stringDelimiter='"';
    if (rowNames.size()>0) offset=1;
    if (columnNames.size()>0) roffset=1;

    int cols=data.size();
    int rows=0;
    for (int i=0; i<data.size(); i++) {
        rows=qMax(rows, data[i].size());
    }
    if (columnNames.size()>0) {
        for (int i=0; i<cols; i++) {
            QString h=columnNames.value(i, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;X%1;Y%2\nC;X%1;Y%2;K\"%3\"\n").arg(i+offset+1).arg(1).arg(h);
        }
    }

    for (int r=0; r<rows; r++) {
        if (rowNames.size()>0) {
            QString h=rowNames.value(r, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;X%1;Y%2\nC;X%1;Y%2;K\"%3\"\n").arg(1).arg(r+roffset+1).arg(h);
        }
        for (int c=0; c<cols; c++) {
            if (c<data[c].size()) {
                const QVariant& v=data[c].at(r);

                switch(v.type()) {
                    case QVariant::Int:
                    case QVariant::UInt:
                    case QVariant::LongLong:
                    case QVariant::ULongLong: {
                            out+=QString("C;X%1;Y%2;K%3\n").arg(c+offset+1).arg(r+roffset+1).arg(v.toString());
                        } break;
                    case QVariant::Double: {
                            out+=QString("C;X%1;Y%2;K%3\n").arg(c+offset+1).arg(r+roffset+1).arg(doubleToQString(v.toDouble(), 16, 'g', '.'));
                        } break;
                    default:
                        if (v.isValid()) {
                            QString h=v.toString();
                            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
                            out+=QString("C;X%1;Y%2;K\"%3\"\n").arg(c+offset+1).arg(r+roffset+1).arg(h);
                        }
                        break;
                }
            }
        }
    }
    out+="F;C1;SDSR\n";
    out+="F;R1;SDS\n";
    out+="F;R2;SDSB\n";
    out+="E\n";

    return out;
}






QString toQFTableModelXML(const QList<QVector<double> >& data, const QStringList& columnsNames, const QStringList& rowNames) {
    return toQFTableModelXML(dataToVariant(data), columnsNames, rowNames);
}


QString toQFTableModelXML(const QList<QList<QVariant> >& data, const QStringList& columnsNames, const QStringList& rowNames) {
    QString res;
    QXmlStreamWriter w(&res);
    int cols=qMax(data.size(), columnsNames.size());
    int rows=0;
    for (int i=0; i<data.size(); i++) {
        rows=qMax(rows, data[i].size());
    }
    int coffset=0;
    if (rowNames.size()>0) coffset=1;
    w.writeStartDocument();
        w.writeStartElement("qfrdrtable");
            w.writeStartElement("state.columns");
            w.writeStartElement("col");
            w.writeAttribute("col", QString::number(0));
            w.writeAttribute("name", QObject::tr("row names"));
            w.writeEndElement();

            for (int i=0; i<cols; i++) {
                w.writeStartElement("col");
                w.writeAttribute("col", QString::number(i+coffset));
                w.writeAttribute("name", columnsNames.value(i));
                w.writeEndElement();
            }
            w.writeEndElement();
            w.writeStartElement("data");
            for (int r=0; r<rowNames.size(); r++) {
                w.writeStartElement("cell");
                w.writeAttribute("row", QString::number(r));
                w.writeAttribute("col", QString::number(0));
                QVariant d=rowNames.value(r, "");
                w.writeAttribute("type", getQVariantType(d));
                w.writeCharacters(getQVariantData(d));
                w.writeEndElement();
            }
            for (int c=0; c<data.size(); c++) {
                for (int r=0; r<data[c].size(); r++) {
                    w.writeStartElement("cell");
                    w.writeAttribute("row", QString::number(r));
                    w.writeAttribute("col", QString::number(coffset+c));
                    QVariant d=data[c].value(r, QVariant());
                    w.writeAttribute("type", getQVariantType(d));
                    w.writeCharacters(getQVariantData(d));
                    w.writeEndElement();
                }
            }
            w.writeEndElement();
        w.writeEndElement();
    w.writeEndDocument();

    //qDebug()<<res;
    return res;
}






QStringList QFDataExportHandler::getFormats()  {
    QStringList sl;
    sl<<QObject::tr("Comma Separated Values (*.csv *.dat)");
    sl<<QObject::tr("Comma Separated Values, flipped (*.csv *.dat)");
    sl<<QObject::tr("Semicolon Separated Values (*.dat *.txt *.csv)");
    sl<<QObject::tr("Semicolon Separated Values, flipped (*.dat *.txt *.csv)");
    sl<<QObject::tr("Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv)");
    sl<<QObject::tr("Semicolon Separated Values [for german Excel], flipped (*.dat *.txt *.csv)");
    sl<<QObject::tr("Tab separated (*.dat *.txt *.tsv)");
    sl<<QObject::tr("Tab separated, flipped (*.dat *.txt *.tsv)");
    sl<<QObject::tr("SYLK dataformat (*.slk *.sylk)");
    sl<<QObject::tr("SYLK dataformat, flipped (*.slk *.sylk)");
    sl<<QObject::tr("Matlab script (*.m)");
    sl<<QObject::tr("Matlab script, flipped (*.m)");
    sl<<QObject::tr("QuickFit table XML (*.qftxml)");
    sl<<QObject::tr("QuickFit table XML, flipped (*.qftxml)");
    for (int i=0; i<writers.size(); i++) {
        sl<<writers[i]->getFilter();
    }
    return sl;
}



QList<QFDataExportHandler::DataWriter*> QFDataExportHandler::writers=QList<QFDataExportHandler::DataWriter*>();

void QFDataExportHandler::registerDataWriter(QFDataExportHandler::DataWriter *writer)
{
    writers.append(writer);
}

void QFDataExportHandler::deregisterDataWriter(QFDataExportHandler::DataWriter *writer)
{
    if (writers.contains(writer)) writers.removeAll(writer);
}

void QFDataExportHandler::save(const QList<QVector<double> >& data, int format, const QString& filename, const QStringList& columnHeaders, const QStringList& rowHeaders) {

    int f=0;
    if (format==f++) { // CSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else  if (format==f++) { // CSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV,
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV,
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // TSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==f++) { // TSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==f++) { // SYLK
        saveStringToFile(filename, toSYLK(data, columnHeaders, rowHeaders));
    } else if (format==f++) { // SYLK flipped
        saveStringToFile(filename, toSYLK(dataRotate(data), rowHeaders, columnHeaders));
    } else if (format==f++) { // Matlab
        saveStringToFile(filename, toMatlab(data, false));
    } else if (format==f++) { // Matlab
        saveStringToFile(filename, toMatlab(dataRotate(data), false));
    } else if (format==f++) { // QFTable XML
        saveStringToFile(filename, toQFTableModelXML(data, columnHeaders, rowHeaders), QString("UTF-8"));
    } else if (format==f++) { // QFTable XML, flipped
        saveStringToFile(filename, toQFTableModelXML(dataRotate(data), rowHeaders, columnHeaders), QString("UTF-8"));
    } else if (format-f>=0 && format-f<writers.size()) {
        writers[format-f]->save(data, filename, columnHeaders, rowHeaders);
    }
}

void QFDataExportHandler::save(const QList<QVector<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    QString lastDir=ProgramOptions::getConfigValue("QFDataExportHandler/saveLastDir", "").toString();
    QString lastFilter=ProgramOptions::getConfigValue("QFDataExportHandler/savelastFilter", getFormats().first()).toString();
    QString filename=qfGetSaveFileName(NULL, QObject::tr("select filename ..."), lastDir, getFormats().join(";;"), &lastFilter);
    if (filename.size()>0) {
        int format=getFormats().indexOf(lastFilter);
        save(data, format, filename, columnHeaders, rowHeaders);
        ProgramOptions::setConfigValue("QFDataExportHandler/saveLastDir", lastDir);
        ProgramOptions::setConfigValue("QFDataExportHandler/savelastFilter", lastFilter);
    }

}

void QFDataExportHandler::copyCSV(const QList<QList<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders, false);
}

void QFDataExportHandler::copyCSV(const QList<QVector<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders, false);
}

void QFDataExportHandler::save(const QList<QList<QVariant> >& data, int format, const QString& filename, const QStringList& columnHeaders, const QStringList& rowHeaders) {

    int f=0;
    if (format==f++) { // CSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else  if (format==f++) { // CSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV,
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // SSV,
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==f++) { // TSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==f++) { // TSV
        saveStringToFile(filename, toCSV(dataRotate(data), rowHeaders, columnHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==f++) { // SYLK
        saveStringToFile(filename, toSYLK(data, columnHeaders, rowHeaders));
    } else if (format==f++) { // SYLK flipped
        saveStringToFile(filename, toSYLK(dataRotate(data), rowHeaders, columnHeaders));
    } else if (format==f++) { // Matlab
        saveStringToFile(filename, toMatlab(data, false));
    } else if (format==f++) { // Matlab
        saveStringToFile(filename, toMatlab(dataRotate(data), false));
    } else if (format==f++) { // QFTable XML
        saveStringToFile(filename, toQFTableModelXML(data, columnHeaders, rowHeaders), QString("UTF-8"));
    } else if (format==f++) { // QFTable XML, flipped
        saveStringToFile(filename, toQFTableModelXML(dataRotate(data), rowHeaders, columnHeaders), QString("UTF-8"));
    } else if (format-f>=0 && format-f<writers.size()) {
        writers[format-f]->save(data, filename, columnHeaders, rowHeaders);
    }
}

void QFDataExportHandler::save(const QList<QList<QVariant> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    QString lastDir=ProgramOptions::getConfigValue("QFDataExportHandler/saveLastDir", "").toString();
    QString lastFilter=ProgramOptions::getConfigValue("QFDataExportHandler/savelastFilter", getFormats().first()).toString();
    QString filename=qfGetSaveFileName(NULL, QObject::tr("select filename ..."), lastDir, getFormats().join(";;"), &lastFilter);
    if (filename.size()>0) {
        int format=getFormats().indexOf(lastFilter);
        save(data, format, filename, columnHeaders, rowHeaders);
        ProgramOptions::setConfigValue("QFDataExportHandler/saveLastDir", lastDir);
        ProgramOptions::setConfigValue("QFDataExportHandler/savelastFilter", lastFilter);
    }
}

void QFDataExportHandler::copyCSV(const QList<QList<QVariant> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders,false);
}

void QFDataExportHandler::copyExcel(const QList<QList<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders, true);
}

void QFDataExportHandler::copyExcel(const QList<QVector<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders, true);
}

void QFDataExportHandler::copyExcel(const QList<QList<QVariant> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders, true);
}

void QFDataExportHandler::copyMatlab(const QList<QList<QVariant> > &data)
{
    matlabCopy(data);
}

void QFDataExportHandler::copyMatlab(const QList<QList<double> > &data)
{
    matlabCopy(data);
}

void QFDataExportHandler::copyMatlab(const QList<QVector<double> > &data)
{
    matlabCopy(data);
}

QList<QList<QVariant> > dataToVariant(const QList<QList<double> >& data) {
    QList<QList<QVariant> > res;
    for (int i=0; i<data.size(); i++) {
        res<<QList<QVariant>();
        for (int j=0; j<data[i].size(); j++) {
            res.last()<<data[i].at(j);
        }
    }
    return res;
}

QList<QList<QVariant> > dataToVariant(const QList<QVector<double> >& data) {
    QList<QList<QVariant> > res;
    for (int i=0; i<data.size(); i++) {
        res<<QList<QVariant>();
        for (int j=0; j<data[i].size(); j++) {
            res.last()<<data[i].at(j);
        }
    }
    return res;
}

QList<QVector<double> > dataReduceToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames) {
    QList<QVector<double> > res;
    QList<QList<QVariant> > in=data;
    dataReduce(in, columnsNames);
    for (int i=0; i<in.size(); i++) {
        res<<QVector<double>();
        for (int j=0; j<in[i].size(); j++) {
            bool ok=false;
            double d=in[i].at(j).toDouble(&ok);
            if (ok) {
                res.last()<<d;
            } else {
                res.last()<<NAN;
            }
        }
    }
    return res;
}

QList<QVector<double> > dataExpandToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames) {
    QList<QVector<double> > res;
    QList<QList<QVariant> > in=data;
    dataExpand(in, columnsNames);
    for (int i=0; i<in.size(); i++) {
        res<<QVector<double>();
        for (int j=0; j<in[i].size(); j++) {
            bool ok=false;
            double d=in[i].at(j).toDouble(&ok);
            if (ok) {
                res.last()<<d;
            } else {
                res.last()<<NAN;
            }
        }
    }
    return res;
}


QList<QVector<double> > dataToDouble(const QList<QList<QVariant> >& data, QStringList* columnsNames) {
    QList<QVector<double> > res;
    QList<QList<QVariant> > in=data;
    for (int i=0; i<in.size(); i++) {
        res<<QVector<double>();
        for (int j=0; j<in[i].size(); j++) {
            bool ok=false;
            double d=in[i].at(j).toDouble(&ok);
            if (ok) {
                res.last()<<d;
            } else {
                res.last()<<NAN;
            }
        }
    }
    return res;
}


void QFDataExportHandler::DataWriter::save(const QList<QVector<double> > &data, const QString &filename, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    save(dataToVariant(data), filename, columnHeaders, rowHeaders);
}

void QFDataExportHandler::DataWriter::save(const QList<QList<QVariant> > &data, const QString &filename, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    save(dataExpandToDouble(data), filename, columnHeaders, rowHeaders);
}


int dataGetRows(const QList<QVector<double> >& data) {
    int r=0;
    for (int i=0; i<data.size(); i++) {
        r=qMax(r, data[i].size());
    }
    return r;
}

int dataGetRows(const QList<QList<double> >& data) {
    int r=0;
    for (int i=0; i<data.size(); i++) {
        r=qMax(r, data[i].size());
    }
    return r;
}


int dataGetRows(const QList<QList<QVariant> >& data) {
    int r=0;
    for (int i=0; i<data.size(); i++) {
        r=qMax(r, data[i].size());
    }
    return r;
}


void QFDataExportTool::save(const QString &filename, int format) const
{
    QFDataExportHandler::save(data, format, filename, colHeaders, rowHeaders);
}

void QFDataExportTool::clear()
{
    data.clear();
    colHeaders.clear();
    rowHeaders.clear();
}

void QFDataExportTool::set(int col, int row, const QVariant &value)
{
    if (col<0 || row<0) return;
    QList<QVariant> dempty;
    int rows=qMax(row+1,getRowCount());
    for (int r=0; r<rows; r++) {
        dempty<<QVariant();
    }

    while (col>=data.size()) {
        data.append(dempty);
    }
    int cols=getColCount();
    for (int c=0; c<cols; c++) {
        while (data[c].size()<rows) {
            data[c].append(QVariant());
        }
    }
    data[col].operator [](row)=value;
}


void QFDataExportTool::setLastColumn(int row, const QVariant &value)
{
    set(getColCount()-1, row, value);
}

void QFDataExportTool::setRowTitle(int row, const QString &title)
{
    if (row<0) return;
    while (row>=rowHeaders.size()) {
        rowHeaders.append(QString());
    }
    rowHeaders[row]=title;
    int rows=getRowCount();
    int cols=getColCount();
    for (int c=0; c<cols; c++) {
        while (data[c].size()<rows) {
            data[c].append(QVariant());
        }
    }
}

void QFDataExportTool::setColTitle(int col, const QString &title)
{
    if (col<0) return;
    while (col>=colHeaders.size()) {
        colHeaders.append(QString());
    }
    colHeaders[col]=title;
    QList<QVariant> dempty;
    int rows=getRowCount();
    for (int r=0; r<rows; r++) {
        dempty<<QVariant();
    }

    while (col>=data.size()) {
        data.append(dempty);
    }
    int cols=getColCount();
    for (int c=0; c<cols; c++) {
        while (data[c].size()<rows) {
            data[c].append(QVariant());
        }
    }
}

int QFDataExportTool::getRowCount() const
{
    return dataGetRows(data);
}

int QFDataExportTool::getColCount() const
{
    return data.size();
}
