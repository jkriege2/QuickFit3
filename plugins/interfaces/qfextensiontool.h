#ifndef QFEXTENSIONTOOL_H
#define QFEXTENSIONTOOL_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>
#include <QAction>



/*! \brief QuickFit QFExtension implementing a tool
    \ingroup qf3extensionplugins
    


 */
class QFExtensionTool {
    public:
        virtual ~QFExtensionTool() {}

        /** \brief returns an action that may be used to start the tool */
        virtual QAction* getToolStartAction()=0;
        /** \brief starts the tool */
        virtual void startTool()=0;
        
};



Q_DECLARE_INTERFACE( QFExtensionTool,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionTool/1.0")

#endif // QFEXTENSIONTOOL_H
