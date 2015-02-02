/*
    Copyright (c) 2008-2014 Nikolas Schnellbächer &
                            Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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


#ifndef QFFCSMAXENTEVALUATIONITEM_H
#define QFFCSMAXENTEVALUATIONITEM_H

#include <QMap>
#include <QSettings>
#include "qftools.h"
#include "qfmathtools.h"
#include "qffcsweightingtools.h"
#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "../base_classes/qfusesresultsbyindexevaluation.h"
#include "../base_classes/qfusesresultsbyindexandmodelevaluation.h"

/*! \brief evaluation item class 
    \ingroup qf3evaldp_fcs_maxent


    
    
*/
class QFFCSMaxEntEvaluationItem : public QFUsesResultsByIndexAndModelEvaluation, public QFSimpleFitParameterEnumeratorInterface, public QFFCSWeightingTools {
        Q_OBJECT
        Q_INTERFACES(QFSimpleFitParameterEnumeratorInterface)
    public:
        /** \brief which data weighting should be applied */
        /** Default constructor */
        QFFCSMaxEntEvaluationItem(QFProject* parent);
        /** Default destructor */
        virtual ~QFFCSMaxEntEvaluationItem();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("fcs_maxent"); }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS MaxEnt Evaluation"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/fcs_maxent/qffcsmaxentevaluation_small.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("maximum entropy data evaluation plugin for FCS data"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/fcs_maxent/qffcsmaxentevaluation.png"); }
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Evaluation"); }
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);

        /** \brief do the evaluation */
        virtual void doFit(QFRawDataRecord* record, int index, int model, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, int runAvgWidth=11, int residualHistogramBins=25);

        virtual int getIndexMin(QFRawDataRecord* r) const;
        virtual int getIndexMax(QFRawDataRecord* r) const;
        virtual int getModelCount(QFRawDataRecord* r, int index) const;

        /** \brief set the current alpha value */
        void setAlpha(double alpha);
        /** \brief return the current alpha value */
        double getAlpha() const;
        /** \brief set the number of distribution points Ndist */
        void setNdist(uint32_t Ndist);

        void setTauMode(int Ndist);
        void setTauMin(double tauMin);
        void setTauMax(double tauMax);
        int getTauMode();
        double getTauMin();
        double getTauMax();

        /** \brief return the current number of distribution points Ndist */
        uint32_t getNdist() const;
        /** \brief set the number of distribution points Ndist */
        void setNumIter(uint32_t NumIter);
        /** \brief return the current number of distribution points Ndist */
        uint32_t getNumIter() const;

        /** \brief set the current wxy value in nanometers */
        void setWXY(double wxy);
        /** \brief return the current wxy value in nanometers */
        double getWXY() const;
        double getWXY(QFRawDataRecord* r, int index, int model) const;

        void setLambda(double val);
        double getLambda() const;
        double getLambda(QFRawDataRecord* r, int index, int model) const;
        void setTheta(double val);
        double getTheta() const;
        double getTheta(QFRawDataRecord* r, int index, int model) const;
        void setRefIndx(double val);
        double getRefIndx() const;
        double getRefIndx(QFRawDataRecord* r, int index, int model) const;
        /** \brief DLS q-value
         *
         *  \f[ q=\frac{4\pi n}{\lambda}\Cdot\sin(\Theta/2) \f]
         *  returned in units of 1/micron!!!
         */
        double getDLSQ() const;
        double getDLSQ(QFRawDataRecord* r, int index, int model) const;



        /** \brief return the name for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterName(int model, int id, bool html=false) const;
        /** \brief return the unit for the given parameter in the given model in either HTML richtext or plain text */
        virtual QString getParameterUnit(int model, int id, bool html) const;
        /** \brief return the number of parameters in the given model */
        virtual int getParameterCount(int model) const;
        /** \brief return the ID for the given parameter in the given model  */
        virtual QString getParameterID(int model, int param) const;

        /** \brief return the name for current model */
        QString getCurrentModelName() const;
        /** \brief return the name for given model */
        virtual QString getModelName(int model) const;

        /*! \brief evaluate the model for a given record and index

            this function evaluates the model \f$ f(\tau) \f$ at the given positions \a taus. The results are written into \a output.
            Both arrays \a taus and \a output have to have the size \a N . The model is evaluated for a given maxent distribution \f$ p(T) = (T_i, p_{T_i}=p_i)_{i=1..N_D} \f$
            stored in \a distribution_tau and  \a distribution which both habe \a distributionN elements.

            For \c model==0 this function outputs this model:
              \f[ g(\tau)=\frac{1}{N}\cdot\left(\frac{1-\theta+\theta\cdot\exp(-\tau/\tau_T)}{1-\theta}\right)\cdot\sum\limits_{i=1}^{N_D}p_{T_i}\cdot\left(1+\frac{\tau}{T_i}\right)^{-1}\cdot\left(1+\frac{\tau}{\gamma^2\cdot T_i}\right)^{-1} \f]
          */
        void evaluateModel(QFRawDataRecord* record, int index, int model, double* taus, double* output, uint32_t N, double* distribution_tau, double* distribution, uint32_t distributionN) const;

        /*! \brief returns an array representing the distribution for the given record, index, model combination
          */
        QVector<double> getDistribution(QFRawDataRecord* record, int index, int model) const;
        /*! \brief returns an array representing the distribution tau values (lag time axis) for the given record, index, model combination
          */
        QVector<double> getDistributionTaus(QFRawDataRecord* record, int index, int model) const;
        /*! \brief returns an array representing the distribution tau values (lag time axis) for the given record, index, model combination
          */
        QVector<double> getDistributionDs(QFRawDataRecord* record, int index, int model) const;


        /*! \brief calculates fit statistics for the given fit function and dataset.

            \note If \a modelVals is \c NULL this function will evaluate the model for the given parameters, retrieving the stored
                  distribution with getDistribution() and getDistributionTaus()
         */
        QFFitStatistics calcFitStatistics(QFRawDataRecord* record, int index, int model, double* taus, double* modelVals, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins);

        virtual QString getEvaluationResultID(int currentIndex, int model) const;
        using QFUsesResultsByIndexAndModelEvaluation::getEvaluationResultID;

        /*! \brief allows to draw certain fit parameters from other sources (e.g. copy a property of the underlying record

            \param r record this appplies to
            \param resultID the result ID which to access in the raw data records result store
            \param paramid the parameter we are looking up
            \param[out] value in this parameter the value is returned, if the function returns \c true
            \param[out] error in this parameter the value' error is returned, if the function returns \c true
            \return \c true if a special value is found. In that case it returns the value in \a value and \a error

         */
        virtual bool hasSpecial(const QFRawDataRecord* r, const QString& resultID, const QString& paramid, double& value, double& error) const;

protected:
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(const QFRawDataRecord* record) const;

        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w) const;
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);



        /** \brief returns default values for a parameter */
        virtual bool getParameterDefault(const QFRawDataRecord* r, const QString& resultID, const QString& parameterID, QFUsesResultsEvaluation::FitParameterDefault& defaultValue) const;


};

#endif // QFFCSMAXENTEVALUATIONITEM_H
