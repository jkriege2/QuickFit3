COMPILE a fit function:

  - MinGW (windows):  
       g++ -static -lstdc++ -lgcc -shared -DLIB_EXPORT_LIBRARY -I.. -fPIC -o example.dll example_minimal.cpp -O3 -s
  -Linux: 	  
      g++ -shared -DLIB_EXPORT_LIBRARY -fPIC -o example.so example.cpp
  