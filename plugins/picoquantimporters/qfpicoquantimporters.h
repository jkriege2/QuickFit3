#ifndef QFPICOQUANTIMPORTERS_H
#define QFPICOQUANTIMPORTERS_H

#include "qfpluginimporters.h"
#include "qftcspcreaderpicoquant.h"

/*!
    \defgroup qf3importerplugins_importers_picoquant Importer Plugin for PicoQuant TTTR files
    \ingroup qf3importerplugins
*/



/*! \brief plugin class for PicoQuant TTTR Importers
    \ingroup qf3importerplugins_importers_picoquant
*/
class QFPicoQuantImporters : public QObject, public QFPluginImporters {
        Q_OBJECT
        Q_INTERFACES(QFPluginImporters)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFPicoQuantImporters(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPicoQuantImporters() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("PicoQuant TTTR Importer Plugin");
        }

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("this plugin provides functionality to import TTTR files produced by PicoQuant Hardware as TCSPC data");
        }

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012 by Jan W. Krieger");
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        }

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "importers_picoquant";
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

#endif // QFPICOQUANTIMPORTERS_H
