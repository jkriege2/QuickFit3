#ifndef DLGFCCSFITALLCONFIGURATION_H
#define DLGFCCSFITALLCONFIGURATION_H

#include <QDialog>

namespace Ui {
    class DlgFCCSFitAllConfiguration;
}

class DlgFCCSFitAllConfiguration : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgFCCSFitAllConfiguration(QWidget *parent = 0);
        ~DlgFCCSFitAllConfiguration();

        bool getFitGuessed() const;
        bool getFitFitted() const;
        bool getFitAllRuns() const;
        
    private:
        Ui::DlgFCCSFitAllConfiguration *ui;
};

#endif // DLGFCCSFITALLCONFIGURATION_H
