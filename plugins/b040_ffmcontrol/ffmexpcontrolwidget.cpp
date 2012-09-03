#include "ffmexpcontrolwidget.h"
#define LOG_PREFIX "[FFMExpControlWidget]: "
/* Constructor only for DEBUGGING stage
*/
FFMExpControlWidget::FFMExpControlWidget(QWidget *parent) : QWidget(parent)
{
    layout = new QGridLayout(this);
    // experiments
    SingleImageButton = new QPushButton("Image");
    SingleImageButton->setToolTip("Record a single Image\nat current position");
    layout->addWidget(SingleImageButton, 5, 0);
    ImageStackButton = new QPushButton("Image Stack");
    ImageStackButton->setToolTip("Record an image stack over several\nz-plains at current xy-position ");
    layout->addWidget(ImageStackButton, 5,1);
    FCSButton = new QPushButton("FCS");
    FCSButton->setToolTip("Measure Fluorescenceat\ndifferent marked positions");
    layout->addWidget(FCSButton, 5,2);
    stagecombobox = new QFStageComboBox();
    //scancontroller = NULL;
    stage = NULL;
    autocorrelator = NULL;
    // testwidget for calling the hardware subsections
    testwidget = NULL;
    //alvsettings_dialog = NULL; <= is part of the constructor of the autocorrelator plugin unless not integrated entirely;
    scantestwidget = NULL;
    //
    scannertest = new QPushButton("Scannertest"); // test //
    Stagetest = new QPushButton("Stagetest"); // test
    Alvtest = new QPushButton("ALVTest");
    layout->addWidget(Alvtest, 0, 3);
    layout->addWidget(scannertest, 0,0); // test
    layout->addWidget(Stagetest, 0,1); // test
    layout->addWidget(stagecombobox, 0, 2);
        // end testwidget

    /*Get hardware control */

    // get references to plugin controllers
    QFExtension *qfext = QFPluginServices::getInstance()->getExtensionManager()->getInstance(QString("stage_mc2000"));
    stage = (MulticontrolStage*) qfext;
    qfext = QFPluginServices::getInstance()->getExtensionManager()->getInstance(QString("alv_autocorrelator5000"));
    autocorrelator = (AlvAutocorrelator*) qfext;
    qfext = QFPluginServices::getInstance()->getExtensionManager()->getInstance(QString("scanner2000_nicounter"));
    scancounter = ((Scanner2000NICounter*) qfext);

    ///////////// connection to testbuttons
    QObject::connect(scannertest, SIGNAL(clicked()), this, SLOT(openscannertest())); // test
    QObject::connect(Stagetest, SIGNAL(clicked()), this, SLOT(openStagetest())); // test
    QObject::connect(Alvtest, SIGNAL(clicked()), this, SLOT(openAlvtest())); // test
    /// end testbutton section


    ///////test
    //openAlvtest();
    openStagetest();
    openscannertest();
}

FFMExpControlWidget::~FFMExpControlWidget()
{
}

void FFMExpControlWidget::setLog(QtLogFile *logfile)
{
    log = logfile;

    //qDebug()<<"HERE";
//    scancontroller->setLog(log);
    //logfile->log_text("asdfsa");
    //autocorrelator->setLog(logfile);


}

/////////////// test buttons
void FFMExpControlWidget::connect_stage()
{
    //qDebug() << "1";
    stage->connectDevice(3);
    // 3 has no meaning, parameter
    // exists only to be consistent with the interface
}

void FFMExpControlWidget::movex()
{
    //double newpos = x_axis->value();
    double newpos = x_axis->text().toDouble();
    stage->move(1, newpos);
}

void FFMExpControlWidget::movey()
{
    //double newpos = y_axis->value();
    double newpos = y_axis->text().toDouble();
    stage->move(2, newpos);
    // TEST
    log_warning(tr("MOVED Y AXIS to %1").arg(newpos));
    // END TEST

}


void FFMExpControlWidget::movez()
{
    //double newpos = z_axis->value();
    double newpos = z_axis->text().toDouble();
    stage->move(3, newpos);

}

void FFMExpControlWidget::disconnect_stage()
{
    stage->disconnectDevice(3);
}

void FFMExpControlWidget::moveScannerSlot()
{
//    scancounter->moveScanner((int16_t) scanx_axis->value(), (int16_t) scany_axis->value(), 5.0);
}

