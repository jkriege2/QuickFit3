#include "qfespimb040opticssetup2.h"
#include "objectives.h"
#include "ui_qfespimb040opticssetup2.h"
#include "objectives.h"
#include "filters.h"
#include "jkiniparser2.h"
#include "qfespimb040shortcutconfigdialog.h"
#include "qfespimb040lightpathsavedialog.h"




template <class T>
void loadValueSettingsForAllInMap(T map_begin, T map_end, QSettings& settings, const QString& prefix) {
    for (T it=map_begin; it!=map_end; it++) {
        it.value()->loadSettings(settings, prefix+it.key()+"/");
    }
}

template <class T>
void saveValueSettingsForAllInMap(T map_begin, T map_end, QSettings& settings, const QString& prefix) {
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

template <class T>
void loadValueSettingsForAllInMap(const T& map, QSettings& settings, const QString& prefix) {
    loadValueSettingsForAllInMap(map.constBegin(), map.constEnd(), settings, prefix);
}
template <class T>
void saveValueSettingsForAllInMap(const T& map, QSettings& settings, const QString& prefix) {
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
    shutterTransmission=NULL;
    shutterLaser2=NULL;
    shutterLaser1=NULL;
    objProjection=NULL;
    objDetection=NULL;

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
                           QGridLayout* wgl=new QGridLayout(w);
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
                           QGridLayout* wgl=new QGridLayout(w);
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
                           QTabWidget* w=new QTabWidget(this);
                           QGridLayout* wgl=new QGridLayout(w);
                           widNew=w;
                           ui_tabs[id]=w;
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                       } else if (type=="tabpage") {

                           if (inwidget.isEmpty() || !ui_tabs.contains(inwidget)) {
                               m_log->log_error(tr("\nERROR reading optSetup-file '%1':\n    %3-item '%2' parent '%4' does not exist. Ignoring element!\n").arg(filename).arg(id).arg(type).arg(inwidget));
                               ok=false;

                           } else {

                               QWidget* w=new QWidget(this);
                               QGridLayout* wgl=new QGridLayout(w);
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
                                   connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].filters.append(id);
                           }
                           ui_filters[id]=w;
                       } else if (type=="camera") {
                           QFESPIMB040SimpleCameraConfig* w=new QFESPIMB040SimpleCameraConfig(this);
                           widNew=w;
                           w->init(ui_cameras.size(), m_pluginServices, m_pluginServices->getGlobalConfigFileDirectory(), this);
                           w->setLog(m_log);
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);
                           connect(w, SIGNAL(clicked(bool)), this, SLOT(changeDVenabledState()));

                           connect(addShortCut(QString("cam%1_acquire_single").arg(ui_cameras.size()), QString("camera %1: acquire single frame").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(previewSingle()));
                           connect(addShortCut(QString("cam%1_acquire_continuous_toggle").arg(ui_cameras.size()), QString("camera %1: toggle preview acquisition").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(startStopPreview()));
                           connect(addShortCut(QString("cam%1_acquire_continuous_stop").arg(ui_cameras.size()), QString("camera %1: stop preview").arg(ui_cameras.size())), SIGNAL(activated()), w, SLOT(stopPreview()));


                           ui_cameras[id].config=w;
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

                           connect(addShortCut(QString("stage%1_x2").arg(ui_stages.size()), QString("stage %1: joystick speed x2").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedX2()));
                           connect(addShortCut(QString("stage%1_x10").arg(ui_stages.size()), QString("stage %1: joystick speed x10").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedX10()));
                           connect(addShortCut(QString("stage%1_d2").arg(ui_stages.size()), QString("stage %1: joystick speed /2").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedD2()));
                           connect(addShortCut(QString("stage%1_d10").arg(ui_stages.size()), QString("stage %1: joystick speed /10").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(speedD10()));
                           connect(addShortCut(QString("stage%1_step").arg(ui_stages.size()), QString("stage %1: step x").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(stepX()));
                           connect(addShortCut(QString("stage%1_stepm").arg(ui_stages.size()), QString("stage %1: step -x").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(stepMinusX()));
                           connect(addShortCut(QString("stage%1_joysticktoggle").arg(ui_stages.size()), QString("stage %1: toggle joystick").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(toggleJoystick()));
                           connect(addShortCut(QString("stage%1_joystick_on").arg(ui_stages.size()), QString("stage %1: joystick on").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(joystickOn()));
                           connect(addShortCut(QString("stage%1_joystick_off").arg(ui_stages.size()), QString("stage %1: joystick off").arg(ui_stages.size())), SIGNAL(activated()), w, SLOT(joystickOff()));


                                   ui_stages[id]=w;
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
                                   connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].filters.append(id);
                           }

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
                       } else if (type=="stages_xyz") {
                           QFESPIMB040SampleStageConfig* w=new QFESPIMB040SampleStageConfig(this);
                           widNew=w;
                           w->init(m_log, m_pluginServices);
                           w->setLog(m_log);
                           if (!title.isEmpty()) w->setTitle(title);
                           ingroupLayout->addWidget(w, y,x, rowSpan, colSpan);

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


                           ui_stageconfigs[id]=w;
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
                               shutterLaser1=w;
                           }
                           if (special_role.contains("laser2_shutter")) {
                               shutterLaser2=w;
                           }
                           if (special_role.contains("transmission_shutter")) {
                               shutterTransmission=w;
                           }
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by)) {
                                   ui_cameras[used_by].shutter=id;
                                   connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), l, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                               if (ui_lightsource.contains(used_by)) ui_lightsource[used_by].shutter=id;
                           }
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
                                   if (l) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), l, SLOT(setEnabled(bool)));
                                   if (cb) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), cb, SLOT(setEnabled(bool)));
                                   w->setEnabled(ui_cameras[used_by].config->isChecked());
                               }
                           }
                           ui_dualviews[id]=qMakePair(cb,w);
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
                               objDetection=w;
                           } else if (special_role=="projection_objective") {
                               objProjection=w;
                           }
                           if (!used_by.isEmpty()) {
                               if (ui_cameras.contains(used_by))  {
                                   if (subtype=="tubelens") {
                                       ui_cameras[used_by].tubelens=id;
                                       connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), w, SLOT(setEnabled(bool)));
                                       if (l) connect(ui_cameras[used_by].config, SIGNAL(clicked(bool)), l, SLOT(setEnabled(bool)));
                                       w->setEnabled(ui_cameras[used_by].config->isChecked());
                                   }
                               }
                           }
                       } else if (type=="horizontal_stretch") {
                           QSpacerItem* w=new QSpacerItem(width,height,QSizePolicy::Expanding,QSizePolicy::Minimum);
                           //w->setObjectName(id);
                           ingroupLayout->addItem(w, y,x, rowSpan, colSpan);
                       } else if (type=="vertical_stretch") {
                           QSpacerItem* w=new QSpacerItem(width,height,QSizePolicy::Minimum,QSizePolicy::Expanding);
                           //w->setObjectName(id);
                           ingroupLayout->addItem(w, y,x, rowSpan, colSpan);
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
void QFESPIMB040OpticsSetup2::loadSettings(QSettings& settings, QString prefix) {
    bool updt=updatesEnabled();
    if (updt) setUpdatesEnabled(false);

    loadValueSettingsForAllInMap(ui_filters, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_objectives, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_dualviews, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_cameras, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_stageconfigs, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_stages, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_filterchangers, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_shutter, settings, prefix+"optSetup2/");
    loadValueSettingsForAllInMap(ui_lightsource, settings, prefix+"optSetup2/");


    /*ui->camConfig1->loadSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->loadSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->loadSettings(settings, prefix+"stages/");
    ui->filtDetection11->loadSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->loadSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->loadSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->loadSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->loadSettings(settings, prefix+"filters/detection");

    ui->filtDualView1Long->loadSettings(settings, prefix+"filters/dv1_long");
    ui->filtDualView1Short->loadSettings(settings, prefix+"filters/dv1_short");
    ui->filtDualView1Splitter->loadSettings(settings, prefix+"filters/dv1_splitter");
    ui->filtDualView2Long->loadSettings(settings, prefix+"filters/dv2_long");
    ui->filtDualView2Short->loadSettings(settings, prefix+"filters/dv2_short");
    ui->filtDualView2Splitter->loadSettings(settings, prefix+"filters/dv2_splitter");
    ui->chkDualView1->setChecked(settings.value(prefix+"filters/dv1_enabled", false).toBool());
    ui->chkDualView2->setChecked(settings.value(prefix+"filters/dv2_enabled", false).toBool());
    ui->cmbDualViewOrientation->setCurrentIndex(settings.value(prefix+"filters/dv1_orientation", 0).toInt());
    ui->cmbDualView2Orientation->setCurrentIndex(settings.value(prefix+"filters/dv2_orientation", 0).toInt());

    ui->objDetection->loadSettings(settings, prefix+"objectives/detection");
    ui->objProjection->loadSettings(settings, prefix+"objectives/projection");
    ui->objTube1->loadSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->loadSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->loadSettings(settings, prefix+"main_illumination_shutter");
    ui->shutterLaser1->loadSettings(settings, prefix+"laser1_shutter");
    ui->shutterLaser2->loadSettings(settings, prefix+"laser2_shutter");
    ui->shutterTransmission->loadSettings(settings, prefix+"transmission_shutter");
    ui->lsLaser1->loadSettings(settings, prefix+"lightsource_laser1");
    ui->lsLaser2->loadSettings(settings, prefix+"lightsource_laser2");
    ui->lsTransmission->loadSettings(settings, prefix+"lightsource_transmission");
    ui->filtcDetection->loadSettings(settings, prefix+"filterchanger_detection");
    ui->chkDetectionFilterWheel->setChecked(settings.value(prefix+"filterchanger_detection", false).toBool());*/

    for (int i=0; i<shortcuts.size(); i++) {
        QKeySequence seq(settings.value(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString()).toString());
        shortcuts[i].shortcut->setKey(seq);
        shortcuts[i].shortcut->setEnabled(!seq.isEmpty());
    }
    if (updt) setUpdatesEnabled(updt);
}

void QFESPIMB040OpticsSetup2::storeSettings(QSettings& settings, QString prefix) {

    saveValueSettingsForAllInMap(ui_filters, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_objectives, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_dualviews, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_cameras, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_stageconfigs, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_stages, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_filterchangers, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_shutter, settings, prefix+"optSetup2/");
    saveValueSettingsForAllInMap(ui_lightsource, settings, prefix+"optSetup2/");


    /*ui->camConfig1->storeSettings(settings, prefix+"cam_config1/");
    ui->camConfig2->storeSettings(settings, prefix+"cam_config2/");
    ui->stageSetup->storeSettings(settings, prefix+"stages/");
    ui->filtDetection11->saveSettings(settings, prefix+"filters/detection11");
    ui->filtDetection21->saveSettings(settings, prefix+"filters/detection21");
    ui->filtSplitter->saveSettings(settings, prefix+"filters/detection_splitter");
    ui->filtTransmission->saveSettings(settings, prefix+"filters/illumination_transmission");
    ui->filtDetection->saveSettings(settings, prefix+"filters/detection");
    ui->objDetection->saveSettings(settings, prefix+"objectives/detection");
    ui->objProjection->saveSettings(settings, prefix+"objectives/projection");
    ui->objTube1->saveSettings(settings, prefix+"objectives/tubelens1");
    ui->objTube2->saveSettings(settings, prefix+"objectives/tubelens2");
    ui->shutterMainIllumination->saveSettings(settings, prefix+"main_illumination_shutter");
    ui->shutterLaser1->saveSettings(settings, prefix+"laser1_shutter");
    ui->shutterLaser2->saveSettings(settings, prefix+"laser2_shutter");
    ui->filtcDetection->saveSettings(settings, prefix+"filterchanger_detection");
    ui->shutterTransmission->saveSettings(settings, prefix+"transmission_shutter");
    ui->lsLaser1->saveSettings(settings, prefix+"lightsource_laser1");
    ui->lsLaser2->saveSettings(settings, prefix+"lightsource_laser2");
    ui->lsTransmission->saveSettings(settings, prefix+"lightsource_transmission");
    ui->filtDualView1Long->saveSettings(settings, prefix+"filters/dv1_long");
    ui->filtDualView1Short->saveSettings(settings, prefix+"filters/dv1_short");
    ui->filtDualView1Splitter->saveSettings(settings, prefix+"filters/dv1_splitter");
    ui->filtDualView2Long->saveSettings(settings, prefix+"filters/dv2_long");
    ui->filtDualView2Short->saveSettings(settings, prefix+"filters/dv2_short");
    ui->filtDualView2Splitter->saveSettings(settings, prefix+"filters/dv2_splitter");
    settings.setValue(prefix+"filters/dv1_enabled", ui->chkDualView1->isChecked());
    settings.setValue(prefix+"filters/dv2_enabled", ui->chkDualView2->isChecked());
    settings.setValue(prefix+"filters/dv1_orientation", ui->cmbDualViewOrientation->currentIndex());
    settings.setValue(prefix+"filters/dv2_orientation", ui->cmbDualView2Orientation->currentIndex());

    settings.setValue(prefix+"filterchanger_detection", ui->chkDetectionFilterWheel->isChecked());*/

    for (int i=0; i<shortcuts.size(); i++) {
        settings.setValue(prefix+"shortcut_"+shortcuts[i].id, shortcuts[i].shortcut->key().toString());
    }
}

QWidget *QFESPIMB040OpticsSetup2::takeLightpathWidget() const
{
    QLayoutItem* it= ui->layHTop->takeAt(ui->layHTop->indexOf(ui->frmLightpath));
    if (it) {
        return it->widget();
    }
    return NULL;
}

void QFESPIMB040OpticsSetup2::loadPluginGlobalSettings(QSettings &settings, QString prefix) {
    /*loadPluginGlobalSettings(settings, ui->lsLaser1->getLightSourceExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->lsLaser2->getLightSourceExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->lsTransmission->getLightSourceExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->shutterLaser1->getShutterExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->shutterLaser2->getShutterExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->shutterMainIllumination->getShutterExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->shutterTransmission->getShutterExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->filtcDetection->getFilterChangerExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->camConfig1->cameraComboBox()->currentCameraQObject(), prefix);
    loadPluginGlobalSettings(settings, ui->camConfig2->cameraComboBox()->currentCameraQObject(), prefix);
    loadPluginGlobalSettings(settings, ui->stageSetup->getXStageExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->stageSetup->getYStageExtensionObject(), prefix);
    loadPluginGlobalSettings(settings, ui->stageSetup->getZStageExtensionObject(), prefix);*/
}

void QFESPIMB040OpticsSetup2::storePluginGlobalSettings(QSettings &settings, QString prefix) const {
    /*storePluginGlobalSettings(settings, ui->lsLaser1->getLightSourceExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->lsLaser2->getLightSourceExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->lsTransmission->getLightSourceExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->shutterLaser1->getShutterExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->shutterLaser2->getShutterExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->shutterMainIllumination->getShutterExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->shutterTransmission->getShutterExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->filtcDetection->getFilterChangerExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->camConfig1->cameraComboBox()->currentCameraQObject(), prefix);
    storePluginGlobalSettings(settings, ui->camConfig2->cameraComboBox()->currentCameraQObject(), prefix);
    storePluginGlobalSettings(settings, ui->stageSetup->getXStageExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->stageSetup->getYStageExtensionObject(), prefix);
    storePluginGlobalSettings(settings, ui->stageSetup->getZStageExtensionObject(), prefix);*/
}


double QFESPIMB040OpticsSetup2::getCameraMagnification(int setup_cam) const {
    /*if (setup_cam==0) {
        return ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    } else if (setup_cam==1) {
        return ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    }*/
    return 1;
}

QMap<QString, QVariant> QFESPIMB040OpticsSetup2::getSetup(int setup_cam) const {
    QMap<QString, QVariant> setup;

    /*FilterDescription filter=ui->filtDetection->filter();
    if (filter.isValid) {
        setup["filters/detection/name"]=filter.name;
        setup["filters/detection/type"]=filter.type;
        setup["filters/detection/manufacturer"]=filter.manufacturer;
    }

    if (ui->chkDetectionFilterWheel->isChecked()) {
        FilterDescription filter=ui->filtcDetection->getCurrentFilterDescription();
        if (filter.isValid) {
            setup["filters/detection_filterchanger/name"]=filter.name;
            setup["filters/detection_filterchanger/type"]=filter.type;
            setup["filters/detection_filterchanger/manufacturer"]=filter.manufacturer;
        }
    }

    if (setup_cam<0 || setup_cam==0) {
        filter=ui->filtDetection11->filter();
        if (filter.isValid) {
            setup["filters/detection_cam1/name"]=filter.name;
            setup["filters/detection_cam1/type"]=filter.type;
            setup["filters/detection_cam1/manufacturer"]=filter.manufacturer;
        }
        if (ui->chkDualView1->isChecked()) {
            if (ui->cmbDualViewOrientation->currentIndex()==0) setup["filters/dualview_cam1/orientation"]=QString("horicontal");
            else setup["filters/dualview_cam1/orientation"]=QString("vertical");
            filter=ui->filtDualView1Long->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam1/detection_long/name"]=filter.name;
                setup["filters/dualview_cam1/detection_long/type"]=filter.type;
                setup["filters/dualview_cam1/detection_long/manufacturer"]=filter.manufacturer;
            }
            filter=ui->filtDualView1Short->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam1/detection_short/name"]=filter.name;
                setup["filters/dualview_cam1/detection_short/type"]=filter.type;
                setup["filters/dualview_cam1/detection_short/manufacturer"]=filter.manufacturer;
            }
            filter=ui->filtDualView1Splitter->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam1/detection_splitter/name"]=filter.name;
                setup["filters/dualview_cam1/detection_splitter/type"]=filter.type;
                setup["filters/dualview_cam1/detection_splitter/manufacturer"]=filter.manufacturer;
            }
        }
    }

    if (setup_cam<0 || setup_cam==1) {
        filter=ui->filtDetection21->filter();
        if (filter.isValid) {
            setup["filters/detection_cam2/name"]=filter.name;
            setup["filters/detection_cam2/type"]=filter.type;
            setup["filters/detection_cam2/manufacturer"]=filter.manufacturer;
        }
        if (ui->chkDualView2->isChecked()) {
            if (ui->cmbDualView2Orientation->currentIndex()==0) setup["filters/dualview_cam2/orientation"]=QString("horicontal");
            else setup["filters/dualview_cam2/orientation"]=QString("vertical");
            filter=ui->filtDualView2Long->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam2/detection_long/name"]=filter.name;
                setup["filters/dualview_cam2/detection_long/type"]=filter.type;
                setup["filters/dualview_cam2/detection_long/manufacturer"]=filter.manufacturer;
            }
            filter=ui->filtDualView2Short->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam2/detection_short/name"]=filter.name;
                setup["filters/dualview_cam2/detection_short/type"]=filter.type;
                setup["filters/dualview_cam2/detection_short/manufacturer"]=filter.manufacturer;
            }
            filter=ui->filtDualView2Splitter->filter();
            if (filter.isValid) {
                setup["filters/dualview_cam2/detection_splitter/name"]=filter.name;
                setup["filters/dualview_cam2/detection_splitter/type"]=filter.type;
                setup["filters/dualview_cam2/detection_splitter/manufacturer"]=filter.manufacturer;
            }
        }
    }

    filter=ui->filtSplitter->filter();
    if (filter.isValid) {
        setup["filters/detection_splitter/name"]=filter.name;
        setup["filters/detection_splitter/type"]=filter.type;
        setup["filters/detection_splitter/manufacturer"]=filter.manufacturer;
    }

    filter=ui->filtTransmission->filter();
    if (filter.isValid) {
        setup["filters/illumination_transmission/name"]=filter.name;
        setup["filters/illumination_transmission/type"]=filter.type;
        setup["filters/illumination_transmission/manufacturer"]=filter.manufacturer;
    }

    ObjectiveDescription objective=ui->objDetection->objective();
    setup["objectives/detection/name"]=objective.name;
    setup["objectives/detection/manufacturer"]=objective.manufacturer;
    setup["objectives/detection/NA"]=objective.NA;
    setup["objectives/detection/magnification"]=objective.magnification;

    objective=ui->objProjection->objective();
    setup["objectives/projection/name"]=objective.name;
    setup["objectives/projection/manufacturer"]=objective.manufacturer;
    setup["objectives/projection/NA"]=objective.NA;
    setup["objectives/projection/magnification"]=objective.magnification;

    if (setup_cam<0 || setup_cam==0) {
        objective=ui->objTube1->objective();
        setup["objectives/tube_lens1/name"]=objective.name;
        setup["objectives/tube_lens1/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens1/NA"]=objective.NA;
        setup["objectives/tube_lens1/magnification"]=objective.magnification;
    }

    if (setup_cam<0 || setup_cam==1) {
        objective=ui->objTube2->objective();
        setup["objectives/tube_lens2/name"]=objective.name;
        setup["objectives/tube_lens2/manufacturer"]=objective.manufacturer;
        setup["objectives/tube_lens2/NA"]=objective.NA;
        setup["objectives/tube_lens2/magnification"]=objective.magnification;
    }

    setup["main_illumination/shutter"]=ui->shutterMainIllumination->getShutterState();
    setup["transmission_illumination/shutter"]=ui->shutterTransmission->getShutterState();
    for (int i=0; i<ui->lsTransmission->getLineCount(); i++) {
        setup[QString("transmission_illumination/line%1/name").arg(i+1)]=ui->lsTransmission->getLineDescription(i);
        setup[QString("transmission_illumination/line%1/enabled").arg(i+1)]=ui->lsTransmission->isLineEnabled(i);
        setup[QString("transmission_illumination/line%1/set_power").arg(i+1)]=ui->lsTransmission->getSetPower(i);
        setup[QString("transmission_illumination/line%1/measured_power").arg(i+1)]=ui->lsTransmission->getMeasuredPower(i);
        setup[QString("transmission_illumination/line%1/unit").arg(i+1)]=ui->lsTransmission->getLineUnit(i);
    }
    setup["laser1/shutter"]=ui->shutterLaser1->getShutterState();
    for (int i=0; i<ui->lsLaser1->getLineCount(); i++) {
        setup[QString("laser1/line%1/name").arg(i+1)]=ui->lsLaser1->getLineDescription(i);
        setup[QString("laser1/line%1/enabled").arg(i+1)]=ui->lsLaser1->isLineEnabled(i);
        setup[QString("laser1/line%1/set_power").arg(i+1)]=ui->lsLaser1->getSetPower(i);
        setup[QString("laser1/line%1/measured_power").arg(i+1)]=ui->lsLaser1->getMeasuredPower(i);
        setup[QString("laser1/line%1/unit").arg(i+1)]=ui->lsLaser1->getLineUnit(i);
    }
    setup["laser2/shutter"]=ui->shutterLaser2->getShutterState();
    for (int i=0; i<ui->lsLaser2->getLineCount(); i++) {
        setup[QString("laser2/line%1/name").arg(i+1)]=ui->lsLaser2->getLineDescription(i);
        setup[QString("laser2/line%1/enabled").arg(i+1)]=ui->lsLaser2->isLineEnabled(i);
        setup[QString("laser2/line%1/set_power").arg(i+1)]=ui->lsLaser2->getSetPower(i);
        setup[QString("laser2/line%1/measured_power").arg(i+1)]=ui->lsLaser2->getMeasuredPower(i);
        setup[QString("laser2/line%1/unit").arg(i+1)]=ui->lsLaser2->getLineUnit(i);
    }
    if (ui->chkDetectionFilterWheel->isChecked()) setup["filterchangers/detection/filter"]=ui->filtcDetection->getFilterChangerState();*/


    return setup;
}

int QFESPIMB040OpticsSetup2::getCameraCount() const
{
    return 0;
}


void QFESPIMB040OpticsSetup2::setLogging(QFPluginLogService* log) {
    m_log=log;
    /*ui->camConfig1->setLog(m_log);
    ui->camConfig2->setLog(m_log);
    ui->stageSetup->setLog(m_log);
    ui->shutterMainIllumination->setLog(m_log);
    ui->shutterLaser1->setLog(m_log);
    ui->shutterLaser2->setLog(m_log);
    ui->filtcDetection->setLog(m_log);
    ui->shutterTransmission->setLog(m_log);
    ui->lsLaser1->setLog(m_log);
    ui->lsLaser2->setLog(m_log);
    ui->lsTransmission->setLog(m_log);*/
}

bool QFESPIMB040OpticsSetup2::lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename) {
    /*switch(setup_cam) {
        case 0: return ui->camConfig1->lockCamera(extension, ecamera, camera,  previewSettingsFilename);
        case 1: return ui->camConfig2->lockCamera(extension, ecamera, camera,  previewSettingsFilename);
    }*/
    return false;
}

