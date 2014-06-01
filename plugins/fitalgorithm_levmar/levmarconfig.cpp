#include "levmarconfig.h"


LevmarConfigDialog::LevmarConfigDialog(QWidget *parent):
    QDialog(parent)
{
    // setup widgets

    QVBoxLayout* main=new QVBoxLayout();
    setLayout(main);


    QGridLayout* grid=new QGridLayout();
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

    l=new QLabel(tr("Damping Parameter <i>&mu;</i>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditMu=new QFDoubleEdit(this);
    neditMu->setRange(0, 1e6);
    neditMu->setSingleStep(0.001);
    neditMu->setDecimals(12);
    l->setBuddy(neditMu);
    grid->addWidget(l, 1, 0);
    grid->addWidget(neditMu, 1, 1);
    grid->addWidget(new QLabel(tr("The damping parameter <i>&mu;</i> of the Levenberg-Marquardt fit algorithm."), this), 1, 2);

    l=new QLabel(tr("Gradient threshold &epsilon;<sub>1</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditEpsilon1=new QFDoubleEdit(this);
    neditEpsilon1->setRange(0, 1e6);
    neditEpsilon1->setSingleStep(0.001);
    neditEpsilon1->setDecimals(12);
    l->setBuddy(neditEpsilon1);
    grid->addWidget(l, 2, 0);
    grid->addWidget(neditEpsilon1, 2, 1);
    grid->addWidget(new QLabel(tr("threshold for the gradient ||J<sup>T</sup>·<b>e</b>||<sub>&infin;</sub>"), this), 2, 2);

    l=new QLabel(tr("Parameter Change threshold &epsilon;<sub>2</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditEpsilon2=new QFDoubleEdit(this);
    neditEpsilon2->setRange(0, 1e6);
    neditEpsilon2->setSingleStep(0.001);
    neditEpsilon2->setDecimals(12);
    l->setBuddy(neditEpsilon2);
    grid->addWidget(l, 3, 0);
    grid->addWidget(neditEpsilon2, 3, 1);
    grid->addWidget(new QLabel(tr("threshold for the change in the parameter vector  ||<b>&delta;p</b>||<sub>2</sub>"), this), 3, 2);

    l=new QLabel(tr("SES change threshold &epsilon;<sub>2</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditEpsilon3=new QFDoubleEdit(this);
    neditEpsilon3->setRange(0, 1e6);
    neditEpsilon3->setSingleStep(0.001);
    neditEpsilon3->setDecimals(12);
    l->setBuddy(neditEpsilon3);
    grid->addWidget(l, 4, 0);
    grid->addWidget(neditEpsilon3, 4, 1);
    grid->addWidget(new QLabel(tr("threshold for the sum of squared errors (SES) ||<b>e</b>||<sub>2</sub> = &Sigma;<sub>i</sub> (x<sub>i</sub> - m(&tau;<sub>i</sub>))<sup>2</sup>"), this), 4, 2);

    l=new QLabel(tr("Gradient Delta ||<b>&delta;p</b>||<sub>2</sub>:"), this);
    l->setAlignment(Qt::AlignRight);
    neditGradDelta=new QFDoubleEdit(this);
    neditGradDelta->setRange(0, 1e6);
    neditGradDelta->setSingleStep(0.001);
    neditGradDelta->setDecimals(12);
    l->setBuddy(neditGradDelta);
    grid->addWidget(l, 5, 0);
    grid->addWidget(neditGradDelta, 5, 1);
    grid->addWidget(new QLabel(tr("difference length (||<b>&delta;p</b>||<sub>2</sub>) when estimating gradients &part;m(<b>p</b>)/&part;<b>p</b> numerically"), this), 5, 2);


    l=new QLabel(tr("Always use Numerical Jaobian:"), this);
    l->setAlignment(Qt::AlignRight);
    chkNumGrad=new QCheckBox("", this);
    l->setBuddy(chkNumGrad);
    grid->addWidget(l, 6, 0);
    grid->addWidget(chkNumGrad, 6, 1);
    grid->addWidget(new QLabel(tr("always use a numerical estimate of the jacobian, even if analytical derivatives are provided by the model function"), this), 6, 2);

    main->addLayout(grid, 10);

    buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    main->addWidget(buttons, 0);



    // set default initial values, defined in header file
    setMu();
    setGradDelta();
    setEpsilon1();
    setEpsilon2();
    setEpsilon3();
    setMaxIterations();
    setNumGrad();
}


void LevmarConfigDialog::setMu(double value, double inc){
    neditMu->setValue(value);
    neditMu->setSingleStep(inc);
}

void LevmarConfigDialog::setGradDelta(double value, double inc){
    neditGradDelta->setValue(value);
    neditGradDelta->setSingleStep(inc);
}

void LevmarConfigDialog::setEpsilon1(double value, double inc){
    neditEpsilon1->setValue(value);
    neditEpsilon1->setSingleStep(inc);
}

void LevmarConfigDialog::setEpsilon2(double value, double inc){
    neditEpsilon2->setValue(value);
    neditEpsilon2->setSingleStep(inc);
}

void LevmarConfigDialog::setEpsilon3(double value, double inc){
    neditEpsilon3->setValue(value);
    neditEpsilon3->setSingleStep(inc);
}

void LevmarConfigDialog::setMaxIterations(int value, int inc){
    spinMaxIterations->setValue(value);
    spinMaxIterations->setSingleStep(inc);
}

void LevmarConfigDialog::setNumGrad(bool value) {
    chkNumGrad->setChecked(value);
}
