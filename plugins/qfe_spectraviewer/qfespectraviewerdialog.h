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
    qfesFilterBandpass=4
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
        void updatePlots();

        void loadSpectraConfig(QSettings& settings, const QString& prefix=QString(""));
        void saveSpectraConfig(QSettings& settings, const QString& prefix=QString(""));
    protected slots:
        void on_btnAddFluorophore_clicked();
        void on_btnAddFilter_clicked();
        void on_btnAddLightsource_clicked();
        void on_btnDelete_clicked();
        void spectrumSelected();
        void saveFromWidgets();
        void loadToWidgets();
        void updateItemPropertiesModel();
    protected:
        Ui::QFESpectraViewerDialog *ui;
        QFESpectraViewer* plugin;
        SpectrumManager* manager;

        QFTableModel modItemProperties;

        int currentIndex;

        QList<QFESpectraViewerPlotItem> plotItems;
};

#endif // QFESPECTRAVIEWERDIALOG_H
