#ifndef DLGFIXFILEPATHS_H
#define DLGFIXFILEPATHS_H

#include <QDialog>

namespace Ui {
    class DlgFixFilepaths;
}

class DlgFixFilepaths : public QDialog
{
    Q_OBJECT

public:
    explicit DlgFixFilepaths(const QString& oldName, const QString& lastDir, QWidget *parent = 0);
    ~DlgFixFilepaths();
    QString getNewFilename() const;
    QString getLastDir() const;
    void init(const QString& oldName, const QString& lastDir);

private:
    Ui::DlgFixFilepaths *ui;
    QString lastDir;
protected slots:
    void selectFile();
    void on_btnApply_clicked();
    void on_btnIgnore_clicked();
    void on_btnCancel_clicked();
};

#endif // DLGFIXFILEPATHS_H
