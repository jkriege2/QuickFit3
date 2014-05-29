#ifndef QFRDRTABLEAXISSETTINGSWIDGET_H
#define QFRDRTABLEAXISSETTINGSWIDGET_H

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
    class QFRDRTableAxisSettingsWidget;
}
class QFRDRTableAxisSettingsWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableAxisSettingsWidget(QWidget *parent);
        ~QFRDRTableAxisSettingsWidget();

        void loadPlotData(const QFRDRTable::AxisInfo &g);
        void storePlotData(QFRDRTable::AxisInfo &g);

        bool getLogX() const;

        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
        void saveToSettings(QSettings& settings, const QString& axis) const;
        void loadFromSettings(QSettings& settings, const QString& axis);
        QString getlabel() const;
        void setRecord(QFRDRTable *record, int graph);

    public slots:
        void setXRange(double xmin, double xmax);
        void setlabel(const QString& label);
        void doUpdateGraph();

        void connectWidgets();
        void disconnectWidgets();

    signals:
        void updateGraph();
        void autoscaleX();
        void dataChanged();
    protected slots:
        void on_btnAutoscaleX_clicked();
        void plotDataChanged();
        void updateComboboxes();

    protected:

        void reloadColumns(QComboBox *combo);
    private:
        Ui::QFRDRTableAxisSettingsWidget *ui;
        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEAXISSETTINGSWIDGET_H
