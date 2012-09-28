#ifndef QFBASICIMAGEIMPORTERS_H
#define QFBASICIMAGEIMPORTERS_H
#include "qfpluginservices.h"
#include "qfpluginimporters.h"
#include "qfimagereaderlibtiff.h"
#include "qfpluginoptionsdialog.h"
#include "qfimagereadertinytiff.h"

/*!
    \defgroup qf3importerplugins_importers_basicimages Importer Plugin
    \ingroup qf3importerplugins
*/



/*! \brief plugin class for Importers
    \ingroup qf3importerplugins_importers_basicimages
*/
class QFBasicImageImporters : public QObject, public QFPluginImporters, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginImporters QFPluginOptionsDialogInterface)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFBasicImageImporters(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFBasicImageImporters() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("basic image series importers");
        }

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin provides a set of common image series importers to all other plugins in QuickFit");
        }

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012 by Jan Krieger");
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        }

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "importers_basicimages";
        }

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            return ":/lib/importer_icon.png";
        }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        }

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFImporter* createImporter(QString id) const ;

        /** \brief return the display name for the options pane */
        QString pluginOptionsName() const;
        /** \brief return the display icon for the options pane */
        QIcon pluginOptionsIcon() const;
        /** \brief create a widget to display in the options dialog */
        QFPluginOptionsWidget* createOptionsWidget(QWidget* parent) ;

        QF_PLUGIN
    private:
};

#endif // QFBASICIMAGEIMPORTERS_H
