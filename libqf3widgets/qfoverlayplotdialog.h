#ifndef QFOVERLAYPLOTDIALOG_H
#define QFOVERLAYPLOTDIALOG_H

#include <QDialog>
#include "qfgetplotdatainterface.h"
#include "libwid_imexport.h"

class QFPlotter; // forward
class QFMatchRDRFunctor;

namespace Ui {
    class QFOverlayPlotDialog;
}

class QFWIDLIB_EXPORT QFOverlayPlotDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFOverlayPlotDialog(QWidget *parent = 0);
        ~QFOverlayPlotDialog();

        QFPlotter* getPlotter() const;
        void clearPlots();
        void startAddingPlots();
        void addPlot(const QVector<double>& x, const QVector<double>& y, const QString& name, const QVector<double>& xerror=QVector<double>(), const QVector<double>& yerror=QVector<double>());
        void addPlot(const QFGetPlotdataInterface::GetPlotDataItem& plotData);
        void setPlotOptions(const QFGetPlotdataInterface::GetPlotPlotOptions& options);
        void endAddingPlots();
        void setLog(bool logX=false, bool logY=false);
        void setAxisLabel(const QString& labX, const QString& labY);

        static QFOverlayPlotDialog *collectOverlayPlot(QFMatchRDRFunctor* matchFunctor);
    protected slots:
        void showHelp();
        void closeClicked();
        void updatePlot();
    private:
        Ui::QFOverlayPlotDialog *ui;
};

#endif // QFOVERLAYPLOTDIALOG_H
