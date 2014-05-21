#include "spectrummanager.h"
#include "qftools.h"
#include "datatable2.h"
#include "qfmanyfilessettings.h"
#include "statistics_tools.h"
#include <QDebug>
#include "statistics_tools.h"

SpectrumManager::SpectrumManager(){
    laserlinesTree=new QFSimpleTreeModel(NULL);
    fluorophoresTree=new QFSimpleTreeModel(NULL);
    lightsourcesTree=new QFSimpleTreeModel(NULL);
    filtersTree=new QFSimpleTreeModel(NULL);
    detectorsTree=new QFSimpleTreeModel(NULL);
}

SpectrumManager::~SpectrumManager() {
    clear();
    delete laserlinesTree;
    delete fluorophoresTree;
    delete lightsourcesTree;
    delete filtersTree;
    delete detectorsTree;
}

int SpectrumManager::getSpectraCount() const
{
    return spectra.size();
}

int SpectrumManager::getFluorophoreCount() const
{
    return fluorophores.size();
}

int SpectrumManager::getLightSourceCount() const
{
    return lightsources.size();
}

int SpectrumManager::getFilterCount() const
{
    return filters.size();
}

int SpectrumManager::getDetectorCount() const
{
    return detectors.size();
}

QStringList SpectrumManager::getFluorophores() const
{
    return fluorophores.keys();
}

QStringList SpectrumManager::getLightSources() const
{
    return lightsources.keys();
}

QStringList SpectrumManager::getFilters() const
{
    return filters.keys();
}

QStringList SpectrumManager::getDetectors() const
{
    return detectors.keys();
}

QFSimpleTreeModel *SpectrumManager::getFluorophoresTree() const
{
    return fluorophoresTree;
}

QFSimpleTreeModel *SpectrumManager::getLightSourcesTree() const
{
    return lightsourcesTree;
}

QFSimpleTreeModel *SpectrumManager::getFiltersTree() const
{
    return filtersTree;
}

QFSimpleTreeModel *SpectrumManager::getDetectorsTree() const
{
    return detectorsTree;
}

QFSimpleTreeModel *SpectrumManager::getLaserlinesTree() const
{
    return laserlinesTree;
}

void SpectrumManager::clear()
{
    for (int i=0; i<spectra.size(); i++) {
        spectra[i]->clear();
    }
    fluorophores.clear();
    filters.clear();
    lightsources.clear();
    spectra.clear();

    laserlinesTree->clear();
    fluorophoresTree->clear();
    lightsourcesTree->clear();
    filtersTree->clear();
    detectorsTree->clear();

}

void SpectrumManager::loadLaserlinesDatabase(const QString &ininame, const QStringList &directories)
{
    QFManyFilesSettings set;
    if (directories.size()>0) {
        for (int i=0; i<directories.size(); i++) {
            set.addSettings(new QSettings(QDir(directories[i]).absoluteFilePath(ininame), QSettings::IniFormat), true);
        }
    } else {
        set.addSettings(new QSettings(ininame, QSettings::IniFormat), true);
    }

    laserlinesTree->clear();
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString gn=groups[i].toLower();
        if (!gn.isEmpty() && gn.size()>0) {
            QString laser=set.value(gn+"/laser", "").toString();
            if (laser.isEmpty()) laser=QObject::tr("other lasers");
            QString lines=set.value(gn+"/lines", "").toString();
            QStringList linessl=lines.split(';');
            //qDebug()<<"laser: "<<laser<<lines;
            for (int i=0; i<linessl.size(); i++) {
                QString line=linessl.value(i, "");
                bool ok=false;
                double wl=line.toDouble(&ok);
                if (ok && wl>0) {
                    laserlinesTree->addFolderedItem(QString("%2/%1 nm").arg(wl).arg(laser), QPointF(wl, 1.0));
                }
            }
        }
    }
}

