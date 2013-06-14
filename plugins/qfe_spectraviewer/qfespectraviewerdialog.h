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
    qfesFluorohpore=0
};

struct QFESpectraViewerPlotItem {
    QFESpectraViewerPlotItemType type;
    QString name;
    bool showExcitation;
    bool showEmission;
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
        void on_btnDelete_clicked();
        void spectrumSelected();
        void saveFromWidgets();
        void loadToWidgets();
    protected:
        Ui::QFESpectraViewerDialog *ui;
        QFESpectraViewer* plugin;
        SpectrumManager* manager;

        QFTableModel modFluorophoreData;

        int currentIndex;

        QList<QFESpectraViewerPlotItem> plotItems;
};

#endif // QFESPECTRAVIEWERDIALOG_H
