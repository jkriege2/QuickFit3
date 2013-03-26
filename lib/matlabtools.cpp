#include "matlabtools.h"
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QChar>
#include "qftools.h"
#include <QApplication>
#include <QMimeData>
#include <QClipboard>


QString toMatlab(const QList<QList<double> >& data, bool noVarDef) {

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

void matlabCopy(const QList<QList<double> >& data) {

    QString matlab=toMatlab(data, false);
    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(matlab);
    clipboard->setMimeData(mime);
}


QString toMatlab(const QList<QList<QVariant> >& data, bool noVarDef) {
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
        return toMatlab(d, noVarDef);
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
                                    matlab+=toMatlab(dd, true);
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


void matlabCopy(const QList<QList<QVariant> >& data) {
    QString matlab=toMatlab(data, false);

    QClipboard *clipboard = QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(matlab);
    clipboard->setMimeData(mime);
}


void matlabCopy(const QList<QVector<double> >& data) {
    QList<QList<double> > d;
    for (int i=0; i<data.size(); i++) {
        d.append(data[i].toList());
    }
    matlabCopy(d);
}

void matlabCopy(const QList<QVector<QVariant> >& data) {
    QList<QList<QVariant> > d;
    for (int i=0; i<data.size(); i++) {
        d.append(data[i].toList());
    }
    matlabCopy(d);
}
