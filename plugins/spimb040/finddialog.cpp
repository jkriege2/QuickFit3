/*
  Name: finddialog.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/
#include <QtGui>
#include <QCheckBox>

#include "finddialog.h"


FindDialog::FindDialog(QWidget *parent)
     : QDialog(parent)
{
    matchCase=false;
    searchFromStart=true;
    wholeWords=false;
    setupUi(this);
}

void FindDialog::setPhrase(QString phr){
    phrase=phr;
    edt_what->setText(phr);
    edt_what->selectAll();
}

void FindDialog::on_chk_matchCase_stateChanged(int state){
    matchCase = (state==Qt::Checked);
};

void FindDialog::on_chk_searchFromStart_stateChanged(int state){
    searchFromStart = (state==Qt::Checked);
};

void FindDialog::on_chk_wholeWords_stateChanged(int state){
    wholeWords = (state==Qt::Checked);
};

void FindDialog::on_edt_what_textChanged ( const QString & text ){
    phrase=text;
};
