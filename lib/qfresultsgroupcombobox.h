/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#ifndef QFRESULTSGROUPCOMBOBOX_H
#define QFRESULTSGROUPCOMBOBOX_H

#include <QComboBox>
#include "qfrawdatarecord.h"
#include "lib_imexport.h"
#include <QRegExp>


/*! \brief combobox that displays a list of all revaluation groups of a specified QFRawDataRecord
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFResultsGroupComboBox : public QComboBox {
        Q_OBJECT
    public:
        explicit QFResultsGroupComboBox(QWidget *parent = 0);

        QString currentEvaluationGroup() const;
        void setGroupFilter(bool enabled, const QRegExp& filter=QRegExp());
        void setContainedParam(const QString& param);
    public slots:
        void setCurrentEvaluationGroup(const QString& group);
        void setRDR(QFRawDataRecord *rdr);
    signals:
        void currentEvaluationGroupChanged(QString egroup);
    protected slots:
        void myCurrentIndexChanged(int i);
        void refill();
    protected:
        QPointer<QFRawDataRecord> rdr;
        QStringList egroups;
        QStringList egnames;
        QRegExp rxFilter;
        bool doFilter;
        QString containsParamInGroup;
};

#endif // QFRESULTSGROUPCOMBOBOX_H
