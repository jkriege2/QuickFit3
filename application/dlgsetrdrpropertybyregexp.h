#ifndef DLGSETRDRPROPERTYBYREGEXP_H
#define DLGSETRDRPROPERTYBYREGEXP_H

#include <QDialog>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include <QRegExp>
#include <QList>
#include <QPointer>


namespace Ui {
    class DlgSetRDRPropertyByRegExp;
}

class DlgSetRDRPropertyByRegExp : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgSetRDRPropertyByRegExp(QWidget *parent = 0);
        ~DlgSetRDRPropertyByRegExp();
        void setProject(QFProject* project);

        QList<QPointer<QFRawDataRecord> > getSelectedRDRs() const;

        QString getInputString(QFRawDataRecord* rdr) const;
        QString getResult(QFRawDataRecord* rdr, bool *apply=NULL, QString *error=NULL) const;
        QString transformOutput(QFRawDataRecord *rdr, const QString& o) const;
        void applyResult(QFRawDataRecord* rdr);
    protected slots:
        void on_btnSelectNone_clicked();
        void on_btnSelectAll_clicked();

    public slots:
        void updateTest();
    private:
        Ui::DlgSetRDRPropertyByRegExp *ui;
        QFProject* project;
        QList<QPointer<QFRawDataRecord> > rdrs;
};

#endif // DLGSETRDRPROPERTYBYREGEXP_H
