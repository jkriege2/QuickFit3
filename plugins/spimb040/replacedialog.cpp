/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  Name: replacedialog.cpp
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

#include "replacedialog.h"


QFESPIMB040ReplaceDialog::QFESPIMB040ReplaceDialog(QWidget *parent)
     : QDialog(parent)
 {
    matchCase=false;
    searchFromStart=true;
    wholeWords=false;
    replaceAll=false;
    askBeforeReplace=true;
    setupUi(this);
 }

void QFESPIMB040ReplaceDialog::setPhrase(QString phr){
    phrase=phr;
    edt_what->setText(phr);
    edt_what->selectAll();
}


void QFESPIMB040ReplaceDialog::on_chk_matchCase_stateChanged(int state){
    matchCase = (state==Qt::Checked);
};

void QFESPIMB040ReplaceDialog::on_chk_searchFromStart_stateChanged(int state){
    searchFromStart = (state==Qt::Checked);
};

void QFESPIMB040ReplaceDialog::on_chk_wholeWords_stateChanged(int state){
    wholeWords = (state==Qt::Checked);
};

void QFESPIMB040ReplaceDialog::on_chk_replaceAll_stateChanged(int state){
    replaceAll = (state==Qt::Checked);
};

void QFESPIMB040ReplaceDialog::on_chk_ask_stateChanged(int state){
    askBeforeReplace = (state==Qt::Checked);
};

void QFESPIMB040ReplaceDialog::on_edt_what_textChanged ( const QString & text ){
    phrase=text;
};

void QFESPIMB040ReplaceDialog::on_edt_replace_textChanged ( const QString & text ){
    replace=text;
};

