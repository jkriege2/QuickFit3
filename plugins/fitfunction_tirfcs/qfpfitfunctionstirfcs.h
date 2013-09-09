#ifndef QFPFITFUNCTIONSTIRFCS_H
#define QFPFITFUNCTIONSTIRFCS_H

#include "qfpluginfitfunction.h"
#include <QStringList>
#include "qffitfunctionconfigforglobalfitinterface.h"
/*!
    \defgroup qf3fitfunp_fitfunctions_spimfcs TIR-FCS Fit Functions
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin TIR-FCS Fit Functions
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFPFitFunctionsTIRFCS : public QObject, public QFPluginFitFunction, public QFFitFunctionConfigForGlobalFitInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction QFFitFunctionConfigForGlobalFitInterface)
        Q_INTERFACES(QFPlugin)
    public:
        QF_PLUGIN


        /** Default constructor */
        QFPFitFunctionsTIRFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsTIRFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("imaging TIR-FCS/TIR-FCCS: Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements a set of fit functions used for imaging FCS/FCCS using the TIR imaging mode");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2013 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunction_tirfcs";
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

        int getGlobalFitConfigCount() const;
        GlobalFitConfig getGlobalFitConfig(int i) const;

    private:
};

#endif // QFPFITFUNCTIONSTIRFCS_H
