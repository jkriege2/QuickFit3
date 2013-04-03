#include "qfertresultcalculator.h"
#include "ui_qfertresultcalculator.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qfmathparser.h"

QFERTResultCalculator::QFERTResultCalculator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFERTResultCalculator)
{
    ui->setupUi(this);
    ui->cmbFileA->init(QFPluginServices::getInstance()->getCurrentProject(), new QFMatchRDRFunctorSelectAll(), true);
    ui->cmbFileB->init(QFPluginServices::getInstance()->getCurrentProject(), new QFMatchRDRFunctorSelectAll(), true);
    connect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroupA, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultA, SLOT(setRDR(QFRawDataRecord*)));
    connect(ui->cmbResultGroupA, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResultA, SLOT(setEvaluationGroup(QString)));

    ui->cmbResultA->setRDR(ui->cmbFileA->currentRDR());
    ui->cmbResultB->setRDR(ui->cmbFileB->currentRDR());
    ui->cmbResultGroupA->setRDR(ui->cmbFileA->currentRDR());
    ui->cmbResultGroupB->setRDR(ui->cmbFileB->currentRDR());
    ui->cmbResultA->setEvaluationGroup(ui->cmbResultGroupA->currentEvaluationGroup());
    ui->cmbResultB->setEvaluationGroup(ui->cmbResultGroupB->currentEvaluationGroup());

    ui->widEquation->getMathParser()->addVariableDouble("A", 1);
    ui->widEquation->getMathParser()->addVariableDouble("B", 1);
    QStringList words;
    words<<"A"<<"B";
    ui->widEquation->addExpressionDefaultWords(words);
    ui->chkSameFiles->setChecked(ProgramOptions::getConfigValue("QFERTResultCalculator/same_files", true).toBool());
    ui->chkSameResultGroups->setChecked(ProgramOptions::getConfigValue("QFERTResultCalculator/same_result_groups", true).toBool());

    checkBoxesChanged();



    ui->widEquation->setExpression(ProgramOptions::getConfigValue("QFERTResultCalculator/expression", "A/B").toString());
    ui->cmbFileA->setCurrentRDRID(ProgramOptions::getConfigValue("QFERTResultCalculator/rdra", 0).toInt());
    ui->cmbFileB->setCurrentRDRID(ProgramOptions::getConfigValue("QFERTResultCalculator/rdrb", 0).toInt());
    ui->cmbResultGroupA->setCurrentEvaluationGroup(ProgramOptions::getConfigValue("QFERTResultCalculator/egroupa", "").toString());
    ui->cmbResultGroupB->setCurrentEvaluationGroup(ProgramOptions::getConfigValue("QFERTResultCalculator/egroupb", "").toString());
    ui->edtNewName->setText(ProgramOptions::getConfigValue("QFERTResultCalculator/new_name", "calculated_result").toString());
    ui->cmbResultA->setCurrentResult(ProgramOptions::getConfigValue("QFERTResultCalculator/resa", "").toString());
    ui->cmbResultB->setCurrentResult(ProgramOptions::getConfigValue("QFERTResultCalculator/resb", "").toString());
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, pos(), size(), "QFERTResultCalculator/size");

    checkBoxesChanged();
}

QFERTResultCalculator::~QFERTResultCalculator()
{
    ProgramOptions::setConfigValue("QFERTResultCalculator/expression", ui->widEquation->getExpression());

    ProgramOptions::setConfigValue("QFERTResultCalculator/same_files", ui->chkSameFiles->isChecked());
    ProgramOptions::setConfigValue("QFERTResultCalculator/same_result_groups", ui->chkSameResultGroups->isChecked());
    ProgramOptions::setConfigValue("QFERTResultCalculator/expression", ui->widEquation->getExpression());
    ProgramOptions::setConfigValue("QFERTResultCalculator/rdra", ui->cmbFileA->currentRDRID());
    ProgramOptions::setConfigValue("QFERTResultCalculator/rdrb", ui->cmbFileB->currentRDRID());
    ProgramOptions::setConfigValue("QFERTResultCalculator/egroupa", ui->cmbResultGroupA->currentEvaluationGroup());
    ProgramOptions::setConfigValue("QFERTResultCalculator/egroupb", ui->cmbResultGroupB->currentEvaluationGroup());
    ProgramOptions::setConfigValue("QFERTResultCalculator/new_name", ui->edtNewName->text());
    ProgramOptions::setConfigValue("QFERTResultCalculator/resa", ui->cmbResultA->currentResult());
    ProgramOptions::setConfigValue("QFERTResultCalculator/resb", ui->cmbResultB->currentResult());


    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFERTResultCalculator/size");
    delete ui;
}

