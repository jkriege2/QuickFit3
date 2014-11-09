Several examples are given in the .cpp files in this directory. Each file
implements one fit function, which can be compiled into a shared library, 
which QuickFit 3 will find the the sdk_fitfunctions subdirectories.

These examples are provided:
  - example_evaluate_only.cpp - a very simple example, only implementing the evaluate function
  - example.cpp - a more sofisticated example, also implementing a calcualted parameter


COMPILE a fit function by hand:
  - MinGW (windows):  
       g++ -static -lstdc++ -lgcc -shared -DQF3SFF_BUILD_DLL -I.. -fPIC -o example.dll example.cpp -O3 -s
  -Linux:       
       g++ -shared -DQF3SFF_BUILD_DLL -fPIC -o example.so example.cpp
   
  
COMPILE using Makefile:
  The makefile has tagets Release [default], Debug and clean and compiles all 
  *.cpp files in the same directory into a library. The makefile works with 
  MinGW on windows and on Linux!


WHICH COMPILER TO USE?
  - on Linux you should use the GCC, which is installed on your system and which 
    you also used for compiling QF3
  - on Windows you can use MinGW, e.g. from http://sourceforge.net/projects/mingwbuilds/
    Make sure to use a 64-bit version, if your QF3 is 64-bit and a 32-bit version
    if QF3 is 32-bit. Generally a version of GCC/MinGW >=4.5 is advisable (We use
	gcc >4.7 for compiling QF3), but in principle any version should work. The 
	Makefile described above should work with MinGW on Windows as well as on
	Linux! In principle the plugins are plain vanilla C, so you should be able to 
	use any C/C++ compiler, that can generate DLLs.
  
ONLINE-HELP for the fit function:
  With each plugin, you can provide an HTML file, describing it. The file should 
  have the same name as the plugin, but with the ending .html. It is read by QF3's
  internal online help system, so you can use its features (e.g. $$math:LATEX$$
  to render equations given in LaTeX) and it is advisable to use  QuickFit's 
  online-help editor (Tools-menu of QuickFit 3) to edi these files. They have to
  reside in the same directory as the DLL. their name has to be made known to the
  plugin, by setting the global variable char QF3SFF_HELP[] in the plugin's main
  C/C++ file!
  
INSTALL plugins:
  After compiling your plugin, copy it (and possibly a help file with the same 
  name, but the ending .html, e.g. example.dll and example.html) into the
  sub-directory ./sdk_fitfunctions/ of the QuickFit3 installation.
  
  Then restart QuickFit and you plugin should be loaded. To check for success, 
  you can look into QuickFit's main log at the bottom of the main window and 
  search (CTRL+F!!!) for your DLL name. A line should appear and tell you, whether
  the library has been loaded successfully (then also the feature of your fit
  function are displayed), or whether an error occured.
  QuickFit also lists all directories, that are searched for such plugins with 
  line in theis log, reading:
      "searching in directory 'DIR_NAME' for fit functions in shared libraries"
  This can be used to detect wrongly placed libraries, as the containing directory
  is simply not searched!  
  

