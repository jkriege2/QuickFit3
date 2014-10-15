/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfrawdatapropertyeditor_private.h"
#include "qfrawdatapropertyeditor.h"
#include "dlgnewproperty.h"
#include "qfrawdatarecordfactory.h"
#include "qfversion.h"
#include "qfhtmldelegate.h"
#include "qfstyleditemdelegate.h"
#include <QModelIndex>
#include <QModelIndexList>
#include "qfhistogramservice.h"
#include <QItemSelectionModel>
#include <QItemSelection>
#include "qfparametercorrelationservice.h"
#include "qfenhancedtabwidget.h"
#include "datatools.h"

QFRawDataPropertyEditor_private::QFRawDataPropertyEditor_private(QFRawDataPropertyEditor *parent) :
    QObject(parent)
{
    d=parent;
    treeNextRecord=NULL;
    projectTree=NULL;

}

void QFRawDataPropertyEditor_private::nameChanged(const QString& text) {
    if (current) {
        current->setName(text);
    }
}

void QFRawDataPropertyEditor_private::folderChanged(const QString& text) {
    if (current) {
        current->setFolder(text);
    }
}

void QFRawDataPropertyEditor_private::roleChanged(const QString &text)
{
    if (current && current->isRoleUserEditable()) {
        current->setRole(text);
    }
}

void QFRawDataPropertyEditor_private::groupChanged(int group)
{
    if (current) {
        if (group-1>=current->getProject()->getRDRGroupCount()) {
            bool ok=false;
            QString new_name=QInputDialog::getText(d, tr("RDR group"), tr("name for the new group:"), QLineEdit::Normal, QString(""), &ok);
            if (ok) {
                group=current->getProject()->addRDRGroup(new_name)+1;
                reloadGroupList();
            } else {
                group=0;
            }
        }
        current->setGroup(group-1);
    }
}

void QFRawDataPropertyEditor_private::descriptionChanged() {
    if (current) {
        current->setDescription(pteDescription->toPlainText());
    }
}




