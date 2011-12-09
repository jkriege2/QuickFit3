#ifndef QFEXTENSIONSHUTTER_H
#define QFEXTENSIONSHUTTER_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control shutter devices
    \ingroup qf3extensionplugins


 */
class QFExtensionShutter {
    public:
        virtual ~QFExtensionShutter() {};

        virtual unsigned int  getShutterCount()  =0;
        virtual void shutterConnect(unsigned int shutter)=0;
        virtual void shutterDisonnect(unsigned int shutter)=0;
        /** \brief returns \c true if the device is connected */
        virtual bool isShutterConnected(unsigned int shutter) =0;
        virtual bool isShutterOpen(unsigned int shutter) =0;
        virtual void setShutterState(unsigned int shutter, bool opened)=0;
};
#endif // QFEXTENSIONSHUTTER_H
