/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "matlabtools.h"
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QChar>
#include "qftools.h"
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include "datatools.h"
#include "tinymatwriter.h"


QString toMatlabScript(const QList<QList<double> >& data, bool noVarDef) {

    QString matlab="M = [";
    if (noVarDef) matlab="[";
    int datas=0;
    for (int r=0; r<data.size(); r++) {
        if (data[r].size()>datas) datas=data[r].size();
    }

    for (int r=0; r<datas; r++) {
        for (int c=0; c<data.size(); c++) {
            if (c>0) {
                matlab+=", ";
            }
            if (r<data[c].size()) {
                double d=data[c].at(r);

                matlab+=CDoubleToQString(d);
            }
        }
        if (r<datas-1) matlab+=";   ";
    }



    if (noVarDef) matlab+="]";
    else matlab+="];";
    return matlab;
}

QString toMatlabScript(const QList<QVector<double> >& data, bool noVarDef) {

    QString matlab="M = [";
    if (noVarDef) matlab="[";
    int datas=0;
    for (int r=0; r<data.size(); r++) {
        if (data[r].size()>datas) datas=data[r].size();
    }

    for (int r=0; r<datas; r++) {
        for (int c=0; c<data.size(); c++) {
            if (c>0) {
                matlab+=", ";
            }
            if (r<data[c].size()) {
                double d=data[c].at(r);

                matlab+=CDoubleToQString(d);
            }
        }
        if (r<datas-1) matlab+=";   ";
    }



    if (noVarDef) matlab+="]";
    else matlab+="];";
    return matlab;
}

void matlabCopyScript(const QList<QList<double> >& data) {

    QString matlab=toMatlabScript(data, false);
    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(matlab);
    clipboard->setMimeData(mime);
}


QString toMatlabScript(const QList<QList<QVariant> >& data, bool noVarDef) {
    bool isNumberOnly=true;
    for (int i=0; i<data.size(); i++) {
        for (int j=0; j<data[i].size(); j++) {
            switch (data[i].at(j).type()) {
                case QVariant::Double:
                case QVariant::Int:
                case QVariant::UInt:
                case QVariant::LongLong:
                case QVariant::ULongLong:
                    break;
                default:
                    isNumberOnly=false;
                    break;
            }
            if (!isNumberOnly) break;
        }
        if (!isNumberOnly) break;
    }
    if (isNumberOnly) {
        QList<QList<double> > d;
        for (int i=0; i<data.size(); i++) {
            QList<double> dd;
            for (int j=0; j<data[i].size(); j++) {
                dd.append(data[i].at(j).toDouble());
            }
            d.append(dd);
        }
        return toMatlabScript(d, noVarDef);
    }

    QString matlab="C = {";
    if (noVarDef) matlab="{";
    int datas=0;
    for (int r=0; r<data.size(); r++) {
        if (data[r].size()>datas) datas=data[r].size();
    }

    for (int r=0; r<datas; r++) {
        for (int c=0; c<data.size(); c++) {
            if (c>0) {
                matlab+=", ";
            }
            if (r<data[c].size()) {
                QVariant d=data[c].at(r);
                QString s=d.toString();
                s=s.replace('\'', "\'\'");
                if (!d.isValid()) {
                    matlab+="[]";
                } else {
                    switch (d.type()) {
                        case QVariant::Double:
                        case QVariant::Int:
                        case QVariant::UInt:
                        case QVariant::LongLong:
                        case QVariant::ULongLong:
                            matlab+=CDoubleToQString(d.toDouble());
                            break;
                        case QVariant::Bool:
                            if(d.toBool()) {
                                matlab+="true";
                            } else {
                                matlab+="false";
                            }
                            break;
                        default:{
                                bool ok=true;
                                if (d.type()==QVariant::List) {
                                    QList<QList<QVariant> > dd;
                                    dd.append(d.toList());
                                    matlab+=toMatlabScript(dd, true);
                                } else if (d.type()==QVariant::Point) {
                                    matlab+=QString("[%1, %2]").arg(CDoubleToQString(d.toPoint().x())).arg(CDoubleToQString(d.toPoint().y()));
                                } else if (d.type()==QVariant::PointF) {
                                    matlab+=QString("[%1, %2]").arg(CDoubleToQString(d.toPointF().x())).arg(CDoubleToQString(d.toPointF().y()));
                                } else if (d.type()==QVariant::Size) {
                                    matlab+=QString("[%1, %2]").arg(CDoubleToQString(d.toSize().width())).arg(CDoubleToQString(d.toSize().height()));
                                } else if (d.type()==QVariant::SizeF) {
                                    matlab+=QString("[%1, %2]").arg(CDoubleToQString(d.toSizeF().width())).arg(CDoubleToQString(d.toSizeF().height()));
                                } else {
                                    ok=false;
                                }
                                if (!ok) {
                                    matlab+=QString("'%1'").arg(s);
                                }
                            }
                            break;
                    }
                }


            }
        }
        if (r<datas-1) matlab+=";   ";
    }




    if (noVarDef) matlab+="}";
    else matlab+="};";
    return matlab;
}


void matlabCopyScript(const QList<QList<QVariant> >& data) {
    QString matlab=toMatlabScript(data, false);

    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(matlab);
    clipboard->setMimeData(mime);
}


void matlabCopyScript(const QList<QVector<double> >& data) {
    QList<QList<double> > d;
    for (int i=0; i<data.size(); i++) {
        d.append(data[i].toList());
    }
    matlabCopyScript(d);
}

void matlabCopyScript(const QList<QVector<QVariant> >& data) {
    QList<QList<QVariant> > d;
    for (int i=0; i<data.size(); i++) {
        d.append(data[i].toList());
    }
    matlabCopyScript(d);
}

void saveToMatlabMATfile(const QString&filename, const QList<QList<QVariant> >& data, const QString& varname,  QStringList columnsNames,  QStringList rowNames, bool convertToNumberMatrix) {
    TinyMATWriterFile* mat=TinyMATWriter_open(filename.toLocal8Bit().data());
    if (mat) {
        if (convertToNumberMatrix) {
            QList<QVector<double> > datad=dataExpandToDouble(data, &columnsNames);
            double* d=doubleDataToDoubleMatrix(datad);
            TinyMATWriter_writeMatrix2D_rowmajor(mat, varname.toLocal8Bit().data(), d, datad.size(), dataGetRows(datad));
            if (d) qfFree(d);
        } else {
            if (mayLossLesslyDoDoubleDataToDouble(data)) {
                QList<QVector<double> > datad=dataToDouble(data, &columnsNames);
                double* d=doubleDataToDoubleMatrix(datad);
                TinyMATWriter_writeMatrix2D_rowmajor(mat, varname.toLocal8Bit().data(), d, datad.size(), dataGetRows(datad));
                if (d) qfFree(d);
            } else {
                TinyMATWriter_writeQVariantMatrix_listofcols(mat, varname.toLocal8Bit().data(), data);
            }
        }
        TinyMATWriter_writeQStringList(mat, "columnNames", columnsNames);
        TinyMATWriter_writeQStringList(mat, "rowNames", rowNames);
        TinyMATWriter_close(mat);
    }
}


void saveToMatlabMATfile(const QString&filename, const QList<QVector<double> >& datad, const QString& varname,  QStringList columnsNames,  QStringList rowNames) {
    TinyMATWriterFile* mat=TinyMATWriter_open(filename.toLocal8Bit().data());
    if (mat) {
        double* d=doubleDataToDoubleMatrix(datad);
        TinyMATWriter_writeMatrix2D_rowmajor(mat, varname.toLocal8Bit().data(), d, datad.size(), dataGetRows(datad));
        if (d) qfFree(d);
        TinyMATWriter_writeQStringList(mat, "columnNames", columnsNames);
        TinyMATWriter_writeQStringList(mat, "rowNames", rowNames);
        TinyMATWriter_close(mat);
    }
}
