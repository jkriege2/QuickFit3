/*
Copyright (c) 2015
	
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


#include "qfrdrcurve_data.h"
#include <QtXml>

QFRDRCurveData::QFRDRCurveData(QFProject* parent):
    QFRawDataRecord(parent)
{
}

QFRDRCurveData::~QFRDRCurveData()
{
    clearCurves();
}

void QFRDRCurveData::exportData(const QString& format, const QString& filename)const  {
	// here you may export the data of the record into the specified format (see getExportFiletypes() )
    // THIS IS OPTIONAL
}

int QFRDRCurveData::curvesGetCount() const
{
    return curves.size();

}

QString QFRDRCurveData::curvesGetName(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].name;
    }
    return QString();
}

QVector<double> QFRDRCurveData::curvesGetX(int index) const
{
    if (index>=0 && index<curves.size()) {
        int c=curves[index].xcolumn;
        const QFTableModel* m=curves[index].table;
        if (m && c>=0 && c<m->columnCount()) {
            return m->getColumnDataAsNumbers(c);
        }
    }

    return QVector<double>();
}

QVector<double> QFRDRCurveData::curvesGetXError(int index) const
{
    if (index>=0 && index<curves.size()) {
        int c=curves[index].xerrorcolumn;
        const QFTableModel* m=curves[index].table;
        if (m && c>=0 && c<m->columnCount()) {
            return m->getColumnDataAsNumbers(c);
        }
    }
    return QVector<double>();
}

QVector<double> QFRDRCurveData::curvesGetY(int index) const
{
    if (index>=0 && index<curves.size()) {
        int c=curves[index].ycolumn;
        const QFTableModel* m=curves[index].table;
        if (m && c>=0 && c<m->columnCount()) {
            return m->getColumnDataAsNumbers(c);
        }
    }
    return QVector<double>();
}

QVector<double> QFRDRCurveData::curvesGetYError(int index) const
{
    if (index>=0 && index<curves.size()) {
        int c=curves[index].yerrorcolumn;
        const QFTableModel* m=curves[index].table;
        if (m && c>=0 && c<m->columnCount()) {
            return m->getColumnDataAsNumbers(c);
        }
    }
    return QVector<double>();
}

bool QFRDRCurveData::curvesGetLogX(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].logX;
    }
    return false;
}

bool QFRDRCurveData::curvesGetLogY(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].logY;
    }
    return false;
}

QFRDRCurvesInterface::CurveType QFRDRCurveData::curvesGetType(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].type;
    }
    return QFRDRCurvesInterface::ctPoints;
}

QString QFRDRCurveData::curvesGetXLabel(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].xlabel;
    }
    return QString();
}

QString QFRDRCurveData::curvesGetYLabel(int index) const
{
    if (index>=0 && index<curves.size()) {
        return curves[index].ylabel;
    }
    return QString();
}

void QFRDRCurveData::curvesSetName(int index, const QString &name)
{
    if (index>=0 && index<curves.size() && curves[index].name!=name) {
        curves[index].name=name;
        emit rawDataChanged();
    }
}

void QFRDRCurveData::curvesSetLogX(int index, bool l)
{
    if (index>=0 && index<curves.size() && curves[index].logX!=l) {
        curves[index].logX=l;
        emit rawDataChanged();
    }

}

void QFRDRCurveData::curvesSetLogY(int index, bool l)
{
    if (index>=0 && index<curves.size() && curves[index].logY!=l) {
        curves[index].logY=l;
        emit rawDataChanged();
    }

}

void QFRDRCurveData::curvesSetType(int index, QFRDRCurvesInterface::CurveType type)
{
    if (index>=0 && index<curves.size() && curves[index].type!=type) {
        curves[index].type=type;
        emit rawDataChanged();
    }

}

void QFRDRCurveData::curvesSetXLabel(int index, const QString &label)
{
    if (index>=0 && index<curves.size() && curves[index].xlabel!=label) {
        curves[index].xlabel=label;
        emit rawDataChanged();
    }

}

void QFRDRCurveData::curvesSetYLabel(int index, const QString &label)
{
    if (index>=0 && index<curves.size() && curves[index].ylabel!=label) {
        curves[index].ylabel=label;
        emit rawDataChanged();
    }

}

#define COLLECTSTRINGCURVEPROPS(prop,pname) { \
    QStringList lst; \
    for (int i=0; i<curves.size(); i++) {\
        lst<<curves[i].prop;\
    } \
    setQFProperty(pname,lst,false,false);\
}
#define COLLECTINTCURVEPROPS(prop,pname) { \
    QStringList lst; \
    for (int i=0; i<curves.size(); i++) {\
        lst<<QString::number(curves[i].prop);\
    } \
    setQFProperty(pname,lst.join(","),false,false);\
}
#define COLLECTBOOLCURVEPROPS(prop,pname) { \
    QStringList lst; \
    for (int i=0; i<curves.size(); i++) {\
        lst<<(curves[i].prop?"true":"false");\
    } \
    setQFProperty(pname,lst.join(","),false,false);\
}
#define COLLECTCURVETYPECURVEPROPS(prop,pname) { \
    QStringList lst; \
    for (int i=0; i<curves.size(); i++) {\
        lst<<QFRDRCurvesInterfaceCurveTypeToString(curves[i].prop);\
    } \
    setQFProperty(pname,lst,false,false);\
}

void QFRDRCurveData::intWriteData(QXmlStreamWriter& /*w*/) {
    COLLECTSTRINGCURVEPROPS(name, "curvename");
    COLLECTSTRINGCURVEPROPS(xlabel, "xlabel");
    COLLECTSTRINGCURVEPROPS(ylabel, "ylabel");
    COLLECTCURVETYPECURVEPROPS(type, "curvetype");
    COLLECTINTCURVEPROPS(xcolumn, "column_x");
    COLLECTINTCURVEPROPS(ycolumn, "column_y");
    COLLECTINTCURVEPROPS(xerrorcolumn, "column_xerr");
    COLLECTINTCURVEPROPS(yerrorcolumn, "column_yerr");
}

