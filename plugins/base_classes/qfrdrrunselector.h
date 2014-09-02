/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef QFRDRRunSelector_H
#define QFRDRRunSelector_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include <QToolButton>
#include "qfrdrrunselection.h"
#include "qfrawdatarecord.h"
#include "qfrdrfcsdatainterface.h"


class QFRDRRunSelector : public QWidget
{
        Q_OBJECT
    public:
        explicit QFRDRRunSelector(QWidget *parent = 0);
        virtual  ~QFRDRRunSelector();
        void setRDR(QFRawDataRecord* record);

        int getCurrentRun() const;
    signals:
        void currentRunChanged(int run);
    public slots:
        void setCurrentRun(int run);


    protected:
        QFRawDataRecord* record;
        QFRDRRunSelectionsInterface* rrRecord;
        QFRDRFCSDataInterface* rfcs;

        int currentRun;
        int runmax;
        int runmin;

        QHBoxLayout* gl;
        QLabel* labInfo;

        QSpinBox* spinRun;
        QLabel* labRun;

        QToolButton* btnEditExclude;
        QAction* actEditExclude;
        QToolButton* btnEditInclude;
        QAction* actEditInclude;

        
};

#endif // QFRDRRunSelector_H
