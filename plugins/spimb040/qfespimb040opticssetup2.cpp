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

#include "qfespimb040opticssetup2.h"
#include "objectives.h"
#include "ui_qfespimb040opticssetup2.h"
#include "objectives.h"
#include "filters.h"
#include "jkiniparser2.h"
#include "qfespimb040shortcutconfigdialog.h"
#include "qfespimb040lightpathsavedialog.h"
#include "qfenhancedtabwidget.h"



template <class T, class TSet>
void loadValueSettingsForAllInMap(T map_begin, T map_end, TSet& settings, const QString& prefix) {
    for (T it=map_begin; it!=map_end; it++) {
        it.value()->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <class T, class TSet>
void saveValueSettingsForAllInMap(T map_begin, T map_end, TSet& settings, const QString& prefix) {
    for (T it=map_begin; it!=map_end; it++) {
        it.value()->saveSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void loadValueSettingsForAllInMap( QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_begin, QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator it=map_begin; it!=map_end; it++) {
        settings.setValue(prefix+it.key()+"/activated", it.value().first->isChecked());
        it.value().second->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap( QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_begin, QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().first->setChecked(settings.value(prefix+it.key()+"/activated", it.value().first->isChecked()).toBool());
        it.value().second->saveSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void loadValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->saveSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void loadValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_end, QSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->storeSettings(settings, prefix+it.key()+"/");
    }
}







template <>
void loadValueSettingsForAllInMap( QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_begin, QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator it=map_begin; it!=map_end; it++) {
        settings.setValue(prefix+it.key()+"/activated", it.value().first->isChecked());
        it.value().second->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap( QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_begin, QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().first->setChecked(settings.value(prefix+it.key()+"/activated", it.value().first->isChecked()).toBool());
        it.value().second->saveSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void loadValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::LightsourceWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->saveSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void loadValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <>
void saveValueSettingsForAllInMap(QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_begin, QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator map_end, QFManyFilesSettings& settings, const QString& prefix) {
    for (QMap<QString, QFESPIMB040OpticsSetup2::CameraWidgets>::const_iterator it=map_begin; it!=map_end; it++) {
        it.value().config->storeSettings(settings, prefix+it.key()+"/");
    }
}







template <class T, class TSet>
void loadValueSettingsForAllInMap(const T& map, TSet& settings, const QString& prefix) {
    loadValueSettingsForAllInMap(map.constBegin(), map.constEnd(), settings, prefix);
}
template <class T, class TSet>
void saveValueSettingsForAllInMap(const T& map, TSet& settings, const QString& prefix) {
    saveValueSettingsForAllInMap(map.constBegin(), map.constEnd(), settings, prefix);
}



























QFESPIMB040OpticsSetup2::QFESPIMB040OpticsSetup2(QWidget* pluginMainWidget, QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices) :
    QFESPIMB040OpticsSetupBase(parent),
    ui(new Ui::QFESPIMB040OpticsSetup2)
{
    lightpathConfigModel=new QFTableModel(this);


    setting_lightpath=false;
    m_pluginServices=pluginServices;
    m_pluginMainWidget=pluginMainWidget;
    m_log=log;
    shutterTransmissionID="";
    shutterLaser2ID="";
    shutterLaser1ID="";
    objProjection="";
    objDetection="";
    filtercDetection="";
    lastOptSetup="";

    ui->setupUi(this);



    //ui->tbMainShutter->setDefaultAction(ui->shutterMainIllumination->getStateAction());
    ui->cmbLightpathConfig->init(m_pluginServices->getConfigFileDirectory()+"/plugins/ext_spimb040/", "lpc");
    ui->cmbLightpathConfig->setIcon(QIcon(":/spimb040/lightpath.png"));
    ui->btnLockFiltersEtc->setChecked(true);
    connect(ui->cmbLightpathConfig, SIGNAL(configsChanged(QFESPIMB040OpticsSetupItems)), this, SLOT(configsChanged(QFESPIMB040OpticsSetupItems)));
    connect(ui->cmbLightpathConfig, SIGNAL(currentConfigChanged(QString)), this, SLOT(userChangedLightpath(QString)));

    actMainShutter=new QAction(QIcon(":/spimb040/shutter_opened.png"), tr("main shutter"), this);
    actMainShutter->setCheckable(true);
    actMainShutter->setChecked(true);
    connect(actMainShutter, SIGNAL(triggered(bool)), this, SLOT(mainShutterToggled(bool)));
    ui->tbMainShutter->setDefaultAction(actMainShutter);
    connect(addShortCut("mainshutter_toggle", "main shutter: toggle"), SIGNAL(activated()), actMainShutter, SLOT(toggle()));
    connect(addShortCut("mainshutter_on", "main shutter: on"), SIGNAL(activated()), this, SLOT(mainShutterOn()));
    connect(addShortCut("mainshutter_off", "main shutter: off"), SIGNAL(activated()), this, SLOT(mainShutterOff()));

    // create shortcuts


}

QFESPIMB040OpticsSetup2::~QFESPIMB040OpticsSetup2()
{
    delete ui;
}

void QFESPIMB040OpticsSetup2::loadOptSetup(const QString &filename)
{
    if (QFile::exists(filename)) {
        lastOptSetup=filename;
        m_log->log_text(tr("loading optSetup-file '%1' ... \n").arg(filename));
        /*QFManyFilesSettings settings;
        QSettings setMain(filename, QSettings::IniFormat);
        settings.addSettings(&setMain, false, true);
        QStringList sgroups=settings.childGroups();*/
        jkINIParser2 settings;
        try {
           settings.readFile(filename.toStdString()); // read in an INI file
           QStringList sgroups;
           for (int i=0; i<settings.getGroupCount(); i++) {
               sgroups<<settings.getGroupName(i).c_str();
           }

           QString global_objectives=m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_objectives.ini";
           QString global_filters=m_pluginServices->getGlobalConfigFileDirectory()+"/spimb040_filters.ini";
           QString stylesheet="";


           qDebug()<<sgroups;
           for (int i=0; i<sgroups.size(); i++) {
               //settings.beginGroup(sgroups[i]);
               settings.enterGroup(sgroups[i].toStdString());
               bool ok=true;
               QString id=sgroups[i];

               if (id=="global") {
                   global_objectives=settings.getAsString("global_objectives", global_objectives.toStdString()).c_str();
                   global_filters=settings.getAsString("global_filters", global_filters.toStdString()).c_str();
                   stylesheet=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("stylesheet", "").c_str()));

                   QStringList stretch=QString(settings.getAsString("colstretch", "").c_str()).split(",");
                   for (int i=0; i<stretch.size(); i++) {
                       ui->gridLayout->setColumnStretch(i, stretch[i].toInt());
                   }
                   stretch=QString(settings.getAsString("rowstretch", "").c_str()).split(",");
                   for (int i=0; i<stretch.size(); i++) {
                       ui->gridLayout->setRowStretch(i, stretch[i].toInt());
                   }

               } else {

                   QString type=QString(settings.getAsString("type", "unknown").c_str()).trimmed().toLower(); //settings.value("type", "unknown").toString().trimmed().toLower();
                   QString title=QString(settings.getAsString("title", "").c_str());//settings.value("title", "").toString();
                   QString ingroup=QString(settings.getAsString("ingroup", "").c_str());//settings.value("ingroup", "").toString();
                   QString inwidget=QString(settings.getAsString("inwidget", "").c_str());//settings.value("ingroup", "").toString();
                   QString special_role=QString(settings.getAsString("special_role", "").c_str()).trimmed().toLower();
                   QString used_by=QString(settings.getAsString("used_by", "").c_str()).trimmed().toLower();
                   QString subtype=QString(settings.getAsString("subtype", "").c_str()).trimmed().toLower();
                   QGridLayout* ingroupLayout=ui->gridLayout;
                   if (ui_groups.contains(ingroup)) {
                       QGridLayout* ggl=qobject_cast<QGridLayout*>(ui_groups[ingroup]->layout());
                       if (ggl) {
                           ingroupLayout=ggl;
                       } else {
                           m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    item '%2': did not find INGROUP (%3). Ignoring element!\n").arg(filename).arg(id).arg(ingroup));
                           ok=false;
                       }
                   }
                   if (ui_widgets.contains(ingroup)) {
                       QGridLayout* ggl=qobject_cast<QGridLayout*>(ui_widgets[ingroup]->layout());
                       if (ggl) {
                           ingroupLayout=ggl;
                       } else {
                           m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    item '%2': did not find INGROUP (%3). Ignoring element!\n").arg(filename).arg(id).arg(ingroup));
                           ok=false;
                       }
                   }

                   int x=settings.getAsInt("x", 0);//settings.value("x", ingroupLayout->columnCount()).toInt();
                   int y=settings.getAsInt("y", 0);//settings.value("y", ingroupLayout->rowCount()).toInt();
                   if (ingroupLayout) y=settings.getAsInt("y",  ingroupLayout->rowCount());
                   int width=settings.getAsInt("width", 16);//settings.value("x", ingroupLayout->columnCount()).toInt();
                   int height=settings.getAsInt("height", 16);//settings.value("y", ingroupLayout->rowCount()).toInt();
                   int rowSpan=settings.getAsInt("rowspan", 1);//settings.value("rowspan", 1).toInt();
                   int colSpan=settings.getAsInt("colspan", 1);//settings.value("colspan", 1).toInt();
                   int margin=settings.getAsInt("margin", 5);//settings.value("rowspan", 1).toInt();
                   int spacing=settings.getAsInt("spacing", 3);//settings.value("colspan", 1).toInt();
                   QWidget* widNew=NULL;
                   if (ok) {
                       qDebug()<<"create "<<type<<"-element '"<<id<<"' in "<<ingroupLayout->parent()->objectName()<<"   (x,y)="<<x<<y<<"   (ingroupW, ingroupH)="<<ingroupLayout->columnCount()<<ingroupLayout->rowCount();
                       if (type=="group") {
                           QGroupBox* w=new QGroupBox(this);
                           QGridLayout* wgl=new QGridLayout();
                           wgl->setContentsMargins(margin,margin,margin,margin);
                           wgl->setSpacing(spacing);
                           QStringList stretch=QString(settings.getAsString("colstretch", "").c_str()).split(",");
                           for (int i=0; i<stretch.size(); i++) {
                               qDebug()<<id<<"  colstretch("<<i<<")="<<stretch[i].toInt();
                               wgl->setColumnStretch(i, stretch[i].toInt());
                           }
                           stretch=QString(settings.getAsString("rowstretch", "").c_str()).split(",");
                           for (int i=0; i<stretch.size(); i++) {
                               wgl->setRowStretch(i, stretch[i].toInt());
                           }

                           widNew=w;
                           w->setLayout(wgl);
                           w->setTitle(title);
                           w->setCheckable(settings.getAsBool("checkable", false));
                           w->setChecked(settings.getAsBool("checked", false));
                           w->setFlat(settings.getAsBool("flat", false));
                           ui_groups[id]=w;
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                       } else if (type=="frame" || type=="hline" || type=="vline") {
                           QFrame* w=new QFrame(this);
                           QGridLayout* wgl=new QGridLayout();
                           wgl->setContentsMargins(margin,margin,margin,margin);
                           wgl->setSpacing(spacing);
                           widNew=w;
                           w->setLayout(wgl);
                           QString setProp=QString(settings.getAsString("shadow", "raised").c_str()).trimmed().toLower();
                           if (setProp=="plain") w->setFrameShadow(QFrame::Plain);
                           if (setProp=="sunken") w->setFrameShadow(QFrame::Sunken);
                           if (setProp=="raised") w->setFrameShadow(QFrame::Raised);
                           setProp=QString(settings.getAsString("shape", "panel").c_str()).trimmed().toLower();
                           if (setProp=="none") w->setFrameStyle(QFrame::NoFrame);
                           if (setProp=="box") w->setFrameStyle(QFrame::Box);
                           if (setProp=="panel") w->setFrameStyle(QFrame::Panel);
                           if (setProp=="styledpanel") w->setFrameStyle(QFrame::StyledPanel);
                           if (setProp=="hline") w->setFrameStyle(QFrame::HLine);
                           if (setProp=="vline") w->setFrameStyle(QFrame::VLine);
                           if (setProp=="winpanel") w->setFrameStyle(QFrame::WinPanel);
                           w->setLineWidth(settings.getAsInt("linewidth", 1));
                           w->setLineWidth(settings.getAsInt("midlinewidth", 1));
                           if (type=="hline") w->setFrameStyle(QFrame::HLine);
                           if (type=="vline") w->setFrameStyle(QFrame::VLine);

                           QStringList stretch=QString(settings.getAsString("colstretch", "").c_str()).split(",");
                           for (int i=0; i<stretch.size(); i++) {
                               wgl->setColumnStretch(i, stretch[i].toInt());
                           }
                           stretch=QString(settings.getAsString("rowstretch", "").c_str()).split(",");
                           for (int i=0; i<stretch.size(); i++) {
                               wgl->setRowStretch(i, stretch[i].toInt());
                           }

                           ui_widgets[id]=w;
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                       } else if (type=="tabwidget") {
                           QFEnhancedTabWidget* w=new QFEnhancedTabWidget(this);
                           QGridLayout* wgl=new QGridLayout();
                           widNew=w;
                           ui_tabs[id]=w;
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                       } else if (type=="tabpage") {

                           if (inwidget.isEmpty() || !ui_tabs.contains(inwidget)) {
                               m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    %3-item '%2' parent '%4' does not exist. Ignoring element!\n").arg(filename).arg(id).arg(type).arg(inwidget));
                               ok=false;

                           } else {

                               QWidget* w=new QWidget(this);
                               QGridLayout* wgl=new QGridLayout();
                               wgl->setContentsMargins(margin,margin,margin,margin);
                               wgl->setSpacing(spacing);
                               widNew=w;
                               w->setLayout(wgl);

                               ui_widgets[id]=w;
                               ui_tabs[inwidget]->addTab(w, title);
                           }


                       } else if (type=="label") {
                           QLabel* w=new QLabel(this);
                           widNew=w;
                           w->setTextFormat(Qt::AutoText);
                           w->setText(title);
                           QString image=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("image", "").c_str()));//settings.value("title", "").toString();
                           qDebug()<<image;
                           if (QFile::exists(image)) {
                               w->setPixmap(QPixmap(image));
                           }
                           ui_labels[id]=w;
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan, Qt::AlignTop|Qt::AlignLeft);
                       } else if (type=="filter") {
                           QF3FilterCombobox* w=new QF3FilterCombobox(this);
                           widNew=w;
                           QString gf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("global_filters", global_filters.toStdString()).c_str()));
                           QString lf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("local_filters", QString(m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini").toStdString()).c_str()));
                           w->setFilterINI(gf, lf);
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by)) {
                                   ui_cameras[used_by].filters.append(id);
                                   connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].filters.append(id);
                           }
                           ui_filters[id]=w;


                       } else if (type=="dualview") {
                           QF3DualViewWidget* w=new QF3DualViewWidget(this);
                           widNew=w;
                           QString gf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("global_filters", global_filters.toStdString()).c_str()));
                           QString lf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("local_filters", QString(m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini").toStdString()).c_str()));
                           w->setFilterINI(gf, lf);
                           QLabel* l=NULL;
                           QCheckBox* cb=NULL;
                           if (settings.getAsBool("checkable", true)) {
                               cb=new QCheckBox(title, this);
                               cb->setChecked(settings.getAsBool("checked", true));
                               ingroupLayout->addWidget(cb, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                               //connect(cb, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                               connect(cb, SIGNAL(toggled(bool)), this, SLOT(changeDVenabledState()));
                           } else {
                               l=new QLabel(title, this);
                               l->setTextFormat(Qt::AutoText);
                               l->setBuddy(w);
                               ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           }
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by)) {
                                   ui_cameras[used_by].dualview.append(id);
                                   //connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
                                   if (cb) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), cb, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                           }
                           ui_dualviews[id]=qMakePair(cb,w);
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                       } else if (type=="objective") {
                           QF3ObjectiveCombobox* w=new QF3ObjectiveCombobox(this);
                           widNew=w;
                           QString gf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("global_objectives", global_objectives.toStdString()).c_str()));
                           QString lf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("local_objectives", QString(m_pluginServices->getConfigFileDirectory()+"/spimb040_objectives.ini").toStdString()).c_str()));
                           w->setObjectivesINI(gf, lf);
                           QLabel* l=new QLabel(title, this);
                           l->setBuddy(w);
                           l->setTextFormat(Qt::AutoText);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           ui_objectives[id]=w;
                           if (special_role=="detection_objective") {
                               objDetection=id;
                           } else if (special_role=="projection_objective") {
                               objProjection=id;
                           }
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by))  {
                                   //if (subtype=="tubelens" || special_role=="tubelens" || special_role=="detection_objective") {
                                       ui_cameras[used_by].tubelens=id;
                                       connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
                                       if (l) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
                                       w->setEnabled(ui_cameras[used_by].config->isChecked());
                                       if (l) l->setEnabled(ui_cameras[used_by].config->isChecked());
                                   //}
                               }
                           }
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                       } else if (type=="horizontal_stretch") {
                           QSpacerItem* w=new QSpacerItem(width,height,QSizePolicy::Expanding,QSizePolicy::Minimum);
                           //w->setObjectName(id);
                           ingroupLayout->addItem(w, y,x, rowSpan, colSpan);
                       } else if (type=="vertical_stretch") {
                           QSpacerItem* w=new QSpacerItem(width,height,QSizePolicy::Minimum,QSizePolicy::Expanding);
                           //w->setObjectName(id);
                           ingroupLayout->addItem(w, y,x, rowSpan, colSpan);
                       } else if (type=="camera") {
                           QFESPIMB040SimpleCameraConfig* w=new QFESPIMB040SimpleCameraConfig(this);
                           widNew=w;
                           w->init(ui_cameras.size(), m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory(), this);
                           w->setLog(m_log);
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                           connect(w, SIGNAL(toggled(bool)), this, SLOT(changeDVenabledState()));

                           connect(addShortCut(QString("cam%1_acquire_single").arg(ui_cameras.size()), QString("camera %1: acquire single frame").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(previewSingle()));
                           connect(addShortCut(QString("cam%1_acquire_continuous_toggle").arg(ui_cameras.size()), QString("camera %1: toggle preview acquisition").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(startStopPreview()));
                           connect(addShortCut(QString("cam%1_acquire_continuous_stop").arg(ui_cameras.size()), QString("camera %1: stop preview").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(stopPreview()));

                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                           cameraIndex<<id;
                           ui_cameras[id].config=w;
                       } else if (type=="measurement_device") {
                           QFMeasurementDeviceConfigWidget* w=new QFMeasurementDeviceConfigWidget(this);
                           widNew=w;
                           w->setLog(m_log);
                           w->init(m_log, m_pluginServices);
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                           measurementdeviceIndex<<id;
                           ui_measurementdevices[id]=w;
                       } else if (type=="stage") {
                           QFStageConfigWidget* w=new QFStageConfigWidget(this);
                           widNew=w;
                           w->init(m_log, m_pluginServices);
                           w->setLog(m_log);
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                           connect(addShortCut(QString("stage%1_x2").arg(ui_stages.size()), QString("stage %1: joystick speed x2").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedX2()));
                           connect(addShortCut(QString("stage%1_x10").arg(ui_stages.size()), QString("stage %1: joystick speed x10").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedX10()));
                           connect(addShortCut(QString("stage%1_d2").arg(ui_stages.size()), QString("stage %1: joystick speed /2").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedD2()));
                           connect(addShortCut(QString("stage%1_d10").arg(ui_stages.size()), QString("stage %1: joystick speed /10").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedD10()));
                           connect(addShortCut(QString("stage%1_step").arg(ui_stages.size()), QString("stage %1: step x").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(stepX()));
                           connect(addShortCut(QString("stage%1_stepm").arg(ui_stages.size()), QString("stage %1: step -x").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(stepMinusX()));
                           connect(addShortCut(QString("stage%1_joysticktoggle").arg(ui_stages.size()), QString("stage %1: toggle joystick").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(toggleJoystick()));
                           connect(addShortCut(QString("stage%1_joystick_on").arg(ui_stages.size()), QString("stage %1: joystick on").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(joystickOn()));
                           connect(addShortCut(QString("stage%1_joystick_off").arg(ui_stages.size()), QString("stage %1: joystick off").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(joystickOff()));

                           if (special_role=="x") {
                               specialStageX.stage=w;
                               specialStageX.device=0;
                           } else if (special_role=="y") {
                               specialStageY.stage=w;
                               specialStageY.device=0;
                           } else if (special_role=="z") {
                               specialStageZ.stage=w;
                               specialStageZ.device=0;
                           } else if (special_role=="r") {
                               specialStageR.stage=w;
                               specialStageR.device=0;
                           }
                            ui_stages[id]=w;
                            stageIndex<<qMakePair(id, 0);
                       } else if (type=="filterchanger") {
                           QFFilterChangerConfigWidget* w=new QFFilterChangerConfigWidget(this);
                           widNew=w;
                           QString gf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("global_filters", global_filters.toStdString()).c_str()));
                           QString lf=QFileInfo(filename).absoluteDir().absoluteFilePath(QString(settings.getAsString("local_filters", QString(m_pluginServices->getConfigFileDirectory()+"/spimb040_filters.ini").toStdString()).c_str()));
                           w->init(gf, lf, m_log, m_pluginServices);
                           w->setLog(m_log);
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by)) {
                                   ui_cameras[used_by].filters.append(id);
                                   connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].filters.append(id);
                           }

                           if (special_role=="detection_filterchanger" || special_role=="detection" || special_role=="detection_filterwheel" || special_role=="detection_filter") {
                               filtercDetection=id;
                           }
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());


                           connect(addShortCut(QString("filterchanger_%1_filter1").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #1").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter0()));
                           connect(addShortCut(QString("filterchanger_%1_filter2").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #2").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter1()));
                           connect(addShortCut(QString("filterchanger_%1_filter3").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #3").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter2()));
                           connect(addShortCut(QString("filterchanger_%1_filter4").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #4").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter3()));
                           connect(addShortCut(QString("filterchanger_%1_filter5").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #5").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter4()));
                           connect(addShortCut(QString("filterchanger_%1_filter6").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #6").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter5()));
                           connect(addShortCut(QString("filterchanger_%1_filter7").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #7").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter6()));
                           connect(addShortCut(QString("filterchanger_%1_filter8").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #8").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter7()));
                           connect(addShortCut(QString("filterchanger_%1_filter9").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #9").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter8()));
                           connect(addShortCut(QString("filterchanger_%1_filter10").arg(ui_filterchangers.size()), QString("filter changer %1: set filter #10").arg(ui_filterchangers.size())), SIGNAL(activated()), w, SLOT(setFilter9()));

                           ui_filterchangers[id]=w;
                           filterChangerIndex<<id;
                       } else if (type=="stages_xyz") {
                           QFESPIMB040SampleStageConfig* w=new QFESPIMB040SampleStageConfig(this);
                           widNew=w;
                           w->init(m_log, m_pluginServices);
                           w->setLog(m_log);
                           if (!title.isEmpty()) w->setTitle(title);
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                           connect(addShortCut(QString("xyzstage%1_x2").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick speed x2").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(speedX2()));
                           connect(addShortCut(QString("xyzstage%1_x10").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick speed x10").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(speedX10()));
                           connect(addShortCut(QString("xyzstage%1_d2").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick speed /2").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(speedD2()));
                           connect(addShortCut(QString("xyzstage%1_d10").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick speed /10").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(speedD10()));
                           connect(addShortCut(QString("xyzstage%1_stepx").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step x").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepX()));
                           connect(addShortCut(QString("xyzstage%1_stepmx").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step -x").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepMinusX()));
                           connect(addShortCut(QString("xyzstage%1_stepy").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step y").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepY()));
                           connect(addShortCut(QString("xyzstage%1_stepmy").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step -y").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepMinusY()));
                           connect(addShortCut(QString("xyzstage%1_stepz").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step z").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepZ()));
                           connect(addShortCut(QString("xyzstage%1_stepmz").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: step -z").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(stepMinusZ()));
                           connect(addShortCut(QString("xyzstage%1_joysticktoggle").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: toggle joystick").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(toggleJoystick()));
                           connect(addShortCut(QString("xyzstage%1_joystick_on").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick on").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(joystickOn()));
                           connect(addShortCut(QString("xyzstage%1_joystick_off").arg(ui_stageconfigs.size()), QString("XYZ translation stage %1: joystick off").arg(ui_stageconfigs.size())), SIGNAL(activated()), w, SLOT(joystickOff()));

                           for (int ri=0; ri<qMin(3,special_role.size()); ri++) {
                               QChar sr=special_role[ri].toLower();
                               /*char dev='x';
                               if (ri==1) dev='y';
                               if (ri==2) dev='z';*/
                               if (sr=='x') {
                                   specialStageX.stageconfig=w;
                                   specialStageX.device=ri;
                               } else if (sr=='y') {
                                   specialStageY.stageconfig=w;
                                   specialStageY.device=ri;
                               } else if (sr=='z') {
                                   specialStageZ.stageconfig=w;
                                   specialStageZ.device=ri;
                               } else if (sr=='r') {
                                   specialStageR.stageconfig=w;
                                   specialStageR.device=ri;
                               }
                           }

                           ui_stageconfigs[id]=w;
                           stageIndex<<qMakePair(id, 0);
                           stageIndex<<qMakePair(id, 1);
                           stageIndex<<qMakePair(id, 2);
                       } else if (type=="shutter") {
                           QFShutterConfigWidget* w=new QFShutterConfigWidget(this);
                           w->init(m_log, m_pluginServices);
                           widNew=w;
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           if (special_role.contains("main_shutter")) {
                               MainShutterMember ms;
                               ms.shutter=w;
                               ms.mode=msComplete;
                               shutterMain.append(ms);
                               //shutterMain=w;
                               //ui->tbMainShutter->setDefaultAction(w->getStateAction());
                           } else if (special_role.contains("main_shutter_open_only")) {
                               MainShutterMember ms;
                               ms.shutter=w;
                               ms.mode=msOpenOnly;
                               shutterMain.append(ms);
                           } else if (special_role.contains("main_shutter_close_only")) {
                               MainShutterMember ms;
                               ms.shutter=w;
                               ms.mode=msCloseOnly;
                               shutterMain.append(ms);
                           }
                           if (special_role.contains("laser1_shutter")) {
                               //shutterLaser1=w;
                               shutterLaser1ID=id;
                           }
                           if (special_role.contains("laser2_shutter")) {
                               //shutterLaser2=w;
                               shutterLaser2ID=id;
                           }
                           if (special_role.contains("transmission_shutter")) {
                               //shutterTransmission=w;
                               shutterTransmissionID=id;
                           }
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by)) {
                                   ui_cameras[used_by].shutter=id;
                                   connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].shutter=id;
                           }
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                           connect(addShortCut(id+"_toggle", id+" toggle"), SIGNAL(activated()), w, SLOT(toggleShutter()));
                           connect(addShortCut(id+"_on",     id+" on"), SIGNAL(activated()), w, SLOT(shutterOn()));
                           connect(addShortCut(id+"_off",    id+" off"), SIGNAL(activated()), w, SLOT(shutterOff()));
                           ui_shutter[id]=w;
                           shutterIndex<<id;
                       } else if (type=="lightsource") {
                           QFLightSourceConfigWidget* w=new QFLightSourceConfigWidget(this);
                           w->init(m_log, m_pluginServices);
                           widNew=w;
                           QLabel* l=new QLabel(title, this);
                           l->setTextFormat(Qt::AutoText);
                           l->setBuddy(w);
                           ingroupLayout->addWidget(l, y,x, rowSpan, 1, Qt::AlignTop|Qt::AlignLeft);
                           ingroupLayout->addWidget(w, y,x+1, rowSpan, qMax(1,colSpan-1));
                           ui_lightsource[id].config=w;
                           lightsourceIndex<<id;
                           if (subtype=="laser") laserIndex<<id;
                           if (subtype=="brightfield") brightfieldIndex<<id;
                           if (subtype=="brightfield_transmission" || subtype=="brightfield_trans") {
                               brightfieldIndex<<id;
                               brightfieldTransmissionID=id;
                           }
                           if (subtype=="brightfield_epi") {
                               brightfieldIndex<<id;
                               brightfieldEpiID=id;
                           }
                           connect(ui->btnLockFiltersEtc, SIGNAL(toggled(bool)), w, SLOT(setReadOnly(bool)));
                           w->setReadOnly(ui->btnLockFiltersEtc->isChecked());

                       } else {
                           m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    item '%2' has unknown type '%3'. Ignoring element!\n").arg(filename).arg(id).arg(type));
                           ok=false;
                       }

                       if (widNew) {
                           widNew->setObjectName(id);
                       }
                   }
               }
               settings.leaveGroup();

            }

           if (QFile::exists(stylesheet)) {
               QFile file(stylesheet);
               if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                   QTextStream in(&file);
                   ui->gridWidget->setStyleSheet(in.readAll());
               }
           }


        } catch (std::exception& e) { // error handling
            m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    %2\n").arg(filename).arg(e.what()));
        }


        m_log->log_text(tr("loading optSetup-file '%1' ... DONE\n").arg(filename));
    } else {
        m_log->log_error(tr("\nERROR: The optSetup-file '%1' does not exist!\n").arg(filename));
    }

    updateMagnifications();
}

QString QFESPIMB040OpticsSetup2::getLastOptSetup() const
{
    return lastOptSetup;
}

void QFESPIMB040OpticsSetup2::lockStages() {
    QMap<QString, QFESPIMB040SampleStageConfig*>::const_iterator it1;
    for (it1=ui_stageconfigs.constBegin(); it1 != ui_stageconfigs.constEnd(); ++it1) {
        it1.value()->lockStages();
    }
    QMap<QString, QFStageConfigWidget*>::const_iterator it2; ;
    for (it2=ui_stages.constBegin(); it2 != ui_stages.constEnd(); ++it2) {
        it2.value()->lockStages();
    }

}

void QFESPIMB040OpticsSetup2::unlockStages() {
    QMap<QString, QFESPIMB040SampleStageConfig*>::const_iterator it1;
    for (it1=ui_stageconfigs.constBegin(); it1 != ui_stageconfigs.constEnd(); ++it1) {
        it1.value()->unlockStages();
    }
    QMap<QString, QFStageConfigWidget*>::const_iterator it2; ;
    for (it2=ui_stages.constBegin(); it2 != ui_stages.constEnd(); ++it2) {
        it2.value()->unlockStages();
    }
}

void QFESPIMB040OpticsSetup2::closeEvent(QCloseEvent * event) {
    QMap<QString, CameraWidgets>::const_iterator it2; ;
    for (it2=ui_cameras.constBegin(); it2 != ui_cameras.constEnd(); ++it2) {
        it2.value().config->close();
    }
    event->accept();
}

void QFESPIMB040OpticsSetup2::showEvent( QShowEvent * event ) {
    QMap<QString, CameraWidgets>::const_iterator it2; ;
    for (it2=ui_cameras.constBegin(); it2 != ui_cameras.constEnd(); ++it2) {
        it2.value().config->show();
    }
}

QShortcut *QFESPIMB040OpticsSetup2::addShortCut(const QString &id, const QString &label, const QKeySequence& sequence) {
    QFESPIMB040OpticsSetup2::shortcutItem itm;
    itm.shortcut=new QShortcut(m_pluginMainWidget);
    itm.shortcut->setKey(sequence);
    itm.shortcut->setWhatsThis(label);
    itm.shortcut->setContext(Qt::ApplicationShortcut);
    itm.shortcut->setEnabled(!sequence.isEmpty());
    itm.id=id;
    itm.label=label;
    shortcuts.append(itm);
    return itm.shortcut;
}

void QFESPIMB040OpticsSetup2::loadPluginGlobalSettings(QSettings &settings, QObject *extensionObject, QString prefix) {
    QFExtensionGlobalSettingsReadWrite* extensionRW=NULL;
    QFExtension* extension=NULL;

    extension=qobject_cast<QFExtension*>(extensionObject);
    extensionRW=qobject_cast<QFExtensionGlobalSettingsReadWrite*>(extensionObject);
    if (extensionRW) {
        extensionRW->readGlobalSettings(settings, prefix+extension->getID()+"/");
    }
}

void QFESPIMB040OpticsSetup2::storePluginGlobalSettings(QSettings &settings, QObject *extensionObject, QString prefix) const {
    QFExtensionGlobalSettingsReadWrite* extensionRW=NULL;
    QFExtension* extension=NULL;

    extension=qobject_cast<QFExtension*>(extensionObject);
    extensionRW=qobject_cast<QFExtensionGlobalSettingsReadWrite*>(extensionObject);
    if (extensionRW) {
        extensionRW->writeGlobalSettings(settings, prefix+extension->getID()+"/");
    }
}
void QFESPIMB040OpticsSetup2::loadSettings(QFManyFilesSettings &settings, QString prefix) {
    bool updt=updatesEnabled()&&isVisible();
    if (updt) setUpdatesEnabled(false);

   /* qDebug()<<"QFESPIMB040OpticsSetup2::loadSettings():";
    for (int i=0; i<settings.levels(); i++) {
        qDebug()<<"   "<<i<<":";
        if (settings.getSettings(i)) qDebug()<<"     "<<settings.getSettings(i)->fileName();
    }*/

    loadValueSettingsForAllInMap(ui_filters, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_objectives, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_dualviews, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_cameras, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_stageconfigs, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_stages, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_filterchangers, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_shutter, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_lightsource, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_measurementdevices, settings, prefix+"optSetup2/");




    for (int i=0; i<shortcuts.size(); i++) {
        QKeySequence seq(settings.value(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString()).toString());
        shortcuts[i].shortcut->setKey(seq);
        shortcuts[i].shortcut->setEnabled(!seq.isEmpty());
    }
    if (updt) setUpdatesEnabled(true);
}

void QFESPIMB040OpticsSetup2::loadSettings(const QString &settingsFilename, QString prefix)
{
    QFManyFilesSettings settings;
    QSettings set(settingsFilename, QSettings::IniFormat);
    settings.addSettings(&set, false);
    loadSettings(settings, prefix);
}

void QFESPIMB040OpticsSetup2::loadSettings(const QStringList &settingsFilenames, QString prefix, bool firstReadonly)
{
    QFManyFilesSettings settings;
    for (int i=0; i<settingsFilenames.size(); i++) {
        QSettings* set=new QSettings(settingsFilenames[i], QSettings::IniFormat);
        settings.addSettings(set, true, (i==0)&&firstReadonly);
    }
    loadSettings(settings, prefix);
}

void QFESPIMB040OpticsSetup2::storeSettings(QFManyFilesSettings &settings, QString prefix) {

    /*qDebug()<<"QFESPIMB040OpticsSetup2::storeSettings():";
    for (int i=0; i<settings.levels(); i++) {
        qDebug()<<"   "<<i<<":";
        if (settings.getSettings(i)) qDebug()<<"     "<<settings.getSettings(i)->fileName();
    }*/
    saveValueSettingsForAllInMap(ui_filters, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_objectives, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_dualviews, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_cameras, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_stageconfigs, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_stages, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_filterchangers, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_shutter, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_lightsource, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_measurementdevices, settings, prefix+"optSetup2/");


    for (int i=0; i<shortcuts.size(); i++) {
        settings.setValue(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString());
    }
}

void QFESPIMB040OpticsSetup2::storeSettings(const QString &settingsFilename, QString prefix)
{
    QFManyFilesSettings settings;
    QSettings set(settingsFilename, QSettings::IniFormat);
    settings.addSettings(&set, false);
    storeSettings(settings, prefix);
}

void QFESPIMB040OpticsSetup2::storeSettings(const QStringList &settingsFilenames, QString prefix, bool firstReadonly)
{
    QFManyFilesSettings settings;
    for (int i=0; i<settingsFilenames.size(); i++) {
        QSettings* set=new QSettings(settingsFilenames[i], QSettings::IniFormat);
        settings.addSettings(set, true, (i==0)&&firstReadonly);
    }
    storeSettings(settings, prefix);
}

QWidget *QFESPIMB040OpticsSetup2::takeLightpathWidget() const
{
    QLayoutItem* it= ui->layHTop->takeAt(ui->layHTop->indexOf(ui->frmLightpath));
    if (it) {
        return it->widget();
    }
    return NULL;
}

int QFESPIMB040OpticsSetup2::getSpecialShutterID(QFESPIMB040OpticsSetupBase::Shutters shutter) const
{
    switch(shutter) {
        case ShutterMain: if (shutterMain.size()>0) { return shutterIndex.indexOf(ui_shutter.key(shutterMain.first().shutter)); } break;
        case ShutterLaser1: if (!shutterLaser1ID.isEmpty()) { return shutterIndex.indexOf(shutterLaser1ID); } break;
        case ShutterLaser2: if (!shutterLaser2ID.isEmpty()) { return shutterIndex.indexOf(shutterLaser2ID); } break;
        case ShutterTransmission: if (!shutterTransmissionID.isEmpty()) { return shutterIndex.indexOf(shutterTransmissionID); } break;
    }
    return -1;
}



int QFESPIMB040OpticsSetup2::getSpecialStageID(QFESPIMB040OpticsSetupBase::specialStages stage) const
{
    switch(stage) {
        case StageX: {
                if (specialStageX.stage!=NULL) {
                    QString id=ui_stages.key(specialStageX.stage);
                    //QString id=mapFindKey(specialStageX.stageconfig, ui_stages);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                } else if (specialStageX.stageconfig!=NULL) {
                    QString id=ui_stageconfigs.key(specialStageX.stageconfig);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                }
            } break;
        case StageY:{
                if (specialStageY.stage!=NULL) {
                    QString id=ui_stages.key(specialStageY.stage);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                } else if (specialStageY.stageconfig!=NULL) {
                    QString id=ui_stageconfigs.key(specialStageY.stageconfig);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                }
            } break;
        case StageZ:{
                if (specialStageZ.stage!=NULL) {
                    QString id=ui_stages.key(specialStageZ.stage);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                } else if (specialStageZ.stageconfig!=NULL) {
                    QString id=ui_stageconfigs.key(specialStageZ.stageconfig);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                }
            } break;
        case StageR:{
                if (specialStageR.stage!=NULL) {
                    QString id=ui_stages.key(specialStageR.stage);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                } else if (specialStageR.stageconfig!=NULL) {
                    QString id=ui_stageconfigs.key(specialStageR.stageconfig);
                    for (int i=0; i<stageIndex.size(); i++) {
                        if (stageIndex[i].first==id) return i;
                    }
                }
            } break;
    }
    return -1;
}

int QFESPIMB040OpticsSetup2::getSpecialBrightfieldID(QFESPIMB040OpticsSetupBase::specialBrightfieldSources lightsource) const
{
    switch(lightsource) {
        case BrightfieldTransmission: if (!brightfieldTransmissionID.isEmpty()) { return lightsourceIndex.indexOf(brightfieldTransmissionID); } break;
        case BrightfieldEpi: if (!brightfieldEpiID.isEmpty()) { return lightsourceIndex.indexOf(brightfieldEpiID); } break;
    }

    return -1;
}

int QFESPIMB040OpticsSetup2::getSpecialFilterChangerID(QFESPIMB040OpticsSetupBase::specialFilterChangers filterchanger) const
{
    switch(filterchanger) {
        case FilterChangerDetection: if (!filtercDetection.isEmpty()) { return filterChangerIndex.indexOf(filtercDetection); } break;
    }
    return -1;
}

#define ITERATEWIDGETMAPAROUND(MAP_TYPE, MAP, FUNCTION) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        FUNCTION; \
    }
#define ITERATEWIDGETMAPAROUND2(MAP_TYPE, MAP, FUNCTION, FUNCTION2) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        FUNCTION; \
        FUNCTION2; \
    }
#define ITERATEWIDGETMAPAROUND3(MAP_TYPE, MAP, FUNCTION, FUNCTION2, FUNCTION3) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        FUNCTION; \
        FUNCTION2; \
        FUNCTION3; \
    }
#define ITERATEWIDGETMAP(MAP_TYPE, MAP, FUNCTION) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        it.value()->FUNCTION; \
    }
#define ITERATEWIDGETMAPMID(MAP_TYPE, MAP, MID, FUNCTION) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        it.value().MID->FUNCTION; \
    }

#define ITERATEWIDGETMAP_LOADGLOBAL(MAP_TYPE, MAP, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        loadPluginGlobalSettings(SETTINGS, it.value()->GETEXTENSIONFUNCTION(), PREFIX); \
    }

#define ITERATEWIDGETMAP_STOREGLOBAL(MAP_TYPE, MAP, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        storePluginGlobalSettings(SETTINGS, it.value()->GETEXTENSIONFUNCTION(), PREFIX); \
    }

#define ITERATEWIDGETMAP_LOADGLOBALMID(MAP_TYPE, MAP, MID, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        loadPluginGlobalSettings(SETTINGS, it.value().MID->GETEXTENSIONFUNCTION(), PREFIX); \
    }

#define ITERATEWIDGETMAP_STOREGLOBALMID(MAP_TYPE, MAP, MID, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        storePluginGlobalSettings(SETTINGS, it.value().MID->GETEXTENSIONFUNCTION(), PREFIX); \
    }

#define ITERATEWIDGETMAP_LOADGLOBAL_LIST(MAP_TYPE, MAP, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        QList<QObject*> l=it.value()->GETEXTENSIONFUNCTION(); \
        for (int jj=0; jj<l.size(); jj++) {\
            loadPluginGlobalSettings(SETTINGS, l[jj], PREFIX); \
        }\
    }

#define ITERATEWIDGETMAP_STOREGLOBAL_LIST(MAP_TYPE, MAP, GETEXTENSIONFUNCTION, SETTINGS, PREFIX) \
    for (QMap<QString, MAP_TYPE>::const_iterator it=MAP.begin(); it!=MAP.end(); it++) { \
        QList<QObject*> l=it.value()->GETEXTENSIONFUNCTION(); \
        for (int jj=0; jj<l.size(); jj++) {\
            storePluginGlobalSettings(SETTINGS, l[jj], PREFIX); \
        }\
    }



void QFESPIMB040OpticsSetup2::loadPluginGlobalSettings(QSettings &settings, QString prefix) {
    ITERATEWIDGETMAP_LOADGLOBALMID(CameraWidgets, ui_cameras, config, cameraComboBox()->currentCameraQObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBALMID(LightsourceWidgets, ui_lightsource, config, getLightSourceExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getXStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getYStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getZStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFStageConfigWidget*, ui_stages, getXStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFFilterChangerConfigWidget*, ui_filterchangers, getFilterChangerExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFShutterConfigWidget*, ui_shutter, getShutterExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_LOADGLOBAL(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, getMeasurementDeviceExtensionObject, settings, prefix)
}

void QFESPIMB040OpticsSetup2::storePluginGlobalSettings(QSettings &settings, QString prefix) const {
    ITERATEWIDGETMAP_STOREGLOBALMID(CameraWidgets, ui_cameras, config, cameraComboBox()->currentCameraQObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBALMID(LightsourceWidgets, ui_lightsource, config, getLightSourceExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getXStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getYStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFESPIMB040SampleStageConfig*, ui_stageconfigs, getZStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFStageConfigWidget*, ui_stages, getXStageExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFFilterChangerConfigWidget*, ui_filterchangers, getFilterChangerExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFShutterConfigWidget*, ui_shutter, getShutterExtensionObject, settings, prefix)
    ITERATEWIDGETMAP_STOREGLOBAL(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, getMeasurementDeviceExtensionObject, settings, prefix)
}


double QFESPIMB040OpticsSetup2::getCameraMagnification(int setup_cam) const {
    QString id=cameraIndex.value(setup_cam, "");
    if (ui_cameras.contains(id)) {
        QString tl=ui_cameras[id].tubelens;
        double m=1;
        if (ui_objectives.contains(tl)) {
            m=m*ui_objectives[tl]->objective().magnification;
        }
        if (ui_objectives.contains(objDetection) && ui_objectives[objDetection]) {
            m=m*ui_objectives[objDetection]->objective().magnification;
        }
        return m;
    } else {
        return 1;
    }
}

void saveMeasurementDevice(QMap<QString, QVariant>& setup, const QString& prefix, const QString& name, QFExtensionMeasurementAndControlDevice* device, int id) {
    if (!device) return;
    for (int i=0; i<(int64_t)device->getMeasurementDeviceValueCount(id); i++) {
        setup[prefix+QString("%2/%1").arg(device->getMeasurementDeviceValueShortName(id, i)).arg(name)]=device->getMeasurementDeviceValue(id, i);
    }
}

template<class T>
void saveMeasurementDeviceCast(QMap<QString, QVariant>& setup, const QString& prefix, const QString& name, T* device, int id) {
    QFExtensionMeasurementAndControlDevice* md=dynamic_cast<QFExtensionMeasurementAndControlDevice*>(device);
    if (md) {
        saveMeasurementDevice(setup, prefix, name, md, id);
    }
}

void saveFilter(QMap<QString, QVariant>& setup, const QString& prefix, const QString& id, const FilterDescription& filter) {
    if (filter.isValid) {
        setup[prefix+QString("%1/name").arg(id)]=filter.name;
        setup[prefix+QString("%1/type").arg(id)]=filter.type;
        setup[prefix+QString("%1/manufacturer").arg(id)]=filter.manufacturer;
    }
}

void saveObjective(QMap<QString, QVariant>& setup, const QString& prefix, const QString& id, const ObjectiveDescription& filter) {
    setup[prefix+QString("%1/name").arg(id)]=filter.name;
    setup[prefix+QString("%1/NA").arg(id)]=filter.NA;
    setup[prefix+QString("%1/magnification").arg(id)]=filter.magnification;
    setup[prefix+QString("%1/manufacturer").arg(id)]=filter.manufacturer;
}

QMap<QString, QVariant> QFESPIMB040OpticsSetup2::getSetup(int setup_cam) const {
    QMap<QString, QVariant> setup;

    for (QMap<QString, QF3FilterCombobox*>::const_iterator it=ui_filters.begin(); it!=ui_filters.end(); it++) {
        saveFilter(setup, "filters/", it.key(), it.value()->filter());
    }
    for (QMap<QString, QFFilterChangerConfigWidget*>::const_iterator it=ui_filterchangers.begin(); it!=ui_filterchangers.end(); it++) {
        saveFilter(setup, "filters/", it.key(), it.value()->getCurrentFilterDescription());
    }
    for (QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator it=ui_dualviews.begin(); it!=ui_dualviews.end(); it++) {
        if (it.value().first->isChecked()) {
            if (it.value().second->orientation()==QF3DualViewWidget::Horizontal) setup[QString("filters/%1/orientation").arg(it.key())]=QString("horizontal");
            else setup[QString("filters/%1/orientation").arg(it.key())]=QString("vertical");

            saveFilter(setup, "filters/filter_splitter/", it.key(), it.value().second->filterSplitter());
            saveFilter(setup, "filters/filter_short/", it.key(), it.value().second->filterShort());
            saveFilter(setup, "filters/filter_long/", it.key(), it.value().second->filterLong());
        }
    }
    for (QMap<QString, QF3ObjectiveCombobox*>::const_iterator it=ui_objectives.begin(); it!=ui_objectives.end(); it++) {
        saveObjective(setup, "objectives/", it.key(), it.value()->objective());
    }

    for (QMap<QString, QFShutterConfigWidget*>::const_iterator it=ui_shutter.begin(); it!=ui_shutter.end(); it++) {
        setup[QString("shutters/%1/state").arg(it.key())]= it.value()->getShutterState();
    }

    for (QMap<QString,LightsourceWidgets>::const_iterator it=ui_lightsource.begin(); it!=ui_lightsource.end(); it++) {
        QString prefix=QString("lightsource/%1/").arg(it.key());
        if (ui_shutter.contains(it.value().shutter) && ui_shutter[it.value().shutter]) {
            setup[prefix+"shutter"]=ui_shutter[it.value().shutter]->getShutterState();
        }
        setup[prefix+"lightsource_description"]=it.value().config->getLightSource()->getLightSourceDescription(it.value().config->getLightSourceID());
        setup[prefix+"lightsource_short_name"]=it.value().config->getLightSource()->getLightSourceShortName(it.value().config->getLightSourceID());
        for (int i=0; i<it.value().config->getLineCount(); i++) {
            setup[prefix+QString("line%1/name").arg(i+1)]=it.value().config->getLineDescription(i);
            setup[prefix+QString("line%1/enabled").arg(i+1)]=it.value().config->isLineEnabled(i);
            setup[prefix+QString("line%1/set_power").arg(i+1)]=it.value().config->getSetPower(i);
            setup[prefix+QString("line%1/measured_power").arg(i+1)]=it.value().config->getMeasuredPower(i);
            setup[prefix+QString("line%1/unit").arg(i+1)]=it.value().config->getLineUnit(i);
        }

    }

    return setup;
}

int QFESPIMB040OpticsSetup2::getCameraCount() const
{
    return ui_cameras.size();
}


void QFESPIMB040OpticsSetup2::setLogging(QFPluginLogService* log) {
    m_log=log;
    ITERATEWIDGETMAPMID(CameraWidgets, ui_cameras, config, setLog(m_log))
    ITERATEWIDGETMAPMID(LightsourceWidgets, ui_lightsource, config, setLog(m_log))
    ITERATEWIDGETMAP(QFFilterChangerConfigWidget*, ui_filterchangers, setLog(m_log))
    ITERATEWIDGETMAP(QFStageConfigWidget*, ui_stages, setLog(m_log))
    ITERATEWIDGETMAP(QFESPIMB040SampleStageConfig*, ui_stageconfigs, setLog(m_log))
    ITERATEWIDGETMAP(QFShutterConfigWidget*, ui_shutter, setLog(m_log))
    ITERATEWIDGETMAP(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, setLog(m_log))
}

bool QFESPIMB040OpticsSetup2::lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename) {
    QString id=cameraIndex.value(setup_cam, "");
    if (ui_cameras.contains(id)) {
        return ui_cameras[id].config->lockCamera(extension, ecamera, camera,  previewSettingsFilename);
    }
    return false;
}

void QFESPIMB040OpticsSetup2::releaseCamera(int setup_cam) {
    QString id=cameraIndex.value(setup_cam, "");
    if (ui_cameras.contains(id)) {
        ui_cameras[id].config->releaseCamera();
    }
}

void QFESPIMB040OpticsSetup2::overrideCameraPreview(int setup_cam, const QString &camera_settings, const QString &lightpath)
{
    if (!lightpath.isEmpty() && QFile::exists(lightpath)) {
        ui->cmbLightpathConfig->setEnabled(false);
        lockLightpath();
        loadLightpathConfig(lightpath, true);
        unlockLightpath();
    }

    QString id=cameraIndex.value(setup_cam, "");
    if (ui_cameras.contains(id)) {
        ui_cameras[id].config->overridePreview(camera_settings);
    }
    loadLightpathConfig(lightpath, true);
}

void QFESPIMB040OpticsSetup2::resetCameraPreview(int setup_cam)
{

    QString id=cameraIndex.value(setup_cam, "");
    if (ui_cameras.contains(id)) {
        ui_cameras[id].config->resetPreview();
    }
    ensureLightpath();
    ui->cmbLightpathConfig->setEnabled(true);
}

void QFESPIMB040OpticsSetup2::on_btnConnectDevices_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Connecting devices ..."));

    ITERATEWIDGETMAPAROUND(QFShutterConfigWidget*, ui_shutter, dlg->addItem(tr("shutter: ")+it.key()))
    ITERATEWIDGETMAPAROUND(LightsourceWidgets, ui_lightsource, dlg->addItem(tr("lightsource: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFFilterChangerConfigWidget*, ui_filterchangers, dlg->addItem(tr("filterwheel: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFStageConfigWidget*, ui_stages, dlg->addItem(tr("stage: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFESPIMB040SampleStageConfig*, ui_stageconfigs, dlg->addItem(tr("stage: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, dlg->addItem(tr("measurement: ")+it.key()))

    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

    ITERATEWIDGETMAPAROUND3(QFShutterConfigWidget*, ui_shutter, if (!dlg->wasCanceled()) it.value()->connectShutter(), dlg->nextItem((it.value()->isShutterConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(LightsourceWidgets, ui_lightsource, if (!dlg->wasCanceled()) it.value().config->connectLightSource(), dlg->nextItem((it.value().config->isLightSourceConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFFilterChangerConfigWidget*, ui_filterchangers, if (!dlg->wasCanceled()) it.value()->connectFilterChanger(), dlg->nextItem((it.value()->isFilterChangerConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFStageConfigWidget*, ui_stages, if (!dlg->wasCanceled()) it.value()->connectStages(), dlg->nextItem((it.value()->isXStageConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFESPIMB040SampleStageConfig*, ui_stageconfigs, if (!dlg->wasCanceled()) it.value()->connectStages(), dlg->nextItem((it.value()->isXStageConnected()||it.value()->isYStageConnected()||it.value()->isZStageConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, if (!dlg->wasCanceled()) it.value()->connectMeasurementDevice(), dlg->nextItem((it.value()->isMeasurementDeviceConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())

    dlg->close();
    delete dlg;
}

void QFESPIMB040OpticsSetup2::on_btnDisconnectDevices_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Disconnecting devices ..."));
    ITERATEWIDGETMAPAROUND(QFShutterConfigWidget*, ui_shutter, dlg->addItem(tr("shutter: ")+it.key()))
    ITERATEWIDGETMAPAROUND(LightsourceWidgets, ui_lightsource, dlg->addItem(tr("lightsource: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFFilterChangerConfigWidget*, ui_filterchangers, dlg->addItem(tr("filterwheel: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFStageConfigWidget*, ui_stages, dlg->addItem(tr("stage: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFESPIMB040SampleStageConfig*, ui_stageconfigs, dlg->addItem(tr("stage: ")+it.key()))
    ITERATEWIDGETMAPAROUND(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, dlg->addItem(tr("measurement: ")+it.key()))
    dlg->setHasCancelButton(false);
    dlg->show();
    dlg->start();

    ITERATEWIDGETMAPAROUND3(QFShutterConfigWidget*, ui_shutter, it.value()->disconnectShutter(), dlg->nextItem(), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(LightsourceWidgets, ui_lightsource, it.value().config->disconnectLightSource(), dlg->nextItem(), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFFilterChangerConfigWidget*, ui_filterchangers, it.value()->disconnectFilterChanger(), dlg->nextItem(), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFStageConfigWidget*, ui_stages, it.value()->disconnectStages(), dlg->nextItem(), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFESPIMB040SampleStageConfig*, ui_stageconfigs, it.value()->disconnectStages(), dlg->nextItem(), QApplication::processEvents())
    ITERATEWIDGETMAPAROUND3(QFMeasurementDeviceConfigWidget*, ui_measurementdevices, it.value()->disconnectMeasurementDevice(), dlg->nextItem(), QApplication::processEvents())

    delete dlg;
}
void QFESPIMB040OpticsSetup2::on_btnConnectCameras_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Connecting cameras ..."));
    ITERATEWIDGETMAPAROUND(CameraWidgets, ui_cameras, dlg->addItem(tr("camera: ")+it.key()))
    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

    ITERATEWIDGETMAPAROUND3(CameraWidgets, ui_cameras, if (!dlg->wasCanceled()) it.value().config->connectCamera(), dlg->nextItem((it.value().config->isCameraConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed)), QApplication::processEvents())

    dlg->close();
    delete dlg;

}


void QFESPIMB040OpticsSetup2::on_btnDisconnectCameras_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Disconnecting cameras ..."));
    ITERATEWIDGETMAPAROUND(CameraWidgets, ui_cameras, dlg->addItem(tr("camera: ")+it.key()))
    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

    ITERATEWIDGETMAPAROUND3(CameraWidgets, ui_cameras, if (!dlg->wasCanceled()) it.value().config->disconnectCamera(), dlg->nextItem(), QApplication::processEvents())

    dlg->close();
    delete dlg;

}

/*void QFESPIMB040OpticsSetup2::on_chkDetectionFilterWheel_toggled(bool checked) {
    if (!checked) {
        ui->filtcDetection->disconnectFilterChanger();
    }
}*/

void QFESPIMB040OpticsSetup2::configsChanged(QFESPIMB040OpticsSetupItems configs) {
    emit lightpathesChanged(configs);
}


void QFESPIMB040OpticsSetup2::configShortcuts() {
    QFESPIMB040ShortcutConfigDialog* dlg=new QFESPIMB040ShortcutConfigDialog(this);

    for (int i=0; i<shortcuts.size(); i++) {
        dlg->addShortcut(shortcuts[i].label, shortcuts[i].shortcut->key());
    }

    if (dlg->exec()==QDialog::Accepted) {
        for (int i=0; i<shortcuts.size(); i++) {
            QKeySequence seq=dlg->getSequence(i);
            shortcuts[i].shortcut->setKey(seq);
            shortcuts[i].shortcut->setEnabled(!seq.isEmpty());
        }
    }
    delete dlg;
}

void QFESPIMB040OpticsSetup2::changeDVenabledState()
{
    QFESPIMB040SimpleCameraConfig* cam=dynamic_cast<QFESPIMB040SimpleCameraConfig*>(sender());
    QCheckBox* chk=dynamic_cast<QCheckBox*>(sender());
    QString dvID="";


    if (cam) {
        QMap<QString, CameraWidgets>::const_iterator i;
        for (i = ui_cameras.constBegin(); i != ui_cameras.constEnd(); ++i) {
            if (i.value().config==cam) {
                dvID=i.value().dualview;
            }
        }
        QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator ic;
        for (ic = ui_dualviews.constBegin(); ic != ui_dualviews.constEnd(); ++ic) {
            if (ic.key()==dvID) {
                chk=ic.value().first;
            }
        }

    }
    if (chk) {
        QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> >::const_iterator i;
        for (i = ui_dualviews.constBegin(); i != ui_dualviews.constEnd(); ++i) {
            if (i.value().first==chk) {
                dvID=i.key();
            }
        }
        QMap<QString, CameraWidgets>::const_iterator ic;
        for (ic = ui_cameras.constBegin(); ic != ui_cameras.constEnd(); ++ic) {
            if (ic.value().dualview==dvID) {
                cam=ic.value().config;
            }
        }
    }
    if (ui_dualviews.contains(dvID)) {
        bool en=true;
        if (chk) en=en&&chk->isChecked();
        if (cam) en=en&&cam->isChecked();
        ui_dualviews[dvID].second->setEnabled(en);
    }
}

void QFESPIMB040OpticsSetup2::mainShutterToggled(bool checked)
{
    if (checked) {
        actMainShutter->setIcon(QIcon(":/spimb040/shutter_opened.png"));
    } else {
        actMainShutter->setIcon(QIcon(":/spimb040/shutter_closed.png"));
    }

    setMainIlluminationShutter(checked, true);
}

void QFESPIMB040OpticsSetup2::mainShutterOn()
{
    actMainShutter->setChecked(true);
}

void QFESPIMB040OpticsSetup2::mainShutterOff()
{
    actMainShutter->setChecked(false);
}


void QFESPIMB040OpticsSetup2::updateMagnifications() {
    QMap<QString, CameraWidgets>::const_iterator i;
    for (i = ui_cameras.constBegin(); i != ui_cameras.constEnd(); ++i) {
        double m=1;
        if (ui_objectives.contains(objDetection) && ui_objectives[objDetection]) m=ui_objectives[objDetection]->objective().magnification;
        if (ui_objectives.contains(i.value().tubelens)) {
            m=m*ui_objectives[i.value().tubelens]->objective().magnification;
        }
        i.value().config->setMagnification(m);
    }
}




QFExtensionFilterChanger *QFESPIMB040OpticsSetup2::getFilterChanger(int changer) const
{
    if (changer==FilterChangerDetection && ui_filterchangers.contains(filtercDetection) && ui_filterchangers[filtercDetection]) return ui_filterchangers[filtercDetection]->getFilterChanger();
    return NULL;
}

int QFESPIMB040OpticsSetup2::getFilterChangerID(int changer) const
{
    if (changer==FilterChangerDetection && ui_filterchangers.contains(filtercDetection) && ui_filterchangers[filtercDetection]) return ui_filterchangers[filtercDetection]->getFilterChangerID();
    return -1;
}

QString QFESPIMB040OpticsSetup2::getFilterChangerName(int changer) const
{
    if (changer==FilterChangerDetection && ui_filterchangers.contains(filtercDetection) && ui_filterchangers[filtercDetection]) return tr("detection");
    return QString();
}

int QFESPIMB040OpticsSetup2::getFilterChangerCount() const
{
    if (ui_filterchangers.contains(filtercDetection) && ui_filterchangers[filtercDetection]) return 1;
    return 0;
}

QFExtensionLightSource *QFESPIMB040OpticsSetup2::getLaser(int laser)
{
    if (laser>=0 && laser<laserIndex.size()) {
        if (ui_lightsource.contains(laserIndex[laser]) && ui_lightsource[laserIndex[laser]].config) return ui_lightsource[laserIndex[laser]].config->getLightSource();
    }

    return NULL;
}


QFExtensionLightSource *QFESPIMB040OpticsSetup2::getBrightfieldLightSource(int laser)
{
    if (laser>=0 && laser<brightfieldIndex.size()) {
        if (ui_lightsource.contains(brightfieldIndex[laser]) && ui_lightsource[brightfieldIndex[laser]].config) return ui_lightsource[brightfieldIndex[laser]].config->getLightSource();
    }

    return NULL;
}

int QFESPIMB040OpticsSetup2::getLaserID(int laser)
{
    if (laser>=0 && laser<laserIndex.size()) {
        if (ui_lightsource.contains(laserIndex[laser]) && ui_lightsource[laserIndex[laser]].config) return ui_lightsource[laserIndex[laser]].config->getLightSourceID();
    }

    return -1;
}


int QFESPIMB040OpticsSetup2::getBrightfieldLightSourceID(int laser)
{
    if (laser>=0 && laser<brightfieldIndex.size()) {
        if (ui_lightsource.contains(brightfieldIndex[laser]) && ui_lightsource[brightfieldIndex[laser]].config) return ui_lightsource[brightfieldIndex[laser]].config->getLightSourceID();
    }

    return -1;
}

int QFESPIMB040OpticsSetup2::getLaserCount() const
{
    return laserIndex.size();
}

bool QFESPIMB040OpticsSetup2::isStageConnected(QFExtensionLinearStage* stage, int id, bool& found) {
    if (!stage || id<0) return false;
    {
        QMapIterator<QString, QFESPIMB040SampleStageConfig*> it(ui_stageconfigs);
        while (it.hasNext())  {
            it.next();
            if (it.value() && stage==it.value()->getXStage()) { found=true; return it.value()->isXStageConnected(); }
            if (it.value() && stage==it.value()->getYStage()) { found=true; return it.value()->isYStageConnected(); }
            if (it.value() && stage==it.value()->getZStage()) { found=true; return it.value()->isZStageConnected(); }
        }
    }
    {
        QMapIterator<QString, QFStageConfigWidget*> it2(ui_stages);
        while (it2.hasNext())  {
            it2.next();
            if (it2.value() && stage==it2.value()->getXStage()) { found=true; return it2.value()->isXStageConnected(); }
        }
    }
    found=false;
    return false;

}

int QFESPIMB040OpticsSetup2::getStageAxis(int stage)
{
    if (stage>=0 && stage<stageIndex.size()) {
        if (ui_stages.contains(stageIndex[stage].first) && ui_stages[stageIndex[stage].first]) return ui_stages[stageIndex[stage].first]->getXStageAxis();
        if (ui_stageconfigs.contains(stageIndex[stage].first) && ui_stageconfigs[stageIndex[stage].first]) {
            if (stageIndex[stage].second==0) return ui_stageconfigs[stageIndex[stage].first]->getXStageAxis();
            if (stageIndex[stage].second==1) return ui_stageconfigs[stageIndex[stage].first]->getYStageAxis();
            if (stageIndex[stage].second==2) return ui_stageconfigs[stageIndex[stage].first]->getZStageAxis();
        }
    }

    return -1;

}

QString QFESPIMB040OpticsSetup2::getStageName(int stage) const {
    if (stage>=0 && stage<stageIndex.size()) {
        if (stage==getSpecialStageID(StageX)) return tr("x");
        if (stage==getSpecialStageID(StageY)) return tr("y");
        if (stage==getSpecialStageID(StageZ)) return tr("z");
        if (stage==getSpecialStageID(StageR)) return tr("r");
        return stageIndex[stage].first;
    }

    return QString("stage%1").arg(stage);

}

int QFESPIMB040OpticsSetup2::getStageCount() const
{
    return stageIndex.size();
}


bool QFESPIMB040OpticsSetup2::isStageConnected(int stage) const
{
    if (stage>=0 && stage<stageIndex.size()) {
        QString id=stageIndex[stage].first;
        int num=stageIndex[stage].second;
        if (ui_stageconfigs.contains(id)) {
            if (num==0) return ui_stageconfigs[id]->isXStageConnected();
            if (num==1) return ui_stageconfigs[id]->isYStageConnected();
            if (num==2) return ui_stageconfigs[id]->isZStageConnected();
        } else if (ui_stages.contains(id)) {
            return ui_stages[id]->isXStageConnected();
        }
    }
    return false;
}


QFExtension *QFESPIMB040OpticsSetup2::getStageExtension(int stage)
{
    if (stage>=0 && stage<stageIndex.size()) {
        QString id=stageIndex[stage].first;
        int num=stageIndex[stage].second;
        if (ui_stageconfigs.contains(id)) {
            if (num==0) return ui_stageconfigs[id]->getXStageExtension();
            if (num==1) return ui_stageconfigs[id]->getYStageExtension();
            if (num==2) return ui_stageconfigs[id]->getZStageExtension();
        } else if (ui_stages.contains(id)) {
            return ui_stages[id]->getXStageExtension();
        }
    }
    return NULL;
}

QFExtensionLinearStage *QFESPIMB040OpticsSetup2::getStage(int stage)
{
    if (stage>=0 && stage<stageIndex.size()) {
        QString id=stageIndex[stage].first;
        int num=stageIndex[stage].second;
        if (ui_stageconfigs.contains(id)) {
            if (num==0) return ui_stageconfigs[id]->getXStage();
            if (num==1) return ui_stageconfigs[id]->getYStage();
            if (num==2) return ui_stageconfigs[id]->getZStage();
        } else if (ui_stages.contains(id)) {
            return ui_stages[id]->getXStage();
        }
    }
    return NULL;
}


QFCameraComboBox* QFESPIMB040OpticsSetup2::cameraComboBox(int camera) const {
    if (camera>=0 && camera<cameraIndex.size()) {
        QString id=cameraIndex[camera];
        if (ui_cameras.contains(id) && ui_cameras[id].config)
            return ui_cameras[id].config->cameraComboBox();
    }

    return NULL;
}

QFESPIMB040SimpleCameraConfig *QFESPIMB040OpticsSetup2::cameraConfig(int camera) const
{
    if (camera>=0 && camera<cameraIndex.size()) {
        QString id=cameraIndex[camera];
        if (ui_cameras.contains(id)) return ui_cameras[id].config;
    }

    return NULL;
}

QFCameraConfigComboBoxStartResume* QFESPIMB040OpticsSetup2::getStopRelease(int camera) const {    
    return cameraConfig(camera);
}

bool QFESPIMB040OpticsSetup2::setMainIlluminationShutter(bool opened, bool blocking) {
    if (!isMainIlluminationShutterAvailable()) return false;

    waitForMainShutter.clear();
    for (int i=0; i<shutterMain.size(); i++) {
        if (shutterMain[i].mode==msComplete) {
            shutterMain[i].shutter->setShutter(opened);
            waitForMainShutter.insert(i);
        } else if (shutterMain[i].mode==msOpenOnly && opened==true) {
            shutterMain[i].shutter->setShutter(true);
            waitForMainShutter.insert(i);
        } else if (shutterMain[i].mode==msCloseOnly && opened==false) {
            shutterMain[i].shutter->setShutter(false);
            waitForMainShutter.insert(i);
        }
    }

    if (blocking) {
        QTime t;
        t.start();
        bool ok=false;
        while (!ok && t.elapsed()<10000) {
            ok=true;
            for (int i=0; i<shutterMain.size(); i++) {
                if (shutterMain[i].mode==msComplete) {
                    ok=ok&&(shutterMain[i].shutter->getShutterState()==opened);
                } else if (shutterMain[i].mode==msOpenOnly && opened==true) {
                    ok=ok&&(shutterMain[i].shutter->getShutterState()==opened);
                } else if (shutterMain[i].mode==msCloseOnly && opened==false) {
                    ok=ok&&(shutterMain[i].shutter->getShutterState()==opened);
                }
            }


            QApplication::processEvents();
        }

        if (t.elapsed()>=10000) {
            m_log->log_error("main shutter timed out after 10s!\n");
            return false;
        }
        return true;
    }
    return true;
}


void QFESPIMB040OpticsSetup2::setSpecialShutter(int shutter, bool opened, bool blocking)
{
    if (shutter==QFESPIMB040OpticsSetup2::ShutterMain) {
        setMainIlluminationShutter(opened, blocking);
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterLaser1 && ui_shutter.contains(shutterLaser1ID) && ui_shutter[shutterLaser1ID]) {
        ui_shutter[shutterLaser1ID]->setShutter(opened);
        if (!opened) ui_shutter[shutterLaser1ID]->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (ui_shutter[shutterLaser1ID]->getShutterState()!=opened && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                m_log->log_error("laser 1 shutter timed out after 10s!\n");
            }
        }
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterLaser2 && ui_shutter.contains(shutterLaser2ID) && ui_shutter[shutterLaser2ID]) {
        ui_shutter[shutterLaser2ID]->setShutter(opened);
        if (!opened) ui_shutter[shutterLaser2ID]->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (ui_shutter[shutterLaser2ID]->getShutterState()!=opened && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                m_log->log_error("laser 2 shutter timed out after 10s!\n");
            }
        }
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterTransmission && ui_shutter.contains(shutterTransmissionID) && ui_shutter[shutterTransmissionID]) {
        ui_shutter[shutterTransmissionID]->setShutter(opened);
        if (!opened) ui_shutter[shutterTransmissionID]->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (ui_shutter[shutterTransmissionID]->getShutterState()!=opened && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                m_log->log_error("transmission shutter timed out after 10s!\n");
            }
        }
    }
}

void QFESPIMB040OpticsSetup2::setShutter(int shutter, bool opened, bool blocking)
{
    if (shutter>=0 && shutter<getShutterCount()) {
        QFShutterConfigWidget* shutterW=ui_shutter.value(shutterIndex.value(shutter, ""), NULL);
        if (shutterW) {
            shutterW->setShutter(opened);
            if (!opened) shutterW->setShutter(false);

            if (blocking) {
                QTime t;
                t.start();
                while (shutterW->getShutterState()!=opened && t.elapsed()<10000) {
                    QApplication::processEvents();
                }

                if (t.elapsed()>=10000) {
                    m_log->log_error(tr("shutter %1 timed out after 10s!\n").arg(shutter));
                }
            }
        }

    } else {
        m_log->log_error(tr("shutter %1 not available!\n").arg(shutter));
    }
}

int QFESPIMB040OpticsSetup2::getShutterCount() const
{
    return ui_shutter.size();
}



bool QFESPIMB040OpticsSetup2::getMainIlluminationShutter() {
    bool opened=true;

    for (int i=0; i<shutterMain.size(); i++) {
        if (shutterMain[i].mode==msComplete) {
            opened=opened&&shutterMain[i].shutter->getShutterState();
        }
    }

    return opened;
}

QString QFESPIMB040OpticsSetup2::getCurrentLightpathFilename() const {
    return ui->cmbLightpathConfig->currentConfigFilename();
}

QString QFESPIMB040OpticsSetup2::getCurrentLightpath() const {
    return ui->cmbLightpathConfig->currentText();
}


void QFESPIMB040OpticsSetup2::saveLightpathConfig(const QString &filename, const QString& name, QList<bool> saveProp, bool saveMeasured) {
    { // this block ensures that set is destroyed (and the file written) before updateItems() is called
        if (QFile::exists(filename)) QFile::remove(filename);
        QSettings set(filename, QSettings::IniFormat);
        set.clear();
        set.setValue("name", name);


        QMap<QString, QVariant> data;
        saveLightpathConfig(data, name, QString(""), saveProp, saveMeasured);
        QMap<QString, QVariant>::iterator i=data.begin();
        for (i = data.begin(); i != data.end(); ++i) {
            set.setValue(i.key(), i.value());
        }

    }
    ui->cmbLightpathConfig->updateItems(name);
    ui->cmbLightpathConfig->setEnabled(true);
}


void QFESPIMB040OpticsSetup2::deleteCurrentLightpatConfig() {
    QString fn=ui->cmbLightpathConfig->currentConfigFilename();
    QString name=ui->cmbLightpathConfig->currentConfig();
    if (QFile::exists(fn)) {
        if (QMessageBox::question(this, tr("B040 SPIM"), tr("Do you really want to delete lightpath '%1'?\nfile: '%2'").arg(name).arg(fn),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
        {
            QFile::remove(fn);
            ui->cmbLightpathConfig->updateItems();
        }
    }
    ui->cmbLightpathConfig->setEnabled(true);
}

void QFESPIMB040OpticsSetup2::emitLighpathesChanged() {
    ui->cmbLightpathConfig->updateItems(ui->cmbLightpathConfig->currentText());
    ui->cmbLightpathConfig->setEnabled(true);
}

bool QFESPIMB040OpticsSetup2::isMainIlluminationShutterAvailable()  {
    return !shutterMain.isEmpty();
}


void QFESPIMB040OpticsSetup2::userChangedLightpath(QString filename) {
    lockLightpath();
    ui->cmbLightpathConfig->setEnabled(false);
    loadLightpathConfig(filename, true);
    ui->cmbLightpathConfig->setEnabled(true);
    unlockLightpath();
    ui->cmbLightpathConfig->setEnabled(true);
}

void QFESPIMB040OpticsSetup2::unlockLighpathCombobox()
{
    ui->cmbLightpathConfig->setEnabled(true);
}

void QFESPIMB040OpticsSetup2::lockLighpathCombobox()
{
    ui->cmbLightpathConfig->setEnabled(false);
}

void QFESPIMB040OpticsSetup2::ensureLightpath() {
   //qDebug()<<"ensureLightpath()";
    lockLightpath();
   //qDebug()<<"ensureLightpath(): locked";
    ui->cmbLightpathConfig->setEnabled(false);
   //qDebug()<<"ensureLightpath(): disabled";
    loadLightpathConfig(getCurrentLightpathFilename(), true);
   //qDebug()<<"ensureLightpath(): load";
    ui->cmbLightpathConfig->setEnabled(true);
   //qDebug()<<"ensureLightpath(): enabled";
    unlockLightpath();
    //qDebug()<<"ensureLightpath(): unlocked";
    ui->cmbLightpathConfig->setEnabled(true);
}






















#define saveLightpathConfig_SAVESUBFILTER(BASE, FID, fi) \
    if (ui_filters.contains(FID) && ui_filters[FID]) { \
        FilterDescription fd=ui_filters[FID]->filter(); \
        if (fd.isValid) { \
            data[prefix+QString("%1/filter%2/id").arg(BASE).arg(fi)]=FID; \
            data[prefix+QString("%1/filter%2/name").arg(BASE).arg(fi)]=fd.name; \
            data[prefix+QString("%1/filter%2/manufacturer").arg(BASE).arg(fi)]=fd.manufacturer; \
            data[prefix+QString("%1/filter%2/type").arg(BASE).arg(fi)]=fd.type; \
        } \
    } else if (ui_filterchangers.containt(FID) && ui_filterchangers[FID]) { \
        FilterDescription fd=ui_filterchangers[FID]->getCurrentFilterDescription(); \
        if (fd.isValid) { \
            data[prefix+QString("%1/filter%2/id").arg(BASE).arg(fi)]=FID; \
            data[prefix+QString("%1/filter%2/name").arg(BASE).arg(fi)]=fd.name; \
            data[prefix+QString("%1/filter%2/manufacturer").arg(BASE).arg(fi)]=fd.manufacturer; \
            data[prefix+QString("%1/filter%2/type").arg(BASE).arg(fi)]=fd.type; \
        } \
    }


#define saveLightpathConfig_SAVESUBSHUTER(BASE, SID) \
    if (ui_shutter.contains(SID) && ui_shutter[SID]) { \
        data[prefix+QString("%1/shutter/name").arg(BASE)]=SID; \
        data[prefix+QString("%1/shutter/state").arg(BASE)]=ui_shutter[SID]->getShutterState(); \
    }

#define saveLightpathConfig_SAVESHUTER(SID) \
    if (ui_shutter.contains(SID) && ui_shutter[SID]) { \
        data[prefix+QString("%1/state").arg(SID)]=ui_shutter[SID]->getShutterState(); \
    }


#define saveLightpathConfig_SAVEFILTER(FID) \
    if (ui_filters.contains(FID) && ui_filters[FID]) { \
        FilterDescription fd=ui_filters[FID]->filter(); \
        if (fd.isValid) { \
            data[prefix+QString("%1/name").arg(FID)]=fd.name; \
            data[prefix+QString("%1/manufacturer").arg(FID)]=fd.manufacturer; \
            data[prefix+QString("%1/type").arg(FID)]=fd.type; \
        } \
    } else if (ui_filterchangers.containt(FID) && ui_filterchangers[FID]) { \
        FilterDescription fd=ui_filterchangers[FID]->getCurrentFilterDescription(); \
        if (fd.isValid) { \
            data[prefix+QString("%1/name").arg(FID)]=fd.name; \
            data[prefix+QString("%1/manufacturer").arg(FID)]=fd.manufacturer; \
            data[prefix+QString("%1/type").arg(FID)]=fd.type; \
        } \
    }



void QFESPIMB040OpticsSetup2::saveLightpathConfigEx(QMap<QString, QVariant> &data, const QString &name, const QString& prefix,  QList<bool> saveProp, bool saveMeasured, QStringList* idlist, QList<bool>* activeList) {

    data[prefix+"name"]=name;

    int saveIdx=0;

    {
        QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
        while (it.hasNext()) {
            it.next();
            if (it.value().config && saveProp.value(saveIdx, true)) {
                if (idlist) *idlist<<it.key();
                if (activeList) *activeList<<it.value().config->isLightSourceConnected();
                for (int i=0; i<it.value().config->getLineCount(); i++) {
                    data[prefix+QString("%2/line%1/enabled").arg(i+1).arg(it.key())]=it.value().config->isLineEnabled(i);
                    data[prefix+QString("%2/line%1/power").arg(i+1).arg(it.key())]=it.value().config->getSetPower(i);
                    if (saveMeasured) {
                        data[prefix+QString("%2/line%1/measured_power").arg(i+1).arg(it.key())]=it.value().config->getMeasuredPower(i);
                    }
                }
            }
            saveIdx++;
        }
    }
    {
        QMapIterator<QString, QFShutterConfigWidget*> it(ui_shutter);
        while (it.hasNext()) {
            it.next();
            if (saveProp.value(saveIdx, true)) {
                if (idlist) *idlist<<it.key();
                if (activeList) *activeList<<it.value()->isShutterConnected();
                if (ui_shutter.contains(it.key()) && ui_shutter[it.key()]) { \
                    data[prefix+QString("%1/state").arg(it.key())]=ui_shutter[it.key()]->getShutterState(); \
                }
            }
            saveIdx++;
        }
    }

    {
        QMapIterator<QString, QFFilterChangerConfigWidget*> it(ui_filterchangers);
        while (it.hasNext()) {
            it.next();
            if (saveProp.value(saveIdx, true)) {
                if (idlist) *idlist<<it.key();
                if (activeList) *activeList<<it.value()->isFilterChangerConnected();
                if (ui_filterchangers.contains(it.key()) && ui_filterchangers[it.key()]) { \
                    data[prefix+QString("%1/state").arg(it.key())]=ui_filterchangers[it.key()]->getFilterChangerState(); \
                }
            }
            saveIdx++;
        }
    }


    if (saveMeasured) {
        QFESPIMB040OpticsSetup2::measuredValues m=getMeasuredValues();
        QMapIterator<QString, QVariant> it(m.data);
        while (it.hasNext()) {
            it.next();
            if (!data.contains(prefix+it.key())) {
                data[prefix+it.key()]=it.value();
            }
        }

    }
}

void QFESPIMB040OpticsSetup2::saveLightpathConfig(QMap<QString, QVariant> &data, const QString &name, const QString& prefix,  QList<bool> saveProp, bool saveMeasured) {
    saveLightpathConfigEx(data, name, prefix, saveProp, false);
}

void QFESPIMB040OpticsSetup2::saveCurrentLightpatConfig() {
    bool dlgOK=true;
    QString name=ui->cmbLightpathConfig->currentText();

    QStringList lightpathlist;
    QList<bool> lightpathDefaultActive;
    {
        QMap<QString, QVariant> dat;
        saveLightpathConfigEx(dat, "name", "prefix", QList<bool>(), false, &lightpathlist, &lightpathDefaultActive);
    }

    lightpathConfigModel->setReadonly(false);
    lightpathConfigModel->resize(lightpathlist.size(), 2);
    lightpathConfigModel->setColumnTitle(1, tr("item"));
    for (int i=0; i<lightpathlist.size(); i++) {
        lightpathConfigModel->setCellCheckedRole(i, 0, lightpathDefaultActive.value(i, true)?Qt::Checked:Qt::Unchecked);
        lightpathConfigModel->setCell(i, 1, lightpathlist[i]);
    }
    lightpathConfigModel->setReadonlyButStillCheckable(true);


    while (true) {
        QStringList l=ui->cmbLightpathConfig->getConfigs();
        int idx=-1;
        if (!name.isEmpty() && !l.contains(name)) {
            l.append(name);
        }
        idx=l.indexOf(name);
        /*name=QInputDialog::getItem(this, tr("B040 SPIM"),
                                      tr("Enter a name for the lightpath:"), l,idx, true, &dlgOK);*/
        QFESPIMB040LightPathSaveDialog* dlg=new QFESPIMB040LightPathSaveDialog(name, l, lightpathConfigModel, this);
        if (dlg->exec()==QDialog::Accepted) {
            name=dlg->getName();
            if (!name.isEmpty()) {
                QString filename=m_pluginServices->getConfigFileDirectory()+"/plugins/ext_spimb040/"+cleanStringForFilename(name)+".lpc";
                if (QFile::exists(filename)) {
                    QMessageBox::StandardButton res=QMessageBox::question(this, tr("B040 SPIM"),
                                                                          tr("A lightpath config with the name '%1' already exists.\n  filename: '%2'\nOverwrite?").arg(name).arg(filename),
                                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
                    if (res==QMessageBox::Yes) {
                        //qDebug()<<dlg->getCheckedItems();
                        QFile::remove(filename);
                        //qDebug()<<dlg->getCheckedItems();
                        saveLightpathConfig(filename, name, dlg->getCheckedItems(), false);
                        ui->cmbLightpathConfig->setCurrentConfig(name);
                        return;
                    } else if (res==QMessageBox::Cancel) {
                        return;
                    }
                } else {
                    saveLightpathConfig(filename, name);
                    ui->cmbLightpathConfig->setCurrentConfig(name);
                    return;
                }
            }
            delete dlg;
        } else {
            delete dlg;
            return;
        }

    }
    ui->cmbLightpathConfig->setEnabled(true);
}

bool QFESPIMB040OpticsSetup2::lightpathLoaded(const QString &filename) {
    if (setting_lightpath) {
        bool ok=true;
        QSettings set(filename, QSettings::IniFormat);

        QString iid;

        {
            QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
            while (it.hasNext()) {
                it.next();
                if (it.value().config && it.value().config->isLightSourceConnected()) {
                    for (int i=0; i<it.value().config->getLineCount(); i++) {
                        if (ok && set.contains(iid=QString("%2/line%1/enabled").arg(i+1).arg(it.key()))) {
                            ok=ok && (it.value().config->isLineEnabled(i)==set.value(iid).toBool()) && it.value().config->isLastActionComplete();
                        }
                        if (ok && set.contains(iid=QString("%2/line%1/power").arg(i+1).arg(it.key()))) {
                            ok=ok && (it.value().config->isLastActionComplete());
                        }
                    }
                }
                if(!ok) break;
            }
        }
        {
            QMapIterator<QString, QFShutterConfigWidget*> it(ui_shutter);
            while (it.hasNext()) {
                it.next();
                if (it.value() && it.value()->isShutterConnected()) {
                    if (ok && set.contains(iid=QString("%1/state").arg(it.key()))) {
                        ok=ok && (it.value()->getShutterState()==set.value(iid).toBool());
                    }
                }
                if(!ok) break;
            }
        }

        {
            QMapIterator<QString, QFFilterChangerConfigWidget*> it(ui_filterchangers);
            while (it.hasNext()) {
                it.next();
                if (it.value() && it.value()->isFilterChangerConnected()) {
                    if (ok && set.contains(iid=QString("%1/state").arg(it.key()))) {
                        ok=ok && (it.value()->getFilterChangerState()==set.value(iid).toInt());
                    }
                }
                if(!ok) break;

            }
        }

        return ok;
    }
    return true;
}


void QFESPIMB040OpticsSetup2::loadLightpathConfig(const QString &filename, bool waiting, QString*name) {
    if (!QFile::exists(filename)) return;
    //qDebug()<<"loadLightpathConfig("<<filename<<")";
    QSettings set(filename, QSettings::IniFormat);

    if (name) *name=set.value("name", *name).toString();

    // LOAD RELEVANT WIDGETS HERE
    bool ok=true;

    QString iid;

    {
        QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
        while (it.hasNext()) {
            it.next();
            if (it.value().config && it.value().config->isLightSourceConnected()) {
                for (int i=0; i<it.value().config->getLineCount(); i++) {
                    if (ok && set.contains(iid=QString("%2/line%1/enabled").arg(i+1).arg(it.key()))) {
                        it.value().config->setLineEnabled(i, set.value(iid).toBool());
                    }
                    if (ok && set.contains(iid=QString("%2/line%1/power").arg(i+1).arg(it.key()))) {
                        it.value().config->setLinePower(i, set.value(iid).toDouble());
                    }
                }
            }
        }
    }
    {
        QMapIterator<QString, QFShutterConfigWidget*> it(ui_shutter);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->isShutterConnected()) {
                if (ok && set.contains(iid=QString("%1/state").arg(it.key()))) {
                    it.value()->setShutter(set.value(iid).toBool());
                }
            }
        }
    }

    {
        QMapIterator<QString, QFFilterChangerConfigWidget*> it(ui_filterchangers);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->isFilterChangerConnected()) {
                if (ok && set.contains(iid=QString("%1/state").arg(it.key()))) {
                    it.value()->setFilter(set.value(iid).toInt());
                }
            }

        }
    }


    if (waiting) {
        setting_lightpath=true;
        QTime t1;
        t1.start();
        while (!lightpathLoaded(filename)) {
            if (t1.elapsed()>20) {
                QApplication::processEvents();
                t1.start();
            }
        }
        setting_lightpath=false;
    } else {
        setting_lightpath=true;
    }

}


void QFESPIMB040OpticsSetup2::lockLightpath() {
    lockLighpathCombobox();

    {
        QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
        while (it.hasNext()) {
            it.next();
            it.value().config->lockLightSource();
        }
    }
    {
        QMapIterator<QString, QFShutterConfigWidget*> it(ui_shutter);
        while (it.hasNext()) {
            it.next();
            it.value()->lockShutters();
        }
    }

    {
        QMapIterator<QString, QFFilterChangerConfigWidget*> it(ui_filterchangers);
        while (it.hasNext()) {
            it.next();
            it.value()->lockFilterChangers();
        }
    }

}

void QFESPIMB040OpticsSetup2::unlockLightpath() {
    {
        QMapIterator<QString, QFFilterChangerConfigWidget*> it(ui_filterchangers);
        while (it.hasNext()) {
            it.next();
            it.value()->unlockFilterChangers();
        }
    }
    {
        QMapIterator<QString, QFShutterConfigWidget*> it(ui_shutter);
        while (it.hasNext()) {
            it.next();
            it.value()->unlockShutters();
        }
    }
    {
        QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
        while (it.hasNext()) {
            it.next();
            it.value().config->unlockLightSource();
        }
    }
    unlockLighpathCombobox();
}


QFESPIMB040OpticsSetup2::measuredValues QFESPIMB040OpticsSetup2::getMeasuredValues() {
    QFESPIMB040OpticsSetup2::measuredValues m;

    {
        QMapIterator<QString, LightsourceWidgets> it(ui_lightsource);
        while (it.hasNext()) {
            it.next();
            if (it.value().config && it.value().config->isLightSourceConnected()) {
                for (int i=0; i<it.value().config->getLineCount(); i++) {
                    m.data[QString("%2/line%1/set_power").arg(i+1).arg(it.key())]=it.value().config->getSetPower(i);
                    m.data[QString("%2/line%1/measured_power").arg(i+1).arg(it.key())]=it.value().config->getMeasuredPower(i);
                }
            }
        }
    }
    {
        QMapIterator<QString, QFStageConfigWidget*> it(ui_stages);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->isXStageConnected()) {
                m.data[QString("%1/position").arg(it.key())]=it.value()->getXStage()->getPosition(it.value()->getXStageAxis());
                m.data[QString("%1/velocity").arg(it.key())]=it.value()->getXStage()->getSpeed(it.value()->getXStageAxis());
            }
        }
    }
    {
        QMapIterator<QString, QFESPIMB040SampleStageConfig*> it(ui_stageconfigs);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->isXStageConnected()) {
                m.data[QString("%1/xstage/position").arg(it.key())]=it.value()->getXStage()->getPosition(it.value()->getXStageAxis());
                m.data[QString("%1/xstage/velocity").arg(it.key())]=it.value()->getXStage()->getSpeed(it.value()->getXStageAxis());
            }
            if (it.value() && it.value()->isYStageConnected()) {
                m.data[QString("%1/ystage/position").arg(it.key())]=it.value()->getYStage()->getPosition(it.value()->getYStageAxis());
                m.data[QString("%1/ystage/velocity").arg(it.key())]=it.value()->getYStage()->getSpeed(it.value()->getYStageAxis());
            }
            if (it.value() && it.value()->isZStageConnected()) {
                m.data[QString("%1/zstage/position").arg(it.key())]=it.value()->getZStage()->getPosition(it.value()->getZStageAxis());
                m.data[QString("%1/zstage/velocity").arg(it.key())]=it.value()->getZStage()->getSpeed(it.value()->getZStageAxis());
            }
        }
    }
    {
        QMapIterator<QString, CameraWidgets> it(ui_cameras);
        while (it.hasNext()) {
            it.next();
            if (it.value().config && it.value().config->isChecked() && it.value().config->isCameraConnected()) {
                it.value().config->storeMeasurements(m.data, it.key()+"/");
            }
        }
    }


    return m;
}

QString QFESPIMB040OpticsSetup2::dualViewMode(int camera) const {
    QString dv="none";
    if (camera>=0 && camera<cameraIndex.size()) {
        QString dvid=ui_cameras[cameraIndex[camera]].dualview;
        if (ui_dualviews.contains(dvid)) {
            if (ui_dualviews[dvid].first->isChecked()) {
                if (ui_dualviews[dvid].second->orientation()==QF3DualViewWidget::Horizontal) dv="horizontal";
                if (ui_dualviews[dvid].second->orientation()==QF3DualViewWidget::Vertical) dv="vertical";
            }
        }
    }

    return dv;
}

QString QFESPIMB040OpticsSetup2::dualViewMode(QFExtensionCamera *ecam, int camera) const
{
    return dualViewMode(camNumFromExtension(ecam, camera));
}

int QFESPIMB040OpticsSetup2::camNumFromExtension(QFExtensionCamera *ecam, int camera) const
{
    QMapIterator<QString, CameraWidgets> it(ui_cameras);
    while (it.hasNext()) {
        it.next();
        if (it.value().config && it.value().config->cameraComboBox()->currentExtensionCamera()==ecam && it.value().config->cameraComboBox()->currentCameraID()==camera) {
            return cameraIndex.indexOf(it.key());
        }
    }
    return -1;
}

QString QFESPIMB040OpticsSetup2::getAxisNameForStage(QFExtensionLinearStage *stage, int axis)
{

    if (specialStageX.stage && specialStageX.stage->getXStage()==stage && specialStageX.device==axis) {
        return "x";
    }
    if (specialStageY.stage && specialStageY.stage->getXStage()==stage && specialStageY.device==axis) {
        return "y";
    }
    if (specialStageZ.stage && specialStageZ.stage->getXStage()==stage && specialStageZ.device==axis) {
        return "z";
    }
    if (specialStageR.stage && specialStageR.stage->getXStage()==stage && specialStageR.device==axis) {
        return "r";
    }

    if (specialStageX.stageconfig && (specialStageX.stageconfig->getXStage()==stage || specialStageX.stageconfig->getYStage()==stage || specialStageZ.stageconfig->getYStage()==stage) && specialStageX.device==axis) {
        return "x";
    }
    if (specialStageY.stageconfig && (specialStageY.stageconfig->getXStage()==stage || specialStageY.stageconfig->getYStage()==stage || specialStageY.stageconfig->getYStage()==stage) && specialStageY.device==axis) {
        return "y";
    }
    if (specialStageZ.stageconfig && (specialStageZ.stageconfig->getXStage()==stage || specialStageZ.stageconfig->getYStage()==stage || specialStageZ.stageconfig->getYStage()==stage) && specialStageZ.device==axis) {
        return "z";
    }
    if (specialStageR.stageconfig && (specialStageR.stageconfig->getXStage()==stage || specialStageR.stageconfig->getYStage()==stage || specialStageR.stageconfig->getYStage()==stage) && specialStageR.device==axis) {
        return "r";
    }



    QString id="";
    int dev=0;
    {
        QMapIterator<QString, QFStageConfigWidget*> it(ui_stages);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->getXStage()==stage && it.value()->getXStageAxis()==axis) {
                id=it.key();
                break;
            }
        }
    }
    {
        QMapIterator<QString, QFESPIMB040SampleStageConfig*> it(ui_stageconfigs);
        while (it.hasNext()) {
            it.next();
            if (it.value() && it.value()->getXStage()==stage && it.value()->getXStageAxis()==axis) {
                id=it.key();
                dev=0;
                break;
            }
            if (it.value() && it.value()->getYStage()==stage && it.value()->getYStageAxis()==axis) {
                id=it.key();
                dev=1;
                break;
            }
            if (it.value() && it.value()->getZStage()==stage && it.value()->getZStageAxis()==axis) {
                id=it.key();
                dev=2;
                break;
            }
        }
    }

    return id;
}

QString QFESPIMB040OpticsSetup2::getLaserConfig()
{
    QString r="";
    {

        for (int i=0; i<laserIndex.size(); i++) {
            QString id=laserIndex[i];
            if (ui_lightsource.contains(id)) {
                LightsourceWidgets wid=ui_lightsource[id];
                if (wid.config && wid.config->isLightSourceConnected()) {
                    QFShutterConfigWidget* shut=ui_shutter.value(wid.shutter, NULL);
                    if (!shut || (shut && shut->isShutterConnected() && shut->getShutterState())) {
                        QString l=wid.config->getLightsoureConfigForFilename();
                        if (!l.isEmpty()) {
                            if (!r.isEmpty()) r+="_";
                            r+=QString("%1_").arg(id)+cleanStringForFilename(l);
                        }
                    }
                }
            }
        }
    }

    return r;
}

void QFESPIMB040OpticsSetup2::on_btnLoadSetup_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ExperimentDescription/lastopticssetupdir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/").toString();
    QString filename=qfGetOpenFileName(this, tr("open optics setup ..."), dir, tr("optics setup configuration (*.osi)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        QFManyFilesSettings settings;
        settings.addSettings(&set, false);
        loadSettings(settings, "optics_setup/");
        loadPluginGlobalSettings(set, "plugin_global/");
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lastopticssetupdir", dir);

}

void QFESPIMB040OpticsSetup2::on_btnSaveSetup_clicked()  {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ExperimentDescription/lastopticssetupdir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/").toString();
    QString filename=qfGetSaveFileName(this, tr("save optics setup ..."), dir, tr("optics setup configuration (*.osi)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save optics setup ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);
            QFManyFilesSettings settings;
            settings.addSettings(&set, false);
            storeSettings(settings, "optics_setup/");
            storePluginGlobalSettings(set, "plugin_global/");
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lastopticssetupdir", dir);

}


int QFESPIMB040OpticsSetup2::getBrightfieldLightSourceCount() const
{
    return lightsourceIndex.size();
}

QString QFESPIMB040OpticsSetup2::getLaserName(int laser) const
{
    return laserIndex.value(laser, tr("laser %1").arg(laser));
}

QString QFESPIMB040OpticsSetup2::getBrightfieldLightSourceName(int lightsource) const
{
    QString id=brightfieldIndex.value(lightsource, tr("brightfield %1").arg(lightsource));
    return id;
}
