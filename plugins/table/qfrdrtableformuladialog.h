#ifndef QFRDRTABLEFORMULADIALOG_H
#define QFRDRTABLEFORMULADIALOG_H

#include <QDialog>
#include "qfcompleterfromfile.h"
#include <QCompleter>
#include <QThread>
#include "qftablemodel.h"
#include <QStringListModel>
#include "qffunctionreferencetool.h"




namespace Ui {
    class QFRDRTableFormulaDialog;
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

class QFRDRTableFormulaDialog : public QDialog
{
        Q_OBJECT
        



    public:
        explicit QFRDRTableFormulaDialog(QWidget *parent = 0);
        ~QFRDRTableFormulaDialog();

        QString getExpression() const;
        void setExpression(const QString& exp);

   protected slots:
        void on_edtFormula_textChanged(QString text);
        void on_btnHelp_clicked();
        void on_lstFunctions_doubleClicked(const QModelIndex& index);
        void on_lstFunctions_clicked(const QModelIndex& index);

        void delayedStartSearch();
    protected:
        Ui::QFRDRTableFormulaDialog *ui;

        QFFunctionReferenceTool* functionRef;

        QString getFunctionTemplate(QString name);
        QString getFunctionHelp(QString name);

};



#endif // QFRDRTABLEFORMULADIALOG_H
