#ifndef ANDORSETTINGSDIALOG_H
#define ANDORSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class AndorSettingsDialog;
}

/*! \brief settings dialog for andor camera
    \ingroup qf3ext_andor

 */
class AndorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AndorSettingsDialog(QWidget *parent = 0);
    ~AndorSettingsDialog();

    /** \brief read dialog contents from QSettings object */
    void readSettings(const QSettings& settings, const QString& prefix);
    /** \brief write dialog contents to QSettings object */
    void writeSettings(QSettings& settings, const QString& prefix) const;

private:
    Ui::AndorSettingsDialog *ui;
};

#endif // ANDORSETTINGSDIALOG_H
