/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef SPECTRUMMANAGER_H
#define SPECTRUMMANAGER_H

#include "qfmanyfilessettings.h"
#include <cmath>
#include <iostream>
#include <string>
#include <map>
#include <exception>
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <cstdlib>
#include <stdexcept>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <QVector>
#include <QString>
#include <QMap>
#include "cpptools.h"
#include "jkiniparser2.h"
#include <QStringList>
#include <QSettings>
#include "qfpseudotreemodel.h"

/** \brief this class is used to manage fluorophor properties and spectra
 *  \ingroup diff4_tools
 *
 * There is also a system in this class that allows to lookup a spectral prefactor [0..1] in a known set of spectra.
 * The known spectra are loaded in the constructor and more may be added by calling add_spectrum(). The lookup
 * is implemented in get_spectral_efficiency(). It is sufficient to give some points of the spectrum, as
 * get_spectral_efficiency() interpolates between them. Spectra should be given in \c *.spec files that contain
 * two columns of data: The first is the wavelength in [nm] and the second is the absorption efficiency [0..1]
 * where 1 is maximum absorptiona and 0 is no absorption. The highest value in the spectrum should be 1.
 */

class SpectrumManager {
    public:
        /** \brief this struct contains all data neede to describe a spectrum */
        struct Spectrum {
            private:
                /** \brief specifies the type of interpolation for the spectra */
                const gsl_interp_type *spectral_interpolation_type;
                /** \brief the file the absorption spectrum was loaded from */
                QString filename;
                /** \brief number of spectrum inside file */
                int spectrumIDInFile;
                /** \brief if \c true the CSV file contains one wavelength column, otherwise the layoutout is wavelength, spectrum, wavelength, spectrum, ... */
                bool separateWavelengthsInFile;
                /** \brief the wavelengths from the file [nanometers] */
                double* wavelength;
                /** \brief the absorption efficiencies from the file [0..1] */
                double* spectrum;
                /** \brief the fluorescence from the file [0..1] */
                int N;
                /** \brief GSL lookup accelerator for absorbance */
                gsl_interp_accel *accel;
                /** \brief this stores the spline calculated from the data in the file for absorbance */
                gsl_spline *spline;
                bool loaded;
                bool normalizeSpectrum;
                void internalInit(const QVector<double>& wavelength, const QVector<double>& spectrum, bool normalizeSpectrum=true);

            public:
                Spectrum();
                /** \brief reference/source for absorption spectrum */
                QString reference;

                void ensureSpectrum();

                void init(const QVector<double>& wavelength, const QVector<double>& spectrum, bool normalizeSPectrum=true);
                void init(const QString& filename, int ID=0, bool separateWavelengths=false, bool normalizeSPectrum=true);
                void clear();
                double getSpectrumAt(double wavelength);
                double getSpectrumIntegral(double lambda_start, double lambda_end) ;
                double getSpectrumIntegral(double lambda_start, double lambda_end, double wavelength_factctor) ;
                double getMulSpectrumIntegral(Spectrum* multWith, double lambda_start, double lambda_end) ;
                double getMulSpectrumIntegral(Spectrum* multWith, double lambda_start, double lambda_end, double lambda_power, double wavelength_factctor=1, double spectrum_factor=1) ;
                double getMulSpectrumIntegral(Spectrum* multWith) ;
                double getMulSpectrumIntegral(Spectrum* multWith, double lambda_power) ;
                double getWavelengthMin() ;
                double getWavelengthMax() ;
                double getSpectrumMin() ;
                double getSpectrumMax() ;
                double getSpectrumMaxWavelength() ;
                double getSpectrumFullIntegral() ;
                inline double* getWavelength()  {
                    //ensureSpectrum();
                    return wavelength;
                }
                inline double* getSpectrum()  {
                    //ensureSpectrum();
                    return spectrum;
                }
                inline int getN() {
                    //ensureSpectrum();
                    return N;
                }
                QString getFilename() const { return filename; }
                int getID() const { return spectrumIDInFile; }
                bool getSeparateWavelengthsInFile() const { return separateWavelengthsInFile; }
        };

        /** \brief specifies the properties of one fluorophor */
        struct FluorophoreData {
            FluorophoreData();
            QString name;
            QString description;
            QString manufacturer;
            QString folder;
            /** \brief the fluorescence efficiency of the fluorophor [0..1] */
            double fluorescence_efficiency;
            /** \brief fluorescence lifetime [sec] */
            double fluorescence_lifetime;
            double fluorescence_efficiency_wavelength;
            /** \brief molar exctinction coefficient [1/(M cm)] */
            double extiction_coefficient;
            /** \brief wavelength at which the absorption cross section is given [nanometers] */
            double extiction_coefficient_wavelength;
            /** \brief absorption specturm */
            int spectrum_abs;
            /** \brief fluorescence spectrum */
            int spectrum_fl;
            /** \brief excitation maximum wavelength [nm] */
            double excitation_maxwavelength;
            /** \brief emission maximum wavelength [nm] */
            double emission_maxwavelength;
            /** \brief reference/source of data */
            QString reference;
            /** \brief conditions of measurements for the above given properties */
            QString condition;
            QString orderNo;

            QMap<QString, QVariant> moreData;
        };

