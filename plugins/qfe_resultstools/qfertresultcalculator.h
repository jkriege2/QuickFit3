#ifndef QFERTRESULTCALCULATOR_H
#define QFERTRESULTCALCULATOR_H

#include <QDialog>
#include "qfrawdatarecord.h"

namespace Ui {
class QFERTResultCalculator;
}

class QFERTResultCalculator : public QDialog
{
    Q_OBJECT
    
public:
    explicit QFERTResultCalculator(QWidget *parent = 0);
    ~QFERTResultCalculator();

    QString getExpression() const;
    QFRawDataRecord* getRecA() const;
    QFRawDataRecord* getRecB() const;
    QString getEvalIDA() const;
    QString getResultIDA() const;
    QString getEvalIDB() const;
    QString getResultIDB() const;
    QString getNewResultName() const;
public slots:
    void eval();
protected slots:
    void checkBoxesChanged();
    void on_btnHelp_clicked();
    void on_cmbResultA_currentResultChanged(QString result, QString evalID);
    void on_cmbResultB_currentResultChanged(QString result, QString evalID);
private:
    Ui::QFERTResultCalculator *ui;
};

#endif // QFERTRESULTCALCULATOR_H
