#ifndef QFESPIMB040LIGHTPATHSAVEDIALOG_H
#define QFESPIMB040LIGHTPATHSAVEDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include "qftablemodel.h"
#include <QList>

namespace Ui {
    class QFESPIMB040LightPathSaveDialog;
}

class QFESPIMB040LightPathSaveDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESPIMB040LightPathSaveDialog(QString name, QStringList names, QFTableModel *model, QWidget *parent = 0);
        ~QFESPIMB040LightPathSaveDialog();

        QString getName() const;
        QList<bool> getCheckedItems() const;
        
    private:
        Ui::QFESPIMB040LightPathSaveDialog *ui;
        QFTableModel* m_model;
};

#endif // QFESPIMB040LIGHTPATHSAVEDIALOG_H
