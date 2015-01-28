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

#include "spimb040.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif



QFESPIMB040::QFESPIMB040(QObject* parent):
    QObject(parent)
{
    main=NULL;
}

QFESPIMB040::~QFESPIMB040()
{
    //dtor
}


void QFESPIMB040::deinit() {
    if (settings && main) {
        main->storeSettings(settings);
    }
    if (main) {
        main->close();
        delete main;
        main=NULL;
    }
}

void QFESPIMB040::projectChanged(QFProject* oldProject, QFProject* project) {
}

void QFESPIMB040::initExtension() {
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    actStartPlugin=new QAction(QIcon(":/spimb040_logo.png"), tr("Start B040 SPIM Control"), this);
    actStartPluginNew=new QAction(QIcon(":/spimb040_logo.png"), tr("Start B040 SPIM Control, new optics setup"), this);

    QDir d(services->getConfigFileDirectory());
    // make sure the directory for the config files of this extension exists
    d.mkpath(services->getConfigFileDirectory()+"/plugins/"+getID());

    connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));
    connect(actStartPluginNew, SIGNAL(triggered()), this, SLOT(startPluginNew()));

    QToolBar* exttb=services->getToolbar("extensions");
    //std::cout<<"extensions toolbars: "<<exttb<<std::endl;
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("extensions");
    //std::cout<<"extensions menu: "<<extm<<std::endl;
    if (extm) {
        extm->addAction(actStartPlugin);
        extm->addAction(actStartPluginNew);
    }

    QFPluginServices::getInstance()->appendOrAddHTMLReplacement("FILEFORMATS_LIST", QString("<li><b>%2:</b><ul>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/ext_spimb040_fileformats.html#config_ini\">Acquisition configuration files <tt>configuration.ini</tt></a></li>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/ext_spimb040_fileformats.html#meas_dat\">Measurements files <tt>measured.dat</tt></a></li>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/optsetupfiles.html\">Optics Setup Configuration Files</a></li>\n"
                                                                                            "</ul></li>\n").arg(getID()).arg(getName()));

    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));
}

void QFESPIMB040::loadSettings(ProgramOptions* settings) {
    if (main) main->loadSettings(settings);
}

void QFESPIMB040::storeSettings(ProgramOptions* settings) {
    if (main) main->storeSettings(settings);
}

void QFESPIMB040::startPlugin() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QMessageBox::information(parentWidget, getName(), getDescription());
    if (!main) {
        main=new QFESPIMB040MainWindow2(services, NULL);
        QFPluginServices::getInstance()->log_global_text("\n\n=========================================================\n");
        QFPluginServices::getInstance()->log_global_text("== STARTING SPIM CONTROL PLUGIN!                       ==\n");
        QFPluginServices::getInstance()->log_global_text("=========================================================\n\n\n");

        QFPluginServices::getInstance()->setProjectMode(false, tr("!!!SPIM CONTROL MODE!!!"));
    }
    if (main) {
        if (settings) main->loadSettings(settings);
        main->show();
    }
    QApplication::restoreOverrideCursor();
}



void QFESPIMB040::startPluginNew() {
    deinit();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QMessageBox::information(parentWidget, getName(), getDescription());
    if (!main) {
        main=new QFESPIMB040MainWindow2(services, NULL, true);
        QFPluginServices::getInstance()->log_global_text("\n\n=========================================================\n");
        QFPluginServices::getInstance()->log_global_text("== STARTING SPIM CONTROL PLUGIN!                       ==\n");
        QFPluginServices::getInstance()->log_global_text("=========================================================\n\n\n");

        QFPluginServices::getInstance()->setProjectMode(false, tr("!!!SPIM CONTROL MODE!!!"));
    }
    if (main) {
        if (settings) main->loadSettings(settings);
        main->show();
    }
    QApplication::restoreOverrideCursor();
}






Q_EXPORT_PLUGIN2(spimb040, QFESPIMB040)