void QFESPIMB040OpticsSetup2::releaseCamera(int setup_cam) {
    /*switch(setup_cam) {
        case 0: ui->camConfig1->releaseCamera(); break;
        case 1: ui->camConfig2->releaseCamera(); break;
    }*/
}

void QFESPIMB040OpticsSetup2::overrideCameraPreview(int setup_cam, const QString &camera_settings, const QString &lightpath)
{
    /*if (!lightpath.isEmpty() && QFile::exists(lightpath)) {
        ui->cmbLightpathConfig->setEnabled(false);
        lockLightpath();
        loadLightpathConfig(lightpath, true);
        unlockLightpath();
    }
    if (setup_cam==0) ui->camConfig1->overridePreview(camera_settings);
    if (setup_cam==1) ui->camConfig2->overridePreview(camera_settings);
    loadLightpathConfig(lightpath, true);*/
}

void QFESPIMB040OpticsSetup2::resetCameraPreview(int setup_cam)
{

    /*if (setup_cam==0) ui->camConfig1->resetPreview();
    if (setup_cam==1) ui->camConfig2->resetPreview();
    ensureLightpath();
    ui->cmbLightpathConfig->setEnabled(true);*/
}

void QFESPIMB040OpticsSetup2::on_btnConnectDevices_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Connecting devices ..."));
    dlg->addItem(tr("shutter: main"));
    dlg->addItem(tr("shutter: laser 1"));
    dlg->addItem(tr("shutter: laser 2"));
    dlg->addItem(tr("shutter: transmission illumination"));
    dlg->addItem(tr("lightsource: laser 1"));
    dlg->addItem(tr("lightsource: laser 2"));
    dlg->addItem(tr("lightsource: transmission"));
    dlg->addItem(tr("stage"));
    dlg->addItem(tr("filterwheel: detection"));
    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

