/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef SPIMB040_H
#define SPIMB040_H

#include "qfextension.h"

#include "qfespimb040mainwindow2.h"
#include <QAction>

/*!
    \defgroup qf3ext_spimb040 SPIM Control Extension (B040, DKFZ Heidelberg)
    \ingroup qf3extensionplugins
*/

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) plugin class
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040 : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFESPIMB040")
#endif

    public:
        /** Default constructor */
        QFESPIMB040(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFESPIMB040();

        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return tr("ext_spimb040"); };

        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("B040 SPIM Control Extension"); };

        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("controls the SPIM microscope in dep. B040 of DKFZ, Heidelberg"); };

        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };

        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011-2014 by Jan Krieger"); };

        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };

        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/spimb040_logo.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };


        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
        QAction* actStartPlugin;
        QAction* actStartPluginNew;
        QAction* actStartPluginOld;

        QPointer<QFESPIMB040MainWindow2> main;

        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);

        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();

        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);

        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);
    protected slots:
        void startPlugin();
        void startPluginNew();
};

#endif // SPIMB040_H
