#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "22";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.10";
	
	//Software Status
	static const char STATUS[] =  "";
	static const char STATUS_SHORT[] =  "";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 0;
	static const long BUILD  = 10;
	static const long REVISION  = 58;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 11;
	#define RC_FILEVERSION 0,0,10,58
	#define RC_FILEVERSION_STRING "0, 0, 10, 58\0"
	static const char FULLVERSION_STRING [] = "0.0.10.58";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 10;
	

#endif //VERSION_H
