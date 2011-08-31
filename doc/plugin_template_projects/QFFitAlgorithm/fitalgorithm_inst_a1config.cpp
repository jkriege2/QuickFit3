#include "fitalgorithm_inst_a1config.h"


QFFitAlgorithmInst_A1ConfigDialog::QFFitAlgorithmInst_A1ConfigDialog(QWidget *parent):
    QDialog(parent)
{
    // setup widgets

    QVBoxLayout* main=new QVBoxLayout(this);
    setLayout(main);


    QGridLayout* grid=new QGridLayout(this);
    QLabel* l;

    l=new QLabel(tr("param:"), this);
    l->setAlignment(Qt::AlignRight);
    spinParam=new QDoubleSpinBox(this);
    spinParam->setRange(0,100);
    spinParam->setSingleStep(1);
    l->setBuddy(spinParam);
    grid->addWidget(l, 0, 0);
    grid->addWidget(spinParam, 0, 1);
    grid->addWidget(new QLabel(tr("Doc for param"), this), 0, 2);

    main->addLayout(grid, 10);

    buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    main->addWidget(buttons, 0);



    // set default initial values, defined in header file
    setParamName();
    
}


void QFFitAlgorithmInst_A1ConfigDialog::setParamName(double value, double inc){
    spinParam->setValue(value);
    spinParam->setSingleStep(inc);
}
