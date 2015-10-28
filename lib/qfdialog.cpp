#include "qfdialog.h"

QFDialog::QFDialog(QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f)
{

}

void QFDialog::open()
{
    QDialog::open();
//#ifdef Q_OS_MAC
//    Qt::WindowFlags flags = windowFlags();
//    if (isModal()) {
//        flags &= ~(Qt::Sheet && (~Qt::Window));
//    }
//    flags |= Qt::WindowMinMaxButtonsHint;
//    setWindowFlags(flags);
//#endif
    setSizeGripEnabled(true);
}

int QFDialog::exec()
{
    int r=QDialog::exec();

    return r;
}

void QFDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
//#ifdef Q_OS_MAC
//    Qt::WindowFlags flags = windowFlags();
//    if (isModal()) {
//        flags &= ~(Qt::Sheet && (~Qt::Window));
//    }
//    flags |= Qt::WindowMinMaxButtonsHint;
//    setWindowFlags(flags);
//#endif
    setSizeGripEnabled(true);
}
