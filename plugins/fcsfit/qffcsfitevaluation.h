#ifndef QFFCSFITEVALUATION_H
#define QFFCSFITEVALUATION_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "../fcs/qfrdrfcsdata.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item class for FCS least square fits
    \ingroup qf3evalp_fcsfit

    \note This evaluation is applicable to all QFRawDataRecords that implement the QFRDRFCSDataInterface interface! Also it makes use of all
          fit functions (see QFFitFunction and QFFitFunctionManager) registered to QuickFit where the id starts with \c "fcs_" !
*/
class QFFCSFitEvaluation : public QFEvaluationItem, public QFFitParameterBasicInterface {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSFitEvaluation(QFProject* parent);
        /** Default destructor */
        virtual ~QFFCSFitEvaluation();
    public:
        /** \brief return type (short type string) */
        virtual QString getType() const { return QString("fcs_fit"); };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("FCS Fit"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/fcs_fit.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("Least squares fitting for fluorescence correlation spectroscopy data"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/fcs_fit_logo.png"); };
        /** \brief returns the number of additional editor panes for this record */
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName() { return QString("Fit"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFEvaluationEditor* createEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** \brief determines whether this evaluation is applicable to a given raw data record. This method is used to generate the
         *         list of raw data records presented to the user */
        virtual bool isApplicable(QFRawDataRecord* record);


        /** \brief set the current fitting algorithm */
        void setFitAlgorithm(QString fitAlgorithm) { m_fitAlgorithm=fitAlgorithm; };
        /** \brief get the current fitting algorithm */
        QFFitAlgorithm* getFitAlgorithm() const { return getFitAlgorithm(m_fitAlgorithm); };

        /*! \brief set the current fitting algorithm

            This function also tries to transform the parameters between different models. The transformation is based on the parameter
            name. If the new model has some parameters with the same name as the old model, the new model parameters are set to the value
            of the old parameter.
        */
        void setFitFunction(QString fitFunction);
        /** \brief get the current fitting algorithm */
        QFFitFunction* getFitFunction();

        /** \brief set the current run to use, -1 = average, 0..N = runs, <-1: invalid */
        int getCurrentRun();//{ return m_currentRun; }

        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value

            The value may be stored at different positions:
              - If no item is highlighted the parameter is not stored at all
              - If a current raw data record is highlighted and no fit has been carried out so far for this record, the value
                is stored in the preset value store of this QFEvaluationItem
              - If a current raw data record is highlighted and a fit has already been carried out for this record, the value
                is stored as a result in the highlighted QFRawDataRecord
            .
            This function also checks whether the value is to be stored (i.e. it's userEditable property is set)

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        void setFitValue(QString id, double value);

        /** \brief stores the given value as a fit result, i.e. into the currently highlighted QFRawDataRecord */
        void setFitResultValue(QString id, double value);


        /** \brief stores the given value and error as a fit result, i.e. into the currently highlighted QFRawDataRecord */
        void setFitResultValue(QString id, double value, double error);

        /*! \brief return the value of a given parameter

            This function looks for the value at several positions:
              -# if a fit has already been performed for the current record, the value is read from raw data record
              -# if no fit has been performed, the value is taken from this evaluation (i.e. the preset value)
              -# if also locally no value is available, the value is looked up in the application INI
              -# if this also fails, the value is taken from the initial value stored in the fitFunction
            .
        */
        double getFitValue(QString id);

        /*! \brief return the fit error of a given parameter

            \return the error associated with the given parameter.
                    The error may only be stored in the QFRawDataRecord, if it is not available or accessible, then \c 0 is returned
        */
        double getFitError(QString id);

        /*! \brief set the fix property of a fit parameter of the current fit function (see m_fitFunction)

            For a description of when data is stored, see setFitValue()

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        void setFitFix(QString id, bool fix);

        /** \brief stores the given fix property as a fit result, i.e. into the currently highlighted QFRawDataRecord */
        void setFitResultFix(QString id, bool fix);

        /*! \brief return the fix property of a given parameter

            For a detailed description of where the value is searched, see getFitValue()
        */
        bool getFitFix(QString id);


        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

            This range is stored locally in the evaluation, never in the raw data record

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
            \param max upper bound for the value

         */
        void setFitRange(QString id, double min, double max);

        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

            This range is stored locally in the evaluation, never in the raw data record

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value

         */
        void setFitMin(QString id, double min);

        /*! \brief set the value range of a fit parameter of the current fit function (see m_fitFunction)

            This range is stored locally in the evaluation, never in the raw data record

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param max upper bound for the value

         */
        void setFitMax(QString id, double max);
        /*! \brief return the lower value bound of a given parameter

            For a detailed description of where the value is searched, see getFitValue()
        */
        double getFitMin(QString id);


        /*! \brief return the upper value bound of a given parameter

            For a detailed description of where the value is searched, see getFitValue()
        */
        double getFitMax(QString id);


        /** \brief returns a list of all FCS fitting functions available for this evaluation
         *  \note This class uses only those fit functions where the ID starts with \c "fcs_"
         */
        QStringList getAvailableFitFunctions() const { return m_fitFunctions.keys(); };
        /** \brief returns the given fit function instance (DO NEVER DELETE THIS INSTANCE!) */
        QFFitFunction* getFitFunction(QString id) const {
            if (m_fitFunctions.contains(id)) return m_fitFunctions[id];
            else return NULL;
        };

        /** \brief returns a list of all available fit algorithms */
        QStringList getAvailableFitAlgorithms() const { return m_fitAlgorithms.keys(); };
        /** \brief return the instance of a given fit algorithm for this evaluation (DO NEVER DELETE THIS INSTANCE!)  */
        QFFitAlgorithm* getFitAlgorithm(QString id) const {
            if (m_fitAlgorithms.contains(id)) return m_fitAlgorithms[id];
            else return NULL;
        };

        /** \brief fill the given array of doubles with the current parameter values, as appropriate to use together with QFFitFunction */
        void fillParameters(double* param);
        /** \brief fill the given array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction */
        void fillParameterErrors(double* param);
        /** \brief fill the given array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction */
        void fillParametersMin(double* param);
        /** \brief fill the given array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction */
        void fillParametersMax(double* param);
        /** \brief fill the given array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction */
        void fillFix(bool* param);

        /*! \brief fill a newly allocated array of doubles with the current parameter values, as appropriate to use together with QFFitFunction
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        double* allocFillParameters();
        /*! \brief fill a newly allocated array of doubles with the current parameter errors, as appropriate to use together with QFFitFunction
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        double* allocFillParameterErrors();
        /*! \brief fill a newly allocated array of doubles with the current parameter lower bounds, as appropriate to use together with QFFitFunction
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        double* allocFillParametersMin();
        /*! \brief fill a newly allocated array of doubles with the current parameter upper bounds, as appropriate to use together with QFFitFunction
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        double* allocFillParametersMax();
        /*! \brief fill a newly allocated array of bools with the current parameter fix values, as appropriate to use together with QFFitFunction
            \note This function calls \c calloc() internally, so you will have to free the created arrays using \c free() of the C standard library!
        */
        bool* allocFillFix();

        /** \brief determine whether a fit has been carried out for the currently highlighted record
         *
         *  \return \c true if fit results exist for the current evaluation and fit function in the highlighted QFRawDataRecord result store.
         *          If no record is highlighted or any other circumstances prevent the determination of a proper result, the result will be \c false!
         */
        bool hasFit();

    public slots:
        /** \brief set the current run to use, -1 = average, 0..N = runs, <-1: invalid */
        void setCurrentRun(int run);

    protected:
        /*! \brief struct used to locally store fit parameter properties

            For each parameter that is stored in parameterStore this saves also whether it is really set, so each
            parameter is represented by two datamemeber: \c value and \c valueSet which is \c true only if the user
            has explicitly set the \c value. This information is used in the \c getFitXYZ() functions and set in the
            \c setFitXYZ() functions.
        */
        struct FitParameter {
            double value;
            bool fix;
            double min;
            double max;
            bool valueSet;
            bool fixSet;
            bool minSet;
            bool maxSet;
            FitParameter() {
                value=0;
                fix=false;
                min=0;
                max=0;
                valueSet=false;
                fixSet=false;
                minSet=false;
                maxSet=false;
            }
        };

        /*! \brief datastore for fit parameter values

            Internally a uniquie key for every fit parameter is used to adress the contents. The key is calculated
            from the \code fit_fuction_id+"___"+parameter_id \endcode
         */
        QMap<QString, FitParameter> parameterStore;

        /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
        inline QString getParameterStoreID(QString parameter) {
            QString ff="";
            if (getFitFunction()!=NULL) {
                ff=getFitFunction()->id();
            }
            return getParameterStoreID(ff, parameter);
        };

        /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
        inline QString getParameterStoreID(QString fitfunction, QString parameter) {
            return QString(fitfunction+"___"+parameter).trimmed().toLower();
        };

        /** \brief create an ID to reference results that belong to this evaluation \u object (includes the evaluation id) and the
         *         current fit function */
        inline QString getEvaluationResultID() {
            return getEvaluationResultID(m_fitFunction);
        }

        /** \brief create an ID to reference results that belong to this evaluation \u object (includes the evaluation id) and the
         *         current fit function for a given fitFunction ID */
        inline QString getEvaluationResultID(QString fitFunction) {
            return getType()+"_"+QString::number(getID())+"_"+fitFunction+"_run"+QString::number(m_currentRun);
        }

        /** \brief write object contents into XML file
         *
         *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
         *  the given XML file.
         */
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read back the data stored by intWriteXML() */
        virtual void intReadData(QDomElement* e);


        /** \brief the current fitting function */
        QString m_fitFunction;
        /** \brief the current fitting algorithm */
        QString m_fitAlgorithm;
        /** \brief map with all available fit functions */
        QMap<QString, QFFitFunction*> m_fitFunctions;
        /** \brief map with all available fit algorithms */
        QMap<QString, QFFitAlgorithm*> m_fitAlgorithms;

        /** \brief settings object to access fit parameters */
        QSettings* fitParamSettings;

        /** \brief current run to use, -1 = average, 0..N = runs, <-1: invalid */
        int m_currentRun;

    private:
};

#endif // QFFCSFITEVALUATION_H
