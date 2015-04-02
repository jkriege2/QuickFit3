/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFPARAMETERCORRELATIONSERVICE_H
#define QFPARAMETERCORRELATIONSERVICE_H





#include "lib_imexport.h"
#include <QString>
#include <QSettings>
#include <QToolBar>
#include <QMenu>

/*! \brief wrapper class that allows plugins to access basic parameter correlation services
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFParameterCorrelationService
{
    public:
        QFParameterCorrelationService();
        virtual ~QFParameterCorrelationService();

        struct ParameterCorrelation {
            QVector<double> dataX;
            QVector<double> dataY;
            QString nameX;
            QString nameY;
        };

        virtual QWidget* getCreateParameterCorrelationView(const QString& name, const QString& title)=0;
        virtual void clearParameterCorrelationView(const QString& name)=0;
        virtual void addCorrelationToView(const QString& name, const ParameterCorrelation& data)=0;

        static QFParameterCorrelationService* getParameterCorrelationInstance() {
            return instance;
        }

    private:
        static QFParameterCorrelationService* instance;
};

#endif // QFPARAMETERCORRELATIONSERVICE_H
