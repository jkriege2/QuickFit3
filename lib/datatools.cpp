#include "datatools.h"
#include "qftools.h"

QList<QVector<double> > dataDataRotate(const QList<QVector<double> >& data) {
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

QList<QList<double> > dataDataRotate(const QList<QList<double> >& data) {
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

QStringList QFDataExportHandler::getFormats()  {
    QStringList sl;
    sl<<QObject::tr("Comma Separated Values (*.csv *.dat)");
    sl<<QObject::tr("Semicolon Separated Values (*.dat *.txt *.csv)");
    sl<<QObject::tr("Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv)");
    sl<<QObject::tr("SYLK dataformat (*.slk *.sylk)");
    sl<<QObject::tr("SYLK dataformat, flipped (*.slk *.sylk)");
    sl<<QObject::tr("Tab separated (*.dat *.txt *.tsv)");
    sl<<QObject::tr("Matlab script (*.m)");
    return sl;
}

void QFDataExportHandler::save(const QList<QVector<double> >& data, int format, const QString& filename, const QStringList& columnHeaders, const QStringList& rowHeaders) {

    if (format==0) { // CSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', ", ", true, '\"', "#! ", 15));
    } else if (format==1) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "; ", true, '\"', "#! ", 15));
    } else if (format==2) { // SSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, ',', "; ", true, '\"', "#! ", 15));
    } else if (format==3) { // SYLK
        saveStringToFile(filename, toSYLK(data, columnHeaders, rowHeaders));
    } else if (format==4) { // SYLK flipped
        saveStringToFile(filename, toSYLK(dataDataRotate(data), columnHeaders, rowHeaders));
    } else if (format==5) { // TSV
        saveStringToFile(filename, toCSV(data, columnHeaders, rowHeaders, '.', "\t", true, '\"', "#! ", 15));
    } else if (format==6) { // Matlab
        saveStringToFile(filename, toMatlab(data, false));
    }
}

void QFDataExportHandler::copyCSV(const QList<QList<double> > &data, const QStringList &columnHeaders, const QStringList &rowHeaders)
{
    csvCopy(data, columnHeaders, rowHeaders);
}
