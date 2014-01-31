#ifndef QFRDRTABLEPLOTSETTINGSWIDGET_H
#define QFRDRTABLEPLOTSETTINGSWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "tools.h"
#include "qfrdrtable.h"
#include "QToolTip"
#include "qfmathparser.h"
#include "qffunctionreferencetool.h"

namespace Ui {
    class QFRDRTablePlotSettingsWidget;
}
class QFRDRTablePlotSettingsWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTablePlotSettingsWidget(QWidget *parent);
        ~QFRDRTablePlotSettingsWidget();

        void setRecord(QFRDRTable* record, int plot);

        bool getLogX() const;
        bool getLogY() const;

        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
    public slots:
        void setXRange(double xmin, double xmax);
        void setYRange(double ymin, double ymax);
        void graphDataChanged(int r);
        void plotDataChanged();
        void doUpdateGraph();

        void connectWidgets();
        void disconnectWidgets();

    signals:
        void plotTitleChanged(int plot, QString title);
        void updateGraph();
        void autoscaleX();
        void autoscaleY();
    protected slots:
        void on_btnLoadSystem_clicked();
        void on_btnSaveSystem_clicked();
        void on_btnAutoscaleXY_clicked();
        void doAutoscaleX();
        void doAutoscaleY();

    protected:

    private:
        Ui::QFRDRTablePlotSettingsWidget *ui;

        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEPLOTSETTINGSWIDGET_H
