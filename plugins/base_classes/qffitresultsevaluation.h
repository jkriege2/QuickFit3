#ifndef QFFITRESULTSEVALUATION_H
#define QFFITRESULTSEVALUATION_H

#include <QMap>
#include <QSettings>

#include "qfevaluationitem.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "../interfaces/qfrdrcountratesinterface.h"
#include "qfevaluationitemfactory.h"
#include "qffitfunction.h"
#include "qffitalgorithm.h"
#include "qffitparameterbasicinterface.h"

/*! \brief evaluation item base class for data fits using QFFitAlgorithm and QFFitFunction
    \ingroup qf3evaluationplugins

    - This class allows to easily implement evaluation items which store fit results to theraw data records.
    - The fit algorithm and model/function may be selected globally.
    .

*/


class QFFitResultsEvaluation : public QFEvaluationItem, public QFFitParameterBasicInterface {
    Q_OBJECT
public:
    QFFitResultsEvaluation(QFProject* parent);
    virtual ~QFFitResultsEvaluation();

    /** \brief returns a list of all FCS fitting functions available for this evaluation
     *  \note This class uses only those fit functions where the ID starts with \c "fcs_"
     */
    virtual QStringList getAvailableFitFunctions() const;
    /** \brief returns the given fit function instance (DO NEVER DELETE THIS INSTANCE!) */
    virtual QFFitFunction* getFitFunction(const QString& id) const;

    /** \brief set the current fitting algorithm by ID */
    virtual void setFitAlgorithm(QString fitAlgorithm);

    /** \brief get the current fitting algorithm */
    virtual QFFitAlgorithm* getFitAlgorithm() const;

    /** \brief returns a list of all available fit algorithms */
    virtual QStringList getAvailableFitAlgorithms() const;

    /** \brief return the instance of a given fit algorithm for this evaluation (DO NEVER DELETE THIS INSTANCE!)  */
    virtual QFFitAlgorithm* getFitAlgorithm(const QString& id) const;

    /*! \brief set the current fitting algorithm

        This function also tries to transform the parameters between different models. The transformation is based on the parameter
        name. If the new model has some parameters with the same name as the old model, the new model parameters are set to the value
        of the old parameter.
    */
    virtual void setFitFunction(QString fitFunction);

    /** \brief get the current fitting algorithm */
    virtual QFFitFunction* getFitFunction();

    /*! \brief save all parameters of a given QFFitAlgorithm to the internal fit algorithm parameter store algorithm_parameterstore
    */
    virtual void storeQFFitAlgorithmParameters(QFFitAlgorithm* algorithm);
    /*! \brief load all parameters of a given QFFitAlgorithm from the internal fit algorithm parameter store algorithm_parameterstore
    */
    virtual void restoreQFFitAlgorithmParameters(QFFitAlgorithm* algorithm);
signals:

public slots:

protected:
    /*! \brief struct used to locally store fit parameter properties

        For each parameter that is stored in parameterStore this saves also whether it is really set, so each
        parameter is represented by two datamemeber: \c value and \c valueSet which is \c true only if the user
        has explicitly set the \c value. This information is used in the \c getFitXYZ() functions and set in the
        \c setFitXYZ() functions.
    */
    struct FitParameter {
        double value;
        double error;
        bool fix;
        double min;
        double max;
        bool valueSet;
        bool fixSet;
        bool minSet;
        bool maxSet;
        bool errorSet;
        FitParameter() {
            value=0;
            error=0;
            fix=false;
            min=0;
            max=0;
            valueSet=false;
            fixSet=false;
            minSet=false;
            maxSet=false;
            errorSet=false;
        }
    };

    /*! \brief datastore for fit parameter values

        Internally a uniquie key for every fit parameter is used to adress the contents. The key is calculated
        from the \code fit_fuction_id+"___"+parameter_id \endcode
     */
    QMap<QString, FitParameter> parameterStore;

    /*! \brief parameter store for fit algorithm parameters

        This map effectively has two strings as key. the first is the fit algorithm ID and the second the parameter name
        The value stored for each key is a QVariant.
     */
     QMap<QString, QMap<QString, QVariant> > algorithm_parameterstore;


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



     /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
     virtual QString getParameterStoreID(QString parameter);

     /** \brief return a valid ID to access parameterStore for the given parameter (id) in the current fit function (m_fitFunction) */
     virtual QString getParameterStoreID(QString fitfunction, QString parameter);

     /** \brief write object contents into XML file
      *
      *  this function saves the id of the current fit function and algorithm, as well as the contents of parameterStore to
      *  the given XML file.
      */
     virtual void intWriteData(QXmlStreamWriter& w);
     /** \brief read back the data stored by intWriteXML() */
     virtual void intReadData(QDomElement* e);

     /*! \brief allows to read additional information from a fit parameter node in the project file

         \param id id of the current fit parameter
         \param fitParam for your information the so far save information
         \param e QDomElement to read from

      */
     virtual void intReadDataFitParam(const QString& id, const FitParameter& fitParam, QDomElement& e) {};
     /*! \brief allows to write additional information to a fit parameter node in the project file

         \param id id of the current fit parameter
         \param w XML writer, currently in the parameter node (i.e. write subnode with w.writeStartElement() and attributes with w.writeAttribute() )
      */
     virtual void intWriteDataFitParam(const QString& id, QXmlStreamWriter& w) const {};
};

#endif // QFFITRESULTSEVALUATION_H
