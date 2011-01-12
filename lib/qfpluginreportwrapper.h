#ifndef QFPLUGINREPORTWRAPPER_H
#define QFPLUGINREPORTWRAPPER_H

#include <QString>

/*! \brief wrapper class that allows plugins to access basic services of the QuickFit application
    \ingroup qf3plugintools
*/
class QFPluginReportWrapper {
    public:
        /** Default destructor */
        virtual ~QFPluginReportWrapper() {}

        /** \copydoc QFPluginReportWrapper::log_text()  */
        virtual void log_text(QString message)=0;
        /** \copydoc QFPluginReportWrapper::log_warning()  */
        virtual void log_warning(QString message)=0;
        /** \copydoc QFPluginReportWrapper::log_error()  */
        virtual void log_error(QString message)=0;
        /** \copydoc QFPluginReportWrapper::setStatusMessage()  */
        virtual void setStatusMessage(QString message)=0;
        /** \copydoc QFPluginReportWrapper::setProgressRange()  */
        virtual void setProgressRange(int minP, int maxP)=0;
        /** \copydoc QFPluginReportWrapper::setProgress()  */
        virtual void setProgress(int value)=0;
        /** \copydoc QFPluginReportWrapper::incProgress()  */
        virtual void incProgress()=0;
};

#endif // QFPLUGINREPORTWRAPPER_H
