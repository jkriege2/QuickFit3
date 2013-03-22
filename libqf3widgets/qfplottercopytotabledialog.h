#ifndef QFPLOTTERCOPYTOTABLEDIALOG_H
#define QFPLOTTERCOPYTOTABLEDIALOG_H

#include <QDialog>
#include "qfrdrtableinterface.h"
#include "qfrdrcolumngraphsinterface.h"

namespace Ui {
    class QFPlotterCopyToTableDialog;
}

class QFPlotterCopyToTableDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFPlotterCopyToTableDialog(QWidget *parent = 0);
        ~QFPlotterCopyToTableDialog();

        QFRDRTableInterface* getTable() const;
        QFRDRColumnGraphsInterface* getGraph() const;
        int getGraphID() const;
        bool getNewGraph(QString& name) const;
        bool getNewTable(QString& name) const;
        bool getAddGraphs() const;

    protected slots:
        void on_cmbRDR_refilled(bool full);
        void on_cmbGraph_refilled(bool full);
    private:
        Ui::QFPlotterCopyToTableDialog *ui;
};

#endif // QFPLOTTERCOPYTOTABLEDIALOG_H
