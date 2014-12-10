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

#ifndef DLGCSVPARAMETERS_H
#define DLGCSVPARAMETERS_H

#include <QDialog>
#include <QRegExp>
#include <QMessageBox>
#include "ui_dlg_csvparameters.h"
#include "qftools.h"


/*! \brief Dialog which allows to enter the parameters needed to import a CSV file (separators)
    \ingroup qf3rdrdp_table

  After the dialog has been successfully closed, you may read the public properties
 column_separator, decimal_separator, comment_start and header_start which contain the values
 entered in the dialog. The dialog also does basic error checking (the first three properties may not have
 the same value).

 */
class dlgCSVParameters : public QDialog, private Ui::dlgCSVParameters
{
        Q_OBJECT
    public:
        /** Default constructor */
        dlgCSVParameters(QWidget* parent=NULL, int mode=0, QString startswith=QString(""), QString endswith=QString(""), QString columnSeparator=QString(","), QString commentStart=QString("#"), double timefactor=1, int startInLine=1);
        /** Default destructor */
        virtual ~dlgCSVParameters();
        /** \brief display a preview of the file */
        void setFileContents(const QString& filename);

        char get_column_separator() { return column_separator; };
        char get_comment_start() { return comment_start; };
        double get_timefactor() { return timefactor; };
        QString get_startswith() { return startswith; };
        QString get_endswith() { return endswith; };
        int get_firstLine() { return firstline; }
        int get_mode() { return mode; }
    protected:
        char column_separator;
        char comment_start;
        double timefactor;
        QString startswith;
        QString endswith;
        int firstline;
        int mode;
        bool updating;
    protected slots:
        void checkValues();
        void showHelp();
        void saveConfig();
        void deleteConfig();
        void currentConfigChanged(const QString& name);
        void dataChanged();

        void on_btnTab_clicked();
    protected:
        QStringList getNames();
        QVariant getProperty(QString index, QString property, QVariant defaultValue);
        void setProperty(QString index, QString property, QVariant value);
        void loadConfigs();
};

#endif // DLGCSVPARAMETERS_H
