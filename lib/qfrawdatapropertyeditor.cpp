#include "qfrawdatapropertyeditor.h"
#include "dlgnewproperty.h"
#include "qfrawdatarecordfactory.h"
#include "../version.h"
#include "qfhtmldelegate.h"
#include "qfstyleditemdelegate.h"
#include <QModelIndex>
#include <QModelIndexList>


// TODO: add some more options to the fit results display: store column under different name


QFRawDataPropertyEditor::QFRawDataPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFRawDataRecord* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    timerResizeProps=new QTimer(this);
    timerResizeProps->setSingleShot(true);
    timerResizeProps->setInterval(500);
    connect(timerResizeProps, SIGNAL(timeout()), this, SLOT(resizePropertiesTable()));
    currentSaveDir="";
    //std::cout<<"creating QFRawDataPropertyEditor ...\n";
    this->current=NULL;
    this->id=id;
    this->services=services;
    setSettings(set);
    //std::cout<<"creating QFRawDataPropertyEditor ... creating widgets ...\n";
    createWidgets();
    //std::cout<<"creating QFRawDataPropertyEditor ... setting current ...\n";
    setCurrent(current);
    //std::cout<<"creating QFRawDataPropertyEditor ... DONE!\n";
    readSettings();
}

QFRawDataPropertyEditor::~QFRawDataPropertyEditor()
{
    writeSettings();
}

#define ADDTOGRID(layout, counter, label, widget) {\
    QWidget* lparent=NULL; \
    QWidget* bwidget=NULL; \
      lparent=widget; \
      bwidget=widget; \
      layout->addWidget(widget, counter, 1); \
    QLabel* l=new QLabel(label, lparent); \
    l->setBuddy(bwidget); \
    layout->addWidget(l, counter, 0); \
    counter++; \
}

#define ADDTOGRIDLAY(layout, counter, label, widget) {\
    QWidget* lparent=NULL; \
    QWidget* bwidget=NULL; \
    bwidget=new QWidget(widget->parentWidget()); \
    bwidget->setLayout(widget); \
    lparent=bwidget; \
    layout->addWidget(bwidget, counter, 1); \
    QLabel* l=new QLabel(label, lparent); \
    l->setBuddy(bwidget); \
    layout->addWidget(l, counter, 0); \
    counter++; \
}

