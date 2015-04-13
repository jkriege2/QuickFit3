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

#ifndef QFFITRESULTSBYINDEXEVALUATION_H
#define QFFITRESULTSBYINDEXEVALUATION_H

#include "qffitresultsevaluation.h"

#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class that extends QFFitResultsEvaluation with an index-based interface
    \ingroup qf3evaluationplugins

*/
class QFFitResultsByIndexEvaluation : public QFFitResultsEvaluation {
        Q_OBJECT
    public:
        /*! \brief class constructor

          \param fitFunctionPrefix only fit functions with this prefix will be used by this object, leave empty to use all, you may supply a list, separated by commas
          \param parent project this object belongs to
        */
        QFFitResultsByIndexEvaluation(const QString& fitFunctionPrefix, QFProject* parent, bool showRDRList=true, bool useSelection=false);

        /** \brief class destructor */
        virtual ~QFFitResultsByIndexEvaluation();


        /** \copydoc QFEvaluationItem::setPresetProperty() */
        virtual void setPresetProperty(const QString& id, const QVariant& data, bool usereditable=true, bool visible=true);


    signals:
        void currentIndexChanged(int newIndex);

    public:
        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(QString fitFunction, int currentIndex) const;


        /** \brief return the smallest available index */
        virtual int getIndexMin(const QFRawDataRecord* r) const=0 ;
        /** \brief return the largest available index */
        virtual int getIndexMax(const QFRawDataRecord* r) const=0;

        /** \brief set the current index to use */
        virtual int getCurrentIndex() const;

        /** \brief return a name for the given index */
        virtual QString getIndexName(const QFRawDataRecord *rec, int index) const;

        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

            The value may be stored at different positions:
              - If no item is highlighted the parameter is not stored at all
              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
                is stored in the preset value store of this QFEvaluationItem
              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
                is stored as a result in the highlighted QFRawDataRecord
            .
            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