void SpectrumManager::loadFluorophoreDatabase(const QString &ininame, const QStringList &directories)
{
    QFManyFilesSettings set;
    if (directories.size()>0) {
        for (int i=0; i<directories.size(); i++) {
            set.addSettings(new QSettings(QDir(directories[i]).absoluteFilePath(ininame), QSettings::IniFormat), true);
        }
    } else {
        set.addSettings(new QSettings(ininame, QSettings::IniFormat), true);
    }

    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString gn=groups[i].toLower();
        QString gns=gn;
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            FluorophoreData d=fluorophores.value(gns, FluorophoreData());

            QStringList read;

            d.folder=set.value(gn+"folder", "").toString(); read<<"folder";
            d.name=set.value(gn+"name", groups[i]).toString(); read<<"name";
            d.description=set.value(gn+"description", "").toString(); read<<"description";
            d.manufacturer=set.value(gn+"manufacturer", "").toString(); read<<"manufacturer";
            d.reference=set.value(gn+"reference", "").toString(); read<<"reference";
            d.condition=set.value(gn+"condition", "").toString(); read<<"condition";
            d.orderNo=set.value(gn+"oder_no", "").toString(); read<<"oder_no";
            d.fluorescence_efficiency=set.value(gn+"q_fluor", NAN).toDouble(); read<<"q_fluor";
            d.fluorescence_lifetime=set.value(gn+"tau_fluor", NAN).toDouble(); read<<"tau_fluor";
            d.extiction_coefficient=set.value(gn+"molar_extinction",0).toDouble(); read<<"molar_extinction";
            d.excitation_maxwavelength=set.value(gn+"excitation_max", NAN).toDouble(); read<<"excitation_max";
            d.emission_maxwavelength=set.value(gn+"emission_max", NAN).toDouble(); read<<"emission_max";
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            read<<"spectrum_abs"<<"spectrum_abs_id"<<"spectrum_abs_separatewavelengths"<<"spectrum_abs_reference";
            fn=set.value(gn+"spectrum_abs", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_abs_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_abs_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_abs=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_abs_reference", d.reference).toString());
            }
            if ((!QFFloatIsOK(d.excitation_maxwavelength)||d.excitation_maxwavelength<0) && spectrumExists(d.spectrum_abs)) {
                spectra[d.spectrum_abs]->ensureSpectrum();
                d.excitation_maxwavelength=spectra[d.spectrum_abs]->getSpectrumMaxWavelength();
            }

            level=-1;
            read<<"spectrum_fl"<<"spectrum_fl_id"<<"spectrum_fl_separatewavelengths"<<"spectrum_fl_reference";
            fn=set.value(gn+"spectrum_fl", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_fl_id", 1).toInt();
            fnSepWL=set.value(gn+"spectrum_fl_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_fl=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_fl_reference", d.reference).toString());
            }
            if ((!QFFloatIsOK(d.emission_maxwavelength)||d.emission_maxwavelength<0) && spectrumExists(d.spectrum_fl)) {
                spectra[d.spectrum_fl]->ensureSpectrum();
                d.emission_maxwavelength=spectra[d.spectrum_fl]->getSpectrumMaxWavelength();
            }
            d.fluorescence_efficiency_wavelength=set.value(gn+"q_fluor_wavelength", d.emission_maxwavelength).toDouble(); read<<"q_fluor_wavelength";
            d.extiction_coefficient_wavelength=set.value(gn+"molar_extinction_wavelength", d.excitation_maxwavelength).toDouble(); read<<"molar_extinction_wavelength";
            loadMoreData(d.moreData, set, gn, read);
            fluorophores[gns]=d;

            if (d.folder.isEmpty()) {
                fluorophoresTree->addFolderedItem(d.name, gns);
            } else {
                fluorophoresTree->addFolderedItem(d.folder+"/"+d.name, gns);
            }

        }
    }

}

void SpectrumManager::loadFilterDatabase(const QString &ininame, const QStringList &directories)
{
    QFManyFilesSettings set;
    if (directories.size()>0) {
        for (int i=0; i<directories.size(); i++) {
            set.addSettings(new QSettings(QDir(directories[i]).absoluteFilePath(ininame), QSettings::IniFormat), true);
        }
    } else {
        set.addSettings(new QSettings(ininame, QSettings::IniFormat), true);
    }

    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString gn=groups[i].toLower();
        QString gns=gn;
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            QStringList read;
            FilterData d=filters.value(gns, FilterData());

            d.folder=set.value(gn+"folder", "").toString(); read<<"folder";
            d.name=set.value(gn+"name", groups[i]).toString(); read<<"name";
            d.description=set.value(gn+"description", "").toString(); read<<"description";
            d.manufacturer=set.value(gn+"manufacturer", "").toString(); read<<"manufacturer";
            d.reference=set.value(gn+"reference", "").toString(); read<<"reference";
            d.orderNo=set.value(gn+"oder_no", "").toString(); read<<"oder_no";
            d.typical_wavelength=set.value(gn+"typical_wavelength", NAN).toDouble(); read<<"typical_wavelength";
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            read<<"spectrum"<<"spectrum_id"<<"spectrum_separatewavelengths"<<"spectrum_reference";
            fn=set.value(gn+"spectrum", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_reference", d.reference).toString(), false);
            }
            if ((!QFFloatIsOK(d.typical_wavelength) || d.typical_wavelength<=0) && spectrumExists(d.spectrum)) {
                spectra[d.spectrum]->ensureSpectrum();
                d.typical_wavelength=spectra[d.spectrum]->getSpectrumMaxWavelength();
            }

            loadMoreData(d.moreData, set, gn, read);
            filters[gns]=d;
            if (d.folder.isEmpty()) {
                filtersTree->addFolderedItem(d.name, gns);
            } else {
                filtersTree->addFolderedItem(d.folder+"/"+d.name, gns);
            }
        }
    }
}

