 /*!
 Thread debugging made easy: gdb, strg-c, info threads, thread <x>, top...
 
 */
 
 #include <QtGui>
 #include <QSettings>
 
 #include "processing_chain.h"
 
 int main(int argc, char *argv[])
 {
	 QApplication app(argc, argv);
	 
	 QWidget window;
	 window.resize(320, 240);
	 window.setWindowTitle(QApplication::translate("childwidget", "Child widget"));
	 window.show();
	 
	 QPushButton *button = new QPushButton(QApplication::translate("childwidget", "run the chain"), &window);
	 button->move(100, 100);
	 button->show();
	 
	 QSettings settings(argv[1], QSettings::IniFormat );/** read ini file */
	 processing_chain pc(&settings,0);
	 QObject::connect(button, SIGNAL(clicked()), &pc, SLOT(run()));
	 
	 return app.exec();
 }
 