#include "spectrummanager.h"
#include "qftools.h"
#include "datatable2.h"
#include "qfmanyfilessettings.h"
#include "statistics_tools.h"
#include <QDebug>

SpectrumManager::SpectrumManager(){
}

SpectrumManager::~SpectrumManager() {
    clear();
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

void SpectrumManager::clear()
{
    for (int i=0; i<spectra.size(); i++) {
        spectra[i]->clear();
    }
    fluorophores.clear();
    filters.clear();
    lightsources.clear();
    spectra.clear();
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
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            FluorophorData d=fluorophores.value(groups[i], FluorophorData());

            d.name=set.value(gn+"name", groups[i]).toString();
            d.description=set.value(gn+"description", "").toString();
            d.manufacturer=set.value(gn+"manufacturer", "").toString();
            d.reference=set.value(gn+"reference", "").toString();
            d.condition=set.value(gn+"condition", "").toString();
            d.orderNo=set.value(gn+"oder_no", "").toString();
            d.fluorescence_efficiency=set.value(gn+"q_fluor", NAN).toDouble();
            d.fluorescence_lifetime=set.value(gn+"tau_fluor", NAN).toDouble();
            d.extiction_coefficient=set.value(gn+"molar_extinction",0).toDouble();
            d.extiction_coefficient_wavelength=set.value(gn+"sigma_abs_wavelength", NAN).toDouble();
            d.excitation_maxwavelength=set.value(gn+"excitation_max", NAN).toDouble();
            d.emission_maxwavelength=set.value(gn+"emission_max", NAN).toDouble();
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            fn=set.value(gn+"spectrum_abs", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_abs_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_abs_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_abs=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_abs_reference", d.reference).toString());
            }
            if (!QFFloatIsOK(d.excitation_maxwavelength) && spectrumExists(d.spectrum_abs)) {
                spectra[d.spectrum_abs]->ensureSpectrum();
                d.excitation_maxwavelength=spectra[d.spectrum_abs]->getSpectrumMaxWavelength();
            }

            level=-1;
            fn=set.value(gn+"spectrum_fl", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_fl_id", 1).toInt();
            fnSepWL=set.value(gn+"spectrum_fl_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_fl=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_fl_reference", d.reference).toString());
            }
            if (!QFFloatIsOK(d.emission_maxwavelength) && spectrumExists(d.spectrum_fl)) {
                spectra[d.spectrum_fl]->ensureSpectrum();
                d.emission_maxwavelength=spectra[d.spectrum_fl]->getSpectrumMaxWavelength();
            }
            fluorophores[groups[i]]=d;

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
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            FilterData d=filters.value(groups[i], FilterData());

            d.name=set.value(gn+"name", groups[i]).toString();
            d.description=set.value(gn+"description", "").toString();
            d.manufacturer=set.value(gn+"manufacturer", "").toString();
            d.reference=set.value(gn+"reference", "").toString();
            d.orderNo=set.value(gn+"oder_no", "").toString();
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            fn=set.value(gn+"spectrum", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_reference", d.reference).toString());
            }

            filters[groups[i]]=d;

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
        if (!gn.isEmpty() && gn.size()>0) {
            gn+="/";
            LightSourceData d=lightsources.value(groups[i], LightSourceData());

            d.name=set.value(gn+"name", groups[i]).toString();
            d.description=set.value(gn+"description", "").toString();
            d.manufacturer=set.value(gn+"manufacturer", "").toString();
            d.reference=set.value(gn+"reference", "").toString();
            d.orderNo=set.value(gn+"oder_no", "").toString();
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            fn=set.value(gn+"spectrum", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum=addSpectrum(fn, fnID, fnSepWL, set.value(gn+"spectrum_reference", d.reference).toString());
            }

            lightsources[groups[i]]=d;

        }
    }
}

int SpectrumManager::addSpectrum(const QString& filename, int ID, bool separateWavelengths, const QString& reference) {
    for (int i=0; i<spectra.size(); i++) {
        if (QFileInfo(spectra[i]->getFilename())==QFileInfo(filename) && spectra[i]->getID()==ID && spectra[i]->getSeparateWavelengthsInFile()==separateWavelengths) {
            spectra[i]->reference=reference;
            return i;
        }
    }

    Spectrum* s=new Spectrum();
    s->init(filename, ID, separateWavelengths);
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
    spectral_interpolation_type=gsl_interp_cspline;
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
            table.load_csv(QFileInfo(filename).absoluteFilePath().toStdString(), ',', '#');
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

            internalInit(wl, sp);
            loaded=true;
            //qDebug()<<"loaded spectrum"<<wavelength<<spectrum<<N<<accel<<spline;
        } catch(datatable2_exception e) {   // error handling with exceptions
            //qDebug()<<"table error: "<<e.get_message().c_str()<<endl;
        }
    }

}

void SpectrumManager::Spectrum::init(const QVector<double> &wavelength, const QVector<double> &spectrum)
{
    internalInit(wavelength, spectrum);
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
    return gsl_spline_eval(spline,lambda, accel);
}

double SpectrumManager::Spectrum::getSpectrumIntegral(double lambda_start, double lambda_end)
{
    //ensureSpectrum();
    if (!spline || !accel || !wavelength || !spectrum || N<=0) return 0;
    return gsl_spline_eval_integ(spline, lambda_start, lambda_end, accel);
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


void SpectrumManager::Spectrum::internalInit(const QVector<double> &lambda, const QVector<double> &eff)
{
    if (lambda.size()>0&&eff.size()>0) {
        N=qMin(lambda.size(), eff.size());
        this->wavelength=(double*)realloc(this->wavelength, N*sizeof(double));
        this->spectrum=(double*)realloc(this->spectrum, N*sizeof(double));
        if (accel) gsl_interp_accel_free(accel);
        if (spline) gsl_spline_free(spline);
        copyArray(this->wavelength, lambda.data(), N);
        copyArray(this->spectrum, eff.data(), N);

        //qDebug()<<arrayToString(this->wavelength, N);
        //qDebug()<<arrayToString(this->spectrum, N);

        double m=statisticsMax(this->spectrum, N);
        for (int i=0; i<N; i++) this->spectrum[i]=this->spectrum[i]/m;



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
    this->filename="";
    loaded=true;
}

void SpectrumManager::Spectrum::init(const QString &filename, int ID, bool separateWavelengths)
{
    clear();
    this->filename=QFileInfo(filename).absoluteFilePath();
    this->spectrumIDInFile=ID;
    this->separateWavelengthsInFile=separateWavelengths;
    this->loaded=false;
}

bool SpectrumManager::lightsourceExists(const QString &f)
{
    return lightsources.contains(f);
}

bool SpectrumManager::spectrumExists(int f)
{
    return (f>=0 && f<spectra.size());
}

SpectrumManager::FluorophorData SpectrumManager::getFluorophoreData(const QString &name)
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
}

SpectrumManager::LightSourceData::LightSourceData()
{
    spectrum=-1;
}

SpectrumManager::FluorophorData::FluorophorData()
{
    fluorescence_efficiency=0;
    fluorescence_lifetime=0;
    extiction_coefficient=0;
    extiction_coefficient_wavelength=0;
    spectrum_abs=-1;
    spectrum_fl=-1;
}

