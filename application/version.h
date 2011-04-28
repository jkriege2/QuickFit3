#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "28";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2011";
	static const char UBUNTU_VERSION_STYLE[] = "11.04";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 3;
	static const long MINOR = 0;
	static const long BUILD = 157;
	static const long REVISION = 911;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 438;
	#define RC_FILEVERSION 3,0,157,911
	#define RC_FILEVERSION_STRING "3, 0, 157, 911\0"
	static const char FULLVERSION_STRING[] = "3.0.157.911";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
