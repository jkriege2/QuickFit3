#include "ffmexpdescriptionwidget.h"

FFMExpDescriptionWidget::FFMExpDescriptionWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout(this);
    //l1, frames
    fr1 = new QFrame(this);
    frl1 = new QGridLayout(fr1);
    fr1->setFrameShape(QFrame::Box);
    fr2 = new QFrame(this);
    frl2 = new QGridLayout(fr2);
    fr2->setFrameShape(QFrame::Box);
    fr3 = new QFrame(this);
    frl3 = new QGridLayout(fr3);
    fr3->setFrameShape(QFrame::Box);
    fr4 = new QFrame(this);
    frl4 = new QGridLayout(fr4);
    fr4->setFrameShape(QFrame::Box);
    layout->addWidget(fr1, 0, 0);
    layout->addWidget(fr2, 1, 0);
    layout->addWidget(fr3, 2, 0);
    layout->addWidget(fr4, 3, 0);

    Number = new QLineEdit();
    frl1->addWidget(new QLabel(tr("<b>ID/Number:</b>")), 0, 0, Qt::AlignLeft);
    frl1->addWidget(Number, 0, 1, 1, 1, Qt::AlignRight);
    Number->setMinimumWidth(250);
    Number->setMaximumWidth(250);
    Title = new QLineEdit();
    frl1->addWidget(new QLabel(tr("<b>Title:</b>")),1,0, Qt::AlignLeft);
    frl1->addWidget(Title, 1, 1, 1, 1, Qt::AlignRight);
    Title->setMinimumWidth(250);
    Title->setMaximumWidth(250);
    //l1, frame 2
    Person =  new QLineEdit();
    frl2->addWidget(new QLabel(tr("<b>Experimenter:</b>")), 0, 0, Qt::AlignLeft);
    frl2->addWidget(Person, 0, 2, 1, 1, Qt::AlignRight);
    Person->setMinimumWidth(250);
    Person->setMaximumWidth(250);
    Page =  new QLineEdit();
    frl2->addWidget(new QLabel(tr("<b>LabBook Page:</b>")),1,0, Qt::AlignLeft);
    frl2->addWidget(Page, 1, 2, 1, 1, Qt::AlignRight);
    Page->setMinimumWidth(250);
    Page->setMaximumWidth(250);
    //l1, frame 3
    Sample = new QTextEdit();
    frl3->addWidget(new QLabel(tr("<b>Sample:</b>")), 0, 0, Qt::AlignLeft);
    frl3->addWidget(Sample, 0, 2, 1, 1, Qt::AlignRight);
    Sample->setMinimumWidth(250);
    Sample->setMaximumWidth(250);
    //l1, frame 4
    Description = new QTextEdit();
    frl4->addWidget(new QLabel(tr("<b>Description:</b>")), 0, 0, Qt::AlignLeft);
    frl4->addWidget(Description, 0, 2, 1, 1, Qt::AlignRight);
    Description->setMinimumWidth(250);
    Description->setMaximumWidth(250);
}

FFMExpDescriptionWidget::~FFMExpDescriptionWidget()
{
}