void SpectrumManager::loadLightSourceDatabase(const QString &ininame, const QStringList &directories)
{
    QFManyFilesSettings set;
    if (directories.size()>0) {
        for (int i=0; i<directories.size(); i++) {
            set.addSettings(new QSettings(QDir(directories[i]).absoluteFilePath(ininame), QSettings::IniFormat), true);
        }
    } else {
        set.addSettings(new QSettings(ininame, QSettings::IniFormat), true);
    }

    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString gn=groups[i].toLower();
        QString gns=gn;
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            QStringList read;
            LightSourceData d=lightsources.value(gns, LightSourceData());

            d.folder=set.value(gn+"folder", "").toString(); read<<"folder";
            d.name=set.value(gn+"name", groups[i]).toString(); read<<"name";
            d.description=set.value(gn+"description", "").toString(); read<<"description";
            d.manufacturer=set.value(gn+"manufacturer", "").toString(); read<<"manufacturer";
            d.reference=set.value(gn+"reference", "").toString(); read<<"reference";
            d.orderNo=set.value(gn+"oder_no", "").toString(); read<<"oder_no";
            d.typical_wavelength=set.value(gn+"typical_wavelength", NAN).toDouble(); read<<"typical_wavelength";
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            read<<"spectrum"<<"spectrum_id"<<"spectrum_separatewavelengths"<<"spectrum_reference";
            fn=set.value(gn+"spectrum", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_reference", d.reference).toString());
            }
            if ((!QFFloatIsOK(d.typical_wavelength) || d.typical_wavelength<=0) && spectrumExists(d.spectrum)) {
                spectra[d.spectrum]->ensureSpectrum();
                d.typical_wavelength=spectra[d.spectrum]->getSpectrumMaxWavelength();
            }

            loadMoreData(d.moreData, set, gn, read);

            lightsources[gns]=d;

            if (d.folder.isEmpty()) {
                lightsourcesTree->addFolderedItem(d.name, gns);
            } else {
                lightsourcesTree->addFolderedItem(d.folder+"/"+d.name, gns);
            }

        }
    }
}

void SpectrumManager::loadDetectorDatabase(const QString &ininame, const QStringList &directories)
{
    QFManyFilesSettings set;
    if (directories.size()>0) {
        for (int i=0; i<directories.size(); i++) {
            set.addSettings(new QSettings(QDir(directories[i]).absoluteFilePath(ininame), QSettings::IniFormat), true);
        }
    } else {
        set.addSettings(new QSettings(ininame, QSettings::IniFormat), true);
    }

    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        QString gn=groups[i].toLower();
        QString gns=gn;
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            DetectorData d=detectors.value(gns, DetectorData());
            QStringList read;

            d.folder=set.value(gn+"folder", "").toString(); read<<"folder";
            d.name=set.value(gn+"name", groups[i]).toString(); read<<"name";
            d.description=set.value(gn+"description", "").toString(); read<<"description";
            d.manufacturer=set.value(gn+"manufacturer", "").toString(); read<<"manufacturer";
            d.reference=set.value(gn+"reference", "").toString(); read<<"reference";
            d.orderNo=set.value(gn+"oder_no", "").toString(); read<<"oder_no";
            d.peak_wavelength=set.value(gn+"peak_wavelength", NAN).toDouble(); read<<"peak_wavelength";
            d.peak_sensitivity=set.value(gn+"peak_sensitivity", NAN).toDouble(); read<<"peak_sensitivity";
            d.peak_sensitivity_unit=set.value(gn+"peak_sensitivity_unit", "").toString(); read<<"peak_sensitivity_unit";
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            read<<"spectrum"<<"spectrum_id"<<"spectrum_separatewavelengths"<<"spectrum_reference";
            fn=set.value(gn+"spectrum", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_reference", d.reference).toString(), false);
            }
            if ((!QFFloatIsOK(d.peak_wavelength) || d.peak_wavelength<=0) && spectrumExists(d.spectrum)) {
                spectra[d.spectrum]->ensureSpectrum();
                d.peak_wavelength=spectra[d.spectrum]->getSpectrumMaxWavelength();
            }
            loadMoreData(d.moreData, set, gn, read);
            detectors[gns]=d;

            if (d.folder.isEmpty()) {
                detectorsTree->addFolderedItem(d.name, gns);
            } else {
                detectorsTree->addFolderedItem(d.folder+"/"+d.name, gns);
            }

        }
    }
}