/*
void FFMExpControlWidget::restore_test()
{
    alvsettings_dialog->activateWindow();
}
*/
void FFMExpControlWidget::openscannertest() // test
{    /* // testwidget
  */
    if(!scantestwidget){
        scantestwidget = new QWidget();
        scantestwidget->resize(300, 150);
        scantestwidget->move(350, 831);
        scantestwidget->setWindowTitle("ScanController: testwindow");
        scanlayout = new QGridLayout(scantestwidget);
        scanx_axis = new QSpinBox(scantestwidget);
        scany_axis = new QSpinBox(scantestwidget);
        scanx_axis->setMinimum(-32768);
        scanx_axis->setMaximum(32767);
        scany_axis->setMinimum(-32768);
        scany_axis->setMaximum(32767);
        scanconnectButton = new QPushButton("Connect!");
        scanlayout->addWidget(scanconnectButton, 0,0);
        scaninitButton = new QPushButton("Initialize!");
        scanlayout->addWidget(scaninitButton,0,1);
        scanlayout->addWidget(new QLabel("X-axis:"), 0, 2, Qt::AlignRight);
        scanlayout->addWidget(scanx_axis, 0, 3);
        scanlayout->addWidget(new QLabel("Y-Axis:"), 1, 2, Qt::AlignRight);
        scanlayout->addWidget(scany_axis, 1, 3);
        scanmoveButton = new QPushButton("Move!");
        scanlayout->addWidget(scanmoveButton,0,4);
        scandisconnectButton = new QPushButton("Disconnect!");
        scanlayout->addWidget(scandisconnectButton, 0, 5);
        scanlayout->addWidget(new QLabel("<b>add triggerMove()<\b>"), 1, 4);
        //layout -> addWidget(log, 4, 0, 8,8);
        //log->setReadOnly(true);
        QObject::connect(scandisconnectButton, SIGNAL(clicked()), this, SLOT(disconnectScannerSlot()));
        QObject::connect(scanconnectButton, SIGNAL(clicked()), this, SLOT(connectScannerSlot()));
        QObject::connect(scaninitButton, SIGNAL(clicked()), this, SLOT(initScannerSlot()));
        QObject::connect(scanmoveButton, SIGNAL(clicked()), this, SLOT(moveScannerSlot()));

        // end testwidget
    }
  //  scantestwidget->show(); // testfunctions
  //  scantestwidget->activateWindow(); //testfunctions
}

void FFMExpControlWidget::openStagetest() // TEST FUNCTION
{
    if (!testwidget){
    ///// TEST WIDGET
        testwidget = new QWidget();
        testwidget->resize(300, 150);
        testwidget->move(0,831);
        testwidget->setWindowTitle("Stage: Testwindow");
        stage_layout = new QGridLayout(testwidget);
        //x_axis = new QSpinBox(testwidget);
        //y_axis = new QSpinBox(testwidget);
        //z_axis = new QSpinBox(testwidget);
        x_axis = new QLineEdit("0.0", testwidget);
        y_axis = new QLineEdit("0.0", testwidget);
        z_axis = new QLineEdit("0.0", testwidget);
        doubleValidator = new QDoubleValidator(testwidget);
        x_axis->setValidator(doubleValidator);
        y_axis->setValidator(doubleValidator);
        z_axis->setValidator(doubleValidator);
        x_axis->setMinimumWidth(50);
        y_axis->setMinimumWidth(50);
        z_axis->setMinimumWidth(50);

        /*x_axis->setMinimum(-10000);
        x_axis->setMaximum(100000);
        y_axis->setMinimum(-100000);
        y_axis->setMaximum(100000);
        z_axis->setMinimum(-100000);
        z_axis->setMaximum(100000);*/
        connectButton = new QPushButton("Connect!", testwidget);
        QObject::connect(connectButton, SIGNAL(clicked()), this, SLOT(connect_stage()));
        stage_layout->addWidget(connectButton, 0,0);
        moveButtonX = new QPushButton("Move X");
        QObject::connect(moveButtonX, SIGNAL(clicked()), this, SLOT(movex()));
        moveButtonY = new QPushButton("Move Y");
        QObject::connect(moveButtonY, SIGNAL(clicked()), this, SLOT(movey()));
        moveButtonZ = new QPushButton("Move Z");
        QObject::connect(moveButtonZ, SIGNAL(clicked()), this, SLOT(movez()));
        stage_layout->addWidget(new QLabel("X-axis:"), 0, 1, Qt::AlignRight);
        stage_layout->addWidget(x_axis, 0, 2);
        stage_layout->addWidget(moveButtonX, 0, 3);
        stage_layout->addWidget(new QLabel("Y-axis:"), 1, 1, Qt::AlignRight);
        stage_layout->addWidget(y_axis, 1, 2);
        stage_layout->addWidget(moveButtonY, 1, 3);
        stage_layout->addWidget(new QLabel("Z-axis:"), 2, 1, Qt::AlignRight);
        stage_layout->addWidget(z_axis, 2, 2);
        stage_layout->addWidget(moveButtonZ, 2, 3);
        disconnectButton = new QPushButton("Disconnect!");
        stage_layout->addWidget(disconnectButton, 0, 4);
        QObject::connect(disconnectButton, SIGNAL(clicked()), this, SLOT(disconnect_stage()));
    }
    //testwidget->show();
    //testwidget->activateWindow();
}

