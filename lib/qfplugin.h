#ifndef QFPLUGIN_H
#define QFPLUGIN_H

#include "lib_imexport.h"
#include "qflib_version.h"
#include <QIcon>
#include <QString>

class QFPluginEvaluationItem; // forward
class QFPluginRawDataRecord; // forward
class QFExtension; // forward

/** \brief this interface allows to implement several plugins inside a single DLL and is part of RDR, evaluation and extension plugins!
    \ingroup qf3plugins

    This defines a basic set of identifying function that any plugin has to implement. Derive your plugins from
    QFPluginBase, as it already implements some of the functions in this interface (namely the functions that cope
    with the API version!)
*/
class QFPluginAdditionalPluginsInterface {
    public:
        /** Default destructor */
        virtual ~QFPluginAdditionalPluginsInterface() {}

        /** \brief allows to implement more than one RDR plugin within a DLL:
          *        The "main" plugin retuns a list with additional plugins in this function.
          *        Note that they HAVE to have unique IDs, as all plugins.
          *
          * This function will be called once after the plugin has been registered and any returned plugin
          * instances in the list will be registered as well. Note that if the main plugin has ID \c "id1",
          * the additional plugins have IDs \c "id1_1" and \c "id1_2" and the DLLs name is \c "myDLL.dll", then the
          * help for subplugin id1_1 will be found in \c ./help/plugins/myDLL/id1_1.html ...
          *
          * This method has a default implementation in:
          *    - QFPluginRawDataRecordBase
          *    - QFPluginEvaluationItemBase
          *    - QFExtensionBase;
          * which returns an empty list!
          */
        virtual QList<QFPluginRawDataRecord*> getAdditionalRDRPlugins() const=0;

        /** \brief allows to implement more than one evaluation plugin within a DLL:
          *        The "main" plugin retuns a list with additional plugins in this function.
          *        Note that they HAVE to have unique IDs, as all plugins.
          *
          * This function will be called once after the plugin has been registered and any returned plugin
          * instances in the list will be registered as well. Note that if the main plugin has ID \c "id1",
          * the additional plugins have IDs \c "id1_1" and \c "id1_2" and the DLLs name is \c "myDLL.dll", then the
          * help for subplugin id1_1 will be found in \c ./help/plugins/myDLL/id1_1.html ...
          *
          * This method has a default implementation in:
          *    - QFPluginRawDataRecordBase
          *    - QFPluginEvaluationItemBase
          *    - QFExtensionBase;
          * which returns an empty list!
          */
        virtual QList<QFPluginEvaluationItem*> getAdditionalEvaluationPlugins() const=0;

        /** \brief allows to implement more than one evaluation plugin within a DLL:
          *        The "main" plugin retuns a list with additional plugins in this function.
          *        Note that they HAVE to have unique IDs, as all plugins.
          *
          * This function will be called once after the plugin has been registered and any returned plugin
          * instances in the list will be registered as well. Note that if the main plugin has ID \c "id1",
          * the additional plugins have IDs \c "id1_1" and \c "id1_2" and the DLLs name is \c "myDLL.dll", then the
          * help for subplugin id1_1 will be found in \c ./help/plugins/myDLL/id1_1.html ...
          *
          * This method has a default implementation in:
          *    - QFPluginRawDataRecordBase
          *    - QFPluginEvaluationItemBase
          *    - QFExtensionBase;
          * which returns an empty list!
          */
        virtual QList<QFExtension*> getAdditionalExtensionPlugins() const=0;

};

Q_DECLARE_INTERFACE(QFPluginAdditionalPluginsInterface,
                     "www.dkfz.de.b040.quickfit3.QFPluginAdditionalPluginsInterface/1.0")




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