int SpectrumManager::addSpectrum(const QString& filename, int ID, bool separateWavelengths, const QString& reference, bool normalizeSPectrum) {
    for (int i=0; i<spectra.size(); i++) {
        if (QFileInfo(spectra[i]->getFilename())==QFileInfo(filename) && spectra[i]->getID()==ID && spectra[i]->getSeparateWavelengthsInFile()==separateWavelengths) {
            spectra[i]->reference=reference;
            return i;
        }
    }

    Spectrum* s=new Spectrum();
    s->init(filename, ID, separateWavelengths, normalizeSPectrum);
    s->reference=reference;
    spectra.append(s);
    return spectra.size()-1;
}

 SpectrumManager::Spectrum* SpectrumManager::getSpectrum(int i)
{
     if (i<0 || i>=spectra.size()) return NULL;
     spectra[i]->ensureSpectrum();
     return spectra[i];
}



SpectrumManager::Spectrum::Spectrum()
{
    spectral_interpolation_type=gsl_interp_linear;
    accel=NULL;
    spline=NULL;
    spectrumIDInFile=0;
    wavelength=NULL;
    spectrum=NULL;
    N=0;
    separateWavelengthsInFile=false;
    loaded=false;
}

void SpectrumManager::Spectrum::ensureSpectrum()
{
    if (!loaded && QFile::exists(filename)) {
        try {
            datatable2 table;
            //qDebug()<<"loading spectrum from "<<QFileInfo(filename).absoluteFilePath();
            table.load_csv(QFileInfo(filename).absoluteFilePath().toStdString(), ',', '#');
            //qDebug()<<table.get_column_count()<<"x"<<table.get_line_count();
            QVector<double> wl, sp;
            if (spectrumIDInFile==0 && table.get_column_count()>=2) {
                for (int i=0; i<table.get_line_count(); i++) {
                    if (!table.is_empty(0,i) && !table.is_empty(1,i)) {
                        wl<<table.get(0,i);
                        sp<<table.get(1,i);
                    }
                }
            } else if (spectrumIDInFile>0) {
                if (separateWavelengthsInFile) {
                    for (int i=0; i<table.get_line_count(); i++) {
                        if (!table.is_empty(spectrumIDInFile*2+0,i) && !table.is_empty(spectrumIDInFile*2+1,i)) {
                            wl<<table.get(spectrumIDInFile*2+0,i);
                            sp<<table.get(spectrumIDInFile*2+1,i);
                        }
                    }
                } else {
                    for (int i=0; i<table.get_line_count(); i++) {
                        if (!table.is_empty(0,i) && !table.is_empty(spectrumIDInFile+1,i)) {
                            wl<<table.get(0,i);
                            sp<<table.get(spectrumIDInFile+1,i);
                        }
                    }
                }

            }
            int i=0;
            while (i<sp.size() && sp[i]==0) i++;
            if (i>0) {
                sp.remove(0,i);
                wl.remove(0,i);
            }
            i=sp.size()-1;
            while (i>=0 && sp[i]==0)  i--;
            if (i<sp.size()-1) {
                sp.remove(i, sp.size()-1-i);
                wl.remove(i, sp.size()-1-i);
            }

            internalInit(wl, sp, normalizeSpectrum);
            loaded=true;
            //qDebug()<<"loaded spectrum"<<wavelength<<spectrum<<N<<accel<<spline;
        } catch(datatable2_exception e) {   // error handling with exceptions
            //qDebug()<<"table error: "<<e.get_message().c_str()<<endl;
        }
    }

}

void SpectrumManager::Spectrum::init(const QVector<double> &wavelength, const QVector<double> &spectrum, bool normalizeSpectrum)
{
    internalInit(wavelength, spectrum, normalizeSpectrum);
    filename="";
}

