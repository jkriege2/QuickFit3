#ifndef USERFITFUNCTIONSEDITOR_H
#define USERFITFUNCTIONSEDITOR_H

#include <QDialog>

namespace Ui {
    class UserFitFunctionsEditor;
}

class UserFitFunctionsEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit UserFitFunctionsEditor(QWidget *parent = 0);
        ~UserFitFunctionsEditor();
        
    private:
        Ui::UserFitFunctionsEditor *ui;
};

#endif // USERFITFUNCTIONSEDITOR_H
