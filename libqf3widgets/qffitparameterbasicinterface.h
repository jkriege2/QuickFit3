#ifndef QFFITPARAMETERBASICINTERFACE_H
#define QFFITPARAMETERBASICINTERFACE_H

#include <QString>
#include "libwid_imexport.h"

/*! \brief generic interface that allows to access values, ranges and fix settinsg for fitting parameters
    \ingroup qf3lib_widgets

    This interface is provided for convenience, so special widgets that display fitting parameters may be re-used later on.
 */
class QFWIDLIB_EXPORT QFFitParameterBasicInterface {
    public:
        virtual ~QFFitParameterBasicInterface() {};

        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored

         */
        virtual void setFitValue(const QString& id, double value)=0;
        /*! \brief return the value of a given parameter        */
        virtual double getFitValue(const QString& id)=0;
        /*! \brief return the fit error of a given parameter
            \return the error associated with the given parameter.
                    The error may only be stored in the QFRawDataRecord, if it is not available or accessible, then \c 0 is returned
        */
        virtual double getFitError(const QString& id)=0;
        /*! \brief set the error of a given parameter

            \param id set the value of the parameter with this id (see QFFitFunction)
            \param error error to be set
        */
        virtual void setFitError(const QString& id, double error)=0;

        /*! \brief set the fix property of a fit parameter of the current fit function
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix value to be stored
         */
        virtual void setFitFix(const QString& id, bool fix)=0;
        /*! \brief return the fix property of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual bool getFitFix(const QString& id)=0;
        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
            \param max upper bound for the value

         */
        virtual void setFitRange(const QString& id, double min, double max)=0;

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param min lower bound for the value
         */
        virtual void setFitMin(const QString& id, double min)=0;

        /*! \brief set the value range of a fit parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param max upper bound for the value
         */
        virtual void setFitMax(const QString& id, double max)=0;
        /*! \brief return the lower value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMin(const QString& id)=0;
        /*! \brief return the upper value bound of a given parameter
            \param id set the value of the parameter with this id (see QFFitFunction)
        */
        virtual double getFitMax(const QString& id)=0;


        /*! \brief set a fit parameter of the current fit function (see m_fitFunction) to the specified value in the initial parameters
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param value value to be stored
            \param error error assigned to the given \a value

         */
        virtual void setInitFitValue(const QString& id, double value, double error=0.0)=0;
        /*! \brief set the fix property of a fit parameter in the initial parameters
            \param id set the value of the parameter with this id (see QFFitFunction)
            \param fix value to be stored
         */
        virtual void setInitFitFix(const QString& id, bool fix)=0;

        /*! \brief set the given parameter \a id to the given value (and error) in all files */
        virtual void setAllFitValues(const QString& id, double value, double error=0.0, bool currentFileOnly=false)=0;
        /*! \brief set the given parameter \a id to the given fix value */
        virtual void setAllFitFixes(const QString& id, bool fix, bool currentFileOnly=false)=0;
        /*! \brief reset the given parameter \a id to the initial/global/default value */
        virtual void resetDefaultFitValue(const QString& id)=0;
        /*! \brief reset the given parameter \a id to the initial/global/default fix */
        virtual void resetDefaultFitFix(const QString& id)=0;
        /*! \brief return the default/initial/global value of a given parameter        */
        virtual double getDefaultFitValue(const QString& id)=0;
        /*! \brief return the default/initial/global fix of a given parameter        */
        virtual bool getDefaultFitFix(const QString& id)=0;
        /*! \brief reset the all parameters to the initial/global/default value in all files */
        virtual void resetAllFitValue()=0;
        /*! \brief reset the all parameters to the initial/global/default fix in all files */
        virtual void resetAllFitFix()=0;
        /*! \brief reset the all parameters to the initial/global/default value in current files */
        virtual void resetAllFitValueCurrent()=0;
        /*! \brief reset the all parameters to the initial/global/default fix in current files */
        virtual void resetAllFitFixCurrent()=0;
};

#endif // QFFITPARAMETERBASICINTERFACE_H
