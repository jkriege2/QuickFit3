#ifndef QFQFDlgCSVParameters_H
#define QFQFDlgCSVParameters_H

#include <QDialog>
#include <QMessageBox>
#include "ui_qfdlg_csvparameters.h"


/*! \brief Dialog which allows to enter the parameters needed to import a CSV file (separators)
    \ingroup qf3lib_widgets

    After the dialog has been successfully closed, you may read the properties
    get_column_separator(), get_decimal_separator(), get_comment_start() and get_header_start() which contain the values
    entered in the dialog. The dialog also does basic error checking (the first three properties may not have
    the same value).

    If you set any of the parameter in the constructor to an empty char or string, the according widget will be disabled.

 */
class QFDlgCSVParameters : public QDialog, private Ui::QFDlgCSVParameters
{
        Q_OBJECT
    public:
        /** Default constructor */
        QFDlgCSVParameters(QWidget* parent=NULL, QString columnSeparator=QString(","), QString decimalSeparator=QString("."), QString commentStart=QString("#"), QString headerStart=QString("#!"));
        /** Default destructor */
        virtual ~QFDlgCSVParameters();
        char get_column_separator() { return column_separator; };
        char get_decimal_separator() { return decimal_separator; };
        char get_comment_start() { return comment_start; };
        QString get_header_start() { return header_start; };
        /** \brief display a preview of the file */
        void setFileContents(const QString& filename);
    protected:
    private slots:
        void checkValues();
    private:
        char column_separator;
        char decimal_separator;
        char comment_start;
        QString header_start;
};

#endif // QFQFDlgCSVParameters_H
