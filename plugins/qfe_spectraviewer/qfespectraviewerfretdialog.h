#ifndef QFESpectraViewerFRETDialog_H
#define QFESpectraViewerFRETDialog_H

#include <QDialog>
#include "qftablemodel.h"
#include "qfespectraviewerdialog.h"
#include "spectrummanager.h"
namespace Ui {
    class QFESpectraViewerFRETDialog;
}

class QFESpectraViewerFRETDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerFRETDialog(QWidget *parent = 0);
        ~QFESpectraViewerFRETDialog();

        void init(const QList<QFESpectraViewerPlotItem>& plotItems, SpectrumManager* manager);
    protected slots:
        void updateFRET();
        
    protected:
        QList<SpectrumManager::FluorophoreData> fluorophores;
        SpectrumManager *manager;
    private:
        Ui::QFESpectraViewerFRETDialog *ui;
        QFTableModel model;
};

#endif // QFESpectraViewerFRETDialog_H
