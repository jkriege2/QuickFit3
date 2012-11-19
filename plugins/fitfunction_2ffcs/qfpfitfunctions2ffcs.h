#ifndef QFPFITFUNCTIONS2FFCS_H
#define QFPFITFUNCTIONS2FFCS_H

#include "qfpluginfitfunction.h"

/*!
    \defgroup qf3fitfunp_fitfunctions_2ffcs Fit Function Plugin for 2f-FCS
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunctions_2ffcs
*/
class QFPFitFunctions2FFCS : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFPFitFunctions2FFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctions2FFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("2-Focus-FCS (2f-FCS) Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements fit functions for Dual-Focus Fluorescence (Cross) Correlation Spectroscopy (2f-FCS)");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_2ffcs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;

        QF_PLUGIN
    private:
};

#endif // QFPFITFUNCTIONS2FFCS_H
