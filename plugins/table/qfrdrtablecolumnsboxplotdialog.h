#ifndef QFRDRTABLECOLUMNSBOXPLOTDIALOG_H
#define QFRDRTABLECOLUMNSBOXPLOTDIALOG_H

#include "qfdialog.h"
#include "qfproperties.h"
#include <QMap>
#include <QList>
#include <QStringList>
#include "qftablepluginmodel.h"
#include "qcheckablestringlistmodel.h"
#include "qfrdrtable.h"

namespace Ui {
    class QFRDRTableColumnsBoxplotDialog;
}

class QFRDRTableColumnsBoxplotDialog : public QFDialog
{
        Q_OBJECT

    public:


        explicit QFRDRTableColumnsBoxplotDialog(QWidget *parent = 0);
        ~QFRDRTableColumnsBoxplotDialog();

        void setTable(QFRDRTable *table);
        void setSelectedColumns(const QList<int>& cols);
        int getResultStartColumn() const;
        int addToGraph() const;
        bool createGraph() const;
        bool addNewGraph() const;
        QList<int> getDataColumns() const;
        bool getPlotBoxplot() const;
        bool getPlotSymbols() const;
        bool getPlot2Symbols() const;
        QString getDataColumnsExpression() const;
        QStringList getExpressions(QStringList &names, QString &addToPre) const;

    private:
        Ui::QFRDRTableColumnsBoxplotDialog *ui;
        QFTablePluginModel* table;
        QFRDRTable *plugin;
        QCheckableStringListModel cols;
};

#endif // QFRDRTABLECOLUMNSBOXPLOTDIALOG_H
