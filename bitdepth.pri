#message($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}qf3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX})
system($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}qf3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX})

BIT_DEPTH=$$system(.$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX} --bitdepth)

message("COMPILE MODE: bit-depth: $$BIT_DEPTH")
