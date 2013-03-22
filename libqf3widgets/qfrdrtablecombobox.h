#ifndef QFRDRTABLECOMBOBOX_H
#define QFRDRTABLECOMBOBOX_H

#include "qfrdrcombobox.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfrdrtableinterface.h"
#include "libwid_imexport.h"
#include "qfmatchrdrfunctor.h"

/** \brief a special QFMatchRDRFunctor which only matches records that implement QFRDRTableInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFWIDLIB_EXPORT QFRDRTableComboBoxMatchFunctor: public QFMatchRDRFunctor {
    public:
        QFRDRTableComboBoxMatchFunctor(bool requireColumnGraphics=false, bool requireNotReadonly=false);
        virtual bool matches(const QFRawDataRecord* record) const ;
        void setRequireColumnGraphics(bool require);
        void setRequireNotReadonly(bool require);
    protected:
        bool requireColumnGraphics;
        bool requireNotReadonly;
};



/** \brief a special QFRDRComboBox which only displays records that implement QFRDRTableInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFWIDLIB_EXPORT QFRDRTableComboBox : public QFRDRComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRTableComboBox(QWidget *parent = 0);
        virtual ~QFRDRTableComboBox();
        virtual void init(QFProject* project);
        QFRDRColumnGraphsInterface* currentColumnGraphics() const;
        QFRDRTableInterface* currentTable() const;
    signals:
        void currentColumnGraphicsChanged(QFRDRColumnGraphsInterface* graphics);
        void currentTableChanged(QFRDRTableInterface* table);

    public slots:
        void setRequireColumnGraphics(bool require);
        void setRequireNotReadonly(bool require);
    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFRDRTableComboBoxMatchFunctor* tabFunctor;
};


/** \brief a special QComboBox which displays all graphs available in a QFRDRColumnGraphsInterface
  * \ingroup qf3lib_widgets
  *
  */
class QFWIDLIB_EXPORT QFRDRColumnGraphsComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRColumnGraphsComboBox(QWidget *parent = 0);
        virtual ~QFRDRColumnGraphsComboBox();
        QFRDRColumnGraphsInterface* currentColumnGraphics() const;
    signals:
        void refilledEmpty();
        void refilled(bool full);
    public slots:
        void setColumnGraphs(QFRDRColumnGraphsInterface* cols);
        void refill();
    protected slots:
        virtual void myCurrentIndexChanged(int i);
    protected:
        QFRDRColumnGraphsInterface* cols;
};

#endif // QFRDRTABLECOMBOBOX_H
