#include "qfespimb040configcombobox.h"
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

QFESPIMB040ConfigComboBox::QFESPIMB040ConfigComboBox(QWidget *parent) :
    QEnhancedComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCurrentIndexChanged(int)));
    baseDir=QApplication::applicationDirPath();
    m_ext="ini";
    icon=QIcon();
    clear();
}

QFESPIMB040ConfigComboBox::~QFESPIMB040ConfigComboBox()
{
    disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCurrentIndexChanged(int)));
}

QString QFESPIMB040ConfigComboBox::currentConfigFilename() const {
    return itemData(currentIndex()).toString();
}

QString QFESPIMB040ConfigComboBox::currentConfig() const {
    return getConfigName(currentConfigFilename());
}

void QFESPIMB040ConfigComboBox::setIcon(QIcon icon) {
    this->icon=icon;
    for (int i=0; i<count(); i++) {
        setItemIcon(i, getConfigIcon(itemData(i).toString()));
    }
}

void QFESPIMB040ConfigComboBox::init(QString baseDir, QString extension) {
    this->baseDir=baseDir;
    this->m_ext=extension;

    clear();

    QDir dir=QDir(baseDir, "*."+m_ext);
    //std::cout<<"should have created all dirs: '"<<directory.toStdString()<<"'"<<std::endl;
    QStringList filenames;
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString absfn=dir.absoluteFilePath(fileName);
        addItem(getConfigIcon(absfn), getConfigName(absfn), absfn);
    }

}

void QFESPIMB040ConfigComboBox::setCurrentConfigFilename(QString filename) {
    setCurrentIndex(findData(filename));
}

void QFESPIMB040ConfigComboBox::setCurrentConfig(QString config)  {
    setCurrentIndex(findText(config));
}

void QFESPIMB040ConfigComboBox::cmbCurrentIndexChanged(int index) {
    emit currentConfigChanged(itemData(index).toString());
}

QString QFESPIMB040ConfigComboBox::getConfigName(QString filename) const {
    QSettings settings(filename, QSettings::IniFormat);
    return settings.value("name", "").toString();
}

QIcon QFESPIMB040ConfigComboBox::getConfigIcon(QString filename) const {
    QSettings settings(filename, QSettings::IniFormat);
    QIcon res=icon;
    QString ic=settings.value("name", QFileInfo(filename).baseName()).toString();
    if (!ic.isEmpty()) res=QIcon(ic);
    return res;

}
