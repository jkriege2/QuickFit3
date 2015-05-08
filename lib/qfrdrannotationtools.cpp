/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-04-02 13:55:22 +0200 (Do, 02 Apr 2015) $  (revision $Rev: 3902 $)

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

#include "qfrdrannotationtools.h"
#include "qftools.h"

QFRDRAnnotationTools::QFRDRAnnotationTools(QFRawDataRecord *rdr, bool saveAsVectors)
{
    idxannot_rdr=rdr;
    this->idxannot_saveAsVectors=saveAsVectors;
    idxannot_evalid="RDRIndexAnnotation_%1";
}

QFRDRAnnotationTools::~QFRDRAnnotationTools()
{

}

void QFRDRAnnotationTools::annotDelete(int i)
{
    int annotation=annotGetCount();

    if (idxannot_rdr && i>=0 && i<annotation) {
        if (i+1<annotation) {
            for (int j=i+1; j<annotation; j++) {
                idxannot_rdr->resultsCopy(idxannot_evalid.arg(j-1), idxannot_evalid.arg(j));
            }
        }
        idxannot_rdr->resultsClear(idxannot_evalid.arg(annotation-1));
        idxannot_rdr->resultsSetInteger("annotations", "annotation_count", annotation-1);

    }
}

void QFRDRAnnotationTools::annotDeleteAll()
{
    int annotation=annotGetCount();

    if (idxannot_rdr) {
        for (int j=0; j<annotation; j++) {
            idxannot_rdr->resultsClear(idxannot_evalid.arg(j));
        }

        idxannot_rdr->resultsSetInteger("annotations", "annotation_count", 0);

    }
}

void QFRDRAnnotationTools::annotSetRDR(QFRawDataRecord *rdr)
{
    idxannot_rdr=rdr;
}

int QFRDRAnnotationTools::annotAdd(QFRDRAnnotationInterface::QFRDRAnnotationTypes type, const QString &label, int index)
{
    int annotation=annotGetCount();

    if (idxannot_rdr) {
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("type"), QFRDRAnnotationTypesToString(type));
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("label"), label);
        idxannot_rdr->resultsSetInteger(idxannot_evalid.arg(annotation), QString("index"), index);
        idxannot_rdr->resultsSetInteger("annotations", "annotation_count", annotation+1);
    }

    return annotation;
}

int QFRDRAnnotationTools::annotAdd(QFRDRAnnotationInterface::QFRDRAnnotationTypes type, const QString &label)
{
    int annotation=annotGetCount();

    if (idxannot_rdr) {
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("type"), QFRDRAnnotationTypesToString(type));
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("label"), label);
        idxannot_rdr->resultsSetInteger("annotations", "annotation_count", annotation+1);
    }

    return annotation;
}

int QFRDRAnnotationTools::annotAdd(QFRDRAnnotationInterface::QFRDRAnnotationTypes type, const QString &label, double x, double y, double z)
{
    int annotation=annotGetCount();

    if (idxannot_rdr) {
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("type"), QFRDRAnnotationTypesToString(type));
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("label"), label);
        idxannot_rdr->resultsSetNumberList(idxannot_evalid.arg(annotation), QString("position_x"), constructQVectorFromItems(x));
        idxannot_rdr->resultsSetNumberList(idxannot_evalid.arg(annotation), QString("position_y"), constructQVectorFromItems(y));
        idxannot_rdr->resultsSetNumberList(idxannot_evalid.arg(annotation), QString("position_z"), constructQVectorFromItems(z));
        idxannot_rdr->resultsSetInteger("annotations", "annotation_count", annotation+1);
    }

    return annotation;}

void QFRDRAnnotationTools::annotAddValue(int annotation, int index, double data)
{
    if (idxannot_rdr && annotation>=0 && index>=0) {
        idxannot_rdr->resultsSetInNumberListAndBool(idxannot_evalid.arg(annotation), QString("value"), index, data, "", QString("value_set"), true);
    }
}

void QFRDRAnnotationTools::annotSetLabel(int annotation, const QString &label)
{
    if (idxannot_rdr && annotation>=0) {
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("label"), label);
    }
}

void QFRDRAnnotationTools::annotSetComment(int annotation, const QString &comment)
{
    if (idxannot_rdr && annotation>=0) {
        idxannot_rdr->resultsSetString(idxannot_evalid.arg(annotation), QString("comment"), comment);
    }

}

QString QFRDRAnnotationTools::annotGetComment(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsString(idxannot_evalid.arg(annotation), QString("comment"));
    }
    return QString();
}

int QFRDRAnnotationTools::annotGetCount() const
{
    if (idxannot_rdr) {
        bool ok=false;
        int p= idxannot_rdr->resultsGetAsInteger("annotations", "annotation_count", &ok);
        if (ok) return p;
    }
    return 0;
}

QString QFRDRAnnotationTools::annotGetLabel(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsString(idxannot_evalid.arg(annotation), QString("label"));
    }
    return QString();
}

int QFRDRAnnotationTools::annotGetIndex(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        bool ok=false;
        int p= idxannot_rdr->resultsGetAsInteger(idxannot_evalid.arg(annotation), QString("index"), &ok);
        if (ok) return p;
    }
    return -1;
}

QFRDRAnnotationInterface::QFRDRAnnotationTypes QFRDRAnnotationTools::annotGetType(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return StringToQFRDRAnnotationTypes(idxannot_rdr->resultsGetAsString(idxannot_evalid.arg(annotation), QString("type")));
    }
    return annotNone;
}

QVector<double> QFRDRAnnotationTools::annotGetValues(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsDoubleList(idxannot_evalid.arg(annotation), QString("value"));
    }
    return QVector<double>();
}

double QFRDRAnnotationTools::annotGetValue(int annotation, int index, double defaultVal) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetInNumberList(idxannot_evalid.arg(annotation), QString("value"), index, defaultVal);
    }
    return defaultVal;
}

QVector<bool> QFRDRAnnotationTools::annotGetValuesSet(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsBooleanList(idxannot_evalid.arg(annotation), QString("value_set"));
    }
    return QVector<bool>();
}

bool QFRDRAnnotationTools::annotIsValueSet(int annotation, int index) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetInBooleanList(idxannot_evalid.arg(annotation), QString("value_set"), index, false);
    }
    return false;
}

QVector<double> QFRDRAnnotationTools::annotGetPositionsX(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsDoubleList(idxannot_evalid.arg(annotation), QString("position_x"));
    }
    return QVector<double>();
}

QVector<double> QFRDRAnnotationTools::annotGetPositionsY(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsDoubleList(idxannot_evalid.arg(annotation), QString("position_y"));
    }
    return QVector<double>();
}

QVector<double> QFRDRAnnotationTools::annotGetPositionsZ(int annotation) const
{
    if (idxannot_rdr && annotation>=0) {
        return idxannot_rdr->resultsGetAsDoubleList(idxannot_evalid.arg(annotation), QString("position_z"));
    }
    return QVector<double>();
}

