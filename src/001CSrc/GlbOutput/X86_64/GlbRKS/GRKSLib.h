#ifndef _GRKSLIB_H
#define _GRKSLIB_H
#include "Windows.h"
//#include "GSLib.h"
#ifndef ERROR_INVALID
	#define ERROR_INVALID   23243  //没有软件许可或验证没通过
#endif
#ifndef ERROR_TRYDATE
	#define ERROR_TRYDATE   56547  //已过试用期
#endif
#ifndef ERROR_OK
	#define ERROR_OK        43645  //验证通过
#endif
#ifdef __cplusplus	//used in C++
extern "C"{
#endif
	
	long gverify(char * moduleName,char * moduleVer);
#ifdef __cplusplus	//used in C++
}
#endif

#endif