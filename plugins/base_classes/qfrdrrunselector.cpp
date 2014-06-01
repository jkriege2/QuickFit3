#include "qfrdrrunselector.h"

QFRDRRunSelector::QFRDRRunSelector(QWidget *parent) :
    QWidget(parent)
{

    spinRun=0;
    labRun=0;
    gl=new QHBoxLayout();
    setLayout(gl);
    gl->setContentsMargins(0,0,0,0);
    gl->setSpacing(2);
    labRun=new QLabel(this);
    labRun->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    spinRun=new QSpinBox(this);
    spinRun->setSpecialValueText(tr("average"));
    spinRun->setVisible(true);
    gl->addWidget(spinRun);
    gl->addWidget(labRun);



    /*btnEditSlected=new QToolButton(this);
    btnEditSlected->setDefaultAction(actEditSelection);
    gl->addWidget(btnEditSlected, row, 2);
    row++;
*/
    setRDR(NULL);
    //spinRun->setUpdatesEnabled(true);
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
    runmax=-1;
    runmin=-1;
    if (rrRecord) {
        runmax=rrRecord->leaveoutGetRunCount();
        runmin=0;
    }
    if (rfcs) {
        runmax=rfcs->getCorrelationRuns()-1;
        runmin=-1;
    }
    //qDebug()<<"run.setRDR: "<<record<<rrRecord<<rfcs<<runmin<<runmax;

    setCurrentRun(currentRun);
}

int QFRDRRunSelector::getCurrentRun() const
{
    return currentRun;
}



void QFRDRRunSelector::setCurrentRun(int runIn)
{
    //qDebug()<<"run: "<<runIn<<runmin<<runmax;
    int run=runIn;
    if (run<runmin) run=runmin;
    if (run>runmax) run=runmax;
    currentRun=run;

    disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));
    spinRun->setRange(runmin, runmax);
    if (currentRun!=spinRun->value()) spinRun->setValue(currentRun);
    spinRun->setSuffix(QString(" / %1").arg(runmax));
    connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(setCurrentRun(int)));
    gl->update();

    emit currentRunChanged(run);
    //setUpdatesEnabled(true);
    //qDebug()<<"run: "<<spinRun->value()<<spinRun->minimum()<<spinRun->maximum();
}

