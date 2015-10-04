/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
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
#include "xlsxwriter.h"
#include <QVariant>
#include <QLine>
#include <QLineF>
#include <QSize>
#include <QSizeF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>


QFEDataXlsxLibExport::QFEDataXlsxLibExport(bool flipped)
{
    this->flipped=flipped;
}

QString QFEDataXlsxLibExport::getFilter() const
{
    if (flipped) return QObject::tr("Microsoft Excel(r) XLSX Worksheet, flipped (*.xlsx)");
    else return QObject::tr("Microsoft Excel(r) XLSX Worksheet (*.xlsx)");
}

void QFEDataXlsxLibExport::save(const QList<QList<QVariant> > &data_in, const QString &filename, const QStringList &columnHeaders_in, const QStringList &rowHeaders_in)
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

    char fns[4096];
    strcpy(fns, filename.toStdString().c_str());

    lxw_workbook  *workbook  = new_workbook(fns);
    //qDebug()<<filename.toLocal8Bit()<<workbook<<workbook->filename;
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
    //qDebug()<<worksheet;
    //qDebug()<<"2 workbook->filename: "<<workbook->filename;

    /* Add a format. */
    lxw_format *format = workbook_add_format(workbook);
    //qDebug()<<"3 workbook->filename: "<<workbook->filename;

    /* Set the bold property for the format */
    format_set_bold(format);
    format_set_bg_color(format, 0xC0C0C0);

    int roffset=0;
    int coffset=0;


    if (columnHeaders.size()>0) {
        for (int i=0; i<columnHeaders.size(); i++) {
            worksheet_write_string(worksheet, 0, coffset+i, columnHeaders[i].toStdString().c_str(), format);
        }
        roffset+=1;
    }
    //qDebug()<<"4 workbook->filename: "<<workbook->filename;
    if (rowHeaders.size()>0) {
        for (int i=0; i<rowHeaders.size(); i++) {
            worksheet_write_string(worksheet, roffset+i, 0, rowHeaders[i].toStdString().c_str(), format);
        }
        coffset+=1;
    }
    //qDebug()<<"5 workbook->filename: "<<workbook->filename;

    for (int c=0; c<data.size(); c++) {
        for (int r=0; r<data[c].size(); r++) {
            const QVariant& v=data[c].at(r);
            if (v.isValid()) {
                switch(v.type()) {
                    case QVariant::Line:
                    case QVariant::LineF:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.toLineF().x1(), NULL);
                        break;
                    case QVariant::Point:
                    case QVariant::PointF:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.toPointF().x(), NULL);
                        break;
                    case QVariant::Size:
                    case QVariant::SizeF:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.toSizeF().width(), NULL);

                        break;
                    case QVariant::Vector2D:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.value<QVector2D>().x(), NULL);
                        break;
                    case QVariant::Vector3D:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.value<QVector3D>().x(), NULL);
                        break;
                    case QVariant::Vector4D:
                        worksheet_write_number(worksheet, roffset+r, coffset+c, v.value<QVector4D>().x(), NULL);
                        break;
                    case QVariant::QVariant::Matrix4x4: {
                            #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                            float* dd=v.value<QMatrix4x4>().data();
                            #else
                            qreal* dd=v.value<QMatrix4x4>().data();
                            #endif

                            worksheet_write_number(worksheet, roffset+r, coffset+c,dd[0], NULL);
                        } break;
                    case QVariant::Int:
                    case QVariant::UInt:
                    case QVariant::LongLong:
                    case QVariant::ULongLong:
                    case QVariant::Double:  {
                             worksheet_write_number(worksheet, roffset+r, coffset+c, v.toDouble(), NULL);
                        } break;
                    case QVariant::Bool:{
                            worksheet_write_number(worksheet, roffset+r, coffset+c, v.toBool()?1:0, NULL);
                            //sh->boolean(roffset+r, coffset+c, v.toBool());
                       } break;


                    default: {
                            worksheet_write_string(worksheet, roffset+r, coffset+c, v.toString().toStdString().c_str(), NULL);
                        } break;

                }
            }
        }
        //qDebug()<<"6/"<<c<<" workbook->filename: "<<workbook->filename;

    }
    //qDebug()<<"7 workbook->filename: "<<workbook->filename;

    //qDebug()<<workbook_close(workbook);

}
