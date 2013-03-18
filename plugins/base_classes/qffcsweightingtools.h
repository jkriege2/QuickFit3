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

            DataWeightingMax=2
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
class QFFCDWeightingCombobox: public QComboBox {
        Q_OBJECT
    public:
        QFFCDWeightingCombobox(QWidget *parent);

        QFFCSWeightingTools::DataWeight currentWeight() const;
        void setCurrentWeight(QFFCSWeightingTools::DataWeight weight);

        static QFFCSWeightingTools::DataWeight indexToWeight(int index);
    signals:
        void currentWeightChanged(QFFCSWeightingTools::DataWeight weight);
    private slots:
        void currentIdxChanged(int idx);
};

#endif // QFFCSWEIGHTINGTOOLS_H
