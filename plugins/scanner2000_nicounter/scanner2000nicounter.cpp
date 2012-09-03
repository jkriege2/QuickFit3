#include "scanner2000nicounter.h"

#define LOG_PREFIX QString("[scanner2000nicounter]: ").toUpper()

Scanner2000NICounter ::Scanner2000NICounter (QObject* parent):
    QObject(parent)
{
    logService=NULL;

    //mw=NULL;
    scancontroller = new ScanController();
    counterCard = new NIDAQ6602();

    ///test
    //createTestWidget();


}

Scanner2000NICounter ::~Scanner2000NICounter () {

}


void Scanner2000NICounter ::deinit() {
    /* add code for cleanup here */
}
/*
ScanController *Scanner2000NICounter::getScanController()
{
    return scancontroller;
}

MulticontrolStage *Scanner2000NICounter ::get_stage()
{
    if (stage)
        return stage;
    else
        return NULL;
}
*/

void Scanner2000NICounter ::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */
}

void Scanner2000NICounter ::initExtension() {
    /* do initializations here but do not yet connect to the camera! */


    // some example code that may be used to register a menu and a tool button:
/*
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    QAction *actStartPlugin=new QAction(QIcon(getIconFilename()), tr("Start Extension Scanner2000NICounter "), this);
    QObject::connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));

    QToolBar* exttb=services->getToolbar("extensions");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        extm->addAction(actStartPlugin);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));
*/

  //  QObject* obj=services->getExtensionManager()->getQObjectInstance("stage_mc2000");
    //stage=NULL;
    //if (obj) {
      //  stage = (MulticontrolStage*) obj;
        //stage->connectDevice(3);
       // qDebug() << "Call this function: Scanner2000NICounter ::initextension"; // debug
        //stage=qobject_cast<MulticontrolStage*>(obj);
    //}
    // FOR TESTING



   // startPlugin();

/*
    if (stage) {
        qDebug()<<"found stage_mc2000 plugin!!!";
    } else {
        qDebug()<<"didn't find' stage_mc2000 plugin!!!    obj="<<obj;
    }*/
}

void Scanner2000NICounter ::startPlugin() {
/*
  //  qDebug() << "Call this function: Scanner2000NICounter ::startPlugin() 1111";
    if(!mw){
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        mw = new ffmMainWindow(this);
        //mw = new ffmMainWindow(this);
        //if (settings) mw->loadSettings(settings);
        mw->show();
        QApplication::restoreOverrideCursor();
    //    qDebug() << "Call this function: Scanner2000NICounter ::startPlugin() 2222";
    } else {
        mw->show();
        mw->activateWindow();
    }
    //qDebug() << "Call this function: Scanner2000NICounter ::startPlugin() 3333";
    */
}



