#ifndef QFPLUGINFITALGORITHM_H
#define QFPLUGINFITALGORITHM_H

#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qffitalgorithm.h"

/** \brief virtual interface class for all QuickFit 3 fit algorithm plugins
    \ingroup qf3fitalgplugins

*/
class QFPluginFitAlgorithm
{
    public:
        /** Default destructor */
        virtual ~QFPluginFitAlgorithm() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const=0;

        /** \brief name for the plugin */
        virtual QString getName() const=0;

        /** \brief short description for the plugin */
        virtual QString getDescription() const=0;

        /** \brief author the plugin */
        virtual QString getAuthor() const=0;

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const=0;

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const=0;

        /** \brief create an instance */
        virtual QFFitAlgorithm* create(QObject* parent) const =0;
    protected:
    private:
};

Q_DECLARE_INTERFACE(QFPluginFitAlgorithm,
                     "www.dkfz.de.b040.quickfit3.QFPluginFitAlgorithm/1.0")

#endif // QFPLUGINFITALGORITHM_H
