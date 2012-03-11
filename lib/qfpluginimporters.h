#ifndef QFPLUGINIMPORTERS_H
#define QFPLUGINIMPORTERS_H

#include "lib_imexport.h"
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qfimporter.h"
#include "qfplugin.h"

/** \brief virtual interface class for (all QuickFit 3 fit function plugins)file) importers in QuickFit 3.0
    \ingroup qf3importerplugins
*/
class QFLIB_EXPORT QFPluginImporters: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginImporters() {}

        /** \brief return the QFImporter's in this plugin */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFImporter instance for the given ID */
        virtual QFImporter* createImporter(QString id) const =0;
};

Q_DECLARE_INTERFACE(QFPluginImporters,
                     "www.dkfz.de.b040.quickfit3.QFPluginImporters/1.0")


#endif // QFPLUGINIMPORTERS_H
