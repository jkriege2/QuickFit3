#ifndef QFCOLLAPSIBLEFRAME_H
#define QFCOLLAPSIBLEFRAME_H

#include <QFrame>

#include "lib_imexport.h"
#include "programoptions.h"
#include "qfrotatablelabel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>

/*! \brief a frame, which has a tile+icon and a button, that collapses/expands its contents
    \ingroup qf3lib_widgets

    Use the implementations of  setWidget() and setLayout() to add a widget or layout for the contents of this frame!


 */
class QFLIB_EXPORT QFCollapsibleFrame : public QFrame
{
        Q_OBJECT
    public:
        enum HeaderPosition {
            North=0,
            West=1,
            South=2,
            East=3
        };

        explicit QFCollapsibleFrame(QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QPixmap& pix, const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleFrame(HeaderPosition pos, QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QString& title, HeaderPosition pos, QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QPixmap& pix, const QString& title, HeaderPosition pos, QWidget *parent = 0);

        QString title() const;
        bool isOpened() const;
        const QPixmap *icon() const;
        int separation() const;
        int headerSeparation() const;
        HeaderPosition headerPos() const;

        /** \brief use a layout for the contents
         *
         *  If you used setWidget() before, the explicitly set widget is removed and replaced by an internal new QWidget
         */
        void setLayout(QLayout* layout);
        QLayout* layout();

        /** \brief use a widget for the contents */
        void setWidget(QWidget* widget);
        QWidget* widget() const;

        /** \brief add a QFormLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QFormLayout *setFormLayout();
        /** \brief add a QHBoxLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QHBoxLayout *setHBoxLayout();
        /** \brief add a QVBoxLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QVBoxLayout *setVBoxLayout();
        /** \brief add a QGridLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QGridLayout *setGridLayout();


    signals:
        void toggled(bool opened);
    public slots:
        /** \brief title of the box */
        void setTitle(const QString &title);
        /** \brief icon for the box */
        void setIcon(const QPixmap& icon);
        /** \brief set opened state */
        void setOpened(bool opened);
        /** \brief separation between header items (pixels) */
        void setSeparation(int separation);
        /** \brief separation between header and widgets */
        void setHeaderSeparation(int separation);
        /** \brief position of the header */
        void setHeaderPosition(HeaderPosition pos);
        /** \brief toggle opened state */
        void toggle();


        void invalidate();
    protected slots:
        void buttonEntered();
        void buttonLeft();
    private:

        bool m_opened;
        QPixmap m_iconOpened;
        QPixmap m_iconClosed;
        QPixmap m_iconOpenedHover;
        QPixmap m_iconClosedHover;


        QFRotatableLabel* labIcon;
        QFRotatableLabel* labTitle;
        QFRotatableLabel* labButton;
        QFrame* frameHeader;

        HeaderPosition m_pos;

        QBoxLayout* mainLayout;
        QBoxLayout* headerLayout;
        QWidget* widMain;
        bool privateMain;



    protected:
        virtual void init();

};


class QFLIB_EXPORT QFCollapsibleForm : public QFCollapsibleFrame
{
        Q_OBJECT
    public:
        explicit QFCollapsibleForm(QWidget *parent = 0): QFCollapsibleFrame(parent) {}
        explicit QFCollapsibleForm(const QString& title, QWidget *parent = 0): QFCollapsibleFrame(title, parent) {}
        explicit QFCollapsibleForm(const QPixmap& pix, const QString& title, QWidget *parent = 0): QFCollapsibleFrame(pix, title, parent) {}
        explicit QFCollapsibleForm(HeaderPosition pos, QWidget *parent = 0): QFCollapsibleFrame(pos, parent) {}
        explicit QFCollapsibleForm(const QString& title, HeaderPosition pos, QWidget *parent = 0): QFCollapsibleFrame(title, pos, parent) {}
        explicit QFCollapsibleForm(const QPixmap& pix, const QString& title, HeaderPosition pos, QWidget *parent = 0): QFCollapsibleFrame(pix, title, pos, parent) {}

        inline void addRow(const QString& label, QWidget* widget) {
            m_form->addRow(label, widget);
        }
        inline void addRow(QWidget* label, QWidget* widget) {
            m_form->addRow(label, widget);
        }

        inline void addRow(const QString& label, QLayout* widget) {
            m_form->addRow(label, widget);
        }
        inline void addRow(QWidget* label, QLayout* widget) {
            m_form->addRow(label, widget);
        }
        inline void addRow(QWidget* widget) {
            m_form->addRow( widget);
        }

        inline void addRow(QLayout* widget) {
            m_form->addRow( widget);
        }

        inline int count() const {
            return m_form->count();
        }

        inline int rowCount() const {
            return m_form->rowCount();
        }

        inline QFormLayout* form() const {
            return m_form;
        }

        inline void insertRow(int row, const QString& label, QWidget* widget) {
            m_form->insertRow(row, label, widget);
        }
        inline void insertRow(int row, QWidget* label, QWidget* widget) {
            m_form->insertRow(row, label, widget);
        }

        inline void insertRow(int row, const QString& label, QLayout* widget) {
            m_form->insertRow(row, label, widget);
        }
        inline void insertRow(int row, QWidget* label, QLayout* widget) {
            m_form->insertRow(row, label, widget);
        }
        inline void insertRow(int row, QWidget* widget) {
            m_form->insertRow(row,  widget);
        }

    private:
        inline virtual void init() {
            QFCollapsibleFrame::init();
            m_form=setFormLayout();
        }

        QFormLayout* m_form;
};


#endif // QFCOLLAPSIBLEFRAME_H

