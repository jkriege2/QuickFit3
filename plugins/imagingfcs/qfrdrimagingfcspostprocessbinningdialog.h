#ifndef QFRDRIMAGINGFCSPOSTPROCESSBINNINGDIALOG_H
#define QFRDRIMAGINGFCSPOSTPROCESSBINNINGDIALOG_H

#include <QDialog>

namespace Ui {
class QFRDRImagingFCSPostProcessBinningDialog;
}

class QFRDRImagingFCSPostProcessBinningDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QFRDRImagingFCSPostProcessBinningDialog(QWidget *parent = 0);
    ~QFRDRImagingFCSPostProcessBinningDialog();
    void init(int bin=2, bool interleaved=false);
    int getBin() const;
    bool getInterleaved() const;
    bool getOtherRDR() const;
protected slots:
    void on_chkInterleaved_toggled(bool checked);
    
private:
    Ui::QFRDRImagingFCSPostProcessBinningDialog *ui;
};

#endif // QFRDRIMAGINGFCSPOSTPROCESSBINNINGDIALOG_H
