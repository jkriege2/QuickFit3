/*
  Name: qtlogfile2.h
  Copyright: (c) 2012
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


/** \file qtlogfile2.h
  * \ingroup tools_qt
  *
  * A more sofisticated replacement for QtLogFile
  */

#ifndef QTLOGFILE2_H
#define QTLOGFILE2_H

#include <QTextStream>
#include <QSettings>
#include <QString>
#include <QList>
#include <QFile>
#include <QDateTime>
#include <QListView>
#include <QAbstractTableModel>
#include <QVBoxLayout>
#include <QTableView>
#include "../tools.h"
#include "../lib_imexport.h"


/** \brief this is the indention increment when calling LogFile.inc_indent()
  * \ingroup logging
 */
#define LOGFILE2_INDENT_INC 2

/*! \brief A logging class for Qt which outputs to a  QtLogFile2Model descendent, which is displayed in a QListView and a file
    \ingroup logging

  This class replaces QtLogFile and adds more modern output and newer features

  This class also introduces a store for the logging messages, i.e. all messages that were written out
  to a file will be stored internally (and cleared when the log widget is cleared). So when opening
  a file all messages logged so far will be written out to the file. This allows to define the log file
  AFTER creating the widget and to change the log file, while the logged messages are preserved.
  */
class LIB_EXPORT QtLogFile2Model : public QAbstractTableModel {
        Q_OBJECT
    public:
        QtLogFile2Model(QObject* parent=NULL);

        virtual ~QtLogFile2Model();




        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;











        /** \brief clear the text in the log widget */
        virtual void clearLog();
        /** \brief open a new log file and activate loggin to file
         *
         * This method also writes any information in the internal store to the file immediately,
         * if the variable writeSTore is \c true.
         */
        virtual void open_logfile(QString filename);

        /** \brief save the widget settings to a QSettings object */
        void saveSettings(QSettings& s, QString namePrefix=QString(""));
        /** \brief read the widget settings from a QSettings object */
        void readSettings(QSettings& s, QString namePrefix=QString(""));


        /** \brief get the current log file name */
        GetMacro(QString, log_filename)

        /** \brief get whether we are logging to a file */
        GetMacro(bool, log_to_file)

        /** \brief set whether to append to a log_file (true) or to generate a new one (false) */
        SetMacro(bool, log_file_append)
        /** \brief get whether to append to a log file (true) or to generate a new one (false). */
        GetMacro(bool, log_file_append)

        /** \brief close the current log file (if one is opened) and deactivate loggin to file */
        virtual void close_logfile();

        /** \brief output a linebreak */
        virtual void log_linebreak();

        /** \brief output a horicontal line + linebreak */
        virtual void log_line();

        /** \brief output a double horicontal line + linebreak */
        virtual void log_dline();

        /** \brief output a horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_header(QString caption);

        /** \brief output a double horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_dheader(QString caption);

        /** \brief output a horicontal line + linebreak, as an error message */
        virtual void log_error_line();

        /** \brief output a double horicontal line + linebreak, as an error message */
        virtual void log_error_dline();

        /** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_header(QString caption);

        /** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_dheader(QString caption);

        /** \brief output a horicontal line + linebreak, as an warning message */
        virtual void log_warning_line();

        /** \brief output a double horicontal line + linebreak, as an warning message */
        virtual void log_warning_dline();

        /** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_header(QString caption);

        /** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_dheader(QString caption);

        /** \brief increase the indention depth */
        virtual void inc_indent();

        /** \brief decrease the indention depth */
        virtual void dec_indent();

    public slots:
        /** \brief output some text message */
        virtual void log_text(QString header, QString message);
        /** \brief output some warning message */
        virtual void log_warning(QString header, QString message);
        /** \brief output some error message */
        virtual void log_error(QString header, QString message);


        /** \brief output some text message */
        virtual void log_text(QString message);
        /** \brief output some warning message */
        virtual void log_warning(QString message);
        /** \brief output some error message */
        virtual void log_error(QString message);
        /** \brief output some error message  and adds a linebreak*/
        virtual void log_error_linebreak(QString message);
        /** \brief output some warning message  and adds a linebreak*/
        virtual void log_warning_linebreak(QString message);
        /** \brief output some text message  and adds a linebreak*/
        virtual void log_text_linebreak(QString message);
    signals:
        /** \brief indicates that a line has been appended to the model. */
        void messageAppended(const QModelIndex& message);


    protected:

        enum logType {
            logText,
            logWarning,
            logError
        };

        struct logMessage {
            logMessage();
            int indent;
            QString message;
            QString header;
            bool isLine;
            logType type;
            QDateTime timestamp;
        };

        QList<logMessage> logStore;

        /** \brief filename of the log file, standard is \c logfile.log */
        QString log_filename;

        /** \brief file handle for the log file */
        QFile* log_file;
        /** \brief used for text I/O to file */
        QTextStream log_file_stream;



        /** \brief determines whether message will be put into the file log_filename */
        bool log_to_file;