void QFRawDataPropertyEditor_private::createWidgets() {
    QLabel* l;
    QVBoxLayout* ml=new QVBoxLayout();
    d->setLayout(ml);

    menuBar=new QMenuBar(d);
    menuBar->setVisible(true);
    ml->addWidget(menuBar);
    menuRDR=menuBar->addMenu("Recor&d");
    menuResults=menuBar->addMenu("&Results");
    menuHelp=menuBar->addMenu("&Help");

    ml->setContentsMargins(2,2,2,2);
    QHBoxLayout* vl=new QHBoxLayout();
    ml->addLayout(vl);
    actPrevious=new QAction(QIcon(":/lib/prop_previous.png"), tr("&previous"), d);
    actPrevious->setToolTip(tr("move to previous record"));
    actPreviousSameRole=new QAction(QIcon(":/lib/prop_previous.png"), tr("&previous, same role"), d);
    actPreviousSameRole->setToolTip(tr("move to previous record of same role"));
    actExportPluginData=new QAction(QIcon(":/lib/export_data.png"), tr("&Export Data"), d);
    actExportPluginData->setToolTip(tr("export the data from the current RDR into a file (enabled, if the plugin supports this feature)"));

    btnPrevious=new QToolButton(d);
    btnPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnPrevious->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnPrevious->setDefaultAction(actPrevious);
    btnPreviousSameRole=new QToolButton(d);
    btnPreviousSameRole->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnPreviousSameRole->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnPreviousSameRole->setDefaultAction(actPreviousSameRole);
    vl->addWidget(btnPreviousSameRole);
    vl->addWidget(btnPrevious);
    connect(actPrevious, SIGNAL(triggered()), this, SLOT(previousPressed()));
    connect(actPreviousSameRole, SIGNAL(triggered()), this, SLOT(previousOfRolePressed()));
    connect(actExportPluginData, SIGNAL(triggered()), this, SLOT(exportData()));

    btnSelectFromTree=new QPushButton(tr("select RDR"), d);
    btnSelectFromTree->setToolTip(tr("click this to select the next record to display from a tree of all records in the current project which are of the semae type"));
    connect(btnSelectFromTree, SIGNAL(clicked()), this, SLOT(selectRecordFromTreeClicked()));
    vl->addWidget(btnSelectFromTree);

    actNext=new QAction(QIcon(":/lib/prop_next.png"), tr("&next"), d);
    actNext->setToolTip(tr("move to next record"));
    actNextSameRole=new QAction(QIcon(":/lib/prop_next.png"), tr("&next, same role"), d);
    actNextSameRole->setToolTip(tr("move to next record of same role"));

    btnNext=new QToolButton(d);
    btnNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnNext->setDefaultAction(actNext);

    btnNextSameRole=new QToolButton(d);
    btnNextSameRole->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnNextSameRole->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnNextSameRole->setDefaultAction(actNextSameRole);

    connect(actNext, SIGNAL(triggered()), this, SLOT(nextPressed()));
    connect(actNextSameRole, SIGNAL(triggered()), this, SLOT(nextOfRolePressed()));
    vl->addWidget(btnNext);
    vl->addWidget(btnNextSameRole);
    labTopIcon=new QLabel(d);
    vl->addWidget(labTopIcon);
    labTop=new QLabel(d);
    vl->addWidget(labTop);
    vl->addStretch();

    actDelete=new QAction(QIcon(":/lib/item_delete.png"), tr("&Remove Record"), d);
    actDelete->setToolTip(tr("removes the currently displayed record from the project"));
    btnDeleteReord=new QToolButton(d);
    btnDeleteReord->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnDeleteReord->setDefaultAction(actDelete);
    btnDeleteReord->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vl->addWidget(btnDeleteReord);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteRecord()));

    actClose=new QAction(QIcon(":/lib/exit.png"), tr("&Close Window"), d);
    connect(actClose, SIGNAL(triggered()), d, SLOT(close()));
    menuRDR->addAction(actPreviousSameRole);
    menuRDR->addAction(actPrevious);
    menuRDR->addAction(actNext);
    menuRDR->addAction(actNextSameRole);
    menuRDR->addSeparator();
    menuRDR->addAction(actDelete);
    menuRDR->addSeparator();
    menuRDR->addAction(actExportPluginData);
    menuRDR->addSeparator();
    menuRDR->addAction(actClose);

    actHelp=new QAction(QIcon(":/lib/help.png"), tr("&Help"), d);
    actHelp->setToolTip(tr("display online-help"));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(displayHelp()));

    actHelpPlugin=new QAction(QIcon(":/lib/help.png"), tr("&Plugin Help"), d);
    actHelpPlugin->setToolTip(tr("display online-help for the specific plugin"));
    connect(actHelpPlugin, SIGNAL(triggered()), this, SLOT(displayHelpPlugin()));

    actHelpPluginTutorial=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("&Plugin Tutorial"), d);
    actHelpPluginTutorial->setToolTip(tr("display the tutorial for the specific plugin"));
    connect(actHelpPluginTutorial, SIGNAL(triggered()), this, SLOT(displayHelpPluginTutorial()));

    actHelpPluginCopyright=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("&Plugin Copyright"), d);
    actHelpPluginCopyright->setToolTip(tr("display copyright note for the specific plugin"));
    connect(actHelpPluginCopyright, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));

    actHelpRDR=new QAction(QIcon(":/lib/help_rdr.png"), tr("&Raw data record help"), d);
    actHelpRDR->setToolTip(tr("display online-help common to all plugins, i.e. for the basic record editor dialog"));
    connect(actHelpRDR, SIGNAL(triggered()), this, SLOT(displayHelpRDR()));

    menuHelp->addAction(actHelpRDR);
    menuHelp->addAction(actHelpPlugin);
    menuHelp->addAction(actHelpPluginTutorial);
    menuHelp->addAction(actHelpPluginCopyright);

    btnHelp=new QToolButton(d);
    btnHelp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnHelp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnHelp->setDefaultAction(actHelp);

    vl->addWidget(btnHelp);

    tabMain=new QFEnhancedTabWidget(d);
    ml->addWidget(tabMain);
    connect(tabMain, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    QWidget* w=new QWidget(tabMain);
    QGridLayout* fl=new QGridLayout();
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
    QHBoxLayout* ptl=new QHBoxLayout();
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
    QHBoxLayout* ntl=new QHBoxLayout();
    ntl->setContentsMargins(0,0,0,0);
    ntl->addWidget(edtName, 1);
    ntl->addStretch();

    cmbGroup=new QComboBox(d);
    cmbGroup->setEditable(false);
    cmbGroup->setEnabled(true);
    cmbGroup->setMaximumWidth(400);
    ntl->addWidget(l=new QLabel(tr("&group:"), d));
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(cmbGroup);
    ntl->addWidget(cmbGroup);

    cmbRole=new QComboBox(d);
    cmbRole->setEditable(true);
    cmbRole->setEnabled(false);
    ntl->addWidget(l=new QLabel(tr("&role:"), d));
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(cmbRole);
    ntl->addWidget(cmbRole);

    fl->addLayout(ntl, 2, 1);
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
    lstFiles->setContextMenuPolicy(Qt::ActionsContextMenu);

    actOpenFileDirectory=new QAction(QIcon(":/lib/file_opendir.png"), tr("&open file's directory"), d);

    actAddFile=new QAction(QIcon(":/lib/file_add.png"), tr("&add file"), d);
    actAddFile->setEnabled(false);
    actRemoveFile=new QAction(QIcon(":/lib/file_delete.png"), tr("&delete file"), d);
    actRemoveFile->setEnabled(false);
    actEditFile=new QAction(QIcon(":/lib/file_edit.png"), tr("&edit file"), d);
    actEditFile->setEnabled(false);
    actCopyFile=new QAction(QIcon(":/lib/file_copy.png"), tr("&duplicate and edit file"), d);
    actCopyFile->setEnabled(false);
    actMoveFilesUp=new QAction(QIcon(":/lib/file_up.png"), tr("&move files up"), d);
    actMoveFilesUp->setEnabled(false);
    actMoveFilesDown=new QAction(QIcon(":/lib/file_down.png"), tr("&move files down"), d);
    actMoveFilesDown->setEnabled(false);

    actEnableFileActions=new QAction(tr("enable file edit functionality"), d);
    actEnableFileActions->setCheckable(true);
    actEnableFileActions->setChecked(false);


    connect(actEnableFileActions, SIGNAL(triggered(bool)), this, SLOT(showEditFilesWarning(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actAddFile, SLOT(setEnabled(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actRemoveFile, SLOT(setEnabled(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actEditFile, SLOT(setEnabled(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actCopyFile, SLOT(setEnabled(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actMoveFilesUp, SLOT(setEnabled(bool)));
    connect(actEnableFileActions, SIGNAL(triggered(bool)), actMoveFilesDown, SLOT(setEnabled(bool)));
    connect(actAddFile, SIGNAL(triggered()), this, SLOT(addFile()));
    connect(actRemoveFile, SIGNAL(triggered()), this, SLOT(removeFile()));
    connect(actEditFile, SIGNAL(triggered()), this, SLOT(editFile()));
    connect(actCopyFile, SIGNAL(triggered()), this, SLOT(copyFile()));
    connect(actMoveFilesUp, SIGNAL(triggered()), this, SLOT(moveFilesUp()));
    connect(actMoveFilesDown, SIGNAL(triggered()), this, SLOT(moveFilesDown()));
    connect(actOpenFileDirectory, SIGNAL(triggered()), this, SLOT(openFilesDirectory()));

    lstFiles->addAction(actOpenFileDirectory);
    lstFiles->addAction(getSeparatorAction(d));
    lstFiles->addAction(actEnableFileActions);
    lstFiles->addAction(getSeparatorAction(d));
    lstFiles->addAction(actAddFile);
    lstFiles->addAction(actEditFile);
    lstFiles->addAction(actCopyFile);
    lstFiles->addAction(actRemoveFile);
    lstFiles->addAction(getSeparatorAction(d));
    lstFiles->addAction(actMoveFilesUp);
    lstFiles->addAction(actMoveFilesDown);


    //fl->addRow(tr("&Files:"), lstFiles);
    fl->addWidget((l=new QLabel(tr("&Files:"), w)), 5, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(lstFiles);
    fl->addWidget(lstFiles, 5, 1);
    fl->setRowStretch(5,1);
    //fl->setRowStretch(flcounter-1, 1);
    tvProperties=new QEnhancedTableView(w);
    QFontMetrics fm(d->font());
    tvProperties->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.1));
    tvProperties->horizontalHeader()->setStretchLastSection(true);
    //tvProperties->setSizePolicy(tvProperties->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
    tvProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tvProperties->setItemDelegate(propDelegate=new QFPropertyItemDelegate(tvProperties));
    /*paramFilterProxy=new QSortFilterProxyModel(d);
    paramFilterProxy->setDynamicSortFilter(false);
    paramFilterProxy->setFilterKeyColumn(0);
    paramFilterProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    paramFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tvProperties->setModel(paramFilterProxy);*/
    tvProperties->setModel(NULL);

    QWidget* widProperties=new QWidget(d);
    QHBoxLayout* pl1=new QHBoxLayout();
    widProperties->setLayout(pl1);
    widProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pl1->setContentsMargins(0,0,0,0);
    pl1->addWidget(tvProperties);
    QVBoxLayout* pl2=new QVBoxLayout();
    pl1->addLayout(pl2);
    btnNewProperty=createButtonAndActionShowText(actNewProperty, QIcon(":/lib/prop_add.png"), tr("In&sert Property"), d);
    connect(actNewProperty, SIGNAL(triggered()), this, SLOT(newPropClicked()));
    pl2->addWidget(btnNewProperty);
    btnDeleteProperty=createButtonAndActionShowText(actDeleteProperty, QIcon(":/lib/prop_delete.png"), tr("&Delete Property"), d);
    connect(actDeleteProperty, SIGNAL(triggered()), this, SLOT(deletePropClicked()));
    pl2->addWidget(btnDeleteProperty);

    btnCopyProperty=createButtonAndActionShowText(actCopyProperty, QIcon(":/lib/edit_copy.png"), tr("&Copy Property"), d);
    connect(actCopyProperty, SIGNAL(triggered()), this, SLOT(copyProperties()));
    pl2->addWidget(btnCopyProperty);

    btnCutProperty=createButtonAndActionShowText(actCutProperty, QIcon(":/lib/edit_cut.png"), tr("C&ut Property"), d);
    connect(actCutProperty, SIGNAL(triggered()), this, SLOT(cutProperties()));
    pl2->addWidget(btnCutProperty);

    btnPasteProperty=createButtonAndActionShowText(actPasteProperty, QIcon(":/lib/edit_paste.png"), tr("&Paste Property"), d);
    connect(actPasteProperty, SIGNAL(triggered()), this, SLOT(pasteProperties()));
    pl2->addWidget(btnPasteProperty);

    pl2->addStretch();
    //fl->addRow(tr("Properties:"), widProperties);
    fl->addWidget((l=new QLabel(tr("Properties:"), w)), 6, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addWidget(widProperties, 6, 1);
    fl->setRowStretch(6,3);
    //fl->setRowStretch(flcounter-1, 5);

    actMakePropertyEditable=new QAction(tr("Make property editable"), d);
    connect(actMakePropertyEditable, SIGNAL(triggered()), this, SLOT(makePropEditable()));

    tvProperties->addAction(actNewProperty);
    tvProperties->addAction(actDeleteProperty);
    tvProperties->addAction(getSeparatorAction(d));
    tvProperties->addAction(actCopyProperty);
    tvProperties->addAction(actCutProperty);
    tvProperties->addAction(actPasteProperty);
    tvProperties->addAction(getSeparatorAction(d));
    tvProperties->addAction(actMakePropertyEditable);
    tvProperties->setContextMenuPolicy(Qt::ActionsContextMenu);


    widResults=new QWidget(d);
    QVBoxLayout* rwvlayout=new QVBoxLayout();
    widResults->setLayout(rwvlayout);
    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->setItemDelegate(new QFHTMLDelegate(tvResults));
    tvResults->setContextMenuPolicy(Qt::ActionsContextMenu);


    tbResults=new QToolBar("toolbar_rdr_results", d);
    rwvlayout->addWidget(tbResults);
    /*actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), d);
    tbResults->addAction(actCopyResults);
    actCopyResultsNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) without header row/column"), d);
    tbResults->addAction(actCopyResultsNoHead);*/
    actCopyResults=tvResults->getActCopyExcel();
    actCopyResultsNoHead=tvResults->getActCopyExcelNoHead();
    tvResults->addActionsToToolbar(tbResults);
    actCopyMedianQuantilesResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection as median+q25+q75"), d);
    actCopyMedianQuantilesNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection as median+q25+q75, w/o header"), d);
    actCopyValErrResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs"), d);
    tbResults->addAction(actCopyValErrResults);
    actCopyValErrResultsNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs, but without a header (so data only)"), d);
    tbResults->addAction(actCopyValErrResultsNoHead);
    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), d);
    actSaveResultsAveraged=new QAction(tr("Save all results to file with averaged vector/matrix results"), d);
    tbResults->addAction(actSaveResults);
    tbResults->addSeparator();
    actStatistics=new QAction(QIcon(":/lib/result_statistics_row.png"), tr("histogram: row-wise"), d);
    tbResults->addAction(actStatistics);
    actStatisticsComparing=new QAction(QIcon(":/lib/result_statistics_compare.png"), tr("Result histogram: cell-wise"), d);
    tbResults->addAction(actStatisticsComparing);
    actCorrelation=new QAction(QIcon(":/lib/result_correlation.png"), tr("Result correlation"), d);
    tbResults->addAction(actCorrelation);

    QMenu* menuCopyExpanded=new QMenu(tr("Copy selection in expanded form"), tvResults);
    menuCopyExpanded->setIcon(QIcon(":/lib/copy16.png"));
    tvResults->addAction(menuCopyExpanded->menuAction());
    actCopyExpanded=new QAction( tr("Copy Selection in expanded form"), d);
    menuCopyExpanded->addAction(actCopyExpanded);;
    actCopyExpandedFlipped=new QAction( tr("Copy Selection in expanded form, flipped"), d);
    menuCopyExpanded->addAction(actCopyExpandedFlipped);;
    actCopyExpandedNoHead=new QAction( tr("Copy Selection in expanded form, w/o header"), d);
    menuCopyExpanded->addAction(actCopyExpandedNoHead);;
    actCopyExpandedNoHeadFlipped=new QAction( tr("Copy Selection in expanded form, w/o header, flipped"), d);
    menuCopyExpanded->addAction(actCopyExpandedNoHeadFlipped);;
    actCopyExpandedNoHeadMatlab=new QAction( tr("Copy Selection in expanded form, to Matlab"), d);
    menuCopyExpanded->addAction(actCopyExpandedNoHeadMatlab);;
    actCopyExpandedNoHeadMatlabFlipped=new QAction( tr("Copy Selection in expanded form, to Matlab, flipped"), d);
    menuCopyExpanded->addAction(actCopyExpandedNoHeadMatlabFlipped);;

    tbResults->addSeparator();
    actDeleteResults=new QAction(QIcon(":/lib/delete16.png"), tr("Delete selected records"), d);
    actDeleteResults->setShortcut(QKeySequence::Delete);
    tbResults->addAction(actDeleteResults);

    tbResults->addSeparator();

    chkShowAvg=new QCheckBox(tr("show Avg+/-SD for vector/matrrix results"), tbResults);
    chkShowAvg->setChecked(true);
    tbResults->addWidget(chkShowAvg);

    tbResults=new QToolBar("toolbar_rdr_results_filter", d);
    rwvlayout->addWidget(tbResults);
    tbResults->addWidget(new QLabel("  evaluation filter: "));
    edtFilterEvaluation=new QFEnhancedLineEdit(d);
    edtFilterEvaluation->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterEvaluation, edtFilterEvaluation));
    edtFilterEvaluation->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "items where the evaluation contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                        "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterEvaluation=new QFCompleterFromFile(d);
    edtFilterEvaluation->setCompleter(compFilterEvaluation);

    tbResults->addWidget(edtFilterEvaluation);
    tbResults->addWidget(new QLabel(tr("  and not contains: "), d));
    edtFilterEvaluationNot=new QFEnhancedLineEdit(d);
    edtFilterEvaluationNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterEvaluationNot, edtFilterEvaluationNot));
    edtFilterEvaluationNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "items where the evaluation does NOT containa match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                        "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                       "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterEvaluationNot=new QFCompleterFromFile(d);
    edtFilterEvaluationNot->setCompleter(compFilterEvaluationNot);
    tbResults->addWidget(edtFilterEvaluationNot);
    chkFilterEvaluationRegExp=new QCheckBox(tr("RegExp"), d);
    chkFilterEvaluationRegExp->setChecked(false);
    tbResults->addWidget(chkFilterEvaluationRegExp);
    connect(edtFilterEvaluationNot, SIGNAL(textChanged(QString)), this, SLOT(filterEvaluationTextChanged(QString)));
    connect(edtFilterEvaluation, SIGNAL(textChanged(QString)), this, SLOT(filterEvaluationTextChanged(QString)));


    tbResults->addSeparator();
    tbResults->addWidget(new QLabel("  result filter (contains): "));
    edtFilterResults=new QFEnhancedLineEdit(d);
    edtFilterResults->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResults, edtFilterResults));
    tbResults->addWidget(edtFilterResults);
    tbResults->addWidget(new QLabel(tr("  and not contains: "), d));
    edtFilterResultsNot=new QFEnhancedLineEdit(d);
    edtFilterResultsNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResultsNot, edtFilterResultsNot));
    tbResults->addWidget(edtFilterResultsNot);
    chkFilterResultsRegExp=new QCheckBox(tr("RegExp"), d);
    chkFilterResultsRegExp->setChecked(false);
    tbResults->addWidget(chkFilterResultsRegExp);
    edtFilterResults->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                    "Simply enter a filter string and the table will only display those"
                                    "rows where the result name contains a match to the filter string."
                                    "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                    "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                    "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResults=new QFCompleterFromFile(d);
    edtFilterResults->setCompleter(compFilterResults);
    edtFilterResultsNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                    "Simply enter a filter string and the table will only display those"
                                    "rows where the result name contains NO match to the filter string."
                                    "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                    "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                    "(match a single character), or full regular expressions in the filter string.<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResultsNot=new QFCompleterFromFile(d);
    edtFilterResultsNot->setCompleter(compFilterResultsNot);
    connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));
    connect(edtFilterResults, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));




    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);


    //connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    //connect(actCopyResultsNoHead, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcelNoHead()));
    connect(actCopyValErrResults, SIGNAL(triggered()), this, SLOT(copyValErrResults()));
    connect(actCopyValErrResultsNoHead, SIGNAL(triggered()), this, SLOT(copyValErrResultsNoHead()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actSaveResultsAveraged, SIGNAL(triggered()), this, SLOT(saveResultsAveraged()));
    connect(actDeleteResults, SIGNAL(triggered()), this, SLOT(deleteSelectedResults()));
    connect(actCopyMedianQuantilesResults, SIGNAL(triggered()), this, SLOT(copyMedianQuantilesResults()));
    connect(actCopyMedianQuantilesNoHead, SIGNAL(triggered()), this, SLOT(copyMedianQuantilesResultsNoHead()));
    connect(actCopyExpanded, SIGNAL(triggered()), this, SLOT(copyResultsExpanded()));
    connect(actCopyExpandedNoHead, SIGNAL(triggered()), this, SLOT(copyResultsExpandedNoHead()));
    connect(actCopyExpandedNoHeadMatlab, SIGNAL(triggered()), this, SLOT(copyResultsExpandedNoHeadMatlab()));
    connect(actCopyExpandedFlipped, SIGNAL(triggered()), this, SLOT(copyResultsExpandedFlipped()));
    connect(actCopyExpandedNoHeadFlipped, SIGNAL(triggered()), this, SLOT(copyResultsExpandedNoHeadFlipped()));
    connect(actCopyExpandedNoHeadMatlabFlipped, SIGNAL(triggered()), this, SLOT(copyResultsExpandedNoHeadMatlabFlipped()));

    connect(actCorrelation, SIGNAL(triggered()), this, SLOT(showCorrelation()));
    connect(actStatistics, SIGNAL(triggered()), this, SLOT(showStatistics()));
    connect(actStatisticsComparing, SIGNAL(triggered()), this, SLOT(showStatisticsComparing()));

    tvResults->addAction(actCopyValErrResults);
    tvResults->addAction(actCopyValErrResultsNoHead);
    tvResults->addAction(actSaveResults);
    tvResults->addAction(actSaveResultsAveraged);
    tvResults->addAction(actDeleteResults);
    tvResults->addAction(actStatistics);
    tvResults->addAction(actStatisticsComparing);
    tvResults->addAction(actCorrelation);

    menuResults->addAction(actSaveResults);
    menuResults->addAction(actSaveResultsAveraged);
    menuResults->addSeparator();
    menuResults->addAction(actDeleteResults);
    menuResults->addSeparator();
    tvResults->addActionsToMenu(menuResults);
    //menuResults->addAction(actCopyResults);
    //menuResults->addAction(actCopyResultsNoHead);
    menuResults->addSeparator();
    menuResults->addAction(actCopyValErrResults);
    menuResults->addAction(actCopyValErrResultsNoHead);
    menuResults->addAction(actCopyMedianQuantilesResults);
    menuResults->addAction(actCopyMedianQuantilesNoHead);
    menuResults->addSeparator();
    menuResults->addAction(actCopyExpanded);
    menuResults->addAction(actCopyExpandedFlipped);
    menuResults->addAction(actCopyExpandedNoHead);
    menuResults->addAction(actCopyExpandedNoHeadFlipped);
    menuResults->addAction(actCopyExpandedNoHeadMatlab);
    menuResults->addAction(actCopyExpandedNoHeadMatlabFlipped);
    menuResults->addSeparator();
    menuResults->addAction(actStatistics);
    menuResults->addAction(actStatisticsComparing);
    menuResults->addAction(actCorrelation);


    tabMain->addTab(widResults, tr("Evaluation &Results"));
    currentTabChanged(0);


}


