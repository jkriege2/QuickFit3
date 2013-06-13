#ifndef QFESPECTRAVIEWERDIALOG_H
#define QFESPECTRAVIEWERDIALOG_H

#include <QDialog>
#include <QList>
#include <QSettings>
class QFESpectraViewer;

namespace Ui {
    class QFESpectraViewerDialog;
}

enum QFESpectraViewerPlotItemType {
    qfesFluorohpore=0
};

struct QFESpectraViewerPlotItem {
    QFESpectraViewerPlotItemType type;
    int spectrum;
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
    protected:
        Ui::QFESpectraViewerDialog *ui;
        QFESpectraViewer* plugin;

        QList<QFESpectraViewerPlotItem> plotItems;
};

#endif // QFESPECTRAVIEWERDIALOG_H
