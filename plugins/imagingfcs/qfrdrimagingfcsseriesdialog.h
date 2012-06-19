#ifndef QFRDRIMAGINGFCSSERIESDIALOG_H
#define QFRDRIMAGINGFCSSERIESDIALOG_H
#include <QList>
#include <QDialog>
#include <QSettings>
#include "qfpluginservices.h"
#include "programoptions.h"

namespace Ui {
    class QFRDRImagingFCSSeriesDialog;
}

class QFRDRImagingFCSSeriesDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSSeriesDialog(QWidget *parent = 0);
        ~QFRDRImagingFCSSeriesDialog();
        int getParameter() const;
        QList<double> getValues() const;
        
    private:
        Ui::QFRDRImagingFCSSeriesDialog *ui;

        void closeEvent(QCloseEvent *event);
};

#endif // QFRDRIMAGINGFCSSERIESDIALOG_H
