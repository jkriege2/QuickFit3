#ifndef QFSELECTFILESLISTWIDGET_H
#define QFSELECTFILESLISTWIDGET_H

#include <QWidget>
#include "libwid_imexport.h"
#include "qfwizard.h"

namespace Ui {
    class QFSelectFilesListWidget;
}

class QFWIDLIB_EXPORT QFSelectFilesListWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit QFSelectFilesListWidget(QWidget *parent = 0);
        ~QFSelectFilesListWidget();
        void setFilters(const QStringList& filters);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;

    protected slots:
        void on_btnAdd_clicked();
        void on_btnRemove_clicked();
        void on_btnUp_clicked();
        void on_btnDown_clicked();
    private:
        Ui::QFSelectFilesListWidget *ui;
        QStringList filters;
        QString lastDirID;
        QString lastFilterID;
};


class QFWIDLIB_EXPORT QFSelectFilesWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFSelectFilesWizardPage(const QString &title, QWidget *parent = 0);
        QFSelectFilesListWidget* getSelFilesWidget() const {
            return sel;
        }

        void setFilters(const QStringList& filters);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;
    signals:

    public slots:
    protected:
        QFSelectFilesListWidget *sel;

};

#endif // QFSELECTFILESLISTWIDGET_H