Here is a list of often used fit parameter IDs:
  * FCS:
    - n_nonfluorescent: number of nonfluorescent components (triplet ...)
    - n_components: number of diffusing components
    - nonfl_tau1: triplet decay time
    - nonfl_theta1: triplet fraction
    - nonfl_tau2 dark: component 2 decay time
    - nonfl_theta2: dark component 2 fraction
    - n_particle: particle number N
    - 1n_particle: 1/particle number N
    - diff_rho1: fraction of first component
    - diff_tau1: diffusion time of first component
    - diff_alpha1: anomality parameter of first component
    - diff_rho2: fraction of second component
    - diff_tau2: diffusion time of second component
    - diff_alpha2: anomality parameter of second component
    - diff_rho3: fraction of third component
    - diff_tau3: diffusion time of third component
    - diff_alpha3: anomality parameter of third component
    - offset: correlation offset
    - focus_struct_fac: focus: axial ratio
    - focus_width: focus: lateral radius
    - focus_volume: focus: effective volume
    - concentration: particle concentration in focus
    - count_rate: count rate during measurement
    - background: background count rate during measurement
    - cpm: photon counts per molecule 
    - diff_tauf, flow time of first component
    - vflow, flow speed of species 1
    - amplitude, amplitude of oscillation
    - frequency, frequency of oscillation
    - period, oscillation period
    - rot_tau, rotation decay time
    - rot_f, rotation fraction
    - rot_c1, rotation prefactor 1
    - rot_c2, rotation prefactor 2
    - diff_rot, rotation diffusion coefficient
    - afterpulsing_type, decay type of afterpulsing
    - afterpulsing_amplitude, afterpulsing amplitude
    - afterpulsing_exponent, afterpulsing exponent b
    - afterpulsing_tau, afterpulsing decay time tau
    - focus_distance, foci: lateral distance, d<sub>xy</sub>
    - pixel_width, pixel width
    - diff_coeff_sigma, width of D distribution

  * FCCS:
    - concentration_a, concentration of species a in focus, C<sub>a</sub>, nM
    - concentration_ab, concentration of species ab in focus, C<sub>ab</sub>, nM
    - diff_coeff_a, diffusion coefficient of species a, D<sub>a</sub>, micron^2/s
    - diff_coeff2_a, diffusion coefficient of second diffusion component of species a, D<sub>2,a</sub>, micron^2/s
    - diff_rho2_a, fraction of second diffusion component of species a, &rho;<sub>2,a</sub>
    - diff_coeff_ab, diffusion coefficient of species ab, D<sub>ab</sub>, micron^2/s
    - diff_coeff2_ab, diffusion coefficient of second diffusion component of species ab, D<sub>2,ab</sub>, micron^2/s
    - diff_rho2_ab, fraction of second diffusion component of species ab, &rho;<sub>2,ab</sub>
    - offset, correlation offset, G<sub>&infin;</sub>
    - focus_distance_x, foci: lateral distance in x-direction, d<sub>x</sub>, nm
    - focus_distance_y, foci: lateral distance in y-direction, d<sub>y</sub>, nm
    - focus_distance_z, foci: longitudinal distance in z-direction, d<sub>z</sub>, nm
    - focus_height1, green PSF: axial radius (1/e� radius), z<sub>g</sub>, nm
    - focus_width1, green PSF: lateral radius (1/e� radius), w<sub>g</sub>, nm
    - focus_volume1, green focus: effective volume, V<sub>eff,g</sub>, fl
    - count_rate1, count rate green, during measurement, &lang;F<sub>g</sub>&rang;, Hz
    - background1, background count rate green, during measurement, B<sub>g</sub>, Hz
    - brightness_a, molar brightness of fluorophore on A, CPM<sub>a</sub>, counts/nM
    - molbrightness_a, molecular brightness of fluorophore on A

	* DLS:
    - epsilon, baseline, &epsilon;
    - frac1, fraction of component 1, &alpha;<sub>1</sub>
    - frac2, fraction of component 2, &alpha;<sub>2</sub>
    - frac3, fraction of component 3, &alpha;<sub>3</sub>
    - frac4, fraction of component 4, &alpha;<sub>4</sub>
    - dls_angle, scattering angle, &Theta;<sub>scatter</sub>, �
    - refractive_index, refractive index, n
    - wavelength, scattering light wavelength, &lambda;, nm
    - q_square, q square, q<sup>2</sup>, 1/nm�
    - tau1, correlation time 1, &tau;<sub>1</sub>, 탎
    - tau2, correlation time 2, &tau;<sub>2</sub>, 탎
    - tau3, correlation time 3, &tau;<sub>3</sub>, 탎
    - tau4, correlation time 4
    
  * Light sheet PSF functions:
    - offset, offset
    - amplitude, amplitude    
    - position, position
    - width, width
  
  * General fit function:
    - offset, offset, Y<sub>0</sub>
    - amplitude, amplitude, A<sub>1</sub>
    - position, position, X<sub>1</sub>
    - width, 1/sqrt(e) width, &sigma;<sub>1</sub>
    - amplitude2, amplitude 2, A<sub>2</sub>
    - position2, position 2, X<sub>2</sub>
    - width2, 1/sqrt(e) width
    - frequency, frequency, f
    - phase, phase, &phi;, 2*pi, 2&pi;
    - period, period
    - position, position, X<sub>0</sub>
    - lifetime, lifetime, &tau;<sub>1</sub>
    - amplitude2, amplitude2, A<sub>2</sub>
    - lifetime2, lifetime
    - max, max, Y<sub>max</sub>
    - position, position, X<sub>1/2</sub>
    - rate, rate
    - mid_slope, slope at position
    - slope, slope
    - asymmetry, asymmetry, S
    - inflection, inflection	
	- degree, degree, degree
    - offset, offset, a<sub>0</sub>
    - factor1, prefactor 1, a<sub>1</sub>
    - factor2, prefactor 2, a<sub>2</sub>
    - factor3, prefactor 3, a<sub>3</sub>
    - factor4, prefactor 4, a<sub>4</sub>
    - factor5, prefactor 5, a<sub>5</sub>
    - factor6, prefactor 6, a<sub>6</sub>
    - factor7, prefactor 7, a<sub>7</sub>
    - factor8, prefactor 8, a<sub>8</sub>
    - factor9, prefactor 9, a<sub>9</sub>
    - factor10, prefactor 10
	- power, power of power-law
	
	
