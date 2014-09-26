/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#include "qfacquisitionconfigcombobox.h"
#include "tools.h"
#include "qftools.h"

QFAcquisitionConfigComboBoxNotifier::QFAcquisitionConfigComboBoxNotifier(QObject* parent):
    QObject(parent)
{

}

void QFAcquisitionConfigComboBoxNotifier::emitUpdate() {
    emit doUpdate();
}









QFAcquisitionConfigComboBoxNotifier* QFAcquisitionConfigComboBox::m_notifier=NULL;


QFAcquisitionConfigComboBox::QFAcquisitionConfigComboBox(QWidget* parent, QString configDirectory):
    QEnhancedComboBox(parent)
{
    m_configDirectory=configDirectory;
    m_cam=NULL;
    m_camIdx=-1;
    m_extension=NULL;
    m_stopresume=NULL;
//    std::cout<<"m_notifier="<<m_notifier<<std::endl;
    if (m_notifier==NULL) m_notifier=new QFAcquisitionConfigComboBoxNotifier(NULL);
    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(rereadConfigFiles()));
    init(configDirectory);
}

void QFAcquisitionConfigComboBox::init(QString configDirectory) {
    m_configDirectory=configDirectory;
    deviceChanged(m_extension, m_cam, m_camIdx);
}

QFAcquisitionConfigComboBox::~QFAcquisitionConfigComboBox()
{
    //dtor
}

void QFAcquisitionConfigComboBox::directoryChanged(const QString& path) {
    deviceChanged(m_extension, m_cam, m_camIdx);
}

void QFAcquisitionConfigComboBox::rereadConfigFiles() {
    deviceChanged(m_extension, m_cam, m_camIdx);
}


void QFAcquisitionConfigComboBox::connectTo(QFAcquisitionComboBox* combo) {
    if (!combo) {
        disconnect(this, SLOT(deviceChanged(QFExtension*, QFExtensionAcquisition*, int)));
        clear();
    } else {
        connect(combo, SIGNAL(deviceChanged(QFExtension*, QFExtensionAcquisition*, int)), this, SLOT(deviceChanged(QFExtension*, QFExtensionAcquisition*, int)));
        deviceChanged(combo->currentExtension(), combo->currentExtensionAcquisition(), combo->currentAcquisitionID());
    }
}

void QFAcquisitionConfigComboBox::deviceChanged(QFExtension* extension, QFExtensionAcquisition *cam, int camIdx) {
    m_cam=cam;
    m_camIdx=camIdx;
    m_extension=extension;

   // store current setting of comboboxes
    QString ac=currentText();
    clear();
    if ((!cam) || (!extension) || (camIdx<0)) return;

    // find applicable .acf files

    QString directory=m_configDirectory+"/plugins/"+extension->getID();
    QDir(m_configDirectory).mkpath("plugins/"+extension->getID());
    //std::cout<<"should have created all dirs: '"<<directory.toStdString()<<"'"<<std::endl;
    directory+="/";
    QStringList filenames;
    QDir dir = QDir(directory, "*.acf");
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString absfn=dir.absoluteFilePath(fileName);
        filenames.append(absfn);
    }
    // if there are no configurations, make sure there is at least default.acf
    if (filenames.isEmpty()) {
        QFile file(directory+"default.acf");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.close();
        filenames.append(directory+"default.acf");
    }
    // fill comboboxes
    for (int i=0; i<filenames.size(); i++) {
        QSettings set(filenames[i],QSettings::IniFormat);
        QString fn=set.value("camconfigname", QFileInfo(filenames[i]).baseName()).toString();
        addItem(QIcon(":/libqf3widgets/acquisition_config.png"), fn, filenames[i]);
    }

    model()->sort(0);
    /*for (int i=0; i<count(); i++) {
        qDebug()<<itemText(i)<<itemData(i);
    }*/
    // FINALLY: make sure the combo boxes have their values set.
    int configidx=findText(ac);
    if (configidx<0) configidx=0;
    setCurrentIndex(configidx);

}

void QFAcquisitionConfigComboBox::setCurrentConfig(QString name) {
    int configidx=findText(name);
    if (configidx<0) configidx=0;
    setCurrentIndex(configidx);
}

void QFAcquisitionConfigComboBox::setStopResume(QFAcquisitionConfigComboBoxStartResume* stopresume) {
    m_stopresume=stopresume;
}

void QFAcquisitionConfigComboBox::editCurrent() {
    QString filename=currentConfigFilename();
    if (filename.size()>0) {
        //qDebug()<<"stop"<<m_stopresume;
        if (m_stopresume) m_stopresume->stop();
        QSettings* settings=new QSettings(filename, QSettings::IniFormat);

        //qDebug()<<"show dialog";
        m_cam->showAcquisitionDeviceSettingsDialog(m_camIdx, *settings, this);
        settings->sync();
        delete settings;
        if (m_notifier) m_notifier->emitUpdate();
        if (m_stopresume) m_stopresume->resume();
        //qDebug()<<"resume"<<m_stopresume;
    }
}


void QFAcquisitionConfigComboBox::deleteCurrent() {
    QString filename=currentConfigFilename();
    if (filename.size()>0) {
        int ret = QMessageBox::question(this, tr("Delete Configurtion ..."),
                        tr("Do you really want to delete the configuration '%1'?").arg(QFileInfo(filename).baseName()),
                        QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QFile::remove(filename);
            if (m_notifier) m_notifier->emitUpdate();
        }
    }
}

