#ifndef QFRAWDATAPROPERTYEDITOR_H
#define QFRAWDATAPROPERTYEDITOR_H

#include <QtGui>
#include <QPointer>
#include <QMap>
#include <QSortFilterProxyModel>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"
#include "qenhancedtableview.h"
#include "lib_imexport.h"

/*! \brief editor widget (window) for raw data items
    \ingroup qf3lib_project


 As there were problems when changing the displayed editor for different types of raw data items. One editor window
 is only useable for one type of  raw data!
*/
class QFLIB_EXPORT QFRawDataPropertyEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRawDataPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFRawDataRecord* current=NULL, int id=0, QWidget* parent=NULL, Qt::WindowFlags f = Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        /** Default destructor */
        virtual ~QFRawDataPropertyEditor();
        /** \brief set the current record */
        void setCurrent(QFRawDataRecord* c);
        /** \brief set a Programoptions object to use for storing application settings */
        void setSettings(ProgramOptions* settings);
        /** \brief write the settings */
        void writeSettings();
    protected:
        /** \brief points to the record currently displayed */
        QPointer<QFRawDataRecord> current;
        /** \brief read the settings */
        void readSettings();
        void closeEvent( QCloseEvent * event );
        void resizeEvent ( QResizeEvent * event );
    private slots:
        /** \brief called when the name editor changes its contents */
        void nameChanged(const QString& text);
        /** \brief called when the folder editor changes its contents */
        void folderChanged(const QString& text);
        /** \brief called when the description editor changes its contents */
        void descriptionChanged();
        /** \brief this will be connected to the project to indicate when  the currently
         *         displayed record should be deleted */
        void recordAboutToBeDeleted(QFRawDataRecord* r);
        /** \brief emitted when the "next" button is pressed */
        void nextPressed();
        /** \brief emitted when the "previous" button is pressed */
        void previousPressed();
        /** \brief emitted when the raw data record data changes (i.e. ID, name, description, ...) */
        void propsChanged();
        /** \brief insert a new property */
        void newPropClicked();
        /** \brief delete a property */
        void deletePropClicked();
        /** \brief resize the cells in the properties table */
        void resizePropertiesTable();
        /** \brief remove the current record */
        void deleteRecord();
        void tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
        /** \brief delete selected results */
        void deleteSelectedResults();

        /** \brief save results to a file */
        void saveResults();

        /** \brief display help dialog */
        void displayHelp();

        void copyValErrResults();
        void copyValErrResultsNoHead();
    private:
        /** \brief create all widgets needed to display data */
        void createWidgets();
        /** \brief QLineEdit for the name */
        QLineEdit* edtName;
        /** \brief QLineEdit for the folder */
        QLineEdit* edtFolder;
        /** \brief QPlainTextEdit for the description */
        QPlainTextEdit* pteDescription;
        /** \brief list of files associated with this rawdatarecord */
        QListWidget* lstFiles;
        /** \brief label next to buttons on top */
        QLabel* labTop;
        /** \brief label next to buttons on top */
        QLabel* labTopIcon;
        /** \brief label for the ID */
        QLabel* labID;
        /** \brief label for the type */
        QLabel* labType;
        /** \brief label for the type icon */
        QLabel* labTypeIcon;
        /** \brief Tab to change between property editor and data view */
        QTabWidget* tabMain;
        /** \brief tabel view for the properties */
        QEnhancedTableView* tvProperties;
        /** \brief button to switch to next record */
        QPushButton* btnNext;
        /** \brief button to insert a new property */
        QPushButton* btnNewProperty;
        /** \brief button to delete a property */
        QPushButton* btnDeleteProperty;
        /** \brief button to switch to previous record */
        QPushButton* btnPrevious;
        /** \brief button to delete the current record */
        QPushButton* btnDeleteReord;
        /** \brief points to a settings object that is used to store application settings */
        ProgramOptions* settings;
        /** \brief tabel display the evaluation results associated with this file  */
        QEnhancedTableView* tvResults;
        /** \brief label for averaging over selected values in tvResults */
        QLabel* labAveragedresults;
        /** \brief toolbar to access functions of tvResults */
        QToolBar* tbResults;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyResults;
        QAction* actCopyResultsNoHead;
        /** \brief action used to save selection in tvResults to file */
        QAction* actSaveResults;
        /** \brief action used to delete selection in tvResults */
        QAction* actDeleteResults;
        /** \brief widget that is used to display the tvResults table + opt. some more compoinents */
        QWidget* widResults;
        /** \brief button to display help */
        QPushButton* btnHelp;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyValErrResults;
        QAction* actCopyValErrResultsNoHead;

        QLineEdit* edtFilterEvaluation;
        QLineEdit* edtFilterResults;
        QCheckBox* chkFilterEvaluationRegExp;
        QCheckBox* chkFilterResultsRegExp;

        QSortFilterProxyModel* paramFilterProxy;

        QList<QPointer<QFRawDataEditor> > editorList;
        int id;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;

        QString currentSaveDir;
};

#endif // QFRAWDATAPROPERTYEDITOR_H
