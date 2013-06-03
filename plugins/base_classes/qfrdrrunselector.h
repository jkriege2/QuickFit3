#ifndef QFRDRRunSelector_H
#define QFRDRRunSelector_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include <QToolButton>
#include "qfrdrrunselection.h"
#include "qfrawdatarecord.h"
#include "qfrdrfcsdatainterface.h"


class QFRDRRunSelector : public QWidget
{
        Q_OBJECT
    public:
        explicit QFRDRRunSelector(QWidget *parent = 0);
        virtual  ~QFRDRRunSelector();
        void setRDR(QFRawDataRecord* record);

        int getCurrentRun() const;
    signals:
        void currentRunChanged(int run);
    public slots:
        void setCurrentRun(int run);


    protected:
        QFRawDataRecord* record;
        QFRDRRunSelectionsInterface* rrRecord;
        QFRDRFCSDataInterface* rfcs;

        int currentRun;
        int runmax;
        int runmin;

        QGridLayout* gl;
        QLabel* labInfo;

        QSpinBox* spinRun;
        QLabel* labRun;

        QToolButton* btnEditExclude;
        QAction* actEditExclude;
        QToolButton* btnEditInclude;
        QAction* actEditInclude;

        
};

#endif // QFRDRRunSelector_H
