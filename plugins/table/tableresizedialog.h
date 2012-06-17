#ifndef TABLERESIZEDIALOG_H
#define TABLERESIZEDIALOG_H

#include <QDialog>

namespace Ui {
    class TableResizeDialog;
}

class TableResizeDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit TableResizeDialog(QWidget *parent = 0);
        ~TableResizeDialog();

        void init(int width, int height);
        int newWidth() const;
        int newHeight() const;
        
    private:
        Ui::TableResizeDialog *ui;
};

#endif // TABLERESIZEDIALOG_H
