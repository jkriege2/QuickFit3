#include "qfcameraconfigcombobox.h"
#include "tools.h"

QFCameraConfigComboBoxNotifier::QFCameraConfigComboBoxNotifier(QObject* parent):
    QObject(parent)
{

}

void QFCameraConfigComboBoxNotifier::emitUpdate() {
    emit doUpdate();
}









QFCameraConfigComboBoxNotifier* QFCameraConfigComboBox::m_notifier=NULL;


QFCameraConfigComboBox::QFCameraConfigComboBox(QWidget* parent, QString configDirectory):
    QComboBox(parent)
{
    m_configDirectory=configDirectory;
    m_cam=NULL;
    m_camIdx=-1;
    m_extension=NULL;
    m_stopresume=NULL;
//    std::cout<<"m_notifier="<<m_notifier<<std::endl;
    if (m_notifier==NULL) m_notifier=new QFCameraConfigComboBoxNotifier(NULL);
    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(rereadConfigFiles()));
    init(configDirectory);
}

void QFCameraConfigComboBox::init(QString configDirectory) {
    m_configDirectory=configDirectory;
}

QFCameraConfigComboBox::~QFCameraConfigComboBox()
{
    //dtor
}

void QFCameraConfigComboBox::directoryChanged(const QString& path) {
    cameraChanged(m_extension, m_cam, m_camIdx);
}

void QFCameraConfigComboBox::rereadConfigFiles() {
    cameraChanged(m_extension, m_cam, m_camIdx);
}


void QFCameraConfigComboBox::connectTo(QFCameraComboBox* combo) {
    if (!combo) {
        disconnect(this, SLOT(cameraChanged(QFExtension*, QFExtensionCamera*, int)));
        clear();
    } else {
        connect(combo, SIGNAL(cameraChanged(QFExtension*, QFExtensionCamera*, int)), this, SLOT(cameraChanged(QFExtension*, QFExtensionCamera*, int)));
        cameraChanged(combo->currentExtension(), combo->currentExtensionCamera(), combo->currentCameraID());
    }
}

void QFCameraConfigComboBox::cameraChanged(QFExtension* extension, QFExtensionCamera* cam, int camIdx) {
    m_cam=cam;
    m_camIdx=camIdx;
    m_extension=extension;

   // store current setting of comboboxes
    QString ac=currentText();
    clear();
    if ((!cam) || (!extension) || (camIdx<0)) return;

    // find applicable .ccf files

    QString directory=m_configDirectory+"/plugins/"+extension->getID();
    QDir(m_configDirectory).mkpath("plugins/"+extension->getID());
    //std::cout<<"should have created all dirs: '"<<directory.toStdString()<<"'"<<std::endl;
    directory+="/";
    QStringList filenames;
    QDir dir = QDir(directory, "*.ccf");
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString absfn=dir.absoluteFilePath(fileName);
        filenames.append(absfn);
    }
    // if there are no configurations, make sure there is at least default.ccf
    if (filenames.isEmpty()) {
        QFile file(directory+"default.ccf");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.close();
        filenames.append(directory+"default.ccf");
    }
    // fill comboboxes
    for (int i=0; i<filenames.size(); i++) {
        QString fn=QFileInfo(filenames[i]).baseName();
        addItem(QIcon(":/libqf3widgets/camera_config.png"), fn, filenames[i]);
    }


    // FINALLY: make sure the combo boxes have their values set.
    int configidx=findText(ac);
    if (configidx<0) configidx=0;
    setCurrentIndex(configidx);

}

void QFCameraConfigComboBox::setCurrentConfig(QString name) {
    int configidx=findText(name);
    if (configidx<0) configidx=0;
    setCurrentIndex(configidx);
}

void QFCameraConfigComboBox::setStopResume(QFCameraConfigComboBoxStartResume* stopresume) {
    m_stopresume=stopresume;
}

void QFCameraConfigComboBox::editCurrent() {
    QString filename=currentConfigFilename();
    if (filename.size()>0) {
        //qDebug()<<"stop"<<m_stopresume;
        if (m_stopresume) m_stopresume->stop();
        QSettings* settings=new QSettings(filename, QSettings::IniFormat);

        //qDebug()<<"show dialog";
        m_cam->showCameraSettingsDialog(m_camIdx, *settings, this);
        settings->sync();
        delete settings;
        if (m_notifier) m_notifier->emitUpdate();
        if (m_stopresume) m_stopresume->resume();
        //qDebug()<<"resume"<<m_stopresume;
    }
}


void QFCameraConfigComboBox::deleteCurrent() {
    QString filename=currentConfigFilename();
    if (filename.size()>0) {
        int ret = QMessageBox::question(this, tr("Delete Camera Configurtion ..."),
                        tr("Do you really want to delete the camera configuration '%1'?").arg(QFileInfo(filename).baseName()),
                        QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QFile::remove(filename);
            if (m_notifier) m_notifier->emitUpdate();
        }
    }
}

