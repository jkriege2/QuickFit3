# QuickFit3
A data evaluation package for (imaging) fluorescnece correlation spectroscopy and other biophysical methods
Project Homepage: http://www.dkfz.de/Macromol/quickfit/

![Logo](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/images/icon_64.png)

##Download
Currently QuickFit can be found here (will be moved to GitHub soon):
- Windows Binaries: http://www.dkfz.de/Macromol/quickfit/#download
- Source Code: https://github.com/jkriege2/QuickFit3

##Description
![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/images/splash.png)

QuickFit 3 is a data evaluation software for FCS and imagingFCS (imFCS) measurements, developed in the group B040 (Prof. Jörg Langowski) at the German Cancer Research Center (DKFZ). Actually QuickFit 3 itself is a project manager and all functionality is added as plugins. A set of tested plugins for FCS, imagingFCS and some microscopy-related image processing tasks is supplied together with the software. New plugins that provide additional features will be added to this distribution of QuickFit when they become available. Currently these features are supported: 
- FCS:
  - dataimport from ALV5000/6000/7000, ISS Alba files, Zeiss Confocor3 files, correlator.com .sin-files, CSV files
  - correlation of TCSPC files from PicoQuant TimeHarp, PicoHarp, ...
  - model fit with different predefined models (normal/anomalous 1D/2D/3D diffusion, photophysics, TIRF-FCS, SPIM-FCS, DLS ...) and different fit algorithms
  - fit error estimates, using bootstrapping
  - global fitting over several files (e.g. for FCCS analysis)
  - export of fit results as report (PDF/PS), image (PDF/PS/SVG/PNG/JPEG/LaTeX...), data (CSV, SYLK, Excel, Matlab ... also direct copy/paste to Excel/Origin)
  - Maximum Entropy data evaluation for FCS/DLS
  - Extract mean squared displacements (MSD) from FCS measurements (e.g. for polymer physics)
  - calculator for diffusion coefficients of several dyes (included comprehensive database) and particles of different shapes (spheres, ellipsoids, cylinders, ...) in a given solution

- imaging FCS (imFCS):

  - importing dataset from CSV files
  - correlating any TIFF image series
  - model fit (SPIM/TIR-FCS...)
  - global imaging FCCS fitting (SPIM/TIR-FCCS...)
  - plotting of parameter images, histograms and correlation plots (incl. several robust/normal statistics)
  - export of fit results and parameter images as report (PDF/PS), image (PDF/PS/SVG/PNG/JPEG/LaTeX...), data (CSV, SYLK, Excel, Matlab ... also direct copy/paste to Excel/Origin)
  - multi-threaded fits (~120 fits/second)
  - SPIM-FCS calibration wizard
  - number and brightness analysis
  - PSF determination
  - camera calibration
  - simple colocalization analysis
  - imFCS diffusion laws

- user-defined fit functions
- image stack 3D volume renderer
- spreadsheet functionality and advanced plotting in "table" plugin
- publication quality plotting
- export of data (CSV, SYLK, Excel, Matlab ... also direct copy/paste to Excel/Origin) and images (PDF/PS/SVG/PNG/JPEG/LaTeX...) of each plot in the program
- organization of data and evaluations as a project (.qfp file in XML format, or compressed as .qfpz) to easily save an evaluation
- online-help system with many references and explanation of the implemented methods (all formulas should be given, so you know what the program does!) and tutorials
- convenient and customizable user interface
- spectra viewer with comprehensive set of spectra (including some lamps, filters, detectors and many standard dyes)
- Linux and later possibly also MacOS X version (runs on MacOS X using Wine) are available on request
- we now also release out SPIM hardware control plugins, including a driver for Andor EMCCD cameras


## Licensing & Citation

This software is partly LGPL 2.1 and partly GPL 3.0, as well as postcard ware: If you find QuickFit usefull and like it, please send us a postcard from your part of the world to
```
    Deutsches Krebsforschungs Zentrum (DKFZ)
    Abteilung B040
    Im Neuenheimer Feld 580
    D-69120 Heidelberg
    GERMANY
```
If you use this software for a publication, you will have to cite QuickFit this in the publication. In the online-help we give examples of how to cite QuickFit. Also have a look at the online-help of the plugins you used to see whether you should cite additional libraries used in these plugins. Here is an example BiBTeX record that can be used to cite QuickFit (update to the version you downloaded or copy an. updated version from the Copyright page of the onine help!):

```BiBTeX
    @misc{quickfit3,
        author={Jan Wolfgang Krieger and Jörg Langowski},
        title={QuickFit 3.0 (status: beta, compiled: ???, SVN: ???): A data evaluation application for biophysics},
        howpublished={[web page] \verb+http://www.dkfz.de/Macromol/quickfit/+},
        year={2015},
        note={[Accessed on ???]},
    } 
```
A citation may look like this:
```
    JW. Krieger, J. Langowski (2015): QuickFit 3.0 (status: beta, compiled: ???, SVN: ???): A data evaluation application for biophysics, [web page] http://www.dkfz.de/Macromol/quickfit/ [Accessed on ???] 
```
This software is provided as is without any warranties. Use it at your own risk. But: We do our best to make a well tested and good pakage (as it is massively used in our department).


## Screenshots
### Welcome Screen
![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_welcome.png)

### Main Window
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_main.png)

### Settings Dialog
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_settings.png)

### Online Help
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_help.png)

### User-Defined Fit Functions
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_userfitfunctions.png)

### imaging FCS
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_imfcs.png)
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_imfcs_paramcorrelation.png)

### imaging FCS Fit
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_imfcsfit.png)

### imaging FCCS Fit
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_imfccsfit.png)

### FCS Fit
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_fcsfit.png)

### FCS: MSD Evaluation
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_msd.png)

### FCS: MaxEnt evaluation
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_maxent.png)

### global FCCS Fit
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_fccsfit.png)

### table plugin (spreadsheet/plots)
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_table.png)
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_tableplot.png)

### SPIM Lightsheet Analysis
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_lightsheet.png)

### Spectra Viewer
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_spectra.png)

### Diffusion Coefficient Calculator
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_dcalc.png)

### 3D Image Stack Viewer
 ![SplashScreen](https://raw.githubusercontent.com/jkriege2/QuickFit3/master/screenshots/screen_3dviewer.png)
