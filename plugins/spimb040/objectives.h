#ifndef OBJECTIVES_H
#define OBJECTIVES_H

#include <QDialog>
#include <QString>
#include <QWidget>

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): description of an objective
    \ingroup qf3ext_spimb040


 */
struct ObjectiveDescription {
    QString name;
    QString manufacturer;
    double magnification;
    double NA;
    ObjectiveDescription();
};



namespace Ui {
    class QFESPIMB040ObjectiveEditor;
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): editor for objective data
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040ObjectiveEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QFESPIMB040ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent = 0);
    ~QFESPIMB040ObjectiveEditor();
    void init(const ObjectiveDescription& objective);
    ObjectiveDescription getData();
private:
    Ui::QFESPIMB040ObjectiveEditor *ui;
};

#endif // OBJECTIVES_H
