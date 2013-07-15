#ifndef QFPFITFUNCTIONSSPIMFCS_H
#define QFPFITFUNCTIONSSPIMFCS_H

#include "qfpluginfitfunction.h"
#include <QStringList>
#include "qffitfunctionconfigforglobalfitinterface.h"
/*!
    \defgroup qf3fitfunp_fitfunctions_spimfcs SPIM-FCS Fit Functions
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin SPIM-FCS Fit Functions
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFPFitFunctionsSPIMFCS : public QObject, public QFPluginFitFunction, public QFFitFunctionConfigForGlobalFitInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction QFFitFunctionConfigForGlobalFitInterface)
        Q_INTERFACES(QFPlugin)
    public:
        QF_PLUGIN


        /** Default constructor */
        QFPFitFunctionsSPIMFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsSPIMFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("SPIM-FCS/SPIM-FCCS: Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements a set of fit functions used for imaging FCS/FCCS using the SPIM imaging mode");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2011-2013 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_spimfcs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=5;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;

        int getGlobalFitConfigCount() const;
        GlobalFitConfig getGlobalFitConfig(int i) const;

    private:
};

#endif // QFPFITFUNCTIONSSPIMFCS_H