            \param r the record to adress
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(QFRawDataRecord* r, int index, const QString& id, double value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value) ;

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, QString unit) ;

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueString(QFRawDataRecord* r, int index, const QString& id, QString value) ;

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value) ;

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& id, int64_t value, QString unit) ;

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueBool(QFRawDataRecord* r, int index, const QString& id, bool value) ;

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param error error associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& id, double value, double error) ;

        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param error error to be stored
         */
        virtual void setFitResultError(QFRawDataRecord* r, int index, const QString& id, double error) ;

        /** \brief stores the given values and errors as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValues(QFRawDataRecord* r, int index, double* values, double* errors) ;

        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValuesVisible(QFRawDataRecord* r, int index, double* values, double* errors)  ;

        /** \brief stores the given values and errors as a fit result if they are visible, i.e. into the currently highlighted QFRawDataRecord.
         *          Also imports the fit parameter description from the QFFitFunction and sets the parameter group to the given value
         *  \param r the record to adress
         *  \param resultID the result ID which to access in the raw data records result store
         *  \param values values to be stored
         *  \param errors errors to be stored
         */
        virtual void setFitResultValuesVisibleWithGroupAndLabel(QFRawDataRecord* r, int index, const double* values, const double* errors, const QString& group, const bool* fix=NULL, const QString& fixGroup=QString(""), bool sortPriority=true) ;

        /*! \brief return the value of a given parameter
            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            \return the value of the given parameter \a id

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        virtual double getFitValue(const QFRawDataRecord* r, int index, const QString& id) const;

        /*! \brief return the fit error of a given parameter
            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            \return the error associated with the given parameter.
        */
        virtual double getFitError(const QFRawDataRecord* r, int index, const QString& id) const ;

        /*! \brief set the error of a given parameter

            \param r the record to adress
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(QFRawDataRecord* r, int index, const QString& id, double error);

        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

            For a description of when data is stored, see setFitValue()

            \param r the record to adress
            \param index the index in which to adress
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix fix to be stored

         */
        virtual void setFitFix(QFRawDataRecord* r, int index, const QString& id, bool fix);

        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index in which to adress
         *  \param id set the value of the parameter with this id (see QFFitFunction)
         *  \param fix fix to be stored
         */
        virtual void setFitResultFix(QFRawDataRecord* r, int index, const QString& id, bool fix) ;

        /*! \brief return the fix property of a given parameter

            \param r the record to adress
            \param index the index in which to adress
            \param id the parameter id
            For a detailed description of where the value is searched, see getFitValue()
        */
        virtual bool getFitFix(const QFRawDataRecord* r, int index, const QString& id) const ;

        /** \brief sets the group of the given fit result
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param group group to be stored
         */
        virtual void setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group) ;

        /** \brief sets the label of the given fit result
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param label label to be stored
         *  \param label_richtext richtext-ed label to be stored
         */
        virtual void setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext=QString("")) ;

        /** \brief sets the group of the given evaluation result ID \a resultID
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param group group to be stored
         */
        virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group) ;

        /** \brief sets the description of the given evaluation result ID \a resultID
         *  \param r the record to adress
            \param index the index in which to adress
         *  \param description description to be stored
         */
        virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description) ;

        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter vector to fill
         */
        virtual void fillParameters(const QFRawDataRecord* r, int index, double* param, QFFitFunction* function=NULL) const;
        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter error vector to fill
        */
        virtual void fillParameterErrors(const QFRawDataRecord* r, int index, double* param, QFFitFunction* function=NULL) const;
        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
         *  \param r record this appplies to
         *  \param index index in \a r this applies to
         *  \param param parameter fix vector to fill
        */
        virtual void fillFix(const QFRawDataRecord* r, int index, bool* param, QFFitFunction* function=NULL) const ;

        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual double* allocFillParameters(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const ;
        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter error vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual double* allocFillParameterErrors(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const;
        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter fix vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual bool* allocFillFix(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const ;









        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual QVector<double> allocVecFillParameters(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const ;
        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter error vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual QVector<double> allocVecFillParameterErrors(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const;
        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
            \param r record this appplies to
            \param index index in \a r this applies to
            \return parameter fix vector
            \note This function calls \c qfCalloc() internally, so you will have to free the created arrays using \c qfFree() of the C standard library!
        */
        virtual QVector<bool> allocVecFillFix(const QFRawDataRecord* r, int index, QFFitFunction* function=NULL) const ;









        /** \brief determine whether a fit has been carried out for the given record and index
         *
         *  \param r the given record, if \c NULL the currently highlighted record is checked
         *  \param index the index to check
         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
         */
        virtual bool hasFit(const QFRawDataRecord* r, int index) const;

        /*! \brief set the given parameter \a id to the given value (and error) in all files and all indexes */
        virtual void setAllFitValues(const QString& id, double value, double error=0.0, bool currentFileOnly=false);
        /*! \brief set the given parameter \a id to the given fix value and all indexes */
        virtual void setAllFitFixes(const QString& id, bool fix, bool currentFileOnly=false);
        /*! \brief reset all parameters to the initial/global/default value in all files and all indexes */
        virtual void resetAllFitValue();
        /*! \brief reset all fit results to the initial/global/default value in all files and all indexes */
        virtual void resetAllFitResults();
        /*! \brief reset all parameters to the initial/global/default fix in all files and all indexes */
        virtual void resetAllFitFix();
        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
        virtual void resetAllFitResultsCurrentFileAllIndices();
        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
        virtual void resetAllFitResultsAllIndices(QFRawDataRecord* rec);
        /*! \brief reset all fit results to the initial/global/default value in the current file but all indexes */
        virtual void resetAllFitResultsAllFilesAllIndices();

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function */
        virtual QString getEvaluationResultID(const QFRawDataRecord *r) const ;

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(QString fitFunction) const;

        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(int currentIndex, const QFRawDataRecord *r) const;


        virtual bool hasSpecial(const QFRawDataRecord* r, int index, const QString& paramid, double& value, double& error) const ;



        /*! \brief fill a 2D array with a chi^2 landscape for the given fit, this is called like doFit() + some output arguments!

            This method evaluates the chi2 landscape for a range of values of two parameters identified by the file/model of the parameter and it's ID therein. The output image
            has to have size paramXValues.size()*paramYvalues.size().

            Global parameters are resolved internally!
         */
        virtual void calcChi2Landscape(double* chi2Landscape, int paramXFile, int paramXID, const QVector<double>& paramXValues, int paramYFile, int paramYID, const QVector<double>& paramYvalues, const QList<QFRawDataRecord*>& records, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1);
        virtual void calcChi2Landscape(double* chi2Landscape, int paramXFile, int paramXID, const QVector<double>& paramXValues, int paramYFile, int paramYID, const QVector<double>& paramYvalues, QFRawDataRecord* record, int run, int defaultMinDatarange=-1, int defaultMaxDatarange=-1);



    public slots:
        /** \brief set the current index to use */
        virtual void setCurrentIndex(int index);



    protected:
        /** \brief current index to use */
        int m_currentIndex;

        /** \brief indicates whether getEvaluationResultID() uses the fitfunction ID in it's return value (default: \c true ) */
        bool getEvaluationResultIDUsesFitFunction;



    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */


        using QFFitResultsEvaluation::setFitValue;
        using QFFitResultsEvaluation::setFitResultValue;
        using QFFitResultsEvaluation::setFitResultValueString;
        using QFFitResultsEvaluation::setFitResultValueInt;
        using QFFitResultsEvaluation::setFitResultValueBool;
        using QFFitResultsEvaluation::setFitResultError;
        using QFFitResultsEvaluation::setFitResultValues;
        using QFFitResultsEvaluation::setFitResultValuesVisible;
        using QFFitResultsEvaluation::getFitValue;
        using QFFitResultsEvaluation::getFitError;
        using QFFitResultsEvaluation::setFitFix;
        using QFFitResultsEvaluation::setFitResultFix;
        using QFFitResultsEvaluation::getFitFix;
        using QFFitResultsEvaluation::setFitError;
        using QFFitResultsEvaluation::setFitRange;
        using QFFitResultsEvaluation::setFitMin;
        using QFFitResultsEvaluation::setFitMax;
        using QFFitResultsEvaluation::hasFit;
        using QFFitResultsEvaluation::hasSpecial;
        using QFFitResultsEvaluation::fillParameters;
        using QFFitResultsEvaluation::fillParameterErrors;
        using QFFitResultsEvaluation::fillParametersMin;
        using QFFitResultsEvaluation::fillParametersMax;
        using QFFitResultsEvaluation::fillFix;
        using QFFitResultsEvaluation::allocFillParameters;
        using QFFitResultsEvaluation::allocFillParameterErrors;
        using QFFitResultsEvaluation::allocFillParametersMin;
        using QFFitResultsEvaluation::allocFillParametersMax;
        using QFFitResultsEvaluation::allocFillFix;
        using QFFitResultsEvaluation::allocVecFillParameters;
        using QFFitResultsEvaluation::allocVecFillParameterErrors;
        using QFFitResultsEvaluation::allocVecFillParametersMin;
        using QFFitResultsEvaluation::allocVecFillParametersMax;
        using QFFitResultsEvaluation::allocVecFillFix;
        using QFFitResultsEvaluation::resetDefaultFitValue;
        using QFFitResultsEvaluation::resetDefaultFitFix;
        using QFFitResultsEvaluation::getDefaultFitValue;
        using QFFitResultsEvaluation::getDefaultFitFix;
        using QFFitResultsEvaluation::resetAllFitResultsCurrent;
        using QFFitResultsEvaluation::resetAllFitResults;
        using QFFitResultsEvaluation::setFitResultGroup;
        using QFFitResultsEvaluation::setFitResultLabel;
        using QFFitResultsEvaluation::setFitResultEvaluationGroup;
        using QFFitResultsEvaluation::setFitResultEvaluationDescription;
        using QFFitResultsEvaluation::setFitResultValuesVisibleWithGroupAndLabel;
        using QFFitResultsEvaluation::getEvaluationResultID;


};

#endif // QFFITRESULTSBYINDEXEVALUATION_H
