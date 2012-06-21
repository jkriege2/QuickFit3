#ifndef QFRDRREPLACEDIALOG_H
#define QFRDRREPLACEDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRReplaceDialog;
}

class QFRDRReplaceDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRReplaceDialog(QWidget *parent = 0);
        ~QFRDRReplaceDialog();

        bool getReplaceName() const;
        bool getReplaceFolder() const;
        QString getFindString() const;
        QString getReplaceString() const;
        bool getCaseSensitivity() const;
        
    private:
        Ui::QFRDRReplaceDialog *ui;
};

#endif // QFRDRREPLACEDIALOG_H
