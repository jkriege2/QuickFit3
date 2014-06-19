#ifndef SELECTRESOURCEIMAGE_H
#define SELECTRESOURCEIMAGE_H

#include <QDialog>
#include <QDir>
#include <QStandardItemModel>

namespace Ui {
    class SelectResourceImage;
}

class SelectResourceImage : public QDialog
{
        Q_OBJECT

    public:
        explicit SelectResourceImage(QWidget *parent = 0);
        ~SelectResourceImage();

        QString getSelectFile() const;
    private:
        Ui::SelectResourceImage *ui;
        QStandardItemModel model;
    protected:
        void listDirectory(QDir dir, QStandardItem *parentItem);
};

#endif // SELECTRESOURCEIMAGE_H
