#include "dlgsetrdrpropertybyexpressioneditor.h"
#include "ui_dlgsetrdrpropertybyexpressioneditor.h"
#include "dlgsetrdrpropertybyexpression.h"

DlgSetRDRPropertyByExpressionEditor::DlgSetRDRPropertyByExpressionEditor(DlgSetRDRPropertyByExpression *par, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DlgSetRDRPropertyByExpressionEditor)
{
    ui->setupUi(this);
    project=NULL;
    this->par=par;
}

DlgSetRDRPropertyByExpressionEditor::~DlgSetRDRPropertyByExpressionEditor()
{
    delete ui;
}

QString DlgSetRDRPropertyByExpressionEditor::getExpression() const
{
    return ui->edtExpression->getExpression();
}

int DlgSetRDRPropertyByExpressionEditor::getType() const
{
    return ui->cmbOutput->currentIndex();
}

QString DlgSetRDRPropertyByExpressionEditor::getPropertyName() const
{
    return ui->cmbPropertyName->currentText();
}

void DlgSetRDRPropertyByExpressionEditor::setExpression(const QString &exp)
{
    ui->edtExpression->setExpression(exp);
}

void DlgSetRDRPropertyByExpressionEditor::setType(int t)
{
    ui->cmbOutput->setCurrentIndex(t);
}

void DlgSetRDRPropertyByExpressionEditor::setPropertyName(const QString &pn)
{
    ui->cmbPropertyName->setEditText(pn);
}


void DlgSetRDRPropertyByExpressionEditor::on_btnDelete_clicked()
{
    emit removeMe(this);
}

void DlgSetRDRPropertyByExpressionEditor::emitWidgetContentsChanged()
{
    emit widgetContentsChanged();
}

void DlgSetRDRPropertyByExpressionEditor::setProject(QFProject *project)
{
    this->project=project;

    if (project) {
        QString n=ui->cmbPropertyName->currentText();
        ui->cmbPropertyName->clear();
        ui->cmbPropertyName->addItems(project->getAllPropertyNames());
        ui->cmbPropertyName->setEditText(n);
    }
}

void DlgSetRDRPropertyByExpressionEditor::setID(const QString &id)
{
    ui->labID->setText(id);
}

void DlgSetRDRPropertyByExpressionEditor::updateTest()
{
    ui->cmbPropertyName->setEnabled(ui->cmbOutput->currentIndex()==4);
    QFRawDataRecord* rdr=par->currentRDR();
    QString error;
    if (rdr) {
        QVariant txt=par->getResult(getExpression(), rdr, NULL, &error);
        ui->labTest->setText(txt.toString());//QString("<tt>%1</tt>").arg(getResult(rdr)));
        if (error.isEmpty()) ui->labError->setText(QString("<font color=\"darkgreen\"><b>OK</b></font>"));
            else ui->labError->setText(QString("<font color=\"red\"><b>ERROR: %1</b></font>").arg(error));

    }
}

bool DlgSetRDRPropertyByExpressionEditor::widgetChanged(QSettings *settings, const QString &prefix)
{
    return (settings->value(prefix+"expression").toString()!=getExpression())||(getType() != settings->value(prefix+"type").toInt()) || (getPropertyName()!= settings->value(prefix+"property").toString());
}

bool DlgSetRDRPropertyByExpressionEditor::saveWidget(QSettings *settings, const QString &prefix)
{
    settings->setValue(prefix+"expression", getExpression());
    settings->setValue(prefix+"type", getType());
    settings->setValue(prefix+"property", getPropertyName());
    return true;
}

bool DlgSetRDRPropertyByExpressionEditor::loadWidget(QSettings *settings, const QString &prefix)
{
    setExpression(settings->value(prefix+"expression").toString());
    setType(settings->value(prefix+"type").toInt());
    setPropertyName(settings->value(prefix+"property").toString());
    return true;
}
