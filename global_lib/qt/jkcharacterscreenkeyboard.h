#ifndef JKCHARACTERSCREENKEYBOARD_H
#define JKCHARACTERSCREENKEYBOARD_H

#include <QWidget>
#include <QAbstractButton>
#include <QString>
#include <QList>
#include <QSize>
#include "flowlayout.h"


/*! \brief displays a set of buttons that allow to insert special characters

    You may display this widget as a stay-on-top tool window and then call \c setKBFocusWidget(w)
    where \c w is the main window of your application. If generateFocusEvent is set \c true (default)
    this widget will post a QKeyEvent message to the focusWidget obtained from KBFocusWidget() .
*/
class JKCharacterScreenKeyboard : public QWidget {
        Q_OBJECT
    public:
        JKCharacterScreenKeyboard(QWidget* parent=NULL);
        virtual ~JKCharacterScreenKeyboard();
        /** \brief set the set of characters that should be displayed as buttons */
        void setCharacters(const QString& characters);
        /** \brief return the set of characters that is currently displayed as buttons */
        QString characters() const;
        /** \brief if set \c true this widget will emit its characterClicked() signal AND send a QKeyEvent to the currently focused widget */
        void setGenerateFocusEvent(bool enable);
        /** \brief return whether the currently focused widget should receive a QKeyEvent event */
        bool generateFocusEvent() const;
        /** \brief set the widget that should be used to look up the focus widget when generateFocusEvent is set \c true */
        void setKBFocusWidget(QWidget* focus);
        /** \brief get the widget that should be used to look up the focus widget when generateFocusEvent is set \c true */
        QWidget* KBFocusWidget();
        /** \brief return the size of the buttons */
        QSize buttonSize() const;
        /** \brief set the size of the buttons */
        void setButtonSize(QSize s);
    signals:
        /** \brief emitted when one of the internal buttons is clicked */
        void characterClicked(QChar character);
    protected slots:
        void buttonClicked();
    protected:
        FlowLayout* lay;
        QSize m_buttonSize;
        QString m_characters;
        QWidget* m_focus;
        bool m_generateApplicationEvent;
        QList<QAbstractButton*> m_buttons;
        /** \brief create new buttons and display them */
        void populateLayout();
    private:
};

#endif // JKCHARACTERSCREENKEYBOARD_H