void QFRawDataPropertyEditor_private::newPropClicked() {
    if (current) {
/*        QStringList items;
        items << tr("integer") << tr("double") << tr("string") << tr("boolean");
        bool ok;
        QString item = QInputDialog::getItem(d, tr("Insert New Property ..."), tr("Property Type:"), items, 1, false, &ok);
        int i=items.indexOf(item);
        if (ok) {
            QString name = QInputDialog::getText(d, tr("Insert New Property ..."),  tr("Property Name:"), QLineEdit::Normal, tr("PROPERTY_NAME"), &ok);
            if (ok) {
                QVariant v;
                if (i==0) v.setValue((int)0);
                if (i==1) v.setValue((double)0.0);
                if (i==2) v.setValue(QString(""));
                if (i==3) v.setValue((bool)false);
                current->setProperty(name, v);
            }
        }*/
        dlgNewProperty* d=new dlgNewProperty(this->d);
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
            //qDebug()<<"QFRawDataPropertyEditor_private::newPropClicked()  pn="<<d->propertyName()<<"   val="<<v<<"   ok="<<ok;
            if (ok) current->setQFProperty(d->propertyName(), v, true, true);
            else QMessageBox::critical(d, tr("QuickFit"), tr("The property '%1' may not bge changed or created, as it is marked readonly!"));
        }

        delete d;
    }
}