QString QFERTResultCalculator::getExpression() const
{
    return ui->widEquation->getExpression();
}

QFRawDataRecord *QFERTResultCalculator::getRecA() const
{
    return ui->cmbFileA->currentRDR();
}

QFRawDataRecord *QFERTResultCalculator::getRecB() const
{
    return ui->cmbFileB->currentRDR();
}

QString QFERTResultCalculator::getEvalIDA() const
{
    return ui->cmbResultA->currentEvaluationID();
}

QString QFERTResultCalculator::getResultIDA() const
{
    return ui->cmbResultA->currentResult();
}

QString QFERTResultCalculator::getEvalIDB() const
{
    return ui->cmbResultB->currentEvaluationID();
}

QString QFERTResultCalculator::getResultIDB() const
{
    return ui->cmbResultB->currentResult();
}

QString QFERTResultCalculator::getNewResultName() const
{
    return ui->edtNewName->text();
}

void QFERTResultCalculator::eval()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRawDataRecord* rdrA=getRecA();
    QFRawDataRecord* rdrB=getRecB();
    QFRawDataRecord::evaluationResult resA=rdrA->resultsGet(getEvalIDA(), getResultIDA());
    QFRawDataRecord::evaluationResult resB=rdrB->resultsGet(getEvalIDB(), getResultIDB());

    int sizeA=resA.getVectorMatrixItems();
    int sizeB=resB.getVectorMatrixItems();

    if (resA.isNumberType() && resB.isNumberType()) {
        if (sizeA==0 && sizeB==0) {
            QFMathParser p;
            p.clearFunctions();
            p.clearVariables();
            p.addStandardFunctions();
            p.addStandardVariables();
            p.addVariableDouble("A", resA.getAsDouble());
            p.addVariableDouble("B", resB.getAsDouble());
            QFMathParser::qfmpNode* n=p.parse(getExpression());
            QFMathParser::qfmpResult r=n->evaluate();
            if (!p.hasErrorOccured()) {
                if (r.type==QFMathParser::qfmpDouble) {
                    rdrA->resultsSetNumber(getEvalIDA(), getNewResultName(), r.num);
                } else if (r.type==QFMathParser::qfmpBool) {
                    rdrA->resultsSetBoolean(getEvalIDA(), getNewResultName(), r.boolean);
                } else {
                    rdrA->resultsSetString(getEvalIDA(), getNewResultName(), QFMathParser::resultToString(r));
                }
                rdrA->resultsSetGroup(getEvalIDA(), getNewResultName(), "evaluation results");
            } else {
                QMessageBox::critical(this, tr("results calculator"), tr("error while parsing/evaluating expression: %1").arg(p.getLastError()));
                //ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(p.getLastError()));
            }

        } else if (sizeA>0 && sizeB>0) {
            QFMathParser p;
            p.clearFunctions();
            p.clearVariables();
            p.addStandardFunctions();
            p.addStandardVariables();
            QVector<double> res, rA=resA.getAsDoubleVector(), rB=resB.getAsDoubleVector();
            QVector<bool> resBo;
            QVector<QString> sl;
            bool isBool=false;
            bool isSTring=false;
            QFMathParser::qfmpNode* n=p.parse(getExpression());
            if (!p.hasErrorOccured()) {
                for (int i=0; i<qMin(sizeA, sizeB); i++) {
                    p.addVariableDouble("A", rA.value(i, 0));
                    p.addVariableDouble("B", rB.value(i, 0));
                    QFMathParser::qfmpResult r=n->evaluate();
                    if (!p.hasErrorOccured()) {
                        if (r.type==QFMathParser::qfmpDouble) {
                            res<<r.num;
                        } else if (r.type==QFMathParser::qfmpBool) {
                            resBo<<r.boolean;
                            isBool=true;
                        } else {
                            sl<<r.toString();
                            isSTring=true;
                        }
                    }
                }
                if (isBool) {
                    rdrA->resultsSetBooleanList(getEvalIDA(), getNewResultName(), resBo);
                } else if (isSTring) {
                    rdrA->resultsSetStringList(getEvalIDA(), getNewResultName(), sl);
                } else {
                    rdrA->resultsSetNumberList(getEvalIDA(), getNewResultName(), res);
                }
                rdrA->resultsSetGroup(getEvalIDA(), getNewResultName(), "evaluation results");
            }
            if (n) delete n;
            if (p.hasErrorOccured()) {
                    QMessageBox::critical(this, tr("results calculator"), tr("error while parsing/evaluating expression:\n    %1").arg(p.getLastErrors().join("\n    ")));
                //ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(E.what()));
            }

        }
    }
    QApplication::restoreOverrideCursor();
}

