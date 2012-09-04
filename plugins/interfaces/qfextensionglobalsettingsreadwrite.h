#ifndef QFEXTENSIONGLOBALSETTINGSREADWRITE_H
#define QFEXTENSIONGLOBALSETTINGSREADWRITE_H



#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief Functions in this interface should be used to read and write global settings for hardware control QFExtensions
    \ingroup qf3extensionplugins

 */
class QFExtensionGlobalSettingsReadWrite {
    public:
        virtual ~QFExtensionGlobalSettingsReadWrite() {}

        /** \brief read the global settings from the given QSettings object. Each name is prepended the supplied \a prefix */
        virtual void readGlobalSettings(QSettings& settings, const QString& prefix=QString("")) const=0;
        /** \brief save the global settings to the given QSettings object. Each name is prepended the supplied \a prefix */
        virtual void writeGlobalSettings(QSettings& settings, const QString& prefix=QString("")) const=0;

};

Q_DECLARE_INTERFACE( QFExtensionGlobalSettingsReadWrite,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionGlobalSettingsReadWrite/1.0")

#endif // QFEXTENSIONGLOBALSETTINGSREADWRITE_H
