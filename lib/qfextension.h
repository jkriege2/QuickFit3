/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFEXTENSION_H
#define QFEXTENSION_H

#include "lib_imexport.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include <QWidget>
#include <QString>
#include "qfproject.h"
#include <QtPlugin>
#include "qfplugin.h"

/** \brief virtual interface class for all QuickFit 3 extension plugins
    \ingroup qf3extensionplugins

    \note Do not use this directly when implementing extensions. Use QFExtensionBase instead,
          as it already implements some functionality (storage of commonly used pointers, ...)
*/

class QFExtension: public QFPlugin, public QFPluginAdditionalPluginsInterface {
    public:
        /** \brief class destructor */
        virtual ~QFExtension() {};

        /*! \brief initialize the plugin, when implementing plugins, do not overload this, but overload QFExtensionBase::initExtension() */
        virtual void init(QFPluginServices* services, QWidget* parentWidget)=0;

        /*! \brief deinitialize the plugin */
        virtual void deinit()=0;


        /** \brief set current project, when implementing plugins, do not overload this, but overload QFExtensionBase::projectChanged() */
        virtual void setProject(QFProject* project)=0;

        /** \brief read settings from ProgramOptions object, when implementing plugins, do not overload this, but overload QFExtensionBase::loadSettings() */
        virtual void readSettings(ProgramOptions* settings)=0;

        /** \brief write settings to ProgramOptions object, when implementing plugins, do not overload this, but overload QFExtensionBase::storeSettings() */
        virtual void writeSettings(ProgramOptions* settings)=0;
};


/** \brief virtual base class for all QuickFit 3 extension plugins
    \ingroup qf3extensionplugins

    Use this class as base when developing extension plugins. This class implements some of
    the methods from QFExtensiom, as they are generic to all plugins!

    When implementing a plugin, do not overwrite init(), but overload initExtension(). In
    addition you should not overwrite setProject() to react on a changing QFProject, but
    overload projectChanged().

*/
class QFLIB_EXPORT QFExtensionBase: public QFExtension {
    public:
        QF_PLUGIN

        /** \brief class destructor */
        virtual ~QFExtensionBase() {};

        /*! \copydoc QFExtension::init() */
        virtual void init(QFPluginServices* services, QWidget* parentWidget) {
            this->services=services;
            this->parentWidget=parentWidget;
            this->settings=services->getOptions();
            initExtension();
        };

        /** \copydoc QFExtension::setProject() */
        virtual void setProject(QFProject* project) {
            QFProject* oldp=this->project;
            this->project=project;
            projectChanged(oldp, project);
        };

        /** \copydoc QFExtension::readSettings() */
        virtual void readSettings(ProgramOptions* settings) {
            this->settings=settings;
            loadSettings(settings);
        }

        /** \copydoc QFExtension::writeSettings() */
        virtual void writeSettings(ProgramOptions* settings) {
            this->settings=settings;
            storeSettings(settings);
        }

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalRDRPlugins() */
        virtual QList<QFPluginRawDataRecord*> getAdditionalRDRPlugins() const { return QList<QFPluginRawDataRecord*>(); }

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalEvaluationPlugins() */
        virtual QList<QFPluginEvaluationItem*> getAdditionalEvaluationPlugins() const  { return QList<QFPluginEvaluationItem*>(); }

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalExtensionPlugins() */
        virtual QList<QFExtension*> getAdditionalExtensionPlugins() const { return QList<QFExtension*>(); }


    protected:
        QFPluginServices* services;
        QFProject* project;
        ProgramOptions* settings;
        QWidget* parentWidget;

        /** \brief this method is called whenever the current project changes */
        virtual void projectChanged(QFProject* oldProject, QFProject* project) {Q_UNUSED(oldProject);Q_UNUSED(project);};

        /*! \brief initialize the plugin in this method

            Use this method instead of init() when implementing some initialization code
        */
        virtual void initExtension() {};

        /** \brief read settings from given \a settings object */
        virtual void loadSettings(ProgramOptions* settings) {Q_UNUSED(settings);};

        /** \brief write settings to given \a settings object */
        virtual void storeSettings(ProgramOptions* settings) {Q_UNUSED(settings);};

};

Q_DECLARE_INTERFACE(QFExtension,
                     "www.dkfz.de.b040.quickfit3.QFExtension/1.1")


#endif // QFEXTENSION_H
