/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfprdrtable.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfrdrtable.h"
#include "qfdlgcsvparameters.h"

QFPRDRTable::QFPRDRTable(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFPRDRTable::~QFPRDRTable()
{
    //dtor
}

QFRawDataRecord* QFPRDRTable::createRecord(QFProject* parent) {
    return new QFRDRTable(parent);
}


void QFPRDRTable::registerToMenu(QMenu* menu) {
    QAction* insertRDTableFileAct = new QAction(QIcon(":/table/table_open.png"), tr("Table from File [readonly]"), parentWidget);
    insertRDTableFileAct->setStatusTip(tr("Insert a new table raw data item from a specified file"));
    connect(insertRDTableFileAct, SIGNAL(triggered()), this, SLOT(insertTableFile()));

    QAction* insertRDTableFileEditableAct = new QAction(QIcon(":/table/table_open.png"), tr("Import Table from File [editable]"), parentWidget);
    insertRDTableFileEditableAct->setStatusTip(tr("Insert a new table raw data item from a specified file"));
    connect(insertRDTableFileEditableAct, SIGNAL(triggered()), this, SLOT(insertTableFileEdit()));

    QAction* insertRDTableAct = new QAction(QIcon(":/table/table_insert.png"), tr("Editable Table"), parentWidget);
    insertRDTableAct->setStatusTip(tr("Insert a new table raw data item, which may be edited in QuickFit 3.0 and is not connected to a file."));
    connect(insertRDTableAct, SIGNAL(triggered()), this, SLOT(insertTable()));

    QMenu* m=new QMenu(tr("&Table"), menu);
    m->setIcon(QIcon(":/table/projecttree_tablefile.png"));
    m->addAction(insertRDTableAct);
    m->addAction(insertRDTableFileAct);
    m->addAction(insertRDTableFileEditableAct);
    menu->addMenu(m);
}


void QFPRDRTable::insertTable() {
    if (project) {
        QFRawDataRecord* rec=project->addRawData("table", tr("NEW_TABLE"));
        QFRDRTable* tab=qobject_cast<QFRDRTable*>(rec);
        if (tab) {
            tab->model()->resize(10,10);
        }
    }
}


void QFPRDRTable::insertTableFile(bool editable) {
    if (project) {
        QString selFilter="";
        QStringList filters;
        filters<<tr("Comma Separated Value Files (*.txt *.dat *.csv)");
        filters<<tr("Table XML file (*.qftxml)");
        filters<<tr("All Files (*.*)");
        QStringList files = qfGetOpenFileNames(parentWidget, tr("Select Table File(s) to Import ..."), settings->getCurrentRawDataDir(), filters.join(";;"), &selFilter);
        QStringList list = files;
        if (list.size()>0) {



            QStringList roParams;
            QMap<QString, QVariant> p;
            bool ok=false;
            if (editable) {
                p["CONVERT_READWRITE"]=true;
                roParams<<"CONVERT_READWRITE";
            }
            if (filters.indexOf(selFilter)==1) {
                p["READFILE_FILEFORMAT"]="QFTABLEXML";
                roParams<<"READFILE_FILEFORMAT";
                ok=true;
            } else {
                QFDlgCSVParameters* csvDlg=new QFDlgCSVParameters(parentWidget, settings->getQSettings()->value("table/column_separator", ",").toString(),
                                                                      settings->getQSettings()->value("table/decimal_separator", ".").toString(),
                                                              settings->getQSettings()->value("table/comment_start", "#").toString(),
                                                              settings->getQSettings()->value("table/header_start", "#!").toString());
                csvDlg->setFileContents(list[0]);
                if (csvDlg->exec()==QDialog::Accepted) {
                    p["column_separator"]=QString(csvDlg->get_column_separator());
                    p["decimal_separator"]=QString(csvDlg->get_decimal_separator());
                    p["comment_start"]=QString(csvDlg->get_comment_start());
                    p["header_start"]=QString(csvDlg->get_header_start());
                    p["READFILE_FILEFORMAT"]="CSV";
                    settings->getQSettings()->setValue("table/column_separator", QString(csvDlg->get_column_separator()));
                    settings->getQSettings()->setValue("table/decimal_separator", QString(csvDlg->get_decimal_separator()));
                    settings->getQSettings()->setValue("table/comment_start", QString(csvDlg->get_comment_start()));
                    settings->getQSettings()->setValue("table/header_start", QString(csvDlg->get_header_start()));
                    roParams<<"READFILE_FILEFORMAT"<<"column_separator"<<"decimal_separator"<<"comment_start"<<"header_start";
                    ok=true;

                }
                delete csvDlg;
            }
            if (ok) {
                QStringList::Iterator it = list.begin();
                services->setProgressRange(0, list.size());
                int i=0;
                while(it != list.end()) {
                    i++;
                    services->log_text(tr("importing %2 file '%1' ...\n").arg(*it).arg(p.value("READFILE_FILEFORMAT", "UNKNOWN").toString())); /*.arg(QString(csvDlg->get_column_separator())).arg(QString(csvDlg->get_decimal_separator())).arg(QString(csvDlg->get_comment_start())).arg(QString(csvDlg->get_header_start()))*/
                    QFRawDataRecord* e=project->addRawData("table", QFileInfo(*it).fileName(), QStringList(*it), p, roParams);
                    if (e->error()) {
                        project->deleteRawData(e->getID());
                        QMessageBox::critical(parentWidget, tr("QuickFit 3.0"), tr("Error while importing '%1':\n%2").arg(*it).arg(e->errorDescription()));
                        services->log_error(tr("Error while importing '%1':\n    %2\n").arg(*it).arg(e->errorDescription()));
                    }
                    settings->setCurrentRawDataDir(QFileInfo(*it).dir().absolutePath());
                    ++it;
                    services->setProgress(i);
                    QApplication::processEvents();
                }
            }
            services->setProgress(0);


        }
    }
}

void QFPRDRTable::insertTableFileEdit()
{
    insertTableFile(true);
}

Q_EXPORT_PLUGIN2(qfrdrtable, QFPRDRTable)
