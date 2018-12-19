#pragma once
#include <map>
using namespace std;
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"
#include "GlbCriticalSection.h"
/**
  * @brief 错误信息类
  *
  * 维护最近一次执行方法的错误信息
  * 
  * 					 
  * @version 1.0
  * @author  LIUJH
  * @date    2014-3-7 14:40 
  */
class GLBCOMM_API CGlbError
{
public:
	CGlbError(void);
	~CGlbError(void){}
	/**
      * @brief 设置错误信息
      *      
      *
      * @param errinfo 错误信息
      * @return 无
    */
	void SetLastError(const glbWChar* errinfo);
	/**
      * @brief 获取错误信息
      *      
      *       
      * @return 错误信息
	  *         如果没有，返回NULL
	  *
	  *         调用方，需要负责销毁返回的指针。
    */
	glbWChar* GetLastError();
private:
	map<glbInt32/*线程Id*/,CGlbWString/*错误信息*/> mpr_lasterrs;
	GlbCriticalSection                              mpr_critical;
};

extern GLBCOMM_API CGlbError g_glbError;
#define GlbSetLastError g_glbError.SetLastError
#define GlbGetLastError g_glbError.GetLastError