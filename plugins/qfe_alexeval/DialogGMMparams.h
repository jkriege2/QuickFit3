/*
Copyright (c) 2012-2015 by Sebastian Isbaner

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DIALOGGMMPARAMS_H
#define DIALOGGMMPARAMS_H

#include <QDialog>
#include "alexdata.h"

namespace Ui {
class DialogGMMparams;
}

class GMMfitModel
{
    QString name;
    GMMfit *gmm;
public:
    GMMfitModel(GMMfit &gmm,QString name);
    GMMfitModel(GMMfit *gmm,QString name);
    GMMfitModel();
    ~GMMfitModel();
    const QString getName() const {return name;}
    void setName(QString _name) {name=_name;}
    GMMfit *getGMM() {return gmm;}
    void setGMM(GMMfit *_gmm) {if(gmm) delete gmm; gmm=_gmm; }
    void writeToIni(jkINIParser2 &ini, std::string group) const;
    int readFromIni(jkINIParser2 &ini, std::string group);
    QString toString() const;
    QString resultToStringHeader() const;
    QString resultToString(const QString sampleName=QString("")) const;
};


class DialogGMMparams : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogGMMparams(QWidget *parent = 0);
    ~DialogGMMparams();

    void setDimensionName(QStringList &names);
    QList<GMMfitModel> getModels() const {return models;}
    void setModels(QList<GMMfitModel> _models) {models=_models;}
    void addModel(const GMMfitModel &newModel) {models.append(newModel);}
    void setIniFile(const QString &_iniFile) {iniFile=_iniFile;}
    QString getIniFile() const {return iniFile;}
    
private:
    Ui::DialogGMMparams *ui;
    QStringList m_MeanHeader;
    QStringList m_SigmaHeader;
    QStringList m_DimensionHeader;
    QString iniFile;
    QList<GMMfitModel> models;
protected:
//    virtual void closeEvent(QCloseEvent* event);

private slots:
    void fillTable(GMMfit *gmm);
    GMMfit *getFromTable();
    void getFromTable(GMMfit *gmm);
    void fillList();
    int writeToIni(const std::string iniFilename) const;
    int readFromIni(const std::string iniFilename);
    void saveAll();

    void dimChanged(int dim);
    void componentsChanged(int k);

    void updateTables(int index);
    void on_pushButtonSave_clicked();
    void on_pushButtonModel_clicked();
    void on_pushButtonRemove_clicked();
};

#endif // DIALOGGMMPARAMS_H
