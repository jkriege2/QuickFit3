#include "spectrummanager.h"
#include "qftools.h"
#include "datatable2.h"
#include "qfmanyfilessettings.h"

SpectrumManager::SpectrumManager(){
    this->database_path=database_path;
    this->test_spectra=test_spectra;
    spectral_interpolation_type=gsl_interp_cspline;
    init_fluorophor_database();
    init_spectra();
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
        spectra[i].clear();
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

            d.name=set.value(gn+"name", gn).toString();
            d.description=set.value(gn+"description", "").toString();
            d.manufacturer=set.value(gn+"manufacturer", "").toString();
            d.reference=set.value(gn+"reference", "").toString();
            d.condition=set.value(gn+"condition", "").toString();
            d.fluorescence_efficiency=set.value(gn+"q_fluor", NAN).toDouble();
            d.fluorescence_lifetime=set.value(gn+"tau_fluor", NAN).toDouble();
            d.absorption_crosssection=set.value(gn+"sigma_abs", set.value(gn+"molar_extinction", NAN)/10.0/6.022e23).toDouble();
            d.absorption_crosssection_wavelength=set.value(gn+"sigma_abs_wavelength", NAN).toDouble();
            QString fn;
            int fnID=0;
            bool fnSepWL;
            int level=-1;
            fn=set.value(gn+"spectrum_abs", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_abs_id", 0).toInt();
            fnSepWL=set.value(gn+"spectrum_abs_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_abs=addSpectrum(fn, fnID, fnSepWL);
            }

            level=-1;
            fn=set.value(gn+"spectrum_fl", "", &level).toString();
            if (fn.size()>0 && level>=0) fn=QFileInfo(set.getSettings(level)->fileName()).absoluteDir().absoluteFilePath(fn);
            fnID=set.value(gn+"spectrum_fl_id", 1).toInt();
            fnSepWL=set.value(gn+"spectrum_fl_separatewavelengths", false).toBool();
            if (QFile::exists(fn)) {
                d.spectrum_fl=addSpectrum(fn, fnID, fnSepWL);
            }

        }
    }

}

int SpectrumManager::addSpectrum(const QString& filename, int ID, bool separateWavelengths) {
    for (int i=0; i<spectra.size(); i++) {
        if (QFileInfo(spectra[i].filename)==QFileInfo(filename) && spectra[i].ID==ID && spectra[i].separateWavelengthsInFile==separateWavelengths) {
            return i;
        }
    }

    Spectrum s;
    s.init(filename, ID, separateWavelengths);
    spectra.append(s);
    return spectra.size()-1;
}

SpectrumManager::Spectrum &SpectrumManager::getSpectrum(int i) const
{
    return spectra[i];
}

const SpectrumManager::Spectrum &SpectrumManager::getSpectrum(int i) const
{
    return spectra[i];
}

void SpectrumManager::init_fluorophor_database() {
    QString dirname=include_trailing_backslash(database_path)+"spectra"+QString(PATHSEPARATOR_STRING);
    jkINIParser2 ini(dirname+"fluorophors.ini");
    fluorophor_database.clear();
    for (int i=0; i<ini.getGroupCount(); i++) {
        QString g=tolower(ini.getGroupName(i));
        QString gg=ini.getGroupName(i);
        fluorophor_database[g].fluorescence_efficiency=ini.getAsDouble(gg+".q_fluor", 1);
        fluorophor_database[g].fluorescence_lifetime=ini.getAsDouble(gg+".tau_fluor", 1e-9);
        if (ini.exists(gg+".sigma_abs")) {
            fluorophor_database[g].sigma_abs=ini.getAsDouble(gg+".sigma_abs", 2e-20);
        } else {
            fluorophor_database[g].sigma_abs=ini.getAsDouble(gg+".molar_extinction", 2e4)/10.0/6.022e23;
        }
        fluorophor_database[g].bleaching_propability=ini.getAsDouble(gg+".p_bleach", 0);
        fluorophor_database[g].triplet_lifetime=ini.getAsDouble(gg+".tau_trip", 0);
        fluorophor_database[g].triplet_propability=ini.getAsDouble(gg+".p_trip", 0);
        fluorophor_database[g].spectrum=-1;
        std::cout<<"added fluorophor '"<<g<<"' to fluorophor database \n";
    }
}




