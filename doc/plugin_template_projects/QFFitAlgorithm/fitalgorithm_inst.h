#ifndef HEADER_H
#define HEADER_H

#include "qfpluginfitalgorithm.h"
#include <levmar/levmar.h>

/*!
    \defgroup doxygen_GROUPNAME Levmar Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/






/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_levmar
*/
class QFFitAlgorithmInstPlugin : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
    public:
		QF_PLUGIN
	
        /** Default constructor */
        QFFitAlgorithmInstPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFFitAlgorithmInstPlugin() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const {
            return QString("target_id");
        };


        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr(/* plugin name */);
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr(/* plugin description */);
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString(/* plugin author */);
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr(/* plugin copyright */);
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr(/* plugin weblink */);
        };
		

        /** \brief plugin ID  */
        virtual QString getID() const {
            return "target_id";
        };

        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            return ":/plugin_fitfunc_incon.png";
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

#endif // HEADER_H
