#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2011";
	static const char UBUNTU_VERSION_STYLE[] = "11.01";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 3;
	static const long MINOR = 0;
	static const long BUILD = 91;
	static const long REVISION = 496;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 127;
	#define RC_FILEVERSION 3,0,91,496
	#define RC_FILEVERSION_STRING "3, 0, 91, 496\0"
	static const char FULLVERSION_STRING[] = "3.0.91.496";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
