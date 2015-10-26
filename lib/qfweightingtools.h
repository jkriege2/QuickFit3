/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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
#include "lib_imexport.h"


/*! \brief tool class that implements tools to set/get data weighting. It also implements a function that returns a new vector of weights
    \ingroup qf3evaluationplugins


*/
class QFLIB_EXPORT QFCurveWeightingTools
{
    public:
        /** \brief which data weighting should be applied */
        enum DataWeight {
            EqualWeighting=0,
            RunErrorWeighting=1,
            RunningStdDev3Weight=2,
            RunningStdDev5Weight=3,
            RunningStdDev7Weight=4,
            RunningStdDev11Weight=5,
            Poly2Weight=6,
            Poly3Weight=7,
            Poly5Weight=8,

            DataWeightingMax=9
        };

        QFCurveWeightingTools();
        virtual ~QFCurveWeightingTools();

        /** \brief set the current fitting algorithm */
        virtual void setFitDataWeighting(DataWeight weighting) ;
        /** \brief set the current fitting algorithm */
        virtual void setFitDataWeighting(int weighting) ;
        /** \brief get the current fitting algorithm */
        virtual DataWeight getFitDataWeighting() const ;
        /** \brief returns \c true, if all weights are equal, i.e. no weighting is used */
        virtual bool isEqualWeights() const;

        static QString dataWeightToString(DataWeight weight);
        static QString dataWeightToName(DataWeight weight, const QString& runname=QString("run"));
        static DataWeight stringToDataWeight(QString weight);

        /** \brief allocate an array for the weights (using qfCalloc(), so use qfFree() to delete the array) and fill
         *         it with the appropriate values, according to the current settings */
        virtual double* allocWeights(bool* weightsOK, const QFRawDataRecord* record_in, int run_in, bool returnEmptyForNoWeights=false) const;

        /** \brief allocate an array for the weights (using qfCalloc(), so use qfFree() to delete the array) and fill
         *         it with the appropriate values, according to the current settings */
        virtual QVector<double> allocVecWeights(bool* weightsOK, const QFRawDataRecord* record_in, int run_in=-1, bool returnEmptyForNoWeights=false) const;

        static DataWeight indexToWeight(int index);
    protected:
        /** \brief type of data weighting */
        DataWeight m_weighting;

};



/*! \brief a combobox to select a data weighting as defined in QFFCSWeightingTools::DataWeight
    \ingroup qf3evaluationplugins

*/
class QFLIB_EXPORT QFCurveWeightingCombobox: public QComboBox {
        Q_OBJECT
    public:
        QFCurveWeightingCombobox(QWidget *parent);

        QFCurveWeightingTools::DataWeight currentWeight() const;
        void setCurrentWeight(QFCurveWeightingTools::DataWeight weight);

        static QFCurveWeightingTools::DataWeight indexToWeight(int index);
    signals:
        void currentWeightChanged(QFCurveWeightingTools::DataWeight weight);
    private slots:
        void currentIdxChanged(int idx);
};

#endif // QFFCSWEIGHTINGTOOLS_H
