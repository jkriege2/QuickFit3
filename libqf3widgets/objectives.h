#ifndef OBJECTIVES_H
#define OBJECTIVES_H

#include <QDialog>
#include <QString>
#include <QWidget>
#include "libwid_imexport.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): description of an objective
    \ingroup qf3ext_spimb040


 */
struct QFWIDLIB_EXPORT ObjectiveDescription {
    QString name;
    QString manufacturer;
    double magnification;
    double NA;
    ObjectiveDescription();
};



namespace Ui {
    class QF3ObjectiveEditor;
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): editor for objective data
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QF3ObjectiveEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QF3ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent = 0);
    ~QF3ObjectiveEditor();
    void init(const ObjectiveDescription& objective);
    ObjectiveDescription getData();
private:
    Ui::QF3ObjectiveEditor *ui;
};

#endif // OBJECTIVES_H
