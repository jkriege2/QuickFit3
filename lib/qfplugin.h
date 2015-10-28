/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFPLUGIN_H
#define QFPLUGIN_H

#include "lib_imexport.h"
#include "qflib_version.h"
#include <QIcon>
#include <QString>
#include <QtPlugin>
#include <QVariant>
#include "qfaction.h"
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


/** \brief this interface allows to send text-encoded commands to other plugins
    \ingroup qf3plugins
*/
class QFPluginCommandsInterface {
    public:
        /** Default destructor */
        virtual ~QFPluginCommandsInterface() {}

        /** \brief send a text command to another plugin */
        virtual QVariant sendPluginCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant())=0;

};

Q_DECLARE_INTERFACE(QFPluginCommandsInterface,
                     "www.dkfz.de.b040.quickfit3.QFPluginCommandsInterface/1.0")




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

#define QF_PLUGIN  \
    public: \
        virtual void getQFLibVersion(int& major, int& minor) const { \
            major=QF3LIB_APIVERSION_MAJOR; \
            minor=QF3LIB_APIVERSION_MINOR;\
        }


Q_DECLARE_INTERFACE(QFPlugin,
                     "www.dkfz.de.b040.quickfit3.QFPlugin/1.0")


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#  ifdef Q_EXPORT_PLUGIN2
#    undef Q_EXPORT_PLUGIN2
#  endif
#  define Q_EXPORT_PLUGIN2(a, b)
#endif

#endif // QFPLUGIN_H
