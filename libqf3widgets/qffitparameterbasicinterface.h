#ifndef QFFITPARAMETERBASICINTERFACE_H
#define QFFITPARAMETERBASICINTERFACE_H

#include <QString>

/*! \brief generic interface that allows to access values, ranges and fix settinsg for fitting parameters
    \ingroup qf3lib_widgets

    This interface is provided for convenience, so special widgets that display fitting parameters may be re-used later on.
 */
class QFFitParameterBasicInterface {
    public:
        virtual ~QFFitParameterBasicInterface() {};

        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(QString id, double value)=0;
        /*! \brief return the value of a given parameter        */
        virtual double getFitValue(QString id)=0;
        /*! \brief return the fit error of a given parameter
            \return the error associated with the given parameter.
                    The error may only be stored in the QFRawDataRecord, if it is not available or accessible, then \c 0 is returned
        */
        virtual double getFitError(QString id)=0;
        /*! \brief set the fix property of a fit parameter of the current fit function
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
         */
        virtual void setFitFix(QString id, bool fix)=0;
        /*! \brief return the fix property of a given parameter  */
        virtual bool getFitFix(QString id)=0;
        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
            \param max upper bound for the value

         */
        virtual void setFitRange(QString id, double min, double max)=0;

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
         */
        virtual void setFitMin(QString id, double min)=0;

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param max upper bound for the value
         */
        virtual void setFitMax(QString id, double max)=0;
        /*! \brief return the lower value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMin(QString id)=0;
        /*! \brief return the upper value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMax(QString id)=0;
};

#endif // QFFITPARAMETERBASICINTERFACE_H
