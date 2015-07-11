#include "qmappablecombobox.h"
#include <iostream>

QMappableComboBox::QMappableComboBox(QWidget* parent):
    QComboBox(parent)
{
    m_imagesDir="";
}

QMappableComboBox::~QMappableComboBox()
{
}

void QMappableComboBox::setText(QString text) {
    //std::cout<<"QMappableComboBox::setText('"<<text.toStdString()<<"')\n";
    int idx=findText(text);
    if (idx>-1) {
        setCurrentIndex(idx);
    } else if (isEditable()) {
        setEditText(text);
    } else {
        setCurrentIndex(-1);
        setEditText("");
        clearEditText();
    }
}

void QMappableComboBox::setData(QVariant data) {
    //std::cout<<"QMappableComboBox::setData('"<<data.toString().toStdString()<<"')\n";
    int idx=findData(data);
    if (idx>-1) setCurrentIndex(idx);
    else setEditText("");
}

QString QMappableComboBox::text() const {
    return currentText();
}

QVariant QMappableComboBox::data() const {
    return itemData(currentIndex());
}

void QMappableComboBox::setFilename(QString fn) {
    disconnect(model(), SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )), this, SLOT(modelDataChanged ( const QModelIndex & , const QModelIndex &  )));
    disconnect(model(), SIGNAL(rowsInserted ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    disconnect(model(), SIGNAL(rowsRemoved ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    m_filename=fn;
    if (m_filename.size()>0) {
        //std::cout<<"opening '"<<m_filename.toStdString()<<"' ... ";
        QFile f(m_filename);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f.readAll());
            QStringList sl=s.split('\n', QString::SkipEmptyParts);
            clear();
            addItems(sl);
            for (int i=0; i<count(); i++) {
                if (QFile::exists(m_imagesDir+itemText(i).toLower().trimmed()+".png")) {
                    setItemIcon(i, QIcon(m_imagesDir+itemText(i).toLower().trimmed()+".png"));
                } else if (translationInternalExternal.contains(itemText(i))) {
                    if (QFile::exists(m_imagesDir+translationInternalExternal[itemText(i)].toLower().trimmed()+".png")) {
                        setItemIcon(i, QIcon(m_imagesDir+translationInternalExternal[itemText(i)].toLower().trimmed()+".png"));
                    }
                }
            }
        } //else std::cout<<"ERROR\n";
        connect(model(), SIGNAL(dataChanged ( const QModelIndex & , const QModelIndex &  )), this, SLOT(modelDataChanged ( const QModelIndex & , const QModelIndex &  )));
        connect(model(), SIGNAL(rowsInserted ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
        connect(model(), SIGNAL(rowsRemoved ( const QModelIndex & , int, int  )), this, SLOT(modelRowChanged ( const QModelIndex & , int, int )));
    }
}

void QMappableComboBox::modelDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight ) {
    //std::cout<<"model changed\n";
    if (m_filename.size()>0) {
        QFile f(m_filename);
       //std::cout<<"opening '"<<m_filename.toStdString()<<"' ... ";
        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            for (int i=0; i<count(); i++) {
                if (translationInternalExternal.contains(itemText(i))) f.write(QString(translationInternalExternal.contains(itemText(i))+"\n").toUtf8());
                else f.write(QString(itemText(i)+"\n").toUtf8());
                //std::cout<<"   writing "<<itemText(i).toStdString()<<std::endl;
            }
        } //else std::cout<<"ERROR\n";
    }
}

void QMappableComboBox::modelRowChanged ( const QModelIndex & parent, int start, int end ) {
    modelDataChanged(QModelIndex(), QModelIndex());
}


void QMappableComboBox::setTranslationFilename(QString fn) {
    translationInternalExternal.clear();
    translationExternalInternal.clear();
    if (!fn.isEmpty()) {
        QFile f(fn);
        //std::cout<<"opening '"<<fn.toStdString()<<"' ... \n";
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            //std::cout<<"OK\n";
            QString s=QString::fromUtf8(f.readAll());
            QStringList sl=s.split('\n', QString::SkipEmptyParts);
            for (int i=0; i<sl.size(); i++) {
                QStringList sl1=sl[i].split(";");
                if (sl1.size()>=2) {
                    translationExternalInternal[sl1[0].trimmed()]=sl1[1].trimmed();
                    translationInternalExternal[sl1[1].trimmed()]=sl1[0].trimmed();
                    //std::cout<<"  added '"<<sl1[0].trimmed().toStdString()<<"' <-> '"<<sl1[1].trimmed().toStdString()<<"'\n";
                }
            }
        } else std::cout<<"ERROR\n";

    }
}

void QMappableComboBox::setTranslatedText(QString data) {
    //std::cout<<"QMappableComboBox::setTranslatedText('"<<data.toStdString()<<"')\n";
    if (translationExternalInternal.contains(data)) {
        setText(translationExternalInternal[data]);
    } else {
        setText(data);
    }
}

QString QMappableComboBox::translatedText() const {
    if (translationInternalExternal.contains(text())) return translationInternalExternal[text()];
    return text();
}

void QMappableComboBox::setImagesDir(QString d) {
    m_imagesDir=d;
}

/*void QMappableComboBox::focusOutEvent ( QFocusEvent * event ) {
    if (isEditable() && (insertPolicy()!=QComboBox::NoInsert)) {
        if (!findText(currentText())) {
            addItem(currentText());
        }
    }
    QComboBox::focusOutEvent(event);
}*/

