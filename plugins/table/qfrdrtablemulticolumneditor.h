#ifndef QFRDRTABLEMULTICOLUMNEDITOR_H
#define QFRDRTABLEMULTICOLUMNEDITOR_H

#include <QDialog>
#include "qftablepluginmodel.h"
#include "qfhtmldelegate.h"
#include "qftablemodel.h"

namespace Ui {
    class QFRDRTableMultiColumnEditor;
}

class QFRDRTableMultiColumnEditor : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRTableMultiColumnEditor(QFTableModel* model, QWidget *parent = 0);
        ~QFRDRTableMultiColumnEditor();

    protected:
        virtual void closeEvent(QCloseEvent * e);
    private:
        Ui::QFRDRTableMultiColumnEditor *ui;

        QFTableModelEditColumnHeaderDataModel hmodel;
};

#endif // QFRDRTABLEMULTICOLUMNEDITOR_H
