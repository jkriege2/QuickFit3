#ifndef QFESPIMB040ACQUISITIONDESCRIPTION_H
#define QFESPIMB040ACQUISITIONDESCRIPTION_H

#include <QWidget>
#include <QSettings>
#include <QMap>
#include <QVariant>
#include "qfespimb040filenametool.h"

namespace Ui {
    class QFESPIMB040AcquisitionDescription;
};

class QFESPIMB040AcquisitionDescription : public QWidget, public QFESPIMB040FilenameTool {
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
        QString getPrefix() const;

        void setOtherSettingWidgets(QFESPIMB040OpticsSetup *setup, QFESPIMB040ExperimentDescription *exp);
    protected slots:
        void on_btnClearAll_clicked();
        void on_btnNextCell_clicked();
        void updateTime();
        void spinChanged();
        void updateReplaces();
    private:
        Ui::QFESPIMB040AcquisitionDescription *ui;
        QFESPIMB040OpticsSetup *opticsSetup;
        QFESPIMB040ExperimentDescription* expDescription;
};

#endif // QFESPIMB040ACQUISITIONDESCRIPTION_H
