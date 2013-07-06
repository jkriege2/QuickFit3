#ifndef QFESpectraViewerDetectorEditor_H
#define QFESpectraViewerDetectorEditor_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraViewerDetectorEditor;
}

class QFESpectraViewerDetectorEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerDetectorEditor(QWidget *parent = 0);
        ~QFESpectraViewerDetectorEditor();

        void setFromData(const QString& ID, const SpectrumManager::DetectorData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        int addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL);
        SpectrumManager::DetectorData getData() const;
        QString getID() const;
        bool hasSpectrumChanged() const;

    protected slots:
        void on_btnClearSpectrum_clicked();
        void on_btnPasteSpectrum_clicked();
        void on_btnLoadSpectrumFile_clicked();
        void on_btnDeleteRow_clicked();
        void on_btnDeleteColumn_clicked();
        void setSpectrumChanged(bool changed=true);
    private:
        Ui::QFESpectraViewerDetectorEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        bool spectrumchanged;
        QFTableModel modSpectrum;
};

#endif // QFESpectraViewerDetectorEditor_H
