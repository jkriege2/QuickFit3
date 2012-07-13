#ifndef QFRDRIMAGINGFCSCOPYDATASELECTDIALOG_H
#define QFRDRIMAGINGFCSCOPYDATASELECTDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRImagingFCSCopyDataSelectDialog;
}

class QFRDRImagingFCSCopyDataSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFRDRImagingFCSCopyDataSelectDialog(QWidget *parent = 0);
    ~QFRDRImagingFCSCopyDataSelectDialog();

    bool copyParam() const;
    bool copyOvf() const;
    bool copyGOF() const;
    bool copyMask() const;
    bool copyID() const;
    bool copyCoordinates() const;
private:
    Ui::QFRDRImagingFCSCopyDataSelectDialog *ui;
};

#endif // QFRDRIMAGINGFCSCOPYDATASELECTDIALOG_H