void QFRawDataPropertyEditor_private::makePropEditable() {
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
            if (QMessageBox::warning(d, tr("QuickFit"), tr("You are about to change the editablility of a property!\n"
                                                              "Possibly it was non-editable on purpose, so changing this\n"
                                                              "is at your own risk and may in some cases be overridden \n"
                                                              "when the project is loaded the next time!\n\n"
                                                              "Are you sure you want to make this property user-editable?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {

                for (int i=0; i<props.size(); i++) {
                    //std::cout<<"deleting "<<props[i].toStdString()<<std::endl;
                    //qDebug()<<"QFRawDataPropertyEditor_private::deletePropClicked '"<<props[i]<<"'";
                    //if (!props[i].isEmpty() && current->isPropertyUserEditable(props[i])) current->deleteProperty(props[i]);
                    current->setQFProperty(props[i], current->getProperty(props[i]), true, true);
                }
            }
        }

    }
}

void QFRawDataPropertyEditor_private::copyProperties() {
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

void QFRawDataPropertyEditor_private::cutProperties()
{
    copyProperties();
    deletePropClicked();
}

void QFRawDataPropertyEditor_private::pasteProperties()
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
                        ok= QMessageBox::question(d, tr("QuickFit"), tr("The property '%1' already exists.\n"
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

void QFRawDataPropertyEditor_private::deletePropClicked() {
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
                //qDebug()<<"QFRawDataPropertyEditor_private::deletePropClicked '"<<props[i]<<"'";
                if (!props[i].isEmpty() && current->isPropertyUserEditable(props[i])) current->deleteProperty(props[i]);
            }
        }

    }
}


void QFRawDataPropertyEditor_private::tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
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

