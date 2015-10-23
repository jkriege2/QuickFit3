#include "qfefcssimmainwidnow.h"
#include "ui_qfefcssimmainwidnow.h"
#include "programoptions.h"
#include <QDir>
#include "qfstyledbutton.h"

QFEFCSSimMainWidnow::QFEFCSSimMainWidnow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFEFCSSimMainWidnow)
{
    ui->setupUi(this);
    QFStyledButton* btn;
    ui->edtExeDir->addButton(btn=new QFStyledButton(QFStyledButton::SelectDirectory, ui->edtExeDir, ui->edtExeDir));
    ui->edtSimulator->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtSimulator, ui->edtSimulator));
    ui->edtSimulatorSpectra->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtSimulatorSpectra, ui->edtSimulatorSpectra));
    btn->setFilter(tr("Executable Files (*.exe *.);;All Files (*.*"));
    ui->tabWidget->setTabsClosable(true);
    loadSettings();
    //on_btnNew_clicked();
}

QFEFCSSimMainWidnow::~QFEFCSSimMainWidnow()
{
    saveSettings();
    delete ui;
}

void QFEFCSSimMainWidnow::loadSettings()
{
#if defined(Q_OS_WIN)
    QString sim=QApplication::applicationDirPath()+"/diffusion4.exe";
#elif defined(Q_OS_MAC)
    QString sim=QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_fcssimulator")+"/diffusion4";
#else
    QString sim=QApplication::applicationDirPath()+"/diffusion4";
#endif
    QDir spectradir(QFPluginServices::getInstance()->getPluginAssetsDirectory("qfe_fcssimulator"));
    spectradir.cd("spectra");
    ProgramOptions::getConfigWindowGeometry(this, "QFEFCSSimMainWidnow/window");
    ProgramOptions::getConfigQLineEdit(ui->edtExeDir, "QFEFCSSimMainWidnow/edtExeDir", QDir::homePath());
    ProgramOptions::getConfigQLineEdit(ui->edtSimulatorSpectra, "QFEFCSSimMainWidnow/edtSimulatorSpectra", spectradir.absolutePath());
    ProgramOptions::getConfigQLineEdit(ui->edtSimulator, "QFEFCSSimMainWidnow/edtSimulator", sim);
    ProgramOptions::getConfigQSpinBox(ui->spinMaxProcesses, "QFEFCSSimMainWidnow/spinMaxProcesses", 1);
    ProgramOptions::getConfigQCheckBox(ui->chkSaveLog, "QFEFCSSimMainWidnow/chkSaveLog", true);
}

void QFEFCSSimMainWidnow::saveSettings()
{
    ProgramOptions::setConfigWindowGeometry(this, "QFEFCSSimMainWidnow/window");
    ProgramOptions::setConfigQLineEdit(ui->edtExeDir, "QFEFCSSimMainWidnow/edtExeDir");
    ProgramOptions::setConfigQLineEdit(ui->edtSimulator, "QFEFCSSimMainWidnow/edtSimulator");
    ProgramOptions::setConfigQLineEdit(ui->edtSimulatorSpectra, "QFEFCSSimMainWidnow/edtSimulatorSpectra");
    ProgramOptions::setConfigQSpinBox(ui->spinMaxProcesses, "QFEFCSSimMainWidnow/spinMaxProcesses");
    ProgramOptions::setConfigQCheckBox(ui->chkSaveLog, "QFEFCSSimMainWidnow/chkSaveLog");
}

bool QFEFCSSimMainWidnow::mayStartProcess() const
{
    int cnt=0;
    for (int i=0; i<tabs.size(); i++) {
        if (tabs[i]->isRunning()) cnt++;
    }
    if (cnt<ui->spinMaxProcesses->value()) return true;
    return false;
}

QString QFEFCSSimMainWidnow::getWorkingDir() const
{
    return ui->edtExeDir->text();
}
QString QFEFCSSimMainWidnow::getSpectraDir() const
{
    return ui->edtSimulatorSpectra->text();
}

bool QFEFCSSimMainWidnow::getSaveLog() const
{
    return ui->chkSaveLog->isChecked();
}

QString QFEFCSSimMainWidnow::getSimulator() const
{
    return ui->edtSimulator->text();
}

void QFEFCSSimMainWidnow::on_btnOpen_clicked()
{
    tabs.append(new QFEFCSSimScriptTab(this));
    if (!tabs.last()->loadFile()) {
        delete tabs.last();
        tabs.removeLast();
        return;
    }
    connect(tabs.last(), SIGNAL(textChanged(bool)), this, SLOT(textChanged(bool)));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(lastTab(), tabs.last()->getFilename()));
}

