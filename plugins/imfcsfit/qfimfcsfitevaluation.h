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

#ifndef QFIMFCSFITEVALUATION_H
#define QFIMFCSFITEVALUATION_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrsimplecountrateinterface.h"
#include "qfrdrcountratesinterface.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"
#include "../base_classes/qffitresultsevaluation.h"
#include "../base_classes/qffitresultsbyindexasvectorevaluation.h"
#include "qffitalgorithm.h"
#include "qfpluginservices.h"
#include "qffcsweightingtools.h"
#include "qffitresultsbyindexevaluationfittools.h"
#include "qfmatchrdrfunctor.h"

class QFImFCSMatchRDRFunctor: public QFMatchRDRFunctor {
    public:
        explicit QFImFCSMatchRDRFunctor(): QFMatchRDRFunctor() {};
        /** \brief reimplement this function to check whether a QFRawDataRecord* is contained in a QFSelectRDRDialog. the default implementation always returns \c true . */
        virtual bool matches(const QFRawDataRecord* record) const ;
};


/*! \brief evaluation item class for imaging FCS least square fits
    \ingroup qf3evalp_imfcsfit

*/
class QFImFCSFitEvaluation : public QFFitResultsByIndexAsVectorEvaluation, public QFFCSWeightingTools, public QFFitResultsByIndexEvaluationFitTools {
        Q_OBJECT

    friend class QFImFCSFitThread;

    public:


        /** Default constructor */
        QFImFCSFitEvaluation(QFProject* parent);
        /** Default destructor */
        virtual ~QFImFCSFitEvaluation();

        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("imfcs_fit"); }
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("imaging FCS Fit"); }
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/imfcs_fit.png"); }
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("Least squares fitting for imaging fluorescence correlation spectroscopy (imaging FCS) data"); }
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/imfcs_fit_logo.png"); }
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Fit"); }
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, QFEvaluationPropertyEditor *propEditor, QWidget *parent=NULL);





        /** \brief return the smallest available index */
        virtual int getIndexMin(const QFRawDataRecord* r) const;
        /** \brief return the largest available index */
        virtual int getIndexMax(const QFRawDataRecord* r) const;

        virtual QString getIndexName(const QFRawDataRecord *rec, int index) const;

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".

            \note this method is intended to perform the fits one after the other or single fits. It will internally starts the fit algorithm
                  in its own thread. If you want to control the multithreading by yourself, use doFitForMultithread() instead !!!
          */
        virtual void doFit(QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFFitAlgorithmReporter* dlgFitProgress=NULL, bool doLog=false);

        /*! \brief perform a fit for the given \a record and \a run

            The parameters \a defaultMinDatarange and \a defaultMaxDatarange set the range of data points taken for the fit.
            If both are -1, the full range is used

            The object \a dlgFitProgress (if supplied) is used to report the progress and to check whether the user clicked "Cancel".
          */
        virtual void doFitForMultithread(QFFitAlgorithm* falg, QFFitFunction* ffunc, QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const;
        virtual void doFitForMultithreadReturn(QFRawDataRecord::QFFitFitResultsStore& result, const QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1, QFPluginLogService *logservice=NULL) const ;
        /*! \brief This function should return a usable QFFitAlgoruthm and QFFitFunction as freshly created objects for the given record and run
         *
         * \note This does not have to be thread safe!!!
         */
        virtual void createFitFunctionAndAlgorithm(QFFitAlgorithm*& falg, QFFitFunction*& ffunc, const QFRawDataRecord* record, int run);

        /** \brief calculates fit statistics for the given fit function and dataset. */
        QFFitStatistics calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, const double* tauvals, const double* corrdata, const double* weights, int datacut_min, int datacut_max, const double* fullParams, const double* errors, const bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record=NULL, int run=-1);

        QFImFCSMatchRDRFunctor* getMatchFunctor() const;

        /*! \brief fill a 2D array with a chi^2 landscape for the given fit, this is called like doFit() + some output arguments!

            This method evaluates the chi2 landscape for a range of values of two parameters identified by the file/model of the parameter and it's ID therein. The output image
            has to have size paramXValues.size()*paramYvalues.size().

            Global parameters are resolved internally!
         */
        virtual void calcChi2Landscape(double* chi2Landscape, int paramXFile, int paramXID, const QVector<double>& paramXValues, int paramYFile, int paramYID, const QVector<double>& paramYvalues, const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1);

    protected:
        QFImFCSMatchRDRFunctor* matchFunctor;
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(const QFRawDataRecord* record) const;

        /*! \copydoc QFFitResultsEvaluation::intWriteDataAlgorithm()      */
        virtual void intWriteDataAlgorithm(QXmlStreamWriter& w) const;

        /*! \copydoc QFFitResultsEvaluation::intReadDataAlgorithm()      */
        virtual void intReadDataAlgorithm(QDomElement& e);

        virtual bool hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const;
        virtual bool hasSpecial(const QFRawDataRecord* r, int index, const QString& paramid, double& value, double& error) const ;


        virtual bool overrideFitFunctionPreset(const QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetError(const QFRawDataRecord* r, QString paramName, double &value) const ;
        virtual bool overrideFitFunctionPresetFix(const QFRawDataRecord* r, QString paramName, bool &value) const ;

    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsByIndexEvaluation::getEvaluationResultID;
        using QFFitResultsByIndexEvaluation::hasSpecial;



};

#endif // QFIMFCSFITEVALUATION_H

