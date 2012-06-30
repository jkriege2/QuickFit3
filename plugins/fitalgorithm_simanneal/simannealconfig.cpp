#include "simannealconfig.h"


SimannealConfigDialog::SimannealConfigDialog(QWidget *parent):
    QDialog(parent)
{
    // setup widgets

    QVBoxLayout* main=new QVBoxLayout(this);
    setLayout(main);


    QGridLayout* grid=new QGridLayout(this);
    QLabel* l;

    l=new QLabel(tr("Maximum Iterations:"), this);
    l->setAlignment(Qt::AlignRight);
    spinMaxIterations=new QSpinBox(this);
    spinMaxIterations->setRange(100,1000000);
    spinMaxIterations->setSingleStep(100);
    l->setBuddy(spinMaxIterations);
    grid->addWidget(l, 0, 0);
    grid->addWidget(spinMaxIterations, 0, 1);
    grid->addWidget(new QLabel(tr("The maximum number of iterations that the Levenberg-Marquardt algorithm will go through before it halts."), this), 0, 2);

    l=new QLabel(tr("Initial Temperature T<sub>0</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditT0=new QFDoubleEdit(this);
    neditT0->setRange(0, 1e6);
    neditT0->setSingleStep(0.001);
    neditT0->setDecimals(12);
    l->setBuddy(neditT0);
    grid->addWidget(l, 1, 0);
    grid->addWidget(neditT0, 1, 1);
    grid->addWidget(new QLabel(tr("The initial temperature is set to be T<sub>start</sub>=T<sub>0</sub>&middot;&chi;<sup>2</sup>(<b>p</b><sub>start</sub>), depending on the initial parameters <b>p</b><sub>start</sub>."), this), 1, 2);

    l=new QLabel(tr("Temperature Decrease Rate r<sub>T</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditRT=new QFDoubleEdit(this);
    neditRT->setRange(0, 1e6);
    neditRT->setSingleStep(0.001);
    neditRT->setDecimals(12);
    l->setBuddy(neditRT);
    grid->addWidget(l, 2, 0);
    grid->addWidget(neditRT, 2, 1);
    grid->addWidget(new QLabel(tr("constant for temperature decrease: T<sub>new</sub> = r<sub>T</sub> &middot; T<sub>old</sub>"), this), 2, 2);

    l=new QLabel(tr("convergence parameter f<sub>max</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditFMax=new QFDoubleEdit(this);
    neditFMax->setRange(0, 1e6);
    neditFMax->setSingleStep(0.001);
    neditFMax->setDecimals(12);
    l->setBuddy(neditFMax);
    grid->addWidget(l, 3, 0);
    grid->addWidget(neditFMax, 3, 1);
    grid->addWidget(new QLabel(tr("if |&chi;<sup>2</sup><sub>opt, i-1</sub> - &chi;<sup>2</sup><sub>opt, i</sub>| &lt; f<sub>max</sub> the algorithm is said to have converged."), this), 3, 2);

    l=new QLabel(tr("step update constant c:"), this);
    l->setAlignment(Qt::AlignRight);
    neditC=new QFDoubleEdit(this);
    neditC->setRange(0, 1e6);
    neditC->setSingleStep(0.001);
    neditC->setDecimals(12);
    l->setBuddy(neditC);
    grid->addWidget(l, 4, 0);
    grid->addWidget(neditC, 4, 1);
    grid->addWidget(new QLabel(tr("constant for the update of the step vectors"), this), 4, 2);

    l=new QLabel(tr("Step Size Variations N<sub>S</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    spinNS=new QSpinBox(this);
    spinNS->setRange(1,100000000);
    spinNS->setSingleStep(100);
    l->setBuddy(spinNS);
    grid->addWidget(l, 5, 0);
    grid->addWidget(spinNS, 5, 1);
    grid->addWidget(new QLabel(tr("The number of variations of the step length before convergence check."), this), 5, 2);

    l=new QLabel(tr("Monte-Carlo Steps per Step Size N<sub>T</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    spinNT=new QSpinBox(this);
    spinNT->setRange(1,100000000);
    spinNT->setSingleStep(100);
    l->setBuddy(spinNT);
    grid->addWidget(l, 6, 0);
    grid->addWidget(spinNT, 6, 1);
    grid->addWidget(new QLabel(tr("Number of Monte-Carlo steps with each step size configuration."), this), 6, 2);

    l=new QLabel(tr("Maximum Iterations:"), this);
    l->setAlignment(Qt::AlignRight);
    spinNEpsilon=new QSpinBox(this);
    spinNEpsilon->setRange(1,100000000);
    spinNEpsilon->setSingleStep(100);
    l->setBuddy(spinNEpsilon);
    grid->addWidget(l, 7, 0);
    grid->addWidget(spinNEpsilon, 7, 1);
    grid->addWidget(new QLabel(tr("The Algorithm looks at the past N<sub>epsilon</sub> values of &chi;<sup>2</sup>(<b>p</b>) to check convergence."), this), 7, 2);



    main->addLayout(grid, 10);

    buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    main->addWidget(buttons, 0);



    // set default initial values, defined in header file
    setFMax();
    setT0();
    setC();
    setNS();
    setNT();
    setNEpsilon();
    setRT();
    setMaxIterations();
}


void SimannealConfigDialog::setFMax(double value, double inc){
    neditFMax->setValue(value);
    neditFMax->setSingleStep(inc);
}

void SimannealConfigDialog::setT0(double value, double inc){
    neditT0->setValue(value);
    neditT0->setSingleStep(inc);
}

void SimannealConfigDialog::setRT(double value, double inc){
    neditRT->setValue(value);
    neditRT->setSingleStep(inc);
}

void SimannealConfigDialog::setC(double value, double inc){
    neditC->setValue(value);
    neditC->setSingleStep(inc);
}

void SimannealConfigDialog::setNS(int value, int inc){
    spinNS->setValue(value);
    spinNS->setSingleStep(inc);
}

void SimannealConfigDialog::setNT(int value, int inc){
    spinNT->setValue(value);
    spinNT->setSingleStep(inc);
}

void SimannealConfigDialog::setNEpsilon(int value, int inc){
    spinNEpsilon->setValue(value);
    spinNEpsilon->setSingleStep(inc);
}

void SimannealConfigDialog::setMaxIterations(int value, int inc){
    spinMaxIterations->setValue(value);
    spinMaxIterations->setSingleStep(inc);
}
