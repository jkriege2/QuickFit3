#ifndef QFPLUGINFITALGORITHM_H
#define QFPLUGINFITALGORITHM_H

#include <QtPlugin>
#include "lib_imexport.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include "qffitalgorithm.h"
#include "qfplugin.h"

/** \brief virtual interface class for all QuickFit 3 fit algorithm plugins
    \ingroup qf3fitalgplugins

    These plugins may contain several fitting functions. The function getIDs() returns a QStringList
    with all implemented plugin IDs.
*/
class QFLIB_EXPORT QFPluginFitAlgorithm: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginFitAlgorithm() {}

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const =0;
    protected:
    private:
};

Q_DECLARE_INTERFACE(QFPluginFitAlgorithm,
                     "www.dkfz.de.b040.quickfit3.QFPluginFitAlgorithm/1.0")

#endif // QFPLUGINFITALGORITHM_H
