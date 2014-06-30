/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include "lib_imexport.h"
#include <QtGui>
#include "lib_imexport.h"


/*! \brief this class manages the overall program options (and may also display an options Dialog
    \ingroup qf3lib_settings

    This class is a singleton in the framework of a running QuickFit3 application. You can always get a pointer to the application's instance by
    calling the static method getInstance().
 */
class QFLIB_EXPORT ProgramOptions: public QObject {
        Q_OBJECT
    public:
        /** \brief Default constructor
         *
         * \param ini filename of the INI file of the application
         * \param parent pointer to the parent object
         * \param app the application this object manages the properties for
         */
        ProgramOptions( QString ini=QString(""), QObject * parent = 0, QApplication* app=NULL);

        /** \brief Default destructor */
        virtual ~ProgramOptions();

        /** \brief Access languageID (which language to use for display) */
        inline QString getLanguageID() { return languageID; }

        /** \brief Access languageID (which language to use for display) */
        inline QString getIniFilename() { return iniFilename; }

        /** \brief return the currently used stylesheet */
        inline QString getStylesheet() { return stylesheet; }

        /** \brief return the currently used style */
        inline QString getStyle() { return style; }

        /** \brief return a pointer to a QSettings object that may be used to save config data throughout the application */
        inline QSettings* getQSettings() { settings->sync(); return settings; }

        /** \brief return the maximum number of threds */
        int getMaxThreads() { return maxThreads; }

        inline QString getCurrentRawDataDir() { return currentRawDataDir; }
        void setCurrentRawDataDir(QString d);


        /** \brief set languageID (which language to use for display) */
        void setLanguageID(QString id);

        /** \brief set the currently used stylesheet */
        void setStylesheet(QString st);

        /** \brief set the currently used style */
        void setStyle(QString st);

        /** \brief set the maximum number of threds */
        void setMaxThreads(int threads);

        /** \brief autosave project every X minutes */
        int getAutosave() { return autosave; }
        /** \brief autosave project every X minutes (0 == off) */
        void setAutosave(int interval);

        /** \brief the directory in which to save configuration data, see \ref qf3whereiswhat */
        QString getConfigFileDirectory() const;

        /** \brief the directory from which to read global configuration data (do not expect to necessarily have write access to this directory!), see \ref qf3whereiswhat */
        QString getGlobalConfigFileDirectory() const;
        /** \brief points to a directory containing the QuickFit plugins, see \ref qf3whereiswhat */
        QString getPluginDirectory() const;
        /** \brief points to a directory containing the QuickFit assets files, see \ref qf3whereiswhat */
        QString getAssetsDirectory() const;
        /** \brief points to a directory containing the QuickFit examples, see \ref qf3whereiswhat */
        QString getExamplesDirectory() const;
        /** \brief points to a directory containing the QuickFit main help, see \ref qf3whereiswhat */
        QString getMainHelpDirectory() const;
        /** \brief returns the directory of the QuickFit main application */
        QString getApplicationDirectory() const;


        void setProxyHost(const QString& host);
        void setProxyPort(quint16 port);
        quint16 getProxyPort() const;
        QString getProxyHost() const;
        int getProxyType() const;
        void setProxyType(int type);

        bool getUserSaveAfterFirstEdit() const;
        void setUserSaveAfterFirstEdit(bool set);
        bool getChildWindowsStayOnTop() const;
        void setChildWindowsStayOnTop(bool set);
        bool getHelpWindowsStayOnTop() const;
        void setHelpWindowsStayOnTop(bool set);
        bool getProjectWindowsStayOnTop() const;
        void setProjectWindowsStayOnTop(bool set);

        bool debugLogVisible() const;
        void setDebugLogVisible(bool visible);
    public slots:
        /** \brief this function reads the current settings from the internal QSettings object settings */
        void readSettings();
        /** \brief this function writes the current settings to the internal QSettings object settings */
        void writeSettings();


    signals:
        /** \brief emitted when any of the data is changed by the user */
        void configDataChanged();

        /** \brief emitted when the language has been changed */
        void languageChanged(QString languageID);

        /** \brief emitted when the stylesheet changed */
        void stylesheetChanged(QString stylesheet);

        /** \brief emitted when the style changed */
        void styleChanged(QString style);

    protected:



        /** \brief QSettings object used for config options I/O */
        QSettings* settings;
        /** \brief the application this object manages the properties for */
        QApplication* app;

        //QNetworkProxy proxy;
        quint16 proxyPort;
        QString proxyHost;
        int proxyType;

        /** \brief style to use in the application */
        QString style;
        /** \brief stylesheet to use in the application */
        QString stylesheet;
        /** \brief filename of INI file */
        QString iniFilename;
        /** \brief language of the user interface */
        QString languageID;
        /** \brief last dir to load a raw data from */
        QString currentRawDataDir;
        /** \brief directory of the QuickFit executable (as this circumvents problems with MacOS bundles ... use this instead of QApplication::applicationDirPath() )*/
        QString appDir;
        /** \brief indicates whether to ask the user to save the project after the first edit operation */
        bool userSaveAfterFirstEdit;
        /** \brief indicates whether child windows (RDR, Eval) are set to stay on top */
        bool childWindowsStayOnTop;
        /** \brief indicates whether help windows are set to stay on top */
        bool helpWindowsStayOnTop;
        /** \brief indicates whether project window is set to stay on top */
        bool projectWindowStayOnTop;

        /** \brief maximum number of threads */
        int maxThreads;
        /** \brief autosave interval in minutes (0=off) */
        int autosave;
        /** \brief the directory in which to save configuration data, see \ref qf3whereiswhat */
        QString configDir;
        /** \brief the directory from which to load global configuration data, see \ref qf3whereiswhat */
        QString globalConfigDir;
        /** \brief assetsDir is a directory that contains QuickFit assets, like Help-Files, additional needed files, lik
         *         data files ... that are NOT changed by QuickFit, see \ref qf3whereiswhat */
        QString assetsDir;
        QString examplesDir;
        /** \brief points to a directory containing the QuickFit plugins, see \ref qf3whereiswhat */
        QString pluginsDir;

        bool m_debugLogVisible;

    private:
        static  ProgramOptions* inst;

   public:

        static ProgramOptions* getInstance() {
            return inst;
        }

        static QSettings* staticGetQSettings() {
            if (!inst) return NULL;
            return inst->settings;
        }

        /** \brief read a value from the main QSettings object */
        static QVariant getConfigValue(const QString& name, QVariant defaultValue=QVariant()) {
            if (!inst) return QVariant();
            if (!inst->settings) return QVariant();
            inst->settings->sync();
            return inst->settings->value(name, defaultValue);
        }

        /** \brief set a value in the main QSettings object */
        static void setConfigValue(const QString& name, QVariant value) {
            if (!inst) return ;
            if (!inst->settings) return;
            inst->settings->setValue(name, value);
            inst->settings->sync();
        }
};


#define ProgramOptionsSetQNetworkProxy(proxy) { \
    proxy.setHostName(ProgramOptions::getInstance()->getProxyHost()); \
    proxy.setPort(ProgramOptions::getInstance()->getProxyPort()); \
    proxy.setType((QNetworkProxy::ProxyType)ProgramOptions::getInstance()->getProxyType()); \
}

#endif // PROGRAMOPTIONS_H
