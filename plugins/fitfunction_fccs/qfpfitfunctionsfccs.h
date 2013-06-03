#ifndef QFPFITFUNCTIONSFCCS_H
#define QFPFITFUNCTIONSFCCS_H

#include "qfpluginfitfunction.h"
#include "qffitfunctionfccsnormaldiff3d.h"

/*!
    \defgroup qf3fitfunp_fitfunctions_fccs Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunctions_fccs
*/
class QFPFitFunctionsFCCS : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFPFitFunctionsFCCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsFCCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("FCCS Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements fit functions for Fluorescence Cross Correlation Spectroscopy (FCCS)");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012-2013 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_fccs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;

        QF_PLUGIN
    private:
};

#endif // QFPFITFUNCTIONSFCCS_H
