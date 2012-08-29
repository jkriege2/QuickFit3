#ifndef QFFITFUNCTIONSLIGHTSHEET_H
#define QFFITFUNCTIONSLIGHTSHEET_H

#include "qfpluginfitfunction.h"

/*!
    \defgroup qf3fitfunp_fitfunctions_lightsheet Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunctions_lightsheet
*/
class QFFitFunctionsLightsheet : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFFitFunctionsLightsheet(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFFitFunctionsLightsheet() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("SPIM: Lightsheet Profile Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("implements fit functions to assess the profile of a lightsheet for lightsheet (SPIM) microscopy");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_lightsheet";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;
		
        QF_PLUGIN
    private:
};

#endif // QFFITFUNCTIONSLIGHTSHEET_H
