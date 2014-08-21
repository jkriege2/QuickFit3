#ifndef DLGRDRSETPROPERTY_H
#define DLGRDRSETPROPERTY_H

#include <QDialog>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"
#include "qftools.h"

namespace Ui {
class dlgRDRSetProperty;
}

class dlgRDRSetProperty : public QDialog {
        Q_OBJECT

    public:
        explicit dlgRDRSetProperty(QWidget *parent = 0);
        ~dlgRDRSetProperty();

        void init(QFProject* project);
        bool doOverwrite() const;
        bool doCreateNew() const;
        int whichToSet() const;

        QList<QPointer<QFRawDataRecord> > getSelectedRDRs() const;
        QStringList getNewPropNames() const;
        QList<QVariant> getNewPropValues() const;

    protected slots:
        void on_btnSelectNone_clicked();
        void on_btnSelectAll_clicked();
        void showHelp();
        void on_cmbWhich_currentIndexChanged(int idx);
    private:
        Ui::dlgRDRSetProperty *ui;
        QFProject* project;
        QList<QPointer<QFRawDataRecord> > rdrs;
};

#endif // DLGRDRSETPROPERTY_H
