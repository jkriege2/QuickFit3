#ifndef QFPLUGINSERVICES_H
#define QFPLUGINSERVICES_H

#include <QString>
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"

/*! \brief wrapper class that allows plugins to access basic services of the QuickFit application
    \ingroup qf3plugintools
*/
class QFPluginServices {
    public:
        /** Default destructor */
        virtual ~QFPluginServices() {}

        /** \brief log text message */
        virtual void log_text(QString message)=0;
        /** \brief log warning message */
        virtual void log_warning(QString message)=0;
        /** \brief log error message */
        virtual void log_error(QString message)=0;
        /** \brief set status message */
        virtual void setStatusMessage(QString message)=0;
        /** \brief set range of progress bar */
        virtual void setProgressRange(int minP, int maxP)=0;
        /** \brief set progress barto \a value */
        virtual void setProgress(int value)=0;
        /** increment progress bar by 1  */
        virtual void incProgress()=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitFunctionManager* getFitFunctionManager()=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitAlgorithmManager* getFitAlgorithmManager()=0;
};

#endif // QFPLUGINSERVICES_H
