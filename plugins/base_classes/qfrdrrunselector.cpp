#include "qfrdrrunselector.h"

QFRDRRunSelector::QFRDRRunSelector(QWidget *parent) :
    QWidget(parent)
{

    int row=0;
    spinRun=0;
    labRun=0;
    gl=new QGridLayout(this);
    gl->setContentsMargins(2,2,2,2);
    gl->setHorizontalSpacing(2);
    gl->setVerticalSpacing(2);
    gl->setColumnStretch(1,1);
    setLayout(gl);
    labRun=new QLabel(this);
    labRun->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    QLabel* l;
    gl->addWidget(l=new QLabel(tr("run:"), this), row, 0);
    spinRun=new QSpinBox(this);
    spinRun->setSpecialValueText(tr("average"));
    l->setBuddy(spinRun);
    spinRun->setVisible(false);
    row++;


    /*btnEditSlected=new QToolButton(this);
    btnEditSlected->setDefaultAction(actEditSelection);
    gl->addWidget(btnEditSlected, row, 2);
    row++;
*/
    setRDR(NULL);
}

QFRDRRunSelector::~QFRDRRunSelector()
{
}

void QFRDRRunSelector::setRDR(QFRawDataRecord *record)
{
    this->record=record;
    rrRecord=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
    rfcs=dynamic_cast<QFRDRFCSDataInterface*>(record);
    currentRun=-1;
    runmax=0;
    runmin=0;
    if (rrRecord) {
        runmax=rrRecord->leaveoutGetRunCount();
        runmin=0;
    }
    if (rfcs) {
        runmax=rfcs->getCorrelationRuns()-1;
        runmin=-1;
    }

    setCurrentRun(currentRun);
}

int QFRDRRunSelector::getCurrentRun() const
{
    return currentRun;
}



void QFRDRRunSelector::setCurrentRun(int runIn)
{
    int run=runIn;
    if (run<runmin) run=runmin;
    if (run>runmax) run=runmax;
    if (run==currentRun) return;
    currentRun=run;

    disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));
    spinRun->setRange(runmin, runmax);
    if (currentRun!=spinRun->value()) spinRun->setValue(currentRun);
    spinRun->setSuffix(QString(" / %1").arg(runmax));
    connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));

    emit currentRunChanged(run);
}

