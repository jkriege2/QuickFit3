/*
  Name: finddialog.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QCheckBox>

#include "finddialog.h"


QFEHelpEditorFindDialog::QFEHelpEditorFindDialog(QWidget *parent)
     : QDialog(parent)
{
    matchCase=false;
    searchFromStart=true;
    wholeWords=false;
    setupUi(this);
}

QFEHelpEditorFindDialog::~QFEHelpEditorFindDialog()
{

}

void QFEHelpEditorFindDialog::setPhrase(QString phr){
    phrase=phr;
    edt_what->setText(phr);
    edt_what->selectAll();
}

void QFEHelpEditorFindDialog::on_chk_matchCase_stateChanged(int state){
    matchCase = (state==Qt::Checked);
};

void QFEHelpEditorFindDialog::on_chk_searchFromStart_stateChanged(int state){
    searchFromStart = (state==Qt::Checked);
};

void QFEHelpEditorFindDialog::on_chk_wholeWords_stateChanged(int state){
    wholeWords = (state==Qt::Checked);
};

void QFEHelpEditorFindDialog::on_edt_what_textChanged ( const QString & text ){
    phrase=text;
};
