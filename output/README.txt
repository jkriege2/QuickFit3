--------------------------------------------------------------------------------
-- QuickFit 3.0                                                               --
-- (c) 2010-2013 by Jan W. Krieger                                            --
--     Biophysics of Macromolecules (B040)                                    --
--     German Cancer Research center (DKFZ)                                   --
--------------------------------------------------------------------------------
--   http://www.dkfz.de/Macromol/quickfit/                                    --
--------------------------------------------------------------------------------



QuickFit 3 is our current in-house data evaluation software for FCS and 
imagingFCS (imFCS) measurement. Actually QuickFit 3 itself is a project manager 
and all functionality is added as plugins. A set of tested plugins is supplied 
together with the software. New plugins that provide additional features will be
added to this distribution of QuickFit when they become available. Currently 
these features are supported:

  * FCS:
      * dataimport from ALV5000, ISS Alba files, CSV files
      * correlation of TCSPC files from PicoQuant TimeHarp, PicoHarp
      * model fit with different predefined models (normal/anomalous 1D/2D/3D 
        diffusion, photophysics, SPIM-FCS, DLS ...) and fit algorithms
      * export of fit results as report (PDF/PS), image (PDF/PS/PNG/JPEG...), 
	    data (CSV, SYLK, ...)
      * Maximum Entropy data evaluation
      * Extract mean squared displacement from FCS measurements (e.g. for 
	    polymer physics)
      * calculator for diffusion coefficients of several dyes in a given solution
  * imaging FCS (imFCS):
      * importing dataset from CSV files
      * correlating TIFF image series
      * model fit (SPIM-FCS, ...)
      * plotting of parameter images and histograms
      * export of fit results and parameter images as report (PDF/PS), image 
	    (PDF/PS/PNG/JPEG...), data (CSV, SYLK, Matlab, ...)
      * multi-threaded fits (~50 fits/second)
      * number and brightness analysis
  * spreadsheet functionality and advanced plotting in "table" plugin
  * organization of data and evaluations as a project (.qfp file in XML format) 
    to easily save an evaluation
  * online-help system with many references and explanation of the implemented 
    methods (all formulas should be given, so you know what the program does!)
  * convenient and customizable user interface
  * Linux and later possibly also MacOS X version are available on request
  * we now also release out SPIM hardware control plugins

	
	
	
Mailing List
~~~~~~~~~~~~

If you want to stay up to date on the development of QuickFit and receive 
announcements on new versions, we have a mailing list (quickfit@dkfz.de) on the 
topic.

If you want to subscribe, please send an e-mail to quickfit-request@dkfz.de with
the subject subscribe (If the link does not work, simply write the e-mail as 
described yourself, or contact Jan Krieger directly). After a short delay you 
should receive a welcome message from the mailing list.