void QFRawDataPropertyEditor::createWidgets() {
    QLabel* l;
    QVBoxLayout* ml=new QVBoxLayout(this);
    setLayout(ml);

    menuBar=new QMenuBar(this);
    menuBar->setVisible(true);
    ml->addWidget(menuBar);
    menuRDR=menuBar->addMenu("Recor&d");
    menuResults=menuBar->addMenu("&Results");
    menuHelp=menuBar->addMenu("&Help");

    ml->setContentsMargins(2,2,2,2);
    QHBoxLayout* vl=new QHBoxLayout(this);
    ml->addLayout(vl);
    actPrevious=new QAction(QIcon(":/lib/prop_previous.png"), tr("&previous"), this);
    actPrevious->setToolTip(tr("move to previous record"));

    btnPrevious=new QToolButton(this);
    btnPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnPrevious->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnPrevious->setDefaultAction(actPrevious);
    vl->addWidget(btnPrevious);
    connect(actPrevious, SIGNAL(triggered()), this, SLOT(previousPressed()));

    actNext=new QAction(QIcon(":/lib/prop_next.png"), tr("&next"), this);
    actNext->setToolTip(tr("move to next record"));

    btnNext=new QToolButton(this);
    btnNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnNext->setDefaultAction(actNext);

    connect(actNext, SIGNAL(triggered()), this, SLOT(nextPressed()));
    vl->addWidget(btnNext);
    labTopIcon=new QLabel(this);
    vl->addWidget(labTopIcon);
    labTop=new QLabel(this);
    vl->addWidget(labTop);
    vl->addStretch();

    actDelete=new QAction(QIcon(":/lib/item_delete.png"), tr("&Remove Record"), this);
    actDelete->setToolTip(tr("removes the currently displayed record from the project"));
    btnDeleteReord=new QToolButton(this);
    btnDeleteReord->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnDeleteReord->setDefaultAction(actDelete);
    btnDeleteReord->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vl->addWidget(btnDeleteReord);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteRecord()));

    actClose=new QAction(QIcon(":/lib/exit.png"), tr("&Close Window"), this);
    connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
    menuRDR->addAction(actPrevious);
    menuRDR->addAction(actNext);
    menuRDR->addSeparator();
    menuRDR->addAction(actDelete);
    menuRDR->addSeparator();
    menuRDR->addAction(actClose);

    actHelp=new QAction(QIcon(":/lib/help.png"), tr("&Help"), this);
    actHelp->setToolTip(tr("display online-help"));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(displayHelp()));

    actHelpPlugin=new QAction(QIcon(":/lib/help.png"), tr("&Plugin Help"), this);
    actHelpPlugin->setToolTip(tr("display online-help for the specific plugin"));
    connect(actHelpPlugin, SIGNAL(triggered()), this, SLOT(displayHelpPlugin()));

    actHelpPluginTutorial=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("&Plugin Tutorial"), this);
    actHelpPluginTutorial->setToolTip(tr("display the tutorial for the specific plugin"));
    connect(actHelpPluginTutorial, SIGNAL(triggered()), this, SLOT(displayHelpPluginTutorial()));

    actHelpPluginCopyright=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("&Plugin Copyright"), this);
    actHelpPluginCopyright->setToolTip(tr("display copyright note for the specific plugin"));
    connect(actHelpPluginCopyright, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));

    actHelpRDR=new QAction(QIcon(":/lib/help_rdr.png"), tr("&Raw data record help"), this);
    actHelpRDR->setToolTip(tr("display online-help common to all plugins, i.e. for the basic record editor dialog"));
    connect(actHelpRDR, SIGNAL(triggered()), this, SLOT(displayHelpRDR()));

    menuHelp->addAction(actHelpRDR);
    menuHelp->addAction(actHelpPlugin);
    menuHelp->addAction(actHelpPluginTutorial);
    menuHelp->addAction(actHelpPluginCopyright);

    btnHelp=new QToolButton(this);
    btnHelp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnHelp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnHelp->setDefaultAction(actHelp);

    vl->addWidget(btnHelp);

    tabMain=new QTabWidget(this);
    ml->addWidget(tabMain);
    connect(tabMain, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    QWidget* w=new QWidget(tabMain);
    QGridLayout* fl=new QGridLayout(w);
    //int flcounter=0;
    //fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    w->setLayout(fl);
    tabMain->addTab(w, tr("&Properties"));
    labID=new QLabel(w);
    labID->setSizePolicy(labID->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    //fl->addRow(tr("ID:"), labID);
    fl->addWidget((l=new QLabel(tr("ID:"), w)), 0, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addWidget(labID, 0, 1);
    fl->setRowStretch(0,1);
    labType=new QLabel(w);
    labTypeIcon=new QLabel(w);
    labType->setSizePolicy(labType->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    labTypeIcon->setSizePolicy(labTypeIcon->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    QHBoxLayout* ptl=new QHBoxLayout(this);
    ptl->setContentsMargins(0,0,0,0);
    ptl->addWidget(labTypeIcon);
    ptl->addWidget(labType);
    ptl->addStretch();
    //fl->addRow(tr("Type:"), ptl);
    fl->addWidget((l=new QLabel(tr("Type:"), w)), 1, 0);
    fl->setRowStretch(1,1);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addLayout(ptl, 1, 1);
    edtName=new QLineEdit(w);
    //fl->addRow(tr("&Name:"), edtName);
    fl->addWidget((l=new QLabel(tr("&Name:"), w)), 2, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(edtName);
    fl->addWidget(edtName, 2, 1);
    fl->setRowStretch(2,1);

    edtFolder=new QLineEdit(w);
    fl->addWidget((l=new QLabel(tr("&Folder:"), w)), 3, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(edtFolder);
    fl->addWidget(edtFolder, 3, 1);
    fl->setRowStretch(3,1);

    pteDescription=new QPlainTextEdit(w);
    pteDescription->setSizePolicy( pteDescription->sizePolicy().horizontalPolicy(),QSizePolicy::Preferred);

    //fl->addRow(tr("&Description:"), pteDescription);
    fl->addWidget((l=new QLabel(tr("&Description:"), w)), 4, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(pteDescription);
    fl->addWidget(pteDescription, 4, 1);
    fl->setRowStretch(4,1);
    //fl->setRowStretch(flcounter-1, 2);
    lstFiles=new QListWidget(w);
    lstFiles->setSizePolicy(lstFiles->sizePolicy().verticalPolicy(), QSizePolicy::Preferred);
    //fl->addRow(tr("&Files:"), lstFiles);
    fl->addWidget((l=new QLabel(tr("&Files:"), w)), 5, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(lstFiles);
    fl->addWidget(lstFiles, 5, 1);
    fl->setRowStretch(5,1);
    //fl->setRowStretch(flcounter-1, 1);
    tvProperties=new QEnhancedTableView(w);
    QFontMetrics fm(font());
    tvProperties->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.1));
    tvProperties->horizontalHeader()->setStretchLastSection(true);
    //tvProperties->setSizePolicy(tvProperties->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
    tvProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tvProperties->setItemDelegate(propDelegate=new QFPropertyItemDelegate(tvProperties));
    /*paramFilterProxy=new QSortFilterProxyModel(this);
    paramFilterProxy->setDynamicSortFilter(false);
    paramFilterProxy->setFilterKeyColumn(0);
    paramFilterProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    paramFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tvProperties->setModel(paramFilterProxy);*/
    tvProperties->setModel(NULL);


    QWidget* widProperties=new QWidget(this);
    QHBoxLayout* pl1=new QHBoxLayout(this);
    widProperties->setLayout(pl1);
    widProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pl1->setContentsMargins(0,0,0,0);
    pl1->addWidget(tvProperties);
    QVBoxLayout* pl2=new QVBoxLayout(this);
    pl1->addLayout(pl2);
    btnNewProperty=createButtonAndActionShowText(actNewProperty, QIcon(":/lib/prop_add.png"), tr("In&sert Property"), this);
    connect(actNewProperty, SIGNAL(triggered()), this, SLOT(newPropClicked()));
    pl2->addWidget(btnNewProperty);
    btnDeleteProperty=createButtonAndActionShowText(actDeleteProperty, QIcon(":/lib/prop_delete.png"), tr("&Delete Property"), this);
    connect(actDeleteProperty, SIGNAL(triggered()), this, SLOT(deletePropClicked()));
    pl2->addWidget(btnDeleteProperty);

    btnCopyProperty=createButtonAndActionShowText(actCopyProperty, QIcon(":/lib/edit_copy.png"), tr("&Copy Property"), this);
    connect(actCopyProperty, SIGNAL(triggered()), this, SLOT(copyProperties()));
    pl2->addWidget(btnCopyProperty);

    btnCutProperty=createButtonAndActionShowText(actCutProperty, QIcon(":/lib/edit_cut.png"), tr("C&ut Property"), this);
    connect(actCutProperty, SIGNAL(triggered()), this, SLOT(cutProperties()));
    pl2->addWidget(btnCutProperty);

    btnPasteProperty=createButtonAndActionShowText(actPasteProperty, QIcon(":/lib/edit_paste.png"), tr("&Paste Property"), this);
    connect(actPasteProperty, SIGNAL(triggered()), this, SLOT(pasteProperties()));
    pl2->addWidget(btnPasteProperty);

    pl2->addStretch();
    //fl->addRow(tr("Properties:"), widProperties);
    fl->addWidget((l=new QLabel(tr("Properties:"), w)), 6, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addWidget(widProperties, 6, 1);
    fl->setRowStretch(6,3);
    //fl->setRowStretch(flcounter-1, 5);

    actMakePropertyEditable=new QAction(tr("Make property editable"), this);
    connect(actMakePropertyEditable, SIGNAL(triggered()), this, SLOT(makePropEditable()));

    tvProperties->addAction(actNewProperty);
    tvProperties->addAction(actDeleteProperty);
    tvProperties->addAction(getSeparatorAction(this));
    tvProperties->addAction(actCopyProperty);
    tvProperties->addAction(actCutProperty);
    tvProperties->addAction(actPasteProperty);
    tvProperties->addAction(getSeparatorAction(this));
    tvProperties->addAction(actMakePropertyEditable);
    tvProperties->setContextMenuPolicy(Qt::ActionsContextMenu);


    widResults=new QWidget(this);
    QVBoxLayout* rwvlayout=new QVBoxLayout(this);
    widResults->setLayout(rwvlayout);
    tbResults=new QToolBar("toolbar_rdr_results", this);
    rwvlayout->addWidget(tbResults);
    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), this);
    tbResults->addAction(actCopyResults);
    actCopyResultsNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) without header row/column"), this);
    tbResults->addAction(actCopyResultsNoHead);

    actCopyValErrResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs"), this);
    tbResults->addAction(actCopyValErrResults);
    actCopyValErrResultsNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs, but without a header (so data only)"), this);
    tbResults->addAction(actCopyValErrResultsNoHead);
    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), this);
    actSaveResultsAveraged=new QAction(tr("Save all results to file with averaged vector/matrix results"), this);
    tbResults->addAction(actSaveResults);

    tbResults->addSeparator();
    actDeleteResults=new QAction(QIcon(":/lib/delete16.png"), tr("Delete selected records"), this);
    actDeleteResults->setShortcut(QKeySequence::Delete);
    tbResults->addAction(actDeleteResults);

    tbResults->addSeparator();
    tbResults->addWidget(new QLabel("  evaluation filter: "));
    edtFilterEvaluation=new QFEnhancedLineEdit(this);
    edtFilterEvaluation->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterEvaluation, edtFilterEvaluation));
    edtFilterEvaluation->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "items where the evaluation contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                        "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterEvaluation=new QFCompleterFromFile(this);
    edtFilterEvaluation->setCompleter(compFilterEvaluation);

    tbResults->addWidget(edtFilterEvaluation);
    tbResults->addWidget(new QLabel(tr("  and not contains: "), this));
    edtFilterEvaluationNot=new QFEnhancedLineEdit(this);
    edtFilterEvaluationNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterEvaluationNot, edtFilterEvaluationNot));
    edtFilterEvaluationNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "items where the evaluation does NOT containa match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                        "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                       "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterEvaluationNot=new QFCompleterFromFile(this);
    edtFilterEvaluationNot->setCompleter(compFilterEvaluationNot);
    tbResults->addWidget(edtFilterEvaluationNot);
    chkFilterEvaluationRegExp=new QCheckBox(tr("RegExp"), this);
    chkFilterEvaluationRegExp->setChecked(false);
    tbResults->addWidget(chkFilterEvaluationRegExp);
    connect(edtFilterEvaluationNot, SIGNAL(textChanged(QString)), this, SLOT(filterEvaluationTextChanged(QString)));
    connect(edtFilterEvaluation, SIGNAL(textChanged(QString)), this, SLOT(filterEvaluationTextChanged(QString)));


    tbResults->addSeparator();
    tbResults->addWidget(new QLabel("  result filter (contains): "));
    edtFilterResults=new QFEnhancedLineEdit(this);
    edtFilterResults->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResults, edtFilterResults));
    tbResults->addWidget(edtFilterResults);
    tbResults->addWidget(new QLabel(tr("  and not contains: "), this));
    edtFilterResultsNot=new QFEnhancedLineEdit(this);
    edtFilterResultsNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResultsNot, edtFilterResultsNot));
    tbResults->addWidget(edtFilterResultsNot);
    chkFilterResultsRegExp=new QCheckBox(tr("RegExp"), this);
    chkFilterResultsRegExp->setChecked(false);
    tbResults->addWidget(chkFilterResultsRegExp);
    edtFilterResults->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                    "Simply enter a filter string and the table will only display those"
                                    "rows where the result name contains a match to the filter string."
                                    "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                    "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                    "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResults=new QFCompleterFromFile(this);
    edtFilterResults->setCompleter(compFilterResults);
    edtFilterResultsNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                    "Simply enter a filter string and the table will only display those"
                                    "rows where the result name contains NO match to the filter string."
                                    "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                    "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                    "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResultsNot=new QFCompleterFromFile(this);
    edtFilterResultsNot->setCompleter(compFilterResultsNot);
    connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));
    connect(edtFilterResults, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));



    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->setItemDelegate(new QFHTMLDelegate(tvResults));
    tvResults->setContextMenuPolicy(Qt::ActionsContextMenu);
    tvResults->addAction(actCopyResults);
    tvResults->addAction(actCopyResultsNoHead);
    tvResults->addAction(actCopyValErrResults);
    tvResults->addAction(actCopyValErrResultsNoHead);
    tvResults->addAction(actSaveResults);
    tvResults->addAction(actSaveResultsAveraged);
    tvResults->addAction(actDeleteResults);
    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);


    connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    connect(actCopyResultsNoHead, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcelNoHead()));
    connect(actCopyValErrResults, SIGNAL(triggered()), this, SLOT(copyValErrResults()));
    connect(actCopyValErrResultsNoHead, SIGNAL(triggered()), this, SLOT(copyValErrResultsNoHead()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actSaveResultsAveraged, SIGNAL(triggered()), this, SLOT(saveResultsAveraged()));
    connect(actDeleteResults, SIGNAL(triggered()), this, SLOT(deleteSelectedResults()));

    menuResults->addAction(actSaveResults);
    menuResults->addAction(actSaveResultsAveraged);
    menuResults->addSeparator();
    menuResults->addAction(actDeleteResults);
    menuResults->addSeparator();
    menuResults->addAction(actCopyResults);
    menuResults->addAction(actCopyResultsNoHead);
    menuResults->addAction(actCopyValErrResults);
    menuResults->addAction(actCopyValErrResultsNoHead);


    tabMain->addTab(widResults, tr("Evaluation &Results"));
    currentTabChanged(0);


}

