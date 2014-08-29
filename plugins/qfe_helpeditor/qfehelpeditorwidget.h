#ifndef QFEHelpEditorWidget_H
#define QFEHelpEditorWidget_H


#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include "qtriple.h"
#include "qrecentfilesmenu.h"
#include "finddialog.h"
#include "replacedialog.h"
#include <QStringListModel>
#include "qfhtmlhighlighter.h"
namespace Ui {
    class QFEHelpEditorWidget;
}



class QFEHelpEditorWidget : public QWidget {
        Q_OBJECT

    public:
        explicit QFEHelpEditorWidget(QWidget* parent=NULL);
        ~QFEHelpEditorWidget();

        QString getScript() const;

        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

        void setWinID(int m_winID);


    protected slots:
        void autosave();
        void reloadLastAutosave();
        void documentWasModified();
        void on_btnExecute_clicked();
        void on_btnNew_clicked();
        void on_btnOpen_clicked();
        bool save();
        bool saveAs();
        bool saveFile(const QString &filename, bool setFilename=true);
        void on_btnSave_clicked();
        void on_btnSaveAs_clicked();
        void on_btnOpenExample_clicked();
        void on_btnOpenTemplate_clicked();
        void on_btnOpenTemplate2_clicked();
        void edtScript_cursorPositionChanged();
        void on_btnHelp_clicked();

        void openScript(QString dir, bool saveDir=true, const QString& filename=QString());
        void openScriptNoAsk(QString filename);

        void on_btnBold_clicked();
        void on_btnItalic_clicked();
        void on_btnUnderline_clicked();
        void on_btnImage_clicked();
        void on_btnAnchor_clicked();
        void on_btnLink_clicked();
        void on_btnBlockquote_clicked();
        void on_btnNumberedList_clicked();
        void on_btnBulletList_clicked();
        void on_btnInsertH1_clicked();
        void on_btnInsertH2_clicked();
        void on_btnInsertH3_clicked();
        void on_btnInsertH4_clicked();
        void on_btnInsertParagraph_clicked();
        void on_btnInsertListItem_clicked();
        void on_btnPasteImage_clicked();
        void on_btnInsertAndCopyImage_clicked();
        void on_btnInsertCode_clicked();
        void on_btnInsertMath_clicked();
        void on_btnInsertBMath_clicked();
        void on_btnInsertPluginLink_clicked();
        void on_btnInsertSubPluginLink_clicked();
        void on_btnCenter_clicked();
        void on_btnInsertFAQ_clicked();
        void on_btnFunctionReference_clicked();
        //void on__clicked();

        /** \brief replaces the currently selected text by newText, hwere %1 is replaced by the selected text*/
        void insertAroundOld(const QString& newText);







        /** \brief slot: find the first occurence of the text */
        void findFirst();

        /** \brief slot: find the next occurence of the text */
        void findNext();

        /** \brief slot: replace the first occurence of the text */
        void replaceFirst();

        /** \brief slot: replace the next occurence of the text */
        void replaceNext();

        /** \brief slot: goto a user selectable line in the text */
        void gotoLine();

        /** \brief slot: print the SDFF document */
        void print();

        /** \brief slot: preview print of the SDFF document */
        void printPreviewClick();

        /** \brief slot: used by printPreview() */
        void printPreview(QPrinter *printer);
        /** \brief set the "find next", ... actions to disabled (this is called whenever a new file is loaded or created) */
        void clearFindActions();

        void insertActionClicked();

        void insertIcon();

    protected:
        bool maybeSave();
        void setScriptFilename(QString filename);
        void closeEvent(QCloseEvent* event);

        QString currentScript;
    private:
        int m_winID;
        Ui::QFEHelpEditorWidget *ui;
        QRecentFilesMenu* recentHelpFiles;

        QStringList defaultWords;
        QString lastScript;

        bool modified;

        QFHTMLHighlighter* highlighter;
        QAction* actLoadAutosave;
        QAction *cutAct;
        /** \brief action object: copy the currently selected text */
        QAction *copyAct;
        /** \brief action object: paste some text */
        QAction *pasteAct;
        /** \brief action object: undo text changes */
        QAction *undoAct;
        /** \brief action object: redo text changes */
        QAction *redoAct;
        /** \brief action object: find text */
        QAction *findAct;
        /** \brief action object: find next occurence text */
        QAction *findNextAct;
        /** \brief action object: find and replace text */
        QAction *replaceAct;
        /** \brief action object: run current SDFF script */
        QAction *runSDFFAct;
        /** \brief action object: comment text */
        QAction *commentAct;
        /** \brief action object: uncomment text */
        QAction *unCommentAct;
        /** \brief action object: incease indention */
        QAction *indentAct;
        /** \brief action object: decrease indention */
        QAction *unindentAct;
        /** \brief action object: goto line */
        QAction *gotoLineAct;
        /** \brief action object: print */
        QAction *printAct;

        QAction* actInsertIcon;

        QCompleter* completer;


        /** \brief the "Find ..." dialog object */
        FindDialog* findDlg;
        /** \brief the "Find & Replace..." dialog object */
        ReplaceDialog* replaceDlg;
        QStringListModel* completermodel;

        /** \brief load a completer model from a file */
        QStringListModel* modelFromFile(const QString& fileName);

        void addInsertAction(const QString& insert);
        void addInsertAction(const QString& label, const QString& insert);
        void addInsertAction(QMenu* menu, const QString& insert);
        void addInsertAction(QMenu* menu, const QString& label, const QString& insert);
        QMap<QAction*, QString> insertmap;

};









#endif // QFEHelpEditorWidget_H
