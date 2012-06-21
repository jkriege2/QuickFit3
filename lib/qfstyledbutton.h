#ifndef QFSTYLEDBUTTON_H
#define QFSTYLEDBUTTON_H

#include <QAbstractButton>
#include "lib_imexport.h"
#include <QIcon>
#include <QItemEditorCreator>

/*! \brief this is a specialized button which is not drawn as a standard button, but with
           user supplied  images for different states
    \ingroup qf3lib_widgets

    The user may supply these images:
      - normal: the default appearance of the button
      - disabled: appearance, when button is disabled
      - hover: when the mouse hover over the button
      - pressed: when the mouse button is pressed
    .
    You can also supply a single QIcon which is used to derive the above states.

    In addition this button allows to set a buddy QWidget. If it provides a user property,
    and one of the special function modes is set this user property is used to execute the
    specified function. Possible implemented functions are:
      - open an URL
      - execute/open a file (possibly using a specified basePath to decode relative file paths)
      - select a new file
    .
    You can reset this behaviour by calling \code setBuddy(NULL, QFStyledButton::None) \endcode .

 */
class QFLIB_EXPORT QFStyledButton : public QAbstractButton {
        Q_OBJECT
    public:
        enum ActionMode { None=0, OpenURL=1, ExecuteFile=2, SelectFile=3, OpenPrependedURL=4, ClearLineEdit=5 };

        QFStyledButton(const QIcon& icon, QWidget* parent=NULL);
        QFStyledButton(QAction* action, QWidget* parent=NULL);
        /** \brief default constructor which loads one of the three predefined icon sets */
        QFStyledButton(ActionMode mode, QWidget* buddy, QWidget* parent=NULL);
        virtual ~QFStyledButton();

        /** \brief set the normal pixmap */
        void setNormalIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setDisabledIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setHoverIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setPressedIcon(const QPixmap& pixmap);



        QWidget* buddy() const;
        /** \brief set buddy + action */
        void setBuddy(QWidget* b, ActionMode=ExecuteFile);
        void setBuddyWithDefaultIcon(QWidget* b, ActionMode=ExecuteFile);

        virtual QSize sizeHint()const;
        virtual QSize minimumSizeHint()const;

        void setBasepath(QString path);
        void setPrependURL(QString prepend);

        void setAction(QAction* action);

    signals:
        void clicked(QString buddyContents);
    protected:
        virtual void paintEvent(QPaintEvent* e);
        virtual void enterEvent(QEvent* e);
        virtual void leaveEvent(QEvent* e);
        QIcon image;
        QWidget* m_buddy;
        QString m_basepath;
        ActionMode m_actionmode;
        QString m_prependURL;
        QAction* m_action;
        bool hover;
    protected slots:
        void openBuddyContents();
        void actionChanged();

    private:
};

#endif // QFSTYLEDBUTTON_H
