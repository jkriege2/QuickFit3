#include "confocor3tools.h"

#define MAX_OLD_ITEMS 5
QList<Confocor3Tools::ConfocorDataset> Confocor3Tools::oldData=QList<Confocor3Tools::ConfocorDataset>();

Confocor3Tools::Confocor3Tools(const QString &filename)
{
    bool found=false;
    for (int i=0; i<oldData.size(); i++) {
        if (QFileInfo(filename)==QFileInfo(oldData[i].filename)) {
            data=oldData[i];
            found=true;
            break;
        }
    }

    if (!found) {
        loadFile(data, filename);
    }
}

void Confocor3Tools::loadFile(Confocor3Tools::ConfocorDataset &data, const QString &filename)
{



    // finally add data object to OLD DATA STORE
    while (oldData.size()>=MAX_OLD_ITEMS) {
        oldData.removeFirst();
    }
    oldData.append(data);
}

void Confocor3Tools::clearStore()
{
    oldData.clear();
}


QString Confocor3Tools::ConfocorDataset::getID(int position, int kinetic, int repetition, int channel, Confocor3Tools::ConfocorDataset::DataType type)
{
    return QString("%1_%2_%3_%4_%5").arg(position).arg(kinetic).arg(repetition).arg(channel).arg((int)type);
}
