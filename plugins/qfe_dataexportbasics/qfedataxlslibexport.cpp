/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfedataxlslibexport.h"
#include <QObject>
#include "xlslib.h"
#include <QVariant>
#include <QLine>
#include <QLineF>
#include <QSize>
#include <QSizeF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>


QFEDataXlsLibExport::QFEDataXlsLibExport(bool flipped)
{
    this->flipped=flipped;
}

QString QFEDataXlsLibExport::getFilter() const
{
    if (flipped) return QObject::tr("Excel 97 Worksheet, flipped (*.xls)");
    else return QObject::tr("Excel 97 Worksheet (*.xls)");
}

void QFEDataXlsLibExport::save(const QList<QList<QVariant> > &data_in, const QString &filename, const QStringList &columnHeaders_in, const QStringList &rowHeaders_in)
{
    QList<QList<QVariant> > data;
    QStringList columnHeaders=columnHeaders_in;
    QStringList rowHeaders=rowHeaders_in;
    if (flipped) {
        data=dataRotate(data_in);
        columnHeaders=rowHeaders_in;
        rowHeaders=columnHeaders_in;
    } else {
        data=data_in;
    }
    // Get the workbook handler
    xlslib_core::workbook wb;
    xlslib_core::worksheet* sh = wb.sheet("data");

    int roffset=1;
    int coffset=1;


    if (columnHeaders.size()>0) {
        for (int i=0; i<columnHeaders.size(); i++) {
            sh->label(1, coffset+i, columnHeaders[i].toStdString());
        }
        roffset+=1;
    }
    if (rowHeaders.size()>0) {
        for (int i=0; i<rowHeaders.size(); i++) {
            sh->label(roffset+i, 1, rowHeaders[i].toStdString());
        }
        coffset+=1;
    }

    for (int c=0; c<data.size(); c++) {
        for (int r=0; r<data[c].size(); r++) {
            const QVariant& v=data[c].at(r);
            if (v.isValid()) {
                switch(v.type()) {
                    case QVariant::Line:
                    case QVariant::LineF:
                        sh->number(roffset+r, coffset+c, v.toLineF().x1());
                        break;
                    case QVariant::Point:
                    case QVariant::PointF:
                        sh->number(roffset+r, coffset+c, v.toPointF().x());
                        break;
                    case QVariant::Size:
                    case QVariant::SizeF:
                        sh->number(roffset+r, coffset+c, v.toSizeF().width());

                        break;
                    case QVariant::Vector2D:
                        sh->number(roffset+r, coffset+c, v.value<QVector2D>().x());
                        break;
                    case QVariant::Vector3D:
                        sh->number(roffset+r, coffset+c, v.value<QVector3D>().x());
                        break;
                    case QVariant::Vector4D:
                        sh->number(roffset+r, coffset+c, v.value<QVector4D>().x());
                        break;
                    case QVariant::QVariant::Matrix4x4: {
                            qreal* dd=v.value<QMatrix4x4>().data();
                            sh->number(roffset+r, coffset+c,dd[0]);
                        } break;
                    case QVariant::Int:
                    case QVariant::UInt:
                    case QVariant::LongLong:
                    case QVariant::ULongLong:
                    case QVariant::Double:  {
                             sh->number(roffset+r, coffset+c, v.toDouble());
                        } break;
                    case QVariant::Bool:{
                            sh->boolean(roffset+r, coffset+c, v.toBool());
                       } break;


                    default: {
                            sh->label(roffset+r, coffset+c, v.toString().toStdString());
                        } break;

                }
            }
        }
    }

    wb.Dump(filename.toStdString());

}
