#ifndef DLGCOLORBARCOLORING_H
#define DLGCOLORBARCOLORING_H

#include <QDialog>

namespace Ui {
    class DlgColorbarColoring;
}

class DlgColorbarColoring : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgColorbarColoring(QWidget *parent = 0);
        ~DlgColorbarColoring();
        void init(const QStringList& graphs);
        QList<bool> getSelected() const;
        QList<QColor> getColors() const;
    private:
        Ui::DlgColorbarColoring *ui;
};

#endif // DLGCOLORBARCOLORING_H
