#ifndef QFPLUGIN_H
#define QFPLUGIN_H

#include "lib_imexport.h"
#include "qflib_version.h"

/** \brief virtual interface class for all QuickFit 3 plugins
    \ingroup qf3plugins

    This defines a basic set of identifying function that any plugin has to implement. Derive your plugins from
    QFPluginBase, as it already implements some of the functions in this interface (namely the functions that cope
    with the API version!)
*/
class QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPlugin() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const=0;

        /** \brief name for the plugin */
        virtual QString getName() const =0;

        /** \brief short description for the plugin */
        virtual QString getDescription() const =0;

        /** \brief author the plugin */
        virtual QString getAuthor() const =0;

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const =0;

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const =0;

        /** \brief icon for the plugin */
        virtual QString getIconFilename() const =0;

        /** \brief returns the version of the plugin */
        virtual void getVersion(int& major, int& minor) const =0;

        /** \brief returns the version of the QuickFit lib that this plugin has been compiled against  */
        virtual void getQFLibVersion(int& major, int& minor) const =0;

};

#define QF_PLUGIN  virtual void getQFLibVersion(int& major, int& minor) const { \
            major=QF3LIB_APIVERSION_MAJOR; \
            minor=QF3LIB_APIVERSION_MINOR;\
        }


Q_DECLARE_INTERFACE(QFPlugin,
                     "www.dkfz.de.b040.quickfit3.QFPlugin/1.0")

#endif // QFPLUGIN_H
