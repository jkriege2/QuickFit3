#include "qfmanyfilessettings.h"
#include <QStringList>

QFManyFilesSettings::QFManyFilesSettings(QObject *parent):
    QObject(parent)
{
}

QFManyFilesSettings::~QFManyFilesSettings()
{
    clear();
}

void QFManyFilesSettings::addSettings(QSettings *settings, bool takeOwnership, bool readony)
{
    this->settings.append(settings);
    if (takeOwnership) settings->setParent(this);
    if (readony) this->readonly.insert(settings);
}

void QFManyFilesSettings::clear(bool deleteSettings)
{
    if (deleteSettings) {
        for (int i=0; i<settings.size(); i++) {
            if (settings[i]) delete settings[i];
        }
    }
    settings.clear();
    readonly.clear();
}

QSettings *QFManyFilesSettings::getSettings(int level)
{
    return settings.value(level, NULL);
}

QVariant QFManyFilesSettings::value(const QString &key, const QVariant &defaultValue, int* level) const
{
    for (int i=settings.size()-1; i>=0; i--) {
        if (settings[i]) {
            if (settings[i]->contains(key)) {
                if (level) *level=i;
                return settings[i]->value(key, defaultValue);
            }
        }
    }
    return defaultValue;
}

bool QFManyFilesSettings::setValue(const QString &key, const QVariant &value, int *level)
{
    int foundOnLevel=settings.size()-1;
    for (int i=settings.size()-1; i>=0; i--) {
        if (settings[i]) {
            if (settings[i]->contains(key)) {
                foundOnLevel=i;
                break;
            }
        }
    }
    if (level) *level=foundOnLevel;
    for (int i=foundOnLevel; i>=0; i--) {
        if (settings[i]) {
            if (settings[i]->isWritable()) {
                settings[i]->setValue(key, value);
                return true;
            }
        }
    }
    return false;
}

QStringList QFManyFilesSettings::childGroups() const
{
    QStringList sl;
    for (int i=foundOnLevel; i>=0; i--) {
        if (settings[i]) {
            if (settings[i]->isWritable()) {
                sl<<settings[i]->childGroups();
            }
        }
    }
    sl.removeDuplicates();
    return sl;
}
