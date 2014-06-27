#ifndef QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H
#define QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H

#include <QDialog>

namespace Ui {
class QFRDRImagingFCSCopyCorrAsRDRDialog;
}

class QFRDRImagingFCSCopyCorrAsRDRDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QFRDRImagingFCSCopyCorrAsRDRDialog(bool isFCCS, QWidget *parent = 0);
    ~QFRDRImagingFCSCopyCorrAsRDRDialog();
    bool getCopyACF() const;
    bool getCopyAllRuns() const;
    bool getCreateSelection() const;
    QString getSelectionName() const;
    
private:
    Ui::QFRDRImagingFCSCopyCorrAsRDRDialog *ui;
};

#endif // QFRDRIMAGINGFCSCOPYCORRASRDRDIALOG_H
