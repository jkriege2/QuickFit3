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

#ifndef QFRAWDATARESULTSCOMBOBOX_H
#define QFRAWDATARESULTSCOMBOBOX_H

#include <QComboBox>
#include "qfrawdatarecord.h"
#include "lib_imexport.h"


/*! \brief combobox that displays a list of all evaluation results in a specified QFRawDataRecord and result group (see e.g. QFResultsGroupComboBox)
    \ingroup qf3lib_tools

    Basically this combobox is filled by a call to QFRawDataRecord::resultsCalcNamesEvalIDsAndLabels().

  */
class QFLIB_EXPORT QFRawDataResultsComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRawDataResultsComboBox(QWidget *parent = 0);
        QString currentResult() const;
        QString currentEvaluationID() const;
        QString evaluationGroup() const;
        void setResultGroupFilters(QStringList& resultGroupFilters);
        QFRawDataRecord* getRDR() const;
    signals:
        void currentResultChanged(QString result, QString evalID);
    public slots:
        void setRDR(QFRawDataRecord *rdr);
        void setEvaluationGroup(const QString& group);
        void setCurrentResult(const QString& result);
        void setEvaluationIDFilter(const QString& evalID);
    protected slots:
        void myCurrentIndexChanged(int i);
        void refill();

    protected:
        QPointer<QFRawDataRecord> rdr;
        QString egroup;
        QString evaluationIDFilter;
        QStringList resultGroupFilters;
        QMap<int, QString> evalIDs;
};

#endif // QFRAWDATARESULTSCOMBOBOX_H
