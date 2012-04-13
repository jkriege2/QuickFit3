#include "qfespimb040configcombobox.h"
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

QFESPIMB040ConfigComboBox::QFESPIMB040ConfigComboBox(QWidget *parent) :
    QEnhancedComboBox(parent)
{
    updating=false;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCurrentIndexChanged(int)));
    baseDir=QApplication::applicationDirPath();
    m_ext="ini";
    icon=QIcon(":/spimb040/lightpath.png");
    clear();
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

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
    updateItems();
}

void QFESPIMB040ConfigComboBox::updateItems() {
    updateItems("");
}

void QFESPIMB040ConfigComboBox::updateItems(const QString &newCurrent) {
    int idx=currentIndex();
    updating=true;
    clear();
    addItem(tr("--- none ---"), "");

    QDir dir=QDir(baseDir, "*."+m_ext);
    //std::cout<<"should have created all dirs: '"<<directory.toStdString()<<"'"<<std::endl;
    QStringList filenames=dir.entryList(QDir::Files);
    QFESPIMB040OpticsSetupItems configs;
    foreach (QString fileName, filenames) {
        QString absfn=dir.absoluteFilePath(fileName);
        configs.append(qMakeTriple(getConfigIcon(absfn), getConfigName(absfn), absfn));
        addItem(getConfigIcon(absfn), getConfigName(absfn), absfn);
    }

    updating=false;
    emit configsChanged(configs);
    if (!newCurrent.isEmpty()) idx=findText(newCurrent);
    setCurrentIndex(idx);
}

QStringList QFESPIMB040ConfigComboBox::getConfigs() const {
    QStringList l;
    for (int i=1; i<count(); i++) l.append(itemText(i));
    return l;
}

void QFESPIMB040ConfigComboBox::setCurrentConfigFilename(QString filename) {
    setCurrentIndex(findData(filename));
}

void QFESPIMB040ConfigComboBox::setCurrentConfig(QString config)  {
    setCurrentIndex(findText(config));
}

void QFESPIMB040ConfigComboBox::cmbCurrentIndexChanged(int index) {
    if (!updating) emit currentConfigChanged(itemData(index).toString());
}

QString QFESPIMB040ConfigComboBox::getConfigName(QString filename) const {
    QSettings settings(filename, QSettings::IniFormat);
    return settings.value("name",  QFileInfo(filename).baseName()).toString();
}

QIcon QFESPIMB040ConfigComboBox::getConfigIcon(QString filename) const {
    QSettings settings(filename, QSettings::IniFormat);
    QIcon res=icon;
    QString ic=QFileInfo(filename).baseName()+".png";
    ic=settings.value("icon", ic).toString();
    if (QFile::exists(ic)) res=QIcon(ic);
    return res;
}
