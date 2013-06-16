#ifndef QFESpectraVieweFilterEditor_H
#define QFESpectraVieweFilterEditor_H

#include <QDialog>
#include "spectrummanager.h"
#include "qftablemodel.h"

namespace Ui {
    class QFESpectraVieweFilterEditor;
}

class QFESpectraViewerFilterEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerFilterEditor(QWidget *parent = 0);
        ~QFESpectraViewerFilterEditor();

        void setFromData(const QString& ID, const SpectrumManager::FilterData& data, SpectrumManager *manager=NULL);
        QString addDataAndSpectrum(QSettings &database, SpectrumManager *manager=NULL);
        int addSpectrum(SpectrumManager *manager=NULL, QString *filenameOut=NULL);
        SpectrumManager::FilterData getData() const;
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
        Ui::QFESpectraVieweFilterEditor *ui;
        SpectrumManager *manager;
        int oldspectrum;
        bool spectrumchanged;
        QFTableModel modSpectrum;
};

#endif // QFESpectraVieweFilterEditor_H
