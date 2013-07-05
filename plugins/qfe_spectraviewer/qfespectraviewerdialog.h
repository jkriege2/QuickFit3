#ifndef QFESPECTRAVIEWERDIALOG_H
#define QFESPECTRAVIEWERDIALOG_H

#include <QDialog>
#include <QList>
#include <QSettings>
#include <QListWidget>
#include "qftablemodel.h"
class QFESpectraViewer;
class SpectrumManager;

namespace Ui {
    class QFESpectraViewerDialog;
}

enum QFESpectraViewerPlotItemType {
    qfesFluorohpore=0,
    qfesLightSourceSpectrum=1,
    qfesLightSourceSingleLine=2,
    qfesFilterSpectrum=3,
    qfesFilterBandpass=4,
    qfesFilterLongpass=5,
    qfesFilterShortpass=6,
    qfesFilterNotch=7
};

struct QFESpectraViewerPlotItem {
    QFESpectraViewerPlotItem(QFESpectraViewerPlotItemType type=qfesFluorohpore);
    QFESpectraViewerPlotItemType type;
    QString name;
    bool showExcitation;
    bool showEmission;
    bool filterIsExcitation;
    double centralWavelength;
    double spectralWidth;
    QString displayName;
};

class QFESpectraViewerDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESpectraViewerDialog(QFESpectraViewer* plugin, QWidget *parent = 0);
        ~QFESpectraViewerDialog();

        void writeSettings();
        void readSettings();

        void loadSpectraConfig(QSettings& settings, const QString& prefix=QString(""));
        void saveSpectraConfig(QSettings& settings, const QString& prefix=QString(""));
    protected slots:
        void updatePlots();
        void on_btnAddFluorophore_clicked();
        void on_btnAddFilter_clicked();
        void on_btnAddLightsource_clicked();
        void on_btnDelete_clicked();
        void spectrumSelected();
        void saveFromWidgets();
        void loadToWidgets();
        void updateItemPropertiesModel();
        void createLightSourceSpectrum();
        void on_btnEditLightsource_clicked();
        void createFilterSpectrum();
        void on_btnEditFilter_clicked();
        void createFluorophoreSpectrum();
        void on_btnEditFluorophore_clicked();
        void reloadComboboxes();
        void calcSpilloverTable();
        void on_btnMailFluorophore_clicked();
        void on_btnMailLightsource_clicked();
        void on_btnMailFilter_clicked();
        void on_cmbFilterType_currentIndexChanged(int i);
    protected:
        Ui::QFESpectraViewerDialog *ui;
        QFESpectraViewer* plugin;
        SpectrumManager* manager;

        QFTableModel modItemProperties;

        int currentIndex;

        QList<QFESpectraViewerPlotItem> plotItems;
};

#endif // QFESPECTRAVIEWERDIALOG_H