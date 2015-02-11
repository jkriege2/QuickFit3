QT += core gui svg xml network opengl
message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
    win32{
        !contains(LIBS, -lopengl32): LIBS += -lopengl32
        !contains(LIBS, -lglu32): LIBS += -lglu32
    }
	greaterThan(QT_MINOR_VERSION, 3) {
	    DEFINE += QFVisualize3DGLWidget_USES_QOPENGLWIDGET
	}
}
CONFIG += exceptions rtti stl link_prl
