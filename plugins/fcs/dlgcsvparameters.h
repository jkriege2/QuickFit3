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
        dlgCSVParameters(QWidget* parent=NULL, QString startswith=QString(""), QString columnSeparator=QString(","), QString commentStart=QString("#"), double timefactor=1, int startInLine=1);
        /** Default destructor */
        virtual ~dlgCSVParameters();
        /** \brief display a preview of the file */
        void setFileContents(const QString& filename);

        char get_column_separator() { return column_separator; };
        char get_comment_start() { return comment_start; };
        double get_timefactor() { return timefactor; };
        QString get_startswith() { return startswith; };
        int get_firstLine() { return firstline; }
    protected:
        char column_separator;
        char comment_start;
        double timefactor;
        QString startswith;
        int firstline;
    private slots:
        void checkValues();
};

#endif // DLGCSVPARAMETERS_H
