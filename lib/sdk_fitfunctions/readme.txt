COMPILE a fit function by hand:

  - MinGW (windows):  
       g++ -static -lstdc++ -lgcc -shared -DBUILD_DLL -I.. -fPIC -o example.dll example.cpp -O3 -s
  -Linux: 	  
      g++ -shared -DBUILD_DLL -fPIC -o example.so example.cpp
  
  
  
COMPILE using Makefile:
  The makefile has tagets Release [default], Debug and clean and compiles all *.cpp files in the same directory into a library. The makefile works with MinGW on windows and on Linux!