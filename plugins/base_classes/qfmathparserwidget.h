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

#ifndef QFMathParserWidget_H
#define QFMathParserWidget_H

#include <QDialog>
#include "qfcompleterfromfile.h"
#include <QCompleter>
#include <QThread>
#include <QStringListModel>
#include "qffunctionreferencetool.h"
#include "qfmathparser.h"



namespace Ui {
    class QFMathParserWidget;
}


/*! \brief dialog which allows to edit math expressions
    \ingroup qf3rdrdp_table

    This dialog parses (async) all HTML files (*.html, *.htm) in the plugin help directory, subdirectory "parserreference".
    In these files it expects a section like this:
    \code
<!-- func:sin -->
    <b><tt><!-- template -->sin(x)<!-- /template --></tt></b> - <i>sine function </i>:<br>
    calculates the sine of the argument (argument in radians)
<!-- /func:sin -->
    \endcode

    Here \c sin (in the \c <!--func:NAME--> tag) is the name of the function. It extracts the contents between this
    tag and the according end-tag as help for the function (displayed below the expression line edit) and the string
    (NO MARKUP!!!) bewteen the \c <!--template--> comment tags as function template for the autocomplete/function list
    on the rhs of the dialog.
*/

class QFMathParserWidget : public QWidget {
        Q_OBJECT
    public:
        explicit QFMathParserWidget(QWidget *parent = 0);
        ~QFMathParserWidget();

        QString getExpression() const;
        void setExpression(const QString& exp);
        void addExpressionDefaultWords(const QStringList& words);
        QFMathParser* getMathParser() const { return mp; }

   protected slots:
        void on_edtFormula_textChanged(QString text);
        void on_btnHelp_clicked();
        void on_lstFunctions_doubleClicked(const QModelIndex& index);
        void on_lstFunctions_clicked(const QModelIndex& index);

        void delayedStartSearch();
    protected:
        Ui::QFMathParserWidget *ui;

        QFMathParser* mp;

        QFFunctionReferenceTool* functionRef;

        QString getFunctionTemplate(QString name);
        QString getFunctionHelp(QString name);

};



#endif // QFMathParserWidget_H
