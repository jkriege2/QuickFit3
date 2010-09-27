#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QtGui>

#include "optionsdialog.h"

/** \brief this class manages the overall program options (and may also display an options Dialog */
class ProgramOptions: public QObject {
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
        inline QSettings* getQSettings() { return settings; }

        /** \brief return the maximum number of threds */
        int getMaxTHreads() { return maxThreads; };

        inline QString getCurrentRawDataDir() { return currentRawDataDir; }
        inline void setCurrentRawDataDir(QString d) { currentRawDataDir=d; }

    public slots:
        /** \brief open options dialog
         *
         * When this modal dialog is closed by "OK", the settings are updated, the config directory is possibly
         * reread and the configDataChanged() signal is emitted (if any data changed)
         */
        void openSettingsDialog();


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
        /** \brief this function reads the current settings from the internal QSettings object settings */
        void readSettings();
        /** \brief this function writes the current settings to the internal QSettings object settings */
        void writeSettings();

        /** \brief QSettings object used for config options I/O */
        QSettings* settings;
        /** \brief the application this object manages the properties for */
        QApplication* app;



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
        /** \brief maximum number of threads */
        int maxThreads;

};

#endif // PROGRAMOPTIONS_H