void QFRawDataPropertyEditor_private::deleteSelectedResults() {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    if (sel.size()>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(d, tr("QuickFit %1").arg(qfInfoVersion()),
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

void QFRawDataPropertyEditor_private::saveResults() {
    if (current) {
        QStringList f=QFDataExportHandler::getFormats();
        QString lastDir=ProgramOptions::getConfigValue("QFRawDataPropertyEditor/lastDataDir", "").toString();
        QString selFilter=ProgramOptions::getConfigValue("QFRawDataPropertyEditor/lastDataFilter", "").toString();
        QString fn=qfGetSaveFileName(d, tr("Save results to file ..."), lastDir, f.join(";;"), &selFilter);
        if (fn.size()>0) {
            int flip=QMessageBox::question(d, tr("Save results to file ..."), tr("Flip table?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
            if (flip==QMessageBox::Yes || flip==QMessageBox::No) {
                current->resultsSave(fn, f.indexOf(selFilter), false, flip==QMessageBox::Yes);
                ProgramOptions::setConfigValue("QFRawDataPropertyEditor/lastDataDir", lastDir);
                ProgramOptions::setConfigValue("QFRawDataPropertyEditor/lastDataFilter", selFilter);
            }
        }






        /*QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(d, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->resultsSaveToCSV(fileName, false, ";", ',', '"');
            } else if (f==2) {
                ok=current->resultsSaveToSYLK(fileName, false);
            } else if (f==3) {
                ok=current->resultsSaveToSYLK(fileName, false, true);
            } else if (f==4) {
                ok=current->resultsSaveToCSV(fileName, false, "\t", '.', '"');
            } else {
                ok=current->resultsSaveToCSV(fileName, false, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }*/
    }
}

void QFRawDataPropertyEditor_private::saveResultsAveraged()
{
    if (current) {
        /*QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(d, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->resultsSaveToCSV(fileName, true, ";", ',', '"');
            } else if (f==2) {
                ok=current->resultsSaveToSYLK(fileName, true);
            } else if (f==3) {
                ok=current->resultsSaveToSYLK(fileName, true);
            } else if (f==4) {
                ok=current->resultsSaveToCSV(fileName, true, "\t", '.', '"');
            } else {
                ok=current->resultsSaveToCSV(fileName, true, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }*/

        QStringList f=QFDataExportHandler::getFormats();
        QString lastDir=ProgramOptions::getConfigValue("QFRawDataPropertyEditor/lastDataDir", "").toString();
        QString selFilter=ProgramOptions::getConfigValue("QFRawDataPropertyEditor/lastDataFilter", "").toString();
        QString fn=qfGetSaveFileName(d, tr("Save results to file ..."), lastDir, f.join(";;"), &selFilter);
        if (fn.size()>0) {
            int flip=QMessageBox::question(d, tr("Save results to file ..."), tr("Flip table?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
            if (flip==QMessageBox::Yes || flip==QMessageBox::No) {
                current->resultsSave(fn, f.indexOf(selFilter), true, flip==QMessageBox::Yes);
                ProgramOptions::setConfigValue("QFRawDataPropertyEditor/lastDataDir", lastDir);
                ProgramOptions::setConfigValue("QFRawDataPropertyEditor/lastDataFilter", selFilter);
            }
        }
    }
}



void QFRawDataPropertyEditor_private::resizePropertiesLater() {
    timerResizeProps->setSingleShot(true);
    timerResizeProps->start(150);
}

void QFRawDataPropertyEditor_private::addFile() {
    if (current && (current->isFilesListEditable() & QFRawDataRecord::FilesEditable)) {
        if (current->isFilesListEditable() & QFRawDataRecord::CustomFilesAddFunction) {
            QStringList files, types, descriptions;
            if (current->selectNewFiles(files, types, descriptions)) {
                for (int i=0; i<qMax(qMax(files.size(), types.size()), descriptions.size()); i++) {
                    current->addFile(files.value(i, ""), types.value(i, ""), descriptions.value(i, ""));
                }
                current->reloadFromFiles();
            }
        } else {
            QFFileEditDialog* dlg=new QFFileEditDialog(d);
            dlg->init("", "", "", current->getPossibleFilesTypes());
            if (dlg->exec()) {
                current->addFile(dlg->getFile(), dlg->getType(), dlg->getDescription());
                current->reloadFromFiles();
            }
            delete dlg;
        }
    }
}

void QFRawDataPropertyEditor_private::removeFile() {
    if (current && (current->isFilesListEditable() & QFRawDataRecord::FilesEditable)) {
        int i=lstFiles->currentRow();
        QStringList ffiles, ttypes, ddescriptions;
        ffiles.append(current->getFiles().value(i, ""));
        ttypes.append(current->getFilesTypes().value(i, ""));
        ddescriptions.append(current->getFilesDescriptions().value(i, ""));
        if (i>=0 && i<current->getFilesCount() && current->mayDeleteFiles(ffiles, ttypes, ddescriptions)) {
            if (QMessageBox::question(d, tr("QuickFit: remove file from raw data record"), tr("Do you really want to remove the file\n   '%1'\nfrom the raw data record?").arg(lstFiles->item(i)->text()), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                current->deleteFile(i);
                current->reloadFromFiles();
            }
        }
    }
}

void QFRawDataPropertyEditor_private::copyFile() {
    if (current && (current->isFilesListEditable() & QFRawDataRecord::FilesEditable) && ((current->isFilesListEditable() & QFRawDataRecord::CustomFilesAddFunction)==0)) {
        int i=lstFiles->currentRow();
        if (i>=0 && i<current->getFilesCount()) {
            QFFileEditDialog* dlg=new QFFileEditDialog(d);
            QStringList files_types=current->getPossibleFilesTypes();
            if (!files_types.contains(current->getFileType(i))) files_types.prepend(current->getFileType(i));
            dlg->init(current->getFileName(i), current->getFileType(i), current->getFileDescription(i), files_types);
            if (dlg->exec()) {
                current->addFile(dlg->getFile(), dlg->getType(), dlg->getDescription());
                current->reloadFromFiles();
            }
            delete dlg;
        }
    }

}

void QFRawDataPropertyEditor_private::moveFilesUp() {
    if (current && (current->isFilesListEditable() & QFRawDataRecord::FilesEditable)) {
        QList<int> items;
        QModelIndexList il=lstFiles->selectionModel()->selectedIndexes();
        for (int i=0; i<il.size(); i++) {
            items<<il[i].row();
        }
        if (items.size()>0) {
            current->moveFilesUp(items);
        }
    }
}

void QFRawDataPropertyEditor_private::moveFilesDown() {
    if (current && (current->isFilesListEditable() & QFRawDataRecord::FilesEditable)) {
        QList<int> items;
        QModelIndexList il=lstFiles->selectionModel()->selectedIndexes();
        for (int i=0; i<il.size(); i++) {
            items<<il[i].row();
        }
        if (items.size()>0) {
            current->moveFilesDown(items);
        }
    }
}

void QFRawDataPropertyEditor_private::openFilesDirectory() {
    if (current) {
        int i=lstFiles->currentRow();
        if (i>=0 && i<current->getFilesCount()) {
            QDesktopServices::openUrl(QUrl(QFileInfo(current->getFileName(i)).absolutePath()));
        }
    }
}

void QFRawDataPropertyEditor_private::showAvgClicked(bool checked)
{
    if (current) {
        current->resultsGetModel()->setShowVectorMatrixAvg(checked);
    }
}

void QFRawDataPropertyEditor_private::showStatistics()
{
    QFHistogramService* hs=QFHistogramService::getInstance();
    if (hs&&current) {
        QModelIndexList idxs=tvResults->selectionModel()->selectedIndexes();
        QMap<int, QFHistogramService::Histogram> hists;
        for (int i=0; i<idxs.size(); i++) {
            QFHistogramService::Histogram h;
            int col=idxs[i].row();
            if (!hists.contains(col)) {
                h.name=current->resultsGetModel()->headerData(col, Qt::Vertical).toString();
                hists[col]=h;
            }
            QString ename=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::EvalNameRole).toString();
            QString rname=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::ResultNameRole).toString();

            hists[col].data<<current->resultsGetAsDoubleList(ename, rname);
        }

        bool onePerCol=true;
        if (hists.size()>1) {
            onePerCol=QMessageBox::question(d, tr("Data histogram"), tr("You selected %1 rows.\nShould QuickFit open\n   [Yes] one histogram window per row, or\n   [No]  combine all data into one window?").arg(hists.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
        }

        if (onePerCol) {
            QMapIterator<int, QFHistogramService::Histogram> ii(hists);
            while (ii.hasNext()) {
                ii.next();
                QString histID=QString("hist")+current->getType()+QString::number(current->getID())+"_"+QString::number(ii.key());
                hs->getCreateView(histID, tr("Histogram from %1: %2").arg(current->getName()).arg(ii.value().name));
                hs->clearView(histID);
                hs->addHistogramToView(histID, ii.value());
            }
        } else {
            QString histID=QString("hist")+current->getType()+QString::number(current->getID());
            hs->getCreateView(histID, tr("Histogram from %1").arg(current->getName()));
            hs->clearView(histID);
            QMapIterator<int, QFHistogramService::Histogram> ii(hists);
            while (ii.hasNext()) {
                ii.next();
                hs->addHistogramToView(histID, ii.value());
            }
        }
    }
}

void QFRawDataPropertyEditor_private::showStatisticsComparing()
{
    QFHistogramService* hs=QFHistogramService::getInstance();
    if (hs&&current) {
        QModelIndexList idxs=tvResults->selectionModel()->selectedIndexes();
        QMap<int, QList<QFHistogramService::Histogram> > hists;
        for (int i=0; i<idxs.size(); i++) {
            QFHistogramService::Histogram h;
            int col=idxs[i].row();
            if (!hists.contains(col)) {
                QList<QFHistogramService::Histogram> l;
                hists[col]=l;
            }
            QString ename=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::EvalNameRole).toString();
            QString rname=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::ResultNameRole).toString();

            h.name=current->resultsGetModel()->headerData(col, Qt::Vertical).toString();
            h.data<<current->resultsGetAsDoubleList(ename, rname);
            hists[col].append(h);
        }

        bool onePerCol=true;
        if (hists.size()>1) {
            onePerCol=QMessageBox::question(d, tr("Data histogram"), tr("You selected %1 rows.\nShould QuickFit open\n   [Yes] one histogram window per row, or\n   [No]  combine all data into one window?").arg(hists.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
        }

        if (onePerCol) {
            QMapIterator<int, QList<QFHistogramService::Histogram> > ii(hists);
            while (ii.hasNext()) {
                ii.next();
                QString histID=QString("hist")+current->getType()+QString::number(current->getID())+"_"+QString::number(ii.key());
                hs->getCreateView(histID, tr("Histogram from %1").arg(current->getName()));
                hs->clearView(histID);
                for (int i=0; i<ii.value().size(); i++) {
                    hs->addHistogramToView(histID, ii.value().at(i));
                }
            }
        } else {
            QString histID=QString("hist")+current->getType()+QString::number(current->getID());
            hs->getCreateView(histID, tr("Histogram from %1").arg(current->getName()));
            hs->clearView(histID);
            QMapIterator<int,  QList<QFHistogramService::Histogram> > ii(hists);
            while (ii.hasNext()) {
                ii.next();
                for (int i=0; i<ii.value().size(); i++) {
                    hs->addHistogramToView(histID, ii.value().at(i));
                }
            }
        }
    }
}

void QFRawDataPropertyEditor_private::showCorrelation()
{
    QFParameterCorrelationService* hs=QFParameterCorrelationService::getParameterCorrelationInstance();
    if (hs&&current) {
        QModelIndexList idxs=tvResults->selectionModel()->selectedIndexes();
        QMap<int, QVector<double> > hists;
        QMap<int, QString > names;
        QSet<int> cols;
        for (int i=0; i<idxs.size(); i++) {
            QVector<double> h;
            int col=idxs[i].row();
            if (!hists.contains(col)) {
                names[col]=current->resultsGetModel()->headerData(col, Qt::Vertical).toString();
                hists[col]=h;
            }
            QString ename=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::EvalNameRole).toString();
            QString rname=current->resultsGetModel()->data(idxs[i], QFRDRResultsModel::ResultNameRole).toString();

            hists[col]<<current->resultsGetAsDoubleList(ename, rname);
            cols.insert(col);
        }

        if (cols.size()==2) {
            QList<int> cs=cols.toList();
            QString histID=QString("pcorr")+current->getType()+QString::number(current->getID());
            hs->getCreateParameterCorrelationView(histID, tr("Correlation from %1").arg(current->getName()));
            hs->clearParameterCorrelationView(histID);
            int i1=cs.first();
            int i2=cs.last();
            QString n=names.value(i1)+tr(" vs. ")+names.value(i2);
            QFParameterCorrelationService::ParameterCorrelation d;
            d.nameX=QString("\\verb{%1}").arg(names.value(i1));
            d.nameY=QString("\\verb{%1}").arg(names.value(i2));
            d.dataX=hists.value(i1);
            d.dataY=hists.value(i2);
            hs->addCorrelationToView(histID, d);
        } else {
            QMessageBox::critical(d, tr("Data Correlation"), tr("You have to select data from exactly two rows for data correlation!"));
        }
    }

}

void QFRawDataPropertyEditor_private::showEditFilesWarning(bool activated) {
    if (activated) QMessageBox::warning(d, tr("QuickFit: Edit Files"), tr("This will activate the edit functionalities for files.\n\n"
                                                              "Note however that you have to use these functions with great care, "
                                                              "as changing the files may render the raw data record unusable, e.g. "
                                                              "if you delet a necessary file! Also note that changes will only take "
                                                              "effect after SAVING AND RELOADING the project."));
}

void QFRawDataPropertyEditor_private::editFile()
{
    if (current) {
        int i=lstFiles->currentRow();
        if (i>=0 && i<current->getFilesCount()) {
            QFFileEditDialog* dlg=new QFFileEditDialog(d);
            QStringList files_types=current->getPossibleFilesTypes();
            if (!files_types.contains(current->getFileType(i))) files_types.prepend(current->getFileType(i));
            dlg->init(current->getFileName(i), current->getFileType(i), current->getFileDescription(i), files_types);
            if (dlg->exec()) {
                current->setFileName(i, dlg->getFile());
                current->setFileType(i, dlg->getType());
                current->setFileDecsription(i, dlg->getDescription());
            }
            delete dlg;
        }
    }
}

void QFRawDataPropertyEditor_private::resizePropertiesTable() {
    if (!current || !tvProperties) return;

    tvProperties->resizeColumnToContents(0);
    tvProperties->resizeColumnToContents(1);
    tvProperties->resizeRowsToContents();
    tvProperties->horizontalHeader()->setStretchLastSection(true);
}


void QFRawDataPropertyEditor_private::setCurrent(QFRawDataRecord* c) {
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
        disconnect(cmbRole, SIGNAL(currentIndexChanged(QString)), this, SLOT(roleChanged(QString)));
        disconnect(cmbRole, SIGNAL(editTextChanged(QString)), this, SLOT(roleChanged(QString)));
        disconnect(cmbGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(groupChanged(int)));
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(current->getProject(), SIGNAL(structureChanged()), this, SLOT(reloadGroupList()));
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
        disconnect(chkShowAvg, SIGNAL(toggled(bool)), this, SLOT(showAvgClicked(bool)));





        if (c) {
            actExportPluginData->setEnabled(c->getExportFiletypes().size()>0);
            btnPreviousSameRole->setVisible(c->showNextPreviousOfSameRoleButton());
            btnNextSameRole->setVisible(c->showNextPreviousOfSameRoleButton());
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
        QString prefix="";
        int whm=ProgramOptions::getConfigValue("quickfit/windowheadermode", 1).toInt();
        if (whm==1) {
            prefix=QFileInfo(current->getProject()->getFile()).fileName();
        } else if (whm==2) {
            prefix=current->getProject()->getName();
            if (prefix.isEmpty()) prefix=QFileInfo(current->getProject()->getFile()).fileName();
        }
        if (!prefix.isEmpty()) prefix += QString(": ");
        d->setWindowTitle(tr("%3Raw Data (%1): %2").arg(current->getTypeDescription()).arg(current->getName()).arg(prefix));
        d->setWindowIcon(current->getLargeIcon());
        if (current->getType()!=oldType) {
            editorList.clear();
            for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName(i);
                QFRawDataEditor* e=current->createEditor(services, d, i, NULL);
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
        cmbRole->clear();
        cmbRole->addItems(current->getAvailableRoles());
        cmbRole->setEditable(cmbRole->count()<=0);
        cmbRole->setEnabled(current->isRoleUserEditable());
        cmbRole->setEditText(current->getRole());
        if (cmbRole->count()>0) cmbRole->setCurrentIndex(cmbRole->findText(current->getRole()));

        reloadGroupList();
        cmbGroup->setCurrentIndex(current->getGroup()+1);

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
        connect(cmbRole, SIGNAL(editTextChanged(QString)), this, SLOT(roleChanged(QString)));
        connect(cmbRole, SIGNAL(currentIndexChanged(QString)), this, SLOT(roleChanged(QString)));
        connect(cmbGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(groupChanged(int)));
        connect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        connect(current->getProject(), SIGNAL(structureChanged()), this, SLOT(reloadGroupList()));
        connect(current, SIGNAL(basicPropertiesChanged()), this, SLOT(basicPropsChanged()));
        connect(current->getPropertyModel(), SIGNAL(modelReset()), this, SLOT(resizePropertiesLater()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeRowsToContents()));
        connect(edtFilterResults, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setResultFilter(QString)));
        connect(chkFilterResultsRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setResultFilterUsesRegExp(bool)));
        connect(edtFilterEvaluation, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setEvaluationFilter(QString)));
        connect(chkFilterEvaluationRegExp, SIGNAL(clicked(bool)), current->resultsGetModel(), SLOT(setEvaluationFilterUsesRegExp(bool)));
        connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setResultFilterNot(QString)));
        connect(edtFilterEvaluationNot, SIGNAL(textChanged(QString)), current->resultsGetModel(), SLOT(setEvaluationFilterNot(QString)));
        connect(chkShowAvg, SIGNAL(toggled(bool)), this, SLOT(showAvgClicked(bool)));
        QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/");
        compFilterEvaluation->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterevals.txt");
        compFilterEvaluationNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterevalsnot.txt");
        compFilterResults->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterresults.txt");
        compFilterResultsNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_rdrfilterresults_not.txt");

        disconnect(actEnableFileActions, SIGNAL(triggered(bool)), this, SLOT(showEditFilesWarning(bool)));
        actEnableFileActions->setChecked(true);
        actEnableFileActions->setChecked(false);
        actEnableFileActions->setEnabled((current->isFilesListEditable()&QFRawDataRecord::FilesEditable)!=0);
        actEnableFileActions->setChecked((current->isFilesListEditable()&QFRawDataRecord::FilesEditable)!=0);
        actAddFile->setEnabled(actEnableFileActions->isChecked());
        actRemoveFile->setEnabled(actEnableFileActions->isChecked());
        actEditFile->setEnabled(actEnableFileActions->isChecked());
        actCopyFile->setEnabled(actEnableFileActions->isChecked());
        actMoveFilesUp->setEnabled(actEnableFileActions->isChecked());
        actMoveFilesDown->setEnabled(actEnableFileActions->isChecked());
        connect(actEnableFileActions, SIGNAL(triggered(bool)), this, SLOT(showEditFilesWarning(bool)));
        current->resultsGetModel()->setResultFilter(edtFilterResults->text());
        current->resultsGetModel()->setResultFilterUsesRegExp(chkFilterResultsRegExp->isChecked());
        current->resultsGetModel()->setEvaluationFilter(edtFilterEvaluation->text());
        current->resultsGetModel()->setEvaluationFilterUsesRegExp(chkFilterEvaluationRegExp->isChecked());
        current->resultsGetModel()->setResultFilterNot(edtFilterResultsNot->text());
        current->resultsGetModel()->setEvaluationFilterNot(edtFilterEvaluationNot->text());

        /*QPoint pos;
        pos.setX(current->getProject()->getProperty(QString("rawdatapropeditor%1/posx").arg(id), 20).toInt());
        pos.setY(current->getProject()->getProperty(QString("rawdatapropeditor%1/posy").arg(id), 20).toInt());
        QSize size;
        size.setWidth(current->getProject()->getProperty(QString("rawdatapropeditor%1/width").arg(id), 800).toInt());
        size.setHeight(current->getProject()->getProperty(QString("rawdatapropeditor%1/height").arg(id), 650).toInt());
        resize(size.boundedTo(QApplication::desktop()->screenGeometry(this).size()));
        if (pos.x()<-width() || pos.x()>QApplication::desktop()->screenGeometry(d).width()-30) pos.setX(0);
        if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(d).height()) pos.setY(0);
        move(pos);
        */

        /*helpWidget->clear();
        QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
        helpWidget->updateHelp(dll);*/

    } else {
        disconnect(actEnableFileActions, SIGNAL(triggered(bool)), this, SLOT(showEditFilesWarning(bool)));
        actEnableFileActions->setChecked(false);
        actEnableFileActions->setEnabled(false);
        connect(actEnableFileActions, SIGNAL(triggered(bool)), this, SLOT(showEditFilesWarning(bool)));
        propDelegate->setProject(NULL);
        edtName->setText("");
        edtName->setEnabled(false);
        edtFolder->setText("");
        edtFolder->setEnabled(false);
        cmbRole->setEnabled(false);
        cmbRole->clear();
        cmbRole->setEditable(true);
        cmbRole->setEditText("");
        cmbRole->setCurrentIndex(-1);
        cmbGroup->clear();
        cmbGroup->setCurrentIndex(-1);
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
    showAvgClicked(chkShowAvg->isChecked());
    checkHelpAvailable();
    //std::cout<<"creating new editors ... DONE!\n";
}

void QFRawDataPropertyEditor_private::reloadGroupList()
{
    if (!current) return;
    int i=cmbGroup->currentIndex();
    cmbGroup->setUpdatesEnabled(false);
    cmbGroup->blockSignals(true);
    cmbGroup->clear();
    QStringList items;
    items<<tr("no group");
    items<<current->getProject()->getRDRGroupNames();
    items<<tr("--- new group ---");
    cmbGroup->addItems(items);
    if (i==items.size()-1) i=0;
    cmbGroup->setCurrentIndex(i);
    cmbGroup->blockSignals(false);
    cmbGroup->setUpdatesEnabled(true);
}

void QFRawDataPropertyEditor_private::selectRecordFromTreeClicked()
{
    if (!current) return;
    if (!treeNextRecord) {
        treeNextRecord=new QTreeView(d);
        treeNextRecord->setVisible(false);
        treeNextRecord->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeNextRecord->setHeaderHidden(true);
        projectTree=new QFProjectTreeModel(treeNextRecord);
        projectTree->init(current->getProject(), true, false);
        projectTree->setRDRTypeFilter(current->getType());
        treeNextRecord->setModel(projectTree);
        connect(treeNextRecord, SIGNAL(clicked(QModelIndex)), this, SLOT(selectRecordFromTreeSelected(QModelIndex)));
    }
    if (!treeNextRecord->isVisible()) {
        treeNextRecord->move(btnSelectFromTree->x(), btnSelectFromTree->y()+btnSelectFromTree->height());
        QSize s=treeNextRecord->sizeHint();
        s.setWidth(qMin(d->width()-btnSelectFromTree->x()-5, qMax(s.width(), d->width()/3)));
        s.setHeight(qMin(d->height()-btnSelectFromTree->y()-btnSelectFromTree->height()-5, (30*QFontMetrics(treeNextRecord->font()).height())/2));
        treeNextRecord->resize(s);
        treeNextRecord->setCurrentIndex(projectTree->index(current));
        treeNextRecord->expandAll();
        treeNextRecord->setVisible(true);
        treeNextRecord->setFocus();
    } else {
        treeNextRecord->close();
        treeNextRecord->hide();
    }

}

void QFRawDataPropertyEditor_private::selectRecordFromTreeSelected(const QModelIndex &index)
{
    if (treeNextRecord) {
        treeNextRecord->close();
    }
    if (projectTree && current) {
        QFRawDataRecord* n=projectTree->getRawDataByIndex(index);
        if (n) {
            if (treeNextRecord) {
                treeNextRecord->hide();
                /*treeNextRecord->disconnect();
                treeNextRecord->setModel(NULL);
                if (projectTree) delete projectTree;
                treeNextRecord->deleteLater();
                treeNextRecord=NULL;
                projectTree=NULL;*/
            }
            setCurrent(n);
            return;
        }
    }
    if (treeNextRecord) {
        treeNextRecord->hide();
        /*treeNextRecord->disconnect();
        treeNextRecord->setModel(NULL);
        if (projectTree) delete projectTree;
        treeNextRecord->deleteLater();
        treeNextRecord=NULL;
        projectTree=NULL;*/
    }

}

void QFRawDataPropertyEditor_private::exportData()
{
    if (current) {
        QStringList ids=current->getExportFiletypes();
        QStringList filters=current->getExportDialogFiletypes().split(";;");
        if (ids.size()>0 && filters.size()>0) {
            QString filter=filters[0];
            QString filename=qfGetSaveFileNameSet("rdr/export-data/", d, current->getExportDialogTitle(), QString(), filters.join(";;"), &filter);
            current->exportData(ids.value(filters.indexOf(filter), ""), filename);
        }
    }
}




void QFRawDataPropertyEditor_private::displayHelp() {
    if (tabMain->currentIndex()==0 || tabMain->currentIndex()==tabMain->count()-1) {
        displayHelpRDR();
    } else {
        displayHelpPlugin();
    }
}

void QFRawDataPropertyEditor_private::displayHelpRDR() {
    QString dll=services->getOptions()->getAssetsDirectory()+QString("/help/qf3_rdrscreen.html");
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor_private::displayHelpPlugin() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor_private::displayHelpPluginTutorial() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginTutorialMain(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor_private::displayHelpPluginCopyright() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginCopyrightFile(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor_private::copyValErrResults() {
    tvResults->copySelectionAsValueErrorToExcel(QFRDRResultsModel::AvgRole, QFRDRResultsModel::SDRole, true, Qt::Vertical);
}
void QFRawDataPropertyEditor_private::copyValErrResultsNoHead() {
    tvResults->copySelectionAsValueErrorToExcel(QFRDRResultsModel::AvgRole, QFRDRResultsModel::SDRole, false, Qt::Vertical);
}

void QFRawDataPropertyEditor_private::copyMedianQuantilesResults()
{
    tvResults->copySelectionAsMedianQuantilesToExcel(QFRDRResultsModel::MedianRole, QFRDRResultsModel::Quantile25Role, QFRDRResultsModel::Quantile75Role, true, Qt::Vertical);
}

void QFRawDataPropertyEditor_private::copyMedianQuantilesResultsNoHead()
{
    tvResults->copySelectionAsMedianQuantilesToExcel(QFRDRResultsModel::MedianRole, QFRDRResultsModel::Quantile25Role, QFRDRResultsModel::Quantile75Role, false, Qt::Vertical);
}

void QFRawDataPropertyEditor_private::copyResultsExpanded()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, true);
}

void QFRawDataPropertyEditor_private::copyResultsExpandedNoHead()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, false);
}

void QFRawDataPropertyEditor_private::copyResultsExpandedNoHeadMatlab()
{
    tvResults->copySelectionToMatlabExpandedNoHead(Qt::EditRole);
}

void QFRawDataPropertyEditor_private::copyResultsExpandedFlipped()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, true, true);
}

void QFRawDataPropertyEditor_private::copyResultsExpandedNoHeadFlipped()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, false, true);
}