void SpectrumManager::Spectrum::clear()
{
    if (accel) gsl_interp_accel_free(accel);
    if (spline) gsl_spline_free(spline);
    if (wavelength) free(wavelength);
    if (spectrum) free(spectrum);
    accel=NULL;
    spline=NULL;
    wavelength=NULL;
    spectrum=NULL;
    N=0;
}

double SpectrumManager::Spectrum::getSpectrumAt(double lambda)
{
    //ensureSpectrum();
    if (!spline || !accel || !wavelength || !spectrum || N<=0) return 0;
    if (lambda<wavelength[0]) return 0;
    if (lambda>wavelength[N-1]) return 0;
    return gsl_spline_eval(spline,lambda, accel);
}

double SpectrumManager::Spectrum::getSpectrumIntegral(double lambda_start, double lambda_end, double wavelength_factctor)
{
    if (lambda_start>=lambda_end) return 0;
    //ensureSpectrum();
    if (!wavelength || !spectrum || N<=0) return 0;

    QVector<double> wl;
    QVector<double> sp;
    if (lambda_start<wavelength[0]) {
        wl<<lambda_start*wavelength_factctor;
        sp<<0;
    }

    for (int i=0; i<N; i++) {
        wl<<wavelength[i]*wavelength_factctor;
        sp<<spectrum[i];
    }

    if (lambda_end>wavelength[N-1]) {
        wl<<lambda_end*wavelength_factctor;
        sp<<0;
    }

    gsl_interp_accel *accel= gsl_interp_accel_alloc();
    gsl_spline *spline=gsl_spline_alloc(spectral_interpolation_type,wl.size());
    gsl_spline_init(spline, wl.data(), sp.data(), wl.size());
    double res;
    if (gsl_spline_eval_integ_e(spline, lambda_start*wavelength_factctor, lambda_end*wavelength_factctor, accel, &res)) {
        res=0;
    }
    //qDebug()<<"int "<<filename<<"*"<<multWith->filename<<"    "<<lambda_start<<" ... "<<lambda_end<<"   = "<<res;
    //qDebug()<<"    "<<arrayToString(wl.data(), wl.size());
    //qDebug()<<"    "<<arrayToString(sp.data(), sp.size());

    gsl_interp_accel_free(accel);
    gsl_spline_free(spline);
    //qDebug()<<"int "<<filename<<"    "<<st<<" ... "<<en<<"   = "<<res;
    return res;
}

double SpectrumManager::Spectrum::getSpectrumIntegral(double lambda_start, double lambda_end)
{
    if (lambda_start>=lambda_end) return 0;
    //ensureSpectrum();
    if (!spline || !accel || !wavelength || !spectrum || N<=0) return 0;
    double st=lambda_start;
    double en=lambda_end;
    if (st<wavelength[0]) st=wavelength[0];
    if (en>wavelength[N-1]) en=wavelength[N-1];
    double res=0;
    if (gsl_spline_eval_integ_e(spline, st, en, accel, &res)) {
        //qDebug()<<"int "<<filename<<"    "<<st<<" ... "<<en<<"   = "<<0;
        return 0;
    }
    //qDebug()<<"int "<<filename<<"    "<<st<<" ... "<<en<<"   = "<<res;
    return res;
}

