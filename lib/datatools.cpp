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
                QVariant vo=QVariant();
                const QVariant& v=data[c].at(r);
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
                            qreal* dd=v.value<QMatrix4x4>().data();
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
                QVariant vo=QVariant();
                const QVariant& v=data[c].at(r);
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
                            qreal* dd=v.value<QMatrix4x4>().data();
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
            out+=QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(i+offset+1).arg(1).arg(h);
        }
    }

    for (int r=0; r<rows; r++) {
        if (rowNames.size()>0) {
            QString h=rowNames.value(r, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(1).arg(r+roffset+1).arg(h);
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
            out+=QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(i+offset+1).arg(1).arg(h);
        }
    }

    for (int r=0; r<rows; r++) {
        if (rowNames.size()>0) {
            QString h=rowNames.value(r, "");
            h=h.replace(',', "_").replace(';', "_").replace('\t', " ").replace('\n', "\\n").replace('\r', "\\r").replace(stringDelimiter, "_");
            out+=QString("F;SDS;Y%1;X%2\nC;Y%1;X%2;K\"%3\"\n").arg(1).arg(r+roffset+1).arg(h);
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

void QFDataExportHandler::save(const QList<QVector<double> >& data, int format, const QString& filename, const QStringList& columnHeaders, const QStringList& rowHeaders) {

    if (format==0) { // CSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else  if (format==1) { // CSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else if (format==2) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==3) { // SSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==4) { // SSV,
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==5) { // SSV,
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==6) { // TSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==7) { // TSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==8) { // SYLK
        saveStringToFile(filename, toSYLK(data, columnHeaders, rowHeaders));
    } else if (format==9) { // SYLK flipped
        saveStringToFile(filename, toSYLK(dataRotate(data), columnHeaders, rowHeaders));
    } else if (format==10) { // Matlab
        saveStringToFile(filename, toMatlab(data, false));
    } else if (format==11) { // Matlab
        saveStringToFile(filename, toMatlab(dataRotate(data), false));
    } else if (format-12>=0 && format-12<writers.size()) {
        writers[format-12]->save(data, filename, columnHeaders, rowHeaders);
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
    csvCopy(data, columnHeaders, rowHeaders);
}

void QFDataExportHandler::save(const QList<QList<QVariant> >& data, int format, const QString& filename, const QStringList& columnHeaders, const QStringList& rowHeaders) {

    if (format==0) { // CSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else  if (format==1) { // CSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else if (format==2) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==3) { // SSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==4) { // SSV,
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==5) { // SSV,
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==6) { // TSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==7) { // TSV
        saveStringToFile(filename, toCSV(dataRotate(data), columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==8) { // SYLK
        saveStringToFile(filename, toSYLK(data, columnHeaders, rowHeaders));
    } else if (format==9) { // SYLK flipped
        saveStringToFile(filename, toSYLK(dataRotate(data), columnHeaders, rowHeaders));
    } else if (format==10) { // Matlab
        saveStringToFile(filename, toMatlab(data, false));
    } else if (format==11) { // Matlab
        saveStringToFile(filename, toMatlab(dataRotate(data), false));
    } else if (format-12>=0 && format-12<writers.size()) {
        writers[format-12]->save(data, filename, columnHeaders, rowHeaders);
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
    csvCopy(data, columnHeaders, rowHeaders);
}

void QFDataExportHandler::copyMatlab(const QList<QList<QVariant> > &data)
{
    matlabCopy(data);
}

void QFDataExportHandler::copyMatlab(const QList<QList<double> > &data)
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