/*    if (!dlg->wasCanceled()) ui->shutterMainIllumination->connectShutter();
    dlg->nextItem((ui->shutterMainIllumination->isShutterConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->shutterLaser1->connectShutter();
    dlg->nextItem((ui->shutterLaser1->isShutterConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->shutterLaser2->connectShutter();
    dlg->nextItem((ui->shutterLaser2->isShutterConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->shutterTransmission->connectShutter();
    dlg->nextItem((ui->shutterTransmission->isShutterConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->lsLaser1->connectLightSource();
    dlg->nextItem((ui->lsLaser1->isLightSourceConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->lsLaser2->connectLightSource();
    dlg->nextItem((ui->lsLaser2->isLightSourceConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->lsTransmission->connectLightSource();
    dlg->nextItem((ui->lsTransmission->isLightSourceConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->stageSetup->connectStages();
    dlg->nextItem((ui->stageSetup->isXStageConnected()||ui->stageSetup->isYStageConnected()||ui->stageSetup->isZStageConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) if (ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->connectFilterChanger();
    dlg->nextItem((ui->filtcDetection->isFilterChangerConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));*/
    dlg->close();
    delete dlg;
}

void QFESPIMB040OpticsSetup2::on_btnDisconnectDevices_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Disconnecting devices ..."));
    dlg->addItem(tr("shutter: main"));
    dlg->addItem(tr("shutter: laser 1"));
    dlg->addItem(tr("shutter: laser 2"));
    dlg->addItem(tr("shutter: transmission illumination"));
    dlg->addItem(tr("lightsource: laser 1"));
    dlg->addItem(tr("lightsource: laser 2"));
    dlg->addItem(tr("lightsource: transmission"));
    dlg->addItem(tr("stage"));
    dlg->addItem(tr("filterwheel: detection"));
    dlg->setHasCancelButton(false);
    dlg->show();
    dlg->start();

    /*ui->shutterMainIllumination->disconnectShutter();
    dlg->nextItem();
    QApplication::processEvents();
    ui->shutterLaser1->disconnectShutter();
    dlg->nextItem();
    QApplication::processEvents();
    ui->shutterLaser2->disconnectShutter();
    dlg->nextItem();
    QApplication::processEvents();
    ui->shutterTransmission->disconnectShutter();
    dlg->nextItem();
    QApplication::processEvents();
    ui->lsLaser1->disconnectLightSource();
    dlg->nextItem();
    QApplication::processEvents();
    ui->lsLaser2->disconnectLightSource();
    dlg->nextItem();
    QApplication::processEvents();
    ui->lsTransmission->disconnectLightSource();
    dlg->nextItem();
    QApplication::processEvents();
    ui->stageSetup->disconnectStages();
    dlg->nextItem();
    QApplication::processEvents();
    if (ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->disconnectFilterChanger();
    dlg->nextItem();
    QApplication::processEvents();
    dlg->close();*/
    delete dlg;
}
void QFESPIMB040OpticsSetup2::on_btnConnectCameras_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Connecting cameras ..."));
    dlg->addItem(tr("camera 1"));
    dlg->addItem(tr("camera 2"));
    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

    /*if (!dlg->wasCanceled()) ui->camConfig1->connectCamera();
    dlg->nextItem((ui->camConfig1->isCameraConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->camConfig2->connectCamera();
    dlg->nextItem((ui->camConfig2->isCameraConnected())?(QProgressListWidget::statusDone):(QProgressListWidget::statusFailed));
    QApplication::processEvents();*/

    dlg->close();
    delete dlg;

}


