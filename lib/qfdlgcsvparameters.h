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

#ifndef QFQFDlgCSVParameters_H
#define QFQFDlgCSVParameters_H

#include "qfdialog.h"
#include <QMessageBox>
#include "lib_imexport.h"
#include "qftablemodel.h"
#include "qcheckablestringlistmodel.h"
namespace Ui {
    class QFDlgCSVParameters; // forward
}

/*! \brief Dialog which allows to enter the parameters needed to import a CSV file (separators)
    \ingroup qf3lib_widgets

    After the dialog has been successfully closed, you may read the properties
    get_column_separator(), get_decimal_separator(), get_comment_start() and get_header_start() which contain the values
    entered in the dialog. The dialog also does basic error checking (the first three properties may not have
    the same value).

    If you set any of the parameter in the constructor to an empty char or string, the according widget will be disabled.

 */
class QFLIB_EXPORT QFDlgCSVParameters : public QFDialog
{
        Q_OBJECT
    public:
        /** Default constructor */
        QFDlgCSVParameters(QWidget* parent=NULL, QString columnSeparator=QString(","), QString decimalSeparator=QString("."), QString commentStart=QString("#"), QString headerStart=QString("#!"));
        /** Default destructor */
        virtual ~QFDlgCSVParameters();
        char get_column_separator() { return column_separator; }
        char get_decimal_separator() { return decimal_separator; }
        char get_comment_start() { return comment_start; }
        QString get_header_start() { return header_start; }
        void set_column_separator(char data) {  column_separator=data; }
        void set_decimal_separator(char data) {  decimal_separator=data; }
        void set_comment_start(char data) {  comment_start=data; }
        void set_header_start(const QString& data) {  header_start=data; }
        /** \brief display a preview of the file */
        void setFileContents(const QString& filename);
        void setShowColumnsSelection(bool en);

        inline QList<int> getChecked() const {
            return colslistCheckable.getChecked();
        }
        inline QVector<bool> getIsChecked() const {
            return colslistCheckable.getIsChecked();
        }
        inline bool isChecked(int i) const {
            return colslistCheckable.isChecked(i);
        }

        void addColSelComboBox(const QString& name);
        int getColSelComboBoxCurrentIndex(int i) ;


    public slots:
        void guessParameters();
    protected slots:
        void checkValues(bool doAccept=true);
        void on_btnTab_clicked();
        void reloadCSV();
    private:
        char column_separator;
        char decimal_separator;
        char comment_start;
        QString header_start;
        QString file_contents;
        QString filename;
        Ui::QFDlgCSVParameters* ui;
        QFTableModel* tabmodel;
        bool show_sel_columns;
        QCheckableStringListModel colslistCheckable;
        QCheckableStringListModel colslist;
        QList<QComboBox*> colCmb;
        int colselcombos;
        QStringList colCmbNames;
};

#endif // QFQFDlgCSVParameters_H