void SpectrumManager::load_spectrum(int ID) {
    if (ID<0) return;
    //std::cout<<"waiting for load_spectrum ID="<<ID<<" ...\n";
    //boost::mutex::scoped_lock scoped_lock(spectrum_load_lock);
    //std::cout<<"in load_spectrum ...\n";
    if (spectra[ID].loaded) {
        //std::cout<<"finished load_spectrum ...\n";
        return;
    }
    //std::cout<<"run load_spectrum ...\n";
    datatable tab;
    QString filename=spectra[ID].filename_abs;
    //std::cout<<"loading spectrum: '"<<filename<<"'\n";
    tab.load_csv(filename);
    spectra[ID].val_count=tab.get_line_count();
    spectra[ID].wavelength=(double*)calloc(spectra[ID].val_count, sizeof(double));
    spectra[ID].eff_abs=(double*)calloc(spectra[ID].val_count, sizeof(double));
    spectra[ID].eff_fl=(double*)calloc(spectra[ID].val_count, sizeof(double));
    double max_abs=tab.column_max(1);
    double sum_fl=1;
    if (tab.get_column_count()>2) sum_fl=tab.column_sum(2);
    double max_fl=1;
    if (tab.get_column_count()>2) max_fl=tab.column_max(2);
    //std::cout<<"max_abs="<<max_abs<<"   sum_fl="<<sum_fl<<std::endl;
    for (size_t i=0; i<spectra[ID].val_count; i++) {
        spectra[ID].wavelength[i]=tab.get(0, i);
        spectra[ID].eff_abs[i]=tab.get(1, i)/max_abs;
        if (tab.get_column_count()>2) spectra[ID].eff_fl[i]=tab.get(2, i)/sum_fl;
    }
    spectra[ID].accel_abs= gsl_interp_accel_alloc ();
    spectra[ID].spline_abs=gsl_spline_alloc (spectral_interpolation_type, spectra[ID].val_count);
    gsl_spline_init(spectra[ID].spline_abs, spectra[ID].wavelength, spectra[ID].eff_abs, spectra[ID].val_count);
    spectra[ID].accel_fl= gsl_interp_accel_alloc ();
    spectra[ID].spline_fl=gsl_spline_alloc (spectral_interpolation_type, spectra[ID].val_count);
    gsl_spline_init(spectra[ID].spline_fl, spectra[ID].wavelength, spectra[ID].eff_fl, spectra[ID].val_count);


    if (tab.get_column_count()>2) {
        double dl=spectra[ID].wavelength[spectra[ID].val_count-1]-spectra[ID].wavelength[0];
        double int_fl=gsl_spline_eval_integ(spectra[ID].spline_fl, spectra[ID].wavelength[0], spectra[ID].wavelength[0]+dl, spectra[ID].accel_fl);
        for (size_t i=0; i<spectra[ID].val_count; i++) {
            spectra[ID].eff_fl[i]=spectra[ID].eff_fl[i]/int_fl;
        }
        /*gsl_spline_free(spectra[ID.spline_abs);
        gsl_interp_accel_free(spectra[ID].accel_abs);
        spectra[ID].accel_fl= gsl_interp_accel_alloc ();
        spectra[ID].spline_fl=gsl_spline_alloc (spectral_interpolation_type, spectra[ID].val_count);*/
        gsl_spline_init(spectra[ID].spline_fl, spectra[ID].wavelength, spectra[ID].eff_fl, spectra[ID].val_count);
    }


    spectra[ID].loaded=true;
    //std::cout<<"finished load_spectrum ...\n";
}



SpectrumManager::Spectrum::Spectrum()
{
    accel=NULL;
    spline=NULL;
    spectrumIDInFile=0;
    wavelength=NULL;
    N=0;
    separateWavelengthsInFile=false;
}