void QFRDRCurveData::intReadData(QDomElement* e) {
    // free any old memory
    clearCurves();


    // read the datatype
    QString datatype=getQFProperty("FILETYPE", "CSV_INTERNAL").toString().toUpper().trimmed();

    // read curves parameters
    char colsep=qfGetFirstChar(getQFProperty("csv_column_separator", ",").toString(), ',').toLatin1();
    char decsep=qfGetFirstChar(getQFProperty("decimal_separator", ".").toString(), '.').toLatin1();
    char commentstart=qfGetFirstChar(getQFProperty("comment_start", "#").toString(), '#').toLatin1();
    QString headstart=getQFProperty("header_start", ",").toString();
    QList<int> colXL=stringToIntList(getQFProperty("column_x", "0").toString(), 0);
    QList<int> colYL=stringToIntList(getQFProperty("column_y", "1").toString(), 1);
    QList<int> colXErrL=stringToIntList(getQFProperty("column_xerr", "-1").toString(), -1);
    QList<int> colYErrL=stringToIntList(getQFProperty("column_yerr", "-1").toString(), -1);

    QList<bool> logXL=stringToBoolList(getQFProperty("logx", "false").toString(), false);
    QList<bool> logYL=stringToBoolList(getQFProperty("logy", "false").toString(), false);
    QStringList curveTypeL=getQFProperty("curvetype", QStringList("points")).toStringList();
    QStringList nameL=getQFProperty("curvename").toStringList();
    QStringList xlabelL=getQFProperty("xlabel", QStringList()).toStringList();
    QStringList ylabelL=getQFProperty("ylabel", QStringList()).toStringList();

    if (datatype=="CSV") {
        QStringList files=getFiles();

        if (files.size()==1 && (colXL.size()>1 || colYL.size()>1)) {
            QFTableModel* table=new QFTableModel();
            table->setReadonly(false);
            if (table->readCSV(files[0], colsep, decsep, headstart, commentstart)) {
                table->setReadonly(true);
                for (int i=0; i<colXL.size(); i++) {
                    CurveData cd;
                    cd.table=table;
                    cd.xcolumn=colXL.value(i,-1);
                    cd.ycolumn=colYL.value(i,-1);
                    cd.xerrorcolumn=colXErrL.value(i,-1);
                    cd.yerrorcolumn=colYErrL.value(i,-1);
                    cd.logX=logXL.value(i, false);
                    cd.logY=logYL.value(i, false);
                    cd.xlabel=xlabelL.value(i, xlabelL.value(0, cd.table->getColumnTitle(cd.xcolumn)));
                    cd.ylabel=xlabelL.value(i, ylabelL.value(0, cd.table->getColumnTitle(cd.ycolumn)));
                    cd.name=nameL.value(i, tr("%1 - columns %2/%4 vs. %3/%5").arg(QFileInfo(files[0]).fileName()).arg(cd.xcolumn).arg(cd.ycolumn).arg(cd.xerrorcolumn).arg(cd.yerrorcolumn));
                    cd.type=QFRDRCurvesInterfaceStringToCurveType(curveTypeL.value(i, curveTypeL.value(0,"points")));
                    curves.append(cd);
                }
            } else {
                delete table;
                setError(tr("Error while reading data from CSV file '%1':\n   ERROR PARSING FILE!").arg(files[0]));
                return ;
            }
        } else if (files.size()>=1){
            for (int i=0; i<files.size(); i++) {
                CurveData cd;
                cd.table=new QFTableModel();
                cd.table->setReadonly(false);
                if (cd.table->readCSV(files[i], colsep, decsep, headstart, commentstart)) {
                    cd.table->setReadonly(true);
                    cd.xcolumn=colXL.value(i,colXL.value(0,-1));
                    cd.ycolumn=colYL.value(i,colYL.value(0,-1));
                    cd.xerrorcolumn=colXErrL.value(i,colXErrL.value(0,-1));
                    cd.yerrorcolumn=colYErrL.value(i,colYErrL.value(0,-1));
                    cd.logX=logXL.value(i, logXL.value(0, false));
                    cd.logY=logYL.value(i, logYL.value(0, false));
                    cd.name=nameL.value(i, nameL.value(0, tr("%1 - columns %2/%4 vs. %3/%5").arg(QFileInfo(files[i]).fileName()).arg(cd.xcolumn).arg(cd.ycolumn).arg(cd.xerrorcolumn).arg(cd.yerrorcolumn)));
                    cd.xlabel=xlabelL.value(i, xlabelL.value(0, cd.table->getColumnTitle(cd.xcolumn)));
                    cd.ylabel=xlabelL.value(i, ylabelL.value(0, cd.table->getColumnTitle(cd.ycolumn)));
                    cd.type=QFRDRCurvesInterfaceStringToCurveType(curveTypeL.value(i, curveTypeL.value(0, "points")));
                    curves.append(cd);
                } else {
                    delete cd.table;
                    clearCurves();
                    setError(tr("Error while reading data from CSV file '%1':\n   ERROR PARSING FILE!").arg(files[i]));
                    return ;
                }
            }
        } else {
            setError(tr("Error while reading data for RDR '%1':\n   NO FILES ARE ASSOCIATEDC TO THIS RDR!").arg(getName()));
            return ;
        }
    } else if (datatype=="CSV_INTERNAL") {
        QString csv;
        if (e) {
            QDomElement te=e->firstChildElement("internal_csv");
            csv=te.text();
        }
        if (csv.isEmpty()) {
            csv=getQFProperty("CSV", "").toString();
            if (propertyExists("CSV")) {
                setQFProperty("CSV", getQFProperty("CSV"), false, false);
            }
        }
        if (csv.size()>0) {
            QFTableModel* table=new QFTableModel();
            table->setReadonly(false);
            if (table->fromCSV(csv, colsep, decsep, headstart, commentstart,0,0,true)) {
                table->setReadonly(true);
                for (int i=0; i<colXL.size(); i++) {
                    CurveData cd;
                    cd.table=table;
                    cd.xcolumn=colXL.value(i,-1);
                    cd.ycolumn=colYL.value(i,-1);
                    cd.xerrorcolumn=colXErrL.value(i,-1);
                    cd.yerrorcolumn=colYErrL.value(i,-1);
                    cd.logX=logXL.value(i, false);
                    cd.logY=logYL.value(i, false);
                    cd.xlabel=xlabelL.value(i, xlabelL.value(0, cd.table->getColumnTitle(cd.xcolumn)));
                    cd.ylabel=xlabelL.value(i, ylabelL.value(0, cd.table->getColumnTitle(cd.ycolumn)));
                    cd.name=nameL.value(i, tr("columns %1/%3 vs. %2/%4").arg(cd.xcolumn).arg(cd.ycolumn).arg(cd.xerrorcolumn).arg(cd.yerrorcolumn));
                    cd.type=QFRDRCurvesInterfaceStringToCurveType(curveTypeL.value(i, curveTypeL.value(0,"points")));
                    curves.append(cd);
                }
            } else {
                delete table;
                setError(tr("Error while reading internal CSV data for RDR '%1':\n   ERROR PARSING DATA!").arg(getName()));
                return ;
            }
        } else {
            setError(tr("Error while reading internal CSV data for RDR '%1':\n   NO DATA PROVIDED IN PROJECT!").arg(getName()));
            return;
        }
    } else {
        setError(tr("Error while reading data for RDR '%1':\n   UNKNOWN DATATYPE '%2'!").arg(getName()).arg(datatype));
        return;
    }

}



void QFRDRCurveData::clearCurves()
{
    for (int i=0; i<curves.size(); i++) {
        if (curves[i].table) delete curves[i].table;
    }
    curves.clear();
}

