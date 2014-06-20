#ifndef QFQFDlgCSVParameters_H
#define QFQFDlgCSVParameters_H

#include <QDialog>
#include <QMessageBox>
#include "lib_imexport.h"

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
class QFLIB_EXPORT QFDlgCSVParameters : public QDialog
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
    public slots:
        void guessParameters();
    protected slots:
        void checkValues();
    private:
        char column_separator;
        char decimal_separator;
        char comment_start;
        QString header_start;
        Ui::QFDlgCSVParameters* ui;
};

#endif // QFQFDlgCSVParameters_H
