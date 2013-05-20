#ifndef QFTABLEGRAPHSETTINGS_H
#define QFTABLEGRAPHSETTINGS_H

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
    class QFTableGraphSettings;
}

class QFTableGraphSettings : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFTableGraphSettings(QWidget *parent = 0);
        ~QFTableGraphSettings();
        void setRecord(QFRDRTable* record, int plot);

        void rawDataChanged();

        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));

    protected slots:
        void on_btnFunctionHelp_clicked();

        void reloadColumns(QComboBox* combo);
        void graphDataChanged(QFRDRTable::GraphInfo &graph);
        void updatePlotWidgetVisibility(QFRDRTable::GraphInfo &graph);

        void connectWidgets();
        void disconnectWidgets();
    protected:

    private:
        Ui::QFTableGraphSettings *ui;
        QFRDRTable* current;
        int plot;
        bool updating;
        QFFunctionReferenceTool* functionRef;
};

#endif // QFTABLEGRAPHSETTINGS_H