void QFESPIMB040OpticsSetup2::on_btnDisconnectCameras_clicked() {
    QProgressListDialog* dlg=new QProgressListDialog(this, Qt::Dialog|Qt::WindowStaysOnBottomHint);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowIcon(windowIcon());
    dlg->setWindowTitle(tr("Disconnecting cameras ..."));
    dlg->addItem(tr("camera 1"));
    dlg->addItem(tr("camera 2"));
    dlg->setHasCancelButton(true);
    dlg->show();
    dlg->start();

    /*if (!dlg->wasCanceled()) ui->camConfig1->disconnectCamera();
    dlg->nextItem();
    QApplication::processEvents();
    if (!dlg->wasCanceled()) ui->camConfig2->disconnectCamera();
    dlg->nextItem();
    QApplication::processEvents();*/

    dlg->close();
    delete dlg;

}

void QFESPIMB040OpticsSetup2::on_chkDetectionFilterWheel_toggled(bool checked) {
    /*if (!checked) {
        ui->filtcDetection->disconnectFilterChanger();
    }*/
}

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
    /*double m=ui->objDetection->objective().magnification*ui->objTube1->objective().magnification;
    ui->camConfig1->setMagnification(m);
    m=ui->objDetection->objective().magnification*ui->objTube2->objective().magnification;
    ui->camConfig2->setMagnification(m);*/
    QMap<QString, CameraWidgets>::const_iterator i;
    for (i = ui_cameras.constBegin(); i != ui_cameras.constEnd(); ++i) {
        double m=1;
        if (objDetection) m=objDetection->objective().magnification;
        if (ui_objectives.contains(i.value().tubelens)) {
            m=m*ui_objectives[i.value().tubelens]->objective().magnification;
        }
        i.value().config->setMagnification(m);
    }
}




QFExtensionFilterChanger *QFESPIMB040OpticsSetup2::getFilterChanger(int changer) const
{
    //if (changer==FilterChangerDetection) return ui->filtcDetection->getFilterChanger();
    return NULL;
}

int QFESPIMB040OpticsSetup2::getFilterChangerID(int changer) const
{
   // if (changer==0) return ui->filtcDetection->getFilterChangerID();
    return -1;
}

QString QFESPIMB040OpticsSetup2::getFilterChangerName(int changer) const
{
    //if (changer==0) return tr("detection");
    return QString();
}

int QFESPIMB040OpticsSetup2::getFilterChangerCount() const
{
    return 1;
}

QFExtensionLightSource *QFESPIMB040OpticsSetup2::getLaser(int laser)
{
    /*if (laser==0) return ui->lsLaser1->getLightSource();
    if (laser==1) return ui->lsLaser2->getLightSource();*/
    return NULL;
}


QFExtensionLightSource *QFESPIMB040OpticsSetup2::getBrightfieldLightSource(int source)
{
    //if (source==0) return ui->lsTransmission->getLightSource();
    return NULL;
}

int QFESPIMB040OpticsSetup2::getLaserID(int laser)
{
    /*if (laser==0) return ui->lsLaser1->getLightSourceID();
    if (laser==1) return ui->lsLaser2->getLightSourceID();*/
    return NULL;
}


int QFESPIMB040OpticsSetup2::getBrightfieldLightSourceID(int source)
{
    //if (source==0) return ui->lsTransmission->getLightSourceID();
    return NULL;
}

int QFESPIMB040OpticsSetup2::getLaserCount() const
{
    return 2;
}

bool QFESPIMB040OpticsSetup2::isStageConnected(QFExtensionLinearStage* stage, int id, bool& found) {
    found=false;
    if (!stage || id<0) return false;
    found=true;
    /*if (stage==getStage(QFESPIMB040OpticsSetupBase::StageX) && id==getStageAxis(QFESPIMB040OpticsSetupBase::StageX)) return isStageConnected(QFESPIMB040OpticsSetupBase::StageX);
    if (stage==getStage(QFESPIMB040OpticsSetupBase::StageY) && id==getStageAxis(QFESPIMB040OpticsSetupBase::StageY)) return isStageConnected(QFESPIMB040OpticsSetupBase::StageY);
    if (stage==getStage(QFESPIMB040OpticsSetupBase::StageZ) && id==getStageAxis(QFESPIMB040OpticsSetupBase::StageZ)) return isStageConnected(QFESPIMB040OpticsSetupBase::StageZ);*/
    found=false;
    return false;
}

int QFESPIMB040OpticsSetup2::getStageAxis(int stage)
{
    /*if (stage==QFESPIMB040OpticsSetupBase::StageX) return ui->stageSetup->getXStageAxis();
    if (stage==QFESPIMB040OpticsSetupBase::StageY) return ui->stageSetup->getYStageAxis();
    if (stage==QFESPIMB040OpticsSetupBase::StageZ) return ui->stageSetup->getZStageAxis();*/
    return -1;

}

QString QFESPIMB040OpticsSetup2::getStageName(int stage) const
{
    /*if (stage==QFESPIMB040OpticsSetupBase::StageX) return "x";
    if (stage==QFESPIMB040OpticsSetupBase::StageY) return "y";
    if (stage==QFESPIMB040OpticsSetupBase::StageZ) return "z";*/
    return "";

}

