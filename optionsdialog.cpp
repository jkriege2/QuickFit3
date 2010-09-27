#include "optionsdialog.h"
#include <iostream>
OptionsDialog::OptionsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
}

OptionsDialog::~OptionsDialog()
{
    //dtor
}

void OptionsDialog::on_cmbStylesheet_currentIndexChanged( const QString & text ) {
    QString fn=QString(QCoreApplication::applicationDirPath()+"/stylesheets/%1.qss").arg(text);
    QFile f(fn);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    //std::cout<<qss.toStdString()<<std::endl;
    this->setStyleSheet(qss);
}

void OptionsDialog::on_cmbStylesheet_highlighted( const QString & text ) {
    on_cmbStylesheet_currentIndexChanged(text);
}

void OptionsDialog::on_cmbStyle_currentIndexChanged( const QString & text ) {
//        QStyle* s=QStyleFactory::create(text);
//        if (s!=NULL) this->setStyle(s);
}

void OptionsDialog::on_cmbStyle_highlighted( const QString & text ) {
    on_cmbStyle_currentIndexChanged(text);
}