void FFMExpControlWidget::openAlvtest()
{

/*
    if(!alvsettings_dialog){

        alvsettings_dialog = new QWidget();
        alvsettings_dialog->resize(400, 600);
        alvsettings_dialog->move(501,431);
        alvsettings_dialog->setWindowTitle("ALV: Testwindow and Settings");
        alvsettings_layout = new QGridLayout(alvsettings_dialog);
        alvsettings_layout->addWidget(new QLabel("<b>Settings:</b>", this), 0,0, Qt::AlignRight);

        // block duration + runnumber
        alvsettings_layout->addWidget(new QLabel("Duration:", this), 1, 0, Qt::AlignRight);
        duration = new QSpinBox(this);
        duration->setMinimum(1);
        alvsettings_layout->addWidget(duration, 1, 1);
        alvsettings_layout->addWidget(new QLabel("Run Number:", this), 1,2, Qt::AlignRight);
        runnumber = new QSpinBox(this);
        runnumber->setMinimum(0);
        alvsettings_layout->addWidget(runnumber, 1, 3);
        //end duration + runnumber

        //// block scaling modes
        scalingmodes = new QGroupBox("Scaling modes", this);
        scalinggrid = new QGridLayout(scalingmodes);
        man = new QRadioButton("Manual", scalingmodes);
        scalinggrid->addWidget(man, 0, 0);
        nor = new QRadioButton("Normal", scalingmodes);
        scalinggrid->addWidget(nor, 0, 1);
        con = new QRadioButton("Conservative", scalingmodes);
        scalinggrid->addWidget(con, 0, 2);
        sec = new QRadioButton("Secure", scalingmodes);
        scalinggrid->addWidget(sec, 0, 3);
        alvsettings_layout->addWidget(scalingmodes, 2, 0, 1,4);
        //// end scalingmodes

        // block scaling levels
        alvsettings_layout->addWidget(new QLabel("Scaling Level Ch0:", this),3,0, Qt::AlignRight);
        scalelevel0 = new QSpinBox(this);
        scalelevel0->setMinimum(0);
        scalelevel0->setMaximum(11);
        alvsettings_layout->addWidget(scalelevel0, 3, 1);
        alvsettings_layout->addWidget(new QLabel("Scaling Level Ch1:", this),3,2, Qt::AlignRight);
        scalelevel1 = new QSpinBox(this);
        scalelevel1->setMinimum(0);
        scalelevel1->setMaximum(11);
        alvsettings_layout->addWidget(scalelevel1, 3, 3);
        // end scaling levels

        // block modes
        sdmode = new QGroupBox(this);
        sdgrid = new QGridLayout(sdmode);
        single = new QRadioButton("Single Mode", sdmode);
        dual = new QRadioButton("Dual Mode", sdmode);
        sdgrid->addWidget(single, 0, 0, 0, 2);
        sdgrid->addWidget(dual, 0, 2, 0, 2);
        alvsettings_layout->addWidget(sdmode, 4, 0 , 1, 4);

        acmode = new QGroupBox(this);
        acgrid = new QGridLayout(acmode);
        autoc = new QRadioButton("Autocorrelation", acmode);
        cross = new QRadioButton("Corsscorrelation", acmode);
        acgrid->addWidget(autoc, 0, 0, 0, 2);
        acgrid->addWidget(cross, 0, 2, 0, 2);
        alvsettings_layout->addWidget(acmode, 5, 0 , 1, 4);

        inmode = new QGroupBox(this);
        ingrid = new QGridLayout(inmode);
        in0 = new QRadioButton("Input (?) 0 Mode", inmode);
        in1 = new QRadioButton("Input (?) 1 Mode", inmode);
        ingrid->addWidget(in0, 0, 0, 0, 2);
        ingrid->addWidget(in1, 0, 2, 0, 2);
        alvsettings_layout->addWidget(inmode, 6, 0 , 1, 4);
        // end modes

        fastmode = new QCheckBox("Fast Mode, whatever this means...", this);
        alvsettings_layout->addWidget(fastmode, 7, 0, 1, 3);

        // block floats
        floatvalid = new QDoubleValidator(this);
        floatvalid->setNotation(QDoubleValidator::StandardNotation);
        floatvalid->setLocale(QLocale(QLocale::English));
        floatvalid->setRange(0.0, std::numeric_limits<float>::max(), 2);

        alvsettings_layout->addWidget(new QLabel("Scattering Angle:"), 8, 0, Qt::AlignRight);
        scattangle = new QLineEdit();
        scattangle->setValidator(floatvalid);
        alvsettings_layout->addWidget(scattangle);
        alvsettings_layout->addWidget(new QLabel("Wavelength:"), 8, 2, Qt::AlignRight);
        wavel = new QLineEdit();
        wavel->setValidator(floatvalid);
        alvsettings_layout->addWidget(wavel);

        alvsettings_layout->addWidget(new QLabel("RI of solvent:"), 9, 0, Qt::AlignRight);
        ri = new QLineEdit();
        ri->setValidator(floatvalid);
        alvsettings_layout->addWidget(ri);
        alvsettings_layout->addWidget(new QLabel("Viscosity:"), 9, 2, Qt::AlignRight);
        visc = new QLineEdit();
        visc->setValidator(floatvalid);
        alvsettings_layout->addWidget(visc);

        alvsettings_layout->addWidget(new QLabel("Temperature:"), 10,0, Qt::AlignRight);
        temp = new QLineEdit();
        temp->setValidator(floatvalid);
        alvsettings_layout->addWidget(temp);
        //end block floats

        runstate = new QGroupBox("RunState Parameter", this);
        rungrid = new QGridLayout(runstate);
        start = new QRadioButton("Start", runstate);
        rungrid->addWidget(start, 0, 0);
        stop = new QRadioButton("Stop", runstate);
        rungrid->addWidget(stop, 0, 1);
        para = new QRadioButton("para", runstate);
        rungrid->addWidget(para, 0, 2);
        alvsettings_layout->addWidget(runstate, 11, 0, 1, 3);

        alvsettings_layout->addWidget(new QLabel("<b>Actions:</b>"), 12, 0);
        alvstarter = new QPushButton("Start ALV!");
        memoryallocator = new QPushButton("Prep shr mem blocks!");
        settings_confirmer = new QPushButton("Confirm Settings!");
        runstartstopper = new QPushButton("Start/Stop/SetParmetersonly");
        test_reader = new QPushButton("Read MetaData!");
        test_reader2 = new QPushButton("Read Corr0!");
        test_reader3 = new QPushButton("Read Corr1!");
        test_reader4 = new QPushButton("Read CR!");
        test_reader5 = new QPushButton("Read Histo!");

        alvsettings_layout->addWidget(alvstarter, 13, 0);
        alvsettings_layout->addWidget(memoryallocator, 13, 1);
        alvsettings_layout->addWidget(settings_confirmer, 13, 2);
        alvsettings_layout->addWidget(runstartstopper, 13, 3);
        alvsettings_layout->addWidget(test_reader, 14, 0);
        alvsettings_layout->addWidget(test_reader2, 14, 1);
        alvsettings_layout->addWidget(test_reader3, 14, 2);
        alvsettings_layout->addWidget(test_reader4, 14, 3);
        alvsettings_layout->addWidget(test_reader5, 14, 4);

        QObject::connect(alvstarter, SIGNAL(clicked()), autocorrelator, SLOT(startAlv()));
        QObject::connect(memoryallocator, SIGNAL(clicked()), autocorrelator, SLOT(prepare_shared_memory()));
        QObject::connect(settings_confirmer, SIGNAL(clicked()), this, SLOT(setALVParameters()));
        QObject::connect(runstartstopper, SIGNAL(clicked()), autocorrelator, SLOT(triggerALVSoftware()));
        QObject::connect(test_reader, SIGNAL(clicked()), autocorrelator, SLOT(test_action()));
        QObject::connect(test_reader2, SIGNAL(clicked()), autocorrelator, SLOT(test_action2()));
        QObject::connect(test_reader3, SIGNAL(clicked()), autocorrelator, SLOT(test_action3()));
        QObject::connect(test_reader4, SIGNAL(clicked()), autocorrelator, SLOT(test_action4()));
        QObject::connect(test_reader5, SIGNAL(clicked()), autocorrelator, SLOT(test_action5()));


    }
    //alvsettings_dialog->show(); // testfunctions
    //alvsettings_dialog->activateWindow(); //testfunctions

*/


   // autocorrelator->showAcquisitionDeviceSettingsDialog();


}

void FFMExpControlWidget::connectScannerSlot()
{
//    scancounter->connectScanner();
}

void FFMExpControlWidget::disconnectScannerSlot()
{
//    scancounter->disconnectScanner();
}

void FFMExpControlWidget::initScannerSlot()
{
//    scancounter->initScanner();
}

////// TEST METHOD


////////////test


void FFMExpControlWidget::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void FFMExpControlWidget::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void FFMExpControlWidget::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}


