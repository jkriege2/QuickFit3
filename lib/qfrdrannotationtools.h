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

#ifndef QFRDRANNOTATIONTOOLS_H
#define QFRDRANNOTATIONTOOLS_H

#include "qfrawdatarecord.h"
#include "qfrdrannotationinterface.h"
#include <QPointer>
#include "lib_imexport.h"

class QFLIB_EXPORT QFRDRAnnotationTools : public QFRDRAnnotationInterface
{
    public:
        QFRDRAnnotationTools(QFRawDataRecord* rdr=NULL, bool saveAsVectors=true);
        virtual ~QFRDRAnnotationTools();

        virtual void annotSetRDR(QFRawDataRecord* rdr);
        int annotAdd(QFRDRAnnotationTypes type, const QString& label, int index);
        int annotAdd(QFRDRAnnotationTypes type, const QString& label, double x, double y=0, double z=0);
        int annotAdd(QFRDRAnnotationTypes type, const QString& label);
        void annotAddValue(int annotation, int index, double data);
        virtual void annotSetLabel(int annotation, const QString& label);
        virtual void annotSetComment(int annotation, const QString& comment);
        virtual QString annotGetComment(int annotation) const;

        int annotGetCount() const;
        QString annotGetLabel(int annotation) const;
        int annotGetIndex(int annotation) const;
        QFRDRAnnotationTypes annotGetType(int annotation) const;
        QVector<double> annotGetValues(int annotation) const;
        double annotGetValue(int annotation, int index, double defaultVal=0) const;
        QVector<bool> annotGetValuesSet(int annotation) const;
        bool annotIsValueSet(int annotation, int index) const;
        QVector<double> annotGetPositionsX(int annotation) const;
        QVector<double> annotGetPositionsY(int annotation) const;
        QVector<double> annotGetPositionsZ(int annotation) const;

    protected:
        QPointer<QFRawDataRecord> idxannot_rdr;
        QString idxannot_evalid;
        bool idxannot_saveAsVectors;
};

#endif // QFRDRANNOTATIONTOOLS_H
