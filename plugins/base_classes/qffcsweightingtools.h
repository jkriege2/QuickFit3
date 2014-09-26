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

#ifndef QFFCSWEIGHTINGTOOLS_H
#define QFFCSWEIGHTINGTOOLS_H

#include "qfrawdatarecord.h"
#include <QWidget>
#include <QComboBox>


/*! \brief tool class that implements tools to set/get data weighting. It also implements a function that returns a new vector of weights
    \ingroup qf3evaluationplugins


*/
class QFFCSWeightingTools
{
    public:
        /** \brief which data weighting should be applied */
        enum DataWeight {
            EqualWeighting=0,
            StdDevWeighting=1,
            RunErrorWeighting=2,
            RunningStdDev3Weight=3,
            RunningStdDev5Weight=4,
            RunningStdDev7Weight=5,
            RunningStdDev11Weight=6,
            Poly2Weight=7,
            Poly3Weight=8,
            Poly5Weight=9,

            DataWeightingMax=9
        };

        QFFCSWeightingTools();
        virtual ~QFFCSWeightingTools();

        /** \brief set the current fitting algorithm */
        virtual void setFitDataWeighting(DataWeight weighting) ;
        /** \brief set the current fitting algorithm */
        virtual void setFitDataWeighting(int weighting) ;
        /** \brief get the current fitting algorithm */
        virtual DataWeight getFitDataWeighting() const ;

        static QString dataWeightToString(DataWeight weight);
        static QString dataWeightToName(DataWeight weight, const QString& runname=QString("run"));
        static DataWeight stringToDataWeight(QString weight);

        /** \brief allocate an array for the weights (using calloc(), so use free() to delete the array) and fill
         *         it with the appropriate values, according to the current settings */
        virtual double* allocWeights(bool* weightsOK, QFRawDataRecord* record_in, int run_in, int data_start=-1, int data_end=-1) const;

        static DataWeight indexToWeight(int index);
    protected:
        /** \brief type of data weighting */
        DataWeight m_weighting;

};



/*! \brief a combobox to select a data weighting as defined in QFFCSWeightingTools::DataWeight
    \ingroup qf3evaluationplugins

*/
class QFFCSWeightingCombobox: public QComboBox {
        Q_OBJECT
    public:
        QFFCSWeightingCombobox(QWidget *parent);

        QFFCSWeightingTools::DataWeight currentWeight() const;
        void setCurrentWeight(QFFCSWeightingTools::DataWeight weight);

        static QFFCSWeightingTools::DataWeight indexToWeight(int index);
    signals:
        void currentWeightChanged(QFFCSWeightingTools::DataWeight weight);
    private slots:
        void currentIdxChanged(int idx);
};

#endif // QFFCSWEIGHTINGTOOLS_H
