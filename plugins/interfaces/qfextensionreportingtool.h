#ifndef QFEXTENSIONREPORTINGTOOL_H
#define QFEXTENSIONREPORTINGTOOL_H


#include <QtPlugin>
#include "qfpluginservices.h"
#include <stdint.h>
#include <QVariant>



/*! \brief QuickFit QFExtension implementing a tool that may have a return value
    \ingroup qf3extensionplugins
    
    \note this interface only has functions to get a return value, but as signals&slots may not be declared
          in interfaces does not have a signal, when the user selected a new values, so you'll have to poll
          every second (or so) to check whether there is a new value.

 */
class QFExtensionReportingTool {
    public:
        virtual ~QFExtensionReportingTool() {}

        /** \brief returns the last value selected by the user for the given name */
        virtual QVariant getReportingToolValue(const QString& name=QString(""))=0;
        
};



Q_DECLARE_INTERFACE( QFExtensionReportingTool,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionReportingTool/1.0")

#endif // QFEXTENSIONREPORTINGTOOL_H