void QFCameraConfigComboBox::saveAsCurrent() {
        QString filename=currentConfigFilename();
        if (filename.size()>0) {
        bool ok;
        QString newname = QInputDialog::getText(this, tr("Save Camera Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          QFileInfo(filename).baseName(), &ok);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".ccf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Camera Configurtion As ..."),
                                tr("A camera configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                copy_file(filename.toStdString(), newfilename.toStdString());
                if (m_notifier) m_notifier->emitUpdate();
                setCurrentConfig(newname);
            }
        }
    }
}

void QFCameraConfigComboBox::renameCurrent() {
        QString filename=currentConfigFilename();
        if (filename.size()>0) {
        bool ok;
        QString newname = QInputDialog::getText(this, tr("Rename Camera Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          QFileInfo(filename).baseName(), &ok);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".ccf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Camera Configurtion As ..."),
                                tr("A camera configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                copy_file(filename.toStdString(), newfilename.toStdString());
                if (m_notifier) m_notifier->emitUpdate();
                setCurrentConfig(newname);
            }
        }
    }
}


void QFCameraConfigComboBox::addNew() {
    QString tempFile="qf3b040spim_XXXXX.tmp";
    {
        QTemporaryFile file;
        if (file.open()) {
            tempFile=file.fileName();
            file.close();
        }
    }

    if (m_stopresume) m_stopresume->stop();
    QSettings* settings=new QSettings(tempFile, QSettings::IniFormat);
    m_cam->showCameraSettingsDialog(m_camIdx, *settings, this);
    settings->sync();
    delete settings;
    if (m_stopresume) m_stopresume->resume();

    QString filename=currentConfigFilename();
    bool ok;
    QString newname = QInputDialog::getText(this, tr("Save Camera Configurtion As ..."),
                                      tr("New Name:"), QLineEdit::Normal,
                                      "new_config_name", &ok);
    if (ok && !newname.isEmpty()) {
        QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".ccf";
        if (QFile::exists(newfilename)) {
            int ret = QMessageBox::question(this, tr("Save Camera Configurtion As ..."),
                            tr("A camera configuration with the name '%1' already exists.\n"
                               "Do you want to overwrite?").arg(newname),
                            QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
            if (ret==QMessageBox::No) ok=false;
        }
        if (ok) {
            //copy_file(tempFile.toStdString(), newfilename.toStdString());
            QFile::copy(tempFile, newfilename);
            QFile::remove(tempFile);
            if (m_notifier) m_notifier->emitUpdate();
            setCurrentConfig(newname);
        }
    }
}








QFCameraConfigEditorWidget::QFCameraConfigEditorWidget(QWidget* parent, QString configDirectory):
    QWidget(parent)
{
    QHBoxLayout* layout=new QHBoxLayout();
    setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    combobox=new QFCameraConfigComboBox(this, configDirectory);
    layout->addWidget(combobox, 2);
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCurrentIndexChanged(int)));

    actConfig = new QAction(QIcon(":/libqf3widgets/acquisition_configuration.png"), tr("&Configure Camera"), this);
    connect(actConfig, SIGNAL(triggered()), combobox, SLOT(editCurrent()));

    actAdd = new QAction(QIcon(":/libqf3widgets/config_add.png"), tr("&Add new Camera Configuration"), this);
    connect(actAdd, SIGNAL(triggered()), combobox, SLOT(addNew()));

    actRename = new QAction(QIcon(":/libqf3widgets/config_rename.png"), tr("&Rename Camera Configuration As ..."), this);
    connect(actRename, SIGNAL(triggered()), combobox, SLOT(renameCurrent()));

    actSaveAs = new QAction(QIcon(":/libqf3widgets/config_saveas.png"), tr("&Save Camera Configuration As ..."), this);
    connect(actSaveAs, SIGNAL(triggered()), combobox, SLOT(saveAsCurrent()));

    actDelete = new QAction(QIcon(":/libqf3widgets/config_delete.png"), tr("&Delete Camera Configuration"), this);
    connect(actDelete, SIGNAL(triggered()), combobox, SLOT(deleteCurrent()));

    layout->addSpacing(5);

    btnConfig=new QToolButton(this);
    btnConfig->setDefaultAction(actConfig);

    btnAdd=new QToolButton(this);
    btnAdd->setDefaultAction(actAdd);

    btnDelete=new QToolButton(this);
    btnDelete->setDefaultAction(actDelete);

    btnRename=new QToolButton(this);
    btnRename->setDefaultAction(actRename);

    btnSaveAs=new QToolButton(this);
    btnSaveAs->setDefaultAction(actSaveAs);

    layout->addWidget(btnConfig, 0);
    layout->addSpacing(5);
    layout->addWidget(btnAdd, 0);
    layout->addWidget(btnRename, 0);
    layout->addWidget(btnSaveAs, 0);
    layout->addWidget(btnDelete, 0);

}

QFCameraConfigEditorWidget::~QFCameraConfigEditorWidget() {
}

void QFCameraConfigEditorWidget::init(QString configDirectory) {
    combobox->init(configDirectory);
}
