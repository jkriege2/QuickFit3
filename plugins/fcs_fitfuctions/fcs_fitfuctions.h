#ifndef FCS_FITFUCTIONS_H
#define FCS_FITFUCTIONS_H

#include "qfpluginfitfunction.h"
#include "qffitfunctionfcsmultidiffusion.h"
#include "qffitfunctionfcsadiff.h"
#include "qffitfunctionfcsdiff.h"
#include "qffitfunctionfcssimplediff.h"
/*!
    \defgroup qf3fitfunp_fcs FCS Fitting Function Plugin
    \ingroup qf3fitfunplugins
*/







/*! \brief plugin class for FCS Fitting Functions
    \ingroup qf3fitfunp_fcs
*/
class QFPFitFunctionFCS : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:
        QF_PLUGIN

        /** Default constructor */
        QFPFitFunctionFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("FCS fit functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("a basic set of FCS fit functions");
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
            return tr("");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "qffcsfitfuncs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            return ":/plugin_fitfunc_incon.png";
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
    private:
};

#endif // FCS_FITFUCTIONS_H
