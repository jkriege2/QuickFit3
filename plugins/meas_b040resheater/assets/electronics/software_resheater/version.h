#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "05";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2012";
	static const char UBUNTU_VERSION_STYLE[] = "12.06";
	
	//Software Status
	static const char STATUS[] = "Release";
	static const char STATUS_SHORT[] = "r";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 0;
	static const long BUILD = 22;
	static const long REVISION = 119;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 22;
	#define RC_FILEVERSION 1,0,22,119
	#define RC_FILEVERSION_STRING "1, 0, 22, 119\0"
	static const char FULLVERSION_STRING[] = "1.0.22.119";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 22;
	

#endif //VERSION_H