void QFRawDataPropertyEditor_private::copyResultsExpandedNoHeadMatlabFlipped()
{
    tvResults->copySelectionToMatlabExpandedNoHead(Qt::EditRole, true);
}
void QFRawDataPropertyEditor_private::currentTabChanged(int tab) {
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

void QFRawDataPropertyEditor_private::checkHelpAvailable() {
    if (!current) {
        actHelpPlugin->setVisible(false);
        actHelpPluginTutorial->setVisible(false);
        actHelpPluginCopyright->setVisible(false);
    } else {
        QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
        actHelpPlugin->setVisible(QFile::exists(dll));
        dll=current->getProject()->getRawDataRecordFactory()->getPluginTutorialMain(current->getType());
        actHelpPluginTutorial->setVisible(QFile::exists(dll));
        dll=current->getProject()->getRawDataRecordFactory()->getPluginCopyrightFile(current->getType());
        actHelpPluginCopyright->setVisible(QFile::exists(dll));
    }
}



void QFRawDataPropertyEditor_private::nextPressed() {
    if (current) {
        QFRawDataRecord* n=current->getNextOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor_private::previousPressed() {
    if (current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor_private::nextOfRolePressed()
{
    if (current) {
        QString role=current->getRole();
        int cnt=0;
        QFRawDataRecord* n=current->getNextOfSameType();
        int cntMax=current->getProject()->getRawDataCount();
        while (n && n->getRole()!=role && current!=n && cnt<cntMax) {
            n=n->getNextOfSameType();
            cnt++;
        }
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor_private::previousOfRolePressed()
{
    if (current) {
        QString role=current->getRole();
        int cnt=0;
        QFRawDataRecord* n=current->getPreviousOfSameType();
        int cntMax=current->getProject()->getRawDataCount();
        while (n && n->getRole()!=role && current!=n && cnt<cntMax) {
            n=n->getPreviousOfSameType();
            cnt++;
        }
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor_private::deleteRecord() {
    if (current) {
        int ret = QMessageBox::question(d, tr("QuickFit %1").arg(qfInfoVersionFull()),
                                tr("Do you really want to delete the current record?\n   '%1'").arg(current->getName()),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QPointer<QFRawDataRecord> m=current;
            previousPressed();
            current->getProject()->deleteRawData(m->getID());
            if (current==m) {
                d->close();
            }
        }
    }
}

void QFRawDataPropertyEditor_private::basicPropsChanged() {
    if (current->getName()!=edtName->text()) {
        edtName->setText(current->getName());
    }
    if (current->getFolder()!=edtFolder->text()) {
        edtFolder->setText(current->getFolder());
    }
    if (current->getDescription()!=pteDescription->toPlainText()) {
        pteDescription->setPlainText(current->getDescription());
    }
    if (current->getRole()!=cmbRole->currentText()) {
        if (cmbRole->count()>0) cmbRole->setCurrentIndex(cmbRole->findText(current->getRole()));
        if (cmbRole->isEditable()) cmbRole->setEditText(current->getRole());
    }
    if (current->getGroup()!=cmbGroup->currentIndex()-1) {
        reloadGroupList();
        if (cmbGroup->count()>0) cmbGroup->setCurrentIndex(current->getGroup()+1);
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

void QFRawDataPropertyEditor_private::filterEvaluationTextChanged(const QString &text)
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

void QFRawDataPropertyEditor_private::filterResultsTextChanged(const QString &text)
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

void QFRawDataPropertyEditor_private::recordAboutToBeDeleted(QFRawDataRecord* r) {
    if ((current==r) && current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        if (!n) n=current->getNextOfSameType();
        setCurrent(n);
    }
    if (r) {
        if (r->getProject()->getRawDataCount()<=1) {
            d->close();
        }
    }
}
