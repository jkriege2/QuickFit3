#ifndef USERFITFUNCTIONSEDITOR_H
#define USERFITFUNCTIONSEDITOR_H

#include <QDialog>
#include <QListWidgetItem>
#include "userfitfunctionmodel.h"
#include "../plugins/base_classes/qffunctionreferencetool.h"

namespace Ui {
    class UserFitFunctionsEditor;
}

class UserFitFunctionsEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit UserFitFunctionsEditor(QWidget *parent = 0);
        ~UserFitFunctionsEditor();

    protected:
        void clearFF();
        void showFF(const QString& id);
        bool storeCurrentFF();
        void init();
    protected slots:
        void on_btnNewF_clicked();
        void on_btnDeleteF_clicked();
        void on_btnSaveEdit_clicked();
        void on_btnCancelEdit_clicked();
        void on_lstFunctions_itemDoubleClicked(QListWidgetItem* item);
        void on_btnUp_clicked();
        void on_btnDown_clicked();
        void on_btnAdd_clicked();
        void on_btnDelete_clicked();
        void on_edtExpression_textChanged(const QString &text);
        void on_btnFunctionHelp_clicked();
        void showHelp();
    private:
        Ui::UserFitFunctionsEditor *ui;
        QString currentID;
        UserFitFunctionModel model;
        QFFunctionReferenceTool* functionRef;
};


#endif // USERFITFUNCTIONSEDITOR_H
