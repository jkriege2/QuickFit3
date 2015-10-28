#ifndef QFDIALOG_H
#define QFDIALOG_H

#include <QWidget>
#include <QDialog>
#include "lib_imexport.h"


class QFLIB_EXPORT QFDialog : public QDialog
{
        Q_OBJECT
    public:
        explicit QFDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    public Q_SLOTS:
        virtual void open();
        virtual int exec();
    protected:
        void showEvent(QShowEvent * event);
};

#endif // QFDIALOG_H
