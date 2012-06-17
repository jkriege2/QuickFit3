#ifndef QFRDRPLOTEDITOR_H
#define QFRDRPLOTEDITOR_H

#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "qfrawdataeditor.h"
#include "qenhancedtableview.h"
#include "qfrawdatapropertyeditor.h"
#include "jkqtplotter.h"
#include "qfrdrtableplotwidget.h"

/*! \brief editor class for plots from data in the associated table
    \ingroup qf3rdrdp_table
*/
class QFRDRPlotEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRPlotEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent=NULL);
        /** Default destructor */
        virtual ~QFRDRPlotEditor();
    protected slots:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addPlot();
        void deleteCurrentPlot();
        void deletePlot(int i);
        void rebuildPlotWidgets();
    protected:
        QTabWidget* tabPlots;
        QAction* actAddPlot;
        QAction* actDeletePlot;
        QString currentPlotDir;
        QToolBar* tbMain;
        QMenu* menuEdit;
        QMenu* menuPlot;
        QList<QFRDRTablePlotWidget*> plotWidgets;
    private:
};

#endif // QFRDRPLOTEDITOR_H
