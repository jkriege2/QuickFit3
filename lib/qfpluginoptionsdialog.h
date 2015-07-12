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

#ifndef QFPLUGINOPTIONSDIALOG_H
#define QFPLUGINOPTIONSDIALOG_H

#include <QWidget>
#include "lib_imexport.h"
#include "qflib_version.h"
#include <QIcon>
#include <QString>
#include "qfplugin.h"


class ProgramOptions;
class QFPluginOptionsDialogInterface; // forward


/** \brief base class for options dialog panes, to be used together with QFPluginOptionsDialog
    \ingroup qf3plugins

    \see QFPluginOptionsDialog
*/
class QFLIB_EXPORT QFPluginOptionsWidget : public QWidget
{
        Q_OBJECT
    public:
        explicit QFPluginOptionsWidget(QObject* pluginObject, QWidget *parent = 0);

        /** \brief read the widget contents from the program options */
        virtual void readSettings(ProgramOptions* options)=0;
        /** \brief write the widget contents to the program options */
        virtual void writeSettings(ProgramOptions* options)=0;

    signals:

    public slots:

    protected:
        /** \brief QObject of the associated plugin */
        QObject* pluginObject;
        /** \brief pluginObject cast to QFPlugin  */
        QFPlugin* pluginPlugin;
        /** \brief pluginObject cast to QFPluginOptionsDialog  */
        QFPluginOptionsDialogInterface* pluginOptions;
};

/** \brief virtual interface class for QuickFit 3 plugins that want to display a pane in the main options dialog
    \ingroup qf3plugins

    If you want to create a config pane in the main options dialog, implement this interface in your QFPlugin class and use the function
    QFPluginTools
*/
class QFPluginOptionsDialogInterface {
    public:
        virtual ~QFPluginOptionsDialogInterface() {}

        /** \brief return the display name for the options pane */
        virtual QString pluginOptionsName() const=0;
        /** \brief return the display icon for the options pane */
        virtual QIcon pluginOptionsIcon() const=0;
        /** \brief create a widget to display in the options dialog */
        virtual QFPluginOptionsWidget* createOptionsWidget(QWidget* parent) =0;
};

Q_DECLARE_INTERFACE(QFPluginOptionsDialogInterface,
                     "www.dkfz.de.b040.quickfit3.QFPluginOptionsDialogInterface/1.0")



#endif // QFPLUGINOPTIONSDIALOG_H