void QFAcquisitionConfigComboBox::saveAsCurrent() {
    if (m_stopresume) m_stopresume->stop();
    QString filename=currentConfigFilename();
    QString configname=currentText();
    if (filename.size()>0) {
        bool ok;
        QString newshowname = QInputDialog::getText(this, tr("Save Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          configname/*QFileInfo(filename).baseName()*/, &ok);
        QString newname=cleanStringForFilename(newshowname);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".acf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Configurtion As ..."),
                                tr("A configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                QFile::copy(filename, newfilename);
                QSettings set(newfilename, QSettings::IniFormat);
                set.setValue("camconfigname", newshowname);
                rereadConfigFiles();
                setCurrentConfig(newshowname);
                if (m_notifier) m_notifier->emitUpdate();
            }
        }
    }
    if (m_stopresume) m_stopresume->resume();
}

void QFAcquisitionConfigComboBox::renameCurrent() {
    if (m_stopresume) m_stopresume->stop();
    QString filename=currentConfigFilename();
    QString configname=currentText();
    if (filename.size()>0) {
        bool ok;
        QString newshowname = QInputDialog::getText(this, tr("Rename Configurtion As ..."),
                                          tr("New Name:"), QLineEdit::Normal,
                                          configname/*QFileInfo(filename).baseName()*/, &ok);
        QString newname=cleanStringForFilename(newshowname);
        if (ok && !newname.isEmpty()) {
            QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".acf";
            if (QFile::exists(newfilename)) {
                int ret = QMessageBox::question(this, tr("Save Configurtion As ..."),
                                tr("A configuration with the name '%1' already exists.\n"
                                   "Do you want to overwrite?").arg(newname),
                                QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
                if (ret==QMessageBox::No) ok=false;
            }
            if (ok) {
                if (filename!=newfilename)  {
                    QFile::copy(filename, newfilename);
                    QFile::remove(filename);
                }
                QSettings set(newfilename, QSettings::IniFormat);
                set.setValue("camconfigname", newshowname);
                rereadConfigFiles();
                setCurrentConfig(newshowname);
                if (m_notifier) m_notifier->emitUpdate();
            }
        }
    }
    if (m_stopresume) m_stopresume->resume();

}


void QFAcquisitionConfigComboBox::addNew() {
    if (m_stopresume) m_stopresume->stop();
    QString tempFile="qf3b040spim_XXXXX.tmp";
    {
        QTemporaryFile file;
        if (file.open()) {
            tempFile=file.fileName();
            file.close();
        }
    }

    QSettings* settings=new QSettings(tempFile, QSettings::IniFormat);
    m_cam->showAcquisitionDeviceSettingsDialog(m_camIdx, *settings, this);
    settings->sync();
    delete settings;

    QString filename=currentConfigFilename();
    bool ok;
    QString newshowname = QInputDialog::getText(this, tr("Save Configurtion As ..."),
                                      tr("New Name:"), QLineEdit::Normal,
                                      "new_config_name", &ok);
    QString newname=cleanStringForFilename(newshowname);
    if (ok && !newname.isEmpty()) {
        QString newfilename=QFileInfo(filename).absolutePath()+"/"+newname+".acf";
        if (QFile::exists(newfilename)) {
            int ret = QMessageBox::question(this, tr("Save Configurtion As ..."),
                            tr("A configuration with the name '%1' already exists.\n"
                               "Do you want to overwrite?").arg(newname),
                            QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
            if (ret==QMessageBox::No) ok=false;
        }
        if (ok) {
            //copy_file(tempFile.toStdString(), newfilename.toStdString());
            QFile::copy(tempFile, newfilename);
            QFile::remove(tempFile);
            QSettings set(newfilename, QSettings::IniFormat);
            set.setValue("camconfigname", newshowname);
            rereadConfigFiles();
            setCurrentConfig(newshowname);
            if (m_notifier) m_notifier->emitUpdate();
        }
    }
    if (m_stopresume) m_stopresume->resume();
}








QFAcquisitionConfigEditorWidget::QFAcquisitionConfigEditorWidget(QWidget* parent, QString configDirectory):
    QWidget(parent)
{
    QHBoxLayout* layout=new QHBoxLayout();
    setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    combobox=new QFAcquisitionConfigComboBox(this, configDirectory);
    combobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    layout->addWidget(combobox, 2);
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCurrentIndexChanged(int)));

    actConfig = new QAction(QIcon(":/libqf3widgets/acquisition_configuration.png"), tr("&Configure"), this);
    connect(actConfig, SIGNAL(triggered()), combobox, SLOT(editCurrent()));

    actAdd = new QAction(QIcon(":/libqf3widgets/config_add.png"), tr("&Add new Configuration"), this);
    connect(actAdd, SIGNAL(triggered()), combobox, SLOT(addNew()));

    actRename = new QAction(QIcon(":/libqf3widgets/config_rename.png"), tr("&Rename Configuration As ..."), this);
    connect(actRename, SIGNAL(triggered()), combobox, SLOT(renameCurrent()));

    actSaveAs = new QAction(QIcon(":/libqf3widgets/config_saveas.png"), tr("&Save Configuration As ..."), this);
    connect(actSaveAs, SIGNAL(triggered()), combobox, SLOT(saveAsCurrent()));

    actDelete = new QAction(QIcon(":/libqf3widgets/config_delete.png"), tr("&Delete Configuration"), this);
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
    layout->addStretch(1);

}

QFAcquisitionConfigEditorWidget::~QFAcquisitionConfigEditorWidget() {
}

void QFAcquisitionConfigEditorWidget::init(QString configDirectory) {
    combobox->init(configDirectory);
}
