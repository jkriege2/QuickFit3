#ifndef DLGFCCSRESETSETPARAMETER_H
#define DLGFCCSRESETSETPARAMETER_H

#include <QDialog>

namespace Ui {
    class DlgFCCSResetSetParameter;
}

class DlgFCCSResetSetParameter : public QDialog
{
        Q_OBJECT

    public:
        struct Parameter {
            QString id;
            QString label;
            double value;
            double error;
            bool fix;
            double rmin;
            double rmax;
        };

        explicit DlgFCCSResetSetParameter(QWidget *parent = 0);
        ~DlgFCCSResetSetParameter();

        void addParameter(const Parameter& p);
        void setParameters(const QList<Parameter>& p);
        void clearParams();
        bool setInAll() const;
        bool setInInitial() const;
        enum Operation {
            opSet,
            opReset,
            opNothing
        };
        Operation operationValue() const;
        Operation operationError() const;
        Operation operationFix() const;
        double getValue() const;
        double getError() const;
        bool getFix() const;
        bool setAllIndices();
        QString getParameter() const;

    protected slots:
        void updateWidgets();
        void on_cmbParam_currentIndexChanged();
        void dataEdited();
    private:
        Ui::DlgFCCSResetSetParameter *ui;
        QList<Parameter> params;

};

#endif // DLGFCCSRESETSETPARAMETER_H
