#ifndef QFPLUGINFITFUNCTION_H
#define QFPLUGINFITFUNCTION_H

#include "lib_imexport.h"
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qffitfunction.h"
#include "qfplugin.h"

/** \brief virtual interface class for all QuickFit 3 fit function plugins
    \ingroup qf3fitfunplugins

    These plugins may contain several fitting functions. The function getIDs() returns a QStringList
    with all implemented plugin IDs.
*/
class QFPluginFitFunction: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginFitFunction() {}

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const =0;
};

Q_DECLARE_INTERFACE(QFPluginFitFunction,
                     "www.dkfz.de.b040.quickfit3.QFPluginFitFunction/1.0")


#endif // QFPLUGINFITFUNCTION_H
