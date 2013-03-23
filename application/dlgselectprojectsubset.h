#ifndef DLGSELECTPROJECTSUBSET_H
#define DLGSELECTPROJECTSUBSET_H

#include <QDialog>
#include "qfproject.h"
#include "qfprojectrawdatamodel.h"

namespace Ui {
    class DlgSelectProjectSubset;
}

class DlgSelectProjectSubset : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgSelectProjectSubset(QFProject* project, QWidget *parent = 0);
        ~DlgSelectProjectSubset();
        
    private:
        Ui::DlgSelectProjectSubset *ui;
        QFProject* project;
};

#endif // DLGSELECTPROJECTSUBSET_H
