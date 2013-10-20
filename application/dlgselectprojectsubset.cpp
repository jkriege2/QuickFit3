#include "dlgselectprojectsubset.h"
#include "ui_dlgselectprojectsubset.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfprojecttreemodel.h"

DlgSelectProjectSubset::DlgSelectProjectSubset(QFProject *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSelectProjectSubset)
{
    this->project=project;
    ui->setupUi(this);

    loadWidgetGeometry(*(ProgramOptions::staticGetQSettings()), this, pos(), size(), "DlgSelectProjectSubset/window");
    loadSplitter(*(ProgramOptions::staticGetQSettings()), ui->splitter, "DlgSelectProjectSubset/splitter1");
    ui->treeView->setModel(project->getTreeModel());
    ui->labFile->setText(project->getFile());
    ui->labCreator->setText(project->getCreator());
    ui->labName->setText(project->getName());
    ui->labDescription->setText(project->getDescription());
    ui->treeView->expandAll();
}

DlgSelectProjectSubset::~DlgSelectProjectSubset()
{
    saveSplitter(*(ProgramOptions::staticGetQSettings()), ui->splitter, "DlgSelectProjectSubset/splitter1");
    saveWidgetGeometry(*(ProgramOptions::staticGetQSettings()), this, "DlgSelectProjectSubset/window");
    delete ui;
}

void DlgSelectProjectSubset::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgselectprojectsubset.html");

}
