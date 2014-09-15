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
        explicit QFCollapsibleFrame(QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleFrame(const QPixmap& pix, const QString& title, QWidget *parent = 0);

        QString title() const;
        bool isOpened() const;
        const QPixmap *icon() const;
        int separation() const;
        int headerSeparation() const;

        void setLayout(QLayout* layout);
        QLayout* layout();

        void setWidget(QWidget* widget);
        QWidget* widget() const;

        QFormLayout* setFormLayout();
        QHBoxLayout* setHBoxLayout();
        QVBoxLayout* setVBoxLayout();
        QGridLayout* setGridLayout();

        //virtual QSize sizeHint() const;
        //virtual QSize minimumSizeHint() const;
    signals:
        void toggled(bool opened);
    public slots:
        void setTitle(const QString &title);
        void setIcon(const QPixmap& icon);
        void setOpened(bool opened);
        void setSeparation(int separation);
        void setHeaderSeparation(int separation);

        void invalidate();
        void toggle();
    protected slots:
        void buttonEntered();
        void buttonLeft();
    private:
        //QString m_title;
        //QPixmap m_icon;
        bool m_opened;
        QPixmap m_iconOpened;
        QPixmap m_iconClosed;
        QPixmap m_iconOpenedHover;
        QPixmap m_iconClosedHover;
        //int m_border;
        //int m_separation;
        //int m_headerSeparation;

        QFRotatableLabel* labIcon;
        QFRotatableLabel* labTitle;
        QFRotatableLabel* labButton;
        QFrame* frameHeader;

        QVBoxLayout* mainLayout;
        QHBoxLayout* headerLayout;
        QWidget* widMain;
        bool privateMain;


    protected:
        virtual void init();
        //virtual void paintEvent(QPaintEvent* event);
        //int getHeaderHeight() const;
        //int getHeaderWidth() const;
        //virtual void myDrawFrame(QPainter *p);
};


class QFLIB_EXPORT QFCollapsibleForm : public QFCollapsibleFrame
{
        Q_OBJECT
    public:
        explicit QFCollapsibleForm(QWidget *parent = 0): QFCollapsibleFrame(parent) {};
        explicit QFCollapsibleForm(const QString& title, QWidget *parent = 0): QFCollapsibleFrame(title, parent) {};
        explicit QFCollapsibleForm(const QPixmap& pix, const QString& title, QWidget *parent = 0): QFCollapsibleFrame(pix, title, parent) {};

        void addRow(const QString& label, QWidget* widget) {
            m_form->addRow(label, widget);
        }
        void addRow(QWidget* label, QWidget* widget) {
            m_form->addRow(label, widget);
        }

        void addRow(const QString& label, QLayout* widget) {
            m_form->addRow(label, widget);
        }
        void addRow(QWidget* label, QLayout* widget) {
            m_form->addRow(label, widget);
        }
        void addRow(QWidget* widget) {
            m_form->addRow( widget);
        }

        void addRow(QLayout* widget) {
            m_form->addRow( widget);
        }

        int count() const {
            return m_form->count();
        }

        int rowCount() const {
            return m_form->rowCount();
        }

        QFormLayout* form() const {
            return m_form;
        }

        void insertRow(int row, const QString& label, QWidget* widget) {
            m_form->insertRow(row, label, widget);
        }
        void insertRow(int row, QWidget* label, QWidget* widget) {
            m_form->insertRow(row, label, widget);
        }

        void insertRow(int row, const QString& label, QLayout* widget) {
            m_form->insertRow(row, label, widget);
        }
        void insertRow(int row, QWidget* label, QLayout* widget) {
            m_form->insertRow(row, label, widget);
        }
        void insertRow(int row, QWidget* widget) {
            m_form->insertRow(row,  widget);
        }

    private:
        virtual void init();
        QFormLayout* m_form;
};


#endif // QFCOLLAPSIBLEFRAME_H

