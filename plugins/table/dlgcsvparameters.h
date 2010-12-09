#ifndef DLGCSVPARAMETERS_H
#define DLGCSVPARAMETERS_H

#include <QDialog>
#include <QMessageBox>
#include "ui_dlg_csvparameters.h"


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
        dlgCSVParameters(QWidget* parent=NULL, QString columnSeparator=QString(","), QString decimalSeparator=QString("."), QString commentStart=QString("#"), QString headerStart=QString("#!"));
        /** Default destructor */
        virtual ~dlgCSVParameters();
        char column_separator;
        char decimal_separator;
        char comment_start;
        QString header_start;
    protected:
    private:
    private slots:
        void checkValues();
};

#endif // DLGCSVPARAMETERS_H
