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


#ifndef QFRDRTABLEINDEXEDSTATDIALOG_H
#define QFRDRTABLEINDEXEDSTATDIALOG_H

#include "qfdialog.h"
#include "qfproperties.h"
#include <QMap>
#include <QList>
#include <QStringList>
#include "qftablepluginmodel.h"
#include "qcheckablestringlistmodel.h"
#include "qfrdrtable.h"
#include "programoptions.h"

namespace Ui {
    class QFRDRTableIndexedStatDialog;
}

class QFRDRTableIndexedStatDialog : public QFDialog
{
        Q_OBJECT

    public:
        struct DataProps {
            inline explicit DataProps() {
                cmean=-1;
                cstd=-1;
                cmin=-1;
                cq25=-1;
                cmedian=-1;
                cq75=-1;
                cmax=-1;
                cnmad=-1;
                cid=-1;
            }

            inline bool dataForBoxplotAvailable() const {
                return ((cmean>=0)||(cmedian>=0))&&(cq25>=0)&&(cq75>=0);
            }
            inline bool dataForMeanStdAvailable() const {
                return (cmean>=0)&&(cstd>=0);
            }
            inline bool dataForMedianNMADAvailable() const {
                return (cmedian>=0)&&(cnmad>=0);
            }

            int cmean;
            int cstd;
            int cmin;
            int cq25;
            int cmedian;
            int cq75;
            int cmax;
            int cnmad;
            int cid;
            QString dataname;
        };

        explicit QFRDRTableIndexedStatDialog(QWidget *parent = 0);
        ~QFRDRTableIndexedStatDialog();
        void setTable(QFRDRTable *table);
        void setSelectedColumns(const QList<int>& cols);

        int indexColumn() const;
        bool avg() const;
        bool std() const;
        bool median() const;
        bool q25() const;
        bool q75() const;
        bool min() const;
        bool max() const;
        bool nmad() const;

        int getResultStartColumn() const;
        int addToGraph() const;
        bool addToExistingGraph() const;
        bool addNewGraphs() const;
        bool createGraphs() const;
        QList<int> getDataColumns() const;

        QString getDataColumnsExpression() const;
        QStringList getExpressions(QStringList &names, QString &addToPre, QList<DataProps>& props) const;

    private:
        Ui::QFRDRTableIndexedStatDialog *ui;
        QFTablePluginModel* table;
        QFRDRTable *plugin;
        QCheckableStringListModel cols;
};

#endif // QFRDRTABLEINDEXEDSTATDIALOG_H
