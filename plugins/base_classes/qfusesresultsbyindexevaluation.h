#ifndef QFUSESRESULTSBYINDEXEVALUATION_H
#define QFUSESRESULTSBYINDEXEVALUATION_H
#include <QHash>
#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qfusesresultsevaluation.h"

/*! \brief extends QFUsesResultsEvaluation with a per-index interface
    \ingroup qf3evaluationplugins


*/
class QFUsesResultsByIndexEvaluation : public QFUsesResultsEvaluation {
        Q_OBJECT
    public:
        explicit QFUsesResultsByIndexEvaluation(QFProject *parent = 0, bool showRDRList=true, bool useSelection=false);
        virtual ~QFUsesResultsByIndexEvaluation();

        /** \copydoc QFEvaluationItem::setPresetProperty() */
        virtual void setPresetProperty(const QString& id, const QVariant& data, bool usereditable=true, bool visible=true);

        /** \breif return \c true if an evaluation has been performed for the given record \a r1 and the given inde x\a index therein */
        virtual bool hasResults(QFRawDataRecord* r1, int index) const;


        /** \brief create an ID to reference results that belong to this evaluation \b object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        virtual QString getEvaluationResultID(int index) const;



        /** \brief sets the group of the given fit result
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param group group to be stored
         */
        virtual void setFitResultGroup(QFRawDataRecord* r, int index, const QString& parameterID, const QString& group);

        /** \brief sets the label of the given fit result
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param label label to be stored
         *  \param label_richtext richtext-ed label to be stored
         */
        virtual void setFitResultLabel(QFRawDataRecord* r, int index, const QString& parameterID, const QString& label, const QString& label_richtext=QString(""));

        /** \brief sets the group of the given evaluation result ID \a resultID
         *  \param r the record to adress
         *  \param index the index to access
         *  \param group group to be stored
         */
        virtual void setFitResultEvaluationGroup(QFRawDataRecord* r, int index, const QString& group);

        /** \brief sets the description of the given evaluation result ID \a resultID
         *  \param r the record to adress
         *  \param index the index to access
         *  \param description description to be stored
         */
        virtual void setFitResultEvaluationDescription(QFRawDataRecord* r, int index, const QString& description);

