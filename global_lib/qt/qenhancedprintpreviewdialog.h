#ifndef QENHANCEDPRINTPREVIEWDIALOG_H
#define QENHANCEDPRINTPREVIEWDIALOG_H

#include <QPrintPreviewDialog>

class QEnhancedPrintPreviewDialog : public QPrintPreviewDialog
{
        Q_OBJECT
    public:
        explicit QEnhancedPrintPreviewDialog(QWidget *parent = 0);
        void addBottomWidget(QWidget* wi);
        void addTopWidget(QWidget* wi);
        void addBottomLayout(QLayout* l);
        void addTopLayout(QLayout* l);
    signals:
        
    public slots:
        
};

#endif // QENHANCEDPRINTPREVIEWDIALOG_H
