#ifndef QFEVALUATIONPROPERTYEDITOR_H
#define QFEVALUATIONPROPERTYEDITOR_H

#include <QtGui>
#include <QSortFilterProxyModel>
#include "qfproject.h"
#include "qfevaluationitem.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qftools.h"
#include "qfhtmlhelpwindow.h"
#include "qfevaluationresultsmodel.h"
#include "qenhancedtableview.h"


/*! \brief This QSortFilterProxyModel implements a filter proxy model which  to filter out records
          that are not usable together with a given QFMultiListEvaluation.
    \ingroup qf3lib_project

    This class depends on the first Qt::UserRole to supply the ID of the record to be filtered. It calls
    QFEvaluation::isApplicable() to find out whether a raw data record is to be displayed.

    If  the given evaluation also supports item selection this implments the feature using the Qt::CheckStateRole role.


 */
class QFEvaluationRawDataModelProxy : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        QFEvaluationRawDataModelProxy(QObject *parent = 0): QSortFilterProxyModel(parent) {
            setDynamicSortFilter(true);
        };
        virtual ~QFEvaluationRawDataModelProxy() {};

        /** \brief set the evaluation against which the records shall be checked */
        void setEvaluation(QFEvaluationItem* eval);

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    protected slots:
        /** \brief called when the selection in the currently set evaluation changed */
        void selectionChanged(QList<QFRawDataRecord*> selectedRecords);
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        /** \brief QFMultiListEvaluation used to check whether a raw data record is applicable */
        QFEvaluationItem* eval;

};


/*! \brief this class implements the parent widget for an evaluation window

    \note This class does not implement a lot of functionality, so it is mainly used, when your QFEvaluationEditor(s)
          do all the work (including which data to work on ...), i.e. if you derived you evaluation item directly from
          QFEvaluationItem. If you derived from a specialised child of QFEvaluationItem, other property editor classes
          may better suit the features of your evaluation (e.g. QFMultiListEvaluationPropertyEditor). Which property
          editor object is created may be set by the class, as it is created in the QFEvaluationItem::createPropertyEditor()
          method of you evaluation item.

 */
class QFEvaluationPropertyEditor : public QWidget {
        Q_OBJECT
    protected:

    public:
        /** Default constructor */
        QFEvaluationPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFEvaluationItem* current=NULL, int id=0, QWidget* parent=NULL, Qt::WindowFlags f = Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        /** Default destructor */
        virtual ~QFEvaluationPropertyEditor();
        /** \brief set a ProgramOptions object to use for storing application settings */
        void setSettings(ProgramOptions* settings);
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief return widget ID */
        int getID() { return id; };
        /** \brief set the current record */
        void setCurrent(QFEvaluationItem* c);
        /** \brief set a pointer to an external (global) replacement list */
        void setHtmlReplacementList(QList<QPair<QString, QString> >* list) {
            helpWidget->setHtmlReplacementList(list);
        }
    private slots:
        /** \brief called when the name editor changes its contents */
        void nameChanged(const QString& text);
        /** \brief called when the description editor changes its contents */
        void descriptionChanged();
        /** \brief emitted when the raw data record data changes (i.e. ID, name, description, ...) */
        void propsChanged();
        /** \brief this will be connected to the project to indicate when  the currently
         *         displayed record should be deleted */
        void evaluationAboutToBeDeleted(QFEvaluationItem* r);
        /** \brief activated when the selection in lstRawData changes
         *  \see rdrModelReset()
         */
        void selectionChanged(const QModelIndex& index, const QModelIndex& oldindex);
        /*! \brief emitted when the model representing the list of raw data records is reset (rdrProxy).

            This is used to work a round the behaviour of Qt that a selection changes whenever
            a model is reset. In this case we simply select the currently selected record, if it
            is still available or another record, if not (it has been deleted).
        */
        void rdrModelReset();

        void tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);


        /** \brief save results to a file */
        void saveResults();
    protected:
        /** \brief points to the record currently displayed */
        QFEvaluationItem* current;
        /** \brief model showing all available raw data records */
        QFProjectRawDataModel* rdrModel;
        /** \brief proxy model to filter rdrModel */
        QFEvaluationRawDataModelProxy* rdrProxy;
        /** \brief datamodel used for display of evaluation results */
        QFEvaluationResultsModel* resultsModel;
        /** \brief read the settings from ProgramOptions set by setSettings() */
        virtual void readSettings();
        void closeEvent( QCloseEvent* event);
        void resizeEvent(QResizeEvent* event);
    private:
        /** \brief create all widgets needed to display data */
        void createWidgets();
        /** \brief QLineEdit for the Name */
        QLineEdit* edtName;
        /** \brief QPlainTextEdit for the description */
        QPlainTextEdit* pteDescription;
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
        /** \brief QListView to display raw data records to which an evaluation is applicable */
        QListView* lstRawData;
        /** \brief splitter between files list and evaluation tab */
        QSplitter* splitMain;
        /** \brief Layout widget for the evaluation editor widget */
        QHBoxLayout* layWidgets;

        /** \brief tabel display the evaluation results associated with this file  */
        QEnhancedTableView* tvResults;
        /** \brief label for averaging over selected values in tvResults */
        QLabel* labAveragedresults;
        /** \brief toolbar to access functions of tvResults */
        QToolBar* tbResults;
        /** \brief action used to copy selection in tvResults to clipbord */
        QAction* actCopyResults;
        /** \brief action used to save selection in tvResults to file */
        QAction* actSaveResults;
        QAction* actRefreshResults;
        /** \brief widget that is used to display the tvResults table + opt. some more compoinents */
        QWidget* widResults;


        /** \brief points to a settings object that is used to store application settings */
        ProgramOptions* settings;
        /** \brief of all currently instaciated editors */
        QPointer<QFEvaluationEditor> editor;
        /** \brief ID used to distinguish between different dialog (e.g. for settings) */
        int id;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;
        /** \brief help widget for evaluation */
        QFHTMLHelpWindow* helpWidget;

        QString currentSaveDir;

};

#endif // QFEVALUATIONPROPERTYEDITOR_H