void Scanner2000NICounter ::loadSettings(ProgramOptions* settingspo) {
    /* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());


    // the QSettings object for quickfit.ini
    // ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void Scanner2000NICounter ::storeSettings(ProgramOptions* settingspo) {
    /* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

    // ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void Scanner2000NICounter ::log_text(QString message) {
    //if (logService) logService->log_text(LOG_PREFIX+message);
    //else if (services) services->log_text(LOG_PREFIX+message);
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void Scanner2000NICounter ::log_warning(QString message) {
    //if (logService) logService->log_warning(LOG_PREFIX+message);
    //else if (services) services->log_warning(LOG_PREFIX+message);
     QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void Scanner2000NICounter ::log_error(QString message) {
    //if (logService) logService->log_error(LOG_PREFIX+message);
    //else if (services) services->log_error(LOG_PREFIX+message);
     QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}

////////////////////////////
/// TEST ENVIRONMENT
///////////////////////////

void Scanner2000NICounter::createTestWidget()
{
    testwidget = new QWidget();
    testLayout = new QGridLayout(testwidget);
    testwidget->show();
    testwidget->resize(500, 300);
    //testwidget->move(800, 600);
    testwidget->setWindowTitle("testwindow: Image Scanner NI Counter Card Acquisition");

    ///// SCANNER
    scanx_axis = new QSpinBox(testwidget);
    scany_axis = new QSpinBox(testwidget);
    scanx_axis->setMinimum(-32768);
    scanx_axis->setMaximum(32767);
    scany_axis->setMinimum(-32768);
    scany_axis->setMaximum(32767);
    scanconnectButton = new QPushButton("Connect!");
    testLayout->addWidget(scanconnectButton, 0,0);
    scaninitButton = new QPushButton("Initialize!");
    testLayout->addWidget(scaninitButton,0,1);
    testLayout->addWidget(new QLabel("X-axis:"), 0, 2, Qt::AlignRight);
    testLayout->addWidget(scanx_axis, 0, 3);
    testLayout->addWidget(new QLabel("Y-Axis:"), 1, 2, Qt::AlignRight);
    testLayout->addWidget(scany_axis, 1, 3);
    scanmoveButton = new QPushButton("Move!");
    testLayout->addWidget(scanmoveButton,0,4);
    scandisconnectButton = new QPushButton("Disconnect!");
    testLayout->addWidget(scandisconnectButton, 0, 5);
    triggerMoveButton = new QPushButton("Trigger Move!");
    triggerChannelBox = new QSpinBox(testwidget);
    triggerChannelBox->setMaximum(10);
    triggerChannelBox->setMinimum(0);
    triggerChannelBox->setValue(1);
    testLayout->addWidget(new QLabel("Trigger Channel:", testwidget), 1, 0);
    testLayout->addWidget(triggerChannelBox, 1, 1);
    moveTimeBox = new QDoubleSpinBox(testwidget);
    moveTimeBox->setMaximum(60000.0);
    moveTimeBox->setMinimum(1.0);
    moveTimeBox->setValue(500.0);
    testLayout->addWidget(new QLabel("moveTime in ms:", testwidget), 1,4);
    testLayout->addWidget(moveTimeBox, 1, 5);

    QObject::connect(scanconnectButton, SIGNAL(clicked()), this, SLOT(connectScanner())),
    QObject::connect(scaninitButton, SIGNAL(clicked()), this, SLOT(initScanner()));
    QObject::connect(scandisconnectButton, SIGNAL(clicked()), this, SLOT(disconnectScanner())),
    QObject::connect(scanmoveButton, SIGNAL(clicked()), this, SLOT(callMoveScanner()));
    QObject::connect(triggerMoveButton, SIGNAL(clicked()), this, SLOT(callTriggerMove()));

    ////// NIDAQ

    readLineButton = new QPushButton("Read Line", testwidget);
    readPointButton = new QPushButton("Read Countrate", testwidget);
    confStaticButton =  new QPushButton("Static Configuration", testwidget);
    confLineButton = new QPushButton("Line Configuration", testwidget);
    //tasksStopper = new QPushButton("StopTasks", testwidget);
    statTaskStarter = new QPushButton("start pointmeas", testwidget);
    lineTaskStarter = new QPushButton("Start Line Tasks", testwidget);

    pointMeasTimeBox = new QDoubleSpinBox(testwidget);
    pointMeasTimeBox->setMaximum(1500.0);
    pointMeasTimeBox->setMinimum(1.0);
    pointMeasTimeBox->setValue(500.0);

    measnumBox = new QSpinBox(testwidget);
    measnumBox->setMaximum(4000);
    measnumBox->setMinimum(50);
    measnumBox->setValue(400);

    measTimeBox = new QDoubleSpinBox(testwidget);
    measTimeBox->setMaximum(100000.0);
    measTimeBox->setMinimum(0.00001);
    measTimeBox->setValue(2000.0);

    testLayout->addWidget(confLineButton, 3, 0);
    testLayout->addWidget(new QLabel("SampleNumber:", testwidget), 3, 1);
    testLayout->addWidget(measnumBox, 3, 2);
    testLayout->addWidget(new QLabel("measTime for Line [ms]:", testwidget), 3, 3);
    testLayout->addWidget(measTimeBox, 3, 4);
    testLayout->addWidget(lineTaskStarter, 3, 5);
    testLayout->addWidget(readLineButton, 3, 6);


    testLayout->addWidget(confStaticButton, 4, 0);
    testLayout->addWidget(new QLabel("pointmeasTime [ms]:", testwidget), 4, 1);
    testLayout->addWidget(pointMeasTimeBox, 4, 2);
    testLayout->addWidget(statTaskStarter, 4, 5);
    testLayout->addWidget(readPointButton, 4, 6);
    //testLayout->addWidget(tasksStopper, 5, 5);
    //testLayout->addWidget();
    QObject::connect(confStaticButton, SIGNAL(clicked()), this, SLOT(configureStaticMode()));
    QObject::connect(confLineButton, SIGNAL(clicked()), this, SLOT(configureLineMode()));

    QObject::connect(statTaskStarter, SIGNAL(clicked()), this, SLOT(pointMeas()));
    QObject::connect(lineTaskStarter, SIGNAL(clicked()), this, SLOT(lineMeas()));

    QObject::connect(readLineButton, SIGNAL(clicked()), this, SLOT(testLineReadfunction()));
    QObject::connect(readPointButton, SIGNAL(clicked()), this, SLOT(testPointReadfunction()));

    //QObject::connect(tasksStopper, SIGNAL(clicked()), this, SLOT(stopTasks()));

}


///////////// Scanner functions
void Scanner2000NICounter::connectScanner()
{
    log_text("Called connectScanner()");
    scancontroller->connectToScanner();
}

void Scanner2000NICounter::disconnectScanner()
{
    log_text("Called disconnectScanner()");
    scancontroller->disconnect();
}

void Scanner2000NICounter::initScanner()
{
    log_text("Called initScanner()");
    scancontroller->initScanner();
}

void Scanner2000NICounter::moveScanner(int16_t newxpos, int16_t newypos, float movetime)
{
    log_text("Called moveScannner()");
    scancontroller->MoveToXY(newxpos, newypos);
}

void Scanner2000NICounter::triggerMove(uint8_t triggerChannel, int16_t newxpos, int16_t newypos, float movetime)
{
    log_text("Called triggerMove()");
    scancontroller->TriggerMove(triggerChannel, newxpos, newypos, movetime);
}

void Scanner2000NICounter::callMoveScanner()
{
    int16_t newxpos = (int16_t) scanx_axis->value();
    int16_t newypos = (int16_t) scany_axis->value();
    float moveTime = (float) moveTimeBox->value();
    moveScanner(newxpos, newypos, moveTime);
}

void Scanner2000NICounter::callTriggerMove()
{
    int16_t newxpos = (int16_t) scanx_axis->value();
    int16_t newypos = (int16_t) scany_axis->value();
    uint8_t triggerChannel = (uint8_t) triggerChannelBox->value();
    float moveTime = (float) moveTimeBox->value();
    triggerMove(triggerChannel, newxpos, newypos, moveTime);
}

////////////////// NIDAQ functions
void Scanner2000NICounter::testLineReadfunction()
{
    log_text("Called testLineReadfunction()");
    bool ok = false;
    QList <QList<float> > data = counterCard->getLineData(&ok);

    if (ok){
        log_text("Read data samples:");
        //first 5 samples
        for(int i=0; i < 5; i++)
            log_text(tr("Values at %1: %2, %3").arg(i).arg(data.at(0).at(i)).arg(data.at(0).at(i)));
        // samples 40-44
        for(int i=40; i < 45; i++)
            log_text(tr("Values at %1: %2, %3").arg(i).arg(data.at(0).at(i)).arg(data.at(0).at(i)));
    }
}

void Scanner2000NICounter::testPointReadfunction()
{
    log_text("Called testPointReadfunction()");
    bool ok = false;
    QList<float> data = counterCard->getStaticCountRate(&ok);
    if (ok){
        log_text("Read data samples:");
        log_text(tr("CountRate 1: %1 , CountRate 2: %2").arg(data.at(0)).arg(data.at(1)));
    }
}

void Scanner2000NICounter::configureStaticMode()
{
    log_text("Called configureStaticMode()");
    uint32_t measTime = (uint32_t) pointMeasTimeBox->value();
    counterCard->configureStaticMode(measTime);
}

void Scanner2000NICounter::pointMeas()
{

    bool ok = false;
    counterCard->startMeasStaticMode(&ok);
    if(ok) log_text("Measurement Started successfully.");
    else log_error("Measurement could not be started .");
}

void Scanner2000NICounter::lineMeas()
{
    log_text("Called lineMeas()");
    bool ok = false;
    counterCard->startMeasInLineMode(&ok);
    if(ok) log_text("Measurement Started successfully.");
    else log_error("Measurement could not be started .");
}

void Scanner2000NICounter::configureLineMode()
{
    log_text("Called configureLineMode()");
    uint32_t measNumber = (uint32_t) measnumBox->value(); // number of measurements
    uint32_t measTimePerPixel = (uint32_t) (measTimeBox->value() * 20000 / measNumber); // ms to ticks of 20 MHz
    counterCard->configureLineMode(measNumber, measTimePerPixel);
}

// END TESTENVIRONMENT

Q_EXPORT_PLUGIN2(scanner2000_nicounter, Scanner2000NICounter )

/*
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    mw = new ffmMainWindow();
    mw->show();
    qDebug() << "startPlugin";
    QApplication::restoreOverrideCursor();
*/
