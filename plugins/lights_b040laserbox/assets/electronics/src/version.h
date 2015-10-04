#ifndef VERSION_H
#define VERSION_H

	//Date Version Types
	static const char DATE[] = "15";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2015";
	static const char UBUNTU_VERSION_STYLE[] =  "15.04";
	
	//Software Status
	static const char STATUS[] =  "";
	static const char STATUS_SHORT[] =  "";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 0;
	static const long BUILD  = 8;
	static const long REVISION  = 48;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 10;
	#define RC_FILEVERSION 0,0,8,48
	#define RC_FILEVERSION_STRING "0, 0, 8, 48\0"
	static const char FULLVERSION_STRING [] = "0.0.8.48";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 8;
	

#endif //VERSION_H
