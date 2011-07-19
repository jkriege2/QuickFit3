#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2011";
	static const char UBUNTU_VERSION_STYLE[] = "11.07";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 3;
	static const long MINOR = 0;
	static const long BUILD = 0;
	static const long REVISION = 0;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 497;
	#define RC_FILEVERSION 3,0,0,0
	#define RC_FILEVERSION_STRING "3, 0, 0, 0\0"
	static const char FULLVERSION_STRING[] = "3.0.0.0";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
