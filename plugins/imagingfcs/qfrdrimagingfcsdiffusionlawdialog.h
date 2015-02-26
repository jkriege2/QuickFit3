#ifndef QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H
#define QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

class QFRDRImagingFCSData; // forward

namespace Ui {
    class QFRDRImagingFCSDiffusionLawDialog;
}

class QFRDRImagingFCSDiffusionLawDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSDiffusionLawDialog(QWidget *parent = 0);
        ~QFRDRImagingFCSDiffusionLawDialog();

        void init(const QList<QFRDRImagingFCSData*>& recs, const QString &evalgroup);
        void setSelection(const bool* data, int width, int height);

    protected slots:
        void on_btnRecalc_clicked();
        void paramChanged();
        void recalcPlot();
        void on_btnHelp_clicked();
    private:
        Ui::QFRDRImagingFCSDiffusionLawDialog *ui;
        QList<QFRDRImagingFCSData*> recs;
        const bool* selImg;
        int width;
        int height;
        QString evalgroup;
        QList<QListWidgetItem*> rdrItems;
        struct fpProp {
            QString label;
            QString resultname;
            QString evalname;
            bool isD;
            bool isTau;
            bool isWxy;
            bool isAeff;
            bool isPixSize;
        };

        QList<fpProp > fitparams;
};

#endif // QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H
