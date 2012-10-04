#ifndef DLG_QFFILEEDITDIALOG_H
#define DLG_QFFILEEDITDIALOG_H

#include <QDialog>
#include "lib_imexport.h"

namespace Ui {
    class QFFileEditDialog;
}

class QFLIB_EXPORT QFFileEditDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFileEditDialog(QWidget *parent = 0);
        ~QFFileEditDialog();
        
        void init(const QString& file, const QString& type, const QString& description, const QStringList& filesTypes );
        QString getFile() const;
        QString getType() const;
        QString getDescription() const;
    private:
        Ui::QFFileEditDialog *ui;
};

#endif // DLG_QFFILEEDITDIALOG_H