double SpectrumManager::Spectrum::getMulSpectrumIntegral(SpectrumManager::Spectrum *multWith, double lambda_start, double lambda_end)
{
    try {
        QVector<double> wl;
        QVector<double> sp;

        int inBetweenThis=0;
        int thisStart=-1;
        for (int i=0; i<N; i++) {
            if (wavelength[i]>lambda_start && wavelength[i]<=lambda_end) {
                if (thisStart<0) thisStart=i;
                inBetweenThis++;
            }
        }
        if (thisStart>0) {
            thisStart--;
            inBetweenThis++;
        }
        if (inBetweenThis<N) {
            inBetweenThis++;
        }

        int inBetweenOther=0;
        int otherStart=-1;
        for (int i=0; i<multWith->N; i++) {
            if (multWith->wavelength[i]>lambda_start && multWith->wavelength[i]<=lambda_end) {
                if (otherStart<0) otherStart=i;
                inBetweenOther++;
            }
        }
        if (otherStart>0) {
            otherStart--;
            inBetweenOther++;
        }
        if (inBetweenOther<multWith->N) {
            inBetweenOther++;
        }


        if (inBetweenThis>inBetweenOther) {
            for (int i=thisStart; i<thisStart+inBetweenThis; i++) {
                wl<<wavelength[i];
                sp<<spectrum[i]*multWith->getSpectrumAt(wavelength[i]);
            }
        } else {
            for (int i=otherStart; i<otherStart+inBetweenOther; i++) {
                wl<<multWith->wavelength[i];
                sp<<multWith->spectrum[i]*getSpectrumAt(multWith->wavelength[i]);
            }
        }

        /*gsl_interp_accel *accel= gsl_interp_accel_alloc();
        gsl_spline *spline=gsl_spline_alloc(spectral_interpolation_type,wl.size());
        gsl_spline_init(spline, wl.data(), sp.data(), wl.size());
        double res;
        if (gsl_spline_eval_integ_e(spline, lambda_start, lambda_end, accel, &res)) {
            res=0;
        }
        //qDebug()<<"int "<<filename<<"*"<<multWith->filename<<"    "<<lambda_start<<" ... "<<lambda_end<<"   = "<<res;
        //qDebug()<<"    "<<arrayToString(wl.data(), wl.size());
        //qDebug()<<"    "<<arrayToString(sp.data(), sp.size());

        gsl_interp_accel_free(accel);
        gsl_spline_free(spline);*/

        double res=0;
        // Sehnentrapezformel
        //qDebug()<<"integrating "<<wl.size()<<" points";
        for (int i=0; i<wl.size()-1; i++) {
            const double b=wl[i+1];
            const double a=wl[i];
            const double fb=sp[i+1];
            const double fa=sp[i];
            res=res+/*(b-a)**/(fa+fb)/2.0;
        }
        return res;

        return res;
    } catch (std::exception& E) {
        //qDebug()<<"ERROR: "<<E.what();
        return 0;
    }

}

double SpectrumManager::Spectrum::getMulSpectrumIntegral(SpectrumManager::Spectrum *multWith, double lambda_start, double lambda_end, double lambda_power, double wavelength_factctor, double spectrum_factor)
{
    //qDebug()<<"lambda="<<lambda_start<<lambda_end<<"  pow="<<lambda_power;
    try {
        QVector<double> wl;
        QVector<double> sp;

        int inBetweenThis=0;
        int thisStart=-1;
        for (int i=0; i<N; i++) {
            if (wavelength[i]>lambda_start && wavelength[i]<=lambda_end) {
                if (thisStart<0) thisStart=i;
                inBetweenThis++;
            }
        }
        if (thisStart>0) {
            thisStart--;
            inBetweenThis++;
        }
        if (inBetweenThis<N) {
            inBetweenThis++;
        }

        int inBetweenOther=0;
        int otherStart=-1;
        for (int i=0; i<multWith->N; i++) {
            if (multWith->wavelength[i]>lambda_start && multWith->wavelength[i]<=lambda_end) {
                if (otherStart<0) otherStart=i;
                inBetweenOther++;
            }
        }
        if (otherStart>0) {
            otherStart--;
            inBetweenOther++;
        }
        if (inBetweenOther<multWith->N) {
            inBetweenOther++;
        }


        if (inBetweenThis>inBetweenOther) {
            //qDebug()<<"inBetweenThis>inBetweenOther: "<<thisStart<<thisStart+inBetweenThis<<N;
            for (int i=thisStart; i<thisStart+inBetweenThis-1; i++) {
                wl<<wavelength[i]*wavelength_factctor;
                sp<<spectrum[i]*multWith->getSpectrumAt(wavelength[i])*pow(wavelength[i]*wavelength_factctor, lambda_power)*spectrum_factor;
                //qDebug()<<wl.last()<<spectrum[i]<<multWith->getSpectrumAt(wavelength[i])<<pow(wavelength[i]*wavelength_factctor, lambda_power)<<sp.last();
            }
        } else {
            //qDebug()<<"inBetweenThis<=inBetweenOther: "<<otherStart<<otherStart+inBetweenOther<<multWith->N;
            for (int i=otherStart; i<otherStart+inBetweenOther-1; i++) {
                wl<<multWith->wavelength[i]*wavelength_factctor;
                sp<<multWith->spectrum[i]*getSpectrumAt(multWith->wavelength[i])*pow(multWith->wavelength[i]*wavelength_factctor, lambda_power)*spectrum_factor;
                //qDebug()<<wl.last()<<multWith->spectrum[i]<<getSpectrumAt(multWith->wavelength[i])<<pow(multWith->wavelength[i]*wavelength_factctor, lambda_power)<<sp.last();
            }
        }


        //qDebug()<<"nc    "<<arrayToString(wl.data(), wl.size());
        //qDebug()<<"nc    "<<arrayToString(sp.data(), sp.size());

        for (int i=0; i<wl.size(); i++) {
            if (!QFFloatIsOK(sp[i])) sp[i]=0;
        }

        /*gsl_interp_accel *accel= gsl_interp_accel_alloc();
        gsl_spline *spline=gsl_spline_alloc(spectral_interpolation_type,wl.size());
        gsl_spline_init(spline, wl.data(), sp.data(), wl.size());
        double res;
        if (gsl_spline_eval_integ_e(spline, lambda_start*wavelength_factctor, lambda_end*wavelength_factctor, accel, &res)) {
            res=0;
        }



        gsl_interp_accel_free(accel);
        gsl_spline_free(spline);*/

        //qDebug()<<"int "<<filename<<"*"<<multWith->filename<<"    "<<lambda_start<<" ... "<<lambda_end;
        //qDebug()<<"    "<<arrayToString(wl.data(), wl.size());
        //qDebug()<<"    "<<arrayToString(sp.data(), sp.size());
        double res=0;
        // Sehnentrapezformel
        //qDebug()<<"2integrating "<<wl.size()<<" points";
        for (int i=0; i<wl.size()-1; i++) {
            const double b=wl[i+1];
            const double a=wl[i];
            const double fb=sp[i+1];
            const double fa=sp[i];
            res=res+/*(b-a)**/(fa+fb)/2.0;
        }
        return res;
    } catch (std::exception& E) {
        //qDebug()<<"ERROR: "<<E.what();
        return 0;
    }
}

