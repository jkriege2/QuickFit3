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
#include "qfplotter.h"
#include "qfrdrtableplotwidget.h"


class QFRDRTable; // forward

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
    public slots:
        void rebuildPlotWidgets(bool keepPosition=false);
    signals:
        void performFit(int xCol, int yCol, int sigmaCol, int plot, QString function, bool xlog, bool ylog);
        void performRegression(int xCol, int yCol, int sigmaCol, int plot, bool xlog, bool ylog);
        void performRefit(int plot, int graph);
    protected slots:
        void doFit(int xCol, int yCol, int sigmaCol, int plot, QString function, bool xlog, bool ylog);
        void doRegression(int xCol, int yCol, int sigmaCol, int plot, bool xlog, bool ylog);
        void doRefit(int plot, int graph);
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
        void duplicatePlot();
        void movePlotUp();
        void movePlotDown();
        void deletePlot(int i);

        void clearPlotWidgets();
        void updatePlotName(int plot, QString title);
    public:
        /*! \brief execute an editor command

            These commands are defined:
              - \c showPlot: param1 is the name or index of the given plot. This will display the given (by param1) plot tab.
            .
          */
        virtual void sendEditorCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant());
    protected:

        QPointer<QFRDRTable> current;
        QTabWidget* tabPlots;
        QAction* actAddPlot;
        QAction* actDeletePlot;
        QAction* actDuplicatePlot;
        QAction* actMovePlotLeft;
        QAction* actMovePlotRight;
        QString currentPlotDir;
        QToolBar* tbMain;
        QMenu* menuEdit;
        QMenu* menuPlot;
        QList<QPointer<QFRDRTablePlotWidget> > plotWidgets;

        bool updating;
    private:
};

#endif // QFRDRPLOTEDITOR_H
