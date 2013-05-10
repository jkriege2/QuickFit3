#ifndef QFPLOTTERCOPYTOTABLEDIALOG_H
#define QFPLOTTERCOPYTOTABLEDIALOG_H

#include <QDialog>
#include "qfrdrtableinterface.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrawdatarecord.h"
#include "libwid_imexport.h"

namespace Ui {
    class QFPlotterCopyToTableDialog;
}

class QFWIDLIB_EXPORT QFPlotterCopyToTableDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFPlotterCopyToTableDialog(QWidget *parent = 0);
        ~QFPlotterCopyToTableDialog();

        QFRDRTableInterface* getTable() const;
        QFRawDataRecord* getRDR() const;
        QFRDRColumnGraphsInterface* getGraph() const;
        int getGraphID() const;
        bool getNewGraph(QString& name) const;
        bool getNewTable(QString& name) const;
        bool getAddGraphs() const;
        bool getShowEditor() const;
        void setDescription(const QString& text);

    protected slots:
        void on_cmbRDR_refilled(bool full);
        void on_cmbGraph_refilled(bool full);
        void on_chkNewTable_toggled(bool checked);
        void on_chCreateNewGraph_toggled(bool checked);
    private:
        Ui::QFPlotterCopyToTableDialog *ui;
};

#endif // QFPLOTTERCOPYTOTABLEDIALOG_H
