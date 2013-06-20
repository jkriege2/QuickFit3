x#ifndef QFGENERALFITFUNCTIONSPLUGIN_H
#define QFGENERALFITFUNCTIONSPLUGIN_H

#include "qfpluginfitfunction.h"
#include "qffitfunctiongeneralsigmoid.h"

/*!
    \defgroup qf3fitfunp_fitfunction_general Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunction_general
*/
class QFGeneralFitFunctionsPlugin : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFGeneralFitFunctionsPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFGeneralFitFunctionsPlugin() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunction_general";
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

#endif // QFGENERALFITFUNCTIONSPLUGIN_H
