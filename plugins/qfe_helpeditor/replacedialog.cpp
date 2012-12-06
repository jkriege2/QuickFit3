/*
  Name: replacedialog.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/
#include <QtGui>
#include <QCheckBox>

#include "replacedialog.h"


ReplaceDialog::ReplaceDialog(QWidget *parent)
     : QDialog(parent)
 {
    matchCase=false;
    searchFromStart=true;
    wholeWords=false;
    replaceAll=false;
    askBeforeReplace=true;
    setupUi(this);
 }

void ReplaceDialog::setPhrase(QString phr){
    phrase=phr;
    edt_what->setText(phr);
    edt_what->selectAll();
}


void ReplaceDialog::on_chk_matchCase_stateChanged(int state){
    matchCase = (state==Qt::Checked);
};

void ReplaceDialog::on_chk_searchFromStart_stateChanged(int state){
    searchFromStart = (state==Qt::Checked);
};

void ReplaceDialog::on_chk_wholeWords_stateChanged(int state){
    wholeWords = (state==Qt::Checked);
};

void ReplaceDialog::on_chk_replaceAll_stateChanged(int state){
    replaceAll = (state==Qt::Checked);
};

void ReplaceDialog::on_chk_ask_stateChanged(int state){
    askBeforeReplace = (state==Qt::Checked);
};

void ReplaceDialog::on_edt_what_textChanged ( const QString & text ){
    phrase=text;
};

void ReplaceDialog::on_edt_replace_textChanged ( const QString & text ){
    replace=text;
};

