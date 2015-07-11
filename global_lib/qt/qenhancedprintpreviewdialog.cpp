#include "qenhancedprintpreviewdialog.h"
#include <QLayout>
QEnhancedPrintPreviewDialog::QEnhancedPrintPreviewDialog(QWidget *parent) :
    QPrintPreviewDialog(parent)
{
}

void QEnhancedPrintPreviewDialog::addBottomWidget(QWidget *wi)
{
    QLayout* lay=layout();
    lay->addWidget(wi);
}

void QEnhancedPrintPreviewDialog::addTopWidget(QWidget *wi)
{
    QLayout* lay=layout();
    QBoxLayout* bl=qobject_cast<QBoxLayout*>(lay);
    if (bl) {
        bl->insertWidget(0, wi);
    } else {
        QList<QLayoutItem*> widOld;
        while (lay->count()>0) {
            widOld.append(lay->takeAt(0));
        }
        lay->addWidget(wi);
        for (int i=0; i<widOld.size(); i++) {
            lay->addItem(widOld[i]);
        }
    }
}

void QEnhancedPrintPreviewDialog::addBottomLayout(QLayout *l)
{
    QWidget* w=new QWidget(this);
    w->setLayout(l);
    addBottomWidget(w);
}

void QEnhancedPrintPreviewDialog::addTopLayout(QLayout *l)
{
    QWidget* w=new QWidget(this);
    w->setLayout(l);
    addTopWidget(w);
}
