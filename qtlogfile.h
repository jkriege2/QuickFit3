/*
  Name: qtlogfile.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


/** \file completertextedit.h
  * \ingroup sdff_editor
  *
  * A QTextEdit descendent with completer capabilities
  */

#ifndef QTLOGFILE_H
#define QTLOGFILE_H

#include "../lib/logfile.h"
#include <QTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QSettings>

/*! \brief A logging class for Qt which implements the features of LogFile in a QTextEdit (inherits both publucly).
           All logging to console is instead written to a QTextEdit. The file logging is unchanged.


  This class also introduces a store for the logging messages, i.e. all messages that were written out
  to a file will be stored internally (and cleared when the log widget is cleared). So when opening
  a file all messages logged so far will be written out to the file. This allows to define the log file
  AFTER creating the widget and to change the log file, while the logged messages are prevered.
  */

class QtLogFile : public LogFile, public QTextEdit
{
    public:
        /** \brief class constructor */
        QtLogFile(QWidget *parent = 0);
        /** \brief clear the text in the log widget */
        virtual inline void clearLog() { document()->clear(); setText(""); logStore.clear(); };
        /** \brief clear the text in the log store, but not in the widget */
        virtual inline void clearLogStore() { logStore.clear(); };
		/** \brief open a new log file and activate loggin to file
		 *
		 * This method also writes any information in the internal store to the file immediately,
		 * if the variable writeSTore is \c treu.
		 */
		virtual void open_logfile(std::string filename, bool writeStore);

        /** \brief save the widget settings to a QSettings object */
        void saveSettings(QSettings& s, QString namePrefix=QString(""));
        /** \brief read the widget settings from a QSettings object */
        void readSettings(QSettings& s, QString namePrefix=QString(""));

        virtual void log_warning(std::string message);
        virtual void log_error(std::string message);
        virtual void log_error_line();
        virtual void log_error_dline();
        virtual void log_error_header(std::string caption);
        virtual void log_error_dheader(std::string caption);
        virtual void log_warning_line();
        virtual void log_warning_dline();
        virtual void log_warning_header(std::string caption);
        virtual void log_warning_dheader(std::string caption);
    protected:
        virtual void raw_log_text(std::string message);
    private:
        /** \brief color of a standard text message */
        QColor textColor;
        /** \brief color of an error message */
        QColor errorColor;
        /** \brief color of a warning message */
        QColor warningColor;
        /** \brief color of an information message */
        QColor infoColor;
        /** \brief the current output color (initially: textColor) */
        QColor currentColor;
        /** \brief font family to use for messages */
        QString fontName;
        /** \brief font size to use for messages */
        int fontSize;
        /** \brief indicates whether to use fixed pitch */
        bool fixedPitch;
        /** \brief store for the loged messages text */
        std::vector<std::string> logStore;
        /** \brief \c true, when the current message should be typeset in bold letters */
        bool currentBold;
};

#endif // QTLOGFILE_H
