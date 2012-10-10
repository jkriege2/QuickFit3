#ifndef DLGCOMPONENTINFO_H
#define DLGCOMPONENTINFO_H

#include <QDialog>
#include "qftablemodel.h"

class QFEDiffusionCoefficientCalculator; // forward
namespace Ui {
    class DlgComponentInfo;
}

class DlgComponentInfo : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgComponentInfo(QFEDiffusionCoefficientCalculator* plugin, int component, QWidget *parent = 0);
        ~DlgComponentInfo();

    public slots:
        void setComponent(int component);
        void showHelp();
        void on_cmbPlot_currentIndexChanged(int index);
    private:
        Ui::DlgComponentInfo *ui;
        QFEDiffusionCoefficientCalculator* plugin;
        QFTableModel* model;
};

#endif // DLGCOMPONENTINFO_H
