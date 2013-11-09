#ifndef QFRDRImageCOPYDATASELECTDIALOG_H
#define QFRDRImageCOPYDATASELECTDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRImageCopyDataSelectDialog;
}

class QFRDRImageCopyDataSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFRDRImageCopyDataSelectDialog(QWidget *parent = 0);
    ~QFRDRImageCopyDataSelectDialog();

    bool copyParam() const;
    bool copyOvf() const;
    bool copyParam2() const;
    bool copyMask() const;
    bool copyID() const;
    bool copyCoordinates() const;
private:
    Ui::QFRDRImageCopyDataSelectDialog *ui;
};

#endif // QFRDRImageCOPYDATASELECTDIALOG_H