        /** \brief determines whether to append to a log file (true, default) or to generate a new one on each run (false) */
        bool log_file_append;

        /** \brief internal: this is set true if the last written character was a linebreak \c 0x0A */
        bool last_was_linebreak;

        /** \brief internal: the current depth of indention */
        int current_indent;

        /** \brief internal: add indention spaces if necessary */
        virtual void do_indent();

        /** \brief internal: add indention spaces after each linebreak in the string */
        virtual QString indentify(QString text);

        /** \brief internal: output some text message without indetify step  */
        virtual void raw_log_text(QString message);

    private:
        /** \brief color of a standard text message */
        QColor textColor;
        /** \brief color of an error message */
        QColor errorColor;
        /** \brief color of a warning message */
        QColor warningColor;
        /** \brief color of an information message */
        QColor infoColor;
        /** \brief font size to use for messages */
        int fontSize;
        /** \brief font family to use for messages */
        QString fontName;
        /** \brief indicates whether to use fixed pitch */
        bool fixedPitch;
        logType currentType;
};














/*! \brief A logging class for Qt which outputs to a  QtLogFile2Model descendent, which is displayed in a QListView and a file
    \ingroup logging

  This class replaces QtLogFile and adds more modern output and newer features

  This class also introduces a store for the logging messages, i.e. all messages that were written out
  to a file will be stored internally (and cleared when the log widget is cleared). So when opening
  a file all messages logged so far will be written out to the file. This allows to define the log file
  AFTER creating the widget and to change the log file, while the logged messages are preserved.
  */
class LIB_EXPORT QtLogFile2 : public QWidget {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QtLogFile2(QWidget *parent = 0);
        /** \brief class destructor */
        virtual ~QtLogFile2();
        /** \brief clear the text in the log widget */
        virtual void clearLog();
		/** \brief open a new log file and activate loggin to file
		 *
		 * This method also writes any information in the internal store to the file immediately,
		 * if the variable writeSTore is \c true.
		 */
        virtual void open_logfile(QString filename);

        /** \brief save the widget settings to a QSettings object */
        void saveSettings(QSettings& s, QString namePrefix=QString(""));
        /** \brief read the widget settings from a QSettings object */
        void readSettings(QSettings& s, QString namePrefix=QString(""));

        /** \brief get the current log file name */
        QString get_log_filename() const;
        /** \brief get whether we are logging to a file */
        bool get_log_to_file() const;
        /** \brief set whether to append to a log_file (true) or to generate a new one (false) */
        void set_log_file_append(bool append);
		/** \brief get whether to append to a log file (true) or to generate a new one (false). */
        bool get_log_file_append() const;

		/** \brief close the current log file (if one is opened) and deactivate loggin to file */
		virtual void close_logfile();

		/** \brief output a linebreak */
		virtual void log_linebreak();

		/** \brief output a horicontal line + linebreak */
		virtual void log_line();

		/** \brief output a double horicontal line + linebreak */
		virtual void log_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption */
        virtual void log_dheader(QString caption);

		/** \brief output a horicontal line + linebreak, as an error message */
		virtual void log_error_line();

		/** \brief output a double horicontal line + linebreak, as an error message */
		virtual void log_error_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an error message */
        virtual void log_error_dheader(QString caption);

		/** \brief output a horicontal line + linebreak, as an warning message */
		virtual void log_warning_line();

		/** \brief output a double horicontal line + linebreak, as an warning message */
		virtual void log_warning_dline();

		/** \brief output a horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_header(QString caption);

		/** \brief output a double horicontal line + linebreak which incorporates the text \c caption, as an warning message */
        virtual void log_warning_dheader(QString caption);

		/** \brief increase the indention depth */
        virtual void inc_indent();

		/** \brief decrease the indention depth */
        virtual void dec_indent();

    public slots:
		/** \brief output some text message */
		virtual void log_text(QString header, QString message);
		/** \brief output some warning message */
		virtual void log_warning(QString header, QString message);
		/** \brief output some error message */
		virtual void log_error(QString header, QString message);


		/** \brief output some text message */
		virtual void log_text(QString message);
		/** \brief output some warning message */
		virtual void log_warning(QString message);
		/** \brief output some error message */
		virtual void log_error(QString message);
		/** \brief output some error message  and adds a linebreak*/
        virtual void log_error_linebreak(QString message);
		/** \brief output some warning message  and adds a linebreak*/
        virtual void log_warning_linebreak(QString message);
		/** \brief output some text message  and adds a linebreak*/
        virtual void log_text_linebreak(QString message);
    protected slots:
        /** \brief indicates that a line has been appended to the model. */
        void messageAppended(const QModelIndex& message);

    private:

        /** \brief store for the loged messages text */
        QtLogFile2Model* logStore;

        /** \brief list view to display the logging data */
        QListView* list;

        /** \brief main layout */
        QVBoxLayout* lay;
};

#endif // QTLOGFILE2_H
