#ifndef QFFITFUNCTION_H
#define QFFITFUNCTION_H

#include <QString>
#include <QList>

/*! \brief describes a virtual base class fitting functions that are used together with QFFitAlgorithm objects.
    \ingroup qf3lib_fitting

*/
class QFFitFunction {
    public:
        /** \brief different types of input widgets for parameters */
        enum ParameterType { FloatNumber=0, IntNumber=1 };
        /** \brief this struct is used to describe the fitting parameters */
        struct ParameterDescription {
            /** \brief type of the parameter */
            ParameterType type;
            /** \brief short unique id for the parameter, e.g. "n" */
            QString id;
            /** \brief description of the parameter, e.g. "particle number N" */
            QString name;
            /** \brief a label for the parameter, e.g. "N = " */
            QString label;
            /** \brief unit of the parameter, e.g. "usec", "g"  */
            QString unit;
            /** \brief is this a fitting parameter, if \c false the user may supply a value, but the value will never be changed by the fitting
             *         algorithm. Use this e.g. to configure a model (number of components, ...) */
            bool fit;
            /** \brief determine whether, or not the user is allowed to change the value of a parameter. You could use a parameter with
             *         \c fit=userEditable=false to output values that were calculated from the fitting parameters after the fit. */
            bool userEditable;
            /** \brief determine whether, or not an error value is displayed for this parameter */
            bool displayError;
            /** \brief an initial value for the parameter */
            double initialValue;
            /** \brief minimum value of the parameter range (if supported by algorithm) */
            double minValue;
            /** \brief maximum value of the parameter range (if supported by algorithm) */
            double maxValue;
            /** \brief value increment for the widget associated with the parameter */
            double inc;
        };

        virtual ~QFFitFunction() {}

        /** \brief returns the number of parameters */
        virtual int paramCount() const {
            return m_parameters.size();
        }
        /** \brief return a name for the model */
        virtual QString name() const=0;
        /** \brief return a short unique model ID string */
        virtual QString id() const=0;
        /** \brief return a HTML file to be displayed as model help */
        virtual QString helpFile() const { return QString(""); };

        /** \brief evaluate the fitting function with the given parameter vector */
        virtual double evaluate(double t, const double* data) const =0;

        /** \brief calculate non-fit parameters, i.e. fit=userEditable=false */
        virtual void calcParameter(double* data) const {};

        /** \brief returns the description of the i-th parameter */
        ParameterDescription getDescription(int i) const  {
            return m_parameters[i];
        }

        /** \brief get num of the given parameter or -1 */
        int getParameterNum(QString param) {
            for (int i=0; i<m_parameters.size(); i++) {
                if (m_parameters[i].id==param) return i;
            }
            return -1;
        }

        /** \brief get id of the given parameter or an empty string */
        QString getParameterID(int i) {
            if ((i>=0) && (i<m_parameters.size())) return m_parameters[i].id;
            return QString("");
        }
    protected:
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \param description parameter description to be aded
            \return the id of the parameter
         */
        int addParameter(const ParameterDescription& description) {
            m_parameters.append(description);
            return m_parameters.size()-1;
        }
        /*! \brief add a parameter description

            used in the constructor to define the model parameters
            \param description parameter description to be aded
            \return the id of the parameter
         */
        int addParameter(ParameterType type, QString id, QString name, QString label, QString unit, bool fit, bool userEditable, bool displayError, double initialValue, double minValue, double maxValue, double inc) {
            ParameterDescription d;
            d.type=type;
            d.id=id;
            d.name=name;
            d.label=label;
            d.unit=unit;
            d.fit=fit;
            d.userEditable=userEditable;
            d.initialValue=initialValue;
            d.minValue=minValue;
            d.maxValue=maxValue;
            d.inc=inc;
            d.displayError=displayError;

            return addParameter(d);
        }

    private:
        QList<ParameterDescription> m_parameters;
};

#endif // QFFITFUNCTION_H
