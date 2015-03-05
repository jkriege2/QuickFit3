#ifndef QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H
#define QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QPointer>
#include <QWidget>
#include "imagetools.h"

class QFRDRImagingFCSData; // forward

namespace Ui {
    class QFRDRImagingFCSDiffusionLawDialog;
}

class QFRDRImagingFCSDiffusionLawDialog : public QWidget
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSDiffusionLawDialog(QWidget *parent = 0);
        ~QFRDRImagingFCSDiffusionLawDialog();

    public slots:
        void setSelection(const bool* data, int selImgWidth, int selImgHeight);
        void setSelection(const QVector<bool>& data, int selImgWidth, int selImgHeight);
        void init(QFRDRImagingFCSData* currentRecord, const QString &evalgroup);
        void saveConfig();
        void loadConfig();
        void loadParams();
        void recalcPlot();
    protected slots:
        void on_btnRecalc_clicked();
        void on_btnClose_clicked();
        void paramChanged();
        void on_btnHelp_clicked();
        void on_btnTutorial_clicked();
        void on_btnSwap_clicked();
        void on_edtScale1_editingFinished();
        void on_edtScale2_editingFinished();
        void on_cmbMode_currentIndexChanged(int index);
    protected:
        bool dontWriteConfig;

        void init(const QList<QFRDRImagingFCSData*>& recs, const QString &evalgroup, const QList<bool>& proposedRecordSelection);
        Ui::QFRDRImagingFCSDiffusionLawDialog *ui;
        QList<QFRDRImagingFCSData*> recs;
        QPointer<QFRDRImagingFCSData> currentRecord;
        QVector<bool> selImg;
        int selImgWidth;
        int selImgHeight;
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
            bool isPixDistance;
        };

        QList<fpProp > fitparams;
};

#endif // QFRDRIMAGINGFCSDIFFUSIONLAWDIALOG_H
