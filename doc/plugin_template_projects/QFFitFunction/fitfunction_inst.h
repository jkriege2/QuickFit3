#ifndef HEADER_H
#define HEADER_H

#include "qfpluginfitfunction.h"
#include "fitfunction_inst_f1.h"

/*!
    \defgroup doxygen_GROUPNAME Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup doxygen_GROUPNAME
*/
class QFPFitFunctionInst : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:
        QF_PLUGIN

        /** Default constructor */
        QFPFitFunctionInst(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionInst() {}

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
            return "target_id";
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

#endif // HEADER_H
