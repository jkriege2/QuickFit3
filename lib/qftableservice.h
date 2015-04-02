/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#ifndef QFTABLESERVICE_H
#define QFTABLESERVICE_H



#include "lib_imexport.h"
#include <QString>
#include <QSettings>
#include <QToolBar>
#include <QMenu>



/*! \brief wrapper class that allows plugins to access basic histogramming services
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFTableService {
    public:
        QFTableService();
        virtual ~QFTableService();


        struct TableColumn {
            QList<QVariant> data;
            inline void setDoubleVec(const QVector<double>& data) {
                this->data.clear();
                for (int i=0; i<data.size(); i++)
                    this->data<<data[i];
            }

            QString name;
        };


        virtual QWidget* getCreateTableView(const QString& name, const QString& title)=0;
        virtual void clearTableView(const QString& name)=0;
        virtual void addColumnToTableView(const QString& name, const TableColumn& histogram)=0;

        static QFTableService* getInstance() {
            return instance;
        }

    private:
        static QFTableService* instance;
};

#endif // QFTABLESERVICE_H
