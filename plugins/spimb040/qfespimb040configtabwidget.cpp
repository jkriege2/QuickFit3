#include "qfespimb040configtabwidget.h"
#include "ui_qfespimb040configtabwidget.h"
#include "programoptions.h"

QFESPIMB040ConfigTabWidget* QFESPIMB040ConfigTabWidget::instance=NULL;

QFESPIMB040ConfigTabWidget::QFESPIMB040ConfigTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ConfigTabWidget)
{
    ui->setupUi(this);

    reloadStylesheets(true);

    if (!instance) instance=this;

}

QFESPIMB040ConfigTabWidget::~QFESPIMB040ConfigTabWidget()
{
    delete ui;
}


void QFESPIMB040ConfigTabWidget::loadSettings(QSettings& settings, QString prefix) {
    ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(settings.value(prefix+"style", ProgramOptions::getInstance()->getStyle()).toString(), Qt::MatchExactly ));
    ui->cmbStylesheet->setCurrentIndex(ui->cmbStylesheet->findText(settings.value(prefix+"stylesheet", ProgramOptions::getInstance()->getStylesheet()).toString(), Qt::MatchExactly ));
    emit styleChanged(getStyle(), getStylesheet());
}

QString QFESPIMB040ConfigTabWidget::getStylesheet()
{
    if (instance) return instance->ui->cmbStylesheet->itemData(instance->ui->cmbStylesheet->currentIndex()).toString();
    else return ProgramOptions::getInstance()->getStylesheet();
}

QString QFESPIMB040ConfigTabWidget::getStyle()
{
    if (instance) return instance->ui->cmbStyle->currentText();
    else return ProgramOptions::getInstance()->getStyle();
}


void QFESPIMB040ConfigTabWidget::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"style", ui->cmbStyle->currentText());
    settings.setValue(prefix+"stylesheet", ui->cmbStylesheet->currentText());
}


void QFESPIMB040ConfigTabWidget::on_cmbStylesheet_currentIndexChanged( int index ) {
    QString fn=ui->cmbStylesheet->itemData(index).toString();
    QFile f(fn);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    this->setStyleSheet(qss);

    emit styleChanged(getStyle(), getStylesheet());
}


void QFESPIMB040ConfigTabWidget::on_cmbStyle_currentIndexChanged( const QString & text ) {
    parentWidget()->setStyle(QStyleFactory::create(text));
    parentWidget()->setPalette(parentWidget()->style()->standardPalette());
    emit styleChanged(getStyle(), getStylesheet());
}

void QFESPIMB040ConfigTabWidget::on_cmbStyle_highlighted( const QString & text ) {
    on_cmbStyle_currentIndexChanged(text);
}

void QFESPIMB040ConfigTabWidget::reloadStylesheets(bool forSure)
{
    QString oldStyle=ui->cmbStyle->currentText();
    QString oldStyleSheet=ui->cmbStylesheet->currentText();
    if (forSure || !ui->cmbStyle->hasFocus()) {
        ui->cmbStyle->setUpdatesEnabled(false);
        ui->cmbStyle->clear();
        ui->cmbStyle->addItems(QStyleFactory::keys());
        ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(oldStyle, Qt::MatchContains));
        ui->cmbStyle->setUpdatesEnabled(true);
    }


    // find all available stylesheets
    if (forSure || !ui->cmbStylesheet->hasFocus()) {
        ui->cmbStylesheet->setUpdatesEnabled(false);
        QDir dir(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/spimb040/stylesheets/");
        QStringList filters, sl;
        dir.cd("stylesheets");
        filters.clear();
        filters << "*.qss";
        ui->cmbStylesheet->clear();
        sl=dir.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            ui->cmbStylesheet->addItem(sl[i].remove(".qss", Qt::CaseInsensitive), dir.absoluteFilePath(sl[i]));
        }


        dir=QDir(ProgramOptions::getInstance()->getAssetsDirectory());
        dir.cd("stylesheets");
        sl.clear();
        sl=dir.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            ui->cmbStylesheet->addItem(sl[i].remove(".qss", Qt::CaseInsensitive), dir.absoluteFilePath(sl[i]));
        }

        ui->cmbStylesheet->setCurrentIndex( ui->cmbStylesheet->findText(oldStyleSheet));
        ui->cmbStylesheet->setUpdatesEnabled(true);
    }

    //QTimer::singleShot(120000, this, SLOT(reloadStylesheets())); // rescan directory every 2 minutes
}

