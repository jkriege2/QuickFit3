#ifndef QFESPIMB040ACQUISITIONDESCRIPTION_H
#define QFESPIMB040ACQUISITIONDESCRIPTION_H

#include <QWidget>
#include <QSettings>
#include <QMap>
#include <QVariant>

namespace Ui {
    class QFESPIMB040AcquisitionDescription;
}

class QFESPIMB040AcquisitionDescription : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFESPIMB040AcquisitionDescription(QWidget *parent = 0);
        ~QFESPIMB040AcquisitionDescription();

        void loadSettings(QSettings &settings, QString prefix);
        void storeSettings(QSettings &settings, QString prefix) const;
        QMap<QString, QVariant> getDescription() const;

        int getCell() const;
        int getWell() const;
        int getPlate() const;
        QString getComment() const;
    protected slots:
        void on_btnClearAll_clicked();
        void on_btnNextCell_clicked();
        void updateTime();
        void spinChanged();
    private:
        Ui::QFESPIMB040AcquisitionDescription *ui;
};

#endif // QFESPIMB040ACQUISITIONDESCRIPTION_H