void QFEFCSSimMainWidnow::on_btnNew_clicked()
{
    tabs.append(new QFEFCSSimScriptTab(this));
    connect(tabs.last(), SIGNAL(textChanged(bool)), this, SLOT(textChanged(bool)));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(lastTab(), tabs.last()->getFilename()));
}

void QFEFCSSimMainWidnow::on_btnNewFromTemplate_clicked()
{
    tabs.append(new QFEFCSSimScriptTab(this));
    if (!tabs.last()->loadTemplate()) {
        delete tabs.last();
        tabs.removeLast();
        return;
    }
    connect(tabs.last(), SIGNAL(textChanged(bool)), this, SLOT(textChanged(bool)));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(lastTab(), tabs.last()->getFilename()));
}

void QFEFCSSimMainWidnow::on_btnSave_clicked()
{
    int idx=tabs.indexOf(currentTab());
    if (idx>=0) {
        tabs[idx]->save();
        ui->tabWidget->setTabText(idx,tabs[idx]->getFilename());
    }
}

void QFEFCSSimMainWidnow::on_btnSaveAs_clicked()
{
    int idx=tabs.indexOf(currentTab());
    if (idx>=0) {
        tabs[idx]->saveAs();
        ui->tabWidget->setTabText(idx,tabs[idx]->getFilename());
    }
}

void QFEFCSSimMainWidnow::on_btnClose_clicked()
{
    close();
}

void QFEFCSSimMainWidnow::on_tabWidget_tabCloseRequested(int i)
{
    closeTab(i);
}

void QFEFCSSimMainWidnow::on_btnExecute_clicked()
{
    int idx=tabs.indexOf(currentTab());
    if (idx>=0) {
        tabs[idx]->execute();
    }
}

void QFEFCSSimMainWidnow::on_btnStop_clicked()
{
    int idx=tabs.indexOf(currentTab());
    if (idx>=0) {
        tabs[idx]->cancelExecution();
    }
}

bool QFEFCSSimMainWidnow::closeTab(int i)
{
    if (i>=0 && i<tabs.size() && i<ui->tabWidget->count()) {
        if (tabs[i]->isRunning()) {
            QMessageBox::StandardButton ret;
            ret = QMessageBox::warning(this, tr("Closing FCS Simulator Script"),
                                       tr("The simulator script\n  %1\nis currently executed.\n"
                                          "Do you want to cancel execution?\n\n  [Yes]: Cancel Execution\n  [No]:  Go on executing ...").arg(tabs[i]->getFullFilename()),
                                       QMessageBox::Yes | QMessageBox::No);
            if (ret==QMessageBox::Yes) {
                tabs[i]->cancelExecution();
            } else {
                return false;
            }
        }
        if (tabs[i]->hasChanged()) {
            QMessageBox::StandardButton ret;
            ret = QMessageBox::warning(this, tr("Closing FCS Simulator Script"),
                                       tr("The simulator script\n  %1\nhas been modified.\n"
                                          "Do you want to save your changes?").arg(tabs[i]->getFullFilename()),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) {
                tabs[i]->save();
            } else if (ret == QMessageBox::Cancel) {
                return false;
            }
        }
        tabs.removeAt(i);
        ui->tabWidget->removeTab(i);
        return true;
    }
    return false;
}

void QFEFCSSimMainWidnow::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("qfe_fcssimulator");
}

void QFEFCSSimMainWidnow::on_btnManual_clicked()
{
    QUrl url(QString("file:///")+QFPluginServices::getInstance()->getPluginHelpDirectory("qfe_fcssimulator")+"/manual.pdf", QUrl::TolerantMode);
    qDebug()<<"opening "<<url;
    QDesktopServices::openUrl(url);
}

void QFEFCSSimMainWidnow::textChanged(bool changed)
{
    int idx=tabs.indexOf(qobject_cast<QFEFCSSimScriptTab*>(sender()));
    if (idx>=0) {
        if (changed) {
            ui->tabWidget->setTabText(idx,tabs[idx]->getFilename()+" *");
        } else {
            ui->tabWidget->setTabText(idx,tabs[idx]->getFilename());
        }
    }
}

void QFEFCSSimMainWidnow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    for (int i=tabs.size()-1; i>=0; i--) {
        if (!closeTab(i)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

QFEFCSSimScriptTab *QFEFCSSimMainWidnow::lastTab() const
{
    if (tabs.size()<=0) return NULL;
    return qobject_cast<QFEFCSSimScriptTab*>(tabs.last());
}

QFEFCSSimScriptTab *QFEFCSSimMainWidnow::currentTab() const
{
    return qobject_cast<QFEFCSSimScriptTab*>(ui->tabWidget->currentWidget());
}
