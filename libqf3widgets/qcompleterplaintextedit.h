#ifndef QCOMPLETERPLAINTEXTEDIT_H
#define QCOMPLETERPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QCompleter>
#include <QtGui>

class QCompleterPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    /** Default constructor */
    QCompleterPlainTextEdit(QWidget* parent = 0);
    /** Default destructor */
    virtual ~QCompleterPlainTextEdit();
    void setCompleter(QCompleter *c);
    QCompleter *completer() const { return c; }
    void setAppendContents(bool app) { appendContents=app; }
    //void setSingleLine(bool app) { singleLine=app; }

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent ( QFocusEvent * event ) ;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *c;
    bool appendContents;
    //bool singleLine;
};

#endif // QCOMPLETERPLAINTEXTEDIT_H
