#ifndef QFRDRTABLEEDITOR_H
#define QFRDRTABLEEDITOR_H

#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "qfrawdataeditor.h"
/*! \brief editor class for tables
    \ingroup qf3rdrdp_table
*/
class QFRDRTableEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRTableEditor(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRTableEditor();
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

        void slAppendRow();
        void slAppendColumn();
        void slInsertRow();
        void slInsertColumn();
        void slClear();
        void slDeleteRow();
        void slDeleteColumn();
        void slSetDatatype();
        void slSaveTable();
        void slLoadTable();
        void slSetColumnTitle();

    protected:
        /** \brief table view for the contents */
        QTableView* tvMain;

        QString currentTableDir;


        QPushButton* btnSaveTable;
        QPushButton* btnLoadTable;
        QPushButton* btnAppendRow;
        QPushButton* btnAppendColumn;
        QPushButton* btnInsertRow;
        QPushButton* btnInsertColumn;
        QPushButton* btnClear;
        QPushButton* btnDeleteRow;
        QPushButton* btnDeleteColumn;
        QPushButton* btnSetDatatype;
        QPushButton* btnSetColumnTitle;
    private:
};

#endif // QFRDRTABLEEDITOR_H