        /** \brief reset all fit results for the given \a index in the given record \a r */
        void resetAllFitResults(QFRawDataRecord* r, int index);






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
            \param index the index to access
            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(QFRawDataRecord* r, int index, const QString& parameterID, double value, const QString& unit=QString());

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& parameterID, double value, const QString& unit=QString());

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueString(QFRawDataRecord* r, int index, const QString& parameterID, const QString& value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param unit unit associated with \a value
         */
        virtual void setFitResultValueInt(QFRawDataRecord* r, int index, const QString& parameterID, int64_t value, const QString& unit=QString());

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         */
        virtual void setFitResultValueBool(QFRawDataRecord* r, int index, const QString& parameterID, bool value);

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value to be stored
         *  \param error error associated with \a value
         */
        virtual void setFitResultValue(QFRawDataRecord* r, int index, const QString& parameterID, double value, double error, const QString& unit=QString());

        /** \brief stores the given error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param error error to be stored
         */
        virtual void setFitResultError(QFRawDataRecord* r, int index, const QString& parameterID, double error);
        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value array to be stored
         *  \param error error array associated with \a value
         *  \param N number of items in \a value and \a error
         *  \param unit the unit name
         */
        virtual void setFitResultValueNumberArray(QFRawDataRecord* r, int index, const QString& parameterID, double* value, double* error, uint32_t N, const QString& unit=QString());

        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param value value array to be stored
         *  \param N number of items in \a value
         *  \param unit the unit name
         */
        virtual void setFitResultValueNumberArray(QFRawDataRecord* r, int index, const QString& parameterID, double* value, uint32_t N, const QString& unit=QString());

        /*! \brief return the value array of a given parameter
            \param r the record to adress
            \param index the index to access
            \param parameterID the parameter id
            \return the value of the given parameter \a id

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        virtual QVector<double> getFitValueNumberArray(QFRawDataRecord* r, int index, const QString& parameterID) const;
        /*! \brief return the fit error array of a given parameter
            \param r the record to adress
            \param index the index to access
            \param parameterID the parameter id
            \return the error associated with the given parameter.
        */
        virtual QVector<double> getFitValueErrorArray(QFRawDataRecord* r, int index, const QString& parameterID) const;



        /*! \brief return the value of a given parameter
            \param r the record to adress
            \param index the index to access
            \param parameterID the parameter id
            \return the value of the given parameter \a id

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        virtual double getFitValue(QFRawDataRecord* r, int index, const QString& parameterID) const;
        /*! \brief return the fit error of a given parameter
            \param r the record to adress
            \param index the index to access
            \param parameterID the parameter id
            \return the error associated with the given parameter.
        */
        virtual double getFitError(QFRawDataRecord* r, int index, const QString& parameterID) const;

        /*! \brief set the error of a given parameter

            \param r the record to adress
            \param index the index to access
            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(QFRawDataRecord* r, int index, const QString& parameterID, double error);

        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

            For a description of when data is stored, see setFitValue()

            \param r the record to adress
            \param index the index to access
            \param parameterID set the value of the parameter with this id (see QFFitFunction)
            \param fix fix to be stored

         */
        virtual void setFitFix(QFRawDataRecord* r, int index, const QString& parameterID, bool fix);

        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord
         *  \param r the record to adress
         *  \param index the index to access
         *  \param parameterID set the value of the parameter with this id (see QFFitFunction)
         *  \param fix fix to be stored
         */
        virtual void setFitResultFix(QFRawDataRecord* r, int index, const QString& parameterID, bool fix);

        /*! \brief return the fix property of a given parameter

            \param r the record to adress
            \param index the index to access
            \param parameterID the parameter id
            For a detailed description of where the value is searched, see getFitValue()
        */
        virtual bool getFitFix(QFRawDataRecord* r, int index, const QString& parameterID) const;
        virtual void setFitResultSortPriority(QFRawDataRecord *r, int index, const QString &parameterID, bool sortPriority);

        void setCurrentIndex(int index);
        int getCurrentIndex() const;


        virtual int getIndexMin(QFRawDataRecord* r) const;
        virtual int getIndexMax(QFRawDataRecord* r) const;
    protected:
        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);

        int currentIndex;

    public:
        using QFUsesResultsEvaluation::hasResults;
        using QFUsesResultsEvaluation::getEvaluationResultID;
        using QFUsesResultsEvaluation::setFitResultGroup;
        using QFUsesResultsEvaluation::setFitResultLabel;
        using QFUsesResultsEvaluation::setFitResultEvaluationGroup;
        using QFUsesResultsEvaluation::setFitResultEvaluationDescription;
        using QFUsesResultsEvaluation::setFitValue;
        using QFUsesResultsEvaluation::setFitResultValue;
        using QFUsesResultsEvaluation::setFitResultValueString;
        using QFUsesResultsEvaluation::setFitResultValueInt;
        using QFUsesResultsEvaluation::setFitResultValueBool;
        using QFUsesResultsEvaluation::setFitResultError;
        using QFUsesResultsEvaluation::getFitValue;
        using QFUsesResultsEvaluation::getFitError;
        using QFUsesResultsEvaluation::setFitError;
        using QFUsesResultsEvaluation::setFitFix;
        using QFUsesResultsEvaluation::setFitResultFix;
        using QFUsesResultsEvaluation::getFitFix;
        using QFUsesResultsEvaluation::setFitResultValueNumberArray;
        using QFUsesResultsEvaluation::getFitValueNumberArray;
        using QFUsesResultsEvaluation::getFitValueErrorArray;
        using QFUsesResultsEvaluation::setFitResultSortPriority;
        using QFUsesResultsEvaluation::resetAllFitResults;

        
};

#endif // QFUSESRESULTSBYINDEXEVALUATION_H
