#ifndef QFFINDERRONEOUSCORRELATIONDLG_H
#define QFFINDERRONEOUSCORRELATIONDLG_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "qftools.h"

namespace Ui {
    class QFFindErroneousCorrelationDlg;
}

class QFFindErroneousCorrelationDlg : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFindErroneousCorrelationDlg(QWidget *parent = 0);
        ~QFFindErroneousCorrelationDlg();
        
        void init(QFRDRRunSelectionsInterface* runselection, QFRDRFCSDataInterface* fcs);

        QList<int> getInitSelection() const;
        QList<int> getMaskedRuns() const;

        bool isPreview() const;
        bool doClearOldMask() const;
    signals:
        void setSelection(QList<int> selection, bool clearOld);
    private:
        Ui::QFFindErroneousCorrelationDlg *ui;

        QFRDRRunSelectionsInterface* runselection;
        QFRDRFCSDataInterface* fcs;
        QList<int> initSelection, mask;

    protected slots:
        void performCalc();
        void on_spinLagMin_valueChanged(int value);
        void on_spinLagMax_valueChanged(int value);
        void on_spinHighQuantile_valueChanged(double value);
        void on_spinLowQuantile_valueChanged(double value);
};

#endif // QFFINDERRONEOUSCORRELATIONDLG_H
