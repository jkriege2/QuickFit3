--------------------------------------------------------------------------------
-- QuickFit 3.0                                                               
-- (c) 2008-2015 by Jan W. Krieger                                            
--     Biophysics of Macromolecules (B040)                                    
--        German Cancer Research center (DKFZ)                                
--     Institute for scientific computation (IWR)                             
--        University of Heidelberg                                            
--------------------------------------------------------------------------------
--   http://www.dkfz.de/Macromol/quickfit/                                    
--   https://github.com/jkriege2/QuickFit3.git                                
--------------------------------------------------------------------------------



QuickFit 3 is our current in-house data evaluation software for FCS and 
imagingFCS (imFCS) measurement. Actually QuickFit 3 itself is a project manager 
and all functionality is added as plugins. A set of tested plugins is supplied 
together with the software. New plugins that provide additional features will be
added to this distribution of QuickFit when they become available. Currently 
these features are supported:

* FCS/FCCS:
    * dataimport from ALV5000/6000/7000, ISS Alba files, Zeiss Confocor3 files, 
	  correlator.com .sin-files, CSV files
    * correlation of TCSPC files from PicoQuant TimeHarp, PicoHarp, ...
    * model fit with different predefined models (normal/anomalous 1D/2D/3D 
	  diffusion, photophysics, TIRF-FCS, SPIM-FCS, DLS ...) and 
	  different fit algorithms
    * fit error estimates, using bootstrapping
    * global fitting over several files (e.g. for FCCS analysis)
    * export of fit results as report (PDF/PS), image (PDF/PS/SVG/PNG/JPEG/LaTeX...),
	  data (CSV, SYLK, Excel, Matlab ... also direct copy/paste to Excel/Origin)
    * Maximum Entropy data evaluation for FCS/DLS
    * Extract mean squared displacements (MSD) from FCS measurements 
	 (e.g. for polymer physics)
    * calculator for diffusion coefficients of several dyes (included 
	  comprehensive database) and particles of different shapes (spheres, 
	  ellipsoids, cylinders, ...) in a given solution

* imaging FCS (imFCS):
    * importing dataset from CSV files
    * correlating any TIFF image series
    * model fit (SPIM/TIR-FCS...)
    * global imaging FCCS fitting (SPIM/TIR-FCCS...)
    * plotting of parameter images, histograms and correlation plots (incl. 
	* several robust/normal statistics)
    * export of fit results and parameter images as report (PDF/PS), 
	  image (PDF/PS/SVG/PNG/JPEG/LaTeX...), data (CSV, SYLK, Excel, Matlab ... 
	  also direct copy/paste to Excel/Origin)
    * multi-threaded fits (~100 fits/second)
    * imaging FCS calibration wizard
    * number and brightness analysis
    * PSF determination
    * camera calibration
    * simple colocalization analysis
    * imFCS diffusion laws

* Microscope Characterization Plugins:
    * PSF from z-stacks of immobilized beads
	* SPIM lightsheet characterization
	* camera calibration
	* simple colocalization analysis

* simple and consistent user-interface
* wizards fro many standard tasks
* single-particle FRET data evaluation (burst selection)
* user-defined fit functions
* image stack 3D volume renderer
* spreadsheet functionality and advanced plotting in "table" plugin
* extensive expression parser as part of QuickFit, can also be used inside
  the table-function for dynamically updated spreadsheets and plots!
* publication quality plotting
* export of data (CSV, SYLK, Excel, Matlab ... also direct copy/paste to 
  Excel/Origin) and images (PDF/PS/SVG/PNG/JPEG/LaTeX...) of each plot in the 
  program
* organization of data and evaluations as a project (.qfp file in XML format, or
  compressed as .qfpz) to easily save an evaluation
* online-help system with many references and explanation of the implemented 
  methods (all formulas should be given, so you know what the program does!) and
  tutorials
* convenient and customizable user interface
* spectra viewer with comprehensive set of spectra (including some lamps, 
  filters, detectors and many standard dyes)
* source code is available from 
       https://github.com/jkriege2/QuickFit3
* we also release out SPIM hardware control plugins
	
	
	
Mailing List
~~~~~~~~~~~~

If you want to stay up to date on the development of QuickFit and receive 
announcements on new versions, we have a mailing list (quickfit@dkfz.de) on the 
topic.

If you want to subscribe, please send an e-mail to quickfit-request@dkfz.de with
the subject subscribe (If the link does not work, simply write the e-mail as 
described yourself, or contact Jan Krieger directly). After a short delay you 
should receive a welcome message from the mailing list.



