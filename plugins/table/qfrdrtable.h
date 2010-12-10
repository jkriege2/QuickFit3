#ifndef QFRDRTABLE_H
#define QFRDRTABLE_H

#include "qftablemodel.h"
#include "qfrdrtableeditor.h"
#include "qfrawdatarecord.h"
#include "qfrawdatarecordfactory.h"



/*! \brief this class is used to manage a table of values (strings/numbers)
    \ingroup qf3rdrdp_table

    The data is stored in a QFTableModel object which is also externally accessible for data access.
 */

class QFRDRTable : public QFRawDataRecord {
        Q_OBJECT
    public:
        /** \brief class constructor, initialises as specified, the ID is obtained from the parent project */
        QFRDRTable(QFProject* parent);

        /** Default destructor */
        virtual ~QFRDRTable();

        /** \brief returns the used datamodel */
        QFTableModel* model() { return datamodel; };
        /** \brief returns the table contents at the given position */
        QVariant getModelData(quint16 row, quint16 column) {
            return datamodel->data(datamodel->index(row, column), Qt::DisplayRole);
        };


        /** \brief return type (short type string) */
        virtual QString getType() const { return "table"; };
        /** \brief return type (longer type string, user readable) */
        virtual QString getTypeName() const { return tr("Data Table"); };
        /** \brief return a small icon (16x16) */
        virtual QIcon getSmallIcon() const { return QIcon(":/projecttree_tablefile.png"); };
        /** \brief return type description */
        virtual QString getTypeDescription() const { return tr("data table"); };
        /** \brief return a large icon (32x32) */
        virtual QIcon getLargeIcon() const { return QIcon(":/projecttree_tablefile.png"); };
        /** \brief returns the number of additional editor panes for this record */
        virtual int getEditorCount() { return 1; };
        /** \brief returns the name for the i-th editor pane */
        virtual QString getEditorName(int i) { return tr("Table Editor"); };
        /** \brief create an object for the i-th editor pane */
        virtual QFRawDataEditor* createEditor(int i=0, QWidget* parent=NULL) {
            return new QFRDRTableEditor(parent);
        };
        /** \brief export the raw data into the specified format */
        virtual void exportData(const QString& format, const QString& filename)const ;
        /** \brief returns a list of filetypes which correspond to the filetypes returned by getExportFiletypes() */
        QStringList getExportFiletypes() {
            QStringList sl;
            sl << "CSV" << "SSV" << "GERMANEXCEL" << "SYLK";
            return sl;
        };
        /** \brief returns the title of the Export file dialog */
        QString getExportDialogTitle() { return tr("Export Data Table ..."); };
        /** \brief returns the filetype of the Export file dialog */
        QString getExportDialogFiletypes() { return tr("Comma Separated Value Files (*.csv, *.txt);;Semicolon Separated Value Files (*.csv, *.txt);;Semicolon Separated Value Files [german Excel] (*.csv, *.txt);;SYLK File (*.sylk, *.slk)"); };
    protected slots:
        void tdataChanged( const QModelIndex & tl, const QModelIndex & br ) {
            emit rawDataChanged();
        }
        void trawDataChanged() {
            emit rawDataChanged();
        }

    protected:
        virtual void intWriteData(QXmlStreamWriter& w);
        /** \brief read in external data files <u>and</u> data stored in the project file <b>IMPLEMENT IN CHILD CLASSES!</b>
         *
         * If \a e is \c NULL then this method should only read the datafiles already saved in the files property.
         */
        virtual void intReadData(QDomElement* e=NULL);

        /** \brief stores a table of QVariants */
        QFTableModel* datamodel;
    private:
};

#endif // QFRDRTABLE_H
