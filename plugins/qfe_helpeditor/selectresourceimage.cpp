#include "selectresourceimage.h"
#include "ui_selectresourceimage.h"

SelectResourceImage::SelectResourceImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectResourceImage)
{
    ui->setupUi(this);


    QStandardItem *parentItem = model.invisibleRootItem();
    listDirectory(QDir(":/"), parentItem);
    ui->treeView->setModel(&model);
}

SelectResourceImage::~SelectResourceImage()
{
    delete ui;
}

QString SelectResourceImage::getSelectFile() const
{
    return ui->treeView->currentIndex().data(Qt::UserRole+1).toString();
}

void SelectResourceImage::listDirectory(QDir dir, QStandardItem *parentItem)
{
    QStringList dirs=dir.entryList(QDir::AllDirs,QDir::Name);
    QStringList filters;
    filters<<"*.png"<<"*.jpg"<<"*.bmp";
    QStringList files=dir.entryList(filters,QDir::Files,QDir::Name);
    for (int i = 0; i < dirs.size(); ++i) {
        QStandardItem *item = new QStandardItem(dirs[i]);
        parentItem->appendRow(item);
        QDir d=dir;
        d.cd(dirs[i]);
        listDirectory(d, item);
    }
    for (int i = 0; i < files.size(); ++i) {
        QString abspath=dir.absoluteFilePath(files[i]);
        QStandardItem *item = new QStandardItem(QIcon(abspath), files[i]);
        item->setData(abspath);
        parentItem->appendRow(item);
    }
}
