#ifndef QFFITALGORITHMGSLPLUGIN_H
#define QFFITALGORITHMGSLPLUGIN_H

#include "qfpluginfitalgorithm.h"

/*!
    \defgroup qf3fitfunp_fit_gsl Levmar Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/






/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_levmar
*/
class QFFitAlgorithmGSLPluginPlugin : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
    public:
		QF_PLUGIN

        /** Default constructor */
        QFFitAlgorithmGSLPluginPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFFitAlgorithmGSLPluginPlugin() {}



        /** \brief name for the plugin */
        virtual QString getName() const {
            return QObject::tr("GSL fit and minimization algorithms");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return QObject::tr("makes the fit and minimization algorithms available in the GNU scientific library (GSL) available in Quickfit");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QObject::tr("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return QObject::tr("(c)2013 by Jan W. krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return QObject::tr("http://www.dkfz.de/Macromol/");
        };


        /** \brief plugin ID  */
        virtual QString getID() const {
            return "fit_gsl";
        };

        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitalg_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const;
    private:
};

#endif // QFFITALGORITHMGSLPLUGIN_H