void QFRawDataPropertyEditor::closeEvent ( QCloseEvent * event ) {
    writeSettings();
    event->accept();
}

void QFRawDataPropertyEditor::resizePropertiesTable() {
    if (!current || !tvProperties) return;

    tvProperties->resizeColumnToContents(0);
    tvProperties->resizeColumnToContents(1);
    tvProperties->resizeRowsToContents();
    tvProperties->horizontalHeader()->setStretchLastSection(true);
}

void QFRawDataPropertyEditor::setCurrent(QFRawDataRecord* c) {
    QString oldType="";
    //std::cout<<"deleting old editors ... \n";
    int oldEditorCount;
    if (current) {
        oldType=current->getType();
        oldEditorCount=current->getEditorCount();
        disconnect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(edtFolder, SIGNAL(textChanged(const QString&)), this, SLOT(folderChanged(const QString&)));
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(current, SIGNAL(basicPropertiesChanged()), this, SLOT(basicPropsChanged()));
        disconnect(current->getPropertyModel(), SIGNAL(modelReset()), this, SLOT(resizePropertiesLater()));
        disconnect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        disconnect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeRowsToContents()));

        disconnect(edtFilterResults, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setResultFilter(QString)));
        disconnect(chkFilterResultsRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setResultFilterUsesRegExp(bool)));
        disconnect(edtFilterEvaluation, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setEvaluationFilter(QString)));
        disconnect(chkFilterEvaluationRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setEvaluationFilterUsesRegExp(bool)));
        disconnect(edtFilterResultsNot, SIGNAL(textChanged(QString)), tvResults->model(), SLOT(setResultFilterNot(QString)));
        disconnect(edtFilterEvaluationNot, SIGNAL(textChanged(QString)), tvResults->model(), SLOT(setEvaluationFilterNot(QString)));






        if (c) {
            if (c->getType()!=oldType) {
                for (int i=editorList.size()-1; i>=0; i--) {
                    QFRawDataEditor* w=editorList[i];
                    if (w) {
                        tabMain->removeTab(tabMain->indexOf(editorList[i]));
                        QApplication::processEvents(QEventLoop::AllEvents, 50);
                        w->saveSettings();
                        w->setSettings(NULL, id);
                        w->setCurrent(NULL, id);
                        w->setParent(NULL);
                        w->deleteLater();
                        //delete w;
                    }
                }
                editorList.clear();
                tabMain->removeTab(tabMain->indexOf(widResults));
                //tabMain->removeTab(tabMain->indexOf(helpWidget));
            }
        }
    }
    //std::cout<<"deleting old editors ... DONE!\n";
    current=c;
    //std::cout<<"creating new editors ... \n";
    if (current) {
        setWindowTitle(tr("Raw Data Editor/Viewer (%1)").arg(current->getTypeDescription()));
        if (current->getType()!=oldType) {
            editorList.clear();
            for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName(i);
                QFRawDataEditor* e=current->createEditor(services, this, i, NULL);
                if (e) {
                    e->setSettings(settings, id);
                    e->setCurrent(current, id);
                    //e->setSettings(settings, id);
                    tabMain->addTab(e, n);
                    editorList.append(e);
                }
            }
            tabMain->addTab(widResults, tr("Evaluation &Results"));
            //tabMain->addTab(helpWidget, QIcon(":/lib/help.png"), tr("Online-&Help"));
        } else {
            for (int i=0; i<editorList.size(); i++) {
                editorList[i]->setCurrent(current, id);
            }
        }
        edtName->setText(current->getName());
        edtName->setEnabled(true);
        edtFolder->setText(current->getFolder());
        edtFolder->setEnabled(true);
        propDelegate->setProject(current->getProject());

        pteDescription->setPlainText(current->getDescription());
        pteDescription->setEnabled(true);
        labID->setText(QString::number(current->getID()));
        labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstFiles->clear();
        QStringList f=current->getFiles();
        QStringList t=current->getFilesTypes();
        for (int i=0; i<f.size(); i++) {
            if (i<t.size()) {
                if (!t[i].isEmpty()) f[i]=f[i]+QString(" [%1]").arg(t[i]);
            }
        }
        lstFiles->addItems(f);
        lstFiles->setEnabled(true);
        //paramFilterProxy->setSourceModel();
        //paramFilterProxy->sort(0);
        tvProperties->setModel(current->getPropertyModel());
        tvProperties->setEnabled(true);
        tvResults->setModel(current->resultsGetModel());
        connect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));
        connect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        connect(edtFolder, SIGNAL(textChanged(const QString&)), this, SLOT(folderChanged(const QString&)));
        connect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        connect(current, SIGNAL(basicPropertiesChanged()), this, SLOT(basicPropsChanged()));
        connect(current->getPropertyModel(), SIGNAL(modelReset()), this, SLOT(resizePropertiesLater()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeRowsToContents()));
        connect(edtFilterResults, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setResultFilter(QString)));
        connect(chkFilterResultsRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setResultFilterUsesRegExp(bool)));
        connect(edtFilterEvaluation, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setEvaluationFilter(QString)));
        connect(chkFilterEvaluationRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setEvaluationFilterUsesRegExp(bool)));
        connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), tvResults->model(), SLOT(setResultFilterNot(QString)));
        connect(edtFilterEvaluationNot, SIGNAL(textChanged(QString)), tvResults->model(), SLOT(setEvaluationFilterNot(QString)));
        QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/");
        compFilterEvaluation->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterevals.txt");
        compFilterEvaluationNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterevalsnot.txt");
        compFilterResults->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterresults.txt");
        compFilterResultsNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterresults_not.txt");

        /*QPoint pos;
        pos.setX(current->getProject()->getProperty(QString("rawdatapropeditor%1/posx").arg(id), 20).toInt());
        pos.setY(current->getProject()->getProperty(QString("rawdatapropeditor%1/posy").arg(id), 20).toInt());
        QSize size;
        size.setWidth(current->getProject()->getProperty(QString("rawdatapropeditor%1/width").arg(id), 800).toInt());
        size.setHeight(current->getProject()->getProperty(QString("rawdatapropeditor%1/height").arg(id), 650).toInt());
        resize(size.boundedTo(QApplication::desktop()->screenGeometry(this).size()));
        if (pos.x()<-width() || pos.x()>QApplication::desktop()->screenGeometry(this).width()-30) pos.setX(0);
        if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(this).height()) pos.setY(0);
        move(pos);
        */

        /*helpWidget->clear();
        QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
        helpWidget->updateHelp(dll);*/

    } else {
        propDelegate->setProject(NULL);
        edtName->setText("");
        edtName->setEnabled(false);
        edtFolder->setText("");
        edtFolder->setEnabled(false);
        pteDescription->setPlainText("");
        pteDescription->setEnabled(false);
        labTopIcon->setText("");
        labTop->setText("");
        labID->setText("");
        labType->setText("");
        labTypeIcon->setText("");
        lstFiles->clear();
        lstFiles->setEnabled(false);
        //paramFilterProxy->setSourceModel(NULL);
        tvProperties->setModel(NULL);
        tvProperties->setEnabled(false);
        compFilterEvaluation->setFilename("");
        compFilterEvaluationNot->setFilename("");
        compFilterResults->setFilename("");
        compFilterResultsNot->setFilename("");
    }
    checkHelpAvailable();
    //std::cout<<"creating new editors ... DONE!\n";
}


