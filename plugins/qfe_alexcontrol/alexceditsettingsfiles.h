#ifndef ALEXCEDITSETTINGSFILES_H
#define ALEXCEDITSETTINGSFILES_H

#include <QDialog>

namespace Ui {
    class ALEXCEditSettingsFiles;
}

class ALEXCEditSettingsFiles : public QDialog
{
        Q_OBJECT

    public:
        explicit ALEXCEditSettingsFiles(const QString& channels, const QString& timing, const QString& ports, QWidget *parent = 0);
        ~ALEXCEditSettingsFiles();
        QString getChannels() const;
        QString getTiming() const;
        QString getPorts() const;

    private:
        Ui::ALEXCEditSettingsFiles *ui;
};

#endif // ALEXCEDITSETTINGSFILES_H