        /** \brief specifies the properties of a lightsource */
        struct LightSourceData {
            LightSourceData();
            QString name;
            QString description;
            QString manufacturer;
            QString orderNo;
            QString folder;
            /** \brief absorption specturm */
            int spectrum;
            /** \brief reference/source of data */
            QString reference;
            /** \brief typical emission wavelength [nm] */
            double typical_wavelength;

            QMap<QString, QVariant> moreData;
        };

        /** \brief specifies the properties of a filter */
        struct FilterData {
            FilterData();
            QString name;
            QString description;
            QString manufacturer;
            QString orderNo;
            QString folder;
            /** \brief absorption specturm */
            int spectrum;
            /** \brief reference/source of data */
            QString reference;
            /** \brief typical emission wavelength [nm] */
            double typical_wavelength;

            QMap<QString, QVariant> moreData;

        };

        /** \brief specifies the properties of a detector */
        struct DetectorData {
            DetectorData();
            QString name;
            QString description;
            QString manufacturer;
            QString orderNo;
            QString folder;
            /** \brief absorption specturm */
            int spectrum;
            /** \brief reference/source of data */
            QString reference;
            /** \brief peak detection wavelength [nm] */
            double peak_wavelength;
            /** \brief peak detection sensitivity (at peak_wavelength) [unit/µW] */
            double peak_sensitivity;
            /** \brief peak detection sensitivity unit */
            QString peak_sensitivity_unit;

            QMap<QString, QVariant> moreData;

        };

    protected:


        /** \brief load all \c *.spec file from the current directory */
        void init_spectra();

        QList<Spectrum*> spectra;

        /** \brief fluorophor database */
        QMap<QString, FluorophoreData> fluorophores;
        QFPseudoTreeModel* fluorophoresTree;
        /** \brief lightsource database */
        QMap<QString, LightSourceData> lightsources;
        QFPseudoTreeModel* lightsourcesTree;
        /** \brief filter database */
        QMap<QString, FilterData> filters;
        QFPseudoTreeModel* filtersTree;
        /** \brief detector database */
        QMap<QString, DetectorData> detectors;
        QFPseudoTreeModel* detectorsTree;

        QFPseudoTreeModel* laserlinesTree;

    private:

    public:
        /** \brief class constructor, initialises the database from the provided path. */
        SpectrumManager();

        /** \brief class destructor */
        ~SpectrumManager();

        /** \brief get number of spectra */
        int getSpectraCount() const;
        /** \brief get number of fluorophores */
        int getFluorophoreCount() const;
        /** \brief get number of light sources */
        int getLightSourceCount() const;
        /** \brief get number of filters */
        int getFilterCount() const;
        /** \brief get number of filters */
        int getDetectorCount() const;
        /** \brief get fluorophore IDs */
        QStringList getFluorophores() const;
        /** \brief get lightsource IDs */
        QStringList getLightSources() const;
        /** \brief get filter IDs*/
        QStringList getFilters() const;
        /** \brief get detector IDs */
        QStringList getDetectors() const;

        /** \brief get fluorophore IDs */
        QFPseudoTreeModel* getFluorophoresTree() const;
        /** \brief get lightsource IDs */
        QFPseudoTreeModel* getLightSourcesTree() const;
        /** \brief get filter IDs*/
        QFPseudoTreeModel* getFiltersTree() const;
        /** \brief get detector IDs */
        QFPseudoTreeModel* getDetectorsTree() const;
        /** \brief get detector IDs */
        QFPseudoTreeModel* getLaserlinesTree() const;

        void clear();

        void loadLaserlinesDatabase(const QString& ininame, const QStringList& directories=QStringList() );
        void loadFluorophoreDatabase(const QString& ininame, const QStringList& directories=QStringList() );
        void loadFilterDatabase(const QString& ininame, const QStringList& directories=QStringList() );
        void loadLightSourceDatabase(const QString& ininame, const QStringList& directories=QStringList() );
        void loadDetectorDatabase(const QString& ininame, const QStringList& directories=QStringList() );


        /** \brief add a spectrum file to the internal database */
        int addSpectrum(const QString &filename, int ID=0, bool separateWavelengths=false, const QString &reference=QString(""), bool normalizeSPectrum=true);
        /** \brief return the i-th spectrum */
        Spectrum *getSpectrum(int i);
        /** \brief return the i-th spectrum */


        /** \brief returns \c true if fluorophor exists in database */
        bool fluorophoreExists(const QString& f);
        /** \brief returns \c true if filter exists in database */
        bool filterExists(const QString& f);
        /** \brief returns \c true if lightsource exists in database */
        bool lightsourceExists(const QString& f);
        /** \brief returns \c true if detector exists in database */
        bool detectorExists(const QString& f);
        bool spectrumExists(int f);

         FluorophoreData getFluorophoreData(const QString& name)  ;
         LightSourceData getLightSourceData(const QString& name)  ;
         FilterData getFilterData(const QString& name)  ;
         DetectorData getDetectorData(const QString& name)  ;

         static void loadMoreData(QMap<QString, QVariant>& store, QFManyFilesSettings& settings, const QString& prefix, const QStringList& readprops);

};
#endif // SPECTRUMMANAGER_H