void QFRawDataPropertyEditor::displayHelp() {
    if (tabMain->currentIndex()==0 || tabMain->currentIndex()==tabMain->count()-1) {
        displayHelpRDR();
    } else {
        displayHelpPlugin();
    }
}

void QFRawDataPropertyEditor::displayHelpRDR() {
    QString dll=services->getOptions()->getAssetsDirectory()+QString("/help/qf3_rdrscreen.html");
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor::displayHelpPlugin() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor::displayHelpPluginTutorial() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginTutorial(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor::displayHelpPluginCopyright() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginCopyrightFile(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor::copyValErrResults() {
    tvResults->copySelectionAsValueErrorToExcel(QFRDRResultsModel::AvgRole, QFRDRResultsModel::SDRole, true, Qt::Vertical);
}
void QFRawDataPropertyEditor::copyValErrResultsNoHead() {
    tvResults->copySelectionAsValueErrorToExcel(QFRDRResultsModel::AvgRole, QFRDRResultsModel::SDRole, false, Qt::Vertical);
}

void QFRawDataPropertyEditor::currentTabChanged(int tab) {
    int idx=tab-1;
    for (int i=0; i<menus.size(); i++) {
        if (menus[i].first==-1 || (idx>=0 && menus[i].first==idx)) {
            menus[i].second->menuAction()->setVisible(true);
        } else {
            menus[i].second->menuAction()->setVisible(false);
        }
    }
    menuResults->menuAction()->setVisible(tab==tabMain->count()-1);
}

void QFRawDataPropertyEditor::checkHelpAvailable() {
    if (!current) {
        actHelpPlugin->setVisible(false);
        actHelpPluginTutorial->setVisible(false);
        actHelpPluginCopyright->setVisible(false);
    } else {
        QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
        actHelpPlugin->setVisible(QFile::exists(dll));
        dll=current->getProject()->getRawDataRecordFactory()->getPluginTutorial(current->getType());
        actHelpPluginTutorial->setVisible(QFile::exists(dll));
        dll=current->getProject()->getRawDataRecordFactory()->getPluginCopyrightFile(current->getType());
        actHelpPluginCopyright->setVisible(QFile::exists(dll));
    }
}


void QFRawDataPropertyEditor::resizeEvent ( QResizeEvent * event ) {
    labAveragedresults->setMaximumWidth(event->size().width());
    QWidget::resizeEvent(event);
}

void QFRawDataPropertyEditor::filterEvaluationTextChanged(const QString &text)
{
    bool error=false;
    if (chkFilterEvaluationRegExp->isChecked()) {
        QRegExp rx(text);
        error=!rx.isValid();
    }
    QWidget* s=qobject_cast<QWidget*>(sender());
    if (s) {
       QPalette p=s->palette();
       if (error) {
           p.setColor(QPalette::Text, QColor("darkred"));
       } else {
           p.setColor(QPalette::Text, edtName->palette().color(QPalette::Text));
       }
       s->setPalette(p);
    }
}

void QFRawDataPropertyEditor::filterResultsTextChanged(const QString &text)
{
    bool error=false;
    if (chkFilterResultsRegExp->isChecked()) {
        QRegExp rx(text);
        error=!rx.isValid();
    }
    QWidget* s=qobject_cast<QWidget*>(sender());
    if (s) {
       QPalette p=s->palette();
       if (error) {
           p.setColor(QPalette::Text, QColor("darkred"));
       } else {
           p.setColor(QPalette::Text, edtName->palette().color(QPalette::Text));
       }
       s->setPalette(p);
    }
}

QMenuBar *QFRawDataPropertyEditor::getMenuBar() const
{
    return menuBar;
}

void QFRawDataPropertyEditor::setMenuBarVisible(bool visible)
{
    menuBar->setVisible(visible);
}

QMenu *QFRawDataPropertyEditor::addMenu(const QString& title, int editor)
{
    menuBar->removeAction(menuHelp->menuAction());
    QMenu* m=menuBar->addMenu(title);
    menuBar->addAction(menuHelp->menuAction());
    menus.append(qMakePair(editor, m));
    currentTabChanged(tabMain->currentIndex());
    return m;
}

void QFRawDataPropertyEditor::registerMenu(QMenu *menu, int editor) {
    menus.append(qMakePair(editor, menu));
    currentTabChanged(tabMain->currentIndex());
}

void QFRawDataPropertyEditor::nameChanged(const QString& text) {
    if (current) {
        current->setName(text);
    }
}

void QFRawDataPropertyEditor::folderChanged(const QString& text) {
    if (current) {
        current->setFolder(text);
    }
}

void QFRawDataPropertyEditor::descriptionChanged() {
    if (current) {
        current->setDescription(pteDescription->toPlainText());
    }
}

void QFRawDataPropertyEditor::recordAboutToBeDeleted(QFRawDataRecord* r) {
    if ((current==r) && current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        if (!n) n=current->getNextOfSameType();
        setCurrent(n);
    }
    if (r) {
        if (r->getProject()->getRawDataCount()<=1) {
            close();
        }
    }
}

void QFRawDataPropertyEditor::nextPressed() {
    if (current) {
        QFRawDataRecord* n=current->getNextOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor::previousPressed() {
    if (current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor::deleteRecord() {
    if (current) {
        int ret = QMessageBox::question(this, tr("QuickFit %1").arg(VERSION_FULL),
                                tr("Do you really want to delete the current record?\n   '%1'").arg(current->getName()),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QPointer<QFRawDataRecord> m=current;
            previousPressed();
            current->getProject()->deleteRawData(m->getID());
            if (current==m) {
                close();
            }
        }
    }
}
/*
void QFRawDataPropertyEditor::propsChanged(const QString& property, bool visible) {
    if (current && (visible || property.isEmpty())) {
        if (current->getName()!=edtName->text()) {
            edtName->setText(current->getName());
        }
        if (current->getFolder()!=edtFolder->text()) {
            edtFolder->setText(current->getFolder());
        }
        if (current->getDescription()!=pteDescription->toPlainText()) {
            pteDescription->setPlainText(current->getDescription());
        }
        labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
        labID->setText(QString::number(current->getID()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstFiles->clear();
        QStringList f=current->getFiles();
        QStringList t=current->getFilesTypes();
        for (int i=0; i<f.size(); i++) {
            if (i<t.size()) {
                if (!t[i].isEmpty()) f[i]=f[i]+QString(" [%1]").arg(t[i]);
            }
        }
        lstFiles->addItems(f);
        //paramFilterProxy->sort(0);
    }
}
*/
void QFRawDataPropertyEditor::basicPropsChanged() {
    if (current->getName()!=edtName->text()) {
        edtName->setText(current->getName());
    }
    if (current->getFolder()!=edtFolder->text()) {
        edtFolder->setText(current->getFolder());
    }
    if (current->getDescription()!=pteDescription->toPlainText()) {
        pteDescription->setPlainText(current->getDescription());
    }
    labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
    labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
    labID->setText(QString::number(current->getID()));
    labType->setText(current->getTypeDescription());
    labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
    lstFiles->clear();
    QStringList f=current->getFiles();
    QStringList t=current->getFilesTypes();
    for (int i=0; i<f.size(); i++) {
        if (i<t.size()) {
            if (!t[i].isEmpty()) f[i]=f[i]+QString(" [%1]").arg(t[i]);
        }
    }
    lstFiles->addItems(f);
}

void QFRawDataPropertyEditor::newPropClicked() {
    if (current) {
/*        QStringList items;
        items << tr("integer") << tr("double") << tr("string") << tr("boolean");
        bool ok;
        QString item = QInputDialog::getItem(this, tr("Insert New Property ..."), tr("Property Type:"), items, 1, false, &ok);
        int i=items.indexOf(item);
        if (ok) {
            QString name = QInputDialog::getText(this, tr("Insert New Property ..."),  tr("Property Name:"), QLineEdit::Normal, tr("PROPERTY_NAME"), &ok);
            if (ok) {
                QVariant v;
                if (i==0) v.setValue((int)0);
                if (i==1) v.setValue((double)0.0);
                if (i==2) v.setValue(QString(""));
                if (i==3) v.setValue((bool)false);
                current->setProperty(name, v);
            }
        }*/
        dlgNewProperty* d=new dlgNewProperty(this);
        d->addPropertyNames(current->getProject()->getAllPropertyNames());
        d->setPropertyName(ProgramOptions::getConfigValue(QString("rawdatapropeditor%1/newprop/name").arg(id), tr("new_property")).toString());
        d->setPropertyType(ProgramOptions::getConfigValue(QString("rawdatapropeditor%1/newprop/type").arg(id), 1).toInt());
        d->setPropertyValue(ProgramOptions::getConfigValue(QString("rawdatapropeditor%1/newprop/value").arg(id), "").toString());

        if (d->exec()==QDialog::Accepted) {
            QVariant v=d->propertyValue();
            switch (d->propertyTypeInt()) {
                case 1: v.convert(QVariant::Double); break;
                case 2: v.convert(QVariant::LongLong); break;
                case 3: v.convert(QVariant::Bool); break;
                case 4: v.convert(QVariant::DateTime); break;
                default: break;
            }
            bool ok=true;

            ProgramOptions::setConfigValue(QString("rawdatapropeditor%1/newprop/name").arg(id), d->propertyName());
            ProgramOptions::setConfigValue(QString("rawdatapropeditor%1/newprop/type").arg(id), d->propertyType());
            ProgramOptions::setConfigValue(QString("rawdatapropeditor%1/newprop/value").arg(id), d->propertyValue());

            if (current->propertyExists(d->propertyName())) {
                ok=current->isPropertyUserEditable(d->propertyName());
            }
            //qDebug()<<"QFRawDataPropertyEditor::newPropClicked()  pn="<<d->propertyName()<<"   val="<<v<<"   ok="<<ok;
            if (ok) current->setQFProperty(d->propertyName(), v, true, true);
            else QMessageBox::critical(this, tr("QuickFit"), tr("The property '%1' may not bge changed or created, as it is marked readonly!"));
        }

        delete d;
    }
}

void QFRawDataPropertyEditor::makePropEditable() {
    if (current) {
        QModelIndexList l=tvProperties->selectionModel()->selectedIndexes();
        QStringList props;
        for (int i=0; i<l.size(); i++) {
            /*int row=l[i].row();
            QString prop=current->getVisibleProperty(row);
            if ((!prop.isEmpty()) && current->propertyExists(prop) && current->isPropertyUserEditable(prop)) {
                props.append(prop);
            }*/
            props.append(current->getPropertyModel()->getPropertyForRow(l[i].row()));
        }
        if (props.size()>0) {
            if (QMessageBox::warning(this, tr("QuickFit"), tr("You are about to change the editablility of a property!\n"
                                                              "Possibly it was non-editable on purpose, so changing this\n"
                                                              "is at your own risk and may in some cases be overridden \n"
                                                              "when the project is loaded the next time!\n\n"
                                                              "Are you sure you want to make this property user-editable?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {

                for (int i=0; i<props.size(); i++) {
                    //std::cout<<"deleting "<<props[i].toStdString()<<std::endl;
                    //qDebug()<<"QFRawDataPropertyEditor::deletePropClicked '"<<props[i]<<"'";
                    //if (!props[i].isEmpty() && current->isPropertyUserEditable(props[i])) current->deleteProperty(props[i]);
                    current->setQFProperty(props[i], current->getProperty(props[i]), true, true);
                }
            }
        }

    }
}

void QFRawDataPropertyEditor::copyProperties() {
    if (current) {
        QModelIndexList l=tvProperties->selectionModel()->selectedIndexes();
        QStringList props;
        for (int i=0; i<l.size(); i++) {
            /*int row=l[i].row();
            QString prop=current->getVisibleProperty(row);
            if ((!prop.isEmpty()) && current->propertyExists(prop) && current->isPropertyUserEditable(prop)) {
                props.append(prop);
            }*/
            props.append(current->getPropertyModel()->getPropertyForRow(l[i].row()));
        }
        if (props.size()>0) {
            QMimeData* mime=new QMimeData();
            QString data;
            for (int i=0; i<props.size(); i++) {
                data.append(escapify(props[i])+"\n");
                QVariant v=current->getProperty(props[i]);
                data.append(getQVariantType(v)+"\n");
                data.append(escapify(getQVariantData(v))+"\n");
                data.append(boolToQString(current->isPropertyUserEditable(props[i])+"\n"));
            }
            mime->setData("quickfit3/properties", data.toUtf8());
            QApplication::clipboard()->setMimeData(mime);
        }

    }
}

void QFRawDataPropertyEditor::cutProperties()
{
    copyProperties();
    deletePropClicked();
}

void QFRawDataPropertyEditor::pasteProperties()
{
    const QMimeData* mime=QApplication::clipboard()->mimeData();
    if (mime) {
        if (mime->hasFormat("quickfit3/properties")) {
            QString data=QString::fromUtf8(mime->data("quickfit3/properties"));
            QStringList sl=data.split("\n");
            for (int i=0; i<sl.size(); i+=4) {
                if (i+3<sl.size()) {
                    QString name=deescapify(sl[i]);
                    QString type=sl[i+1];
                    QVariant v=getQVariantFromString(type, sl[i+2]);
                    bool editable=QStringToBool(sl[i+3]);
                    bool ok=true;
                    if (current->propertyExists(name)) {
                        QString otherData=current->getProperty(name).toString();
                        ok= QMessageBox::question(this, tr("QuickFit"), tr("The property '%1' already exists.\n"
                                                                           "   old data: %2\n"
                                                                           "   new data: %3\n\n"
                                                                           "Overwrite?").arg(name).arg(otherData).arg(v.toString()), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
                    }
                    if (ok) current->setQFProperty(name, v, editable, true);
                }
            }
        }
    }
}

void QFRawDataPropertyEditor::deletePropClicked() {
    if (current) {
        QModelIndexList l=tvProperties->selectionModel()->selectedIndexes();
        QStringList props;
        for (int i=0; i<l.size(); i++) {
            /*int row=l[i].row();
            QString prop=current->getVisibleProperty(row);
            if ((!prop.isEmpty()) && current->propertyExists(prop) && current->isPropertyUserEditable(prop)) {
                props.append(prop);
            }*/
            props.append(current->getPropertyModel()->getPropertyForRow(l[i].row()));
        }
        if (props.size()>0) {
            for (int i=0; i<props.size(); i++) {
                //std::cout<<"deleting "<<props[i].toStdString()<<std::endl;
                //qDebug()<<"QFRawDataPropertyEditor::deletePropClicked '"<<props[i]<<"'";
                if (!props[i].isEmpty() && current->isPropertyUserEditable(props[i])) current->deleteProperty(props[i]);
            }
        }

    }
}

void QFRawDataPropertyEditor::setSettings(ProgramOptions* settings) {
    //std::cout<<"QFRawDataPropertyEditor::setSettings("<<settings<<")\n";
    this->settings=settings;
    if (current && tabMain) {
        for (int i=0; i<current->getEditorCount(); i++) {
            if (qobject_cast<QFRawDataEditor *>(tabMain->widget(i))) {
                qobject_cast<QFRawDataEditor *>(tabMain->widget(i))->setSettings(settings, id);
            }
        }
    }
    readSettings();
}

void QFRawDataPropertyEditor::readSettings() {
    if (!settings) return;
    loadWidgetGeometry(*(settings->getQSettings()), this, QPoint(20, 20), QSize(800, 600), "rawdatapropeditor/");
    if (tabMain) {
        //helpWidget->readSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
        //int idx=settings->getQSettings()->value("rawdatapropeditor/currentTab", 0).toInt();
        //if ((idx>=0) && (idx<tabMain->count())) tabMain->setCurrentIndex(idx);
    }
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->readSettings();
        }
    }
    currentSaveDir=settings->getQSettings()->value("rawdatapropeditor/lastSaveDir", currentSaveDir).toString();
}

void QFRawDataPropertyEditor::writeSettings() {
    if (!settings) return;
    saveWidgetGeometry(*(settings->getQSettings()), this, "rawdatapropeditor/");
    if (tabMain) {
        settings->getQSettings()->setValue("rawdatapropeditor/currentTab", tabMain->currentIndex());
        //helpWidget->writeSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
    }
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->writeSettings();
        }
    }
    settings->getQSettings()->setValue("rawdatapropeditor/lastSaveDir", currentSaveDir);
}

void QFRawDataPropertyEditor::tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    QMap<int, QString> names;
    QMap<int, double> sum, sum2, count;
    for (int i=0; i<sel.size(); i++) {
        int r=sel[i].row();
        //int c=sel[i].column();
        QVariant dataS=sel[i].data(QFRDRResultsModel::SumRole);
        QVariant dataS2=sel[i].data(QFRDRResultsModel::Sum2Role);
        QVariant dataC=sel[i].data(QFRDRResultsModel::CountRole);
        QString name=sel[i].data(QFRDRResultsModel::NameRole).toString();
        double d=0;
        bool ok=false;
        if (dataS.canConvert(QVariant::Double) && dataS2.canConvert(QVariant::Double) && dataC.canConvert(QVariant::LongLong)) {
            if (names.contains(r)) {
                sum[r] = sum[r]+dataS.toDouble();
                sum2[r] = sum2[r]+dataS2.toDouble();
                count[r] = count[r]+dataC.toLongLong();
            } else {
                sum[r] = dataS.toDouble();
                sum2[r] = dataS2.toDouble();
                count[r] = dataC.toLongLong();
                names[r] = name;
            }

        }

        /*QVariant data=sel[i].data(QFRDRResultsModel::ValueRole);
        QString name=sel[i].data(QFRDRResultsModel::NameRole).toString();
        double d=0;
        bool ok=false;
        if (data.canConvert(QVariant::Double)) {
            d=data.toDouble();
            ok=true;
        }
        if (data.canConvert(QVariant::PointF)) {
            d=data.toPointF().x();
            ok=true;
        }
        if (data.type()==QVariant::String) { ok=false; }
        if (ok) {
            if (names.contains(r)) {
                sum[r] = sum[r]+d;
                sum2[r] = sum2[r]+d*d;
                count[r] = count[r]+1;
            } else {
                sum[r] = d;
                sum2[r] = d*d;
                count[r] = 1;
                names[r] = name;
            }
        }*/
    }

    int lineHeight=labAveragedresults->fontMetrics().lineSpacing();
    int maxheight=labAveragedresults->maximumHeight();
    int linespercol=maxheight/(2*lineHeight)-1;
    QStringList datalist;
    QMapIterator<int, QString> it(names);
    while (it.hasNext()) {
        it.next();
        QString name=it.value();
        int i=it.key();
        if (count[i]>0) {
            double avg=sum[i]/count[i];
            double error=sqrt(sum2[i]/count[i]-sum[i]*sum[i]/count[i]/count[i]);
            datalist.append(QString("<td align=\"right\"><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%1:&nbsp;</b></td><td>&nbsp;&nbsp;%2 &plusmn; %3&nbsp;</td>").arg(name).arg(roundWithError(avg, error)).arg(roundError(error)));
        }
    }
    QString results=tr("<table border=\"1\" cellspacing=\"0\"><tr>");
    int cols=(int)ceil((double)datalist.size()/(double)linespercol);
    for (int i=0; i<cols; i++) {
        results+=tr("<th align=\"right\"><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Field:&nbsp;</b></th><th><b>&nbsp;&nbsp;Average &plusmn; StdDev&nbsp;</b></th>");
    }
    results+=tr("</th>");
    for (int r=0; r<linespercol; r++) {
        results+=tr("<tr>");
        for (int c=0; c<cols; c++) {
            int idx=c*linespercol+r;
            if ((idx>=0)&&(idx<datalist.size())) {
                results+=datalist[idx];
            } else {
                results+=tr("<td></td><td></td>");
            }
        }
        results+=tr("</tr>");
    }
    results+=tr("</table>");
    labAveragedresults->setText(results);
}

void QFRawDataPropertyEditor::deleteSelectedResults() {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    if (sel.size()>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(this, tr("QuickFit %1").arg(QF_VERSION),
                     tr("Do you really want to delete the selected results?"),
                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            // first we store a list of all items to be deleted (one list for the evaluation name (col)
            // and one list for the result name (row).
            //std::cout<<"\n\n-- deleteing "<<sel.size()<<" items:\n";
            /*QStringList row, col;
            for (int i=0; i<sel.size(); i++) {
                QString hh=tvResults->model()->headerData(sel[i].column(), Qt::Horizontal).toString();
                QString vh=tvResults->model()->headerData(sel[i].row(), Qt::Vertical).toString();
                //std::cout<<"hh="<<hh.toStdString()<<"   vh="<<vh.toStdString()<<"\n";
                row.append(vh);
                col.append(hh);
            }
            //QAbstractItemModel* m=tvResults->model();
            //tvResults->setModel(NULL);
            current->disableEmitResultsChanged();
            for (int i=0; i<qMin(row.size(), col.size()); i++) {
                QString hh=col[i];
                QString vh=row[i];
                current->resultsRemove(hh, vh, false);
            }
            current->enableEmitResultsChanged();
            current->emitResultsChanged();
            //tvResults->setModel(m);*/

            current->disableEmitResultsChanged();
            for (int i=0; i<sel.size(); i++) {
                QString en=sel[i].data(QFRDRResultsModel::EvalNameRole).toString();
                QString rn=sel[i].data(QFRDRResultsModel::ResultNameRole).toString();
                current->resultsRemove(en, rn, false);
            }
            current->enableEmitResultsChanged(true);

        }
    }
}

void QFRawDataPropertyEditor::saveResults() {
    if (current) {
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->resultsSaveToCSV(fileName, false, ";", ',', '"');
            } else if (f==2) {
                ok=current->resultsSaveToSYLK(fileName, false);
            } else if (f==3) {
                ok=current->resultsSaveToCSV(fileName, false, "\t", '.', '"');
            } else {
                ok=current->resultsSaveToCSV(fileName, false, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }
    }
}

void QFRawDataPropertyEditor::saveResultsAveraged()
{
    if (current) {
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->resultsSaveToCSV(fileName, true, ";", ',', '"');
            } else if (f==2) {
                ok=current->resultsSaveToSYLK(fileName, true);
            } else if (f==3) {
                ok=current->resultsSaveToCSV(fileName, true, "\t", '.', '"');
            } else {
                ok=current->resultsSaveToCSV(fileName, true, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }
    }
}

QMenu *QFRawDataPropertyEditor::getHelpMenu() const
{
    return menuHelp;
}

void QFRawDataPropertyEditor::resizePropertiesLater() {
    timerResizeProps->setSingleShot(true);
    timerResizeProps->start(150);
}

