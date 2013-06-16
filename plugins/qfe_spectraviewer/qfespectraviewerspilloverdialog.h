#ifndef QFESPECTRAVIEWERSPILLOVERDIALOG_H
#define QFESPECTRAVIEWERSPILLOVERDIALOG_H

#include <QDialog>
#include "qftablemodel.h"
#include "qfespectraviewerdialog.h"
#include "spectrummanager.h"
namespace Ui {
    class QFESpectraViewerSpilloverDialog;
}

class QFESpectraViewerSpilloverDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerSpilloverDialog(QWidget *parent = 0);
        ~QFESpectraViewerSpilloverDialog();

        void init(const QList<QFESpectraViewerPlotItem>& plotItems, SpectrumManager* manager);
        
    private:
        Ui::QFESpectraViewerSpilloverDialog *ui;
        QFTableModel model;
};

#endif // QFESPECTRAVIEWERSPILLOVERDIALOG_H