int QFESPIMB040OpticsSetup2::getStageCount() const
{
    return 3;
}


bool QFESPIMB040OpticsSetup2::isStageConnected(int stage) const
{
    /*if (stage==QFESPIMB040OpticsSetupBase::StageX) return ui->stageSetup->isXStageConnected();
    if (stage==QFESPIMB040OpticsSetupBase::StageY) return ui->stageSetup->isYStageConnected();
    if (stage==QFESPIMB040OpticsSetupBase::StageZ) return ui->stageSetup->isZStageConnected();*/
    return false;
}


QFExtension *QFESPIMB040OpticsSetup2::getStageExtension(int stage)
{
    /*if (stage==QFESPIMB040OpticsSetupBase::StageX) return ui->stageSetup->getXStageExtension();
    if (stage==QFESPIMB040OpticsSetupBase::StageY) return ui->stageSetup->getYStageExtension();
    if (stage==QFESPIMB040OpticsSetupBase::StageZ) return ui->stageSetup->getZStageExtension();*/
    return NULL;
}

QFExtensionLinearStage *QFESPIMB040OpticsSetup2::getStage(int stage)
{
    /*if (stage==QFESPIMB040OpticsSetupBase::StageX) return ui->stageSetup->getXStage();
    if (stage==QFESPIMB040OpticsSetupBase::StageY) return ui->stageSetup->getYStage();
    if (stage==QFESPIMB040OpticsSetupBase::StageZ) return ui->stageSetup->getZStage();*/
    return NULL;
}


QFCameraComboBox* QFESPIMB040OpticsSetup2::cameraComboBox(int camera) const {
    /*if (camera==0) return ui->camConfig1->cameraComboBox();
    if (camera==1) return ui->camConfig2->cameraComboBox();*/

    return NULL;
}