double SpectrumManager::Spectrum::getMulSpectrumIntegral(SpectrumManager::Spectrum *multWith)
{
    double lstart=getWavelengthMin();
    double lend=getWavelengthMax();
    if (multWith->getWavelengthMin()>lstart) lstart=multWith->getWavelengthMin();
    if (multWith->getWavelengthMax()<lend) lend=multWith->getWavelengthMax();
    return getMulSpectrumIntegral(multWith, lstart, lend);
}

double SpectrumManager::Spectrum::getMulSpectrumIntegral(SpectrumManager::Spectrum *multWith, double lambda_power)
{
    double lstart=getWavelengthMin();
    double lend=getWavelengthMax();
    if (multWith->getWavelengthMin()>lstart) lstart=multWith->getWavelengthMin();
    if (multWith->getWavelengthMax()<lend) lend=multWith->getWavelengthMax();
    return getMulSpectrumIntegral(multWith, lstart, lend, lambda_power);
}

double SpectrumManager::Spectrum::getWavelengthMin()
{
    //ensureSpectrum();
    if (!wavelength || N<=0) return 0;
    return statisticsMin(wavelength, N);
}

double SpectrumManager::Spectrum::getWavelengthMax()
{
    //ensureSpectrum();
    if (!wavelength || N<=0) return 0;
    return statisticsMax(wavelength, N);
}

double SpectrumManager::Spectrum::getSpectrumMin()
{
    //ensureSpectrum();
    if (!spectrum || N<=0) return 0;
    return statisticsMin(spectrum, N);
}

double SpectrumManager::Spectrum::getSpectrumMax()
{
    //ensureSpectrum();
    if (!spectrum || N<=0) return 0;
    return statisticsMax(spectrum, N);
}

double SpectrumManager::Spectrum::getSpectrumMaxWavelength()
{
    //ensureSpectrum();
    if (!spectrum || !wavelength || N<=0) return 0;
    long long maxpos=-1;
    statisticsMax(spectrum, N, &maxpos);
    if (maxpos>=0 && maxpos<N) return wavelength[maxpos];
    return NAN;
}

double SpectrumManager::Spectrum::getSpectrumFullIntegral()
{
    //ensureSpectrum();
    if (N<=0) return 0;
    return getSpectrumIntegral(getWavelengthMin(), getWavelengthMax());
}


