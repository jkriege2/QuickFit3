#ifndef QFPFITFUNCTIONSFCSDISTRIBUTION_H
#define QFPFITFUNCTIONSFCSDISTRIBUTION_H

#include "qfpluginfitfunction.h"
#include "qffitfunctionfcsdistributionloggaussian.h"

/*!
    \defgroup qf3fitfunp_fitfunctions_fcsdistribution Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunctions_fcsdistribution
*/
class QFPFitFunctionsFCSDistribution : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFPFitFunctionsFCSDistribution(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsFCSDistribution() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("FCS: Fit Functions with parameter distributions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements FCS fit functions, where e.g. the diffusion time follows a given distribution (normal, log-normal, ...)");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2011 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_fcsdistribution";
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

#endif // QFPFITFUNCTIONSFCSDISTRIBUTION_H
