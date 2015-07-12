#include "testmain.h"
#include <iostream>
#define N 5

TestMain::TestMain(QWidget* parent):
    QWidget(parent)
{
    QVBoxLayout* lay=new QVBoxLayout(this);
    setLayout(lay);

    JKDoubleEdit* edit[N];

    for (int i=0; i<N; i++) {
        edit[i]=new JKDoubleEdit(this);
        lay->addWidget(edit[i]);
        connect(edit[i], SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
    }
    edit[0]->setValue(1.23456789123456789e3);
    edit[1]->setValue(0.0001);
    edit[2]->setValue(0.000000000100000000);
    edit[3]->setValue(1000000000001.0);
    edit[4]->setValue(1.25);
}

TestMain::~TestMain()
{
    //dtor
}

void TestMain::valueChanged(double value) {
    std::cout<<"value changed: "<<value<<std::endl;
}
