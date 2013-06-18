#ifndef QFESpectraVieweFluorophoreEditor_H
#define QFESpectraVieweFluorophoreEditor_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraVieweFluorophoreEditor;
}

class QFESpectraViewerFluorophoreEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerFluorophoreEditor(QWidget *parent = 0);
        ~QFESpectraViewerFluorophoreEditor();

        void setFromData(const QString& ID, const SpectrumManager::FluorophoreData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        void addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL, QString *filenameOut_fl=NULL, int* spec_abs=NULL, int* spec_fl=NULL);
        void addSpectrumFl(SpectrumManager *manager=NULL, QString *filenameOut_fl=NULL, int* spec_fl=NULL);
        void addSpectrumAbs(SpectrumManager *manager=NULL, QString *filenameOut=NULL, int* spec_abs=NULL);
        SpectrumManager::FluorophoreData getData() const;
        QString getID() const;
        bool hasSpectrumChanged() const;

    protected slots:
        void on_btnClearSpectrum_clicked();
        void on_btnPasteSpectrum_clicked();
        void on_btnLoadSpectrumFile_clicked();
        void on_btnDeleteRow_clicked();
        void on_btnDeleteColumn_clicked();
        void on_btnClearSpectrum_2_clicked();
        void on_btnPasteSpectrum_2_clicked();
        void on_btnLoadSpectrumFile_2_clicked();
        void on_btnDeleteRow_2_clicked();
        void on_btnDeleteColumn_2_clicked();
        void setSpectrumChanged(bool changed=true);
        void setSpectrumChanged2(bool changed=true);

    private:
        Ui::QFESpectraVieweFluorophoreEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        int oldspectrum_2;
        bool spectrumchanged;
        bool spectrumchanged_2;
        QFTableModel modSpectrum;
        QFTableModel modSpectrum_2;
};

#endif // QFESpectraVieweFluorophoreEditor_H
