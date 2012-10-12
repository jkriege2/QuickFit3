#ifndef DLGNEWVERSION_H
#define DLGNEWVERSION_H

#include <QDialog>

namespace Ui {
    class DlgNewVersion;
}

class DlgNewVersion : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgNewVersion(QWidget *parent = 0);
        ~DlgNewVersion();
        
    private:
        Ui::DlgNewVersion *ui;
};

#endif // DLGNEWVERSION_H
