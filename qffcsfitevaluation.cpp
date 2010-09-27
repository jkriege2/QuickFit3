#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"

QFEvaluationItem* createQFFCSFit(QFProject* parent) {
    return new QFFCSFitEvaluation(parent);
}

void insertQFFCSFit(QFProject* project, ProgramOptions* options, QWidget* parent, QtLogFile* logWidget, QStatusBar* statusbar, QProgressBar* progressBar) {
    project->addEvaluation("fcs_fit", "FCS Fit");
}

QList<QPair<QAction*, QFEvaluationItemFactory::QFEvaluationItemUserCreateFunc> > registerQFFCSFitMenu(QWidget* parent, QMenu* menu) {
    QList<QPair<QAction*, QFRawDataRecordFactory::QFRawDataRecordUserCreateFunc> > lst;
    QAction* actFCS=new QAction(QIcon(":/fcs_open.png"), parent->tr("FCS Least Squares Fit"), parent);
    actFCS->setStatusTip(parent->tr("Insert a new FCS least squares fit evaluation"));
    menu->addAction(actFCS);
    lst.append(qMakePair(actFCS, &insertQFFCSFit));
    return lst;
}


QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFEvaluationItem(parent, true, true)//false)
{
    //ctor
}

QFFCSFitEvaluation::~QFFCSFitEvaluation()
{
    //dtor
}


void QFFCSFitEvaluation::intWriteXML(QXmlStreamWriter& w) {
}

void QFFCSFitEvaluation::intReadData(QDomElement* e) {
}


QFEvaluationEditor* QFFCSFitEvaluation::createEditor(int i, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(parent);
};