void SpectrumManager::Spectrum::clear()
{
    if (accel) gsl_flfree(accel);
    if (spline) gsl_interp_accel_free(spline);
    if (wavelength) free(wavelength);
    if (spectrum) free(spectrum);
    accel=NULL;
    spline=NULL;
    wavelength=NULL;
    N=0;
}

double SpectrumManager::Spectrum::getSpectrumAt(double lambda) const
{
    return gsl_spline_eval(spline,lambda, acc);
}

double SpectrumManager::Spectrum::getSpectrumIntegral(double lambda_start, double lambda_end) const
{
    return gsl_spline_eval_integ(spline, lambda_start, lambda_end, acc);
}


void SpectrumManager::Spectrum::init(const QVector<double> &lambda, const QVector<double> &eff)
{
    if (lambda.size()>0&&eff.size()>0) {
        N=qMin(lambda.size(), eff.size());
        this->wavelength=(double*)realloc(this->wavelength, N);
        this->spectrum=(double*)realloc(this->spectrum, N);
        if (accel) gsl_spline_free(accel);
        if (spline) gsl_interp_accel_free(spline);
        copyArray(this->wavelength, lambda.data(), N);
        copyArray(this->spectrum, eff.data(), N);
        accel= gsl_interp_accel_alloc ();
        spline=gsl_spline_alloc(spectral_interpolation_type,N);
        gsl_spline_init(spline, this->wavelength, this->spectrum, N);
    } else {
        if (accel) gsl_spline_free(accel);
        if (spline) gsl_interp_accel_free(spline);
        if (this->wavelength) free(this->wavelength);
        if (this->spectrum) free(this->spectrum);
        accel=NULL;
        spline=NULL;
        this->wavelength=NULL;
        this->spectrum=NULL;
        N=0;
    }
}

void SpectrumManager::Spectrum::init(const QString &filename, int ID, bool separateWavelengths)
{
    datatable2 table;
    table.load_csv(QFileInfo(filename).absoluteFilePath().toStdString());
    QVector<double> wl, sp;
    if (ID==0 && table.get_column_count()>=2) {
        for (int i=0; i<table.get_line_count(); i++) {
            if (!table.is_empty(0,i) && !table.is_empty(1,i)) {
                wl<<table.get(0,i);
                sp<<table.get(1,i);
            }
        }
    } else if (ID>0) {
        if (separateWavelengths) {
            for (int i=0; i<table.get_line_count(); i++) {
                if (!table.is_empty(ID*2+0,i) && !table.is_empty(ID*2+1,i)) {
                    wl<<table.get(ID*2+0,i);
                    sp<<table.get(ID*2+1,i);
                }
            }
        } else {
            for (int i=0; i<table.get_line_count(); i++) {
                if (!table.is_empty(0,i) && !table.is_empty(ID+1,i)) {
                    wl<<table.get(0,i);
                    sp<<table.get(ID+1,i);
                }
            }
        }

    }
    this->filename=QFileInfo(filename).absoluteFilePath();
    this->spectrumIDInFile=ID;
    this->separateWavelengthsInFile=separateWavelengths;
    init(wl, sp);
}

bool SpectrumManager::lightsourceExists(const QString &f)
{
    return lightsources.contains(f);
}

SpectrumManager::FluorophorData &SpectrumManager::getFluorophoreData(const QString &name)
{
    return fluorophores[name];
}

SpectrumManager::LightSourceData &SpectrumManager::getLightSourceData(const QString &name)
{
    return lightsources[name];
}

SpectrumManager::FilterData &SpectrumManager::getFilterData(const QString &name)
{
    return filters[name];
}

const SpectrumManager::FluorophorData &SpectrumManager::getFluorophoreData(const QString &name)
{
    return fluorophores[name];
}

const SpectrumManager::LightSourceData &SpectrumManager::getLightSourceData(const QString &name)
{
    return lightsources[name];
}

const SpectrumManager::FilterData &SpectrumManager::getFilterData(const QString &name)
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
    absorption_crosssection=0;
    absorption_crosssection_wavelength=0;
    spectrum_abs=-1;
    spectrum_fl=-1;
}

