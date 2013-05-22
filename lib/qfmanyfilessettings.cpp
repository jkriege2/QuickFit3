#include "qfmanyfilessettings.h"

QFManyFilesSettings::QFManyFilesSettings(QObject *parent):
    QObject(parent)
{
}

QFManyFilesSettings::~QFManyFilesSettings()
{
    clear();
}

void QFManyFilesSettings::addSettings(QSettings *settings, bool takeOwnership)
{
    this->settings.append(settings);
    if (takeOwnership) settings->setParent(this);
}

void QFManyFilesSettings::clear(bool deleteSettings)
{
    if (deleteSettings) {
        for (int i=0; i<settings.size(); i++) {
            if (settings[i]) delete settings[i];
        }
    }
    settings.clear();
}

QSettings *QFManyFilesSettings::getSettings(int level)
{
    return settings.value(level, NULL);
}

QVariant QFManyFilesSettings::value(const QString &key, const QVariant &defaultValue) const
{
    for (int i=settings.size()-1; i>=0; i--) {
        if (settings[i]) {
            if (settings[i]->contains(key)) return settings[i]->value(key, defaultValue);
        }
    }
    return defaultValue;
}

bool QFManyFilesSettings::setValue(const QString &key, const QVariant &value)
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
