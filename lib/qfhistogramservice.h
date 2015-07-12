/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFHISTOGRAMSERVICE_H
#define QFHISTOGRAMSERVICE_H





#include "lib_imexport.h"
#include <QString>
#include <QSettings>
#include <QToolBar>
#include <QMenu>



/*! \brief wrapper class that allows plugins to access basic histogramming services
    \ingroup qf3plugintools
*/
class QFLIB_EXPORT QFHistogramService {
    public:
        QFHistogramService();
        virtual ~QFHistogramService();

        struct Histogram {
            QVector<double> data;
            QString name;
        };


        virtual QWidget* getCreateView(const QString& name, const QString& title)=0;
        virtual void clearView(const QString& name)=0;
        virtual void addHistogramToView(const QString& name, const Histogram& histogram)=0;

        static QFHistogramService* getInstance() {
            return instance;
        }

    private:
        static QFHistogramService* instance;
};



#endif // QFHISTOGRAMSERVICE_H
