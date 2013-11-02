#ifndef QFRDRIMAGESTACK3DVIEWER_H
#define QFRDRIMAGESTACK3DVIEWER_H

#include <QWidget>
#include <QPointer>

class QFRDRImageStackData; // forward


namespace Ui {
    class QFRDRImageStack3DViewer;
}

class QFRDRImageStack3DViewer : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRImageStack3DViewer(QWidget *parent = 0);
        ~QFRDRImageStack3DViewer();
        void init(QFRDRImageStackData* rdr, int stack);
    public slots:
        void showHelp();
        void copyImage();
        void printImage(QPrinter *p=0);
        void saveImage(const QString &filename=QString());
    protected:
        void saveSettings();
        void closeEvent(QCloseEvent *event);
    private:
        Ui::QFRDRImageStack3DViewer *ui;
        QPointer<QFRDRImageStackData> rdr;
        int stack;
};

#endif // QFRDRIMAGESTACK3DVIEWER_H
