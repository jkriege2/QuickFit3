#ifndef QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H
#define QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraViewerLightsourceEditor;
}

class QFESpectraViewerLightsourceEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerLightsourceEditor(QWidget *parent = 0);
        ~QFESpectraViewerLightsourceEditor();

        void setFromData(const QString& ID, const SpectrumManager::LightSourceData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        int addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL);
        SpectrumManager::LightSourceData getData() const;
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
        Ui::QFESpectraViewerLightsourceEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        bool spectrumchanged;
        QFTableModel modSpectrum;
};

#endif // QFESPECTRAVIEWERLIGHTSOURCEEDITOR_H