void SpectrumManager::Spectrum::internalInit(const QVector<double> &lambda, const QVector<double> &eff, bool normalizeSpectrum)
{
    this->normalizeSpectrum=normalizeSpectrum;
    if (lambda.size()>0&&eff.size()>0) {
        N=qMin(lambda.size(), eff.size());
        this->wavelength=(double*)realloc(this->wavelength, N*sizeof(double));
        this->spectrum=(double*)realloc(this->spectrum, N*sizeof(double));
        if (accel) gsl_interp_accel_free(accel);
        if (spline) gsl_spline_free(spline);
        /*copyArray(this->wavelength, lambda.data(), N);
        copyArray(this->spectrum, eff.data(), N);*/

        QVector<double> l=lambda;
        QVector<double> ef=eff;
        statisticsSort2(l.data(), ef.data(), N);

        int N2=N;
        N=0;
        for (int i=0; i<N2; i++) {
            if (i==0 || (i>0&&l[i]>l[i-1])) {
                this->wavelength[N]=l[i];
                this->spectrum[N]=ef[i];
                N++;
            }
        }



        //qDebug()<<arrayToString(this->wavelength, N);
        //qDebug()<<arrayToString(this->spectrum, N);

        double m=statisticsMax(this->spectrum, N);
        if (normalizeSpectrum) {
            for (int i=0; i<N; i++) this->spectrum[i]=this->spectrum[i]/m;
        } else if (m>1.2 && m<110) { // seems to be 0..100%
            for (int i=0; i<N; i++) this->spectrum[i]=this->spectrum[i]/100.0;
        }



        //qDebug()<<"allcoing GSL N="<<N;
        accel= gsl_interp_accel_alloc();
        spline=gsl_spline_alloc(spectral_interpolation_type,N);
        gsl_spline_init(spline, this->wavelength, this->spectrum, N);
    } else {
        if (accel) gsl_interp_accel_free(accel);
        if (spline) gsl_spline_free(spline);
        if (this->wavelength) free(this->wavelength);
        if (this->spectrum) free(this->spectrum);
        accel=NULL;
        spline=NULL;
        this->wavelength=NULL;
        this->spectrum=NULL;
        N=0;
    }
    //this->filename="";
    loaded=true;
}

void SpectrumManager::Spectrum::init(const QString &filename, int ID, bool separateWavelengths, bool normalizeSPectrum)
{
    clear();
    this->normalizeSpectrum=normalizeSPectrum;
    this->filename=QFileInfo(filename).absoluteFilePath();
    this->spectrumIDInFile=ID;
    this->separateWavelengthsInFile=separateWavelengths;
    this->loaded=false;
}

bool SpectrumManager::lightsourceExists(const QString &f)
{
    return lightsources.contains(f);
}

bool SpectrumManager::detectorExists(const QString &f)
{
    return detectors.contains(f);
}

bool SpectrumManager::spectrumExists(int f)
{
    return (f>=0 && f<spectra.size());
}

SpectrumManager::FluorophoreData SpectrumManager::getFluorophoreData(const QString &name)
{
    return fluorophores[name];
}

SpectrumManager::LightSourceData SpectrumManager::getLightSourceData(const QString &name)
{
    return lightsources[name];
}

SpectrumManager::FilterData SpectrumManager::getFilterData(const QString &name)
{
    return filters[name];
}

SpectrumManager::DetectorData SpectrumManager::getDetectorData(const QString &name)
{
    return detectors[name];
}

void SpectrumManager::loadMoreData(QMap<QString, QVariant> &store, QFManyFilesSettings &settings, const QString &prefix, const QStringList &readprops)
{
    settings.beginGroup(prefix);
    QStringList keys=settings.allKeys();
    QSet<QString> ks=readprops.toSet();
    for (int i=0; i<keys.size(); i++) {
        if (!ks.contains(keys[i])) {
            store[keys[i]]=settings.value(keys[i]);
        }
    }
    settings.endGroup();
}


bool SpectrumManager::filterExists(const QString &f)
{
    return filters.contains(f);
}

bool SpectrumManager::fluorophoreExists(const QString &f)
{
    return fluorophores.contains(f);
}

SpectrumManager::FilterData::FilterData()
{
    spectrum=-1;
    folder="";
}

SpectrumManager::LightSourceData::LightSourceData()
{
    spectrum=-1;
    folder="";
}

SpectrumManager::FluorophoreData::FluorophoreData()
{
    fluorescence_efficiency=0;
    fluorescence_lifetime=0;
    extiction_coefficient=0;
    extiction_coefficient_wavelength=0;
    spectrum_abs=-1;
    spectrum_fl=-1;
    folder="";
}

SpectrumManager::DetectorData::DetectorData()
{
    spectrum=-1;
    folder="";
}