void QFERTResultCalculator::checkBoxesChanged()
{
    ui->cmbFileB->setEnabled(!ui->chkSameFiles->isChecked());
    ui->cmbResultGroupB->setEnabled(!ui->chkSameResultGroups->isChecked());


    disconnect(ui->cmbFileB, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroupB, SLOT(setRDR(QFRawDataRecord*)));
    disconnect(ui->cmbFileB, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultB, SLOT(setRDR(QFRawDataRecord*)));
    disconnect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroupB, SLOT(setRDR(QFRawDataRecord*)));
    disconnect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultB, SLOT(setRDR(QFRawDataRecord*)));
    disconnect(ui->cmbResultGroupB, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResultB, SLOT(setEvaluationGroup(QString)));
    disconnect(ui->cmbResultGroupA, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResultB, SLOT(setEvaluationGroup(QString)));

    if (ui->chkSameFiles->isChecked()) {
        connect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroupB, SLOT(setRDR(QFRawDataRecord*)));
        connect(ui->cmbFileA, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultB, SLOT(setRDR(QFRawDataRecord*)));
        ui->cmbResultGroupB->setRDR(ui->cmbFileA->currentRDR());
        ui->cmbResultB->setRDR(ui->cmbFileA->currentRDR());
    } else {
        connect(ui->cmbFileB, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultGroupB, SLOT(setRDR(QFRawDataRecord*)));
        connect(ui->cmbFileB, SIGNAL(currentRDRChanged(QFRawDataRecord*)), ui->cmbResultB, SLOT(setRDR(QFRawDataRecord*)));
        ui->cmbResultGroupB->setRDR(ui->cmbFileB->currentRDR());
        ui->cmbResultB->setRDR(ui->cmbFileB->currentRDR());
    }
    if (ui->chkSameResultGroups->isChecked()) {
        connect(ui->cmbResultGroupA, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResultB, SLOT(setEvaluationGroup(QString)));
        ui->cmbResultB->setEvaluationGroup(ui->cmbResultGroupA->currentEvaluationGroup());
    } else {
        connect(ui->cmbResultGroupB, SIGNAL(currentEvaluationGroupChanged(QString)), ui->cmbResultB, SLOT(setEvaluationGroup(QString)));
        ui->cmbResultB->setEvaluationGroup(ui->cmbResultGroupB->currentEvaluationGroup());
    }

}

void QFERTResultCalculator::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("qfe_resultstools", "qfe_resultstools.html#calctool");
}

void QFERTResultCalculator::on_cmbResultA_currentResultChanged(QString result, QString evalID)
{
    QFRawDataRecord* rdrA=getRecA();
    if(rdrA) {
        QFRawDataRecord::evaluationResult resA=rdrA->resultsGet(evalID, result);
        ui->labPropertiesA->setText(QString("type: %1  items: %2").arg(QFRawDataRecord::evaluationResultType2String(resA.type)).arg(resA.getVectorMatrixItems()));
    } else {
        ui->labPropertiesA->setText("");
    }
}

void QFERTResultCalculator::on_cmbResultB_currentResultChanged(QString result, QString evalID)
{
    QFRawDataRecord* rdrA=getRecB();
    if (rdrA) {
        QFRawDataRecord::evaluationResult resA=rdrA->resultsGet(evalID, result);
        ui->labPropertiesB->setText(QString("type: %1  items: %2").arg(QFRawDataRecord::evaluationResultType2String(resA.type)).arg(resA.getVectorMatrixItems()));
    } else {
        ui->labPropertiesB->setText("");
    }
}

