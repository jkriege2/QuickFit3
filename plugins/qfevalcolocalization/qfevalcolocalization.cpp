/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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


#include "qfevalcolocalization.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "qfevalcolocalization_item.h"

QFEValColocalization::QFEValColocalization(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFEValColocalization::~QFEValColocalization()
{
    //dtor
}

QFEvaluationItem* QFEValColocalization::createRecord(QFProject* parent) {
    return new QFEValColocalizationItem(parent);
}


void QFEValColocalization::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFEValColocalization::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(eval_colocalization, QFEValColocalization)
