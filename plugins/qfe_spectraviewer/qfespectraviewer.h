/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFESPECTRAVIEWER_H
#define QFESPECTRAVIEWER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfpluginoptionsdialog.h"
#include <QThread>

/*!
    \defgroup qf3ext_qfe_spectraviewer QFExtension implementation
    \ingroup qf3extensionplugins
*/
class SpectrumManager;
class QFESpectraViewerDialog;
class QFESpectraViewer;

class QFESpectraViewerLoadThread: public QThread {
        Q_OBJECT
    public:
        explicit QFESpectraViewerLoadThread(SpectrumManager*manager, QFESpectraViewer* parent=NULL);
    signals:
        void slog_text(const QString& text);
        void enableAction(bool enabled);
    protected:
        //void log_text(const QString& text);
        SpectrumManager* manager;
        virtual void run();
        QString sv;
};


/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_spectraviewer
 */
class QFESpectraViewer : public QObject, public QFExtensionBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFPluginOptionsDialogInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFESpectraViewer")
#endif

    public:
        /** Default constructor */
        QFESpectraViewer(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFESpectraViewer();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_spectraviewer"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Spectra Viewer"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("displays fluorophore and filter spectra"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013-2015 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_spectraviewer/qfespectraviewer.png"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();


        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);


        SpectrumManager* getSpectrumManager() const;
        void reloadDatabases();
    public:
        static QString intReloadDatabases(SpectrumManager* manager, const QString &id);

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

	protected:
        QFPluginLogService* logService;
        QFESpectraViewerDialog* dlg;
        SpectrumManager* manager;
        QFESpectraViewerLoadThread* loadThread;
        QAction* actStartPlugin;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void showViewer();
        void doLog(const QString& text);

        void loadThreadFinished(bool enabled);

};

#endif // QFESPECTRAVIEWER_H
