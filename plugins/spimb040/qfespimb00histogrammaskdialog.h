#ifndef QFESPIMB00HISTOGRAMMASKDIALOG_H
#define QFESPIMB00HISTOGRAMMASKDIALOG_H

#include <QDialog>

namespace Ui {
    class QFESPIMB00HistogramMaskDialog; // forward
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) dialog to set mask creation by histogram in QFESPIMB040CameraView
    \ingroup qf3ext_spimb040


 */
class QFESPIMB00HistogramMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFESPIMB00HistogramMaskDialog(QWidget *parent = 0);
    ~QFESPIMB00HistogramMaskDialog();

    int mode() const;
    void setMode(int m);
    int pixels() const;
    void setPixels(int pixels, int maxPixels);

private:
    Ui::QFESPIMB00HistogramMaskDialog *ui;
};

#endif // QFESPIMB00HISTOGRAMMASKDIALOG_H
