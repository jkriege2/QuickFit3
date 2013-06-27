#ifndef QFSimpleTCSPCImporterPlugin_H
#define QFSimpleTCSPCImporterPlugin_H

#include "qfpluginimporters.h"

/*!
    \defgroup qf3importerplugins_importers_picoquant Importer Plugin for simple TCSPC files
    \ingroup qf3importerplugins
*/



/*! \brief plugin class for simple TCSPC Importers
    \ingroup qf3importerplugins_importers_picoquant
*/
class QFSimpleTCSPCImporterPlugin : public QObject, public QFPluginImporters {
        Q_OBJECT
        Q_INTERFACES(QFPluginImporters)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFSimpleTCSPCImporterPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFSimpleTCSPCImporterPlugin() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("Simple TCSPC File Importer Plugin");
        }

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("this plugin provides functionality to import simple TCSPC files");
        }

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger, Gabor Moscar");
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2013 by Jan W. Krieger, Gabor Moscar");
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        }

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "importers_simpletcspcimporter";
        }

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            return ":/lib/importer_icon.png";
        }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFImporter* createImporter(QString id) const ;
		
		QF_PLUGIN
    private:
};

#endif // QFSimpleTCSPCImporterPlugin_H
