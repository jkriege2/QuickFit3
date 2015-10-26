#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "20";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2012";
	static const char UBUNTU_VERSION_STYLE[] = "12.04";
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 1;
	static const long BUILD = 149;
	static const long REVISION = 832;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 158;
	#define RC_FILEVERSION 1,1,149,832
	#define RC_FILEVERSION_STRING "1, 1, 149, 832\0"
	static const char FULLVERSION_STRING[] = "1.1.149.832";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 49;
	

#endif //VERSION_H