QFCameraConfigComboBoxStartResume* QFESPIMB040OpticsSetup2::getStopRelease(int camera) const {
    /*if (camera==0) return ui->camConfig1;
    if (camera==1) return ui->camConfig2;*/

    return NULL;
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
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterLaser1 && shutterLaser1) {
        shutterLaser1->setShutter(opened);
        if (!opened) shutterLaser1->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (shutterLaser1->getShutterState()!=opened && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                m_log->log_error("laser 1 shutter timed out after 10s!\n");
            }
        }
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterLaser2 && shutterLaser2) {
        shutterLaser2->setShutter(opened);
        if (!opened) shutterLaser2->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (shutterLaser2->getShutterState()!=opened && t.elapsed()<10000) {
                QApplication::processEvents();
            }

            if (t.elapsed()>=10000) {
                m_log->log_error("laser 2 shutter timed out after 10s!\n");
            }
        }
    } else if (shutter==QFESPIMB040OpticsSetup2::ShutterTransmission && shutterTransmission) {
        shutterTransmission->setShutter(opened);
        if (!opened) shutterTransmission->setShutter(false);

        if (blocking) {
            QTime t;
            t.start();
            while (shutterTransmission->getShutterState()!=opened && t.elapsed()<10000) {
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




























void QFESPIMB040OpticsSetup2::saveLightpathConfig(QMap<QString, QVariant> &data, const QString &name, const QString& prefix,  QList<bool> saveProp, bool saveMeasured) {
    data[prefix+"name"]=name;

    //qDebug()<<saveProp;

    // SAVE RELEVANT WIDGETS HERE
    /*if (saveProp.value(0, true) && ui->lsTransmission->getLightSource() && ui->lsTransmission->isLightSourceConnected()) {
        for (int i=0; i<ui->lsTransmission->getLineCount(); i++) {
            data[prefix+QString("transmission/line%1/enabled").arg(i+1)]=ui->lsTransmission->isLineEnabled(i);
            data[prefix+QString("transmission/line%1/power").arg(i+1)]=ui->lsTransmission->getSetPower(i);
            if (saveMeasured) {
                data[prefix+QString("transmission/line%1/measured_power").arg(i+1)]=ui->lsTransmission->getMeasuredPower(i);
            }
        }
    }
    if (saveProp.value(1, true) && ui->shutterTransmission->getShutter() && ui->shutterTransmission->getShutter()->isShutterConnected(ui->shutterTransmission->getShutterID())) {
        data[prefix+"transmission/shutter/state"]=ui->shutterTransmission->getShutterState();
    }
    if (saveProp.value(2, true) && ui->lsLaser1->getLightSource() && ui->lsLaser1->isLightSourceConnected()) {
        for (int i=0; i<ui->lsLaser1->getLineCount(); i++) {
            data[prefix+QString("laser1/line%1/enabled").arg(i+1)]=ui->lsLaser1->isLineEnabled(i);
            data[prefix+QString("laser1/line%1/power").arg(i+1)]=ui->lsLaser1->getSetPower(i);
            if (saveMeasured) {
                data[prefix+QString("laser1/line%1/measured_power").arg(i+1)]=ui->lsLaser1->getMeasuredPower(i);
            }
        }
    }
    if (saveProp.value(3, true) && ui->shutterLaser1->getShutter() && ui->shutterLaser1->getShutter()->isShutterConnected(ui->shutterLaser1->getShutterID())) {
        data[prefix+"laser1/shutter/state"]=ui->shutterLaser1->getShutterState();
    }
    if (saveProp.value(4, true) && ui->lsLaser2->getLightSource() && ui->lsLaser2->isLightSourceConnected()) {
        for (int i=0; i<ui->lsLaser2->getLineCount(); i++) {
            data[prefix+QString("laser2/line%1/enabled").arg(i+1)]=ui->lsLaser2->isLineEnabled(i);
            data[prefix+QString("laser2/line%1/power").arg(i+1)]=ui->lsLaser2->getSetPower(i);
            if (saveMeasured) {
                data[prefix+QString("laser2/line%1/measured_power").arg(i+1)]=ui->lsLaser2->getMeasuredPower(i);
            }
        }
    }
    if (saveProp.value(5, true) && ui->shutterLaser2->getShutter() && ui->shutterLaser2->getShutter()->isShutterConnected(ui->shutterLaser2->getShutterID())) {
        data[prefix+"laser2/shutter/state"]=ui->shutterLaser2->getShutterState();
    }
    if (saveProp.value(6, true) && ui->chkDetectionFilterWheel->isChecked() && ui->filtcDetection->getFilterChanger() && ui->filtcDetection->isFilterChangerConnected()) {
        data[prefix+"detection/filterchanger/filter"]=ui->filtcDetection->getFilterChangerState();
        if (saveMeasured) {
            data[prefix+"detection/filterchanger/filter_name"]=ui->filtcDetection->getCurrentFilter();
            data[prefix+"detection/filterchanger/filter_manufacturer"]=ui->filtcDetection->getCurrentFilterDescription().manufacturer;
            data[prefix+"detection/filterchanger/filter_type"]=ui->filtcDetection->getCurrentFilterDescription().type;
        }
    }*/

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

void QFESPIMB040OpticsSetup2::saveCurrentLightpatConfig() {
    bool dlgOK=true;
    QString name=ui->cmbLightpathConfig->currentText();

    QStringList lightpathlist;
    lightpathlist<<tr("transmission illumintaion: source");
    lightpathlist<<tr("transmission illumintaion: shutter");
    lightpathlist<<tr("laser 1: source");
    lightpathlist<<tr("laser 1: shutter");
    lightpathlist<<tr("laser 2: source");
    lightpathlist<<tr("laser 2: shutter");
    lightpathlist<<tr("detection: filter changer");

    lightpathConfigModel->setReadonly(false);
    lightpathConfigModel->resize(lightpathlist.size(), 2);
    lightpathConfigModel->setColumnTitle(1, tr("item"));
    for (int i=0; i<lightpathlist.size(); i++) {
        lightpathConfigModel->setCellCheckedRole(i, 0, Qt::Checked);
        lightpathConfigModel->setCell(i, 1, lightpathlist[i]);
    }

    /*lightpathConfigModel->setCellCheckedRole(0, 0, (ui->lsTransmission->isLightSourceConnected())?Qt::Checked:Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(1, 0, (ui->shutterTransmission->isShutterConnected())?Qt::Checked:Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(2, 0, Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(3, 0, (ui->shutterLaser1->isShutterConnected())?Qt::Checked:Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(4, 0, Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(5, 0, (ui->shutterLaser2->isShutterConnected())?Qt::Checked:Qt::Unchecked);
    lightpathConfigModel->setCellCheckedRole(6, 0, (ui->chkDetectionFilterWheel->isChecked()&&ui->filtcDetection->isFilterChangerConnected())?Qt::Checked:Qt::Unchecked);
    lightpathConfigModel->setReadonlyButStillCheckable(true);
*/

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
                        saveLightpathConfig(filename, name, dlg->getCheckedItems());
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
        /*QSettings set(filename, QSettings::IniFormat);
        if (ok && set.contains("laser1/shutter/state")) ok=ok&&ui->shutterLaser1->isShutterDone()&&(ui->shutterLaser1->getShutterState()==set.value("laser1/shutter/state").toBool());
        if (ok && set.contains("laser2/shutter/state")) ok=ok&&ui->shutterLaser2->isShutterDone()&&(ui->shutterLaser2->getShutterState()==set.value("laser2/shutter/state").toBool());
        if (ok && set.contains("transmission/shutter/state")) ok=ok&&ui->shutterTransmission->isShutterDone()&&(ui->shutterTransmission->getShutterState()==set.value("transmission/shutter/state").toBool());
        if (ui->lsTransmission->isLightSourceConnected()) {
            for (int i=0; i<ui->lsTransmission->getLineCount(); i++) {
                if (ok && set.contains(QString("transmission/line%1/enabled").arg(i+1))) ok=ok&&ui->lsTransmission->isLastActionComplete()&&(ui->lsTransmission->isLineEnabled(i)==set.value(QString("transmission/line%1/enabled").arg(i+1)).toBool());
                if (ok && set.contains(QString("transmission/line%1/power").arg(i+1))) ok=ok&&ui->lsTransmission->isLastActionComplete();
            }
        }
        if (ui->lsLaser1->isLightSourceConnected()) {
            for (int i=0; i<ui->lsLaser1->getLineCount(); i++) {
                if (ok && set.contains(QString("laser1/line%1/enabled").arg(i+1))) ok=ok&&ui->lsLaser1->isLastActionComplete()&&(ui->lsLaser1->isLineEnabled(i)==set.value(QString("laser1/line%1/enabled").arg(i+1)).toBool());
                if (ok && set.contains(QString("laser1/line%1/power").arg(i+1))) ok=ok&&ui->lsLaser1->isLastActionComplete();
            }
        }
        if (ui->lsLaser2->isLightSourceConnected()) {
            for (int i=0; i<ui->lsLaser2->getLineCount(); i++) {
                if (ok && set.contains(QString("laser2/line%1/enabled").arg(i+1))) ok=ok&&ui->lsLaser2->isLastActionComplete()&&(ui->lsLaser2->isLineEnabled(i)==set.value(QString("laser2/line%1/enabled").arg(i+1)).toBool());
                if (ok && set.contains(QString("laser2/line%1/power").arg(i+1))) ok=ok&&ui->lsLaser2->isLastActionComplete();
            }
        }
        if (ok && set.contains("detection/filterchanger/filter") && ui->chkDetectionFilterWheel->isChecked())
            ok=ok&&ui->filtcDetection->isLastActionComplete()&&(ui->filtcDetection->getFilterChangerState()==set.value("detection/filterchanger/filter").toInt());
            */
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
    /*if (set.contains("laser1/shutter/state")) ui->shutterLaser1->setShutter(set.value("laser1/shutter/state").toBool());
    if (set.contains("laser2/shutter/state")) ui->shutterLaser2->setShutter(set.value("laser2/shutter/state").toBool());
    if (set.contains("transmission/shutter/state")) ui->shutterTransmission->setShutter(set.value("transmission/shutter/state").toBool());
    if (set.contains("detection/filterchanger/filter") && ui->chkDetectionFilterWheel->isChecked()) ui->filtcDetection->setFilterChanger(set.value("detection/filterchanger/filter").toInt());
    if (ui->lsTransmission->isLightSourceConnected()) {
        for (int i=0; i<ui->lsTransmission->getLineCount(); i++) {
            if (set.contains(QString("transmission/line%1/enabled").arg(i+1))) ui->lsTransmission->setLineEnabled(i, set.value(QString("transmission/line%1/enabled").arg(i+1)).toBool());
            if (set.contains(QString("transmission/line%1/power").arg(i+1))) ui->lsTransmission->setLinePower(i, set.value(QString("transmission/line%1/power").arg(i+1)).toDouble());
        }
    }
    if (ui->lsLaser1->isLightSourceConnected()) {
        for (int i=0; i<ui->lsLaser1->getLineCount(); i++) {
            if (set.contains(QString("laser1/line%1/enabled").arg(i+1))) ui->lsLaser1->setLineEnabled(i, set.value(QString("laser1/line%1/enabled").arg(i+1)).toBool());
            if (set.contains(QString("laser1/line%1/power").arg(i+1))) ui->lsLaser1->setLinePower(i, set.value(QString("laser1/line%1/power").arg(i+1)).toDouble());
        }
    }
    if (ui->lsLaser2->isLightSourceConnected()) {
        for (int i=0; i<ui->lsLaser2->getLineCount(); i++) {
            if (set.contains(QString("laser2/line%1/enabled").arg(i+1))) ui->lsLaser2->setLineEnabled(i, set.value(QString("laser2/line%1/enabled").arg(i+1)).toBool());
            if (set.contains(QString("laser2/line%1/power").arg(i+1))) ui->lsLaser2->setLinePower(i, set.value(QString("laser2/line%1/power").arg(i+1)).toDouble());
        }
    }*/
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
   //qDebug()<<"locking lighpath 1";
    /*ui->shutterLaser1->lockShutters();
   //qDebug()<<"locking lighpath 2";
    ui->shutterLaser2->lockShutters();
   //qDebug()<<"locking lighpath 3";
    ui->shutterTransmission->lockShutters();
   //qDebug()<<"locking lighpath 4";
    ui->lsLaser1->lockLightSource();
   //qDebug()<<"locking lighpath 5";
    ui->lsLaser2->lockLightSource();
   //qDebug()<<"locking lighpath 6";
    ui->lsTransmission->lockLightSource();
   //qDebug()<<"locking lighpath 7";
    ui->filtcDetection->lockFilterChangers();
   //qDebug()<<"locking lighpath done";
   */
}

void QFESPIMB040OpticsSetup2::unlockLightpath() {
   //qDebug()<<"unlocking lightpath";
    /*ui->shutterLaser1->unlockShutters();
    ui->shutterLaser2->unlockShutters();
    ui->shutterTransmission->unlockShutters();
    ui->lsLaser1->unlockLightSource();
    ui->lsLaser2->unlockLightSource();
    ui->lsTransmission->unlockLightSource();
    ui->filtcDetection->unlockFilterChangers();*/
    unlockLighpathCombobox();
   //qDebug()<<"unlocking lightpath done";
}


QFESPIMB040OpticsSetup2::measuredValues QFESPIMB040OpticsSetup2::getMeasuredValues() {
    QFESPIMB040OpticsSetup2::measuredValues m;
    /*for (int i=0; i<ui->lsTransmission->getLineCount(); i++) {
        m.data[QString("transmission/line%1/set_power").arg(i+1)]=ui->lsTransmission->getSetPower(i);
        m.data[QString("transmission/line%1/measured_power").arg(i+1)]=ui->lsTransmission->getMeasuredPower(i);
    }

    for (int i=0; i<ui->lsLaser1->getLineCount(); i++) {
        m.data[QString("laser1/line%1/set_power").arg(i+1)]=ui->lsLaser1->getSetPower(i);
        m.data[QString("laser1/line%1/measured_power").arg(i+1)]=ui->lsLaser1->getMeasuredPower(i);
    }

    for (int i=0; i<ui->lsLaser2->getLineCount(); i++) {
        m.data[QString("laser2/line%1/set_power").arg(i+1)]=ui->lsLaser2->getSetPower(i);
        m.data[QString("laser2/line%1/measured_power").arg(i+1)]=ui->lsLaser2->getMeasuredPower(i);
    }

    m.data["stagex/position"]=ui->stageSetup->getXStage()->getPosition(ui->stageSetup->getXStageAxis());
    m.data["stagex/velocity"]=ui->stageSetup->getXStage()->getSpeed(ui->stageSetup->getXStageAxis());
    m.data["stagey/position"]=ui->stageSetup->getYStage()->getPosition(ui->stageSetup->getYStageAxis());
    m.data["stagey/velocity"]=ui->stageSetup->getYStage()->getSpeed(ui->stageSetup->getYStageAxis());
    m.data["stagez/position"]=ui->stageSetup->getZStage()->getPosition(ui->stageSetup->getZStageAxis());
    m.data["stagez/velocity"]=ui->stageSetup->getZStage()->getSpeed(ui->stageSetup->getZStageAxis());

    ui->camConfig1->storeMeasurements(m.data, "camera1/");
    ui->camConfig2->storeMeasurements(m.data, "camera2/");*/

    //qDebug()<<"measuredValues(): "<<m.data;

    return m;
}

QString QFESPIMB040OpticsSetup2::dualViewMode(int camera) const {
    QString dv="none";
    /*if (camera==0 &&  ui->chkDualView1->isChecked()) {
        if (ui->cmbDualViewOrientation->currentIndex()==0) dv="horicontal";
        else if (ui->cmbDualViewOrientation->currentIndex()==1) dv="vertical";
        else dv="unknown";
    } else if (camera==1 && ui->chkDualView2->isChecked()) {
        if (ui->cmbDualView2Orientation->currentIndex()==0) dv="horicontal";
        else if (ui->cmbDualView2Orientation->currentIndex()==1) dv="vertical";
        else dv="unknown";
    }*/
    return dv;
}

QString QFESPIMB040OpticsSetup2::dualViewMode(QFExtensionCamera *ecam, int camera) const
{
    return dualViewMode(camNumFromExtension(ecam, camera));
}

int QFESPIMB040OpticsSetup2::camNumFromExtension(QFExtensionCamera *ecam, int camera) const
{
    /*if (ui->camConfig1->cameraComboBox()->currentExtensionCamera()==ecam && ui->camConfig1->cameraComboBox()->currentCameraID()==camera) return 0;
    if (ui->camConfig2->cameraComboBox()->currentExtensionCamera()==ecam && ui->camConfig2->cameraComboBox()->currentCameraID()==camera) return 1;*/
    return -1;
}

QString QFESPIMB040OpticsSetup2::getAxisNameForStage(QFExtensionLinearStage *stage, int axis)
{
    /*if (ui->stageSetup->getXStage()==stage && ui->stageSetup->getXStageAxis()==axis) return "x";
    if (ui->stageSetup->getYStage()==stage && ui->stageSetup->getYStageAxis()==axis) return "y";
    if (ui->stageSetup->getZStage()==stage && ui->stageSetup->getZStageAxis()==axis) return "z";*/
    return "";
}

QString QFESPIMB040OpticsSetup2::getLaserConfig()
{
    QString r="";
    /*if (ui->lsLaser1->isLightSourceConnected() && ui->shutterLaser1->isShutterConnected() && ui->shutterLaser1->getShutterState())   {
        QString l=ui->lsLaser1->getLightsoureConfigForFilename();
        if (!l.isEmpty()) {
            if (!r.isEmpty()) r+="_";
            r+="laser1_"+cleanStringForFilename(l);
        }
    }
    if (ui->lsLaser2->isLightSourceConnected() && ui->shutterLaser2->isShutterConnected() && ui->shutterLaser2->getShutterState())   {
        QString l=ui->lsLaser2->getLightsoureConfigForFilename();
        if (!l.isEmpty()) {
            if (!r.isEmpty()) r+="_";
            r+="laser2_"+cleanStringForFilename(l);
        }
    }*/
    return r;
}

void QFESPIMB040OpticsSetup2::on_btnLoadSetup_clicked() {
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040ExperimentDescription/lastopticssetupdir", ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/optics_setup/").toString();
    QString filename=qfGetOpenFileName(this, tr("open optics setup ..."), dir, tr("optics setup configuration (*.osi)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        loadSettings(set, "optics_setup/");
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
            storeSettings(set, "optics_setup/");
            storePluginGlobalSettings(set, "plugin_global/");
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040ExperimentDescription/lastopticssetupdir", dir);

}


int QFESPIMB040OpticsSetup2::getBrightfieldLightSourceCount() const
{
    return 1;
}

QString QFESPIMB040OpticsSetup2::getLaserName(int laser) const
{
    return tr("Laser %1").arg(laser);
}

QString QFESPIMB040OpticsSetup2::getBrightfieldLightSourceName(int lightsource) const
{
    if (lightsource==QFESPIMB040OpticsSetupBase::BrightfieldTransmission) return tr("transmission illumination");
    return QString();
}
