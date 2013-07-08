#ifndef IMFCSCALIBRATIONDIALOG_H
#define IMFCSCALIBRATIONDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
    class ImFCSCalibrationDialog;
}

class ImFCSCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImFCSCalibrationDialog(QWidget *parent = 0);
    ~ImFCSCalibrationDialog();

    QList<double> getValues() const;
    double getFocusHeight() const;
    double getFocusHeightError() const;
    bool getFixOffset() const;

    void setFitModels(const QStringList& models, const QString& defaultModel);
    QString getFitModel() const;
protected slots:

    void on_btnHelp_clicked();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::ImFCSCalibrationDialog *ui;
};

#endif // IMFCSCALIBRATIONDIALOG_H
